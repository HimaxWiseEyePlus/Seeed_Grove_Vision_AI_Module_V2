#ifndef _HX_CC312_DRV_SEC_H_
#define _HX_CC312_DRV_SEC_H_

#include <stdio.h>
#include "hx_drv_def.h"
#include "cc_pal_log.h"
#include "WE2_debug.h"
#include "xprintf.h"
//******************************************
//otp
#if 0
#include "cc_otp_defs.h"
#include "dx_nvm.h"
#include "bsv_hw_defs.h"
#include "bsv_error.h"
#include "bsv_defs.h"
#include "cc_bitops.h"
#include "util.h"
#endif
#include "hx_otp_def.h"

//******************************************
//#include "bootimagesverifier_def.h"
//#include "secureboot_defs.h"
#include "hx_sb_def.h"

#include "prod_crypto_driver.h"

// tmp solution
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6100100)
    #include "cmsis_armclang.h"
#elif defined ( __GNUC__ )
    #include "cmsis_gcc.h"
#endif
#include "com_flash_boot.h"

/**
 *
 * Assert Macros
 *
 */
 
/** print a line */
//#define DEBUG_PRINT

#if 0
#ifdef DEBUG_PRINT
#define DBG_PRINT(format, ...)  \
                printf(format, ##__VA_ARGS__)
#else
#define DBG_PRINT(format, ...)
#endif
#endif
 
#define RUN_ASSERT(_c)                                                                \
                do {                                                                  \
                    rc = _c;                                                        \
                    if( rc != 0)                                                        \
                    {                                                                 \
                        dbg_printf(DBG_MORE_INFO, "Run %s failed [line: %d] rc = %d\n", __FUNCTION__, __LINE__, rc);                      \
                        rc = SEC_ERROR_FAIL;                                       \
                        goto bail;                                                    \
                    }                                                                 \
                }                                                                     \
                while(0)

//#include "driver_defs.h"
#define OTP_AHB_SW_CM_SIGNATURE_EN				0x00000001ULL
#define OTP_AHB_SW_DM_SIGNATURE_EN				0x00000010ULL


SecError_t hx_drv_sec_init(void);
SecError_t hx_drv_sec_deinit(void);
SecError_t hx_drv_aes(CryptoCtx_AES_t *crytoCtxAES, uint8_t *pOutput);
SecError_t hx_drv_aes_ae(CryptoInfo_AES_AE_t *crytoCtxAES, uint8_t *pOutput);
SecError_t hx_drv_rsa_crytpo(RSAEncDecInfo_t *rsaEncDecInfo,uint8_t *pOutput);
SecError_t hx_drv_hash(HashInfo_t *hashInfo,uint8_t *pOutput);
SecError_t hx_drv_RSA_keygen(RSAKeyGenPair_t *rsaKeyGenPair, uint8_t *pOutput);
SecError_t hx_drv_rsa_sign_verify(RSASignVerifyInfo_t *rsaSignVerifyInfo,uint8_t *pOutput);
SecError_t hx_drv_kdf(KDFDataInfo_t *kdfDataInfo, uint8_t *pOutput);
SecError_t hx_drv_kdf_extexp(HKDFDataInfo_t *hkdfDataInfo, uint8_t *pOutput);
SecError_t hx_drv_mac(MACDataInfo_t *macDataInfo, uint8_t *pOutput);

SecError_t hx_drv_ecdsa_sign_verify(ECDSADataInfo_t *ecdsaDataInfo,uint8_t *pOutput);
SecError_t hx_drv_ECC_keygen(ECCKeyPair_t *keyPair, uint8_t *pOutput);
uint32_t SEC_Header_Judge(uint32_t param);



/* RND function */
SecError_t rnd_init(void);
SecError_t rnd_deinit(void);


/*! Defines the CM life-cycle state value. */
#define CC_BSV_CHIP_MANUFACTURE_LCS     0x0
/*! Defines the DM life-cycle state value. */
#define CC_BSV_DEVICE_MANUFACTURE_LCS       0x1
/*! Defines the Secure life-cycle state value. */
#define CC_BSV_SECURE_LCS           0x5
/*! Defines the RMA life-cycle state value. */
#define CC_BSV_RMA_LCS              0x7
/* Identify platform: ARM MPS2 PLUS */

#define PUF_BASE_ADDRESS 0x43050000
#define PUF_OTP_BASE_ADDRESS 0x43050000 + 0x400

#define HX_MAX_NUM_OF_IMAGES 5

typedef struct {
        CCSbCertHeader_t		certHeader;
		CCSbNarams_t        	certPubKeyN;
        CCSbNParams_t        	certPubKey;
        ContentCertMain_t		certBody;
        CCSbSignature_t      	certSign;
		CCSbSwImgAddData_t		certnonesign;
} hx_ContentCert_t;

typedef struct
{   
	KeyCert_t keycert;
	hx_ContentCert_t contcert;
} SB_Header_t;


typedef struct {
	uint32_t N[2048/CC_BITS_IN_32BIT_WORD]; 				/*!< N public key, big endian representation. */
}CCSb2KNarams_t;

typedef struct {
	uint32_t sig[2048/CC_BITS_IN_32BIT_WORD];
}CCSb2KSignature_t;

typedef struct {
        CCSbCertHeader_t	certHeader;
		CCSb2KNarams_t        	certPubKeyN;
        CCSbNParams_t        	certPubKey;
        KeyCertMain_t		certBody;
        CCSb2KSignature_t      	certSign;
} KeyCert_2K_t;

typedef struct {
        CCSbCertHeader_t		certHeader;
		CCSb2KNarams_t        	certPubKeyN;
        CCSbNParams_t        	certPubKey;
        ContentCertMain_t		certBody;
        CCSb2KSignature_t      	certSign;
		CCSbSwImgAddData_t		certnonesign;
} ContentCert_2K_t;

typedef struct
{   
	KeyCert_2K_t keycert;
	ContentCert_2K_t contcert;
} SB_Header_2K_t;

typedef struct {
        CCSbCertHeader_t	certHeader;
		CCSb2KNarams_t      certPubKeyN;		
        CCSbNParams_t 		certPubKey;
        EnablerCertMain_t	certBody;
        CCSb2KSignature_t   certSign;
} EnablerCert_2K_t;

typedef struct {
        CCSbCertHeader_t	certHeader;
		CCSb2KNarams_t        certPubKeyN;		
        CCSbNParams_t       certPubKey;
        DeveloperCertMain_t	certBody;
        CCSb2KSignature_t     certSign;
} DeveloperCert_2K_t;

typedef struct
{
    uint8_t * Data_p;                   // Data part
    uint32_t ByteDataSize;              // Data size in bytes
} SEC_DATA_INFO;


typedef struct {
	uint32_t	FlashAddr;
	uint32_t	SramAddr;
	uint32_t	imageSize;
	uint32_t 	AES_Iv[AES_IV_COUNTER_SIZE_IN_WORDS];
}hx_Reencrypt_t;

typedef struct
{
	hx_Reencrypt_t certReencrypt[CC_SB_MAX_NUM_OF_IMAGES];
	uint32_t *gSwImagesAddData;
	uint8_t *gSwRecSignedData;
	CCswCryptoType_t gswCryptoType;
	CCswLoadVerifyScheme_t gswLoadVerifyScheme;
	uint8_t gnumOfSwComps;
	uint8_t Sw_count;
	uint32_t gnonce[AES_IV_COUNTER_SIZE_IN_WORDS];
}SEC_Reencrypt_INFO;


typedef struct
{
	uint32_t	*FlashAddr;
	uint32_t	*SramAddr;
	uint32_t	*imageSize;
	uint8_t 	Hbk_Id;
	uint8_t		CodeEncType;
	uint8_t 	numofswcomps;
	uint8_t 	verify_only_in_flash;
	uint32_t	headersize;
}SEC_GET_INFO;

SecError_t SEC_Header_GET_INFO(uint32_t Header,SEC_GET_INFO *param);

#define DXH_ENV_CC_HOST_INT_REG_OFFSET			0x46000F14
#define DXH_ENV_CC_WARM_BOOT_REG_OFFSET			0x46000300
#define DXH_ENV_CC_COLD_BOOT_REG_OFFSET			0x46000300
#define CC_BASE_OFFSET							0x43030000
#define DXH_NVM_IS_IDLE_REG_OFFSET        		CC_BASE_OFFSET + 0x1f10 
#define Write_Reset(Address, Value) 			((*(volatile uint32_t*)(Address)) = (Value))
#define Wait_Reset(Address, Mask, Expected) 	do{;}while(((*(volatile uint32_t*)(Address)) & (Mask)) != (Expected))

#if 0
/* Identify platform: ARM MPS2 PLUS */
#define DX_PLAT_MPS2_PLUS 1

#define DX_BASE_CC 0x43030000//0x50088000
#define DX_BASE_CODE 0x1E000000

#define DX_BASE_ENV_REGS 0x50088000//0x400A8000
#define DX_BASE_ENV_NVM_LOW 0x400AA000
#define DX_BASE_ENV_NVM_HI  0x400AB000
#define DX_BASE_ENV_PERF_RAM 0x400A9000

#define DX_BASE_HOST_RGF 0x0UL
#define DX_BASE_CRY_KERNEL     0x0UL

#define DX_BASE_RNG 0x0000UL

/*! Defines BSV base error. */
#define CC_BSV_BASE_ERROR                    	0x0B000000
/*! Illegal input parameter error. */
#define CC_BSV_ILLEGAL_INPUT_PARAM_ERR          (CC_BSV_BASE_ERROR + 0x00000001)
/*! OTP write compare failure error. */
#define CC_BSV_OTP_WRITE_CMP_FAIL_ERR			(CC_BSV_BASE_ERROR + 0x0000000A)
/*! Illegal operation error. */
#define CC_BSV_ILLEGAL_OPERATION_ERR			(CC_BSV_BASE_ERROR + 0x00000014)
/*! Illegal SW version error. */
#define CC_BSV_ILLEGAL_SW_VERSION_ERR			(CC_BSV_BASE_ERROR + 0x00000018)

/*! Definition for all ones word. */
#define CC_BSV_ALL_ONES_VALUE	0xffffffffUL
/*! Definition for number of bits in a 32bit word. */
#define CC_BSV_ALL_ONES_NUM_BITS 32

/*! ICV Firmware minimal version maximal size. */
#define CC_BSV_MAX_HBK0_VERSION_COUNTER		64
/*! OEM Firmware minimal version maximal size. */
#define CC_BSV_MAX_HBK1_VERSION_COUNTER		96
/*! OEM Firmware minimal version maximal size (no ICV). */
#define CC_BSV_MAX_HBK_VERSION_COUNTER		160
// --------------------------------------
// BLOCK: NVM
// --------------------------------------
#define DX_AIB_FUSE_PROG_COMPLETED_REG_OFFSET 	0x1F04UL 
#define DX_AIB_FUSE_PROG_COMPLETED_VALUE_BIT_SHIFT 	0x0UL
#define DX_AIB_FUSE_PROG_COMPLETED_VALUE_BIT_SIZE 	0x1UL
#define DX_NVM_DEBUG_STATUS_REG_OFFSET 	0x1F08UL 
#define DX_NVM_DEBUG_STATUS_VALUE_BIT_SHIFT 	0x1UL
#define DX_NVM_DEBUG_STATUS_VALUE_BIT_SIZE 	0x3UL
#define DX_LCS_IS_VALID_REG_OFFSET 	0x1F0CUL 
#define DX_LCS_IS_VALID_VALUE_BIT_SHIFT 	0x0UL
#define DX_LCS_IS_VALID_VALUE_BIT_SIZE 	0x1UL
#define DX_NVM_IS_IDLE_REG_OFFSET 	0x1F10UL 
#define DX_NVM_IS_IDLE_VALUE_BIT_SHIFT 	0x0UL
#define DX_NVM_IS_IDLE_VALUE_BIT_SIZE 	0x1UL
#define DX_LCS_REG_REG_OFFSET 	0x1F14UL 
#define DX_LCS_REG_LCS_REG_BIT_SHIFT 	0x0UL
#define DX_LCS_REG_LCS_REG_BIT_SIZE 	0x3UL
#define DX_LCS_REG_ERROR_KDR_ZERO_CNT_BIT_SHIFT 	0x8UL
#define DX_LCS_REG_ERROR_KDR_ZERO_CNT_BIT_SIZE 	0x1UL
#define DX_LCS_REG_ERROR_PROV_ZERO_CNT_BIT_SHIFT 	0x9UL
#define DX_LCS_REG_ERROR_PROV_ZERO_CNT_BIT_SIZE 	0x1UL
#define DX_LCS_REG_ERROR_KCE_ZERO_CNT_BIT_SHIFT 	0xAUL
#define DX_LCS_REG_ERROR_KCE_ZERO_CNT_BIT_SIZE 	0x1UL
#define DX_LCS_REG_ERROR_KPICV_ZERO_CNT_BIT_SHIFT 	0xBUL
#define DX_LCS_REG_ERROR_KPICV_ZERO_CNT_BIT_SIZE 	0x1UL
#define DX_LCS_REG_ERROR_KCEICV_ZERO_CNT_BIT_SHIFT 	0xCUL
#define DX_LCS_REG_ERROR_KCEICV_ZERO_CNT_BIT_SIZE 	0x1UL
#define DX_HOST_SHADOW_KDR_REG_REG_OFFSET 	0x1F18UL 
#define DX_HOST_SHADOW_KDR_REG_VALUE_BIT_SHIFT 	0x0UL
#define DX_HOST_SHADOW_KDR_REG_VALUE_BIT_SIZE 	0x1UL
#define DX_HOST_SHADOW_KCP_REG_REG_OFFSET 	0x1F1CUL 
#define DX_HOST_SHADOW_KCP_REG_VALUE_BIT_SHIFT 	0x0UL
#define DX_HOST_SHADOW_KCP_REG_VALUE_BIT_SIZE 	0x1UL
#define DX_HOST_SHADOW_KCE_REG_REG_OFFSET 	0x1F20UL 
#define DX_HOST_SHADOW_KCE_REG_VALUE_BIT_SHIFT 	0x0UL
#define DX_HOST_SHADOW_KCE_REG_VALUE_BIT_SIZE 	0x1UL
#define DX_HOST_SHADOW_KPICV_REG_REG_OFFSET 	0x1F24UL 
#define DX_HOST_SHADOW_KPICV_REG_VALUE_BIT_SHIFT 	0x0UL
#define DX_HOST_SHADOW_KPICV_REG_VALUE_BIT_SIZE 	0x1UL
#define DX_HOST_SHADOW_KCEICV_REG_REG_OFFSET 	0x1F28UL 
#define DX_HOST_SHADOW_KCEICV_REG_VALUE_BIT_SHIFT 	0x0UL
#define DX_HOST_SHADOW_KCEICV_REG_VALUE_BIT_SIZE 	0x1UL
#define DX_OTP_ADDR_WIDTH_DEF_REG_OFFSET 	0x1F2CUL 
#define DX_OTP_ADDR_WIDTH_DEF_VALUE_BIT_SHIFT 	0x0UL
#define DX_OTP_ADDR_WIDTH_DEF_VALUE_BIT_SIZE 	0x4UL

/* NVM definitions */
#define CC_OTP_BASE_ADDR			0x2000UL
#define CC_OTP_START_OFFSET			0x00UL
#define CC_OTP_LAST_OFFSET			0x7FFUL

/* [0x00-0x07] Device root key (HUK) */
#define CC_OTP_HUK_OFFSET			0x00UL
#define CC_OTP_HUK_SIZE_IN_WORDS              	8

/* [0x08-0x0B] ICV provisioning secret (KPICV) */
#define CC_OTP_KPICV_OFFSET			0x08UL
#define CC_OTP_KPICV_SIZE_IN_WORDS              4

/* [0x0C-0x0F] ICV Code encryption key (KCEICV) */
#define CC_OTP_KCEICV_OFFSET			0x0CUL
#define CC_OTP_KCEICV_SIZE_IN_WORDS             4

/* [0x10] Manufacturer-programmed flags */
#define CC_OTP_MANUFACTURE_FLAG_OFFSET		0x10UL

/* [0x11-0x18] Root-of-Trust Public Key.
* May be used in one of the following configurations:
* - A single 256-bit SHA256 digest of the Secure Boot public key (HBK). 								       :
* - Two 128-bit truncated SHA256 digests of Secure Boot public keys 0 and 1 (HBK0, HBK1) */
#define CC_OTP_HBK_OFFSET			0x11UL
#define CC_OTP_HBK_SIZE_IN_WORDS         	8
#define CC_OTP_HBK0_OFFSET			0x11UL
#define CC_OTP_HBK0_SIZE_IN_WORDS         	4
#define CC_OTP_HBK1_OFFSET			0x15UL
#define CC_OTP_HBK1_SIZE_IN_WORDS         	4

/* [0x19-0x1C] OEM provisioning secret (Kcp) */
#define CC_OTP_KCP_OFFSET			0x19UL
#define CC_OTP_KCP_SIZE_IN_WORDS              	4

/* OEM Code encryption key (KCE) */
#define CC_OTP_KCE_OFFSET			0x1DUL
#define CC_OTP_KCE_SIZE_IN_WORDS             	4

/* OEM-programmed flags */
#define CC_OTP_OEM_FLAG_OFFSET			0x21UL

/* HBK Trusted Firmware minimum version (anti-rollback counter) */
#define CC_OTP_HBK_MIN_VERSION_OFFSET		0x22UL
#define CC_OTP_HBK_MIN_VERSION_SIZE_IN_WORDS	5

/* HBK0 Trusted Firmware minimum version (anti-rollback counter) */
#define CC_OTP_HBK0_MIN_VERSION_OFFSET		0x22UL
#define CC_OTP_HBK0_MIN_VERSION_SIZE_IN_WORDS	2

/* HBK1 Trusted Firmware minimum version (anti-rollback counter) */
#define CC_OTP_HBK1_MIN_VERSION_OFFSET		0x24UL
#define CC_OTP_HBK1_MIN_VERSION_SIZE_IN_WORDS	3

/* General purpose configuration flags */
#define CC_OTP_ICV_GENERAL_PURPOSE_FLAG_OFFSET	0x27UL
#define CC_OTP_ICV_GENERAL_PURPOSE_FLAG_SIZE_IN_WORDS	1

/* OTP DCU lock mask */
#define CC_OTP_DCU_OFFSET			0x28UL
#define CC_OTP_DCU_SIZE_IN_WORDS             	4

/* First stage secure boot loader code and data sections (optional) */
#define CC_OTP_SB_LOADER_CODE_OFFSET		0x2CUL


/* Manufacturer-programmed flags */

/* [7:0] Number of "0" bits in HUK */
#define CC_OTP_MANUFACTURE_FLAG_HUK_ZERO_BITS_BIT_SHIFT        0
#define CC_OTP_MANUFACTURE_FLAG_HUK_ZERO_BITS_BIT_SIZE         8

/* [14:8] Number of "0" bits in KPICV (128 bit) */
#define CC_OTP_MANUFACTURE_FLAG_KPICV_ZERO_BITS_BIT_SHIFT      8
#define CC_OTP_MANUFACTURE_FLAG_KPICV_ZERO_BITS_BIT_SIZE       7

/* [15:15] KPICV "Not In Use" bit */
#define CC_OTP_MANUFACTURE_FLAG_KPICV_NOT_IN_USE_BIT_SHIFT     15
#define CC_OTP_MANUFACTURE_FLAG_KPICV_NOT_IN_USE_BIT_SIZE       1

/* [22:16] Number of "0" bits in KCEICV */
#define CC_OTP_MANUFACTURE_FLAG_KCEICV_ZERO_BITS_BIT_SHIFT     16
#define CC_OTP_MANUFACTURE_FLAG_KCEICV_ZERO_BITS_BIT_SIZE       7

/* [23:23] KCEICV "Not In Use" bit */
#define CC_OTP_MANUFACTURE_FLAG_KCEICV_NOT_IN_USE_BIT_SHIFT    23
#define CC_OTP_MANUFACTURE_FLAG_KCEICV_NOT_IN_USE_BIT_SIZE      1

/* [30:24] Number of "0" bits in HBK0 (in case it is used as 4 words of the ICV) */
#define CC_OTP_MANUFACTURE_FLAG_HBK0_ZERO_BITS_BIT_SHIFT       24
#define CC_OTP_MANUFACTURE_FLAG_HBK0_ZERO_BITS_BIT_SIZE         7

/* [31:31] HBK0 "Not In Use" bit */
#define CC_OTP_MANUFACTURE_FLAG_HBK0_NOT_IN_USE_BIT_SHIFT      31
#define CC_OTP_MANUFACTURE_FLAG_HBK0_NOT_IN_USE_BIT_SIZE        1


/* OEM-programmed flags */

/* [7:0] Number of "0" bits in HBK1/HBK (128/256 bits public key) */
#define CC_OTP_OEM_FLAG_HBK_ZERO_BITS_BIT_SHIFT       	0
#define CC_OTP_OEM_FLAG_HBK_ZERO_BITS_BIT_SIZE        	8
#define CC_OTP_OEM_FLAG_HBK1_ZERO_BITS_BIT_SHIFT       	0
#define CC_OTP_OEM_FLAG_HBK1_ZERO_BITS_BIT_SIZE        	8

/* [14:8] Number of "0" bits in KCP (128 bit) */
#define CC_OTP_OEM_FLAG_KCP_ZERO_BITS_BIT_SHIFT       	8
#define CC_OTP_OEM_FLAG_KCP_ZERO_BITS_BIT_SIZE        	7

/* [15:15] KCP "Not In Use" bit */
#define CC_OTP_OEM_FLAG_KCP_NOT_IN_USE_BIT_SHIFT       15
#define CC_OTP_OEM_FLAG_KCP_NOT_IN_USE_BIT_SIZE         1

/* [22:16] Number of "0" bits in KCE */
#define CC_OTP_OEM_FLAG_KCE_ZERO_BITS_BIT_SHIFT        16
#define CC_OTP_OEM_FLAG_KCE_ZERO_BITS_BIT_SIZE        	7

/* [23:23] KCE "Not In Use" bit */
#define CC_OTP_OEM_FLAG_KCE_NOT_IN_USE_BIT_SHIFT       23
#define CC_OTP_OEM_FLAG_KCE_NOT_IN_USE_BIT_SIZE         1

/* [29:24] Reserved */

/* [30:30] OEM RMA mode flag */
#define CC_OTP_OEM_FLAG_OEM_RMA_MODE_BIT_SHIFT         30
#define CC_OTP_OEM_FLAG_OEM_RMA_MODE_BIT_SIZE         	1

/* [31:31] ICV RMA mode flag */
#define CC_OTP_OEM_FLAG_ICV_RMA_MODE_BIT_SHIFT         31
#define CC_OTP_OEM_FLAG_ICV_RMA_MODE_BIT_SIZE           1









/*! Reads a 32-bit value from a CryptoCell-312 memory-mapped register. */
#define SB_HAL_READ_REGISTER(addr,val)  \
        ((val) = (*((volatile uint32_t*)(addr))))
#define SB_HAL_WRITE_REGISTER(addr,val)     \
        ((*((volatile uint32_t*)(addr))) = (unsigned long)(val))
/*! Definition for bitmask */
#define BITMASK(mask_size) (((mask_size) < 32) ?	\
	((1UL << (mask_size)) - 1) : 0xFFFFFFFFUL)
