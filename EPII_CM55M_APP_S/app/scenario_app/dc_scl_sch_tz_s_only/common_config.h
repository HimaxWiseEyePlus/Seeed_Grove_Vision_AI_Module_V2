/*
 * common_config.h
 *
 *  Created on: Nov 22, 2022
 *      Author: bigcat-himax
 */


#ifndef APP_SCENARIO_DC_SCL_SCH_TZ_S_ONLY_COMMON_CONFIG_H_
#define APP_SCENARIO_DC_SCL_SCH_TZ_S_ONLY_COMMON_CONFIG_H_

//#define FRAME_CHECK_DEBUG	1
#define FLASH_OPERATION		1
//#define EN_ALGO				1
//#define CPU_24MHZ_VERSION
#define SPI_SEN_PIC_CLK				(10000000)

#define BLOCK_64KB_SZ		0x10000
#define SECTOR_4KB_SZ		0x1000
#define DEFAULT_ERASE_POS	0x100000
//each jpeg length information takes FILE_LEN_SZ to record
#define FILE_LEN_SZ			0x4

#define FLASH_XIP_POS		0x28000000

#define MEM_FREE_POS		(BOOT2NDLOADER_BASE) //0x340A0000

////////////////////////////////////////////////////////////////////////////
//we use first FLASH_LEN_SIZE as an area to store jpeg file length
#define FLASH_LEN_SIZE		0x800

#define SIP_FLASH_SZ		0x200000

#define RECORD_FRAME		30		//image count
#define SLEEP_PREIOD		1000	// sleep 1000ms and wakeup to run algorithm



#endif /* APP_SCENARIO_DC_SCL_SCH_TZ_S_ONLY_COMMON_CONFIG_H_ */
