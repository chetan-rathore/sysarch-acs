/** @file
 * Copyright (c) 2020,2021,2023-2025, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#include "val/include/acs_val.h"
#include "val/include/acs_pcie_enumeration.h"
#include "val/include/acs_pcie.h"
#include "val/include/acs_pe.h"
#include "val/include/acs_smmu.h"
#include "val/include/acs_pgt.h"
#include "val/include/acs_iovirt.h"
#include "val/include/acs_memory.h"
#include "val/include/acs_exerciser.h"
#include "val/include/val_interface.h"

#define TEST_NUM   (ACS_EXERCISER_TEST_NUM_BASE + 2)
#define TEST_RULE  "PCI_PP_04"
#define TEST_DESC  "Check ACS Redirect Req Valid          "

static
uint32_t
get_target_exer_bdf(uint32_t req_rp_bdf, uint32_t *tgt_e_bdf,
                    uint32_t *tgt_rp_bdf, uint64_t *bar_base)
{

  uint32_t erp_bdf;
  uint32_t e_bdf;
  uint32_t instance;
  uint32_t cap_base;

  instance = val_exerciser_get_info(EXERCISER_NUM_CARDS);

  while (instance-- != 0)
  {
      /* if init fail moves to next exerciser */
      if (val_exerciser_init(instance))
          continue;

      e_bdf = val_exerciser_get_bdf(instance);

      /* Read e_bdf BAR Register to get the Address to perform P2P */
      /* If No BAR Space, continue */
      val_pcie_get_mmio_bar(e_bdf, bar_base);
      if (*bar_base == 0)
          continue;

      /* Get RP of the exerciser */
      if (val_pcie_get_rootport(e_bdf, &erp_bdf))
          continue;

      /* It ACS Not Supported, continue */
      if (val_pcie_find_capability(erp_bdf, PCIE_ECAP, ECID_ACS, &cap_base) != PCIE_SUCCESS) {
          val_print(ACS_PRINT_DEBUG, "\n       ACS Not Supported for BDF : 0x%x", erp_bdf);
          continue;
      }

      if (req_rp_bdf != erp_bdf)
      {
          *tgt_e_bdf = e_bdf;
          *tgt_rp_bdf = erp_bdf;

          /* Enable Bus Master Enable */
          val_pcie_enable_bme(e_bdf);
          /* Enable Memory Space Access */
          val_pcie_enable_msa(e_bdf);

          return ACS_STATUS_PASS;
      }
  }

  /* Return failure if No Such Exerciser Found */
  *tgt_e_bdf = 0;
  *tgt_rp_bdf = 0;
  *bar_base = 0;
  return ACS_STATUS_FAIL;
}