/*! Definition for getting bits value from a word. */
#define BITFIELD_GET(word, bit_offset, bit_size) \
	(((word) >> (bit_offset)) & BITMASK(bit_size))
/*! Bit fields get */
#define CC_REG_FLD_GET(unit_name, reg_name, fld_name, reg_val)	      \
	(DX_ ## reg_name ## _ ## fld_name ## _BIT_SIZE == 0x20 ?	      \
	reg_val /*!< \internal Optimization for 32b fields */ :			      \
	BITFIELD_GET(reg_val, DX_ ## reg_name ## _ ## fld_name ## _BIT_SHIFT, \
		     DX_ ## reg_name ## _ ## fld_name ## _BIT_SIZE))
/*! Bit fields access */
#define CC_REG_FLD_GET2(unit_name, reg_name, fld_name, reg_val)	      \
	(CC_ ## reg_name ## _ ## fld_name ## _BIT_SIZE == 0x20 ?	      \
	reg_val /*!< \internal Optimization for 32b fields */ :			      \
	BITFIELD_GET(reg_val, CC_ ## reg_name ## _ ## fld_name ## _BIT_SHIFT, \
		     CC_ ## reg_name ## _ ## fld_name ## _BIT_SIZE))
/* Register Offset macro */
#define CC_REG_OFFSET(unit_name, reg_name)               \
	(DX_BASE_ ## unit_name + DX_ ## reg_name ## _REG_OFFSET)
/************************ Defines ******************************/
#define SB_REG_ADDR(base, reg_name) 	(base + CC_REG_OFFSET(CRY_KERNEL, reg_name))
/* calc OTP memory length:
   read RTL OTP address width. The supported sizes are 6 (for 2 Kbits),7,8,9,10,11 (for 64 Kbits).
   convert value parameter to addresses of 32b words */
#define CC_BSV_GET_OTP_LENGTH(hwBaseAddress, otpLength)							\
	do { 												\
		SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, OTP_ADDR_WIDTH_DEF), otpLength);	\
		otpLength = CC_REG_FLD_GET(0, OTP_ADDR_WIDTH_DEF, VALUE, otpLength);			\
		otpLength = (1 << otpLength);								\
	}while(0)
/* check Hbk configuration in OTP memory */
#define CC_BSV_IS_HBK_FULL(hwBaseAddress, isHbkFull, error)								\
	do { 													  	\
		error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_MANUFACTURE_FLAG_OFFSET, &isHbkFull);			\
		isHbkFull = CC_REG_FLD_GET2(0, OTP_MANUFACTURE_FLAG, HBK0_NOT_IN_USE, isHbkFull);			\
	}while(0)
/* poll on the AIB acknowledge bit */
#define CC_BSV_WAIT_ON_AIB_ACK_BIT(hwBaseAddress) 		\
	do { 												\
		uint32_t regVal; 								\
		do { 											\
			SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, AIB_FUSE_PROG_COMPLETED), regVal); \
		}while( !(regVal & 0x1 ));						\
	}while(0)
