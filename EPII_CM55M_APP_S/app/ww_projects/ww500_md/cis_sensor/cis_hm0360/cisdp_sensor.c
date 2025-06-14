/*
 * cisdp_sensor.c
 *
 *  Created on: 2024/11/18
 *      Author: 901912
 */

#include "cisdp_sensor.h"

#include "cisdp_cfg.h"
#include "printf_x.h"
#include "WE2_debug.h"
#include "hx_drv_CIS_common.h"
#include "hx_drv_timer.h"
#include "hx_drv_hxautoi2c_mst.h"

#include "WE2_core.h"
#include "hx_drv_swreg_aon.h"
#include "hx_drv_scu_export.h"
#include "driver_interface.h"
#include "hx_drv_scu.h"
#include "math.h"
#include "hm0360_regs.h"


#ifdef TRUSTZONE_SEC
#ifdef IP_INST_NS_csirx
#define	CSIRX_REGS_BASE 				BASE_ADDR_MIPI_RX_CTRL
#define CSIRX_DPHY_REG					(BASE_ADDR_APB_MIPI_RX_PHY+0x50)
#define CSIRX_DPHY_TUNCATE_REG			(BASE_ADDR_APB_MIPI_RX_PHY+0x48)
#else
#define CSIRX_REGS_BASE             	BASE_ADDR_MIPI_RX_CTRL_ALIAS
#define CSIRX_DPHY_REG					(BASE_ADDR_APB_MIPI_RX_PHY_ALIAS+0x50)
#define CSIRX_DPHY_TUNCATE_REG			(BASE_ADDR_APB_MIPI_RX_PHY_ALIAS+0x48)
#endif
#else
#ifndef TRUSTZONE
#define CSIRX_REGS_BASE             	BASE_ADDR_MIPI_RX_CTRL_ALIAS
#define CSIRX_DPHY_REG					(BASE_ADDR_APB_MIPI_RX_PHY_ALIAS+0x50)
#define CSIRX_DPHY_TUNCATE_REG			(BASE_ADDR_APB_MIPI_RX_PHY_ALIAS+0x48)
#else
#define CSIRX_REGS_BASE             	BASE_ADDR_MIPI_RX_CTRL
#define CSIRX_DPHY_REG					(BASE_ADDR_APB_MIPI_RX_PHY+0x50)
#define CSIRX_DPHY_TUNCATE_REG			(BASE_ADDR_APB_MIPI_RX_PHY+0x48)
#endif
#endif

#define JPEG_BUFSIZE  76800 //640*480/4
__attribute__(( section(".bss.NoInit"))) uint8_t jpegbuf[JPEG_BUFSIZE] __ALIGNED(32);

#define RAW_BUFSIZE  921600 //640*480*3
__attribute__(( section(".bss.NoInit"))) uint8_t demosbuf[RAW_BUFSIZE] __ALIGNED(32);

#define JPEG_HEADER_BUFSIZE 100
__attribute__(( section(".bss.NoInit"))) uint8_t jpegfilesizebuf[JPEG_HEADER_BUFSIZE] __ALIGNED(32);

static volatile uint32_t g_wdma1_baseaddr = (uint32_t)jpegbuf; // = (uint32_t)cdmbuf; // - no use
static volatile uint32_t g_wdma2_baseaddr = (uint32_t)jpegbuf;
static volatile uint32_t g_wdma3_baseaddr = (uint32_t)demosbuf;
static volatile uint32_t g_jpegautofill_addr = (uint32_t)jpegfilesizebuf;

static APP_DP_INP_SUBSAMPLE_E g_subs=APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X;

// New values of 8/4/25
static HX_CIS_SensorSetting_t HM0360_md_init_setting[] = {
#include "HM0360_OSC_Bayer_640x480_setA_VGA_setB_QVGA_md_8b_ParallelOutput_R2.i"
};

// Define the period between frames
#define TWOFPS (0x4020)
#define ONEFPS (0x8040)
#define TENFPS (0x0780)

// Choose a sleep period
#define RUNSLEEPCOUNT TENFPS
#define DPDSLEEPCOUNT ONEFPS


// Replaced by cisdp_sensor_set_mode()
//// For DPD mode
//HX_CIS_SensorSetting_t  HM0360_md_stream_on[] = {
//		{HX_CIS_I2C_Action_W, 0x3024, 0x01},	// select context B
//		{HX_CIS_I2C_Action_W, 0x3029, (DPDSLEEPCOUNT >> 8)},	// Sleep count H
//		{HX_CIS_I2C_Action_W, 0x302A, (DPDSLEEPCOUNT & 0xff)},	// Sleep count L
//		//		{HX_CIS_I2C_Action_W, 0x3510, 0x00},	// No need for this: Context A disable parallel output
//		{HX_CIS_I2C_Action_W, 0x0100, 0x02},
//};

