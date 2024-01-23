#ifndef _HX_DEBUG_UTIL_H_
#define _HX_DEBUG_UTIL_H_

#include <stdint.h>
#include "printf_color.h"

#define BOOTROM_SECDBG1 0x5200F014
#define BOOTROM_SECDBG2 0x5200F018

typedef enum SEC_STATE_S {
    HX_SEC_INIT = 0,
    HX_SEC_IMG_BOOT_CHK_INIT_FALG = 0x0001,
    HX_DRV_SB_IMG_VERIFY = 0x002,
    CC_SBCERT_VERIFY_SINGLE = 0x004,
    CC_LOAD_CERT = 0x0008,
    HX_CCSbCalcPublicKeyHASHAndCompare = 0x0010,
    HX_RSA_PSS_Decode = 0x0020,
    HX_CCSbImageLoadAndVerify = 0x0040,
    HX_DRV_SB_IMG_PROC = 0x0080,
    HX_SEC_DBG_TEST = 0x8000,
} SEC_STATE_E;

typedef enum SEC_STATUS_S {
    SUCCESS = 0,
    FAIL = 0x0001,
    KEY_CERT_FAIL = 0x0002,
    CONT_CERT_FAIL = 0x0004,
    VERIFY_SINGLE_PARAM_FAIL = 0x0008,
    LOAD_CERT_FAIL = 0x0010,
    LOAD_CERT_WK_SPACE_SMALL = 0x0020,
    LOAD_CERT_READ_FLASH_1_FAIL = 0x0040,
    LOAD_CERT_CERT_SIZE_BIG = 0x0080,
    LOAD_CERT_READ_FLASH_2_FAIL = 0x0100,
    COMMON_CERT_VERIFY_FAIL = 0x0200,
    CCSbCalcPublicKeyHASHAndCompare_FAIL = 0x0400,
    RSA_PSS_Decode_FAIL = 0x0800,
    SW_comp_failed_verification_FAIL = 0x1000,
    OTP_READ_FAIL = 0x2000,
    CODE_ENC_TPYE_FAIL = 0x4000,
    IN_OUT_P_FAIL = 0x8000,
} SEC_STATUS_E;

void print_array(char *name, uint8_t *data, int size);
void set_secure_dbg_status(SEC_STATE_E state, SEC_STATUS_E status);
void set_signture_eanble_dbg_status(uint32_t otp_value);

#endif //_HX_DEBUG_UTIL_H_