/* read a word directly from OTP memory */
#define CC_BSV_READ_OTP_WORD(hwBaseAddress, otpAddr, otpData)							\
	do { 																				\
		SB_HAL_READ_REGISTER((hwBaseAddress + CC_OTP_BASE_ADDR + otpAddr), otpData); 	\
	}while(0)
/* write a word directly from OTP memory */
#define CC_BSV_WRITE_OTP_WORD(hwBaseAddress, otpAddr, otpData)							\
	do { 																				\
		SB_HAL_WRITE_REGISTER((hwBaseAddress + CC_OTP_BASE_ADDR + otpAddr), otpData);	\
		CC_BSV_WAIT_ON_AIB_ACK_BIT(hwBaseAddress);  									\
	}while(0)
	
	/* MACRO to count one bits */
#define COUNT_ONE_BITS(number, BitCount) \
do \
{ \
      uint32_t tmp_num = number; \
      BitCount = 0; \
      while (tmp_num)	\
      {			\
        tmp_num = tmp_num & (tmp_num - 1); \
        BitCount = BitCount + 1; \
      }			\
} while (0)

/* poll NVM register to assure that the NVM boot is finished (and LCS and the keys are valid) */
#define CC_BSV_WAIT_ON_NVM_IDLE_BIT(hwBaseAddress) 						\
	do { 											\
		uint32_t regVal; 								\
		do { 										\
			SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, NVM_IS_IDLE), regVal); \
			regVal = CC_REG_FLD_GET(0, NVM_IS_IDLE, VALUE, regVal); 		\
		}while( !regVal ); 								\
	}while(0)
