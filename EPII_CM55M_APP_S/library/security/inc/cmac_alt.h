/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
 */

#ifndef MBEDTLS_CMAC_ALT_H
#define MBEDTLS_CMAC_ALT_H

#if defined(MBEDTLS_CONFIG_FILE)
#include "config-cc312-mps2-no-os.h"
#endif



#include <stddef.h>
#include <stdint.h>


/* hide internal implementation of the struct. Allocate enough space for it.*/
#define MBEDTLS_CMAC_CONTEXT_SIZE_IN_WORDS			33


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          CMAC cipher context structure
 */
struct mbedtls_cmac_context_t{
    /*! Internal buffer */
    uint32_t buf[MBEDTLS_CMAC_CONTEXT_SIZE_IN_WORDS];
};

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_CMAC_ALT_H */
