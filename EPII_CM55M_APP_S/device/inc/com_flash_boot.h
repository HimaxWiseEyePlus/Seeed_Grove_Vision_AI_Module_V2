/*
 * flash_boot.h
 *
 *  Created on: 2021¦~4¤ë29¤é
 *      Author: 902447
 */

#ifndef INC_COM_FLASH_BOOT_H_
#define INC_COM_FLASH_BOOT_H_

/***********************************************
 * CONSTANT DEFINITION
 **********************************************/
#define SB_HEADER_VERSION_NUM			0x01
#define SB_HEADER_PID					6

#define SB_HEADER_IMAGE_SECT_MAX		16
#define SB_HEADER_MEMDES_SECT_MAX		96
#define SECURE_MEM_SECT_MAX             31
#define SB_HEADER_CRC_OFFSET			4

#define MEMDES_ATT_SECURE_FORMAT_POS			0
#define MEMDES_ATT_SECURE_FORMAT_SIZE			8
#define MEMDES_ATT_SECTION_TYPE_POS				8
#define MEMDES_ATT_SECTION_TYPE_SIZE			8
#define MEMDES_ATT_SECURE_HEADER_SIZE_POS		16
#define MEMDES_ATT_SECURE_HEADER_SIZE_SIZE		16
/***********************************************
 * MACRO/INLINE DECLARATION
 **********************************************/

/***********************************************
 * STRUNCTURE/EMULATOR DECLARATION
 **********************************************/
typedef enum SB_SECURE_TYPE_S {
	SB_SECURE_TYPE_RAW = 0,
	SB_SECURE_TYPE_BLP,
	SB_SECURE_TYPE_BLW,
	SB_SECURE_TYPE_BLW_REENC,
	SB_SECURE_TYPE_HMAC
} SB_SECURE_TYPE_E;

typedef enum SB_SECTION_TYPE_S {
	SB_SECTION_TYPE_TEXT = 0,
	SB_SECTION_TYPE_RODATA,
	SB_SECTION_TYPE_DATA,
	SB_SECTION_TYPE_BSS,
	SB_SECTION_TYPE_HEAP,
	SB_SECTION_TYPE_STACK,
} SB_SECTION_TYPE_E;

typedef enum EPII_BOOT_ECODE_S {
	EPII_BOOT_ROM_OK = 0, ///< Operation succeeded
	EPII_BOOT_ROM_ERROR = -1, ///< Unspecified error
	EPII_BOOT_ROM_ERROR_BUSY = -2, ///< SYS is busy
	EPII_BOOT_ROM_ERROR_TIMEOUT = -3, ///< Timeout occurred
	EPII_BOOT_ROM_ERROR_UNSUPPORTED = -4, ///< Operation not supported
	EPII_BOOT_ROM_ERROR_PARAMETER = -5, ///< Parameter error
	EPII_BOOT_ROM_ERROR_SECURE = -6,
	EPII_BOOT_ROM_ERROR_INVALID_HEADER = -7, ///< Header file content error
	EPII_BOOT_ROM_ERROR_CRC_HEADER = -8 ///< Header file crc error
} EPII_BOOT_ECODE_E;

typedef __PACKED_STRUCT epii_memory_dsp_item_s {
	uint32_t pat_type;		//(refer to SEC_IMAGE_TYPE_E)
	uint32_t pat_att;
	uint32_t pat_addr;		// partition in flash_addr
	uint32_t pat_size;
	uint32_t pat_exec_addr;     // execution address
	uint32_t pat_ccitt_crc16;     // crc
}epii_memory_dsp_item_t;

typedef __PACKED_STRUCT epii_mds_header_s {
	uint8_t type;// indicate the secure image type (refer to SEC_IMAGE_TYPE_E)
	uint8_t version;		// Version of the memory descriptor
	uint16_t crc;// CRC value (CRC is calculated over the bytes from pid to dsp_item*N)

	uint32_t pid;			// Product ID
	uint32_t size;	// the size of secure boot header (except Type/Version/CRC)

	uint32_t fw_image_version; //reserved for FW image version if future OTA need check

	uint8_t num_sbs;		// section count of secure boot image
	uint8_t reserved1;
	uint16_t reserved2;
	uint32_t flash_maxsize;
}epii_mds_header_t;