#endif
/************************ Defines ******************************/
extern unsigned long gCcRegBase;

#define SB_REG_ADDR(base, reg_name) 	(base + CC_REG_OFFSET(CRY_KERNEL, reg_name))
#define SB_REG_ADDR_UNIT(base, reg_name, unit) 	(base + CC_REG_OFFSET(unit, reg_name))
#define SB_HAL_READ_REGISTER(addr,val)  \
        ((val) = (*((volatile uint32_t*)(addr))))
#define SB_HAL_WRITE_REGISTER(addr,val)     \
        ((*((volatile uint32_t*)(addr))) = (unsigned long)(val))
#define SB_HAL_WRITE_Wait(Address, Mask, Expected) 	do{;}while(((*(volatile uint32_t*)(Address)) & (Mask)) != (Expected))
        

CCError_t CC_BsvLcsGet(unsigned long hwBaseAddress,uint32_t *pLcs);
CCError_t CC_BsvOTPWordRead(unsigned long hwBaseAddress, uint32_t otpAddress, uint32_t *pOtpWord);
CCError_t CC_BsvOTPWordWrite(unsigned long hwBaseAddress, uint32_t otpAddress, uint32_t otpWord);
CCError_t CC_BsvSwVersionGet(unsigned long hwBaseAddress,CCSbPubKeyIndexType_t keyIndex,uint32_t *swVersion);
CCError_t CC_BsvSwVersionSet(unsigned long hwBaseAddress,CCSbPubKeyIndexType_t keyIndex, uint32_t swVersion);