// Replaced by cisdp_sensor_set_mode()
//// For powered-up operation
//static HX_CIS_SensorSetting_t HM0360_stream_on[] = {
//		{HX_CIS_I2C_Action_W, 0x3024, 0x00},	// select context A
////		{HX_CIS_I2C_Action_W, 0x3029, (RUNSLEEPCOUNT >> 8)},	// 10fps sleep count H
////		{HX_CIS_I2C_Action_W, 0x302A, (RUNSLEEPCOUNT & 0xff)},	// 10fps sleep count L
//////		{HX_CIS_I2C_Action_W, 0x3510, 0x01},	// No need for this: enable parallel output
////		{HX_CIS_I2C_Action_W, 0x0100, 0x02},
//};


// Replaced by cisdp_sensor_set_mode()
//// sleep - sets MODE_SELECT to 0 = sleep
//static HX_CIS_SensorSetting_t HM0360_stream_off[] = {
//        {HX_CIS_I2C_Action_W, MODE_SELECT, MODE_SLEEP},
//};

// Writes to IMAGE_ORIENTATION register: vertical flip & mirror
static HX_CIS_SensorSetting_t HM0360_mirror_setting[] = {
	{HX_CIS_I2C_Action_W, 0x0101, CIS_MIRROR_SETTING},
};


static void HM0360_dp_wdma_addr_init(APP_DP_INP_SUBSAMPLE_E subs)
{
    sensordplib_set_xDMA_baseaddrbyapp(g_wdma1_baseaddr, g_wdma2_baseaddr, g_wdma3_baseaddr);
    sensordplib_set_jpegfilesize_addrbyapp(g_jpegautofill_addr);

	xprintf("WD1[%x], WD2_J[%x], WD3_RAW[%x], JPAuto[%x]\n", g_wdma1_baseaddr, g_wdma2_baseaddr,
			g_wdma3_baseaddr, g_jpegautofill_addr);
}

void hm0360_set_dp_rc96()
{
	SCU_PDHSC_DPCLK_CFG_T cfg;

	hx_drv_scu_get_pdhsc_dpclk_cfg(&cfg);

	cfg.mipiclk.hscmipiclksrc = SCU_HSCMIPICLKSRC_RC96M48M;
	cfg.mipiclk.hscmipiclkdiv = 0;

	hx_drv_scu_set_pdhsc_dpclk_cfg(cfg, 0, 1);

	uint32_t mipi_pixel_clk = 96;
	hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_HSC_MIPI_RXCLK, &mipi_pixel_clk);
	mipi_pixel_clk = mipi_pixel_clk / 1000000;

	dbg_printf(DBG_LESS_INFO, "MIPI TX CLK: %dM\n", mipi_pixel_clk);
}


/**
 * Initialise HX6538 and HM0360
 *
 * Normally, the first time this is called sensor_init is true
 * and the many of the sensor registers are set with their default values.
 *
 * For subsequent calls, sensor_init is false, so most of the register initialisation is skipped.
 * @param - true to initialise all sensor regsiters
 * @return error code
 */
