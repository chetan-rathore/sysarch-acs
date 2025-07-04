/** @file
 * Copyright (c) 2025, Arm Limited or its affiliates. All rights reserved.
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
#include "val/include/acs_memory.h"

#define TEST_NUM   (ACS_DRTM_DL_TEST_NUM_BASE  + 10)
#define TEST_RULE  "R44030"
#define TEST_DESC  "Check DL on PE other than BOOT PE     "

volatile int64_t dl_status = ACS_STATUS_FAIL;

/* Payload to run on secondary PE */
void secondary_pe_payload(void)
{
  uint32_t index = val_pe_get_index_mpid(val_pe_get_mpid());
  DRTM_PARAMETERS *drtm_params_buffer;
  uint64_t buffer_ptr, addr;

  val_get_test_data(index, &addr, &buffer_ptr);
  drtm_params_buffer = (DRTM_PARAMETERS *)buffer_ptr;

  dl_status = val_drtm_dynamic_launch(drtm_params_buffer);
  val_data_cache_ops_by_va((addr_t)&dl_status, CLEAN_AND_INVALIDATE);
  val_set_status(index, RESULT_PASS(TEST_NUM, 1));
}

static
void
payload(uint32_t num_pe)
{

  /* This test will verify the DRTM Dynamic Launch
   * Input parameter will be 64 bit address of DRTM Parameters
   * */
  uint32_t index = val_pe_get_index_mpid(val_pe_get_mpid());
  int64_t  status;
  uint32_t timeout;
  uint32_t sec_pe_index;

  DRTM_PARAMETERS *drtm_params;
  uint64_t drtm_params_size = DRTM_SIZE_4K;
  uint32_t num_of_pe;

  num_of_pe = val_pe_get_num();
  if (num_of_pe < 2) {
    /* Skip the test as there is no secondary PE */
    val_print(ACS_PRINT_ERR, "\n       No secondary PE Present. Skipping", 0);
    val_set_status(index, RESULT_SKIP(TEST_NUM, 1));
    return;
  }

  for (uint8_t i = 0; i < num_of_pe; i++) {
    if (i == index)
      continue;

    sec_pe_index = i;
    break;
  }

  /* Allocate Memory For DRTM Parameters 4KB Aligned */
  drtm_params = (DRTM_PARAMETERS *)((uint64_t)val_aligned_alloc(DRTM_SIZE_4K, drtm_params_size));
  if (!drtm_params) {
    val_print(ACS_PRINT_ERR, "\n    Failed to allocate memory for DRTM Params", 0);
    val_set_status(index, RESULT_FAIL(TEST_NUM, 1));
    return;
  }

  status = val_drtm_init_drtm_params(drtm_params);
  if (status != ACS_STATUS_PASS) {
    val_print(ACS_PRINT_ERR, "\n       DRTM Init Params failed err=%d", status);
    val_set_status(index, RESULT_FAIL(TEST_NUM, 2));
    goto free_drtm_params;
  }

  /* Invoke DRTM Dynamic Launch, This will return only in case of error */

  timeout = TIMEOUT_LARGE;

  val_set_status(sec_pe_index, RESULT_PENDING(TEST_NUM));
  val_execute_on_pe(sec_pe_index, secondary_pe_payload, (uint64_t)drtm_params);

  while ((--timeout) && (IS_RESULT_PENDING(val_get_status(sec_pe_index))));

  val_data_cache_ops_by_va((addr_t)&dl_status, CLEAN_AND_INVALIDATE);

  if (timeout == 0) {
    val_print(ACS_PRINT_ERR, "\n       **Timed out** for PE index = %d", sec_pe_index);
    val_print(ACS_PRINT_ERR, " Found = %d", dl_status);
    val_set_status(index, RESULT_FAIL(TEST_NUM, 3));
    goto free_dlme_region;
  }

  /* This will return invalid parameter */
  if (dl_status != DRTM_ACS_DENIED) {
    val_print(ACS_PRINT_ERR, "\n       DRTM Dynamic Launch failed, Expected = %d",
                            DRTM_ACS_DENIED);
    val_print(ACS_PRINT_ERR, " Found = %d", dl_status);
    val_set_status(index, RESULT_FAIL(TEST_NUM, 4));
    if (status == DRTM_ACS_SUCCESS) {
      status = val_drtm_unprotect_memory();
      if (status < DRTM_ACS_SUCCESS) {
        val_print(ACS_PRINT_ERR, "\n       DRTM Unprotect Memory failed err=%d", status);
        val_set_status(index, RESULT_FAIL(TEST_NUM, 5));
      }
    }
    goto free_dlme_region;
  }

  val_set_status(index, RESULT_PASS(TEST_NUM, 1));

free_dlme_region:
  val_memory_free_aligned((void *)drtm_params->dlme_region_address);
free_drtm_params:
  val_memory_free_aligned((void *)drtm_params);

  return;
}

uint32_t dl010_entry(uint32_t num_pe)
{

  uint32_t status = ACS_STATUS_FAIL;

  status = val_initialize_test(TEST_NUM, TEST_DESC, num_pe);

  if (status != ACS_STATUS_SKIP)
    /* execute payload, which will execute relevant functions on current and other PEs */
    payload(num_pe);

  /* get the result from all PE and check for failure */
  status = val_check_for_error(TEST_NUM, num_pe, TEST_RULE);

  val_report_status(0, ACS_END(TEST_NUM), NULL);

  return status;
}
