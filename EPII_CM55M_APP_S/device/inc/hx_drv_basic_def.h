#ifndef INCLUDE_HX_DRV_BASIC_DEF_H
#define INCLUDE_HX_DRV_BASIC_DEF_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define SEC_ENCRYPT 1
#define SEC_DECRYPT 0
#define SEC_AES_WRAP 1
#define SEC_AES_UNWRAP 0

/** SEC Errors Type **/
#if 0

typedef enum SEC_ERROR_S {
	SEC_NO_ERROR = 0, /**< No ERROR */
	SEC_ERROR_INVALID_PARA, SEC_ERROR_NOT_SUPPORT, SEC_ERROR_INTERNAL,
} SEC_ERROR_E;



typedef enum {
	SEC_BOOT_VERIFY_SUCCESS = 0, /**< No ERROR */
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
} SEC_BOOT_ERROR;
#endif
typedef enum SEC_IMAGEGEN_TYPE_S {
	SEC_IMAGEGEN_TYPE_RAW = 0x00524157,
	SEC_IMAGEGEN_TYPE_BLP = 0x00424c70,
	SEC_IMAGEGEN_TYPE_BLW = 0x00424c77,
} SEC_IMAGEGEN_TYPE_E;

typedef enum SEC_SECTION_TYPE_S {
	SEC_SECTION_TYPE_TEXT = 0,
	SEC_SECTION_TYPE_RODATA,
	SEC_SECTION_TYPE_DATA,
	SEC_SECTION_TYPE_BSS,
	SEC_SECTION_TYPE_HEAP,
	SEC_SECTION_TYPE_STACK,
	SEC_SECTION_MAX,
} SEC_SECTION_TYPE_E;

typedef enum SEC_IMAGE_TYPE_S {
	SEC_IMAGE_TYPE_BOOTLOADER = 0,
	SEC_IMAGE_TYPE_SECOND_BOOTLOADER,
	SEC_IMAGE_TYPE_LAYOUT_DESCRIPTOR,
	SEC_IMAGE_TYPE_CM55M_S_APP,
	SEC_IMAGE_TYPE_CM55M_NS_APP,
	SEC_IMAGE_TYPE_CM55S_S_APP,
	SEC_IMAGE_TYPE_CM55S_NS_APP,
	SEC_IMAGE_TYPE_CM55M_MODEL,
	SEC_IMAGE_TYPE_CM55S_MODEL,
	SEC_IMAGE_TYPE_CM55M_APPCFG,
	SEC_IMAGE_TYPE_CM55S_APPCFG,
	SEC_IMAGE_TYPE_SEC_MEM_LAYOUT,
	SEC_IMAGE_TYPE_SEC_DEBUG,
	SEC_IMAGE_TYPE_LAYOUT_DESCRIPTOR_OTA_NEW,
	SEC_IMAGE_TYPE_LOADER_CONFIG,
	SEC_IMAGE_MAX,
} SEC_IMAGE_TYPE_E;
	
typedef enum SEC_CSTM_IMAGE_TYPE_S {
	SEC_IMAGE_TYPE_HX_IMAGE = 0,
	SEC_IMAGE_TYPE_CSTM_MEMORY_DESCRIPTOR,
	SEC_IMAGE_TYPE_CSTM_CM55M_NS_APP,
	SEC_IMAGE_TYPE_CSTM_CM55S_NS_APP,
	SEC_IMAGE_TYPE_CSTM_CM55M_MODEL,
	SEC_IMAGE_TYPE_CSTM_CM55S_MODEL,
	SEC_IMAGE_TYPE_CSTM_CM55M_APPCFG,
	SEC_IMAGE_TYPE_CSTM_CM55S_APPCFG,
	SEC_IMAGE_CSTM_MAX,
} SEC_CSTM_IMAGE_TYPE_E;	
#ifndef SUPPORT_SEC_LIB

#define SB_NUM_ATTRIBUTES 8
#define SB_RSA_BYTES 256
#define SB_RSA_BITS (8*SB_RSA_BYTES)
#define SB_ENCRYPTIONKEY256_LEN ((256 / 32) + 2)
#define SB_ENCRYPTIONIV_LEN (128 / 32)
#define SB_ATTRIBUTE_TYPE_PROJECT_ID 0x00504944 /* field for project ID. */
#define SB_ATTRIBUTE_TYPE_CRC 0x00435243 /* field for CRC. */

typedef struct {
	uint8_t signature[SB_RSA_BYTES];
} SB_Signature_t;

typedef struct {
	uint8_t pubkeyExp[4];
	uint8_t modulus[SB_RSA_BYTES];
} SB_PublicKey_t;

typedef struct {
	SB_PublicKey_t PublicKey; /** Public key. */
	SB_Signature_t Signature; /** Signature. */
	/* Notice: add padding to get the size which is multiple of 8 */
	uint8_t Pad[4];
} SB_Certificate_t;

/** Attribute data (incl. version id). */
typedef struct {
	uint32_t ElementType;
	uint32_t ElementValue;
} SB_AttributeElement_t;

typedef struct SB_Attributes {
	/** Attribute data element. */
	SB_AttributeElement_t AttributeElements[SB_NUM_ATTRIBUTES];
} SB_Attributes_t;

/**
 Signing header for the images.
 */
typedef struct {
	uint32_t Type; /** Type. refer to SEC_IMAGEGEN_TYPE_S*/
	uint32_t PubKeyType; /** Type of public key */
	SB_Signature_t Signature; /** Signature. */
	SB_PublicKey_t PublicKey; /** Public key (if included in image). */
	uint32_t EncryptionKey[SB_ENCRYPTIONKEY256_LEN]; /** Key. */
	uint32_t EncryptionIV[SB_ENCRYPTIONIV_LEN]; /** IV. */
	uint32_t ImageLen; /** Image length. */
	SB_Attributes_t ImageAttributes; /** Image attributes. */
	uint32_t CertificateCount; /** Certificate count. */
} SB_Header_t;
#endif
typedef struct {
	uint32_t cpu_clk; //cpu frequency
	uint32_t sec_system_mem; //memory address for secure driver internal use
} Hx_System_Info;

#endif

