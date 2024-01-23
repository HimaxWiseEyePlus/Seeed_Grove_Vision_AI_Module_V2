#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//********************************************************************************************/
//_CC_PAL_SB_PLAT_H
/*! DMA address types: 32 bits or 64 bits, according to platform. */
typedef uint32_t        CCDmaAddr_t;
/*! CryptocCell address types: 32 bits or 64 bits, according to platform. */
typedef uint32_t        CCAddr_t;

//********************************************************************************************/
//_BOOT_IMAGES_VERIFIER_DEF_H

/*! The maximal number of SW images per content certificate. */
#define CC_SB_MAX_NUM_OF_IMAGES 16

#ifdef CC_SB_X509_CERT_SUPPORTED
/*! The maximal size of an X.509 certificate in bytes.*/
#define CC_SB_MAX_CERT_SIZE_IN_BYTES    (0xB10)
#else
/*! The maximal size of an Arm proprietary certificate in bytes. */
#define CC_SB_MAX_CERT_SIZE_IN_BYTES    (0x700)
#endif
/*! The maximal size of a certificate in words.*/
#define CC_SB_MAX_CERT_SIZE_IN_WORDS    (CC_SB_MAX_CERT_SIZE_IN_BYTES/CC_32BIT_WORD_SIZE)

/*! The size of the Secure Debug workspace in bytes. This workspace is used
to store RSA parameters. For example, modulus and signature. */
#define CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES   (0x350)

//********************************************************************************************/
//_SECUREBOOT_ERROR_H
/*! Defines the boot images verifier base error = 0xF1000000. */
/*! Defines the error number space used for the Secure Boot different modules. */
#define CC_SECUREBOOT_BASE_ERROR                 0xF0000000

/*! Defines the secure boot's base layer's error number. */
#define CC_SECUREBOOT_LAYER_BASE_ERROR           0x01000000

/*! Defines the Secure boot's verifier layer's error prefix number. */
#define CC_SB_VERIFIER_LAYER_PREFIX      	1
/*! Defines the Secure boot's driver layer's error prefix number. */
#define CC_SB_DRV_LAYER_PREFIX    		2
/*! Defines the Secure boot's revocation layer's error prefix number. */
#define CC_SB_SW_REVOCATION_LAYER_PREFIX 	3
/*! Defines the Secure boot's HAL layer's error prefix number. */
#define CC_SB_HAL_LAYER_PREFIX                  6
/*! Defines the Secure boot's RSA layer's error prefix number. */
#define CC_SB_RSA_LAYER_PREFIX		        7
/*! Defines the Secure boot's certificate verifier layer's error prefix number. */
#define CC_SB_VERIFIER_CERT_LAYER_PREFIX	8
/*! Defines the Secure boot's X509 certificate layer's error prefix number. */
#define CC_SB_X509_CERT_LAYER_PREFIX		9


/*! Defines the boot images verifier base error = 0xF1000000. */
#define CC_BOOT_IMG_VERIFIER_BASE_ERROR          (CC_SECUREBOOT_BASE_ERROR + CC_SB_VERIFIER_LAYER_PREFIX*CC_SECUREBOOT_LAYER_BASE_ERROR)
/*! Defines the NVM base error = 0xF4000000. */
#define CC_SB_HAL_BASE_ERROR                     (CC_SECUREBOOT_BASE_ERROR + CC_SB_HAL_LAYER_PREFIX*CC_SECUREBOOT_LAYER_BASE_ERROR)
/*! Defines the RSA's base error = 0xF7000000. */
#define CC_SB_RSA_BASE_ERROR                     (CC_SECUREBOOT_BASE_ERROR + CC_SB_RSA_LAYER_PREFIX*CC_SECUREBOOT_LAYER_BASE_ERROR)

/*! Defines the boot images verifier certificates base error = 0xF8000000. */
#define CC_BOOT_IMG_VERIFIER_CERT_BASE_ERROR     (CC_SECUREBOOT_BASE_ERROR + CC_SB_VERIFIER_CERT_LAYER_PREFIX*CC_SECUREBOOT_LAYER_BASE_ERROR)

/*! Defines the X.509's base error = 0xF9000000. */
#define CC_SB_X509_CERT_BASE_ERROR     	         (CC_SECUREBOOT_BASE_ERROR + CC_SB_X509_CERT_LAYER_PREFIX*CC_SECUREBOOT_LAYER_BASE_ERROR)

/*! Defines the cryptographic driver base error = 0xF2000000. */
#define CC_SB_DRV_BASE_ERROR     		 (CC_SECUREBOOT_BASE_ERROR + CC_SB_DRV_LAYER_PREFIX*CC_SECUREBOOT_LAYER_BASE_ERROR)

/*! Defines a HAL fatal error. */
#define CC_SB_HAL_FATAL_ERROR_ERR       	 (CC_SB_HAL_BASE_ERROR + 0x00000001)
/*! Illegal input error. */
#define	CC_SB_DRV_ILLEGAL_INPUT_ERR		 (CC_SB_DRV_BASE_ERROR + 0x00000001)
/*! Illegal key error. */
#define	CC_SB_DRV_ILLEGAL_KEY_ERR		 (CC_SB_DRV_BASE_ERROR + 0x00000002)
/*! Illegal size error. */
#define	CC_SB_DRV_ILLEGAL_SIZE_ERR		 (CC_SB_DRV_BASE_ERROR + 0x00000003)



/************************ Defines ******************************/
/*! Defines the error number space used for the Secure Boot different modules. */
#define CC_SECUREBOOT_BASE_ERROR                 0xF0000000

/*! Defines the secure boot's base layer's error number. */
#define CC_SECUREBOOT_LAYER_BASE_ERROR           0x01000000

/*! Defines the Secure boot's verifier layer's error prefix number. */
#define CC_SB_VERIFIER_LAYER_PREFIX      	1
/*! Defines the Secure boot's driver layer's error prefix number. */
#define CC_SB_DRV_LAYER_PREFIX    		2
/*! Defines the Secure boot's revocation layer's error prefix number. */
#define CC_SB_SW_REVOCATION_LAYER_PREFIX 	3
/*! Defines the Secure boot's HAL layer's error prefix number. */
#define CC_SB_HAL_LAYER_PREFIX                  6
/*! Defines the Secure boot's RSA layer's error prefix number. */
#define CC_SB_RSA_LAYER_PREFIX		        7
/*! Defines the Secure boot's certificate verifier layer's error prefix number. */
#define CC_SB_VERIFIER_CERT_LAYER_PREFIX	8
/*! Defines the Secure boot's X509 certificate layer's error prefix number. */
#define CC_SB_X509_CERT_LAYER_PREFIX		9
//********************************************************************************************/
//__DX_ID_REGS_H__
#define DX_PERIPHERAL_ID_4_REG_OFFSET 	0x0FD0UL 
#define DX_PERIPHERAL_ID_4_VALUE_BIT_SHIFT 	0x0UL
#define DX_PERIPHERAL_ID_4_VALUE_BIT_SIZE 	0x4UL
#define DX_PIDRESERVED0_REG_OFFSET 	0x0FD4UL 
#define DX_PIDRESERVED1_REG_OFFSET 	0x0FD8UL 
#define DX_PIDRESERVED2_REG_OFFSET 	0x0FDCUL 
#define DX_PERIPHERAL_ID_0_REG_OFFSET 	0x0FE0UL 
#define DX_PERIPHERAL_ID_0_VALUE_BIT_SHIFT 	0x0UL
#define DX_PERIPHERAL_ID_0_VALUE_BIT_SIZE 	0x8UL
#define DX_PERIPHERAL_ID_1_REG_OFFSET 	0x0FE4UL 
#define DX_PERIPHERAL_ID_1_PART_1_BIT_SHIFT 	0x0UL
#define DX_PERIPHERAL_ID_1_PART_1_BIT_SIZE 	0x4UL
#define DX_PERIPHERAL_ID_1_DES_0_JEP106_BIT_SHIFT 	0x4UL
#define DX_PERIPHERAL_ID_1_DES_0_JEP106_BIT_SIZE 	0x4UL
#define DX_PERIPHERAL_ID_2_REG_OFFSET 	0x0FE8UL 
#define DX_PERIPHERAL_ID_2_DES_1_JEP106_BIT_SHIFT 	0x0UL
#define DX_PERIPHERAL_ID_2_DES_1_JEP106_BIT_SIZE 	0x3UL
#define DX_PERIPHERAL_ID_2_JEDEC_BIT_SHIFT 	0x3UL
#define DX_PERIPHERAL_ID_2_JEDEC_BIT_SIZE 	0x1UL
#define DX_PERIPHERAL_ID_2_REVISION_BIT_SHIFT 	0x4UL
#define DX_PERIPHERAL_ID_2_REVISION_BIT_SIZE 	0x4UL
#define DX_PERIPHERAL_ID_3_REG_OFFSET 	0x0FECUL 
#define DX_PERIPHERAL_ID_3_CMOD_BIT_SHIFT 	0x0UL
#define DX_PERIPHERAL_ID_3_CMOD_BIT_SIZE 	0x4UL
#define DX_PERIPHERAL_ID_3_REVAND_BIT_SHIFT 	0x4UL
#define DX_PERIPHERAL_ID_3_REVAND_BIT_SIZE 	0x4UL
#define DX_COMPONENT_ID_0_REG_OFFSET 	0x0FF0UL 
#define DX_COMPONENT_ID_0_VALUE_BIT_SHIFT 	0x0UL
#define DX_COMPONENT_ID_0_VALUE_BIT_SIZE 	0x8UL
#define DX_COMPONENT_ID_1_REG_OFFSET 	0x0FF4UL 
#define DX_COMPONENT_ID_1_PRMBL_1_BIT_SHIFT 	0x0UL
#define DX_COMPONENT_ID_1_PRMBL_1_BIT_SIZE 	0x4UL
#define DX_COMPONENT_ID_1_CLASS_BIT_SHIFT 	0x4UL
#define DX_COMPONENT_ID_1_CLASS_BIT_SIZE 	0x4UL
#define DX_COMPONENT_ID_2_REG_OFFSET 	0x0FF8UL 
#define DX_COMPONENT_ID_2_VALUE_BIT_SHIFT 	0x0UL
#define DX_COMPONENT_ID_2_VALUE_BIT_SIZE 	0x8UL
#define DX_COMPONENT_ID_3_REG_OFFSET 	0x0FFCUL 
#define DX_COMPONENT_ID_3_VALUE_BIT_SHIFT 	0x0UL
#define DX_COMPONENT_ID_3_VALUE_BIT_SIZE 	0x8UL


//********************************************************************************************/
//_BSV_CRYPTO_DEFS_H
/*! AES CMAC result size in words. */
#define	CC_BSV_CMAC_RESULT_SIZE_IN_WORDS 	4  /* 128b */
/*! AES CMAC result size in Bytes. */
#define	CC_BSV_CMAC_RESULT_SIZE_IN_BYTES 	16 /* 128b */
/*! AES CCM 128bit key size in Bytes. */
#define CC_BSV_CCM_KEY_SIZE_BYTES           	16
/*! AES CCM 128bit key size in words. */
#define CC_BSV_CCM_KEY_SIZE_WORDS           	4
/*! AES CCM NONCE size in Bytes. */
#define CC_BSV_CCM_NONCE_SIZE_BYTES		12


/*! Definitions for AES key types. */
typedef enum  {
        /*! Root key (HUK).*/
        CC_BSV_HUK_KEY = 0,
        /*! RTL key (Krtl).*/
        CC_BSV_RTL_KEY = 1,
        /*! OEM Provision key (Kcp).*/
        CC_BSV_PROV_KEY = 2,
        /*! OEM Code encryption key (Kce).*/
        CC_BSV_CE_KEY = 3,
        /*! ICV Provision key (Kpicv).*/
        CC_BSV_ICV_PROV_KEY = 4,
        /*! ICV Code encryption key (Kceicv).*/
        CC_BSV_ICV_CE_KEY = 5,
        /*! User's key.*/
        CC_BSV_USER_KEY = 6,
        /*! Reserved.*/
        CC_BSV_END_OF_KEY_TYPE = 0x7FFFFFFF
}CCBsvKeyType_t;


/*! CMAC result buffer. */
typedef uint32_t CCBsvCmacResult_t[CC_BSV_CMAC_RESULT_SIZE_IN_WORDS];

/*! AES_CCM key buffer definition.*/
typedef uint32_t CCBsvCcmKey_t[CC_BSV_CCM_KEY_SIZE_WORDS];

/*! AES_CCM nonce buffer definition.*/
typedef uint8_t CCBsvCcmNonce_t[CC_BSV_CCM_NONCE_SIZE_BYTES];

/*! AES_CCM MAC buffer definition.*/
typedef uint8_t CCBsvCcmMacRes_t[CC_BSV_CMAC_RESULT_SIZE_IN_BYTES];

