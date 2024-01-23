#ifndef _HX_HAL_SEC_H_
#define _HX_HAL_SEC_H_

#include <stdint.h>
#include "hx_drv_def.h"
#include "hx_cc312_drv_sec.h"

//#include "test_proj_defs.h"

#define CC_MNG_LCS_CM           0x0 /*! Chip manufacturer (CM LCS). */
#define CC_MNG_LCS_DM           0x1 /*! Device manufacturer (DM LCS). */
#define CC_MNG_LCS_SEC_ENABLED  0x5 /*! Security enabled (Secure LCS). */
#define CC_MNG_LCS_RMA          0x7 /*! RMA (RMA LCS). */

typedef struct
{
	uint8_t *input;
	uint32_t length;
	uint8_t *hmac;
}HMAC_PARA_t;

typedef struct
{
	uint32_t mode;	
	uint8_t *input;
	uint32_t length;
	uint8_t *aad;
	uint32_t aad_length;
	uint8_t *iv;
	uint32_t iv_length;
	bool f_encrypt;
	uint8_t *tag;
	uint8_t *output;
}CRYPTO_PARA_t;

typedef enum {
    I2C2AHB_DBG_LIMIT = 0,
    I2C2AHB_DBG_DISABLE,
    I2C2AHB_FLASHWR_DISABLE,
    SPI2AHB_DISABLE,
    SW_JTAG_DISABLE,
}FUSE_TYPE;


SecError_t himax_sec_init(void);
SecError_t himax_sec_deinit(void);
SecError_t himax_sec_crypto(SEC_CMD_TYPE type, uint32_t param, uint8_t *pOutput);
SecError_t himax_sec_hash(SEC_CMD_TYPE type, uint32_t param, uint8_t *pOutput);
SecError_t himax_sec_mac(SEC_CMD_TYPE type, uint32_t param, uint8_t *output);
SecError_t himax_sec_gen_keypair(SEC_CMD_TYPE type, uint32_t param, uint8_t *output);
SecError_t himax_sec_sign_verify(SEC_CMD_TYPE type, uint32_t param, uint32_t *state_asset_Id);
SecError_t himax_sec_key_derived(SEC_CMD_TYPE type, uint32_t param, uint8_t *pOutput);
SecError_t himax_sec_trng(SEC_CMD_TYPE type, uint32_t param, uint8_t *output);
SecError_t himax_sec_nop(SEC_CMD_TYPE type, uint32_t param);
SecError_t himax_sec_otp(SEC_CMD_TYPE type, uint32_t param, uint32_t *len);
SEC_BOOT_ERROR himax_sec_SB_image_process(bool header_export,uint8_t *Input_p,uint8_t *Output_p);
SEC_BOOT_ERROR himax_sec_OTA_image_process(bool use_cert_addr, uint8_t *Input_p, uint8_t *Output_p);
SecError_t himax_sec_SOCID(SEC_CMD_TYPE type, CCHashResult_t param);

SecError_t himax_sec_secure_debug(SEC_CMD_TYPE type,uint32_t param);
SecError_t himax_sec_crypto_key_from_otp(CRYPTO_PARA_t CRYPTO_para);
SecError_t himax_sec_HMAC_key_from_otp(HMAC_PARA_t HMAC_para);
SecError_t himax_sec_fuse(FUSE_TYPE type);

SEC_BOOT_ERROR himax_sec_set_flash_offset(uint32_t flash_offset);
SEC_BOOT_ERROR himax_sec_get_flash_offset(uint32_t *flash_offset);

#endif //_HX_HAL_SEC_H_