static
uint32_t
create_va_pa_mapping (uint64_t txn_va, uint64_t txn_pa,
                      smmu_master_attributes_t *smmu_master,
                      pgt_descriptor_t *pgt_desc, uint32_t req_instance,
                      uint32_t req_rp_bdf, uint32_t pgt_ap)
{
  smmu_master_attributes_t master;
  memory_region_descriptor_t mem_desc_array[2], *mem_desc;
  uint32_t e_bdf = val_exerciser_get_bdf(req_instance);
  uint64_t ttbr;
  uint32_t num_smmus;
  uint32_t instance;
  uint32_t device_id, its_id;
  uint32_t status, dma_status;
  uint64_t bar_value;
  uint32_t old_val, new_val;

  master = *smmu_master;

  val_memory_set(&master, sizeof(master), 0);
  val_memory_set(mem_desc_array, sizeof(mem_desc_array), 0);
  mem_desc = &mem_desc_array[0];

  /* Get translation attributes via TCR and translation table base via TTBR */
  if (val_pe_reg_read_tcr(0 /*for TTBR0*/, &pgt_desc->tcr))
    return ACS_STATUS_FAIL;
  if (val_pe_reg_read_ttbr(0 /*TTBR0*/, &ttbr))
    return ACS_STATUS_FAIL;

  pgt_desc->pgt_base = (ttbr & AARCH64_TTBR_ADDR_MASK);
  pgt_desc->mair = val_pe_reg_read(MAIR_ELx);
  pgt_desc->stage = PGT_STAGE1;

  /* Get memory attributes of the test buffer, we'll use the same attibutes to create
   * our own page table later.
   */
  if (val_pgt_get_attributes(*pgt_desc, (uint64_t)txn_va, &mem_desc->attributes))
    goto test_fail;

  num_smmus = val_iovirt_get_smmu_info(SMMU_NUM_CTRL, 0);

  /* Enable all SMMUs */
  for (instance = 0; instance < num_smmus; ++instance)
     val_smmu_enable(instance);

  /* Get SMMU node index for this exerciser instance */
  master.smmu_index = val_iovirt_get_rc_smmu_index(PCIE_EXTRACT_BDF_SEG(e_bdf),
                                                    PCIE_CREATE_BDF_PACKED(e_bdf));

  if (master.smmu_index != ACS_INVALID_INDEX &&
      val_iovirt_get_smmu_info(SMMU_CTRL_ARCH_MAJOR_REV, master.smmu_index) == 3) {
      if (val_iovirt_get_device_info(PCIE_CREATE_BDF_PACKED(e_bdf),
                                     PCIE_EXTRACT_BDF_SEG(e_bdf),
                                     &device_id, &master.streamid,
                                     &its_id))
          goto test_fail;

      /* Each exerciser instance accesses a unique IOVA, which, because of SMMU translations,
       * will point to the same physical address. We create the requisite page tables and
       * configure the SMMU for each exerciser as such.
       */

      mem_desc->virtual_address = (uint64_t)txn_va;
      mem_desc->physical_address = txn_pa;
      mem_desc->length = 4; /* 4 Bytes */
      mem_desc->attributes |= pgt_ap;

      /* Need to know input and output address sizes before creating page table */
      pgt_desc->ias = val_smmu_get_info(SMMU_IN_ADDR_SIZE, master.smmu_index);
      if (!pgt_desc->ias)
        goto test_fail;

      pgt_desc->oas = val_smmu_get_info(SMMU_OUT_ADDR_SIZE, master.smmu_index);
      if (!pgt_desc->oas)
        goto test_fail;

      /* set pgt_desc.pgt_base to NULL to create new translation table, val_pgt_create
       will update pgt_desc.pgt_base to point to created translation table */
      pgt_desc->pgt_base = (uint64_t) NULL;
      if (val_pgt_create(mem_desc, pgt_desc))
        goto test_fail;

      /* Configure SMMU tables for this exerciser to use this page table for VA to PA translations*/
      if (val_smmu_map(master, *pgt_desc))
      {
          val_print(ACS_PRINT_DEBUG, "\n       SMMU mapping failed (%x)     ", e_bdf);
          goto test_fail;
      }

      /* Trigger DMA from req_e_bdf */
      val_exerciser_set_param(DMA_ATTRIBUTES, (uint64_t)txn_va, 1, req_instance);

      /* Clear Error Status Bits */
      val_pcie_clear_device_status_error(req_rp_bdf);
      val_pcie_clear_sig_target_abort(req_rp_bdf);

      /* Store the value of BAR reg. To be rewritten back later. */
      bar_value = val_mmio_read64((uint64_t)txn_va);

      /* Corrupt the BAR and read the value before making a DMA transaction. */
      val_mmio_write(txn_va, 0xABCDABCD);
      old_val = val_mmio_read((uint64_t)txn_va);
      val_print(ACS_PRINT_DEBUG, "\n       Bar value before DMA is %llx", old_val);

      /* Trigger DMA from Exerciser to the target device. */
      val_exerciser_ops(START_DMA, EDMA_FROM_DEVICE, req_instance);

      /* Read the memory location to check if DMA is successful or not. */
      new_val = val_mmio_read((uint64_t)txn_va);
      val_print(ACS_PRINT_DEBUG, "\n       Bar value after DMA is %llx", new_val);

      /* If the values of targeted reads are same, the DMA is failure. */
      if (old_val == new_val)
            dma_status = 0;  // DMA Fail
      else
            dma_status = 1;  // DMA Success

      /* Write back the original BAR value to the address. */
      val_mmio_write64(txn_va, bar_value);

      /* DMA must fail because Write permission not given */
      if ((pgt_ap == PGT_STAGE1_AP_RO) && (dma_status != 0)) {
          val_print(ACS_PRINT_DEBUG,
                    "\n       Seq1:DMA Write must not happen For : %4x", req_instance);
          goto test_fail;
      }

      /* DMA must not fail because Write permission given */
      if ((pgt_ap == PGT_STAGE1_AP_RW) && (dma_status == 0)) {
          val_print(ACS_PRINT_DEBUG, "\n       Seq2:DMA Write must happen For : %4x", req_instance);
          goto test_fail;
      }
      status = ACS_STATUS_PASS;
      goto test_clean;
  }

test_fail:
      status = ACS_STATUS_FAIL;

test_clean:
      val_pgt_destroy(*pgt_desc);
      val_smmu_unmap(master);

  return status;
}


