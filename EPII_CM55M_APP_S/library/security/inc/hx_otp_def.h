//********************************************************************************************/
#if 0
#include "hx_cc312_drv_sec.h"
#include "secureboot_general_hwdefs.h"
#include "dx_crys_kernel.h"
//#include "secureboot_stage_defs.h"
#endif

//_CC_OTP_DEFS_H
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
//********************************************************************************************/
//__DX_NVM_H__
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

//********************************************************************************************/
//_BSV_HW_DEFS_H
/* ********************** Macros ******************************* */

/* poll NVM register to assure that the NVM boot is finished (and LCS and the keys are valid) */
#define CC_BSV_WAIT_ON_NVM_IDLE_BIT(hwBaseAddress) 						\
	do { 											\
		uint32_t regVal; 								\
		do { 										\
			SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, NVM_IS_IDLE), regVal); \
			regVal = CC_REG_FLD_GET(0, NVM_IS_IDLE, VALUE, regVal); 		\
		}while( !regVal ); 								\
	}while(0)

/* poll on the AIB acknowledge bit */
#define CC_BSV_WAIT_ON_AIB_ACK_BIT(hwBaseAddress) 		\
	do { 												\
		uint32_t regVal; 								\
		do { 											\
			SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, AIB_FUSE_PROG_COMPLETED), regVal); \
		}while( !(regVal & 0x1 ));						\
	}while(0)

/* check HUK error bit in LCS register */
#define CC_BSV_IS_OTP_HUK_ERROR(hwBaseAddress, regVal) 						\
	do { 											\
		SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, LCS_REG), regVal);		\
		regVal = CC_REG_FLD_GET(0, LCS_REG, ERROR_KDR_ZERO_CNT, regVal);		\
	}while(0)

/* check KPICV error bit in LCS register */
#define CC_BSV_IS_OTP_KPICV_ERROR(hwBaseAddress, regVal) 					\
	do { 											\
		SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, LCS_REG), regVal);		\
		regVal = CC_REG_FLD_GET(0, LCS_REG, ERROR_KPICV_ZERO_CNT, regVal);		\
	}while(0)

/* check KCEICV error bit in LCS register */
#define CC_BSV_IS_OTP_KCEICV_ERROR(hwBaseAddress, regVal) 					\
	do { 											\
		SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, LCS_REG), regVal);		\
		regVal = CC_REG_FLD_GET(0, LCS_REG, ERROR_KCEICV_ZERO_CNT, regVal);		\
	}while(0)

/* check KCP error bit in LCS register */
#define CC_BSV_IS_OTP_KCP_ERROR(hwBaseAddress, regVal) 						\
	do { 											\
		SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, LCS_REG), regVal);		\
		regVal = CC_REG_FLD_GET(0, LCS_REG, ERROR_PROV_ZERO_CNT, regVal);		\
	}while(0)

/* check KCE error bit in LCS register */
#define CC_BSV_IS_OTP_KCE_ERROR(hwBaseAddress, regVal) 						\
	do { 											\
		SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, LCS_REG), regVal);		\
		regVal = CC_REG_FLD_GET(0, LCS_REG, ERROR_KCE_ZERO_CNT, regVal);		\
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

/* check OEM RMA flag bit in OTP memory */
#define CC_BSV_IS_OEM_RMA_FLAG_SET(hwBaseAddress, isOemRmaFlag, error)							\
	do { 														\
		error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_OEM_FLAG_OFFSET, &isOemRmaFlag);			\
		isOemRmaFlag = CC_REG_FLD_GET2(0, OTP_OEM_FLAG, OEM_RMA_MODE, isOemRmaFlag);				\
	}while(0)

