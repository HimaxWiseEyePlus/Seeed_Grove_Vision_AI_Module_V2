#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "powermode_export.h"

#include "person_detect_model_data.h"


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

#include "spi_master_protocol.h"
#include "hx_drv_spi.h"
#include "spi_eeprom_comm.h"
#include "board.h"
#include "xprintf.h"
#include "dc_scl_sch_tz_s_only.h"
#include "board.h"
#include "WE2_core.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"
#ifdef IP_sensorctrl
#include "hx_drv_sensorctrl.h"
#endif
#ifdef IP_xdma
#include "hx_drv_xdma.h"
#include "sensor_dp_lib.h"
#endif
#ifdef IP_cdm
#include "hx_drv_cdm.h"
#endif
#ifdef IP_gpio
#include "hx_drv_gpio.h"
#endif
#include "hx_drv_pmu_export.h"
#include "hx_drv_pmu.h"
#include "powermode.h"
//#include "dp_task.h"
#include "BITOPS.h"

#include "common_config.h"
#include "cisdp_sensor.h"
#include "event_handler.h"
#include "memory_manage.h"
#include "hx_drv_watchdog.h"
#include "mb_cust_task.h"
#include "cvapp.h"

#ifdef EPII_FPGA
#define DBG_APP_LOG             (1)
#else
#define DBG_APP_LOG             (1)
#endif
#if DBG_APP_LOG
    #define dbg_app_log(fmt, ...)       xprintf(fmt, ##__VA_ARGS__)
#else
    #define dbg_app_log(fmt, ...)
#endif



static uint8_t 	g_xdma_abnormal, g_md_detect, g_cdm_fifoerror, g_wdt1_timeout, g_wdt2_timeout,g_wdt3_timeout;
static uint8_t 	g_hxautoi2c_error, g_inp1bitparer_abnormal;
static uint32_t g_dp_event;
static uint8_t 	g_frame_ready;
static uint32_t g_cur_jpegenc_frame;
static uint8_t 	g_time;
static uint8_t g_spi_master_initial_status;
/*volatile*/ uint32_t jpeg_addr, jpeg_sz;


//flash image start position
//To prevent information losses when M55M sleep w/o retentation, we will add needed information in the MB share data
uint32_t g_flash_record_start_pos = 0;
uint32_t g_flash_image_cur_pos = 0;
uint32_t g_flash_length_cur_pos = 0;
uint32_t g_idle_time = 0;
uint32_t g_img_data = 0;

static uint8_t g_store_image_count;


void app_start_state(APP_STATE_E state);

static void dp_var_init()
{
	g_xdma_abnormal = 0;
	g_md_detect = 0;
	g_cdm_fifoerror = 0;
	g_wdt1_timeout = 0;
	g_wdt2_timeout = 0;
	g_wdt3_timeout = 0;
	g_inp1bitparer_abnormal = 0;
	g_dp_event = 0;
	g_frame_ready = 0;
	g_time = 0;
	g_cur_jpegenc_frame = 0;
	g_hxautoi2c_error = 0;
	g_spi_master_initial_status = 0;
	g_store_image_count = 1;
}

void SetAlarmPMU() {
	uint32_t id;
	TIMER_CFG_T timer_cfg;
	//TIMER_ERROR_E ret;

#ifdef EPII_FPGA
	timer_cfg.period = g_idle_time;//30000;
#else
	timer_cfg.period = g_idle_time;//30000;
#endif
	timer_cfg.mode = TIMER_MODE_ONESHOT;
	timer_cfg.ctrl = TIMER_CTRL_PMU;
	timer_cfg.state = TIMER_STATE_PMU;
	id = 1;

	//ret = hx_drv_timer_hw_start(id, &timer_cfg, NULL);
	hx_drv_timer_hw_start(id, &timer_cfg, NULL);
}