static
uint32_t
check_redirected_req_validation (uint32_t req_instance, uint32_t req_rp_bdf, uint64_t bar_base)
{
  uint64_t txn_va;
  uint32_t instance;
  uint32_t num_smmus;
  uint32_t status;
  smmu_master_attributes_t master;
  pgt_descriptor_t pgt_desc;

  /* Sequence 1 : No Write Permission, Trigger a DMA Write to bar address
   * It must Result into ACS Violation
   */

  /* Create VA-PA Mapping in SMMU with PGT permissions as Read Only */
  /* Initialize DMA master and memory descriptors */

  /* Set the virtual and physical addresses for test buffers */
  txn_va = (uint64_t)val_memory_phys_to_virt(bar_base);

  num_smmus = val_iovirt_get_smmu_info(SMMU_NUM_CTRL, 0);

  status = create_va_pa_mapping(txn_va, bar_base, &master,
                                &pgt_desc, req_instance,
                                req_rp_bdf, PGT_STAGE1_AP_RO);
  if (status) {
      val_print(ACS_PRINT_DEBUG,
                      "\n       Seq1:SMMU Mapping Failed For : %4x", req_instance);
      goto test_fail;
  }

  /* Check For Error in Device Status Register / Status Register
   * Secondary Status Register
   */
  if ((val_pcie_is_device_status_error(req_rp_bdf) == 0) &&
     (val_pcie_is_sig_target_abort(req_rp_bdf) == 0)) {
      val_print(ACS_PRINT_DEBUG, "\n       Seq1:Expected Error For RootPort : 0x%x", req_rp_bdf);
      goto test_fail;
  }

  /* Disable all SMMUs */
  for (instance = 0; instance < num_smmus; ++instance)
     val_smmu_disable(instance);

  /* Sequence 2 : Read Write Permission, Trigger a DMA Write to bar address
   * It must NOT Result into ACS Violation
   */

  /* Create VA-PA Mapping in SMMU with PGT permissions as Read Write */
  status = create_va_pa_mapping(txn_va, bar_base, &master,
                                &pgt_desc, req_instance,
                                req_rp_bdf, PGT_STAGE1_AP_RW);
  if (status) {
      val_print(ACS_PRINT_DEBUG, "\n       Seq2:SMMU Mapping Failed For : %4x", req_instance);
      goto test_fail;
  }

  /* Check For Error in Device Status Register / Status Register
   * Secondary Status Register
   */
  if (val_pcie_is_device_status_error(req_rp_bdf) ||
     val_pcie_is_sig_target_abort(req_rp_bdf)) {
      val_print(ACS_PRINT_DEBUG, "\n       Seq2:Expected No Error For RootPort : 0x%x", req_rp_bdf);
      goto test_fail;
  }

  status = ACS_STATUS_PASS;
  goto test_clean;

test_fail:
  status = ACS_STATUS_FAIL;

test_clean:

  /* Clear Error Status Bits */
  val_pcie_clear_device_status_error(req_rp_bdf);
  val_pcie_clear_sig_target_abort(req_rp_bdf);

  /* Disable all SMMUs */
  for (instance = 0; instance < num_smmus; ++instance)
     val_smmu_disable(instance);

  return status;
}