//********************************************************************************************/
//_BOOT_IMAGES_VERIFIER_ERROR_H
#define CC_SB_MIN_WORKSPACE_SIZE_IN_BYTES   (CC_SB_MAX_CERT_SIZE_IN_BYTES + CC_MAX(CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES, CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES))
/*! Defines error code for invalid input parameters. */
#define CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM                            (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000001)
/*! Defines error code for invalid OTP version. */
#define CC_BOOT_IMG_VERIFIER_OTP_VERSION_FAILURE                        (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000002)
/*! Defines error code for illegal certificate's magic number. */
#define CC_BOOT_IMG_VERIFIER_CERT_MAGIC_NUM_INCORRECT                   (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000003)
/*! Defines error code for illegal certificate version. */
#define CC_BOOT_IMG_VERIFIER_CERT_VERSION_NUM_INCORRECT                 (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000004)
/*! Defines error code for illegal certificate SW version, that is smaller than the version stored in the OTP. */
#define CC_BOOT_IMG_VERIFIER_SW_VER_SMALLER_THAN_MIN_VER                (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000005)
/*! Defines error code for public key verification compared to the OTP value failed. */
#define CC_BOOT_IMG_VERIFIER_PUB_KEY_HASH_VALIDATION_FAILURE            (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000006)
/*! Defines error code for certificate's RSA signature verification failure. */
#define CC_BOOT_IMG_VERIFIER_RSA_SIG_VERIFICATION_FAILED                (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000007)
/*! Defines error code for workspace buffer given to the API is too small. */
#define CC_BOOT_IMG_VERIFIER_WORKSPACE_SIZE_TOO_SMALL                   (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000008)
/*! Defines error code for SW image hash verification failure. */
#define CC_BOOT_IMG_VERIFIER_SW_COMP_FAILED_VERIFICATION                (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000009)
/*! Defines error code for illegal SW version or ID of SW version. */
#define CC_BOOT_IMG_VERIFIER_CERT_SW_VER_ILLEGAL		        (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000000D)
/*! Defines error code for illegal number of SW components (zero). */
#define CC_BOOT_IMG_VERIFIER_SW_COMP_SIZE_IS_NULL                       (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000011)
/*! Defines error code for hash of public key is not burned yet. */
#define CC_BOOT_IMG_VERIFIER_PUBLIC_KEY_HASH_EMPTY                      (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000014)
/*! Defines error code for illegal lifecycle state (LCS) for operation.*/
#define CC_BOOT_IMG_VERIFIER_ILLEGAL_LCS_FOR_OPERATION_ERR   		(CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000015)
/*! Defines error code for hash of public key is already programmed.*/
#define CC_BOOT_IMG_VERIFIER_PUB_KEY_ALREADY_PROGRAMMED_ERR		(CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000016)
/*! Defines error code for OTP write failure.*/
#define CC_BOOT_IMG_VERIFIER_OTP_WRITE_FAIL_ERR			  	(CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000017)
/*! Defines error code for incorrect certificate type.*/
#define CC_BOOT_IMG_VERIFIER_INCORRECT_CERT_TYPE			(CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000018)
/*! Defines error code for illegal Hash boot key index.*/
#define CC_BOOT_IMG_VERIFIER_ILLEGAL_HBK_IDX				(CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000019)
/*! Defines error code for hash boot key of ICV is not programmed.*/
#define CC_BOOT_IMG_VERIFIER_PUB_KEY1_NOT_PROGRAMMED_ERR		(CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000001A)
/*! Defines error code for illegal certificate version value.*/
#define CC_BOOT_IMG_VERIFIER_CERT_VER_VAL_ILLEGAL		        (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000001C)
/*! Defines error code for illegal certificate decoding value.*/
#define CC_BOOT_IMG_VERIFIER_CERT_DECODING_ILLEGAL		        (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000001D)
/*! Defines error code for illegal Kce in RMA LCS.*/
#define CC_BOOT_IMG_VERIFIER_ILLEGAL_KCE_IN_RMA_STATE		        (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000001E)
/*! Defines error code for illegal SOC ID value.*/
#define CC_BOOT_IMG_VERIFIER_ILLEGAL_SOC_ID_VALUE		        (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000001F)
/*! Defines error code for illegal number of SW images per content certificate. */
#define CC_BOOT_IMG_VERIFIER_ILLEGAL_NUM_OF_IMAGES		        (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000020)
/*! Defines error code for no need to verify hashed public key. */
#define CC_BOOT_IMG_VERIFIER_SKIP_PUBLIC_KEY_VERIFY                      (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000014)
/*! Defines error code for CC312 DMA move driver. */
#define CC_BOOT_IMG_VERIFIER_SEC_ATTRIBUTE_ERROR                     (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000002C)
//********************************************************************************************/
//CC_PAL_TYPES_H
#if 0
/*! Boolean types.*/
typedef enum {
    /*! Boolean false definition.*/
    CC_FALSE = 0,
    /*! Boolean true definition.*/
    CC_TRUE = 1
} CCBool;
#endif
/*! Success definition. */
#define CC_SUCCESS              0UL
/*! Failure definition. */
#define CC_FAIL         1UL

/*! Success (OK) definition. */
#define CC_OK   0

/*! Handles unused parameters in the code, to avoid compilation warnings.  */
#define CC_UNUSED_PARAM(prm)  ((void)prm)

/*! The maximal uint32 value.*/
#define CC_MAX_UINT32_VAL   (0xFFFFFFFF)

#if 0
/* Minimal and Maximal macros */
#ifdef  min
/*! Definition for minimal calculation. */
#define CC_MIN(a,b) min( a , b )
#else
/*! Definition for minimal calculation. */
#define CC_MIN( a , b ) ( ( (a) < (b) ) ? (a) : (b) )
#endif

#ifdef max
/*! Definition for maximal calculation. */
#define CC_MAX(a,b) max( a , b )
#else
/*! Definition for maximal calculation.. */
#define CC_MAX( a , b ) ( ( (a) > (b) ) ? (a) : (b) )
#endif
#endif
/*! This macro calculates the number of full bytes from bits, where seven bits 
are one byte. */
#define CALC_FULL_BYTES(numBits)        ((numBits)/CC_BITS_IN_BYTE + (((numBits) & (CC_BITS_IN_BYTE-1)) > 0))
/*! This macro calculates the number of full 32-bit words from bits, where 
31 bits are one word. */
#define CALC_FULL_32BIT_WORDS(numBits)      ((numBits)/CC_BITS_IN_32BIT_WORD +  (((numBits) & (CC_BITS_IN_32BIT_WORD-1)) > 0))
/*! This macro calculates the number of full 32-bit words from bytes, where 
three bytes are one word. */
#define CALC_32BIT_WORDS_FROM_BYTES(sizeBytes)  ((sizeBytes)/CC_32BIT_WORD_SIZE + (((sizeBytes) & (CC_32BIT_WORD_SIZE-1)) > 0))
/*! This macro calculates the number of full 32-bit words from 64-bits 
dwords. */
#define CALC_32BIT_WORDS_FROM_64BIT_DWORD(sizeWords)  (sizeWords * CC_32BIT_WORD_IN_64BIT_DWORD)
/*! This macro rounds up bits to 32-bit words. */
#define ROUNDUP_BITS_TO_32BIT_WORD(numBits)     (CALC_FULL_32BIT_WORDS(numBits) * CC_BITS_IN_32BIT_WORD)
/*! This macro rounds up bits to bytes. */
#define ROUNDUP_BITS_TO_BYTES(numBits)      (CALC_FULL_BYTES(numBits) * CC_BITS_IN_BYTE)
/*! This macro rounds up bytes to 32-bit words. */
#define ROUNDUP_BYTES_TO_32BIT_WORD(sizeBytes)  (CALC_32BIT_WORDS_FROM_BYTES(sizeBytes) * CC_32BIT_WORD_SIZE)
/*! Definition of 1 KB in bytes. */
#define CC_1K_SIZE_IN_BYTES 1024
/*! Definition of number of bits in a byte. */
#define CC_BITS_IN_BYTE     8
/*! Definition of number of bits in a 32-bits word. */
#define CC_BITS_IN_32BIT_WORD   32
/*! Definition of number of bytes in a 32-bits word. */
#define CC_32BIT_WORD_SIZE  4
/*! Definition of number of 32-bits words in a 64-bits dword. */
#define CC_32BIT_WORD_IN_64BIT_DWORD 2

//********************************************************************************************/
//_CC_CRYPTO_BOOT_DEFS_H


#define CC_SB_MAX_SIZE_NONCE_BYTES		(2*sizeof(uint32_t))

/*! HASH boot key definition. */
typedef enum {
	CC_SB_HASH_BOOT_KEY_0_128B 	= 0,		/*!< 128-bit truncated SHA256 digest of public key 0. */
	CC_SB_HASH_BOOT_KEY_1_128B	= 1, 		/*!< 128-bit truncated SHA256 digest of public key 1. */
	CC_SB_HASH_BOOT_KEY_256B	= 2, 		/*!< 256-bit SHA256 digest of public key. */
	CC_SB_HASH_BOOT_NOT_USED	= 0xF,
	CC_SB_HASH_MAX_NUM 	        = 0x7FFFFFFF,	/*!\internal use external 128-bit truncated SHA256 digest */
}CCSbPubKeyIndexType_t;


/*! SW image code encryption type definition. */
typedef enum {
	CC_SB_NO_IMAGE_ENCRYPTION 	= 0,		/*!< Plain SW image. */
	CC_SB_ICV_CODE_ENCRYPTION	= 1, 		/*!< use Kceicv for cipher SW image. */
	CC_SB_OEM_CODE_ENCRYPTION	= 2, 		/*!< use Kce for cipher SW image. */
	CC_SB_ICV_CODE_RE_ENCRYPTION	= 5,
	CC_SB_OEM_CODE_RE_ENCRYPTION	= 6,
	CC_SB_CODE_ENCRYPTION_MAX_NUM 	= 0x7FFFFFFF,	/*!\internal NA */
}CCswCodeEncType_t;

/*! SW image load and verify scheme. */
typedef enum {
	CC_SB_LOAD_AND_VERIFY 		= 0,		/*!< Load & Verify from flash to memory. */
	CC_SB_VERIFY_ONLY_IN_FLASH	= 1, 		/*!< Verify only in flash. */
	CC_SB_VERIFY_ONLY_IN_MEM	= 2, 		/*!< Verify only in memory. */
	CC_SB_LOAD_ONLY				= 3,		/*!< Load only from flash to memory. */
	CC_SB_LOAD_VERIFY_MAX_NUM 	= 0x7FFFFFFF,	/*!\internal NA */
}CCswLoadVerifyScheme_t;

/*! SW image cryptographic type. */
typedef enum {
	CC_SB_HASH_ON_DECRYPTED_IMAGE 	= 0,		/*!< AES to HASH. */
	CC_SB_HASH_ON_ENCRYPTED_IMAGE	= 1, 		/*!< AES and HASH. */
	CC_SB_CRYPTO_TYPE_MAX_NUM 	= 0x7FFFFFFF,	/*!\internal NA */
}CCswCryptoType_t;

/*! Table nonce used in composing IV for SW-component decryption. */
typedef uint8_t CCSbNonce_t[CC_SB_MAX_SIZE_NONCE_BYTES];

/*! SW components data.*/
typedef struct {
	/*! Num of SW components. */
	uint32_t  numOfSwComps;

	/*! SW image code encryption type. */
	CCswCodeEncType_t swCodeEncType;

	/*! SW image load & verify scheme. */
	CCswLoadVerifyScheme_t swLoadVerifyScheme;

	/*! SW image crypto type. */
	CCswCryptoType_t swCryptoType;

	/*! Nonce. */
	CCSbNonce_t nonce;

	/*! Pointer to start of sw components data. */
	uint8_t *pSwCompsData;

}CCSbCertParserSwCompsInfo_t;

/*! SW version */
typedef struct {
	CCSbPubKeyIndexType_t keyIndex;		/*!< Enumeration defining the key hash to retrieve: 128-bit HBK0, 128-bit HBK1, or 256-bit HBK. */
	uint32_t swVersion;			/*!< Sw version.*/
}CCSbSwVersion_t;
//********************************************************************************************/
//_CC_SEC_DEFS_H

/*! The hashblock size in words. */
#define HASH_BLOCK_SIZE_IN_WORDS             	16

/*! SHA256 result size in words. */
#define HASH_RESULT_SIZE_IN_WORDS            	8
/*! SHA256 result size in Bytes. */
#define HASH_RESULT_SIZE_IN_BYTES            	32

/*! Defines the hash result array. */
typedef uint32_t CCHashResult_t[HASH_RESULT_SIZE_IN_WORDS];

/*! Definition for converting pointer to address. */
#define CONVERT_TO_ADDR(ptr) 	(unsigned long)ptr

//********************************************************************************************/
//_CC_PAL_TYPES_PLAT_H

/*! Type definition for virtual address. */
typedef uintptr_t	    CCVirtAddr_t;
/*! Type Definition for boolean variable. */
typedef uint32_t            CCBool_t;
/*! Type definition for return status. */
typedef uint32_t            CCStatus;

/*! Type definition for error return. */
#define CCError_t           CCStatus
/*! Defines inifinite value, used to define unlimited time frame. */
#define CC_INFINITE         0xFFFFFFFF

/*! Type definition for C export. */
#define CEXPORT_C
/*! Type definition for C import. */
#define CIMPORT_C

//_SECURE_BOOT_DEFS_H

/*! Input or output structure for the Secure Boot verification API. */
typedef struct{
    /*! The NV counter saved in OTP. */
	uint32_t otpVersion;
    /*! The key hash to retrieve: The 128-bit Hbk0, the 128-bit Hbk1, or the 
	256-bit Hbk. */
	CCSbPubKeyIndexType_t keyIndex;
    /*! The value of the SW version for the certificate-chain. */
	uint32_t activeMinSwVersionVal;
    /*! In: The hash of the public key (N||Np), to compare to the public key 
	stored in the certificate. Out: The hash of the public key (N||Np) stored 
	in the certificate, to be used for verification of the public key of the 
	next certificate in the chain. */
	CCHashResult_t pubKeyHash;
    /*! Internal flag for the initialization indication. */
	uint32_t initDataFlag;
}CCSbCertInfo_t;



/*! The size of the data of the SW-image certificate. */
#define SW_REC_SIGNED_DATA_SIZE_IN_BYTES            44  // HS(8W) + load_adddr(1) + maxSize(1) + isCodeEncUsed(1)
/*! The size of the additional-data of the SW-image certificate in bytes. */
#define SW_REC_NONE_SIGNED_DATA_SIZE_IN_BYTES       8   // storage_addr(1) + ActualSize(1)
/*! The size of the additional-data of the SW-image certificate in words. */
#define SW_REC_NONE_SIGNED_DATA_SIZE_IN_WORDS       SW_REC_NONE_SIGNED_DATA_SIZE_IN_BYTES/CC_32BIT_WORD_SIZE
/*! Indication whether or not to load the SW image to memory. */
#define CC_SW_COMP_NO_MEM_LOAD_INDICATION       0xFFFFFFFFUL

//********************************************************************************************/
//_SECURE_BOOT_GEN_DEFS_H

/*! The maximal size of the additional-data of the Secure Boot in bytes. */
#define CC_SB_MAX_SIZE_ADDITIONAL_DATA_BYTES    128

/*! Definition of public key hash array. */
typedef uint32_t CCSbCertPubKeyHash_t[HASH_RESULT_SIZE_IN_WORDS];
/*! Definition of <em>SoC_ID</em> array. */
typedef uint32_t CCSbCertSocId_t[HASH_RESULT_SIZE_IN_WORDS];


/********* Function pointer definitions ***********/