/* check ICV RMA flag bit in OTP memory */
#define CC_BSV_IS_ICV_RMA_FLAG_SET(hwBaseAddress, isIcvRmaFlag, error)							\
	do { 														\
		error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_OEM_FLAG_OFFSET, &isIcvRmaFlag);			\
		isIcvRmaFlag = CC_REG_FLD_GET2(0, OTP_OEM_FLAG, ICV_RMA_MODE, isIcvRmaFlag);				\
	}while(0)

/* poll on the crypto busy till it is = 0 */
#define CC_BSV_WAIT_ON_CRYPTO_BUSY()								\
	do {											\
		uint32_t regVal=1;								\
		do {										\
			SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, CRYPTO_BUSY), regVal);	\
                }while( regVal );								\
        }while(0)

/* check KPICV "Not In Use" bit in OTP memory */
#define CC_BSV_IS_KPICV_IN_USE(hwBaseAddress, isKpicvInUse, error)							\
	do { 													  	\
		error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_MANUFACTURE_FLAG_OFFSET, &isKpicvInUse);		\
		isKpicvInUse = 0x1UL ^ (CC_REG_FLD_GET2(0, OTP_MANUFACTURE_FLAG, KPICV_NOT_IN_USE, isKpicvInUse));	\
	}while(0)

/* check KCEICV "Not In Use" bit in OTP memory */
#define CC_BSV_IS_KCEICV_IN_USE(hwBaseAddress, isKceicvInUse, error)							\
	do { 													  	\
		error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_MANUFACTURE_FLAG_OFFSET, &isKceicvInUse);		\
		isKceicvInUse = 0x1UL ^ (CC_REG_FLD_GET2(0, OTP_MANUFACTURE_FLAG, KCEICV_NOT_IN_USE, isKceicvInUse));	\
	}while(0)

/* check KCP "Not In Use" bit in OTP memory */
#define CC_BSV_IS_KCP_IN_USE(hwBaseAddress, isKcpInUse, error)						\
	do { 												\
		error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_OEM_FLAG_OFFSET, &isKcpInUse);		\
		isKcpInUse = 0x1UL ^ (CC_REG_FLD_GET2(0, OTP_OEM_FLAG, KCP_NOT_IN_USE, isKcpInUse));	\
	}while(0)

/* check KCE "Not In Use" bit in OTP memory */
#define CC_BSV_IS_KCE_IN_USE(hwBaseAddress, isKceInUse, error)						\
	do { 												\
		error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_OEM_FLAG_OFFSET, &isKceInUse);		\
		isKceInUse = 0x1UL ^ (CC_REG_FLD_GET2(0, OTP_OEM_FLAG, KCE_NOT_IN_USE, isKceInUse));	\
	}while(0)



/* ********************** Definitions ******************************* */

/* HW clocks */
#define CC_BSV_CLOCK_ENABLE		0x1UL
#define CC_BSV_CLOCK_DISABLE		0x0UL

/* Peripheral ID registers values */
#define CC_BSV_PID_0_VAL		0x000000C0UL
#define CC_BSV_PID_1_VAL		0x000000B0UL
#define CC_BSV_PID_2_VAL		0x0000000BUL
#define CC_BSV_PID_3_VAL		0x00000000UL
#define CC_BSV_PID_4_VAL		0x00000004UL
#define CC_BSV_PID_SIZE_WORDS		5

#define CC_BSV_PID_2_1_VAL		0x0000002BUL

/* Component ID registers values */
#define CC_BSV_CID_0_VAL		0x0DUL
#define CC_BSV_CID_1_VAL		0xF0UL
#define CC_BSV_CID_2_VAL		0x05UL
#define CC_BSV_CID_3_VAL		0xB1UL
#define CC_BSV_CID_SIZE_WORDS		4

/* Secret Key */
#define CC_BSV_SK_ERASER_VAL			0xFFFFFFFFUL
#define CC_BSV_256B_SK_INTEGRITY_ERASER_VAL	0xFFUL
#define CC_BSV_128B_SK_INTEGRITY_ERASER_VAL	0x7FUL