void SetPSAudVidonly()
{
	PM_PD_VIDAUDPRE_CFG_T cfg;
	uint8_t speed,reset, trigger, retention;
	uint32_t pmu_pad_pa01_mask, pmu_rtc_mask, support_debugdump;
	uint32_t pmu_pad_pa23_mask, pmu_i2cw_mask, pmu_timer_mask, pmu_cmp_mask, pmu_ts_mask;
	uint32_t dcdcpin, freq, cm55mdiv, cm55sdiv, pmu_anti_mask, pmu_senint_mask;
	SCU_LSC_CLK_CFG_T lsc_cfg;
	SCU_PDHSC_HSCCLK_CFG_T hsc_cfg;


#ifdef SUPPORT_WARMBOOT_DISPLL
	speed = SCU_PLL_FREQ_DISABLE;
#else
	speed = SCU_PLL_FREQ_ENABLE;
#endif
#ifdef SUPPORT_CM55S_RESET
	reset = 1;
#else
	reset = 0;
#endif
#ifdef SUPPORT_MEM_RETENTION
	retention = 1;
#else
	retention = 0;
#endif

	pmu_pad_pa01_mask = 0;
	pmu_rtc_mask = 0;
	pmu_pad_pa23_mask = 0;
	pmu_i2cw_mask = 0;
	pmu_timer_mask = ~(0x1<<1); //TIMER876543210, TIMER MASK1 = 0x2
	pmu_cmp_mask = 0;
	pmu_ts_mask = 0;
	trigger = 1;
	support_debugdump = 0;
	dcdcpin = 0;
	pmu_senint_mask = 0;

	freq = 400000000;
	cm55mdiv = SCU_HSCCLKDIV_1;
	cm55sdiv = SCU_LSCCLKDIV_4;

	pmu_anti_mask = 0;

	cfg.bootromspeed.bootromclkfreq = speed;
	cfg.bootromspeed.pll_freq = freq;
	cfg.bootromspeed.cm55m_div = cm55mdiv;
	cfg.bootromspeed.cm55s_div = cm55sdiv;

#ifdef EPII_FPGA
	cfg.sensor_timer = g_idle_time-100;//29900;	 /**< Sensor Timer **/
	cfg.wdt_timer = 1000;	 /**< WDT Timer **/
#else
	cfg.sensor_timer = g_idle_time-100;//29900;	 /**< Sensor Timer **/
	cfg.wdt_timer = 1000;	 /**< WDT Timer **/
#endif
	cfg.nframeend_ctrl = PMU_NFRAMEEND_CTRL_I2C;
	cfg.cm55s_reset = reset;
	cfg.pmu_pad_pa01_mask = pmu_pad_pa01_mask;
	cfg.pmu_rtc_mask = pmu_rtc_mask;
	cfg.pmu_pad_pa23_mask = pmu_pad_pa23_mask;			/**< PMU SB GPIO Interrupt Mask **/
	cfg.pmu_i2cw_mask = pmu_i2cw_mask;			/**< PMU I2C Wakeup Interrupt Mask **/
	cfg.pmu_timer_mask = pmu_timer_mask;			/**< PMU Timer0~5 Wakeup Interrupt Mask  **/
	cfg.pmu_cmp_mask = pmu_cmp_mask;			/**< PMU CMP Wakeup Interrupt Mask  **/
	cfg.pmu_ts_mask = pmu_ts_mask;			/**< PMU TS Wakeup Interrupt Mask  **/
	cfg.pmu_anti_mask = pmu_anti_mask;
	cfg.pmu_mipii2c_noack_mask = 0;
	cfg.pmu_senint_mask = pmu_senint_mask;		/**< PMU SENSOR Interrupt Mask **/
	cfg.support_debugdump = support_debugdump;

	cfg.tcm_retention = retention;			/**< CM55M TCM Retention**/
	cfg.hscsram_retention[0] = retention;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[1] = retention;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[2] = retention;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[3] = retention;	/**< HSC SRAM Retention**/
	cfg.lscsram_retention = 1;		//cfg.lscsram_retention = retention;		/**< LSC SRAM Retention**/
	cfg.skip_bootflow.sec_mem_flag = retention;			/**< Skip Boot Flow**/
	cfg.skip_bootflow.first_bl_flag = retention; /*!< First BL Retention */
	cfg.skip_bootflow.cm55m_s_app_flag = retention; /*!< cm55m_s_app Retention */
	cfg.skip_bootflow.cm55m_ns_app_flag = retention; /*!< cm55m_ns_app Retention */
	cfg.skip_bootflow.cm55s_s_app_flag = 1;//cfg.skip_bootflow.cm55s_s_app_flag = retention; /*!< cm55s_s_app Retention */
	cfg.skip_bootflow.cm55s_ns_app_flag = 1;//cfg.skip_bootflow.cm55s_ns_app_flag = retention; /*!< cm55s_ns_app Retention */
	cfg.skip_bootflow.cm55m_model_flag = retention; /*!< cm55m model Retention */
	cfg.skip_bootflow.cm55s_model_flag = 1;//cfg.skip_bootflow.cm55s_model_flag = retention; /*!< cm55s model Retention */
	cfg.skip_bootflow.cm55m_appcfg_flag = retention; /*!< cm55m appcfg Retention */
	cfg.skip_bootflow.cm55s_appcfg_flag = 1;//cfg.skip_bootflow.cm55s_appcfg_flag = retention; /*!< cm55s appcfg Retention */
	cfg.skip_bootflow.cm55m_s_app_rwdata_flag = retention;/*!< cm55m_s_app RW Data Retention */
	cfg.skip_bootflow.cm55m_ns_app_rwdata_flag = retention;/*!< cm55m_ns_app RW Data Retention */
	cfg.skip_bootflow.cm55s_s_app_rwdata_flag = 1;//cfg.skip_bootflow.cm55s_s_app_rwdata_flag = retention;/*!< cm55s_s_app RW Data Retention */
	cfg.skip_bootflow.cm55s_ns_app_rwdata_flag = 1;//cfg.skip_bootflow.cm55s_ns_app_rwdata_flag = retention;/*!< cm55s_ns_app RW Data Retention */

	cfg.fast_vpr = 0;
	cfg.pmu_dcdc_outpin = dcdcpin;
	cfg.mclk_ctrl = PM_MCLK_CTRL_BYPMU;
	hx_lib_pm_cfg_set(&cfg, sensordplib_pmudpinit, PM_MODE_PS_VIDAUD_PREROLLING);

	SetAlarmPMU();

	hsc_cfg.hscclk.hscclksrc = SCU_HSCCLKSRC_XTAL24M;
	hsc_cfg.hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
	hsc_cfg.hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
	hsc_cfg.i3chcdiv = SCU_HSCI3CHCLKDIV_1;
	hsc_cfg.sdiodiv = SCU_HSCSDIOCLKDIV_1;
	lsc_cfg.lscclksrc = SCU_LSCCLKSRC_XTAL24M;
	lsc_cfg.lscclkdiv = SCU_LSCCLKDIV_1;

	if(trigger == 1)
	{
		hx_lib_pm_trigger(hsc_cfg, lsc_cfg, PM_CLK_PARA_CTRL_BYAPP);
	}

}


