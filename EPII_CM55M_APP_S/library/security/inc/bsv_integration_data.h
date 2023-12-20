/****************************************************************************
* The confidential and proprietary information contained in this file may   *
* only be used by a person authorised under and to the extent permitted     *
* by a subsisting licensing agreement from Arm Limited or its affiliates.   *
*   (C) COPYRIGHT [2001-2020] Arm Limited or its affiliates.                *
*       ALL RIGHTS RESERVED                                                 *
* This entire notice must be reproduced on all copies of this file          *
* and copies of this file may only be made by a person if such person is    *
* permitted to do so under the terms of a subsisting license agreement      *
* from Arm Limited or its affiliates.                                       *
*****************************************************************************/


#ifndef _BSV_INTEGRATION_DATA_H
#define _BSV_INTEGRATION_DATA_H

#include "stdint.h"
//#include "cc_bitops.h"
//#include "cc_otp_defs.h"

#include "bsv_integration_data_def.h"
//#include "bsv_integration_test.h"
#include "hx_otp_def.h"



/************************************************************
 *
 * defines
 *
 ************************************************************/
#define ASSET_BLOB_DATA_CM_SIZE             64

#define BVSIT_NUM_OF_LCS                    4
#define BSVIT_MAX_CHAIN_LENGTH              3
#define BSVIT_TEST_NAME                     BSVIT_TEST_NAME_VALUE
#define BSVIT_HASH_BLOCK_SIZE               64
/************************************************************
 *
 * type decelerations
 *
 ************************************************************/
typedef enum BsvItMoveToRmaEntity_t
{
    BSVIT_MOVE_TO_RMA__OEM,
    BSVIT_MOVE_TO_RMA__ICV,

    BSVIT_MOVE_TO_RMA__MAX
}BsvItMoveToRmaEntity_t;


/**
 * An enum to indicate under which life cycle to perform a test.
 */
/*typedef enum BsvItRunMode_t
{
    BSVIT_RUNMODE__CM,
    BSVIT_RUNMODE__DM,
    BSVIT_RUNMODE__SECURE,
    BSVIT_RUNMODE__RMA,
    BSVIT_RUNMODE__ALL,

    BSVIT_RUNMODE__MAX = BSVIT_RUNMODE__ALL
}BsvItRunMode_t;*/

/**
 * An enum to indicate SHA256 non-integrated data sizes.
 */
typedef enum BsvItSHA256_sizes_t
{
    BSVIT_SHA256_DATA_IN_2BLOCKS__NIST_MSG_SIZE=56,
    BSVIT_SHA256_DATA_IN_128B_SIZE=128,
}BsvItSHA256_sizes_t;

/**
 * This structure define a use case
 * for SHA256 non-integrated.
 */
typedef struct BsvItSHA256case_t
{
    /** length of data */
    BsvItSHA256_sizes_t dataSize;
    /** the binary data */
    const uint8_t   *data;
    /** sha256 digest */
    const uint32_t  *digest;
} BsvItSHA256case_t;


/**
 * This structure maps a certain life cycle to the
 * the initial OTP image.
 */
typedef struct BsvItOtpTables_t{
    char          otpName[16];
    uint32_t      lcsState;
    uint32_t     *otpValues;
} BsvItOtpTables_t;

typedef struct BsvItBinaryFile_t
{
    /** mainly for debug */
    const char  *name;
    /** length of data */
    uint32_t    length;
    /** the binary data */
    uint8_t     *data;
} BsvItBinaryFile_t;

/**
 * This structure maps a certain life cycle to the
 * asset provisioning package (a static blob of data that will serve as an input)
 * and the asset plain text to be verified after
 * the asset has been successfully opened and verified.
 */
typedef struct BsvItAssetTables_t{
    BsvItBinaryFile_t     assetBinary;
    uint8_t             *assetPlainText;
} BsvItAssetTables_t;

/**
 * This structures maps a certain life cycle to the
 * secure debug certificate (a static blob of data that will serve as input)
 * and the expected DCU and DCU_LOCK values to be verified after the process
 * is completed successfully
 */