//********************************************************************************************/
//_SBROM_BSV_ERROR_H
/*! Defines BSV base error. */
#define CC_BSV_BASE_ERROR                    		0x0B000000
/*! Defines BSV cryptographic base error. */
#define CC_BSV_CRYPTO_ERROR                    		0x0C000000

/*! Illegal input parameter error. */
#define CC_BSV_ILLEGAL_INPUT_PARAM_ERR           	(CC_BSV_BASE_ERROR + 0x00000001)
/*! Illegal HUK value error. */
#define CC_BSV_ILLEGAL_HUK_VALUE_ERR			(CC_BSV_BASE_ERROR + 0x00000002)
/*! Illegal Kcp value error. */
#define CC_BSV_ILLEGAL_KCP_VALUE_ERR			(CC_BSV_BASE_ERROR + 0x00000003)
/*! Illegal Kce value error. */
#define CC_BSV_ILLEGAL_KCE_VALUE_ERR			(CC_BSV_BASE_ERROR + 0x00000004)
/*! Illegal Kpicv value error. */
#define CC_BSV_ILLEGAL_KPICV_VALUE_ERR			(CC_BSV_BASE_ERROR + 0x00000005)
/*! Illegal Kceicv value error. */
#define CC_BSV_ILLEGAL_KCEICV_VALUE_ERR			(CC_BSV_BASE_ERROR + 0x00000006)
/*! Hash boot key not programmed in the OTP error. */
#define CC_BSV_HASH_NOT_PROGRAMMED_ERR			(CC_BSV_BASE_ERROR + 0x00000007)
/*! Illegal Hash boot key zero count in the OTP error. */
#define CC_BSV_HBK_ZERO_COUNT_ERR			(CC_BSV_BASE_ERROR + 0x00000008)
/*! Illegal LCS error. */
#define CC_BSV_ILLEGAL_LCS_ERR				(CC_BSV_BASE_ERROR + 0x00000009)
/*! OTP write compare failure error. */
#define CC_BSV_OTP_WRITE_CMP_FAIL_ERR			(CC_BSV_BASE_ERROR + 0x0000000A)
/*! Erase key in OTP failed error. */
#define CC_BSV_ERASE_KEY_FAILED_ERR			(CC_BSV_BASE_ERROR + 0x0000000B)
/*! Illegal PIDR error. */
#define CC_BSV_ILLEGAL_PIDR_ERR				(CC_BSV_BASE_ERROR + 0x0000000C)
/*! Illegal CIDR error. */
#define CC_BSV_ILLEGAL_CIDR_ERR				(CC_BSV_BASE_ERROR + 0x0000000D)
/*! Device failed to move to fatal error state. */
#define CC_BSV_FAILED_TO_SET_FATAL_ERR			(CC_BSV_BASE_ERROR + 0x0000000E)
/*! Failed to set RMA LCS error. */
#define CC_BSV_FAILED_TO_SET_RMA_ERR			(CC_BSV_BASE_ERROR + 0x0000000F)
/*! Illegal RMA indication error. */
#define CC_BSV_ILLEGAL_RMA_INDICATION_ERR		(CC_BSV_BASE_ERROR + 0x00000010)
/*! BSV version is not initialized error. */
#define CC_BSV_VER_IS_NOT_INITIALIZED_ERR		(CC_BSV_BASE_ERROR + 0x00000011)
/*! APB secure mode is locked error. */
#define CC_BSV_APB_SECURE_IS_LOCKED_ERR			(CC_BSV_BASE_ERROR + 0x00000012)
/*! APB privilege mode is locked error. */
#define CC_BSV_APB_PRIVILEG_IS_LOCKED_ERR		(CC_BSV_BASE_ERROR + 0x00000013)
/*! Illegal operation error. */
#define CC_BSV_ILLEGAL_OPERATION_ERR			(CC_BSV_BASE_ERROR + 0x00000014)
/*! Illegal asset size error. */
#define CC_BSV_ILLEGAL_ASSET_SIZE_ERR			(CC_BSV_BASE_ERROR + 0x00000015)
/*! Illegal asset value error. */
#define CC_BSV_ILLEGAL_ASSET_VAL_ERR			(CC_BSV_BASE_ERROR + 0x00000016)
/*! Kpicv is locked error. */
#define CC_BSV_KPICV_IS_LOCKED_ERR			    (CC_BSV_BASE_ERROR + 0x00000017)
/*! Illegal SW version error. */
#define CC_BSV_ILLEGAL_SW_VERSION_ERR			(CC_BSV_BASE_ERROR + 0x00000018)
/*! AO write operation error. */
#define CC_BSV_AO_WRITE_FAILED_ERR			    (CC_BSV_BASE_ERROR + 0x00000019)
/*! Device is locked in fatal error state. */
#define CC_BSV_FATAL_ERR_IS_LOCKED_ERR          (CC_BSV_BASE_ERROR + 0x0000001A)