/*! @brief Typedef of the pointer to the Flash read function that you  
must implement.

The Flash read function is called to read the certificates and SW modules from 
flash memory.

  @note It is your responsibility to verify that this function does not copy 
  data from restricted memory regions.
 */
typedef uint32_t (*CCSbFlashReadFunc) (
                     /*! [in] The address for reading from flash memory. */
					 CCAddr_t flashAddress,
                     /*! [out] A pointer to the RAM destination address to 
					 write the data to. */
					 uint8_t *memDst,
                     /*! [in] The size to read in bytes. */
					 uint32_t sizeToRead,
                     /*! [in] For partner use. */
					 void* context
                     );


/*! @brief Typedef of the pointer to the Flash write function that you must 
implement.

  The Flash write function is called to write authenticated and decrypted SW 
  modules to flash memory. */
typedef uint32_t (*CCBsvFlashWriteFunc) (
                    /*! [in] The address for writing to flash memory. */
					CCAddr_t flashAddress,
                    /*! [out] A pointer to the RAM source to read the 
					data from. */
					uint8_t *memSrc,
                    /*! [in] The size to write in bytes. */
					uint32_t sizeToWrite,
                    /*! [in] For partner use. */
					void* context 
                    );

/********* End of Function pointer definitions ***********/
//********************************************************************************************/
//RSA_H
/************************ Defines ******************************/

#define CC_BOOT_RSA_VERIFIER_ALG_FAILURE    (CC_SB_RSA_BASE_ERROR + 0x00000001)
#define CC_BOOT_RSA_VERIFIER_CMP_FAILURE    (CC_SB_RSA_BASE_ERROR + 0x00000002)

/* the modulus size ion bits */
#define RSA_EXP_SIZE_WORDS 			1


/* PKA max count of SRAM registers: */
#define RSA_HW_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS  PKA_MAX_COUNT_OF_PHYS_MEM_REGS /*32*/
/* PKA required count of SRAM registers: */
#define RSA_PKA_REQUIRED_COUNT_OF_PHYS_MEM_REGS     7

/* maximal size of extended register in "big PKA words" and in 32-bit words:  *
   the size defined according to RSA as more large, and used to define some   *
*  auxiliary buffers sizes      					      */
#define RSA_PKA_MAX_REGISTER_SIZE_IN_PKA_WORDS \
        ((SB_CERT_RSA_KEY_SIZE_IN_BITS + RSA_PKA_EXTRA_BITS + RSA_PKA_BIG_WORD_SIZE_IN_BITS - 1)/RSA_PKA_BIG_WORD_SIZE_IN_BITS + 1)
#define RSA_PKA_MAX_REGISTER_SIZE_WORDS  (RSA_PKA_MAX_REGISTER_SIZE_IN_PKA_WORDS*(RSA_PKA_BIG_WORD_SIZE_IN_BITS/CC_BITS_IN_32BIT_WORD))
#define RSA_PKA_MAX_REGISTER_SIZE_BITS   (RSA_PKA_MAX_REGISTER_SIZE_WORDS * CC_BITS_IN_32BIT_WORD)

/* size of Barrett modulus tag NP, used in PKA algorithms */
#define RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_BITS    (RSA_PKA_BIG_WORD_SIZE_IN_BITS + RSA_PKA_EXTRA_BITS)
#define RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_BYTES   (CALC_FULL_BYTES(RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_BITS))
#define RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS   (CALC_FULL_32BIT_WORDS(RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_BITS))

/* size of buffer for Barrett modulus tag NP, used for both 64- and 128-bits PKA */
#define RSA_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS  5 /*maximal of RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS*/
#define RSA_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_BYTES  (RSA_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE)

/* the public exponent */
#define RSA_PUBL_EXP_SIZE_IN_BITS  17UL
#define RSA_PUBL_EXP_SIZE_IN_BYTES (CALC_FULL_BYTES(RSA_PUBL_EXP_SIZE_IN_BITS))


/* RSA PSS verify definitions */
#define RSA_HASH_LENGTH  32 /*SHA256*/
#define RSA_PSS_SALT_LENGTH  32
#define RSA_PSS_PAD1_LEN     8

/* RSA Encrypt definitions */
#define RSA_ENCR_RND_PS_SIZE_BYTES   8
#define RSA_ENCR_DATA_IN_SIZE_BYTES  16

#define RSA_PKCS1_VER21   1



/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/


/*************************************************************************/
/**

   \brief RSA_PSS_Verify implements the Verify algorithm
   as defined in PKCS#1 v2.1

   @param[in] hwBaseAddress - HW base address. Relevant for HW
                    	      implementation, for SW it is ignored.
   @param[in] dataInHashResult - the DataIn hash result.
   @param[in] pN           - the modulus (2048 bits).
   @param[in] pNp          - the Barrett tag. Relevant for HW
                    	      implementation, for SW it is ignored.
   @param[in] pSig	 - the pointer to the signature to be
        		   verified.
        Note: All input arrays are in LE format of bytes and words.

   @return CCError_t - On success the value CC_OK is returned,
           on failure - a value from secureboot_error.h
*/

CCError_t RSA_PSS_Verify( unsigned long    hwBaseAddress,
                           CCHashResult_t  dataInHashResult,
                           uint32_t      *pN,
                           uint32_t      *pNp,
                           uint32_t      *pSig);


/*************************************************************************/
/**
 * @brief The RSA_CalcExponent calculates The following:
 *
 *                   Res = (Base ^ Exp) mod N ( Exp = 0x10001 )
 *
 *        The calculation is done in a secured way using the PIC.
 *
 * @param[in] hwBaseAddress - HW base address. Relevant for HW
 *                  	implementation, for SW it is ignored.
 * @Base_ptr[in]         - The pointer to the base buffer.
 * @pN[in]            - The pointer to the modulus buffer (2048 bits).
 * @pNp[in]           - The np vector buffer (160 bits). Relevant for HW
 *                         implementation, for SW it is ignored.
 * @pRes[out]         - The pointer to the buffer that will contain the result.
 *
 *     NOTE:  The SW version of the function uses a temp buffer, pointed by
 *            global pointer gSecBootExpTempBuff; the size of the buffer must be
 *            not less than 8*SB_CERT_RSA_KEY_SIZE_IN_WORDS + 1 word.
 *
 * @return CCError_t - On success CC_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
void RSA_CalcExponent(
		unsigned long hwBaseAddress,
		uint32_t *Base_ptr,
		uint32_t *pN,
		uint32_t *pNp,
		uint32_t *pRes);


/*************************************************************************/
/**
 * @brief The RSA_CalcNp calculates Np value and saves it into pNp:
 *
 * @param[in] hwBaseAddress - The HW base address. Relevant for HW
 *                      implementation, for SW it is ignored.
 * @pN[in]    - The pointer to the modulus buffer.
 * @pNp[out]  - The pointer to Np vector buffer. Its size must be >= 160.
 */
void RSA_CalcNp( unsigned long hwBaseAddress,
                        uint32_t *pN,
                        uint32_t *pNp);


//********************************************************************************************/
//_SECUREBOOT_PARSER_GEN_DEFS_H_
/*! Public key data structure. */
typedef struct {
	//uint32_t N[SB_CERT_RSA_KEY_SIZE_IN_WORDS]; 				/*!< N public key, big endian representation. */
	uint32_t Np[RSA_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];  	/*!< Np (Barrett n' value). */
}CCSbNParams_t;
typedef struct {
	uint32_t N[CC_RSA_MAX_KEY_GENERATION_HW_SIZE_BITS/CC_BITS_IN_32BIT_WORD]; 				/*!< N public key, big endian representation. */
}CCSbNarams_t;


/*! SW image data structure. */
typedef struct {
	uint32_t SwHashResult[HASH_RESULT_SIZE_IN_WORDS];  		/*!< Hash calculated on the record.*/
	CCAddr_t    memLoadAddr;					/*!< Memory load address. */
}CCSbHashRecordInfo_t;


/*! Signature structure. */
typedef struct {
	//uint32_t sig[SB_CERT_RSA_KEY_SIZE_IN_WORDS];				/*!< RSA PSS signature. */
	uint32_t sig[CC_RSA_MAX_KEY_GENERATION_HW_SIZE_BITS/CC_BITS_IN_32BIT_WORD];
}CCSbSignature_t;

/*! SW component additional parameters. */
typedef struct {
	CCAddr_t  StoreAddr;						/*!< Storage address. */
	uint32_t    Len;						/*!< Size of the SW component in words. */
}CCSbSwImgAddData_t;


/********* Supported algorithms definitions ***********/

/*! hash supported algorithms. */
typedef enum {
	HASH_SHA256_Alg_Output 		= 0x01, 	/*!< hash SHA 256 output. */
	HASH_SHA256_Alg_128_Output 	= 0x02,		/*!< hash SHA 256 output truncated to 128 bits. */
	HASH_Last              		= 0x7FFFFFFF

}CCSbHashAlg_t;


/*! RSA supported algorithms */
typedef enum {
	RSA_ALG_MIN,
	RSA_PSS_2048           = 0x01, 			/*!< RSA PSS 2048 after hash SHA 256 */
	RSA_PSS_3072           = 0x02, 			/*!< RSA PSS 3072 after hash SHA 256 */
	RSA_ALG_MAX,
	RSA_Last               = 0x7FFFFFFF
}CCSbSignAlg_t;
//********************************************************************************************/
//_SECDEBUG_API_H
/*! SOC-id size. */
#define CC_BSV_SEC_DEBUG_SOC_ID_SIZE    		0x20

/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
@brief This API enables/disables debug (through the DCU registers), according to the
permissions given in the debug certificate, or predefined values.
For more information, see ARM TrustZone CryptoCell-312 Software Integrator's Manual.


@return CC_OK on success.
@return A non-zero value from bsv_error.h on failure.
*/
CCError_t CC_BsvSecureDebugSet(
	unsigned long   hwBaseAddress,	/*!< [in] CryptoCell HW registers' base address. */
	uint32_t   *pDebugCertPkg,	/*!< [in] Pointer to the Secure Debug certificate package. NULL is a valid value. */
	uint32_t   certPkgSize,		/*!< [in] Byte size of the certificate package. */
	uint32_t   *pEnableRmaMode,	/*!< [out] RMA entry flag. Non-zero indicates RMA LCS entry is required. */
	uint32_t   *pWorkspace,		/*!< [in] Pointer buffer used internally */
	uint32_t   workspaceSize	/*!< [in] Size of the buffer used internally, minimal size is CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES. */
	);



//********************************************************************************************/
//_SECDEBUG_DEFS_H
#define CC_BSV_SEC_DEBUG_HASH_SIZE_IN_WORDS (HASH_SHA256_DIGEST_SIZE_IN_BYTES/CC_32BIT_WORD_SIZE)

#define CC_BSV_SEC_DEBUG_DCU_SIZE_IN_BITS   128
#define CC_BSV_SEC_DEBUG_DCU_SIZE_IN_BYTES   (CC_BSV_SEC_DEBUG_DCU_SIZE_IN_BITS/CC_BITS_IN_BYTE)
#define CC_BSV_SEC_DEBUG_DCU_SIZE_IN_WORDS   (CC_BSV_SEC_DEBUG_DCU_SIZE_IN_BITS/CC_BITS_IN_32BIT_WORD)

/*! Defines SOC ID */
typedef uint8_t        SocId_t[CC_BSV_SEC_DEBUG_SOC_ID_SIZE];
/*! Defines DCU */
typedef uint32_t       Dcu_t[CC_BSV_SEC_DEBUG_DCU_SIZE_IN_WORDS];
#if 0
typedef struct {  // must be word aligned!!!
		CCSbNarams_t	pubKeyN;
        CCSbNParams_t	pubKey;
        CCSbSignature_t 	signature;
} workspaceInt_t;
#endif
#if 1
typedef struct {  // must be word aligned!!!
		uint32_t	*pubKeyN;
        uint32_t	*pubKey;
        uint32_t 	*signature;
} workspaceInt_t;
#endif

/*! Secure Boot key certificate magic number. "S,B,K,C" */
#define CC_SB_KEY_CERT_MAGIC_NUMBER	 	0x53426b63
/*! Secure Boot content certificate magic number.  "S,B,C,C" */
#define CC_SB_CONTENT_CERT_MAGIC_NUMBER 	0x53426363
/*! Certificate debug enabler magic number. */
#define CC_CERT_SEC_DEBUG_ENABLER_MAGIC 	0x5364656E
/*! Certificate debug developer magic number. */
#define CC_CERT_SEC_DEBUG_DEVELOPER_MAGIC 	0x53646465



/* The  restiction mask is actualy teh ICV ownership mask; meaning all ICV bits are set to 1 , OEM bit are 0 */
#define DX_AO_ICV_DCU_OWNERSHIP_MASK0_REG_OFFSET 	             DX_AO_ICV_DCU_RESTRICTION_MASK0_REG_OFFSET
#define DX_AO_ICV_DCU_OWNERSHIP_MASK0_VALUE_BIT_SHIFT           DX_AO_ICV_DCU_RESTRICTION_MASK0_VALUE_BIT_SHIFT
#define DX_AO_ICV_DCU_OWNERSHIP_MASK0_VALUE_BIT_SIZE 	           DX_AO_ICV_DCU_RESTRICTION_MASK0_VALUE_BIT_SIZE
#define DX_AO_ICV_DCU_OWNERSHIP_MASK1_REG_OFFSET 	             DX_AO_ICV_DCU_RESTRICTION_MASK1_REG_OFFSET
#define DX_AO_ICV_DCU_OWNERSHIP_MASK1_VALUE_BIT_SHIFT           DX_AO_ICV_DCU_RESTRICTION_MASK1_VALUE_BIT_SHIFT
#define DX_AO_ICV_DCU_OWNERSHIP_MASK1_VALUE_BIT_SIZE             DX_AO_ICV_DCU_RESTRICTION_MASK1_VALUE_BIT_SIZE
#define DX_AO_ICV_DCU_OWNERSHIP_MASK2_REG_OFFSET 	             DX_AO_ICV_DCU_RESTRICTION_MASK2_REG_OFFSET
#define DX_AO_ICV_DCU_OWNERSHIP_MASK2_VALUE_BIT_SHIFT           DX_AO_ICV_DCU_RESTRICTION_MASK2_VALUE_BIT_SHIFT
#define DX_AO_ICV_DCU_OWNERSHIP_MASK2_VALUE_BIT_SIZE 	           DX_AO_ICV_DCU_RESTRICTION_MASK2_VALUE_BIT_SIZE
#define DX_AO_ICV_DCU_OWNERSHIP_MASK3_REG_OFFSET 	             DX_AO_ICV_DCU_RESTRICTION_MASK3_REG_OFFSET
#define DX_AO_ICV_DCU_OWNERSHIP_MASK3_VALUE_BIT_SHIFT           DX_AO_ICV_DCU_RESTRICTION_MASK3_VALUE_BIT_SHIFT
#define DX_AO_ICV_DCU_OWNERSHIP_MASK3_VALUE_BIT_SIZE 	           DX_AO_ICV_DCU_RESTRICTION_MASK3_VALUE_BIT_SIZE

