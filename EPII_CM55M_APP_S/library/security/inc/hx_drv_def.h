#ifndef _HX_DRV_DEF_H_
#define _HX_DRV_DEF_H_


//#include <ctype.h>
//#include "ARMCM55.h"
//#include "cli_debug.h"
//#include "cli_type.h"
//#include "cli_console.h"
//#include "cli_config.h"

//#include "board.h"
#include "xprintf.h"

#define BOOL_T  bool
#define TRUE    true

//#include "cc_pka_hw_plat_defs.h"

/* system init header */
/* CryptoCell library header that contains all of the
   CryptoCell library basic APIs, their enums and definitions */
#include "cc_lib.h"

// CryptoCell random-number generation APIs and definitions
#include "cc_rnd_common.h"

/* mbedTLS APIs and definitions for entropy accumulator and
   CTR-DRBG required for CryptoCell library initialisation */
#include "entropy.h"
#include "ctr_drbg.h"


/* mbedtls lib */
/* AES ECB,CBC,CTR,OFB */
#include "aes.h"
#include "aes_driver.h"
#include "driver_defs.h"

#include "aesccm_driver.h"
#include "aesgcm_driver.h"
#include "hash_driver.h"

/* aes ae */
#include "gcm.h"
#include "ccm.h"
#include "mbedtls_cc_ccm_star.h"

/* hash */
#include "mbedtls_hash_common.h"


#include "sha1.h"
#include "sha256.h"
#include "rsa.h"


#include "md.h"
#include "bignum.h"


/* rsa */
#include "rsa_alt.h"


/* kdf */
#include "mbedtls_cc_util_defs.h"
#include "mbedtls_cc_util_key_derivation.h"


#include "cmac.h"

/* ecc */
#include "ecdsa.h"
#include "ecp.h"
#include "asn1.h"



#include "mbedtls_cc_ecdsa_edwards.h"

/* trng */
//#include "cc_rnd_common.h"
#include "cc_rnd_local.h"
#include "cc_rng_plat.h"


//#include "ctr_drbg.h"


#include "llf_rnd_trng.h"

/* hx debug utilty */
#include "hx_debug_util.h"


//#define SB_2LV
//#define Secure_Debug
/* for verification only */
#define PRINTBOOTPLAIN

typedef struct {
    uint32_t key1Cert_size;
	#ifndef SB_2LV
    uint32_t key2Cert_size;
	#endif
    uint32_t contCert_size;
    uint32_t bootImage_size;
#ifdef PRINTBOOTPLAIN
    uint32_t bootImagePlain_size; //(do not need)
#endif
	uint32_t chain_size;
}secBoot_CertChain_t;


#if 0
#include "bootimagesverifier_def.h"
#include "secureboot_defs.h"
#include "mbedtls_cc_sbrt.h"
#endif

#define RUNIT_SECURE_BOOT__KEY_PKG_1_FLASH_ADDRESS          0x00000000UL
#define RUNIT_SECURE_BOOT__KEY_PKG_2_FLASH_ADDRESS          0x00001000UL
#define RUNIT_SECURE_BOOT__CONTENT_PKG_1_FLASH_ADDRESS      0x00002000UL
#define RUNIT_SECURE_BOOT__IMAGE_PKG_1_FLASH_ADDRESS        0x00003000UL
#define RUNIT_SECURE_BOOT__IMAGE_PKG_2_FLASH_ADDRESS        0x00004000UL
#define RUNIT_SECURE_BOOT__IMAGE_PKG_3_FLASH_ADDRESS        0x00005000UL
#define RUNIT_SECURE_BOOT__IMAGE_PKG_3_NEW_FLASH_ADDRESS    0x00006000UL


#define SECURE_BOOT_KEY_FLASH_ADDRESS         				0x00000000UL

#define EPII_SECURE_INFO_BASE			(0x34000000)//BASE_ADDR_SRAM0_ALIAS

/**
 * Error Codes
 */
typedef enum SecError_t
{
    SEC_ERROR_OK = 0,
    SEC_ERROR_WC_OTP,
    SEC_ERROR_NOT_SUPPORT,
    SEC_ERROR_FAIL = 0x0000FFFF,
}SecError_t;