/*! Illegal data in pointer error. */
#define CC_BSV_INVALID_DATA_IN_POINTER_ERROR		(CC_BSV_CRYPTO_ERROR + 0x00000001)
/*! Illegal data out pointer error. */
#define CC_BSV_INVALID_DATA_OUT_POINTER_ERROR		(CC_BSV_CRYPTO_ERROR + 0x00000002)
/*! Illegal data size error. */
#define CC_BSV_INVALID_DATA_SIZE_ERROR			(CC_BSV_CRYPTO_ERROR + 0x00000003)
/*! Illegal key type error. */
#define	CC_BSV_INVALID_KEY_TYPE_ERROR			(CC_BSV_CRYPTO_ERROR + 0x00000004)
/*! Illegal key size error. */
#define CC_BSV_INVALID_KEY_SIZE_ERROR			(CC_BSV_CRYPTO_ERROR + 0x00000005)
/*! Illegal KDF label error. */
#define CC_BSV_ILLEGAL_KDF_LABEL_ERROR			(CC_BSV_CRYPTO_ERROR + 0x00000006)
/*! Illegal KDF context error. */
#define CC_BSV_ILLEGAL_KDF_CONTEXT_ERROR		(CC_BSV_CRYPTO_ERROR + 0x00000007)
/*! Invalid CCM key error. */
#define	CC_BSV_CCM_INVALID_KEY_ERROR			(CC_BSV_CRYPTO_ERROR + 0x00000008)
/*! Invalid CCM Nonce error. */
#define	CC_BSV_CCM_INVALID_NONCE_ERROR			(CC_BSV_CRYPTO_ERROR + 0x00000009)
/*! Invalid CCM associated data error. */
#define	CC_BSV_CCM_INVALID_ASSOC_DATA_ERROR		(CC_BSV_CRYPTO_ERROR + 0x0000000A)
/*! Invalid CCM text data error. */
#define	CC_BSV_CCM_INVALID_TEXT_DATA_ERROR		(CC_BSV_CRYPTO_ERROR + 0x0000000B)
/*! Invalid CCM-MAC buffer error. */
#define	CC_BSV_CCM_INVALID_MAC_BUF_ERROR		(CC_BSV_CRYPTO_ERROR + 0x0000000C)
/*! Output and input data are overlapping error. */
#define CC_BSV_CCM_DATA_OUT_DATA_IN_OVERLAP_ERROR	(CC_BSV_CRYPTO_ERROR + 0x0000000D)
/*! CCM-MAC comparison failed error. */
#define CC_BSV_CCM_MAC_INVALID_ERROR			(CC_BSV_CRYPTO_ERROR + 0x0000000E)
/*! Invalid CCM mode error. */
#define CC_BSV_CCM_INVALID_MODE_ERROR			(CC_BSV_CRYPTO_ERROR + 0x0000000F)
/*! Invalid out pointer error. */
#define CC_BSV_INVALID_OUT_POINTER_ERROR		(CC_BSV_CRYPTO_ERROR + 0x00000010)
/*! Illegal cryptographic mode error. */
#define CC_BSV_INVALID_CRYPTO_MODE_ERROR		(CC_BSV_CRYPTO_ERROR + 0x00000011)
/*! Illegal IV pointer error. */
#define CC_BSV_INVALID_IV_POINTER_ERROR			(CC_BSV_CRYPTO_ERROR + 0x00000012)
/*! Illegal result buffer pointer error. */
#define CC_BSV_INVALID_RESULT_BUFFER_POINTER_ERROR	(CC_BSV_CRYPTO_ERROR + 0x00000013)