static
void
payload(void)
{

  uint32_t status;
  uint32_t pe_index;
  uint32_t bdf;
  uint32_t req_e_bdf;
  uint32_t req_rp_bdf;
  uint32_t tgt_e_bdf;
  uint32_t tgt_rp_bdf;
  uint32_t instance;
  uint32_t fail_cnt;
  uint32_t cap_base;
  uint32_t reg_value;
  uint32_t test_skip;
  uint32_t tbl_index;
  uint64_t bar_base;
  uint32_t req_e_seg_num;
  uint32_t req_e_bus_num;
  uint32_t req_e_dev_num;
  uint32_t req_e_func_num;
  uint32_t tgt_e_seg_num;
  uint32_t tgt_e_bus_num;
  uint32_t tgt_e_dev_num;
  uint32_t tgt_e_func_num;

  fail_cnt = 0;
  test_skip = 1;
  pe_index = val_pe_get_index_mpid(val_pe_get_mpid());
  instance = val_exerciser_get_info(EXERCISER_NUM_CARDS);
  pcie_device_bdf_table *bdf_tbl_ptr;

  tbl_index = 0;
  bdf_tbl_ptr = val_pcie_bdf_table_ptr();
  uint32_t acsctrl_default[bdf_tbl_ptr->num_entries][1];

  /* Check If PCIe Hierarchy supports P2P. */
  if (val_pcie_p2p_support() == NOT_IMPLEMENTED) {
    val_print(ACS_PRINT_DEBUG, "\n       The test is applicable only if the system supports", 0);
    val_print(ACS_PRINT_DEBUG, "\n       P2P traffic. If the system supports P2P, pass the", 0);
    val_print(ACS_PRINT_DEBUG, "\n       command line option '-p2p' while running the binary", 0);
    val_set_status(pe_index, RESULT_SKIP(TEST_NUM, 1));
    return;
  }

  /* Store ACS Control reg bits in an array for every BDF and reset them to default at the end. */
  val_pcie_read_acsctrl(acsctrl_default);

  while (tbl_index < bdf_tbl_ptr->num_entries)
  {
      bdf = bdf_tbl_ptr->device[tbl_index++].bdf;
      if (val_pcie_find_capability(bdf, PCIE_ECAP, ECID_ACS, &cap_base) == PCIE_SUCCESS) {
          /* Enable P2P Request Redirect & Upstream Forwarding */
          val_pcie_read_cfg(bdf, cap_base + ACSCR_OFFSET, &reg_value);

          reg_value = reg_value | (1 << ACS_CTRL_RRE_SHIFT) | (1 << ACS_CTRL_UFE_SHIFT);
          val_pcie_write_cfg(bdf, cap_base + ACSCR_OFFSET, reg_value);
      }
  }

  while (instance-- != 0)
  {

      /* if init fail moves to next exerciser */
      if (val_exerciser_init(instance))
          continue;

      req_e_bdf = val_exerciser_get_bdf(instance);
      val_print(ACS_PRINT_DEBUG, "\n       Requester exerciser BDF - 0x%x", req_e_bdf);

      /* Get RP of the exerciser */
      if (val_pcie_get_rootport(req_e_bdf, &req_rp_bdf))
          continue;

      /* It ACS Not Supported, Fail.*/
      if (val_pcie_find_capability(req_rp_bdf, PCIE_ECAP, ECID_ACS, &cap_base) != PCIE_SUCCESS) {
          val_print(ACS_PRINT_ERR, "\n       ACS Not Supported for BDF : 0x%x", req_rp_bdf);
          fail_cnt++;
          continue;
      }

      /* Find another exerciser on other rootport,
         Skip the current exerciser if no such exerciser if found */
      if (get_target_exer_bdf(req_rp_bdf, &tgt_e_bdf, &tgt_rp_bdf, &bar_base))
          continue;

      /* If Both RP's Supports ACS Then Only Run Otherwise Skip the EP */
      test_skip = 0;

      /* Check For Redirected Request Validation Functionality */
      status = check_redirected_req_validation(instance, req_rp_bdf, bar_base);
      if (status == ACS_STATUS_SKIP)
          val_print(ACS_PRINT_ERR, "\n       ACS Validation Check Skipped for 0x%x", req_rp_bdf);
      else if (status) {
          fail_cnt++;
          val_print(ACS_PRINT_ERR, "\n       ACS Redirected Req Check Failed for 0x%x", req_rp_bdf);
      }

      /* Check for Redirected Request Validation Functionality for the same device
       * wih different function
       */
      tbl_index = 0;
      req_e_seg_num = PCIE_EXTRACT_BDF_SEG(req_e_bdf);
      req_e_bus_num = PCIE_EXTRACT_BDF_BUS(req_e_bdf);
      req_e_dev_num = PCIE_EXTRACT_BDF_DEV(req_e_bdf);
      req_e_func_num = PCIE_EXTRACT_BDF_FUNC(req_e_bdf);

      while (tbl_index < bdf_tbl_ptr->num_entries)
      {
          tgt_e_bdf = bdf_tbl_ptr->device[tbl_index++].bdf;
          tgt_e_seg_num = PCIE_EXTRACT_BDF_SEG(tgt_e_bdf);
          tgt_e_bus_num = PCIE_EXTRACT_BDF_BUS(tgt_e_bdf);
          tgt_e_dev_num = PCIE_EXTRACT_BDF_DEV(tgt_e_bdf);
          tgt_e_func_num = PCIE_EXTRACT_BDF_FUNC(tgt_e_bdf);

          /* Check if the requestor and target exerciser belong to same device but
           * different function.
           */
          if ((req_e_seg_num == tgt_e_seg_num) && (req_e_bus_num == tgt_e_bus_num)
             && (req_e_dev_num == tgt_e_dev_num) && (req_e_func_num != tgt_e_func_num))
          {
              /* Read e_bdf BAR Register to get the Address to perform P2P
               * If No BAR Space, continue.
               */
              val_pcie_get_mmio_bar(tgt_e_bdf, &bar_base);
              if (bar_base == 0)
                  continue;

              /* Enable Bus Master Enable */
              val_pcie_enable_bme(tgt_e_bdf);
              /* Enable Memory Space Access */
              val_pcie_enable_msa(tgt_e_bdf);

              /* Check For Redirected Request Validation Functionality */
              status = check_redirected_req_validation(instance, req_rp_bdf, bar_base);
              if (status == ACS_STATUS_SKIP)
                  val_print(ACS_PRINT_ERR, "\n       ACS Validation Check Skipped for 0x%x",
                                                                                req_rp_bdf);
              else if (status) {
                  fail_cnt++;
                  val_print(ACS_PRINT_ERR, "\n       ACS Redirected Req Check Failed for 0x%x",
                                                                                   req_rp_bdf);
              }
         }
      }


  }

  /* Write back default values of ACS Control reg. */
  val_pcie_write_acsctrl(acsctrl_default);

  if (test_skip == 1)
      val_set_status(pe_index, RESULT_SKIP(TEST_NUM, 2));
  else if (fail_cnt)
      val_set_status(pe_index, RESULT_FAIL(TEST_NUM, fail_cnt));
  else
      val_set_status(pe_index, RESULT_PASS(TEST_NUM, 1));

  return;

}

uint32_t
e002_entry(void)
{
  uint32_t num_pe = 1;
  uint32_t status = ACS_STATUS_FAIL;

  status = val_initialize_test(TEST_NUM, TEST_DESC, num_pe);
  if (status != ACS_STATUS_SKIP)
      val_run_test_payload(TEST_NUM, num_pe, payload, 0);

  /* Get the result from all PE and check for failure */
  status = val_check_for_error(TEST_NUM, num_pe, TEST_RULE);

  val_report_status(0, ACS_END(TEST_NUM), NULL);

  return status;
}