/********* Certificate structure definitions ***********/


/*! Certificate header structure. */
typedef struct {
        uint32_t magicNumber;           /*!< Magic number to validate the certificate. */
        uint32_t certVersion;           /*!< Certificate version to validate the certificate. */
        uint32_t certSize;              /*!< Offset in words to the Certificate signature.
                                            And number of SW components , if any exist.*/
        uint32_t certFlags;             /*!< Bit field according to certificate type */
}CCSbCertHeader_t;


// All certificate header flags, first 4 bits are for certificate type,
// next 4 bits are rsa algorithm used.
// for  key certficate and enabler ecrtificate next 4 bits are HBK-id used

/* Key certificate definitions */
typedef union {
        struct {
                uint32_t      hbkId:4;  // must be first
                uint32_t      reserved:28;
        }flagsBits;
        uint32_t      flagsWord;
} keyCertFlags_t;

typedef struct {
        uint32_t 	swVer;
        CCHashResult_t		nextPubKeyHash;
} KeyCertMain_t;


typedef struct {
        CCSbCertHeader_t	certHeader;
		CCSbNarams_t        	certPubKeyN;
        CCSbNParams_t        	certPubKey;
        KeyCertMain_t		certBody;
        CCSbSignature_t      	certSign;
} KeyCert_t;

/* Content certificate definitions */
/*! Content Certificate flag bit field structure. */
typedef union {
        /*! Flags definitions in bits.*/
        struct {
                uint32_t      hbkId:4;
                uint32_t      swCodeEncType:4;
                uint32_t      swLoadVerifyScheme:4;
                uint32_t      swCryptoType:4;
                uint32_t      numOfSwCmp:16;
        }flagsBits;
        /*! Flags definition as a word.*/
        uint32_t      flagsWord;
} CCSbCertFlags_t;


typedef struct {
        CCHashResult_t		imageHash;
        CCAddr_t 	loadAddr;
        uint32_t 	imageMaxSize;
        uint32_t 	isAesCodeEncUsed;
} ContentCertImageRecord_t;

typedef struct {
        uint32_t 	swVer;
        CCSbNonce_t 	nonce;
        ContentCertImageRecord_t 	imageRec[CC_SB_MAX_NUM_OF_IMAGES];
} ContentCertMain_t;



typedef struct {
        CCSbCertHeader_t	certHeader;
		CCSbNarams_t        certPubKeyN;		
        CCSbNParams_t       certPubKey;
        ContentCertMain_t	certBody;
        CCSbSignature_t     certSign;
} ContentCert_t;


/* Enabler certificate definitions */
typedef union {
        struct {
                uint32_t      hbkId:4; // must be first
                uint32_t      lcs:4;
                uint32_t      isRma:4;
                uint32_t      reserved:20;
        }flagsBits;
        uint32_t      flagsWord;
} EnablerCertFlags_t;

/* definition for enabler certificate */
typedef struct {
        Dcu_t      	debugMask;
        Dcu_t      	debugLock;
        CCHashResult_t		nextPubKeyHash;
} EnablerCertMain_t;

typedef struct {
        CCSbCertHeader_t	certHeader;
		CCSbNarams_t        certPubKeyN;		
        CCSbNParams_t 		certPubKey;
        EnablerCertMain_t	certBody;
        CCSbSignature_t     certSign;
} EnablerCert_t;

/* Developer certificate definitions */
typedef struct {
        struct {
                uint32_t      reserved:32;
        }flagsBits;
        uint32_t      flagsWord;
} DeveloperCertFlags_t;

typedef struct {
        Dcu_t      	debugMask;
        SocId_t         socId;
} DeveloperCertMain_t;

typedef struct {
        CCSbCertHeader_t	certHeader;
		CCSbNarams_t        certPubKeyN;		
        CCSbNParams_t       certPubKey;
        DeveloperCertMain_t	certBody;
        CCSbSignature_t     certSign;
} DeveloperCert_t;



/*! Certificate types structure. */
typedef enum {
        /*! Reserved.*/
        CC_SB_MIN_CERT,
        /*! Key certificate. */
        CC_SB_KEY_CERT = 1,
        /*! Content certificate. */
        CC_SB_CONTENT_CERT = 2,
        /*! Key or content certificate. */
        CC_SB_KEY_OR_CONTENT_CERT = 3,
        /*! Debug enabler certificate. */
        CC_SB_ENABLER_CERT = 4,
        /*! Debug developer certificate. */
        CC_SB_DEVELOPER_CERT = 5,
        /*! Max number of certificates types.*/
        CC_SB_MAX_CERT,
        /*! Reserved.*/
        CC_SB_CERT_TYPE_LAST = 0x7FFFFFFF

}CCSbCertTypes_t;


/*! Certificate types structure. */
typedef enum {
        /*! First certificate in chain.*/
        CC_SB_FIRST_CERT_IN_CHAIN = 0,
        /*! Second certificate in chain.*/
        CC_SB_SECOND_CERT_IN_CHAIN = 1,
        /*! Third and last certificate in chain.*/
        CC_SB_THIRD_CERT_IN_CHAIN = 2,
        /*! Last certificate in chain.*/
        CC_SB_LAST_CERT_IN_CHAIN = 3,
        /*! Reserved.*/
        CC_SB_RESERVED_CERT_IN_CHAIN = 0x7FFFFFFF

}CCSbCertOrderInChain_t;



/*!  MAX size of certificate pkg. */
#ifdef CC_SB_X509_CERT_SUPPORTED
#define CC_SB_MAX_KEY_CERT_SIZE_IN_BYTES	(0x500UL)
#define CC_SB_MAX_CONTENT_CERT_SIZE_IN_BYTES	(0x7A0UL) /* may contain up to 16 signed sw images */
#define CC_SB_MAX_ENABLER_CERT_SIZE_IN_BYTES	(0x500UL)
#define CC_SB_MAX_DEVELOPER_CERT_SIZE_IN_BYTES	(0x500UL)
#define CC_SB_MAX_CERT_SIGN_SIZE_IN_BYTES               (0x1D0)
#else
#define CC_SB_MAX_KEY_CERT_SIZE_IN_BYTES	(sizeof(KeyCert_t))
#define CC_SB_MAX_CONTENT_CERT_SIZE_IN_BYTES	(sizeof(ContentCert_t)) /* may contain up to 16 signed sw images */
#define CC_SB_MAX_ENABLER_CERT_SIZE_IN_BYTES	(sizeof(EnablerCert_t))
#define CC_SB_MAX_DEVELOPER_CERT_SIZE_IN_BYTES	(sizeof(DeveloperCert_t))
#define CC_SB_MAX_CERT_SIGN_SIZE_IN_BYTES               (sizeof(CCSbSignature_t))
#endif

#define CC_SB_MAX_KEY_CERT_BODY_SIZE_IN_BYTES	(sizeof(KeyCertMain_t))
#define CC_SB_MAX_CONTENT_CERT_BODY_SIZE_IN_BYTES	(sizeof(ContentCertMain_t)) /* may contain up to 16 signed sw images */
#define CC_SB_MAX_ENABLER_CERT_BODY_SIZE_IN_BYTES	(sizeof(EnablerCertMain_t))
#define CC_SB_MAX_DEVELOPER_CERT_BODY_SIZE_IN_BYTES	(sizeof(DeveloperCertMain_t))

#define CC_SB_MAX_CONTENT_PKG_SIZE_IN_BYTES     (CC_SB_MAX_CONTENT_CERT_SIZE_IN_BYTES + SW_REC_NONE_SIGNED_DATA_SIZE_IN_BYTES*CC_SB_MAX_NUM_OF_IMAGES)

#define CC_SB_MAX_CERT_PKG_SIZE_IN_BYTES	(CC_SB_MAX_KEY_CERT_SIZE_IN_BYTES+CC_SB_MAX_ENABLER_CERT_SIZE_IN_BYTES+CC_SB_MAX_DEVELOPER_CERT_SIZE_IN_BYTES)


/* check KDR error bit in LCS register */
#define DCU_RESET_OVERRIDE_BIT_SHIFT	0x0
#define DCU_RESET_OVERRIDE_BIT_SIZE	0x1
#define IS_DCU_RESET_OVERRIDE(dcuVal) 	((dcuVal>>DCU_RESET_OVERRIDE_BIT_SHIFT) & DCU_RESET_OVERRIDE_BIT_SIZE)

#define CLEAR_ALL_DCU(dcuVal) {\
	dcuVal[0] = DCU_DISABLE_ALL_DBG; \
	dcuVal[1] = DCU_DISABLE_ALL_DBG; \
	dcuVal[2] = DCU_DISABLE_ALL_DBG; \
	dcuVal[3] = DCU_DISABLE_ALL_DBG; \
}

#define LOCK_ALL_DCU(dcuLock) {\
	dcuLock[0] = DCU_ENABLE_ALL_DBG; \
	dcuLock[1] = DCU_ENABLE_ALL_DBG; \
	dcuLock[2] = DCU_ENABLE_ALL_DBG; \
	dcuLock[3] = DCU_ENABLE_ALL_DBG; \
}


#define WRITE_DCU_LOCK(hwBaseAddress, dcuLock, rc) {\
	uint32_t ii = 0;\
	volatile uint32_t rr = 0;\
	for (ii = 0; ii < CC_BSV_SEC_DEBUG_DCU_SIZE_IN_WORDS; ii++) {\
		SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HOST_DCU_LOCK0) + ii * sizeof(uint32_t), dcuLock[ii]); \
		SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress,HOST_DCU_LOCK0) + ii * sizeof(uint32_t), rr);       \
		if(rr!=dcuLock[ii]) { \
			rc = CC_BSV_AO_WRITE_FAILED_ERR; \
		} \
	}\
}

#define WRITE_DCU_VAL(hwBaseAddress, dcuVal) {\
	uint32_t ii = 0;\
	for (ii = 0; ii < CC_BSV_SEC_DEBUG_DCU_SIZE_IN_WORDS; ii++) {\
		SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HOST_DCU_EN0) + ii * sizeof(uint32_t), dcuVal[ii]); \
		CC_BSV_WAIT_ON_NVM_IDLE_BIT(hwBaseAddress); \
	}\
}

/* Read-Modify-Write a field of a register */
#define READ_MODIFY_WRITE_AO_REGISTER(hwBaseAddress, regName, fldName, fldVal, tc) \
do {								            \
	volatile uint32_t regVal = 0; \
	volatile uint32_t rr = 0; \
	SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress,regName), regVal);       \
	CC_REG_FLD_SET(DX, regName, fldName, regVal, fldVal); \
	SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, regName), regVal);       \
	SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress,regName), rr);       \
	if(rr!=regVal) { \
		rc = CC_BSV_AO_WRITE_FAILED_ERR; \
	} \
} while (0)


#define READ_DCU_LOCK_DEFAULT(hwBaseAddress, dcuLock, rc) \
do {\
	uint32_t ii; \
	for (ii = 0; ii<CC_OTP_DCU_SIZE_IN_WORDS; ii++) { \
		rc = CC_BsvOTPWordRead(hwBaseAddress, (CC_OTP_DCU_OFFSET+ii), &dcuLock[ii]); \
		if (rc != CC_OK) { \
			break;\
		} \
	}  \
} while(0)
//********************************************************************************************/
//_COMMON_CERT_PARSER_H
/* certificate version Major offset */
#define CERT_VERSION_MAJOR_BIT_SHIFT	  16

typedef struct {
        CCSbCertTypes_t certType;
        CCSbCertHeader_t certHeader;
        uint8_t		*pCertBody;
        uint32_t certBodySize;
} CertFieldsInfo_t;

typedef struct {
        uint32_t   *pBuffer;
        uint32_t   bufferSize;
} BufferInfo32_t;

CCError_t CCCertSecDbgParse(uint32_t   *pDebugCertPkg,
                            uint32_t   certPkgSize,
                            BufferInfo32_t  *pKeyCert,       // out
                            BufferInfo32_t  *pEnablerCert,   // out
                            BufferInfo32_t  *pDeveloperCert); // out

uint32_t CCCertLoadCertificate(CCSbFlashReadFunc flashRead_func,
                               void *userContext,
                               CCAddr_t certAddress,
                               uint32_t *pCert,
                               uint32_t *pCertBufferWordSize);

/**
   @brief This function copy N, Np (CCSbNParams_t) and signature
   (certificate start address + sizeof certificate in certificate header) from the certificate to workspace.
   Return pointer to certificate header CCSbCertHeader_t, and pointer to cert body sizeof()

 */
CCError_t CCCertFieldsParse(BufferInfo32_t  *pCertInfo,
                            BufferInfo32_t  *pWorkspaceInfo,
                            CertFieldsInfo_t  *pCertFields,
                            uint32_t	**ppCertStartSign,
                            uint32_t	*pCertSignedSize,
                            BufferInfo32_t  *pX509HeaderInfo);
//********************************************************************************************/
//_BSV_CRYPTO_DRIVER_H
/*! SHA256 digest result in words. */
#define CC_BSV_SHA256_DIGEST_SIZE_IN_WORDS	8
/*! SHA256 digest result in bytes. */
#define CC_BSV_SHA256_DIGEST_SIZE_IN_BYTES	CC_BSV_SHA256_DIGEST_SIZE_IN_WORDS*sizeof(uint32_t)

/*! SHA256 maximal data size to be hashed */
#define CC_BSV_SHA256_MAX_DATA_SIZE_IN_BYTES	0x00010000 /* 64KB */


/*! The derived key size for 128 bits. */
#define CC_BSV_128BITS_KEY_SIZE_IN_BYTES	16

/*! The derived key size for 256 bits. */
#define CC_BSV_256BITS_KEY_SIZE_IN_BYTES	32