CCError_t CC_SbCertChainVerificationInit(
	CCSbCertInfo_t *certPkgInfo		/*!< [in/out] Pointer to the information about the certificate package */
	);
CCError_t CC_SbCertVerifySingle(
	CCSbFlashReadFunc flashReadFunc,	/*!< [in] Pointer to the flash read function. */
	void *userContext,			/*!< [in] An additional pointer for flashRead usage. May be NULL. */
	unsigned long hwBaseAddress,		/*!< [in] CryptoCell HW registers' base address. */
	CCAddr_t certStoreAddress,		/*!< [in] Flash address where the certificate is located. This address is provided to flashReadFunc. */
	CCSbCertInfo_t *certPkgInfo,		/*!< [in/out] Pointer to the information about the certificate package. */
	uint32_t  *pHeader,                 /*!< [in/out] Pointer to a buffer used for extracting the X509 TBS Headers. Must be NULL for proprietary certificates. */
	uint32_t  headerSize,			/*!< [in] The size of pHeader in bytes. Must be 0 for proprietary certificates. */
	uint32_t *pWorkspace,			/*!< [in] Buffer for the function's internal use. */
	uint32_t workspaceSize			/*!< [in] The size of the workspace in bytes. Must be at least CC_SB_MIN_WORKSPACE_SIZE_IN_BYTES. */
	);