typedef enum
{
	SEC_BOOT_VERIFY_SUCCESS = 0,	/**< No ERROR */
	SEC_BOOT_NEED_WRITE_FLASH,
	SEC_BOOT_SECURE_DISABLE,
	SEC_BOOT_ERROR_INVALID_PARA,
	SEC_BOOT_ERROR_PUBKEY_VERIFY,
	SEC_BOOT_ERROR_KEY_DERIVE,
	SEC_BOOT_ERROR_IMAGE_DECRYPT,
	SEC_BOOT_ERROR_IMAGE_VERIFY,
	SEC_BOOT_ERROR_IMAGE_REENCRYT,
	SEC_BOOT_ERROR_CERTI_VERIFY,
	SEC_BOOT_ERROR_ASSET_DEL,
	SEC_BOOT_ERROR_INTERNAL,
	SEC_BOOT_ERROR_NOT_INITIAL,
	SEC_BOOT_ERROR_SEC_ATTRIBUTE_ERROR,
}SEC_BOOT_ERROR;

typedef enum 	
{	
	CMD_TYPE_NOP,
	CMD_TYPE_CRYPTO,
	CMD_TYPE_ASYM_CRYPTO,
	CMD_TYPE_CRYPTO_AE,//for AES-GCM,AES-CCM,ARIA-CCM,ARIA-GCM,AEAD-ChaCha20-Poly1305
	CMD_TYPE_KEY_WRAP,
	CMD_TYPE_HASH,
	CMD_TYPE_MAC_UPDATE,
	CMD_TYPE_MAC_GENERATE,
	CMD_TYPE_MAC_VERIFY,
	CMD_TYPE_ASSET_SEARCH,
	CMD_TYPE_ASSET_CREATE,
	CMD_TYPE_ASSET_LOAD_IMPORT,
	CMD_TYPE_ASSET_LOAD_DERIVE,
	CMD_TYPE_ASSET_LOAD_PLAINTEXT,
	CMD_TYPE_ASSET_LOAD_RANDOM,
	CMD_TYPE_ASSET_LOAD_AESUNWRAP,
	CMD_TYPE_ASSET_PUBLIC_DATA_READ,
	CMD_TYPE_ASSET_DEL,
	CMD_TYPE_SIGN_VERIFY_NO_ASSET,
	CMD_TYPE_SIGN_WITH_ASSET,       // chagne to CMD_TYPE_SIGN ?
	CMD_TYPE_VERIFY_WITH_ASSET,     // chagne to CMD_TYPE_VERIFY ?
	CMD_TYPE_GEN_PUBKEY,
	CMD_TYPE_GEN_KEYPAIR,
	CMD_TYPE_ECDHE_GEN_SHAREKEY,
	CMD_TYPE_OTP_ON_OFF,
	CMD_TYPE_OTP_READ,
	CMD_TYPE_OTP_WRITE,
	CMD_TYPE_OTP_Cmpu,
	CMD_TYPE_OTP_Dmpu,
	CMD_TYPE_OTP_WC_READ,
	CMD_TYPE_OTP_WC_WRITE,
	CMD_TYPE_OTP_MONOTONIC_READ,
	CMD_TYPE_OTP_MONOTONIC_INCR,
	CMD_TYPE_OTP_MONOTONIC_READ_CM,
	CMD_TYPE_OTP_MONOTONIC_INCR_CM,
	CMD_TYPE_OTP_MONOTONIC_READ_DM,
	CMD_TYPE_OTP_MONOTONIC_INCR_DM,
	CMD_TYPE_OTP_RANDOMHUK,
	CMD_TYPE_TRNG_CONFIG,
	CMD_TYPE_TRNG_RANDOM,
	CMD_TYPE_AUTH_UNLOCK_START,
	CMD_TYPE_AUTH_UNLOCK_VERIFY,
	CMD_TYPE_SECURE_DEBUG,
	CMD_TYPE_SECURE_DEBUG_MOVE_RMA,
	CMD_TYPE_SYSTEM_RESET,
	CMD_TYPE_SYSTEM_INFOR,
	/* ARM only */
    CMD_TYPE_KEY_DERIVE,
    CMD_TYPE_GEN_KEYPAIR_ECC,
    CMD_TYPE_GEN_KEYPAIR_RSA,
    CMD_TYPE_SIGN_VERIFY_RSA,
    CMD_TYPE_SIGN_VERIFY_ECC,
    CMD_TYPE_CRYPTO_DMA,
    CMD_TYPE_GEN_SOCID,
}SEC_CMD_TYPE;	