typedef struct BsvItSecureDebugCertTables_t{
    BsvItBinaryFile_t     certPkgBin;
    uint32_t             expectedDcus[CC_OTP_DCU_SIZE_IN_WORDS];
    uint32_t             expectedDcuLocks[CC_OTP_DCU_SIZE_IN_WORDS];
} BsvItSecureDebugCertTables_t;

typedef struct BsvItSecureBoot_SwCompEnc_t
{
    /** offset of image in flash */
    uint32_t flashAddr;
    /** address where to load the image in host's address space */
    uint32_t virtualHostOffset;
    /** binary file itself */
    BsvItBinaryFile_t   *swCompBin;
    /** plain text binary file used to compare result */
    BsvItBinaryFile_t   *swCompPlainTextBin;
} BsvItSecureBoot_SwCompEnc_t;

typedef struct BsvItSecureBootCertInfo_t
{
    BsvItBinaryFile_t   *cert;
    uint32_t storeFlashAddress;
} BsvItSecureBootCertInfo_t;

typedef struct BsvItSecureBoot_CertChain_t
{
    /** An Arrays of Maximum number of certificates */
    BsvItSecureBootCertInfo_t certArr[BSVIT_MAX_CHAIN_LENGTH];
    /** a list of images in this certificate */
    BsvItSecureBoot_SwCompEnc_t *cntCompsInfo;
    /** amount of images is the list */
    uint8_t numOfSwCompImg;
    uint32_t expSwVersion;
}BsvItSecureBoot_CertChain_t;

/************************************************************
 *
 * externs
 *
 ************************************************************/
#define CC_32BIT_WORD_SIZE  4
extern const uint8_t                         bsvIt_KpicvBytes[BVSIT_NUM_OF_LCS][CC_OTP_KPICV_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE];
extern const uint8_t                         bsvIt_KceicvBytes[BVSIT_NUM_OF_LCS][CC_OTP_KCEICV_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE];
extern const uint8_t                         bsvIt_KceBytes[BVSIT_NUM_OF_LCS][CC_OTP_KCEICV_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE];
extern const BsvItOtpTables_t                bsvIt_otpTables[];
extern const BsvItAssetTables_t              bsvIt_assetTables[];
extern const uint8_t                         bsvIt_hbkUsed[BVSIT_NUM_OF_LCS];
extern const uint32_t                        bsvIt_hbkBytes[BVSIT_NUM_OF_LCS][CC_OTP_HBK_SIZE_IN_WORDS];
extern const BsvItSecureDebugCertTables_t    bsvIt_secureDebug_certTables[];
extern const BsvItBinaryFile_t               bsvIt_secureDebug_moveToRma_certTables[];
extern const uint32_t                        bsvItsecureDebug_permanentDisable[BVSIT_NUM_OF_LCS][CC_OTP_DCU_SIZE_IN_WORDS];
extern const BsvItSecureBoot_CertChain_t     bsvIt_secureBoot_CertChain;
extern const uint8_t                         bsvIt_isSecureBootImagesEncrypted;
extern const uint8_t                         bsvIt_isSecureBootSkip[BVSIT_NUM_OF_LCS];
extern const uint8_t                         bsvIt_non_integrated_sha256_data_1[BSVIT_SHA256_DATA_IN_2BLOCKS__NIST_MSG_SIZE];
extern const uint32_t                        bsvIt_non_integrated_sha256_data_1_digest[BSVIT_SHA256_DIGEST_SIZE_IN_WORDS];
                                             /* Note that this data_2 will undertake 513 times of hash update to simulate more than 64KB */   
extern const uint8_t                         bsvIt_non_integrated_sha256_data_2[BSVIT_SHA256_DATA_IN_128B_SIZE];
                                             /* Note that this data_2_digest is for 128B*513 ( grater than hash limit 64KB) */   
extern const uint32_t                        bsvIt_non_integrated_sha256_data_2_digest[BSVIT_SHA256_DIGEST_SIZE_IN_WORDS]; 
extern const BsvItSHA256case_t               BSVIT_SHA256_DATA[];
#endif
