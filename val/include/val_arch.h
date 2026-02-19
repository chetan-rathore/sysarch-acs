/*
 * Copyright (c) 2026, Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef VAL_ARCH_H
#define VAL_ARCH_H

/*
 * For those constants to be shared between C and other sources, apply a 'U',
 * 'UL', 'ULL', 'L' or 'LL' suffix to the argument only in C, to avoid
 * undefined or unintended behaviour.
 *
 * The GNU assembler and linker do not support these suffixes (it causes the
 * build process to fail) therefore the suffix is omitted when used in linker
 * scripts and assembler files.
*/
#if defined(__LINKER__) || defined(__ASSEMBLY__)
# define   U(_x)    (_x)
# define  UL(_x)    (_x)
# define ULL(_x)    (_x)
# define   L(_x)    (_x)
# define  LL(_x)    (_x)
#else
# define   U(_x)    (_x##U)
# define  UL(_x)    (_x##UL)
# define ULL(_x)    (_x##ULL)
# define   L(_x)    (_x##L)
# define  LL(_x)    (_x##LL)
#endif


/* CPSR/SPSR definitions */
#define DAIF_FIQ_BIT        (U(1) << 0)
#define DAIF_IRQ_BIT        (U(1) << 1)
#define DAIF_ABT_BIT        (U(1) << 2)
#define DAIF_DBG_BIT        (U(1) << 3)
#define DAIF_CONFIG         (U(0x7))

#define PMSELR_EL0_SEL_MASK U(0x1f)

#define EL_IMPL_NONE        ULL(0)
#define MODE_EL_SHIFT       U(0x2)
#define MODE_EL_MASK        U(0x3)
#define GET_EL(mode)        (((mode) >> MODE_EL_SHIFT) & MODE_EL_MASK)

#define ID_AA64PFR0_EL1_SHIFT U(4)
#define ID_AA64PFR0_ELX_MASK  ULL(0xf)

#endif /* VAL_ARCH_H */