//********************************************************************************************/
//_BSV_DEFS_H
/*! Defines the maximal hash boot key size in words. */
#define CC_BSV_MAX_HASH_SIZE_IN_WORDS		8
/*! Defines the maximal hash boot key size in bytes. */
#define CC_BSV_MAX_HASH_SIZE_IN_BYTES		(CC_BSV_MAX_HASH_SIZE_IN_WORDS*sizeof(uint32_t))
/*! Defines the maximal full-hash boot key size in words. */
#define CC_BSV_256B_HASH_SIZE_IN_WORDS		CC_BSV_MAX_HASH_SIZE_IN_WORDS
/*! Defines the maximal dual-hash boot key size in words. */
#define CC_BSV_128B_HASH_SIZE_IN_WORDS		CC_BSV_MAX_HASH_SIZE_IN_WORDS/2

/*! ICV Firmware minimal version maximal size. */
#define CC_BSV_MAX_HBK0_VERSION_COUNTER		64
/*! OEM Firmware minimal version maximal size. */
#define CC_BSV_MAX_HBK1_VERSION_COUNTER		96
/*! OEM Firmware minimal version maximal size (no ICV). */
#define CC_BSV_MAX_HBK_VERSION_COUNTER		160

/*! HUK status bit definition. */
#define DX_BSV_STAUS_HUK_ERR_BIT_SHIFT         0x0UL
/*! HUK status size bit definition. */
#define DX_BSV_STAUS_HUK_ERR_BIT_SIZE          0x1UL
/*! Kpicv status bit definition. */
#define DX_BSV_STAUS_KPICV_ERR_BIT_SHIFT         0x1UL
/*! Kpicv status size bit definition. */
#define DX_BSV_STAUS_KPICV_ERR_BIT_SIZE          0x1UL
/*! Kceicv status bit definition. */
#define DX_BSV_STAUS_KCEICV_ERR_BIT_SHIFT         0x2UL
/*! Kceicv status size bit definition. */
#define DX_BSV_STAUS_KCEICV_ERR_BIT_SIZE          0x1UL
/*! Kcp status bit definition. */
#define DX_BSV_STAUS_KCP_ERR_BIT_SHIFT         0x3UL
/*! Kcp status size bit definition. */
#define DX_BSV_STAUS_KCP_ERR_BIT_SIZE          0x1UL
/*! Kce status bit definition. */
#define DX_BSV_STAUS_KCE_ERR_BIT_SHIFT         0x4UL
/*! Kce status size bit definition. */
#define DX_BSV_STAUS_KCE_ERR_BIT_SIZE          0x1UL

/*! Definition for all ones word. */
#define CC_BSV_ALL_ONES_VALUE	0xffffffffUL
/*! Definition for number of bits in a 32bit word. */
#define CC_BSV_ALL_ONES_NUM_BITS 32