/*! Maximal label length in bytes. */
#define CC_BSV_MAX_LABEL_LENGTH_IN_BYTES	8

/*! Maximal context length in bytes. */
#define CC_BSV_MAX_CONTEXT_LENGTH_IN_BYTES	32

/*! KDF 128 bits key fixed data size in bytes. */
#define CC_BSV_KDF_DATA_128BITS_SIZE_IN_BYTES	3 /*!< \internal 0x01, 0x00, lengt(-0x80) */
/*! KDF 256 bits key fixed data size in bytes. */
#define CC_BSV_KDF_DATA_256BITS_SIZE_IN_BYTES	4 /*!< \internal 0x02, 0x00, lengt(-0x0100) */

/*! KDF data maximal size in bytes. */
#define CC_BSV_KDF_MAX_SIZE_IN_BYTES (CC_BSV_KDF_DATA_256BITS_SIZE_IN_BYTES + CC_BSV_MAX_LABEL_LENGTH_IN_BYTES + CC_BSV_MAX_CONTEXT_LENGTH_IN_BYTES)


/*! Maximal AES CCM associated data size in bytes. */
#define CC_BSV_CCM_MAX_ASSOC_DATA_SIZE_IN_BYTES		    0xff00 	/* 2^16-2^8 */

/*! Maximal AES CCM text data size in bytes. */
#define CC_BSV_CCM_MAX_TEXT_DATA_SIZE_IN_BYTES		0x00010000 	/* 64KB */

/*! AES block size in bytes. */
#define BSV_AES_BLOCK_SIZE_IN_BYTES	16
/*! AES IV size in bytes. */
#define BSV_AES_IV_SIZE_IN_BYTES        16
/*! AES IV size in words. */
#define BSV_AES_IV_SIZE_IN_WORDS        4

/*! HASH SHA256 control value. */
#define BSV_HASH_CTL_SHA256_VAL	        0x2UL
/*! HASH SHA256 padding configuration. */
#define BSV_HASH_PAD_CFG_VAL	        0x4UL

/************************ Typedefs  *****************************/
/*! Definitions of cryptographic mode. */
typedef enum bsvCryptoMode {
	/*! AES.*/
	BSV_CRYPTO_AES = 1,
	/*! AES and HASH.*/
	BSV_CRYPTO_AES_AND_HASH = 3,
	/*! HASH.*/
	BSV_CRYPTO_HASH = 7,
	/*! AES to HASH and to DOUT.*/
	BSV_CRYPTO_AES_TO_HASH_AND_DOUT = 10,
	/*! Reserved.*/
	BSV_CRYPTO_RESERVE32B = INT32_MAX
}bsvCryptoMode_t;

/*! Definitions for AES modes. */
typedef enum bsvAesMode {
	/*! AES CTR mode.*/
	BSV_AES_CIPHER_CTR = 2,
	/*! AES CBC MAC mode.*/
	BSV_AES_CIPHER_CBC_MAC = 3,
	/*! AES CMAC mode.*/
	BSV_AES_CIPHER_CMAC = 7,
	/*! AES CCM PE mode.*/
	BSV_AES_CIPHER_CCMPE = 9,
	/*! AES CCM PD mode.*/
	BSV_AES_CIPHER_CCMPD = 10,
	/*! Reserved.*/
	BSV_AES_CIPHER_RESERVE32B = INT32_MAX
}bsvAesMode_t;

/*! Definitions for AES directions. */
typedef enum bsvAesDirection {
	/*! Encrypt.*/
	BSV_AES_DIRECTION_ENCRYPT = 0,
	/*! Decrypt.*/
	BSV_AES_DIRECTION_DECRYPT = 1,
	/*! Reserved.*/
	BSV_AES_DIRECTION_RESERVE32B = INT32_MAX
}bsvAesDirection_t;

/*! Defintions for AES key sizes. */
typedef enum bsvAesKeySize {
	/*! 128 bits AES key. */
	BSV_AES_KEY_SIZE_128BITS = 0,
	/*! 256 bits AES key. */
	BSV_AES_KEY_SIZE_256BITS = 2,
	/*! Reserved.*/
	BSV_AES_KEY_SIZE_RESERVE32B = INT32_MAX
}bsvAesKeySize_t;

/***************************** function declaration **************************/


CCError_t BsvAes(unsigned long 			hwBaseAddress,
			bsvAesMode_t 		mode,
			CCBsvKeyType_t       	keyType,
			uint32_t	    	*pUserKey,
			size_t         		userKeySize,
			uint32_t 		*pIvBuf,
			uint8_t			*pDataIn,
			uint8_t 		*pDataOut,
			size_t                 	dataSize,
			CCBsvCmacResult_t 	cmacResBuf);

CCError_t BsvCryptoImageInit( unsigned long 	hwBaseAddress,
			bsvCryptoMode_t 	mode,
			CCBsvKeyType_t		keyType);

CCError_t BsvCryptoImageUpdate( unsigned long 	hwBaseAddress,
			bsvCryptoMode_t 	mode,
			CCBsvKeyType_t       	keyType,
			uint32_t 		*pCtrStateBuf,
			uint8_t			*pDataIn,
			uint8_t 		*pDataOut,
			size_t                 	dataSize,
			CCHashResult_t      	hashBuff,
			uint8_t 		isLoadIV);

CCError_t BsvCryptoImageFinish( unsigned long 	hwBaseAddress,
			bsvCryptoMode_t 	mode,
			CCHashResult_t      	hashBuff);


/* SHA256 */
void InitBsvHash(unsigned long hwBaseAddress);
void FreeBsvHash(unsigned long hwBaseAddress);
CCError_t ProcessBsvHash(unsigned long hwBaseAddress, uint32_t inputDataAddr, uint32_t dataInSize);
void FinishBsvHash(unsigned long hwBaseAddress, CCHashResult_t	HashBuff);


/* AES (CTR, CMAC ) */

void InitBsvAes(unsigned long hwBaseAddress);
void FreeBsvAes(unsigned long hwBaseAddress);
CCError_t ProcessBsvAes(unsigned long 	hwBaseAddress,
		bsvAesMode_t 		mode,
		CCBsvKeyType_t 		keyType,
		uint32_t	    	*pUserKey,
		size_t         		userKeySize,
		uint32_t 		*pCtrStateBuf,
		uint32_t 		inputDataAddr,
		uint32_t 		outputDataAddr,
		uint32_t 		blockSize,
		uint8_t			isLoadIv);

void FinishBsvAes(unsigned long hwBaseAddress,
	bsvAesMode_t 		mode,
	CCBsvCmacResult_t 	cmacResBuf);

/* AES-CCM */

CCError_t ProcessBsvAesCcm(unsigned long 	hwBaseAddress,
			bsvAesMode_t 	mode,
			uint32_t 	*pKeyBuf,
			uint32_t 	*pIvBuf,
			uint32_t 	*pCtrStateBuf,
			uint32_t 	inputDataAddr,
			uint32_t 	outputDataAddr,
			uint32_t 	blockSize);

void FinishBsvAesCcm(unsigned long hwBaseAddress,
		bsvAesMode_t 	mode,
		uint32_t 	*pIvBuf,
		uint32_t 	*pCtrStateBuf);
//********************************************************************************************/
//CC_AES_DEFS_H
/************************ Defines  ******************************/
/*! The size of the AES block in words. */
#define CC_AES_CRYPTO_BLOCK_SIZE_IN_WORDS 4
/*! The size of the AES block in bytes. */
#define CC_AES_BLOCK_SIZE_IN_BYTES  (CC_AES_CRYPTO_BLOCK_SIZE_IN_WORDS * sizeof(uint32_t))

/*! The size of the IV buffer in words. */
#define CC_AES_IV_SIZE_IN_WORDS   CC_AES_CRYPTO_BLOCK_SIZE_IN_WORDS
/*! The size of the IV buffer in bytes. */
#define CC_AES_IV_SIZE_IN_BYTES  (CC_AES_IV_SIZE_IN_WORDS * sizeof(uint32_t))

#if 0
/************************ Enums ********************************/
/*! The AES operation:<ul><li>Encrypt</li><li>Decrypt</li></ul>. */
typedef enum {
    /*! An AES encrypt operation. */
    CC_AES_ENCRYPT = 0,
    /*! An AES decrypt operation. */
    CC_AES_DECRYPT = 1,
    /*! The maximal number of operations. */
    CC_AES_NUM_OF_ENCRYPT_MODES,
    /*! Reserved. */
    CC_AES_ENCRYPT_MODE_LAST = 0x7FFFFFFF
}CCAesEncryptMode_t;

/*! The AES operation mode. */
typedef enum {
    /*! ECB mode. */
    CC_AES_MODE_ECB          = 0,
    /*! CBC mode. */
    CC_AES_MODE_CBC          = 1,
    /*! CBC-MAC mode. */
    CC_AES_MODE_CBC_MAC      = 2,
    /*! CTR mode. */
    CC_AES_MODE_CTR          = 3,
    /*! XCBC-MAC mode. */
    CC_AES_MODE_XCBC_MAC     = 4,
    /*! CMAC mode. */
    CC_AES_MODE_CMAC         = 5,
    /*! XTS mode. */
    CC_AES_MODE_XTS          = 6,
    /*! CBC-CTS mode. */
    CC_AES_MODE_CBC_CTS      = 7,
    /*! OFB mode. */
    CC_AES_MODE_OFB          = 8,

    /*! The maximal number of AES modes. */
    CC_AES_NUM_OF_OPERATION_MODES,
    /*! Reserved. */
    CC_AES_OPERATION_MODE_LAST = 0x7FFFFFFF
}CCAesOperationMode_t;

/*! The AES padding type. */
typedef enum {
       /*! No padding. */
       CC_AES_PADDING_NONE  = 0,
       /*! PKCS7 padding. */
       CC_AES_PADDING_PKCS7 = 1,
       /*! The maximal number of AES padding modes. */
       CC_AES_NUM_OF_PADDING_TYPES,
       /*! Reserved. */
       CC_AES_PADDING_TYPE_LAST = 0x7FFFFFFF
}CCAesPaddingType_t;


/*! The AES key type. */
typedef enum {
    /*! The user key. */
    CC_AES_USER_KEY          = 0,
    /*! The Kplt hardware key. */
    CC_AES_PLATFORM_KEY      = 1,
    /*! The Kcst hardware key. */
    CC_AES_CUSTOMER_KEY      = 2,
    /*! The maximal number of AES key types. */
    CC_AES_NUM_OF_KEY_TYPES,
    /*! Reserved. */
    CC_AES_KEY_TYPE_LAST = 0x7FFFFFFF
}CCAesKeyType_t;
#endif
/************************ Typedefs  ****************************/

/*! Defines the IV buffer. A 16-byte array. */
typedef uint8_t CCAesIv_t[CC_AES_IV_SIZE_IN_BYTES];

/*! Defines the AES key data buffer. */
typedef uint8_t CCAesKeyBuffer_t[CC_AES_KEY_MAX_SIZE_IN_BYTES];

/************************ Structs  ******************************/
#if 0
/*!
 The context prototype of the user.

 The argument type that is passed by the user to the AES APIs. The context
 saves the state of the operation, and must be saved by the user until
 the end of the API flow.
 */
typedef struct CCAesUserContext_t {
    /*! The context buffer for internal usage. */
    uint32_t buff[CC_AES_USER_CTX_SIZE_IN_WORDS] ;
}CCAesUserContext_t;


/*! The AES key data of the user. */
typedef struct CCAesUserKeyData_t {
    /*! A pointer to the key. */
    uint8_t * pKey;
    /*! The size of the key in bytes. Valid values for XTS mode, if supported:
    32 bytes or 64 bytes, indicating the full size of the double key (2x128 or
    2x256 bit). Valid values for XCBC-MAC mode: 16 bytes, as limited by the
    standard. Valid values for all other modes: 16 bytes, 24 bytes, or
    32 bytes. */
    size_t    keySize;
}CCAesUserKeyData_t;

/*! The AES HW key Data. */
typedef struct CCAesHwKeyData_t {
    /*! Slot number. */
    size_t slotNumber;
}CCAesHwKeyData_t;
#endif
//********************************************************************************************/
//_CC_CRYPTO_DEFS_H
/************************ Hash Definitions ******************************/

#define HASH_MD5_DIGEST_SIZE_IN_BYTES		16
#define HASH_SHA1_DIGEST_SIZE_IN_BYTES		20
#define HASH_SHA224_DIGEST_SIZE_IN_BYTES	28
#define HASH_SHA256_DIGEST_SIZE_IN_BYTES	32
#define HASH_SHA384_DIGEST_SIZE_IN_BYTES	48
#define HASH_SHA512_DIGEST_SIZE_IN_BYTES	64

#define HASH_MD5_BLOCK_SIZE_IN_BYTES		64
#define HASH_SHA1_BLOCK_SIZE_IN_BYTES		64
#define HASH_SHA224_BLOCK_SIZE_IN_BYTES		64
#define HASH_SHA256_BLOCK_SIZE_IN_BYTES		64
#define HASH_SHA384_BLOCK_SIZE_IN_BYTES		128
#define HASH_SHA512_BLOCK_SIZE_IN_BYTES		128



/************************ AES Definitions ******************************/

#define AES_BLOCK_SIZE_IN_BYTES		16

#define AES_IV_SIZE_IN_BYTES		AES_BLOCK_SIZE_IN_BYTES


/* AES-CCM Definitions */
#define AES_CCM_NONCE_LENGTH_MIN	7
#define AES_CCM_NONCE_LENGTH_MAX	13

#define AES_CCM_TAG_LENGTH_MIN		4
#define AES_CCM_TAG_LENGTH_MAX		16



/************************ DES Definitions ******************************/

#define DES_IV_SIZE_IN_BYTES		8


//********************************************************************************************/
//_CRYPTO_DRIVER_DEFS_H
/******************************************************************************
*                        	DEFINITIONS
******************************************************************************/
#define SB_SET_CLK_ENABLE_VAL		0x1UL
#define SB_SET_CLK_DISABLE_VAL		0x0UL

#define SB_CRYPTO_CTL_AES_MODE		0x1UL
#define SB_CRYPTO_CTL_AES_TO_HASH_MODE	0xAUL
#define SB_CRYPTO_CTL_HASH_MODE		0x7UL