CCError_t CCCommonCertVerify(unsigned long   hwBaseAddress,
                             BufferInfo32_t  *pCertInfo,
                             CertFieldsInfo_t  *pCertFields,  // in/out
                             CCSbCertInfo_t  *pSbCertInfo,   //in/out
                             BufferInfo32_t  *pWorkspaceInfo,
                             BufferInfo32_t  *pX509HeaderInfo);

uint32_t CCCommonKeyCertVerify(unsigned long   hwBaseAddress,
                               uint32_t	certFlags,
                               uint8_t  *pCertMain,
                               CCSbCertInfo_t *pCertPkgInfo);

uint32_t CCCommonContentCertVerify(CCSbFlashReadFunc flashReadFunc,
                                   void *userContext,
                                   unsigned long hwBaseAddress,
                                   CCAddr_t certStoreAddress,
                                   CCSbCertInfo_t *certPkgInfo,
                                   uint32_t	certFlags,
                                   uint8_t *pCertMain,
                                   BufferInfo32_t  *pWorkspaceInfo);
CCError_t CCSbImageLoadAndVerify(CCSbFlashReadFunc preHashflashRead_func,
					   void *preHashUserContext,
					   unsigned long hwBaseAddress,
					   uint8_t isLoadFromFlash,
					   uint8_t isVerifyImage,
					   CCswLoadVerifyScheme_t swLoadVerifyScheme,
					   bsvCryptoMode_t cryptoMode,
					   cryptoKeyType_t  keyType,
					   AES_Iv_t	AESIv,
					   uint8_t *pSwRecSignedData,
					   uint32_t *pSwRecNoneSignedData,
					   uint32_t *workspace_ptr,
					   uint32_t workspaceSize);
CCError_t CCSbVerifyNvCounter(unsigned long hwBaseAddress, uint32_t swVersion, CCSbCertInfo_t *certPkgInfo);
#if 0
CCError_t CCSbSetNvCounter(unsigned long hwBaseAddress, CCSbCertInfo_t *certPkgInfo);
#endif
//CCError_t NVM_GetLCS(unsigned long hwBaseAddress, uint32_t *lcs_ptr);
CCError_t NVM_ReadHASHPubKey(unsigned long hwBaseAddress, CCSbPubKeyIndexType_t pubKeyIndex, CCHashResult_t PubKeyHASH, uint32_t hashSizeInWords);
CCError_t NVM_ReadAESKey(unsigned long hwBaseAddress, AES_Key_t AESKey);
CCError_t CCSbCalcPublicKeyHASH(unsigned long hwBaseAddress,
				uint32_t *NAndRevNp_ptr,
				uint32_t *hashResult);
CCError_t CCSbCalcPublicKeyHASHAndCompare(unsigned long hwBaseAddress,
					     uint32_t *NAndRevNp_ptr,
					     uint32_t *NHASH_ptr,
					     uint32_t HashSize);
CCError_t CCSbVerifySignature(unsigned long hwBaseAddress,
				uint32_t *pData,
				CCSbNarams_t *pNarams,
				CCSbNParams_t *pNParams,
				CCSbSignature_t *pSignature,
				uint32_t sizeOfData,
				CCSbSignAlg_t sigAlg);
CCError_t CC_BsvPubKeyHashGet(unsigned long hwBaseAddress, 
			      CCSbPubKeyIndexType_t keyIndex, 
			      uint32_t *hashedPubKey, 
			      uint32_t hashResultSizeWords);

void UTIL_ReverseBuff( uint8_t *pBuff , uint32_t size );
void UTIL_ReverseMemCopy( uint8_t *pDst, uint8_t *pSrc, uint32_t size );
SEC_BOOT_ERROR himax_drv_SB_image_Verify(uint8_t *Input_p,uint8_t *Output_p);
//SEC_BOOT_ERROR himax_drv_SB_image_process(SB_Header_t *Header_in,uint8_t *Input_p,SB_Header_t * Header_out,uint8_t *Output_p);
SEC_BOOT_ERROR himax_drv_SB_image_process(void *Header_in,uint8_t *Input_p,void * Header_out,uint8_t *Output_p);
CCError_t Get_RSA(uint32_t *RSA_VALUE);

uint32_t runIt_SBReadWrap(void* flashAddress, /*! Flash address to read from */
                             uint8_t *memDst, /*! memory destination to read the data to */
                             uint32_t sizeToRead, /*! size to read from Flash (in bytes) */
                             void* context);
uint8_t* hx_get_SB_addr(void);
#define MASK_WORD_ALIGN 			(CC_32BIT_WORD_SIZE-1)

#define DCU0_RESET_OVERRIDE   			(0x1UL)
#define DCU_DISABLE_ALL_DBG   			(0x0UL)
#define DCU_ENABLE_ALL_DBG   			(~(DCU_DISABLE_ALL_DBG))

#define NO_CERT_EXIT_OK			0x1


typedef struct {
        Dcu_t      	debugMask;
        Dcu_t      	debugLock;
        CCSbCertInfo_t sbCertInfo;
} DbgPkgInfo_t;


typedef enum BsvItError_t
{
    BSVIT_ERROR__OK = 0,
    BSVIT_ERROR__FAIL = 0x0000FFFF,
}BsvItError_t;

CCError_t CC_BsvInit(
    /*! [in] The base address for CryptoCell HW registers. */
    unsigned long hwBaseAddress
    );

CCError_t CC_BsvLcsGetAndInit(
    /*! [in] The base address for CryptoCell HW registers. */
    unsigned long hwBaseAddress,
    /*! [out] The returned life-cycle state. */
	uint32_t *pLcs
    );