/* ********************** Macros ******************************* */
/*! This macro counts the number of zeroes in a 32bits word. */
#define CC_BSV_COUNT_ZEROES(regVal, regZero) 					\
	do {									\
		uint32_t val = regVal;                                         	\
		val = val - ((val >> 1) & 0x55555555);                   		\
		val = (val & 0x33333333) + ((val >> 2) & 0x33333333);    		\
		val = ((((val + (val >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24); 	\
		regZero	+= (32 - val);						\
	}while(0)


//********************************************************************************************/
//_CC_BITOPS_H_
/*! Defintion of number of 32bit maximum value. */
#define CC_32BIT_MAX_VALUE  (0xFFFFFFFFUL)

/*! Definition for bitmask */
#define BITMASK(mask_size) (((mask_size) < 32) ?	\
	((1UL << (mask_size)) - 1) : 0xFFFFFFFFUL)
/*! Definition for bitmask in a given offset. */
#define BITMASK_AT(mask_size, mask_offset) (BITMASK(mask_size) << (mask_offset))

/*! Definition for getting bits value from a word. */
#define BITFIELD_GET(word, bit_offset, bit_size) \
	(((word) >> (bit_offset)) & BITMASK(bit_size))
/*! Definition for setting bits value from a word. */
#define BITFIELD_SET(word, bit_offset, bit_size, new_val)   do {    \
	word = ((word) & ~BITMASK_AT(bit_size, bit_offset)) |	    \
		(((new_val) & BITMASK(bit_size)) << (bit_offset));  \
} while (0)

/*!Definition for is val aligned to "align" ("align" must be power of 2). */
#ifndef IS_ALIGNED
#define IS_ALIGNED(val, align)		\
	(((uintptr_t)(val) & ((align) - 1)) == 0)
#endif
/*!Definition swap endianity for 32 bits word. */
#define SWAP_ENDIAN(word)		\
	(((word) >> 24) | (((word) & 0x00FF0000) >> 8) | \
	(((word) & 0x0000FF00) << 8) | (((word) & 0x000000FF) << 24))

#ifdef BIG__ENDIAN
#define SWAP_TO_LE(word) SWAP_ENDIAN(word)
#define SWAP_TO_BE(word) word
#else
/*! Definition for swapping to LE. */
#define SWAP_TO_LE(word) word
/*! Definition for swapping to BE. */
#define SWAP_TO_BE(word) SWAP_ENDIAN(word)
#endif

/*!Align X to uint32_t size. */
#ifndef ALIGN_TO_4BYTES
#define ALIGN_TO_4BYTES(x)		(((unsigned long)(x) + (CC_32BIT_WORD_SIZE-1)) & ~(CC_32BIT_WORD_SIZE-1))
#endif



/*! Definition for is val a multiple of "mult" ("mult" must be power of 2). */
#define IS_MULT(val, mult)              \
	(((val) & ((mult) - 1)) == 0)

/*! Definition for is NULL address. */
#define IS_NULL_ADDR(adr)		\
	(!(adr))
//********************************************************************************************/
//UTIL_H
/* invers the bytes on a word- used for output from HASH */
#ifdef BIG__ENDIAN
#define UTIL_INVERSE_UINT32_BYTES( val )	(val)
#else
#define UTIL_INVERSE_UINT32_BYTES( val ) \
   ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24) )
#endif

/* invers the bytes on a word - used for input data for HASH */
#ifdef BIG__ENDIAN
#define UTIL_REVERT_UINT32_BYTES( val ) \
   ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24) )
#else
#define UTIL_REVERT_UINT32_BYTES( val )	(val)
#endif

/* the minimum and maximum macros */
#undef min
#define min( a , b ) ( ( (a) < (b) ) ? (a) : (b) )

#undef max
#define max( a , b ) ( ( (a) > (b) ) ? (a) : (b) )

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

#define CONVERT_BYTE_ARR_TO_WORD(inPtr, outWord, numOfBytes) \
do{\
	uint8_t index;\
	outWord = 0;\
	for (index = 0; index < numOfBytes; index ++){\
		outWord |= (*(inPtr + index)<<8*index);\
	}\
} while(0)
//********************************************************************************************/
//_CC_BITOPS_H_