int flash_init() {
	hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
	return 0;
}

uint32_t flash_load_app_end_and_erase_after() {
	uint32_t app_end_addr=DEFAULT_ERASE_POS, erase_pos = 0;

	//app_end_addr = DEFAULT_ERASE_POS;
	hx_drv_swreg_aon_get_fwimagesize(&app_end_addr);

	dbg_app_log("App end addr: 0x%08X \n", app_end_addr);
	app_end_addr = app_end_addr%BLOCK_64KB_SZ?((app_end_addr/BLOCK_64KB_SZ)+1)*BLOCK_64KB_SZ:app_end_addr;
	erase_pos = app_end_addr;

	dbg_app_log("select start record addr: 0x%08X \n", app_end_addr);

	while(erase_pos<SIP_FLASH_SZ) {
		hx_lib_spi_eeprom_erase_sector(USE_DW_SPI_MST_Q,erase_pos, FLASH_64KBLOCK);
		erase_pos+=BLOCK_64KB_SZ;
	}


	return app_end_addr;

}

int flash_write(uint32_t flash_addr, uint32_t *data, uint32_t sz) {

	//if(hx_lib_spi_eeprom_write(USE_DW_SPI_MST_Q, flash_addr, data, sz,true)!= E_OK)
	if(hx_lib_spi_eeprom_word_write(USE_DW_SPI_MST_Q, flash_addr, data, sz)!= E_OK)
		return -1;

	return 0;
}