typedef __PACKED_STRUCT memory_dsp_s {
	epii_mds_header_t info;
	epii_memory_dsp_item_t item[SB_HEADER_MEMDES_SECT_MAX];
}memory_dsp_t;

typedef struct {
	uint32_t flash_addr;		// 1st or 2nd bl flash address
	uint32_t exec_addr; 		// 1st or 2nd bl execution address
	uint32_t size;
}epii_bl_memory_item_t;

//
typedef __PACKED_STRUCT epii_secure_memory_item_s {
	uint32_t start_addr;		// secure memory layout start address
	uint32_t end_addr;			// secure memory layout end address
}epii_secure_memory_item_t;

typedef __PACKED_STRUCT secure_memory_layout_s {
	uint8_t num_sbs;		// section count of secure memory layout
	uint8_t alias_bit;      // alias bit position
	uint8_t secure_bit;     // secure bit is 1 or 0
	uint8_t reserved;
	epii_secure_memory_item_t item[SECURE_MEM_SECT_MAX];
}secure_memory_layout_t;

typedef __PACKED_STRUCT epii_img_sec_item_s {
	uint32_t offset;// offset address of secure boot image section #n start from flash_header_t
	uint32_t type;			// section image type refer to SEC_SECTION_TYPE_E
	uint32_t dst;			// destination address
	uint32_t payload;// payload size for secure boot image section #n (SB Image Header + SB Image Section)
}epii_img_sec_item_t;

typedef __PACKED_STRUCT epii_app_img_header_s {
	uint8_t type;// indicate the secure image type (refer to SEC_IMAGE_TYPE_E)
	uint8_t version;		// Version of the app image header
	uint16_t crc;// CRC value (CRC is calculated over the bytes from Size to section header*N)

	uint32_t size;// the size of app_image_header(except Type/Version/CRC) + Section header*N
	uint8_t num_sbs;		// section count of secure boot image

	uint16_t reserved0;

	uint8_t reserved1;
}epii_app_img_header_t;

typedef __PACKED_STRUCT flash_header_s {
	epii_app_img_header_t info;
	epii_img_sec_item_t item[SB_HEADER_IMAGE_SECT_MAX];
}flash_header_t;

typedef struct
{
    uint32_t valid_code;
    uint8_t boot_clk_case;
	uint8_t boot_clk_type;	//SCU_PLL_FREQ_DISABLE, /**< CLK Source PLL Disable */,SCU_PLL_FREQ_ENABLE, /**< CLK Source PLL Enable */
    uint8_t boot_other_flag;//NOXTAL24M_PLLSRC_XTAL24M ->1 ,LSCDISPLL - >2 
    uint8_t reserved;
    uint32_t boot_CPU_frequency;
    uint32_t boot_freq_set_group1;
    uint32_t boot_freq_set_group2;
    uint32_t boot_freq_set_group3;
    uint32_t boot_freq_set_group4;
    uint32_t boot_freq_set_group5;
    uint32_t boot_freq_set_group6;
    uint8_t boot2nd_clk_case;
    uint8_t boot2nd_clk_type;	//SCU_PLL_FREQ_DISABLE, /**< CLK Source PLL Disable */,SCU_PLL_FREQ_ENABLE, /**< CLK Source PLL Enable */
    uint8_t boot2nd_other_flag;//NOXTAL24M_PLLSRC_XTAL24M ->1 ,LSCDISPLL - >2 
	uint8_t boot2nd_command_uart_pinmux;
	uint32_t boot2nd_console_uart_baud;
    uint32_t boot2nd_command_uart_baud;
    uint32_t boot2nd_CPU_frequency;
    uint32_t boot2nd_freq_set_group1;
    uint32_t boot2nd_freq_set_group2;
    uint32_t boot2nd_freq_set_group3;
    uint32_t boot2nd_freq_set_group4;
    uint32_t boot2nd_freq_set_group5;
    uint32_t boot2nd_freq_set_group6;

}LOADER_CONFIG_t;

#endif /* INC_COM_FLASH_BOOT_H_ */
