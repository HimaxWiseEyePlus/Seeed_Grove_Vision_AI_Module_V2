#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#ifdef TRUSTZONE_SEC
#ifdef FREERTOS
/* Trustzone config. */
//
/* FreeRTOS includes. */
//#include "secure_port_macros.h"
#else
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
//#include "veneer_table.h"
//
#endif
#endif

#include "WE2_device.h"
#include "WE2_device_addr.h"
#include "board.h"
#include "hx_drv_scu.h"
#include "hx_drv_sdio.h"

#define BUFF_SIZE	1024
uint32_t mem_rbuffer[BUFF_SIZE];
uint32_t mem_wbuffer[BUFF_SIZE];

void dump_memory(uint32_t addr, uint32_t len)
{
	int i;

	addr &= (~0x3);

	for (i = 0; i < len; i++) {
		if (i % 4 == 0)
			xprintf("\n%08x:", addr);
		xprintf(" %08x", *((volatile unsigned int*) addr));
		addr += 4;
	}
	xprintf("\n");
}

/*!
 * @brief Main function
 */
int app_main(void) {

	uint32_t sdmem_addr = 0;
	uint32_t size = BUFF_SIZE;

	xprintf("Start sdio test app\n");

	for ( int i = 0; i < size/4; i++ )
	{
		mem_wbuffer[i] = i;
	}

	xprintf("dump mem_rbuffer[] before read SD card :\n");
	dump_memory((uint32_t)mem_rbuffer, size/4);

	// select SDIO pin mux
	hx_drv_scu_set_PB6_pinmux(SCU_PB6_PINMUX_SDIO_CMD, 1);
	hx_drv_scu_set_PB7_pinmux(SCU_PB7_PINMUX_SDIO_SDCLK, 1);
	hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SDIO_DAT1, 1);
	hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SDIO_DAT0, 1);
	hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SDIO_DAT2, 1);
	hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SDIO_DAT3, 1);

	if ( hx_drv_sdio_init(25000, 4, SD_REG_BASE)!=SDIO_PASS ) {
		xprintf("sdio init fail\n");
		return -1;
	}

	//sdmem_addr and size should align to 512
	if ( hx_drv_sdio_mem_write(sdmem_addr, size, (unsigned char *)(mem_wbuffer))!=SDIO_PASS ) {
		xprintf("write fail\n");
		return -1;
	}

	//sdmem_addr and size should align to 512
	if ( hx_drv_sdio_mem_read(sdmem_addr, size, (unsigned char *)(mem_rbuffer))!=SDIO_PASS ) {
		xprintf("read fail\n");
		return -1;
	}

	xprintf("dump mem_rbuffer[] after write SD card :\n");
	dump_memory((uint32_t)mem_rbuffer, size/4);

	return 0;
}