#define SB_AES_ENCRYPT			0x0UL
#define SB_AES_DECRYPT			0x1UL

#define SB_HASH_CTL_SHA256_VAL		0x2UL
#define SB_AES_CTRL_MODE_BIT_SHIFT      2

#define SB_HASH_BLOCK_SIZE_BYTES	64

typedef enum {
	CC_SB_ECB_MODE      = 0,
	CC_SB_CBC_MODE      = 1,
	CC_SB_CTR_MODE      = 2,
	CC_SB_CBC_MAC_MODE  = 3,
	CC_SB_CMAC_MODE     = 7,
	CC_SB_MAX_MODES     = 0x7FFFFFFFUL

}CCSbAesModes;


/* Poll on the DOUT DMA busy till it is = 0 */
#define CC_SB_WAIT_ON_DOUT_DMA_BUSY(hwBaseAddress) 						\
	do { 											\
		uint32_t regVal; 								\
		do { 										\
			SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, DOUT_MEM_DMA_BUSY), regVal); 	\
		}while( regVal ); 								\
	}while(0)

/* Poll on the DIN DMA busy till it is = 0 */
#define CC_SB_WAIT_ON_DIN_DMA_BUSY(hwBaseAddress) 						\
	do { 											\
		uint32_t regVal; 								\
		do { 										\
			SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, DIN_MEM_DMA_BUSY), regVal); 	\
		}while( regVal ); 								\
	}while(0)

/* Poll on the DOUT DMA busy till it is = 0 */
#define CC_SB_WAIT_ON_CRYPTO_BUSY(hwBaseAddress) 						\
	do { 											\
		uint32_t regVal; 								\
		do { 										\
			SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, CRYPTO_BUSY), regVal); 	\
		}while( regVal ); 								\
	}while(0)

/* Poll on the AES busy till it is = 0 */
#define CC_SB_WAIT_ON_AES_BUSY()\
	do {\
		uint32_t regVal=1;\
	        do {\
			SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, AES_BUSY), regVal); 	\
                }while( regVal );\
        }while(0)

/* Poll on the HASH busy till it is = 0 */
#define CC_SB_WAIT_ON_HASH_BUSY()\
	do {\
		uint32_t regVal=1;\
	        do {\
			SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, HASH_BUSY), regVal); 	\
                }while( regVal );\
        }while(0)

/* Use constant counter ID and AXI ID */
#define SB_COUNTER_ID	0


/* The AES block size in words and in bytes */
#define AES_BLOCK_SIZE_IN_WORDS 4

/* The size of the IV or counter buffer */
#define AES_IV_COUNTER_SIZE_IN_WORDS   AES_BLOCK_SIZE_IN_WORDS
#define AES_IV_COUNTER_SIZE_IN_BYTES  (AES_IV_COUNTER_SIZE_IN_WORDS * sizeof(uint32_t))

/* The size of the AES KEY in words and bytes */
#define AES_KEY_SIZE_IN_WORDS AES_BLOCK_SIZE_IN_WORDS
#define AES_KEY_SIZE_IN_BYTES (AES_KEY_SIZE_IN_WORDS * sizeof(uint32_t))

#define AES_Key128Bits_SIZE_IN_WORDS	AES_BLOCK_SIZE_IN_WORDS
#define AES_Key128Bits_SIZE_IN_BYTES	AES_BLOCK_SIZE_IN_BYTES
#define AES_Key256Bits_SIZE_IN_WORDS	8
#define AES_Key256Bits_SIZE_IN_BYTES	(AES_Key256Bits_SIZE_IN_WORDS * sizeof(uint32_t))


/* Hash IV+Length */
#define HASH_DIGEST_SIZE_IN_WORDS	8
#define HASH_DIGEST_SIZE_IN_BYTES	(HASH_DIGEST_SIZE_IN_WORDS * sizeof(uint32_t))
#define HASH_LENGTH_SIZE_IN_WORDS	4
#define HASH_LENGTH_SIZE_IN_BYTES	(HASH_LENGTH_SIZE_IN_WORDS * sizeof(uint32_t))


/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/


#define NVM_HASH_Result_t CCHashResult_t

/* Defines the IV counter buffer  - 16 bytes array */
typedef uint32_t AES_Iv_t[AES_IV_COUNTER_SIZE_IN_WORDS];

/* Defines the AES key buffer */
typedef uint32_t AES_Key_t[AES_KEY_SIZE_IN_WORDS];

/* Defines the AES CMAC output result */
typedef uint8_t AES_CMAC_RESULT_t[AES_BLOCK_SIZE_IN_BYTES];


typedef enum {
	CRYPTO_DRIVER_HASH_MODE  	= 0,
	CRYPTO_DRIVER_AES_CTR_MODE	= 1,
	CRYPTO_DRIVER_AES_CTR_TO_HASH_MODE = 2,
	CRYPTO_DRIVER_AES_CMAC_MODE	= 3
}CryptoDriverMode_t;


/* enum definitons for crypto operation completion mode */
typedef enum
{
	CC_SB_CRYPTO_COMPLETION_NO_WAIT = 0,
	CC_SB_CRYPTO_COMPLETION_NO_WAIT_ASK_ACK = 1,
	CC_SB_CRYPTO_COMPLETION_WAIT_UPON_START = 2,
	CC_SB_CRYPTO_COMPLETION_WAIT_UPON_END = 3
} CCSbCryptoCompletionMode_t;

//********************************************************************************************/
//_CRYPTO_DRIVER_H
#define SBROM_CryptoHash(wBaseAddress, inputDataAddr, dataSize, hashBuff)		\
	CC_BsvSHA256(hwBaseAddress, (uint8_t *)inputDataAddr, (size_t)dataSize, hashBuff);

CCError_t CC_BsvSHA256(
    /*! [in] The base address for CryptoCell HW registers. */
    unsigned long   hwBaseAddress,
    /*! [in] A pointer to the input data to be hashed. The buffer must be
    contiguous. */
    uint8_t     *pDataIn,
    /*! [in] The size of the data to be hashed in bytes. Limited to 64KB. */
    size_t      dataSize,
    /*! [out] A pointer to a word-aligned 32-byte buffer. */
    CCHashResult_t  hashBuff
    );
//********************************************************************************************/
//RSA_HWDEFS_H
/************************ Defines ******************************/

/* PKA big word size in bits */
#define RSA_PKA_BIG_WORD_SIZE_IN_BITS  CC_PKA_WORD_SIZE_IN_BITS
#define RSA_PKA_BIG_WORD_SIZE_IN_BYTES  (RSA_PKA_BIG_WORD_SIZE_IN_BITS/8)
#define RSA_PKA_BIG_WORD_SIZE_IN_32_BIT_WORDS  (RSA_PKA_BIG_WORD_SIZE_IN_BITS/32)

#define RSA_PKA_EXTRA_BITS  PKA_EXTRA_BITS

/* PKA max count of SRAM registers: */
#define RSA_HW_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS  PKA_MAX_COUNT_OF_PHYS_MEM_REGS

/* The maximal size of allowed PKA physical registers memory including tables */
#define RSA_PKA_MAX_REGS_MEM_SIZE_BYTES   (6 * 1024)

/* SRAM definitions */
#define RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR           g_SramPkaAddr

/* The maximal count of allowed sizes of PKA operands or register-variables */
#define RSA_PKA_MAX_COUNT_OF_REGS_SIZES                   8 /*PKA_MAX_COUNT_OF_REGS_SIZES */
#define RSA_PKA_MAX_COUNT_OF_ADDITIONAL_REGS		  10
#define RSA_PKA_SRAM_REGS_MEM_OFFSET_WORDS                RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR


/* PKA OPCODE register fields positions (low bit position) */
#define RSA_HW_PKI_PKA_OPCODE_TAG_POS                               0
#define RSA_HW_PKI_PKA_OPCODE_RESULT_POS                            6
#define RSA_HW_PKI_PKA_OPCODE_R_DISCARD_POS                        11
#define RSA_HW_PKI_PKA_OPCODE_OPERAND_2_POS                        12
#define RSA_HW_PKI_PKA_OPCODE_OPERAND_2_IMMED_POS                  17
#define RSA_HW_PKI_PKA_OPCODE_OPERAND_1_POS                        18
#define RSA_HW_PKI_PKA_OPCODE_OPERAND_1_IMMED_POS                  23
#define RSA_HW_PKI_PKA_OPCODE_LEN_POS                              24
#define RSA_HW_PKI_PKA_OPCODE_OPERATION_ID_POS                     27


/* PKA N_NP_T0_T1 register fields positions (low bit position) */
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_N_POS                         DX_N_NP_T0_T1_ADDR_N_VIRTUAL_ADDR_BIT_SHIFT
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_NP_POS                        DX_N_NP_T0_T1_ADDR_NP_VIRTUAL_ADDR_BIT_SHIFT
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_T0_POS                        DX_N_NP_T0_T1_ADDR_T0_VIRTUAL_ADDR_BIT_SHIFT
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_T1_POS                        DX_N_NP_T0_T1_ADDR_T1_VIRTUAL_ADDR_BIT_SHIFT

/* PKA N_NP_T0_T1 register default (reset) value: N=0, NP=1, T0=30, T1=31 */
#define PKA_N                                                    0UL
#define PKA_NP                                                   1UL
#define PKA_T0                                                  30UL
#define PKA_T1                                                  31UL
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_DEFAULT_VAL                  ( PKA_N  << RSA_HW_PKI_PKA_N_NP_T0_T1_REG_N_POS  | \
                                                                  PKA_NP << RSA_HW_PKI_PKA_N_NP_T0_T1_REG_NP_POS | \
                                                                  PKA_T0 << RSA_HW_PKI_PKA_N_NP_T0_T1_REG_T0_POS | \
                                                                  PKA_T1 << RSA_HW_PKI_PKA_N_NP_T0_T1_REG_T1_POS )

/* PKA control values  */
#define RSA_PKA_PIPE_READY                                   1
#define RSA_PKA_OP_DONE                                      1
#define RSA_PKA_SW_REST                                      1

/* Machine Opcodes definitions (according to HW CRS ) */
#define   PkaSepDone        0x00
#define   PkaAdd            0x04
#define   PkaAddIm          0x04
#define   PkaSub            0x05
#define   PkaSubIm          0x05
#define   PkaNeg            0x05
#define   PkaModAdd         0x06
#define   PkaModAddIm       0x06
#define   PkaModSub         0x07
#define   PkaModSubIm       0x07
#define   PkaModNeg         0x07
#define   PkaAND            0x08
#define   PkaTest0          0x08
#define   PkaClr0           0x08
#define   PkaClr            0x08
#define   PkaOR             0x09
#define   PkaCopy	    0x09
#define   PkaSetBit0        0x09
#define   PkaXOR  	    0x0A
#define   PkaFlip0	    0x0A
#define   PkaInvertBits     0x0A
#define   PkaCompare	    0x0A
#define   PkaSHR0	    0x0C
#define   PkaSHR1           0x0D
#define   PkaSHL0	    0x0E
#define   PkaSHL1	    0x0F
#define   PkaLMul	    0x10
#define   PkaModMul	    0x11
#define   PkaModMulNR	    0x12
#define   PkaModExp	    0x13
#define   PkaDiv 	    0x14
#define   PkaModInv 	    0x15
#define   PkaHMul	    0x17
#define   PkaModMulAcc	    0x18
#define   PkaModMulAccN     0x19
#define   PkaSepInt         0x1A
#define   PkaReduce         0x1B

#define   PKA_MAX_OPCODE    0x1B


/* **********************           MACROS          ******************************* */


/* Full register (operation) size including extra bits + one PKA-word (64 or  *
*  128 bits), rounded up to PKA words.                                        */
#define GET_FULL_OP_SIZE_PKA_WORDS(opSizeInBits)     (((opSizeInBits)/RSA_PKA_BIG_WORD_SIZE_IN_BITS + (((opSizeInBits) & (RSA_PKA_BIG_WORD_SIZE_IN_BITS-1)) > 0)) + 1)

/*************************************************************/
/* Macros for waiting PKA machine ready states               */
/*************************************************************/

/* defining a macro for waiting to the PKA_PIPE_READY */
#define RSA_PKA_WAIT_ON_PKA_PIPE_READY(VirtualHwBaseAddr) \
do \
{ \
   volatile uint32_t output_reg_val; \
   do \
   { \
	SB_HAL_READ_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_PIPE_RDY), output_reg_val ); \
   }while( (output_reg_val & 0x01) != RSA_PKA_PIPE_READY ); \
}while(0)

/* defining a macro for waiting to the PKA_OP_DONE */
#define RSA_PKA_WAIT_ON_PKA_DONE(VirtualHwBaseAddr) \
do \
{ \
   volatile uint32_t output_reg_val; \
   do \
   { \
	SB_HAL_READ_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_DONE), output_reg_val); \
   }while( (output_reg_val & 0x01) != RSA_PKA_OP_DONE ); \
}while(0)

/******************************************************************/
/* Macros for setting and reading sizes from PKA regsSizesTable   */
/******************************************************************/


/*  ReadRegSizeInTable:  Readss the size from regsSizesTable entry */
#define RSA_PKA_ReadRegSize(SizeBits, EntryNum, VirtualHwBaseAddr) \
	RSA_PKA_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
	SB_HAL_READ_REGISTER( (SB_REG_ADDR(VirtualHwBaseAddr,PKA_L0) + 4*(EntryNum)), \
                                (SizeBits) )

/******************************************************************/
/* Macros for setting and reading addresses of PKA data registers */
/******************************************************************/

/*The following macros are used for setting and reading the data registers addresses in mapping table.*/

/*  GetRegAddress:  Returns the physical address of register VirtReg from mapping table  */
#define RSA_PKA_GetRegAddress(VirtReg, VirtualHwBaseAddr) \
 ( *((volatile uint32_t*)(SB_REG_ADDR(VirtualHwBaseAddr,MEMORY_MAP0) + 4*(VirtReg))) )

/******************************************************************/
/*          Macros for setting Full PKI opcode                    */
/******************************************************************/