void flash_read_and_send() {
	uint32_t len = 0;
	uint32_t erase_pos = 0;
	uint32_t end_packet[16] = {0};
#if FLASH_OPERATION
	if(g_flash_image_cur_pos == g_flash_record_start_pos + FLASH_LEN_SIZE) {
		dbg_app_log("no image stored\n");

		//EPII_set_memory(0x42001110, 0x01010101);
		hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1, 1);
		hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1, 1);
    	hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1, 1);
    	hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_M_CS_1, 1);
		hx_drv_spi_mst_open();
		hx_drv_spi_mst_protocol_write_sp((uint32_t)end_packet, sizeof(end_packet),DATA_TYPE_END_OF_PACKET);
	}
	else {
		g_flash_image_cur_pos = g_flash_record_start_pos + FLASH_LEN_SIZE; //offset 2048 bytes
		g_flash_length_cur_pos = g_flash_record_start_pos;
		erase_pos = g_flash_record_start_pos;

		dbg_app_log("g_flash_record_start_pos=%x,\n",g_flash_record_start_pos);
		dbg_app_log("g_flash_image_cur_pos=%x,\n",g_flash_image_cur_pos);
		dbg_app_log("g_flash_length_cur_pos=%x,\n",g_flash_record_start_pos);

		//EPII_set_memory(0x42001110, 0x01010101);
		hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1, 1);
		hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1, 1);
    	hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1, 1);
    	hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_M_CS_1, 1);
		hx_drv_spi_mst_open();

		hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
		hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true , FLASH_QUAD, true);

		dbg_app_log("flash_read_and_send step\n");

		while(g_flash_length_cur_pos < g_flash_record_start_pos+FLASH_LEN_SIZE) {
			len = *(uint32_t*)(BASE_ADDR_FLASH1_R_ALIAS+g_flash_length_cur_pos);
			dbg_app_log("len=%d,\n",len);
			if(len == 0xffffffff) {
				dbg_app_log("reach end of storage files:%d\n",len);
				break;
			}

			//hx_drv_spi_mst_protocol_write(BASE_ADDR_FLASH1_R_ALIAS+g_flash_image_cur_pos, len);
			memcpy((uint32_t*)app_get_raw_addr(),(uint32_t*)(BASE_ADDR_FLASH1_R_ALIAS+g_flash_image_cur_pos), len);
			//EPII_CleanDCache_by_Addr(app_get_raw_addr(), len);
			hx_CleanDCache_by_Addr((volatile void *)app_get_raw_addr(), len);
			hx_drv_spi_mst_protocol_write(app_get_raw_addr(), len);

			dbg_app_log("write frame0x%x sz:%d bytes\n",g_flash_image_cur_pos,len);
			g_flash_image_cur_pos += len;
			g_flash_length_cur_pos += FILE_LEN_SZ;
		}
		if(len != 0) //send end packet when read finish
			hx_drv_spi_mst_protocol_write_sp((uint32_t)end_packet, sizeof(end_packet),DATA_TYPE_END_OF_PACKET);

		//end of flash xip mode
		hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, false , FLASH_QUAD, true);
		//
		if(erase_pos == 0) {
			dbg_app_log("in correct flash erase position!!!\n");
		}
		else {
			dbg_app_log("erase position=0x%x!!!\n",erase_pos);

			while(erase_pos<SIP_FLASH_SZ) {
				hx_lib_spi_eeprom_erase_sector(USE_DW_SPI_MST_Q,erase_pos, FLASH_64KBLOCK);
				erase_pos+=BLOCK_64KB_SZ;
			}

			g_flash_image_cur_pos = g_flash_record_start_pos + FLASH_LEN_SIZE; //offset 2048 bytes
			mb_cust_set_data(MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_IMAGE_POS,g_flash_image_cur_pos);

			g_flash_length_cur_pos = g_flash_record_start_pos;
			mb_cust_set_data(MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_LENGTH_POS,g_flash_length_cur_pos);
		}

		dbg_app_log("read process done, flash area for image store is erased !!\n");
	}
	//set pinmux back to UART1 for M55S console use
	hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_UART1_RX, 1);
	hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_UART1_TX, 1);
#else
	dbg_app_log("read process done, flash area for image store is erased !!\n");
#endif
}

void initial(bool cold_boot_flag) {
	//cold boot case initial
	if(cold_boot_flag == true) {
		//share data initial will be called when M55M cold boot
		mb_cust_data_init();

		xprintf("CM55M_ENABLE_CM55S \n\n\n");
		hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_RESET);
		hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_NORMAL);
		hx_drv_scu_set_CM55S_CPUWAIT(SCU_CM55_CPU_RUN);