CCError_t CC_BsvOTPPrivateKeysErase(
    /*! [in] The base address for CryptoCell HW registers. */
	unsigned long hwBaseAddress,
	/*! [in] The HUK secret key: CC_TRUE: HUK is erased. CC_FALSE: HUK
	remains unchanged. */
    CCBool_t isHukErase,
    /*! [in] Kpicv secret key: CC_TRUE: Kpicv is erased. CC_FALSE: Kpicv
	remains unchanged. */
	CCBool_t isKpicvErase,
    /*! [in] Kceicv secret key: CC_TRUE: Kceicv is erased. CC_FALSE:
	Kceicv remains unchanged. */
	CCBool_t isKceicvErase,
    /*! [in] Kcp secret key: CC_TRUE: Kcp is erased. CC_FALSE: Kcp
	remains unchanged.  */
	CCBool_t isKcpErase,
    /*! [in] Kce secret key: CC_TRUE: Kce is erased. CC_FALSE: Kce
	remains unchanged. */
	CCBool_t isKceErase,
    /*! [out] Returned status word. */
	uint32_t *pStatus
    );
CCError_t CC_BsvCoreClkGatingEnable(
    /*! [in] The base address for CryptoCell HW registers. */
	unsigned long hwBaseAddress
    );
CCError_t CC_BsvSecModeSet(
    /*! [in] The base address for CryptoCell HW registers. */
	unsigned long hwBaseAddress,
    /*! [in] The APB secure filter mode: CC_TRUE: only secure accesses are
	served. CC_FALSE: both secure and non-secure accesses are served. */
	CCBool_t isSecAccessMode,
    /*! [in] The APB security lock mode: CC_TRUE: secure filter mode is locked
	for further changes. CC_FALSE: secure filter mode is not locked. */
	CCBool_t isSecModeLock
    );
CCError_t CC_BsvPrivModeSet(
    /*! [in] The base address for CryptoCell HW registers. */
	unsigned long hwBaseAddress,
    /*! [in] The APB privileged mode: CC_TRUE: only privileged accesses are
	served. CC_FALSE: both privileged and non-privileged accesses are served. */
	CCBool_t isPrivAccessMode,
    /*! [in] The privileged lock mode: CC_TRUE: privileged mode is locked for
	further changes. CC_FALSE: privileged mode is not locked. */
	CCBool_t isPrivModeLock
    );

CCError_t CC_BsvSocIDCompute(
                 /*! [in] The base address for CryptoCell HW registers. */
				 unsigned long hwBaseAddress,
                 /*! [out] The derived SOC ID. */
				 CCHashResult_t hashResult
    );

CCError_t CC_BsvSecureDebugSet(
    /*! [in] The base address for CryptoCell HW registers. */
	unsigned long   hwBaseAddress,
    /*! [in] A pointer to the Secure Debug certificate package. NULL is a
	valid value. */
	uint32_t   *pDebugCertPkg,
    /*! [in] The size of the certificate package in bytes. */
	uint32_t   certPkgSize,
    /*! [out] The RMA entry flag. Non-zero value indicates entry into RMA LCS
	is required. */
	uint32_t   *pEnableRmaMode,
    /*! [in] A pointer to a buffer used internally. */
	uint32_t   *pWorkspace,
    /*! [in] The size of the buffer used internally. Minimal size is
	CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES. */
	uint32_t   workspaceSize
    );
typedef enum SEC_DEBUG_DCU_S {
	DEBUG_PERIPHERAL_SDC_600_AP_ENABLE				= 0x0001,
	DEBUG_PERIPHERAL_SDC_600_AP_SECURE_ENABLE		= 0x0002,
	M55_BIG_SDC_600_AP_ENABLE						= 0x0004,
	M55_BIG_SDC_600_AP_AP_SECURE_ENABLE 			= 0x0008,
	M55_LITTLE_SDC_600_AP_ENABLE					= 0x0010,
	M55_LITTLE_SDC_600_AP_SECURE_ENABLE 			= 0x0020,
	SDC_600_COM_PORT_DISABLE						= 0x0040,
	ESS600_INVASIVE_DEBUG_ENABLE					= 0x0080,
	ESS600_NON_INVASIVE_DEBUG_ENABLE				= 0x0100,
	M55_BIG_INVASIVE_DEBUG_ENABLE					= 0x0200,
	M55_BIG_NON_INVASIVE_DEBUG_ENABLE				= 0x0400,
	M55_BIG_SECURE_INVASIVE_DEBUG_ENABLE			= 0x0800,
	M55_BIG_SECURE_NON_INVASIVE_DEBUG_ENABLE		= 0x1000,
	M55_LITTLE_INVASIVE_DEBUG_ENABLE				= 0x2000,
	M55_LITTLE_NON_INVASIVE_DEBUG_ENABLE			= 0x4000,
	M55_LITTLE_SECURE_INVASIVE_DEBUG_ENABLE 		= 0x8000,
	M55_LITTLE_SECURE_NON_INVASIVE_DEBUG_ENABLE 	= 0x10000,
}SEC_DEBUG_DCU_E;

typedef enum
{
	TYPE_DCU1			= 0x00,
	TYPE_DCU2			= 0x01,
	TYPE_DCU3			= 0x02,
	TYPE_DCU4			= 0x03,
}SEC_DEBUG_DCU_TYPE_E;

SecError_t hx_drv_secure_debug(SEC_DATA_INFO *debug_info);

CCError_t CC_BsvKeyDerivation(	unsigned long		hwBaseAddress,
				CCBsvKeyType_t			keyType,
				uint32_t			*pUserKey,
				size_t				userKeySize,
				const uint8_t		*pLabel,
				size_t				labelSize,
				const uint8_t		*pContextData,
				size_t				contextSize,
				uint8_t 		*pDerivedKey,
				size_t			derivedKeySize);