#define RSA_PKA_FullOpCode( Opcode,LenID,IsAImmed,OpA,IsBImmed,OpB,ResDiscard,Res,Tag ) \
   ( ((Opcode))      << RSA_HW_PKI_PKA_OPCODE_OPERATION_ID_POS     | \
     ((LenID))       << RSA_HW_PKI_PKA_OPCODE_LEN_POS              | \
     ((IsAImmed))    << RSA_HW_PKI_PKA_OPCODE_OPERAND_1_IMMED_POS  | \
     ((OpA))         << RSA_HW_PKI_PKA_OPCODE_OPERAND_1_POS        | \
     ((IsBImmed))    << RSA_HW_PKI_PKA_OPCODE_OPERAND_2_IMMED_POS  | \
     ((OpB))         << RSA_HW_PKI_PKA_OPCODE_OPERAND_2_POS        | \
     ((ResDiscard))  << RSA_HW_PKI_PKA_OPCODE_R_DISCARD_POS        | \
     ((Res))         << RSA_HW_PKI_PKA_OPCODE_RESULT_POS           | \
     ((Tag))         << RSA_HW_PKI_PKA_OPCODE_TAG_POS  )

/******************************************************************/
/*          Macros for reading and loading PKA memory data        */
/******************************************************************/

/* MACRO DEFINITIONS FOR WORKING WITH INDIRECT ACCESS TO SRAM PKA DATA REGISTERS */

	/*These registers  not included in the HW_CC definitions because they are HOST registers */
#ifdef CC_SB_INDIRECT_SRAM_ACCESS
/* MACRO DEFINITIONS FOR WORKING WITH INDIRECT ACCESS TO SRAM PKA DATA REGISTERS */

	#define RSA_PKA_SRAM_WRITE_CLR(VirtualHwBaseAddr, SizeWords) \
	for(; ii < 4*(((SizeWords)+3)/4) ; ii++) { \
		SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_WDATA), 0); \
	}

	#ifndef BIG__ENDIAN
	/* macro to load a value to SRAM with 32-bit access */
	#define RSA_HW_PKI_HW_LOAD_VALUE_TO_PKA_MEM(VirtualHwBaseAddr, Addr, Val) \
	do \
	{ \
	   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_ADDR), ((Addr)+ RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)); \
	   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_WDATA), (Val)); \
	}while(0)
	#else
	#define RSA_HW_PKI_HW_LOAD_VALUE_TO_PKA_MEM(VirtualHwBaseAddr, Addr, Val) \
	do \
	{ \
	   uint32_t tempVal; \
	   UTIL_ReverseMemCopy((uint8_t*)&tempVal , (uint8_t*)&Val , sizeof(uint32_t) ); \
	   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_ADDR), ((Addr)+ RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)); \
	   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_WDATA), (tempVal)); \
	}while(0)
	#endif

	#ifndef BIG__ENDIAN
	/* macro to load block to SRAM memory */
	#define RSA_HW_PKI_HW_LOAD_BLOCK_TO_PKA_MEM(VirtualHwBaseAddr, Addr, ptr, SizeWords) \
	do \
	{ \
	   uint32_t ii; \
	   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_ADDR), ((Addr)+RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)); \
	   for(ii = 0; ii < (SizeWords); ii++) \
	   { \
		   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_WDATA), ((uint32_t*)(ptr))[ii]); \
	   } \
	   RSA_PKA_SRAM_WRITE_CLR(VirtualHwBaseAddr, SizeWords) \
	}while(0)
	/* defining a macro to load a block to the PKA data memory */

	#else
#if 0
	#define RSA_HW_PKI_HW_LOAD_BLOCK_TO_PKA_MEM(VirtualHwBaseAddr, Addr, ptr, SizeWords) \
	do \
	{ \
	   uint32_t ii; \
	   uint32_t tempWord; \
	   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_ADDR), ((Addr)+RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)); \
	   for(ii = 0; ii < (SizeWords); ii++) \
	   { \
		   UTIL_ReverseMemCopy((uint8_t*)&tempWord , (uint8_t*)&(ptr)[ii] , sizeof(uint32_t) ); \
		   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_WDATA), tempWord); \
	   } \
	   RSA_PKA_SRAM_WRITE_CLR(VirtualHwBaseAddr, SizeWords) \
	}while(0)
	#endif
	#endif

        /* macro to clear PKA memory: Addr must be alighned to PKA_WORD size */
	#define RSA_HW_PKI_HW_CLEAR_PKA_MEM(VirtualHwBaseAddr, Addr, SizeWords) \
	do \
	{ \
	   uint32_t ii; \
	   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_ADDR), (Addr)); \
	   for( ii = 0; ii < (uint32_t)(SizeWords); ii++ ) \
	   { \
		SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_WDATA), 0x0UL ); \
	   }\
	   RSA_PKA_SRAM_WRITE_CLR(VirtualHwBaseAddr, (SizeWords)); \
	}while(0)

	#ifndef BIG__ENDIAN
	/* macro to read a value from the PKA data memory */
	#define RSA_HW_PKI_HW_READ_VALUE_FROM_PKA_MEM(VirtualHwBaseAddr, Addr, Val) \
	do \
	{ \
	   volatile uint32_t dummy; \
	   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RADDR), ((Addr)+ RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)); \
	   SB_HAL_READ_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RDATA), (dummy)); \
	   (Val) = dummy; \
	}while(0)
	#else
	#define RSA_HW_PKI_HW_READ_VALUE_FROM_PKA_MEM(VirtualHwBaseAddr, Addr, Val) \
	do \
	{ \
	   volatile uint32_t dummy; \
	   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RADDR), ((Addr)+ RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)); \
	   SB_HAL_READ_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RDATA), (dummy)); \
	   (Val) = dummy; \
	   UTIL_ReverseBuff((uint8_t*)&Val , sizeof(uint32_t) ); \
	}while(0)
	#endif

	#ifndef BIG__ENDIAN
	/* defining a macro to read a block from the PKA data memory */
	#define RSA_HW_PKI_HW_READ_BLOCK_FROM_PKA_MEM(VirtualHwBaseAddr, Addr, ptr, SizeWords) \
	do \
	{ \
	   uint32_t ii; \
	   volatile uint32_t dummy; \
	   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RADDR), (Addr)); \
	   for(ii = 0; ii < (SizeWords); ii++) \
	   { \
	      SB_HAL_READ_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RDATA), (dummy));\
	      ((uint32_t*)(ptr))[ii] = (dummy); \
	   } \
	}while(0)
	#else
#if 0
	#define RSA_HW_PKI_HW_READ_BLOCK_FROM_PKA_MEM(VirtualHwBaseAddr, Addr, ptr, SizeWords) \
	do \
	{ \
	   uint32_t ii; \
	   volatile uint32_t dummy; \
	   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RADDR), (Addr)); \
	   for(ii = 0; ii < (SizeWords); ii++) \
	   { \
	      SB_HAL_READ_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RDATA), (dummy));\
	      ((uint32_t*)(ptr))[ii] = (dummy); \
	      UTIL_ReverseBuff((uint8_t*)&(ptr)[ii] , sizeof(uint32_t) ); \
	   } \
	}while(0)
	#endif
	#endif

#else //CC_SB_INDIRECT_SRAM_ACCESS=0 => direct access