int cisdp_sensor_init(bool sensor_init) {
	HX_CIS_ERROR_E ret;

    dbg_printf(DBG_LESS_INFO, "Initialising HM0360\r\n");

    hx_drv_cis_set_slaveID(CIS_I2C_ID);

    // Set HM0360 mode to SLEEP before initialisation
    ret = cisdp_sensor_set_mode(CONTEXT_A, MODE_SLEEP, 0, 0);

    if (ret != HX_CIS_NO_ERROR) {
    	dbg_printf(DBG_LESS_INFO, "HM0360 off fail\r\n");
        return -1;
    }

//    if(hx_drv_cis_setRegTable(HM0360_stream_off, HX_CIS_SIZE_N(HM0360_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR) {
//    	dbg_printf(DBG_LESS_INFO, "HM0360 off fail\r\n");
//        return -1;
//    }

//#define TESTCISFILE
#ifdef TESTCISFILE
    // DEBUG here to understand the structure of the HX_CIS_SensorSetting_t tables, and

    HX_CIS_SensorSetting_t  HM0360_madeup[] = {
    		{HX_CIS_I2C_Action_W, 0x3510, 0x00},
    		{HX_CIS_I2C_Action_W, 0x0100, 0x02},
    		{HX_CIS_I2C_Action_R, 0x1234, 0x55},
    		{HX_CIS_I2C_Action_R, 0x6789, 0xab},
    		{HX_CIS_I2C_Action_S, 0xcdef, 0x66},
    };

    dbg_printf(DBG_LESS_INFO, "HM0360_stream_off[] has %d entry (%d bytes) Contents:\r\n",
    		HX_CIS_SIZE_N(HM0360_stream_off, HX_CIS_SensorSetting_t), sizeof(HM0360_stream_off));
    printf_x_printBuffer((uint8_t *)HM0360_stream_off, sizeof(HM0360_stream_off));

    dbg_printf(DBG_LESS_INFO, "HM0360_stream_on[] has %d entries (%d bytes) Contents:\r\n",
    		HX_CIS_SIZE_N(HM0360_stream_on, HX_CIS_SensorSetting_t), sizeof(HM0360_stream_on));

    printf_x_printBuffer((uint8_t *)HM0360_stream_on, sizeof(HM0360_stream_on));

    // More than 16 bytes
    dbg_printf(DBG_LESS_INFO, "HM0360_madeup[] has %d entries (%d bytes) Contents:\r\n",
    		HX_CIS_SIZE_N(HM0360_madeup, HX_CIS_SensorSetting_t), sizeof(HM0360_madeup));

    printf_x_printBuffer((uint8_t *)HM0360_madeup, sizeof(HM0360_madeup));

    // Test here for the cis_file functions
    cis_file_test("0:/cis_test_1.bin", false);

#endif // TESTCISFILE

	if (sensor_init == true) {
		// This is the long list of registers
		if(hx_drv_cis_setRegTable(HM0360_md_init_setting, HX_CIS_SIZE_N(HM0360_md_init_setting, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR) {
			dbg_printf(DBG_LESS_INFO, "HM0360 Init fail \r\n");
			return -1;
		}
		else {
			dbg_printf(DBG_LESS_INFO, "HM0360 Init \n");
		}

		// image orientation
		if(hx_drv_cis_setRegTable(HM0360_mirror_setting, HX_CIS_SIZE_N(HM0360_mirror_setting, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR) {
			dbg_printf(DBG_LESS_INFO, "HM0360 Init Mirror 0x%02X fail \r\n", HM0360_mirror_setting[0].Value);
			return -1;
		}
		else {
			#if (CIS_MIRROR_SETTING == 0x01)
				dbg_printf(DBG_LESS_INFO, "HM0360 Init Horizontal Mirror\n");
			#elif (CIS_MIRROR_SETTING == 0x02)
				dbg_printf(DBG_LESS_INFO, "HM0360 Init Vertical Mirror\n");
			#elif (CIS_MIRROR_SETTING == 0x03)
				dbg_printf(DBG_LESS_INFO, "HM0360 Init Horizontal & Vertical Mirror\n");
			#else
				dbg_printf(DBG_LESS_INFO, "HM0360 Init Mirror Off\n");
			#endif
		}

#ifdef TESTCISFILE
		// Some set/get testing
		hm0360_x_test_roi();
		hm0360_x_test_threshold();
		hm0360_x_test_sensitivity();
		hm0360_x_test_latency();
#endif	// 	TESTCISFILE

		dbg_printf(DBG_LESS_INFO, "HM0360 Init finished\n");
	}


    return 0;
}

/**
 * Seems to initialise hidden data path functions.
 *
 * In practise it is called like this:
 *
 * 	cisdp_dp_init(true,
 * 	 SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG,
 * 	 os_app_dplib_cb,
 * 	 g_img_data,
 * 	 APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X)
 *
 * Looks like:
 * @param inp_init - something to do with "INP & Sensor control related setting"
 * @param dp_type - data path selection - (but what is this?)
 * 		"Datapath selection this enumeration use in Datapath library internal and you can also use in application layer"
 * @param cb_event - function that receives a callback - most likely 'frame ready'
 * @param jpg_ratio - JPEG Encoding quantization table Selection (4x or 10x)
 * @param subs - sub-sample choice: x1, x2, x3, RGB or YUV
 * @return error code (error is -1)
 */
int cisdp_dp_init(bool inp_init, SENSORDPLIB_PATH_E dp_type, sensordplib_CBEvent_t cb_event,
		uint32_t jpg_ratio, APP_DP_INP_SUBSAMPLE_E subs) {

    HW2x2_CFG_T 	hw2x2_cfg;
    CDM_CFG_T 		cdm_cfg;
    HW5x5_CFG_T 	hw5x5_cfg;
    JPEG_CFG_T 		jpeg_cfg;

    g_subs = subs;	//sub-sample choice: x1, x2, x3, RGB or YUV

    // HW2x2 Cfg
    // Looks like this is used in some of the 'dp_type' options
    hw2x2_cfg.hw2x2_path = DP_HW2X2_PATH;
    hw2x2_cfg.hw_22_process_mode = DP_HW2X2_PROCESS_MODE;
    hw2x2_cfg.hw_22_crop_stx = DP_HW2X2_CROP_START_X;
    hw2x2_cfg.hw_22_crop_sty = DP_HW2X2_CROP_START_Y;
    hw2x2_cfg.hw_22_in_width = (subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X)?
    		640:(subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_2X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_2X)?
    		320:(subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_4X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_4X)?
    		160:640;//DP_HW2X2_CROP_WIDTH;

    hw2x2_cfg.hw_22_in_height = (subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X)?
    		480:(subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_2X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_2X)?
    		240:(subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_4X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_4X)?
			120:480;//DP_HW2X2_CROP_HEIGHT;

    hw2x2_cfg.hw_22_mono_round_mode = DP_HW2X2_ROUND_MODE;

    // CDM Cfg  no idea what "CDM" is
    // Looks like this is used in some of the 'dp_type' options
    cdm_cfg.cdm_enable = DP_CDM_ENABLE;
    cdm_cfg.cdm_crop_stx = DP_CDM_IN_START_X;
    cdm_cfg.cdm_crop_sty = DP_CDM_IN_START_Y;
    cdm_cfg.cdm_in_width = DP_CDM_IN_WIDTH;
    cdm_cfg.cdm_in_height = DP_CDM_IN_HEIGHT;
    cdm_cfg.meta_dump = DP_CDM_META_DUMP;
    cdm_cfg.ht_packing = DP_CDM_HT_PACKING;
    cdm_cfg.cdm_min_allow_dis = DP_CDM_MIN_ALLOW_DIS;
    cdm_cfg.cdm_tolerance = DP_CDM_TOLERANCE;
    cdm_cfg.cdm_reactance = DP_CDM_REACTANCE;
    cdm_cfg.cdm_relaxation = DP_CDM_RELAXATION;
    cdm_cfg.cdm_eros_th = DP_CDM_EROS_TH;
    cdm_cfg.cdm_num_ht_th = DP_CDM_NUM_HT_TH;
    cdm_cfg.cdm_num_ht_vect_th_x = DP_CDM_NUM_HT_VECT_TH_X;
    cdm_cfg.cdm_num_ht_vect_th_y = DP_CDM_NUM_HT_VECT_TH_X;
    cdm_cfg.cdm_num_cons_ht_bin_th_x = DP_CDM_NUM_CONS_HT_BIN_TH_X;
    cdm_cfg.cdm_num_cons_ht_bin_th_y = DP_CDM_NUM_CONS_HT_BIN_TH_Y;
    cdm_cfg.cpu_activeflag = DP_CDM_CPU_ACTIVEFLAG;
    cdm_cfg.init_map_flag = DP_CDM_INIT_MAP_FLAG;

    //HW5x5 Cfg   no idea what "hw5x5" is
    // Looks like this is used in some of the 'dp_type' options
    hw5x5_cfg.hw5x5_path = DP_HW5X5_PATH;
    hw5x5_cfg.demos_bndmode = DP_HW5X5_DEMOS_BNDMODE;
    hw5x5_cfg.demos_color_mode =
    		(subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X||subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_2X||subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_4X)?
    		DEMOS_COLORMODE_RGB:
			(subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_2X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_4X)?
    		DEMOS_COLORMODE_YUV420:DEMOS_COLORMODE_YUV420;//DP_HW5X5_DEMOS_COLORMODE;

    hw5x5_cfg.demos_pattern_mode = DP_HW5X5_DEMOS_PATTERN;
    hw5x5_cfg.demoslpf_roundmode = DP_HW5X5_DEMOSLPF_ROUNDMODE;
    hw5x5_cfg.hw55_crop_stx = DP_HW5X5_CROP_START_X;
    hw5x5_cfg.hw55_crop_sty = DP_HW5X5_CROP_START_X;
    hw5x5_cfg.hw55_in_width = (subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X)?
    		640:(subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_2X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_2X)?
    		320:(subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_4X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_4X)?
    		160:640;//DP_HW5X5_CROP_WIDTH;
    hw5x5_cfg.hw55_in_height = (subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X)?
    		480:(subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_2X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_2X)?
    		240:(subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_4X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_4X)?
			120:480;//DP_HW5X5_CROP_HEIGHT;

    //JPEG Cfg   no idea what "jpeg" is
    // Looks like this is used in some of the 'dp_type' options
    jpeg_cfg.jpeg_path = DP_JPEG_PATH;
    jpeg_cfg.enc_width = (subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X)?
    		640:(subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_2X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_2X)?
    		320:(subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_4X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_4X)?
    		160:640;//DP_JPEG_ENC_WIDTH;
    jpeg_cfg.enc_height = (subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X)?
    		480:(subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_2X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_2X)?
    		240:(subs==APP_DP_RES_RGB640x480_INP_SUBSAMPLE_4X||subs==APP_DP_RES_YUV640x480_INP_SUBSAMPLE_4X)?
			120:480;//DP_JPEG_ENC_HEIGHT;
    jpeg_cfg.jpeg_enctype = DP_JPEG_ENCTYPE;

    if(jpg_ratio == 4) {
    	jpeg_cfg.jpeg_encqtable = JPEG_ENC_QTABLE_4X;
    }
    else {
    	jpeg_cfg.jpeg_encqtable = JPEG_ENC_QTABLE_10X;
    }

    HM0360_dp_wdma_addr_init(subs);

#if (CIS_ENABLE_MIPI_INF != 0x00)
    //setup MIPI RX
	set_mipi_csirx_enable();
#endif

#ifdef USECROP
	// Not used
    INP_CROP_T crop;
    crop.start_x = DP_INP_CROP_START_X;
    crop.start_y = DP_INP_CROP_START_Y;

    if(DP_INP_CROP_WIDTH >= 1) {
    	crop.last_x = DP_INP_CROP_WIDTH - 1;
    }
    else {
    	crop.last_x = 0;
    }

    if(DP_INP_CROP_HEIGHT >= 1) {
    	crop.last_y = DP_INP_CROP_HEIGHT - 1;
    }
    else {
    	crop.last_y = 0;
    }
#endif // USECROP

	if (inp_init == true) {
		if (subs == APP_DP_RES_RGB640x480_INP_SUBSAMPLE_2X || subs == APP_DP_RES_YUV640x480_INP_SUBSAMPLE_2X) {
			sensordplib_set_sensorctrl_inp(SENCTRL_SENSOR_TYPE, SENCTRL_STREAM_TYPE, SENCTRL_SENSOR_WIDTH,
						SENCTRL_SENSOR_HEIGHT, INP_SUBSAMPLE_4TO2);
		}
		else if(subs == APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X || subs == APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X) {
			sensordplib_set_sensorctrl_inp(SENCTRL_SENSOR_TYPE, SENCTRL_STREAM_TYPE, SENCTRL_SENSOR_WIDTH,
						SENCTRL_SENSOR_HEIGHT, INP_SUBSAMPLE_DISABLE);
		}
	}

	uint8_t cyclic_buffer_cnt = 1;
	int32_t non_support = 0;

	switch (dp_type) {
	case SENSORDPLIB_PATH_INP_WDMA2:
	    sensordplib_set_raw_wdma2(DP_INP_OUT_WIDTH, DP_INP_OUT_HEIGHT,
	    		NULL);
	    break;
	case SENSORDPLIB_PATH_INP_HW2x2_CDM:
	    sensordplib_set_HW2x2_CDM(hw2x2_cfg, cdm_cfg, NULL);
	    break;
	case SENSORDPLIB_PATH_INP_HW5x5:
	    sensordplib_set_hw5x5_wdma3(hw5x5_cfg, NULL);
	    break;
	case SENSORDPLIB_PATH_INP_HW5x5_JPEG:
	    sensordplib_set_hw5x5_jpeg_wdma2(hw5x5_cfg, jpeg_cfg, cyclic_buffer_cnt, NULL);
	    break;
	case SENSORDPLIB_PATH_INP_HW2x2:
		sensordplib_set_HW2x2_wdma1(hw2x2_cfg, NULL);
		break;
	case SENSORDPLIB_PATH_INP_CDM:
		sensordplib_set_CDM(cdm_cfg, NULL);
		break;
	case SENSORDPLIB_PATH_INT1:
	    sensordplib_set_INT1_HWACC(hw2x2_cfg,
	            cdm_cfg, hw5x5_cfg,jpeg_cfg,
				cyclic_buffer_cnt,
	            NULL);
	    break;
	case SENSORDPLIB_PATH_INTNOJPEG:
		sensordplib_set_INTNoJPEG_HWACC(hw2x2_cfg,
	            cdm_cfg, hw5x5_cfg, NULL);
		break;
	case SENSORDPLIB_PATH_INT3:
		sensordplib_set_int_raw_hw5x5_wdma23(DP_INP_OUT_WIDTH,
				DP_INP_OUT_HEIGHT,
				hw5x5_cfg,
				NULL);
		break;
	case SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG:
		if(hw5x5_cfg.demos_color_mode == DEMOS_COLORMODE_RGB) {
			sensordplib_set_int_hw5x5rgb_jpeg_wdma23(hw5x5_cfg, jpeg_cfg,
					cyclic_buffer_cnt,
		            NULL);
		}
		else {
			sensordplib_set_int_hw5x5_jpeg_wdma23(hw5x5_cfg, jpeg_cfg,
					cyclic_buffer_cnt,
					NULL);
		}
		break;

	case SENSORDPLIB_PATH_INT_INP_HW2x2_HW5x5_JPEG:
		sensordplib_set_int_hw2x2_hw5x5_jpeg_wdma12(hw2x2_cfg,
	            hw5x5_cfg, jpeg_cfg,
				cyclic_buffer_cnt,
	            NULL);
		break;

	case SENSORDPLIB_PATH_JPEGDEC:
	case SENSORDPLIB_PATH_TPG_JPEGENC:
	case SENSORDPLIB_PATH_TPG_HW2x2:
	case SENSORDPLIB_PATH_INP_HXCSC_CDM:
	case SENSORDPLIB_PATH_INP_HXCSC:
	case SENSORDPLIB_PATH_INP_HXCSC_JPEG:
	case SENSORDPLIB_PATH_INT1_CSC:
	case SENSORDPLIB_PATH_INTNOJPEG_CSC:
	case SENSORDPLIB_PATH_INT3_CSC:
	case SENSORDPLIB_PATH_INT_INP_HXCSC_JPEG:
	case SENSORDPLIB_PATH_NO:
	default:
		dbg_printf(DBG_LESS_INFO, "Not support case \r\n");
		non_support = 1;
		break;
	}

	if(non_support == 1) {
		return -1;
	}

#if (CIS_ENABLE_HX_AUTOI2C != 0x00)
    //init hxauotoi2c
    set_hxautoi2c();
#endif

	hx_dplib_register_cb(cb_event, SENSORDPLIB_CB_FUNTYPE_DP);

#ifdef EVT_DATAPATH_AUTOI2C
    hx_drv_hxautoi2c_register_cb(hxautoi2c_noack_cb);
#endif

	return 0;
}

/**
 * Sets up sensor registers for normal image capture,
 * and starts data path sensor control block
 */
void cisdp_sensor_start(void) {

	// Now done by cisdp_sensor_set_mode()
//    if(hx_drv_cis_setRegTable(HM0360_stream_on, HX_CIS_SIZE_N(HM0360_stream_on, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR) {
//    	dbg_printf(DBG_LESS_INFO, "HM0360 on fail\r\n");
//        return;
//    }

    // starts data path sensor control block,
   	sensordplib_set_sensorctrl_start();
}

void cisdp_sensor_stop(void) {

    sensordplib_stop_capture();
    sensordplib_start_swreset();
    sensordplib_stop_swreset_WoSensorCtrl();

#if (CIS_ENABLE_HX_AUTOI2C == 0x00)

	// Set HM0360 operation: sleep
	cisdp_sensor_set_mode(CONTEXT_A, MODE_SLEEP, 0, 0);

//    // Writes to MODE_SELECT to select sleep
//    if(hx_drv_cis_setRegTable(HM0360_stream_off, HX_CIS_SIZE_N(HM0360_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)  {
//    	dbg_printf(DBG_LESS_INFO, "HM0360 off fail\r\n");
//    }
//    else  {
//    	dbg_printf(DBG_LESS_INFO, "HM0360 off \n");
//    }
#else
    //disable hxauotoi2c
    sensordplib_autoi2c_disable();
    dbg_printf(DBG_LESS_INFO, "hxauto i2c disable \n");
#endif

#if (CIS_ENABLE_MIPI_INF != 0x00)
    set_mipi_csirx_disable();
#endif
}


/**
 * Sets HM0360 for motion detection, prior to entering deep sleep.
 *
 * This is a heavily redacted version of the original Hiamx code.
 * See ww500_md_test_1 to see what I cut out.
 */
int cisdp_sensor_md_init(void) {
	HX_CIS_ERROR_E ret;

	// Set HM0360 operation: sleep

//  Now use cisdp_sensor_set_mode(), which sets sleep mode first
//    //
//    if(hx_drv_cis_setRegTable(HM0360_stream_off, HX_CIS_SIZE_N(HM0360_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR) {
//    	dbg_printf(DBG_LESS_INFO, "HM0360 off fail\r\n");
//        return -1;
//    }

	// Clear interrupts
    hx_drv_cis_set_reg(INT_CLEAR, 0xff, 0x01);

    // Set HM0360 mode to SLEEP before initialisation
    ret = cisdp_sensor_set_mode(CONTEXT_B, MODE_SW_NFRAMES_SLEEP, 1, DPDSLEEPCOUNT);

    if (ret != HX_CIS_NO_ERROR) {
    	dbg_printf(DBG_LESS_INFO, "HM0360 md on fail\r\n");
        return -1;
    }
//
//    // Switch to Context B motion detection mode
//	if (hx_drv_cis_setRegTable(HM0360_md_stream_on, HX_CIS_SIZE_N(HM0360_md_stream_on, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR) {
//    	dbg_printf(DBG_LESS_INFO, "HM0360 md on fail\r\n");
//        return -1;
//    }

    // This version has no delay
    dbg_printf(DBG_LESS_INFO, "HM0360 Motion Detection on!\r\n");

	return 0;
}


/*
 * Change HM0360 operating mode
 *
 * @param context - bits to write to the context control register (PMU_CFG_3, 0x3024)
 * @param mode - one of 8 modes of MODE_SELECT register
 * @param numFrames - the number of frames to capture before sleeping
 * @param sleepTime - the time to sleep before waking again
 * @return error code
 */
HX_CIS_ERROR_E cisdp_sensor_set_mode(uint8_t context, mode_select_t newMode, uint8_t numFrames, uint16_t sleepTime) {
	mode_select_t currentMode;
	HX_CIS_ERROR_E ret;

	ret = hx_drv_cis_get_reg(MODE_SELECT , &currentMode);
	if (ret != HX_CIS_NO_ERROR) {
		return ret;
	}

	xprintf("  Changing mode from %d to %d with nFrames=%d and sleepTime=%d\r\n",
			currentMode, newMode, numFrames, sleepTime);

	// Disable before making changes
	ret = hx_drv_cis_set_reg(MODE_SELECT, MODE_SLEEP, 0);
	if (ret != HX_CIS_NO_ERROR) {
		return ret;
	}

	// Context control
	ret = hx_drv_cis_set_reg(PMU_CFG_3, context, 0);
	if (ret != HX_CIS_NO_ERROR) {
		return ret;
	}

	if (numFrames != 0) {
		// Applies to MODE_SW_NFRAMES_SLEEP, MODE_SW_NFRAMES_STANDBY and MODE_HW_NFRAMES_SLEEP
		// This is the number of frames to take continguously, after the sleep finishes
		// It is NOT the total number of frames
		ret = hx_drv_cis_set_reg(PMU_CFG_7, numFrames, 0);
		if (ret != HX_CIS_NO_ERROR) {
			return ret;
		}
	}

	if (sleepTime != 0) {
		// Applies to MODE_SW_NFRAMES_SLEEP and MODE_HW_NFRAMES_SLEEP
		// This is the period of time between groups of frames.
		// Units are not specified. (0x8030 gives about 1s)
		ret = hx_drv_cis_set_reg(PMU_CFG_8, (uint8_t) (sleepTime >> 8), 0);	// msb
		if (ret != HX_CIS_NO_ERROR) {
			return ret;
		}
		ret = hx_drv_cis_set_reg(PMU_CFG_9, (uint8_t) (sleepTime & 0xff), 0);	// lsb
		if (ret != HX_CIS_NO_ERROR) {
			return ret;
		}
	}

	if (currentMode == MODE_SW_CONTINUOUS) {
		// consider delaying to finish current image before changing mode
	}

	ret = hx_drv_cis_set_reg(MODE_SELECT, newMode, 0);

	return ret;
}

/**
 * Read the HM0360 interrupt status register
 *
 * @param - pointer to byte to receive the status
 * @return error code
 */
HX_CIS_ERROR_E cisdp_sensor_get_int_status(uint8_t * val) {
	uint8_t currentStatus;
	HX_CIS_ERROR_E ret;

	ret = hx_drv_cis_get_reg(INT_INDIC , &currentStatus);
	if (ret != HX_CIS_NO_ERROR) {
		return ret;
	}
	else {
		*val = currentStatus;
	}
	return HX_CIS_NO_ERROR;
}


/**
 * Clear the HM0360 interrupt bits
 *
 * @param - mask for the bits to clear
 * @return error code
 */
HX_CIS_ERROR_E cisdp_sensor_clear_interrupt(uint8_t val) {
	HX_CIS_ERROR_E ret;

	ret = hx_drv_cis_set_reg(INT_CLEAR, val, 0);

	return ret;
}

void cisdp_get_jpginfo(uint32_t *jpeg_enc_filesize, uint32_t *jpeg_enc_addr)
{
    uint8_t frame_no;
    uint8_t buffer_no = 0;
    uint32_t jpeg_enc_filesize_real;

    hx_drv_xdma_get_WDMA2_bufferNo(&buffer_no);
    hx_drv_xdma_get_WDMA2NextFrameIdx(&frame_no);
    if(frame_no == 0)
    {
        frame_no = buffer_no - 1;
    }else{
        frame_no = frame_no - 1;
    }
    hx_drv_jpeg_get_EncOutRealMEMSize(&jpeg_enc_filesize_real);

    //dbg_printf(DBG_LESS_INFO, "current jpeg_size=0x%x\n", jpeg_enc_filesize_real);

    hx_drv_jpeg_get_FillFileSizeToMem(frame_no, g_jpegautofill_addr, jpeg_enc_filesize);
    hx_drv_jpeg_get_MemAddrByFrameNo(frame_no, g_wdma2_baseaddr, jpeg_enc_addr);

    if( jpeg_enc_filesize_real != *jpeg_enc_filesize)
    {
        dbg_printf(DBG_LESS_INFO, "*jpeg_enc_filesize_real(0x%08X) != *jpeg_enc_filesize(0x%08X)\n"
        		, jpeg_enc_filesize_real, *jpeg_enc_filesize);

        //change value
        *jpeg_enc_filesize = jpeg_enc_filesize_real;
    }

    //dbg_printf(DBG_LESS_INFO, "g_jpegautofill_addr: 0x%08X\n" "g_wdma2_baseaddr: 0x%08X\n", g_jpegautofill_addr, g_wdma2_baseaddr);
    //dbg_printf(DBG_LESS_INFO, "current frame_no=%d, jpeg_size=0x%x,addr=0x%x\n",frame_no,*jpeg_enc_filesize,*jpeg_enc_addr);
}

uint32_t app_get_jpeg_addr()
{
    //EPII_InvalidateDCache_by_Addr(g_wdma2_baseaddr, 4);
	return g_wdma2_baseaddr;
}

uint32_t app_get_jpeg_sz()
{
	hx_InvalidateDCache_by_Addr((volatile void *)g_jpegautofill_addr, 32);
	return *((uint32_t*)g_jpegautofill_addr);
}

uint32_t app_get_raw_addr()
{
	//raw data area BBBBBB/GGGGGG/RRRRRR
	return g_wdma3_baseaddr;	//return B for use
}

uint32_t app_get_raw_sz()
{
	if(g_subs == APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X)
		return 460800;//640*480*1.5;
	else if(g_subs == APP_DP_RES_YUV640x480_INP_SUBSAMPLE_2X)
		return 115200;//320*240*1.5;
	else if(g_subs == APP_DP_RES_YUV640x480_INP_SUBSAMPLE_4X)
		return 28800;//160*120*1.5;
	else if(g_subs == APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X)
		return 921600;//640*480*3;
	else if(g_subs == APP_DP_RES_RGB640x480_INP_SUBSAMPLE_2X)
		return 230400;//320*240*3;
	else if(g_subs == APP_DP_RES_RGB640x480_INP_SUBSAMPLE_4X)
		return 57600;//160*120*3;
	else
		return 640*480*3;
}

uint32_t app_get_raw_width() {

	if(g_subs == APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X||g_subs == APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X)
		return 640;
	else if(g_subs == APP_DP_RES_RGB640x480_INP_SUBSAMPLE_2X||g_subs == APP_DP_RES_YUV640x480_INP_SUBSAMPLE_2X)
		return 320;
	else if(g_subs == APP_DP_RES_RGB640x480_INP_SUBSAMPLE_4X||g_subs == APP_DP_RES_YUV640x480_INP_SUBSAMPLE_4X)
		return 160;
	else
		return 640;
}

uint32_t app_get_raw_height() {
	if(g_subs == APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X||g_subs == APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X)
		return 480;
	else if(g_subs == APP_DP_RES_RGB640x480_INP_SUBSAMPLE_2X||g_subs == APP_DP_RES_YUV640x480_INP_SUBSAMPLE_2X)
		return 240;
	else if(g_subs == APP_DP_RES_RGB640x480_INP_SUBSAMPLE_4X||g_subs == APP_DP_RES_YUV640x480_INP_SUBSAMPLE_4X)
		return 120;
	else
		return 480;
}

uint32_t app_get_raw_channels() {
	return SENCTRL_SENSOR_CH;
}