typedef enum {
    CRYPTO_AES,
    CRYPTO_AES_CCM,
    CRYPTO_AES_CCM_STAR,
    CRYPTO_AES_GCM,
    CRYPTO_CHACHA,
} CRYPTO_TYPE;

typedef struct {
    /* overwrite crypto mode (optional in AES, used for secure boot, run AES+HASH) */
    bool                enHash;
    unsigned int        crypto_ctl_mode;
    hashMode_t          hashMode;
    unsigned char      *digest;
} AesAndHash_t;

typedef struct {
    AesContext_t        ctx;
    unsigned int        keybits;
    size_t              length;
    size_t              *iv_off;
    const unsigned char *input;
    unsigned char       *output;
    /* overwrite crypto mode (optional in AES, used for secure boot, run AES+HASH) */
    AesAndHash_t        hash;
} CryptoCtx_AES_t;

typedef struct {
    AesCcmContext_t     ctx;
    unsigned int        keybits;
    const unsigned char *input;
    size_t              inputLen;
    unsigned char       *output;
    const unsigned char *ivBuf;
    const unsigned char *AD;
    size_t              adLen;
    unsigned char       *randomSrc;
    uint32_t            frameCounter;
} CryptoCtx_AES_CCM_t;

typedef struct {
    AesGcmContext_t     ctx;
    unsigned int        keybits;
    const unsigned char *ivBuf;
    const unsigned char *addBuf;
    const unsigned char *tagBuf;
    const unsigned char *input;
    unsigned char       *output;
} CryptoCtx_AES_GCM_t;

typedef struct {
    void                *cryptoCtx;       // CryptoCtx_AES_CCM, CryptoCtx_AES_GCM
    CRYPTO_TYPE         crypto_type;
} CryptoInfo_AES_AE_t;

typedef struct {
    HashContext_t       ctx;
    const unsigned char *input;
    size_t              inputLen;
    unsigned char       *output;
} HashInfo_t;

typedef struct {
    mbedtls_mpi *n;
    mbedtls_mpi *e;
}RSAPubKey_t;

typedef struct {
    mbedtls_mpi *p;
    mbedtls_mpi *q;
    mbedtls_mpi *n;
    mbedtls_mpi *e;
    mbedtls_mpi *d;
}RSAPrvKey_t;

typedef struct {
    unsigned int nbits;     /*<! RSA modulus size in bits */
    int         pubExp;     /*<! public exponent value */
    RSAPubKey_t pubKey;     /*<! output public key */
    RSAPrvKey_t prvKey;     /*<! output private key */
}RSAKeyGenPair_t;

typedef enum {
    RSA_ENC,
    RSA_DEC,
} RSA_ENCDEC_TYPE;

typedef enum {
    RSA_SIGN,
    RSA_VERIFY,
} RSA_SIGNVEIFY_TYPE;

typedef struct {
    mbedtls_rsa_context ctx;
    RSA_ENCDEC_TYPE type;           //RSA_ENC, //RSA_DEC
//    int (*f_rng)(void *, unsigned char *, size_t);
//    void *p_rng;
    int mode;           //MBEDTLS_RSA_PUBLIC or MBEDTLS_RSA_PRIVATE
    RSAPubKey_t pubKey;
    RSAPrvKey_t prvKey;
    size_t inputLen;
    const unsigned char *input;
    size_t olen;
    size_t outputLen;
    unsigned char *output;
} RSAEncDecInfo_t;

typedef struct {
    mbedtls_rsa_context ctx;
    RSA_SIGNVEIFY_TYPE  type;           //RSA_SIGN, //RSA_VERIFY
//    int (*f_rng)(void *, unsigned char *, size_t);
//    void *p_rng;
    int mode;           //MBEDTLS_RSA_PUBLIC or MBEDTLS_RSA_PRIVATE
    mbedtls_md_type_t  md_alg;
    RSAPubKey_t pubKey;
    RSAPrvKey_t prvKey;
    size_t hashlen;
    unsigned char *hash;
    size_t msglen;
    const unsigned char *msg;
    unsigned char *sig;     //signature
} RSASignVerifyInfo_t;