#if 0
    	/* MACRO DEFINITIONS FOR WORKING WITH INDIRECT ACCESS TO SRAM PKA DATA REGISTERS */

	/*These registers  not included in the HW_CC definitions because they are HOST registers */

	/* defining a macro to load a value to the PKA data memory */
	#define RSA_HW_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
	do \
	{ \
	   *(uint32_t*)(((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)) = (uint32_t)Val; \
	}while(0)


	/* defining a macro to load a block to the PKA data memory */
	#define RSA_HW_PKI_HW_LOAD_BLOCK_TO_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	    memmove((uint8_t*)((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),(uint8_t*)(ptr),(SizeWords)*sizeof(uint32_t)); \
	}while(0)


	/* defining a macro to clear PKA data memory */
	#define RSA_HW_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr , Addr , SizeWords ) \
	do \
	{ \
	    memset((uint8_t*)((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),0x0,(SizeWords)*sizeof(uint32_t)); \
	}while(0)


	/* defining a macro to read a value from the PKA data memory */
	#define RSA_HW_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
	do \
	{ \
	    Val = *(uint32_t*)((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR); \
	}while(0)


	/* defining a macro to read a block from the PKA data memory */
	#define RSA_HW_PKI_HW_READ_BLOCK_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
	do \
	{ \
	    memmove((uint8_t*)(ptr),(uint8_t*)((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),(SizeWords)*sizeof(uint32_t)); \
	}while(0)
#endif

#endif //CC_SB_INDIRECT_SRAM_ACCESS


//********************************************************************************************/
//RSA_PKI_PKA_H
/* values for defining, that PKA entry is not in use */ 
#define PKA_SIZE_ENTRY_NOT_USED  0xFFFFFFFF
#define PKA_ADDRESS_ENTRY_NOT_USED  0xFFC

/* define NpCreateFlag values */
#define PKA_CreateNP  1
#define PKA_SetNP     0

/* define "discard result" value */
#define RES_DISCARD  0x3F


/* default LenID-s definitions. Last 3 names used in RSA KG.  
   Note: extended sizes are in the entries LenID+1 (i.e. 1,3) accordingly */
#define LenIDn    0  /* for operation or modulus size*/
#define LenIDpq   2  /* for P, Q size */
#define LenID101  4  /* for P1,P2,Q1,Q2 101 bit size */
#define LenIDmin  6  /* for min allowed operation size (now 128 bit) */
#define LenIDmax  7  /* for max allowed operation size (now 2112 bit) */
                    
/* define of PKA Tag for debug printing of PKA operations result 
   into tempRes global buffer */
#define PKA_TAG_DebugPtint  1                   
                    

/************************ Enums ********************************/

/* defines enumerator for all allowed PKA operations */
typedef enum
{
	PKA_Terminate_ID    =  0,
	PKA_Add_ID          =  1,
	PKA_AddIm_ID        =  2,
	PKA_Sub_ID          =  3,
	PKA_SubIm_ID        =  4, 
	PKA_Neg_ID          =  5,

	PKA_ModAdd_ID       =  6,
	PKA_ModAddIm_ID     =  7,
	PKA_ModSub_ID       =  8,
	PKA_ModSubIm_ID     =  9,
	PKA_ModNeg_ID       = 10,

	PKA_And_ID          = 11,
	PKA_Tst0_ID         = 12,
	PKA_Clr0_ID         = 13,
	PKA_Clr_ID          = 14, 
	
	PKA_OR_ID           = 15,
	PKA_Copy_ID         = 16,
	PKA_SetBit0_ID      = 17,
	
	PKA_XOR_ID          = 18, 
	PKA_Flip0_ID        = 19,
	PKA_InvBits_ID      = 20,
	PKA_Compare_ID      = 21,
	
	PKA_SHR0_ID         = 22,
	PKA_SHR1_ID         = 23,
	PKA_SHL0_ID         = 24,
	PKA_SHL1_ID         = 25,
	        
	PKA_LMul_ID         = 26,
	PKA_HMul_ID         = 27,
	PKA_ModMul_ID       = 28,
	PKA_ModMulNR_ID     = 29,
	PKA_ModExp_ID       = 30,
	
	PKA_Div_ID          = 31,
	PKA_ModInv_ID       = 32,	
	
	PKA_OpOffMode		        
		
} RSA_HW_PKI_PKA_OpID_t;


/*  Enumerator, defining whether the functions performs default settings 
    of PKI registers, or takes they from user passed arguments  */
typedef enum
{
      DefaultRegs       = 0,
      SetRegs           = 1,
      RegsSetOffMode
      
} RSA_HW_PKI_PKA_SetRegs_t;


/************************ Typedefs  ****************************/

/************************ Typedefs  ****************************/

/************************ Structs  ******************************/

/* definition of the Registers sizes array type, used for initialisation 
   of the PKA sizes table */
typedef uint32_t RSA_HW_PKI_PKA_RegsSizes_t[RSA_PKA_MAX_COUNT_OF_REGS_SIZES];               

 /* This structure contains virtual pointers (numbers in range 0-31) of 
    PKI registers RegsNum[]  and registers physical addresses RegsAddr[]. 
    For example: RegsNum[3] = 5 and RegsAddr[3] = 0x400 means, that address of 
    register 5 is 0x400. The user may set into this structure only needed registers, 
    used in planned PKA operations. */

typedef  struct  
{
       uint32_t    RegsNum[RSA_HW_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS] ;
       uint32_t    RegsAddr[RSA_HW_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS] ;
}RSA_HW_PKI_PKA_RegsMap_t;

/************************ Public Variables **********************/

/************************ Public Functions ******************************/


/***********************************************************************************************/
/***********************************************************************************************/
/*                                                                                             */ 
/*                   RSA PKI FUNCTIONS, PERFORMING PKA OPERATIONS                              */
/*                                                                                             */
/***********************************************************************************************/
/***********************************************************************************************/



/*******************************************************************************/
/**********      RSA PKI PKA initialisation functions and macros      **********/
/*******************************************************************************/

/***********      PkaSetRegsSizesTab function      **********************/
/**
 * @brief This function initializes the PKA registers sizes table. 
 *
 *      The function sets sizes table as follows:
 *            -  tab[0] = MaxSizeBits; //maximal size, usually this is exact modulus size in bits
 *            -  tab[1] = Extended size with extra bits, aligned to big words.
 *            -  other entrie,
		uint32_t  Xs = PKA_SIZE_ENTRY_NOT_USED, means - not used. 
 *
 * @param[in] opSizeInBits - Size of PKA operations (modulus) in bits. The value must be in interval 
 *                          from defined Min. to Max. size bits.   
 * @param[in] regSizeInPkaWords - Sise of registers in PKA big words (e.g. 128-bit words).
 *
 * @return - no return value	
 *
 */
void RSA_PKA_SetRegsSizesTab( uint32_t     opSizeInBits,       
			      int32_t      regSizeInPkaWords,
			      unsigned long  VirtualHwBaseAddr);                                                       
                                                            
 
/***********      PkaSetRegsMapTab function      **********************/
/**
 * @brief This function initializes the PKA registers sizes table. 
 *
 *   The function checks input parameters and sets the physical memory registers mapping-table 
 *   according to parameters, passed by the user: 
 *     - start address of register 0 is the start address of PKA data registers memory 
 *       PKA_SRAM_REGS_MEM_OFFSET_WORDS (defined in pka_hw_defs.h file); 
 *     - special registers are set as follows: N=0,NP=1,T0=30,T1=31;
 *     - all registers have the same size, equalled to given size;
 *
 * @param[in] countOfRegs  - The count of registeres, requirred by the user.
 * @param[in] regSizeInPkaWords - Sise of registers in PKA big words (e.g. 128-bit words).
 * @param[in] VirtualHwBaseAddr - ARM CryptoCell base address
 *
 * @return - no return value	
 *
 */
void RSA_PKA_SetRegsMapTab(int32_t   countOfRegs,   
		       int32_t   regSizeInPkaWords,
		       unsigned long  VirtualHwBaseAddr);
                          


/***********      PkaInitPka function      **********************/
/**
 * @brief This function initializes the PKA engine. 
 *
 *    The function performs the following:
 *      - initializes the PKA_SizesTable, PKA_MappingTable and special register 
 *        N_NP_T0_T1 according to user passed register sizes, registers mapping 
 *        and default N_NP_T0_T1 value.
 *        
 *    The function calls the PkaSetRegsSizesTab  and PkaSetRegsMapTab 
 *    functions and sets N_NP_T0_T1 value into N_NP_T0_T1 register.
 *    Notes: 
 *            - See remarks to PkaSetRegsSizesTab and PkaSetRegsMapTab functions.
 *            - The function allocates one additional word for each register if it is needed for extra bits. 
 *
 * 
 * @return void
 *
 */
 void RSA_PKA_InitPka(uint32_t   opSizeInBits,	/*in*/
                      uint32_t   regsCount,     /*in*/
                      unsigned long  virtualHwBaseAddr)	/*out*/;  
 

/***********      RSA_HW_PKI_PKA_FinishPKA function      **********************/
/**
 * @brief This function ends the PKA engine session and disables PKA clocks. 
 *
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void RSA_HW_PKI_PKA_FinishPKA( unsigned long VirtualHwBaseAddr /*in*/ );
 
 
 			    
/*******************************************************************************/
/************    RSA PKI PKA mathmatic functions and macros     ****************/
/*******************************************************************************/
                               
/***********    PkaDbgExecOperation (with virtual pointers)     ******************/
/**
 * @brief This function executes any allowed PKA mathematic operation according to 
 *        user passed Opcode.
 *
 *        The function receives code of operation, virtual pointers to PKI registers  
 *        (sequence number), for arguments and result, and operates PKA machine by writing  
 *        full operation code into OPCODE register. Then the function calls macros for  
 *        waiting the PKA pipe ready signal.
 *        If opcode is illegal or one of operands is illegal, the function returns an 
 *        error code defined in llf_pki_error.h file.
 *
 *        The user don't call this function directly. For user convenience, in llf_pki.h file  are 
 *        given some macros for calling this function according to each performed operation.  
 *
 *     NOTES:  
 *       -	Before executing modular operations, the modulus must be set into N=r0 register of PKA.
 *       -	Before modular multiplication and exponentiation must be calculated and set into NP=r1 
 *          register the Barrett modulus tag NP = 2**(sizeN+132) / N.   
 *       -	In operations with immediate operands (IsImmediate bit = 1), the operand value (5-bit)
 *          is treated as sign-extended. That means: low 4 bits are treated as unsigned operand 
 *          value in range 0-15 and bit 5 is a sign (with extension to all high bits of register, 
 *          in which the full operand shall be set).
 *       -	In shift operations the 5-bits shift operand is treated as unsigned value in range 0-31
 *          (count of shifts is equaled to shift operand value + 1).
 *       -  The LMul operation gives the low half of multiplication result of length equaled to 
 *          operation size. The leading not significant bits of the operands and result (including 
 *          the the extra word) must be zeroed.  
 *       -  The HMul operation gives the high half of multiplication result plus one high word of low 
 *          half of full multiplication result. Therefore this result is by one word large, than 
 *          operation size. The leading not significant bits of the operands and result, 
 *          including extra word must be zeroed. 
 *       -  The ModInv operation calculates Res = 1/OpB mod N for odd modulus. Operand A is ignored.
 *          In case of even modulus the function returns an error. Therefore in this case 
 *          (also for odd modulus) the user may call the PkaExecFullModInv function.  
 *
 * @param[in] Opcode   - The operation code according HW PKA definitions. Valid values: 0 - max Opcode.
 * @param[in] LenID    - ID of the length of operands according to register sizes table 
 *                       (means the number of entry in the table). Valid values: 0...7.
 * @param[in] IsAImmed - If IsAImmed = 1, then operand A treated as immediate value, else - 
 *                       as virtual register pointer. Valid values:	0,1.
 * @param[in] OpA      - Operand A: an immediate value or virtual register pointer, according to IsAImmed 
 *                       IsAImmed parameter. Valid values: 0 <= OpA <= 31. 
 * @param[in] IsBImmed - If IsBImmed = 1, then operand B treated as immediate value, else - 
 *                       as virtual register pointer. Valid values:	0,1.
 * @param[in] OpB      - Operand B: an immediate value or virtual register pointer, according to IsAImmed 
 *                       IsBImmed parameter. Valid values: 0 <= OpA <= 31.
 * @param[in] ResDiscard -	If ResDiscard = 1, then result is discarded.
 * @param[in] Res        - Virtual register pointer for result data. 
 *                         Valid values: 0 <= Res <= 31. Value Res = RES_DISCARD means result must be discarded.
 * @param[in] Tag        - The user defined value (Tag <= 31), used for indication goals. 
 *   
 *
 * @return CCError_t - On success CC_OK is returned, on failure an error code:
 *                       PKA_ILLEGAL_OPCODE_ERROR
 *                       PKA_ILLEGAL_OPERAND_LEN_ERROR
 *                       PKA_ILLEGAL_OPERAND_TYPE_ERROR
 *                       PKA_ILLEGAL_OPERAND_ERROR
 *                       PKA_INVERSION_NOT_EXISTS_ERROR
 */


CCError_t _RSA_PKA_ExecOperation( uint32_t      Opcode,		 /*in*/ 
				uint8_t       LenID,		 /*in*/ 
				uint8_t       IsAImmed,		 /*in*/
				int8_t        OpA,		 /*in*/
				uint8_t       IsBImmed,		 /*in*/
				int8_t        OpB,		 /*in*/ 
				uint8_t       ResDiscard,	 /*in*/
				int8_t        Res,		 /*in*/
				uint8_t       Tag, 		 /*in*/
			        unsigned long      VirtualHwBaseAddr);

/*************************************************************************/ 
/* Macros for calling PKA operations (names according to operation issue */
/*************************************************************************/
 
/*----------------------------------*/
/*   1.  ADD - SUBTRACT operations  */
/*----------------------------------*/

/*  Add:   Res =  OpA + OpB  */
#define   RSA_PKA_Add(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaAdd,(LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  AddIm:  Res =  OpA + OpBIm  */
#define   RSA_PKA_Add_IM(LenID, OpA, OpBIm, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaAddIm,(LenID), 0, (OpA), 1, (OpBIm), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  SubIm:  Res =  OpA - OpBIm  */
#define   RSA_PKA_SUB_IM(LenID, OpA, OpBIm, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaSubIm,(LenID), 0, (OpA), 1, (OpBIm), 0, (Res), (Tag), (VirtualHwBaseAddr) )

/*----------------------------------*/
/*   2.  Logical   operations       */
/*----------------------------------*/

/*  Clr:  Res =  OpA & 0  - clears the operand A.  */
#define   RSA_PKA_Clr(LenID, OpA, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaAND, (LenID), 0, (OpA), 1, 0x00, 0, (OpA), (Tag), (VirtualHwBaseAddr))

/*  Clear:  for full clearing the actual register opA, this macro calls Clr operation twice.  */
#define   RSA_PKA_Clear(LenID, OpA, Tag, VirtualHwBaseAddr)   \
            RSA_PKA_Clr(LenID, OpA, Tag, VirtualHwBaseAddr);  \
            RSA_PKA_Clr(LenID, OpA, Tag, VirtualHwBaseAddr)
            
/*  Set0:  Res =  OpA || 1  : set bit0 = 1, other bits are not changed */
#define   RSA_PKA_Set0(LenID, OpA, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation(PkaOR, (LenID), 0, (OpA), 1, 0x01, 0, (Res), (Tag), (VirtualHwBaseAddr))

/*-----------------------------------------------------*/
/*   2.  Multiplication and other   operations         */
/*       Note:  See notes to PKA_ExecOperation */
/*-----------------------------------------------------*/
#define   RSA_PKA_Add(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaAdd,(LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )

/*  ModExp:  Res =  OpA ** OpB  mod N - modular exponentiation */
#define   RSA_HW_PKI_PKA_ModExp(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
             _RSA_PKA_ExecOperation( PkaModExp, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr))

/*  Copy:  OpDest =  OpSrc || 0  */
#define   RSA_PKA_Copy(LenID, OpDest, OpSrc, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation(PkaOR, (LenID), 0, (OpSrc), 1, 0x00, 0, (OpDest), (Tag), (VirtualHwBaseAddr))

/*  SHR0:  Res =  OpA >> (S+1) :   shifts right operand A by S+1 bits, insert 0 to left most bits */
#define   RSA_PKA_SHR0(LenID, OpA, S, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaSHR0, (LenID), 0, (OpA), 0, (S), 0, (Res), (Tag), (VirtualHwBaseAddr))

/*  SHL0:  Res =  OpA << (S+1) :   shifts left operand A by S+1 bits, insert 0 to right most bits */
#define   RSA_PKA_SHL0(LenID, OpA, S, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation(PkaSHL0, (LenID), 0, (OpA), 0, (S), 0, (Res), (Tag), (VirtualHwBaseAddr))

/*  Divide:  Res =  OpA / OpB , OpA = OpA mod OpB - division,  */
#define   RSA_PKA_Div(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaDiv, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag),  (VirtualHwBaseAddr))



#define RSA_PKA_WRITE_WORD_TO_REG(Val,i,VirtReg, VirtualHwBaseAddr) {\
    uint32_t  currAddr;\
    RSA_PKA_WAIT_ON_PKA_DONE(VirtualHwBaseAddr); \
    currAddr = RSA_PKA_GetRegAddress(VirtReg, VirtualHwBaseAddr);\
    RSA_HW_PKI_HW_LOAD_VALUE_TO_PKA_MEM(VirtualHwBaseAddr, currAddr+(i), (Val));\
}


/* ***************************************************************************** */
/* ************   Auxiliary functions used in PKA               **************** */
/* ***************************************************************************** */

 
/***********      RSA_HW_PKI_PKA_ClearBlockOfRegs function      **********************/
/**
 * @brief This function clears block of PKA registers. 
 *
 *        Assumings: - PKA is initialised.
 *                   - Length of extended (by word) registers is plased into LenID entry of 
 *                     sizes table.
 *
 * @param[in] firstReg    - Virtual address (number) of first register in block.
 * @param[in] countOfRegs - Count of registers to clear.
 * @param[in] LenId       - ID of entry of regsSizesTable defines registers lengts with word extension.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 */
 void RSA_HW_PKI_PKA_ClearBlockOfRegs( 
                                uint8_t  firstReg,          /*in*/
                                uint8_t  countOfRegs,       /*in*/
                                uint8_t  LenID,             /*in*/
                                unsigned long VirtualHwBaseAddr  /*in*/ ); 
                                
                                
 
/***********      RSA_HW_PKI_PKA_CopyDataIntoPkaReg function      **********************/
/**
 * @brief This function  copies source data into PKA register . 
 *
 *        Assumings: - PKA is initialised.
 *                   - Length of extended (by word) registers is plased into LenID entry of 
 *                     sizes table.
 *                   - If the extra word of register must be cleared also the user must 
 *                     set LenID according to extended register size
 *
 * @param[in] dstReg       - Virtual address (number) of destination register.
 * @param[in] LenId        - ID of entry of regsSizesTable defines registers lengts with word extension.
 * @param[in] src_ptr      - Pointer to source buffer.
 * @param[in] sizeBytes    - Data size in bytes.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void RSA_HW_PKI_PKA_CopyDataIntoPkaReg(uint32_t    dstReg,	  /*out*/
				       uint32_t    LenID,	  /*in*/
				       const  uint32_t  *src_ptr, /*in*/
				       uint32_t    sizeWords,  /*in*/
				       unsigned long  VirtualHwBaseAddr); 
                                
                                
                          
                                
/***********      RSA_HW_PKI_PKA_CopyDataFromPkaReg      **********************/
/**
 * @brief This function copies data from PKA register into output buffer . 
 *
 *        Assumings: - PKA is initialised.
 *                   - Length of extended (by word) registers is plased into LenID entry of 
 *                     sizes table.
 *                   - If the extra word of register must be cleared also the user must 
 *                     set LenID according to extended register size
 *
 * @param[in] srcReg       - Virtual address (number) of source PKA register.
 * @param[in] dst_ptr      - Pointer to destination buffer.
 * @param[in] sizeBytes - Source size in bytes.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void RSA_HW_PKI_PKA_CopyDataFromPkaReg(uint32_t *dst_ptr,		/*out*/
			    uint32_t  sizeWords,		/*in*/
			    uint32_t  srcReg,		/*in*/
			    unsigned long  VirtualHwBaseAddr); 
                                                    

 /***********      RSA_HW_PKI_SET_MSB_ADDR      **********************/
/**
 * @brief This function sets the PKI MSB register according ot the SRAM address . 
 *
 *        Assumings: - PKA registers are accessable.
 *                   - SRAM address is multiple of 4KB.
 *
 * @param[in] pkaAdrress       - base address of the SRAM for PKA.
 *
 * @return - no return parameters.
 *
 */
void RSA_HW_PKI_SET_MSB_ADDR(unsigned long virtualAddress, uint32_t pkaAdrress);

/**
 * The function uses physical data pointers to calculate  
 * the Barrett tag Np. Np is saved into Register indexed by regNp.
 *  
 *  For RSA it uses truncated sizes:
 *      Np = truncated(2^(3*A+3*X-1) / ceiling(n/(2^(N-2*A-2*X)));
 *  
 *      function assumes modulus in PKA reg 0, and output is to PKA reg 1
 *  
 */           
uint32_t  RSA_HW_PKA_CalcNpIntoPkaReg(uint32_t lenId, 
				uint32_t    sizeNbits, 
				int8_t      regN, 
				int8_t      regNp,   // out
				int8_t      regTemp1, 
				int8_t      regTempN,
				unsigned long VirtualHwBaseAddr) ;

//********************************************************************************************/
//