#define TEST_CALC_BUFF_ZEROS(wordBuf, buffWordSize, zeros) {\
    uint32_t i = 0;\
    uint32_t j = 0;\
    uint32_t mask = 0;\
    zeros = 0;\
    for (i = 0; i< buffWordSize; i++) {\
        for (j = 0; j< CC_BITS_IN_32BIT_WORD; j++) {\
            mask = 0x1;\
            if (!(*(wordBuf+i) & (mask << j))) {\
                zeros++;\
            }\
        }\
    }\
}
CCError_t CC_BsvRMAModeEnable(
    /*! [in] The base address for CryptoCell HW registers. */
	unsigned long hwBaseAddress
    );

#include "cc_util_error.h"

/*! The maximal length of the label in bytes. */
#define CC_UTIL_MAX_LABEL_LENGTH_IN_BYTES   64
/*! The maximal length of the context in bytes. */
#define CC_UTIL_MAX_CONTEXT_LENGTH_IN_BYTES     64
/*! The minimal size of the fixed data in bytes. */
#define CC_UTIL_FIX_DATA_MIN_SIZE_IN_BYTES  3 /*!< \internal counter, 0x00, lengt(-0xff) */
/*! The maximal size of the fixed data in bytes. */
#define CC_UTIL_FIX_DATA_MAX_SIZE_IN_BYTES  4 /*!< \internal counter, 0x00, lengt(0x100-0xff0) */
/*! The maximal size of the derived-key material in bytes. */
#define CC_UTIL_MAX_KDF_SIZE_IN_BYTES (CC_UTIL_MAX_LABEL_LENGTH_IN_BYTES+CC_UTIL_MAX_CONTEXT_LENGTH_IN_BYTES+CC_UTIL_FIX_DATA_MAX_SIZE_IN_BYTES)
/*! The maximal size of the derived-key in bytes. */
#define CC_UTIL_MAX_DERIVED_KEY_SIZE_IN_BYTES 4080

#define CC_ASSET_PROV_MAX_ASSET_SIZE  (4*CC_1K_SIZE_IN_BYTES)

#define CC_ASSET_PROV_TOKEN     0x41736574UL
#define CC_ASSET_PROV_VERSION   0x10000UL

#define CC_ASSET_PROV_NONCE_SIZE    12
#define CC_ASSET_PROV_RESERVED_SIZE     8
#define CC_ASSET_PROV_RESERVED_WORD_SIZE    (CC_ASSET_PROV_RESERVED_SIZE/CC_32BIT_WORD_SIZE)
#define CC_ASSET_PROV_TAG_SIZE      16
#define CC_ASSET_PROV_BLOCK_SIZE    16

#define CC_ASSET_PROV_ADATA_SIZE    (3*CC_32BIT_WORD_SIZE+CC_ASSET_PROV_RESERVED_SIZE)  // token||version||assetSize||reserved

#define CC_ASSET_PROV_MAX_ASSET_PKG_SIZE  4144

typedef uint8_t CCUtilAesCmacResult_t[CC_UTIL_AES_CMAC_RESULT_SIZE_IN_BYTES];

/*! The type of key used to pack the asset. */
typedef enum {
       /*! The ICV provisioning key (Kpicv) key was used to pack the asset. */
       ASSET_PROV_KEY_TYPE_KPICV = 1,
       /*! The OEM provisioning key (Kcp) key was used to pack the asset. */
       ASSET_PROV_KEY_TYPE_KCP = 2,
       /*! Reserved. */
       ASSET_PROV_KEY_TYPE_RESERVED    = 0x7FFFFFFF,
} CCAssetProvKeyType_t;

#define CC_ASSET_PROV_MAX_ASSET_SIZE  (4*CC_1K_SIZE_IN_BYTES)

#define CC_ASSET_PROV_TOKEN     0x41736574UL
#define CC_ASSET_PROV_VERSION   0x10000UL

#define CC_ASSET_PROV_NONCE_SIZE    12
#define CC_ASSET_PROV_RESERVED_SIZE     8
#define CC_ASSET_PROV_RESERVED_WORD_SIZE    (CC_ASSET_PROV_RESERVED_SIZE/CC_32BIT_WORD_SIZE)
#define CC_ASSET_PROV_TAG_SIZE      16
#define CC_ASSET_PROV_BLOCK_SIZE    16

#define CC_ASSET_PROV_ADATA_SIZE    (3*CC_32BIT_WORD_SIZE+CC_ASSET_PROV_RESERVED_SIZE)  // token||version||assetSize||reserved


typedef struct {
        uint32_t  token;
        uint32_t  version;
        uint32_t  assetSize;
        uint32_t  reserved[CC_ASSET_PROV_RESERVED_WORD_SIZE];
        uint8_t   nonce[CC_ASSET_PROV_NONCE_SIZE];
        uint8_t   encAsset[CC_ASSET_PROV_MAX_ASSET_SIZE+CC_ASSET_PROV_TAG_SIZE];
}CCAssetProvPkg_t;
typedef enum  {
	UTIL_USER_KEY = 0,
	UTIL_ROOT_KEY = 1,
	UTIL_KCP_KEY = 2,
	UTIL_KCE_KEY = 3,
	UTIL_KPICV_KEY = 4,
	UTIL_KCEICV_KEY = 5,
	UTIL_END_OF_KEY_TYPE = 0x7FFFFFFF
}UtilKeyType_t;

/*! User buffer attribute (secure / non-secure). */
#define DATA_BUFFER_IS_NONSECURE    1
#define DATA_BUFFER_IS_SECURE       0

/*! get current Image.txt address is read from ouside or in Image.txt */
bool get_use_cert_addr_status();

/*! CC312 DMA */
SecError_t hx_crypto_dma_run(uint32_t inputDataAddr, uint32_t outputDataAddr, uint32_t data_size);


/*! secure/non-secure memory region check */
SECMEM_t check_secure_mem(uint32_t dest_addr, uint32_t len);
/*! check fw partition's memory is secure/non-secure/violation */
SECMEM_t check_secure_mem_with_hbkid(uint32_t dest_addr, uint32_t len);
    
/*! OTP PUF hard macro sleep active */
SecError_t hx_otp_puf_switch(HARD_MACRO_STATE state);
/*! User Filtering control  */
SecError_t hx_user_filtering_ctl(bool enable);
    
#endif //_HX_CC312_DRV_SEC_H_