#if FLASH_OPERATION
		//flash store area - erase
		flash_init();
		g_flash_record_start_pos = flash_load_app_end_and_erase_after();
		mb_cust_set_data(MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_IMAGE_INIT_POS,g_flash_record_start_pos);

		g_flash_image_cur_pos = g_flash_record_start_pos + FLASH_LEN_SIZE; //offset 2048 bytes
		mb_cust_set_data(MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_IMAGE_POS,g_flash_image_cur_pos);

		g_flash_length_cur_pos = g_flash_record_start_pos;
		mb_cust_set_data(MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_LENGTH_POS,g_flash_length_cur_pos);

		dbg_app_log("image start pos=0x%x, length start pos=0x%x, image cur pos=0x%x\n", g_flash_record_start_pos, g_flash_length_cur_pos, g_flash_image_cur_pos);
#else
		g_flash_record_start_pos = DEFAULT_ERASE_POS;
		mb_cust_set_data(MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_IMAGE_INIT_POS,g_flash_record_start_pos);

		g_flash_image_cur_pos = g_flash_record_start_pos + FLASH_LEN_SIZE; //offset 2048 bytes
		mb_cust_set_data(MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_IMAGE_POS,g_flash_image_cur_pos);

		g_flash_length_cur_pos = g_flash_record_start_pos;
		mb_cust_set_data(MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_LENGTH_POS,g_flash_length_cur_pos);
#endif
		if(mb_cust_get_data(MB_CUST_MUTEX_SHAREDATA_IDX_IMG_QUALITY, &g_img_data) == 0)
			dbg_app_log("image quality data= %d\r\n", g_img_data);
		else
			dbg_app_log("unable to get image quality data\r\n");

		if(mb_cust_get_data(MB_CUST_MUTEX_SHAREDATA_IDX_IDLE_TIME, &g_idle_time) == 0)
			dbg_app_log("idle time data= %d\r\n", g_idle_time);
		else
			dbg_app_log("unable to get idle time data\r\n");
	}
	else {

		if(mb_cust_get_data(MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_IMAGE_INIT_POS, &g_flash_record_start_pos)!=0)
			dbg_app_log("unable to retrieve flash initial position\r\n");
		if(mb_cust_get_data(MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_IMAGE_POS, &g_flash_image_cur_pos)!=0)
			dbg_app_log("unable to retrieve current flash image position\r\n");
		if(mb_cust_get_data(MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_LENGTH_POS, &g_flash_length_cur_pos)!=0)
			dbg_app_log("unable to retrieve current flash length data position\r\n");

		if(mb_cust_get_data(MB_CUST_MUTEX_SHAREDATA_IDX_IMG_QUALITY, &g_img_data) == 0)
			dbg_app_log("image quality data= %d\r\n", g_img_data);
		else
			dbg_app_log("unable to get image quality data\r\n");

		if(mb_cust_get_data(MB_CUST_MUTEX_SHAREDATA_IDX_IDLE_TIME, &g_idle_time) == 0)
			dbg_app_log("idle time data= %d\r\n", g_idle_time);
		else
			dbg_app_log("unable to get idle time data\r\n");

		dbg_app_log("image start pos=0x%x, length start pos=0x%x, image cur pos=0x%x\n", g_flash_record_start_pos, g_flash_length_cur_pos, g_flash_image_cur_pos);

	}
}