typedef enum {
    NON_EXT_EXP,
    EXTRACT_EXPAND,
} KDF_TYPE;

typedef struct {
    cryptoKeyType_t         keyType;
    mbedtls_util_keydata    *pUserKey;
    mbedtls_util_prftype_t  prfType; //(CC_UTIL_PRF_HMAC)
    CCHashOperationMode_t   hashMode;
    const uint8_t           *pLabel;
    size_t                  labelSize; 
    const uint8_t           *pContextData; 
    size_t                  contextSize; 
    uint8_t                 *pDerivedKey;
    size_t                  derivedKeySize;
} KDFDataInfo_t;

typedef struct {
    const mbedtls_md_info_t *md;
    const unsigned char *salt;
    size_t salt_len;
    const unsigned char *ikm;
    size_t ikm_len;
    const unsigned char *info;
    size_t info_len;
    unsigned char *okm;
    size_t okm_len;
} HKDFDataInfo_t;

typedef struct {
    void        *pKDF;
    KDF_TYPE    kdf_type; 
} KDFInfo_t;

typedef enum {
    MAC_HMAC,
    MAC_CMAC,
} MAC_TYPE;

typedef struct {
    MAC_TYPE                 macType;
    const unsigned char     *key;
    size_t                   keylen;
    const unsigned char     *input;
    size_t                   ilen;
    unsigned char           *output; 
    mbedtls_md_type_t        mdType;      //HMAC use
//    mbedtls_cipher_type_t    cipher_type; //CMAC use
} MACDataInfo_t;

typedef enum {
    NON_EDWARD,
    EDWARD,
} ECCKEY_TYPE;

typedef struct {
    ECCKEY_TYPE eccKeyType;
    mbedtls_ecdsa_context ctx;
    mbedtls_ecp_group_id gid;
//    int (*f_rng)(void *, unsigned char *, size_t);
//    void *p_rng;
} ECCKeyPair_t;

typedef enum {
    ECDSA_SIGN,
    ECDSA_DET_SIGN,
    ECDSA_Edwards_SIGN,
    ECDSA_VERIFY,
    ECDSA_Edwards_VERIFY,
} ECDSA_TYPE;

typedef struct {
    ECDSA_TYPE ecdsaType;
    mbedtls_ecdsa_context *ctx;     //ecc key pair
    mbedtls_md_type_t  md_alg;
    //const unsigned char *hash;
    //size_t  hlen;
    const unsigned char *msg;
    size_t  msgLen;    
    unsigned char *sig;
    size_t slen;
} ECDSADataInfo_t;

typedef struct {
    uint8_t *output;
    uint32_t size;
} TRNGInfo_t;

typedef struct {
    uint32_t inputAddr;
    uint32_t outputAddr;
    uint32_t size;
} CryptoDMAInfo_t;

typedef enum otp_ahb_wc_partition
{
	OTP_AHB_PARTITION_SW = 0,
	OTP_AHB_PARTITION_HW,				
	OTP_AHB_PARTITION_MAGIC	
}otp_ahb_wc_partition;
typedef struct
{
	otp_ahb_wc_partition otp_partition;
	uint32_t offset;
	uint32_t *Data_p;
} otp_wc_read;

typedef struct
{
	otp_ahb_wc_partition otp_partition;
	uint32_t offset;
	uint32_t otp_Write_value;
} otp_wc_write;

typedef enum BsvItRunMode_t
{
    BSVIT_RUNMODE__CM,
    BSVIT_RUNMODE__DM,
    BSVIT_RUNMODE__SECURE,
    BSVIT_RUNMODE__RMA,
    BSVIT_RUNMODE__ALL,

    BSVIT_RUNMODE__MAX = BSVIT_RUNMODE__ALL
}BsvItRunMode_t;

typedef enum { 
    SEC_VIOLATION = -1,
    SECURE_MEM = 0,
    NON_SECURE_MEM,
} SECMEM_t;

typedef enum{
    SLEEP = 0,
    ACTIVE,
    POFF,
    PON,
}HARD_MACRO_STATE;

typedef struct {
    uint32_t signature;
    uint8_t hbk_id;
}SecAttrInfo_t;


#endif //_HX_DRV_DEF_H_