static void dp_app_eventhdl_cb(EVT_INDEX_E event)
{
	uint16_t err;
	int32_t read_status;
	int score;

	dbg_printf(DBG_LESS_INFO, "EVT event = %d\n", event);
	g_dp_event = event;

	switch(event)
	{
	case EVT_INDEX_1BITPARSER_ERR:  /*reg_inpparser_fs_cycle_error*/
		hx_drv_inp1bitparser_get_errstatus(&err);
		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_1BITPARSER_ERR err=0x%x\r\n",err);
		hx_drv_inp1bitparser_clear_int();
		hx_drv_inp1bitparser_set_enable(0);
		g_inp1bitparer_abnormal = 1;
		break;
	case EVT_INDEX_EDM_WDT1_TIMEOUT:
		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_EDM_WDT1_TlenIMEOUT\r\n");
		g_wdt1_timeout = 1;
		break;
	case EVT_INDEX_EDM_WDT2_TIMEOUT:
		dbg_printf(DBG_LESS_INFO,"EVT_INDEX_EDM_WDT2_TIMEOUT\r\n");
		g_wdt2_timeout = 1;
		break;
	case EVT_INDEX_EDM_WDT3_TIMEOUT:
		dbg_printf(DBG_LESS_INFO,"EVT_INDEX_EDM_WDT3_TIMEOUT\r\n");
		g_wdt3_timeout = 1;
		break;

	case EVT_INDEX_CDM_FIFO_ERR:
		/*
		 * error happen need CDM timing & TPG setting
		 * 1. SWRESET Datapath
		 * 2. restart streaming flow
		 */
		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_CDM_FIFO_ERR\r\n");
		g_cdm_fifoerror = 1;

		break;

	case EVT_INDEX_XDMA_WDMA1_ABNORMAL:
	case EVT_INDEX_XDMA_WDMA2_ABNORMAL:
	case EVT_INDEX_XDMA_WDMA3_ABNORMAL:
	case EVT_INDEX_XDMA_RDMA_ABNORMAL:
		/*
		 * error happen need
		 * 1. SWRESET Datapath
		 * 2. restart streaming flow
		 */
		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_XDMA_WDMA123_ABNORMAL or EVT_INDEX_XDMA_RDMA_ABNORMAL\r\n");
		g_xdma_abnormal = 1;
		break;

	case EVT_INDEX_CDM_MOTION:
		/*
		 * app anything want to do
		 * */
		dbg_printf(DBG_LESS_INFO, "Motion Detect\n");
		g_md_detect = 1;
		break;
	case EVT_INDEX_XDMA_FRAME_READY:
		g_cur_jpegenc_frame++;
    	g_frame_ready = 1;
		dbg_printf(DBG_LESS_INFO, "SENSORDPLIB_STATUS_XDMA_FRAME_READY %d \n", g_cur_jpegenc_frame);
		break;

	case EVT_INDEX_SENSOR_RTC_FIRE:
		g_time++;
		break;
	case EVT_INDEX_HXAUTOI2C_ERR:
		dbg_printf(DBG_LESS_INFO,"EVT_INDEX_HXAUTOI2C_ERR\r\n");
		g_hxautoi2c_error = 1;
		break;
	default:
		dbg_printf(DBG_LESS_INFO,"Other Event %d\n", event);
		break;
	}


	if(g_frame_ready == 1)
	{
		g_frame_ready = 0;

		cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);

		//check if flash is full
		if((g_flash_image_cur_pos+jpeg_sz)>(uint32_t)SIP_FLASH_SZ) {
			app_start_state(APP_STATE_ENTER_PMU);
		}

		if(g_store_image_count ==  1) {

			cvapp_init(true, true);
			//read image raw data and do CV
			cvapp_run(&score);

			//if CV result is positive, save to flash and go into APP_STATE_ALGO_JPEG_STORE state
			if(score >0) {
				//record image to flash for X seconds
				//store this image and get into save X second procedure
				dbg_app_log("try save image to[%x], addr[%x] sz[%d] !!\r\n",g_flash_image_cur_pos,jpeg_addr,jpeg_sz);
#if FLASH_OPERATION
				if((g_flash_image_cur_pos+jpeg_sz)<(uint32_t)SIP_FLASH_SZ) {
					//save this image to flash
					flash_init();

				}
				else {
					dbg_app_log("storage full!!\n");
					app_start_state(APP_STATE_ENTER_PMU);
				}
#endif
			}
			else {
				//todo: sensor setup and go to sleep
				app_start_state(APP_STATE_ENTER_PMU);
			}

		}
		//store image into flash
		if(g_store_image_count<RECORD_FRAME) {

#if FRAME_CHECK_DEBUG
			if(g_spi_master_initial_status == 0) {
				//EPII_set_memory(0x42001110, 0x01010101);
				hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1);
				hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1);
				hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1);
				hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_M_CS_1);
				if(hx_drv_spi_mst_open_speed(SPI_SEN_PIC_CLK) != 0)
				{
					dbg_printf(DBG_LESS_INFO, "DEBUG SPI master init fail\r\n");
					sensordplib_retrigger_capture();
					return ;
				}
				g_spi_master_initial_status = 1;
			}

			read_status = hx_drv_spi_mst_protocol_write_sp(jpeg_addr, jpeg_sz, DATA_TYPE_JPG);
					dbg_printf(DBG_LESS_INFO, "write frame result %d, data size=%d,addr=0x%x\n",read_status,
							jpeg_sz,jpeg_addr);
#endif
#if FLASH_OPERATION
			hx_InvalidateDCache_by_Addr((volatile void *)jpeg_addr, jpeg_sz);

		    flash_write(g_flash_image_cur_pos, (uint32_t *)jpeg_addr, jpeg_sz);
			flash_write(g_flash_length_cur_pos, (uint32_t*)&jpeg_sz, 4);

			dbg_app_log("save done, g_flash_image_cur_pos=%x, jpg_addr=%x, jpeg_sz=%d\n",g_flash_image_cur_pos, jpeg_addr, jpeg_sz);

			g_flash_image_cur_pos += jpeg_sz;
			g_flash_length_cur_pos += FILE_LEN_SZ;

			mb_cust_set_data(MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_IMAGE_POS,g_flash_image_cur_pos);
			mb_cust_set_data(MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_LENGTH_POS,g_flash_length_cur_pos);
#endif

			g_store_image_count++;

		}
		else {
			app_start_state(APP_STATE_ENTER_PMU);
		}

		//recapture image
		sensordplib_retrigger_capture();
	}

	if(g_md_detect == 1)
	{
		g_md_detect = 0;
	}

	if(g_inp1bitparer_abnormal == 1 || g_wdt1_timeout == 1 || g_wdt2_timeout == 1 || g_wdt3_timeout == 1
			|| g_cdm_fifoerror == 1 || g_xdma_abnormal == 1 || g_hxautoi2c_error == 1)
	{
		cisdp_sensor_stop();
	}

}

void read_operation_check() {
	//MB_CUST_READ_IMAGE_STATUS_E read_flash_flag = MB_CUST_READ_IMAGE_END;
	uint32_t read_data = (MB_CUST_READ_IMAGE_STATUS_E)MB_CUST_READ_IMAGE_END;


	if(mb_cust_get_data(MB_CUST_MUTEX_SHAREDATA_IDX_READ_IAMGE_STATUS, &read_data) == 0 && (MB_CUST_READ_IMAGE_STATUS_E)read_data == MB_CUST_READ_IMAGE_START ) {
		dbg_app_log("go into read flash procedure\r\n");
		flash_read_and_send();

		if(mb_cust_set_data(MB_CUST_MUTEX_SHAREDATA_IDX_READ_IAMGE_STATUS, MB_CUST_READ_IMAGE_END) != 0)
			dbg_app_log("set share data fail - MB_CUST_MUTEX_SHAREDATA_IDX_READ_IAMGE_STATUS\r\n");

		//got to APP_STATE_ENTER_PMU
		app_start_state(APP_STATE_ENTER_PMU);
	}
}

void app_start_state(APP_STATE_E state)
{
	if(state == APP_STATE_ENTER_PMU) {
		SetPSAudVidonly();
	}
	else if(state == APP_STATE_ALGO_JPEG_STORE) {
        if(cisdp_sensor_init() < 0)
        {
        	xprintf("\r\nCIS Init fail\r\n");
        	APP_BLOCK_FUNC();
        }

        dp_var_init();

        //if wdma variable is zero when not init yet, then this step is a must be to retrieve wdma address
        if(cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, dp_app_eventhdl_cb, g_img_data, APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X) < 0)
        {
        	xprintf("\r\nDATAPATH Init fail\r\n");
        	APP_BLOCK_FUNC();
        }

        event_handler_init();

        cisdp_sensor_start();

    	event_handler_start();
	}


}

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int app_main(void) {

	uint32_t wakeup_event;
	uint32_t wakeup_event1;
	int32_t read_status;
	uint32_t freq=0;
	int score;

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);

    hx_drv_swreg_aon_get_pllfreq(&freq);
    xprintf("wakeup_event=0x%x,WakeupEvt1=0x%x, freq=%d\n", wakeup_event, wakeup_event1, freq);

    //initial when cold boot
    initial((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE));

    read_operation_check();

    mm_set_initial(MEM_FREE_POS+g_person_detect_model_data_size+sizeof(g_person_detect_model_data_size), 0x00200000-(MEM_FREE_POS+g_person_detect_model_data_size+sizeof(g_person_detect_model_data_size)-0x34000000));

	if((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE)) {
		app_start_state(APP_STATE_ENTER_PMU);
	}
	else {
		xprintf("WARM BOOT\n\n\n\n\n");
		hx_lib_pm_ctrl_fromPMUtoCPU(sensordplib_pmutocpuctrl);
		app_start_state(APP_STATE_ALGO_JPEG_STORE);
	}
	return 0;

}
