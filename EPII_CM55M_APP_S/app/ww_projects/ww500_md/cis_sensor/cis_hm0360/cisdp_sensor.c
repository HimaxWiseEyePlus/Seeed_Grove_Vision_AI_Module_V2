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

#ifdef CONTEXTSWITCH
// New values of 8/4/25
static HX_CIS_SensorSetting_t HM0360_md_init_setting[] = {
#include "cis_sensor/cis_hm0360/HM0360_OSC_Bayer_640x480_setA_VGA_setB_QVGA_md_8b_ParallelOutput_R2.i"
};

#else
// Original values
static HX_CIS_SensorSetting_t HM0360_md_init_setting[] = {
#include "cis_sensor/cis_hm0360/HM0360_OSC_Bayer_640x480_setA_VGA_md_4b_ParallelOutput_R2.i"
};

#endif	// CONTEXTSWITCH

#ifdef CONTEXTSWITCH

// Define the period between frames
#define TWOFPS (0x4020)
#define ONEFPS (0x8040)
#define TENFPS (0x0780)

// Choose a sleep period
#define RUNSLEEPCOUNT TENFPS
#define DPDSLEEPCOUNT ONEFPS

// For DPD mode
HX_CIS_SensorSetting_t  HM0360_md_stream_on[] = {
		{HX_CIS_I2C_Action_W, 0x3024, 0x01},	// select context B
		{HX_CIS_I2C_Action_W, 0x3029, (DPDSLEEPCOUNT >> 8)},	// Sleep count H
		{HX_CIS_I2C_Action_W, 0x302A, (DPDSLEEPCOUNT & 0xff)},	// Sleep count L
		//		{HX_CIS_I2C_Action_W, 0x3510, 0x00},	// No need for this: Context A disable parallel output
		{HX_CIS_I2C_Action_W, 0x0100, 0x02},
};

// For powered-up operation
static HX_CIS_SensorSetting_t HM0360_stream_on[] = {
		{HX_CIS_I2C_Action_W, 0x3024, 0x00},	// select context A
		{HX_CIS_I2C_Action_W, 0x3029, (RUNSLEEPCOUNT >> 8)},	// 10fps sleep count H
		{HX_CIS_I2C_Action_W, 0x302A, (RUNSLEEPCOUNT & 0xff)},	// 10fps sleep count L
//		{HX_CIS_I2C_Action_W, 0x3510, 0x01},	// No need for this: enable parallel output
		{HX_CIS_I2C_Action_W, 0x0100, 0x02},
};

#else
// Parallel output disabled. "Automatic wakeup sleep cycles"
// Sets MASK_OUTPUT_EN to 0 = disable parallel output
// Sets MODE_SELECT to 2 = "Automatic wakeup sleep cycles"
// Output N frames then sleep (0x3028 sets frame count and is set to 1).
HX_CIS_SensorSetting_t  HM0360_md_stream_on[] = {
		{HX_CIS_I2C_Action_W, FRAME_OUTPUT_EN, 0x00},
		{HX_CIS_I2C_Action_W, MODE_SELECT, MODE_SW_NFRAMES_SLEEP},
};

// Parallel output enabled. Continous stream
// Sets MASK_OUTPUT_EN to 1 = enable parallel output
// Sets MODE_SELECTto 1 = continuous streaming
static HX_CIS_SensorSetting_t HM0360_stream_on[] = {
		{HX_CIS_I2C_Action_W, FRAME_OUTPUT_EN, 0x01},
		{HX_CIS_I2C_Action_W, MODE_SELECT, MODE_SW_CONTINUOUS},
};

#endif // CONTEXTSWITCH

// sleep - sets MODE_SELECT to 0 = sleep
static HX_CIS_SensorSetting_t HM0360_stream_off[] = {
        {HX_CIS_I2C_Action_W, MODE_SELECT, MODE_SLEEP},
};

// Writes to IMAGE_ORIENTATION register: vertical flip & mirror
static HX_CIS_SensorSetting_t HM0360_mirror_setting[] = {
	{HX_CIS_I2C_Action_W, 0x0101, CIS_MIRROR_SETTING},
};

#if 0
// CGP Not used
static HX_CIS_SensorSetting_t HM0360_stream_xsleep[] = {
        {HX_CIS_I2C_Action_W, MODE_SELECT, MODE_SW_NFRAMES_SLEEP},
};
#endif // 0

static void HM0360_dp_wdma_addr_init(APP_DP_INP_SUBSAMPLE_E subs)
{
    sensordplib_set_xDMA_baseaddrbyapp(g_wdma1_baseaddr, g_wdma2_baseaddr, g_wdma3_baseaddr);
    sensordplib_set_jpegfilesize_addrbyapp(g_jpegautofill_addr);

	xprintf("WD1[%x], WD2_J[%x], WD3_RAW[%x], JPAuto[%x]\n",g_wdma1_baseaddr,g_wdma2_baseaddr,
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

#if 0
// CGP Not used
static void set_mipi_csirx_enable()
{
	uint32_t bitrate_1lane = HM0360_MIPI_CLOCK_FEQ*2;
	uint32_t mipi_lnno = HM0360_MIPI_LANE_CNT;
	uint32_t pixel_dpp = HM0360_MIPI_DPP;
	uint32_t line_length = SENCTRL_SENSOR_WIDTH;
	uint32_t frame_length = SENCTRL_SENSOR_HEIGHT;
	uint32_t byte_clk = bitrate_1lane/8;
	uint32_t continuousout = HM0360_MIPITX_CNTCLK_EN;
	uint32_t deskew_en = 0;
	uint32_t mipi_pixel_clk = 96;

	dbg_printf(DBG_LESS_INFO, "MIPI CSI Init Enable\n");

	hm0360_set_dp_rc96();
	hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_HSC_MIPI_RXCLK, &mipi_pixel_clk);

	mipi_pixel_clk = mipi_pixel_clk / 1000000;

	dbg_printf(DBG_LESS_INFO, "MIPI CSI Init Enable\n");
	dbg_printf(DBG_LESS_INFO, "MIPI TX CLK: %dM\n", mipi_pixel_clk);
	dbg_printf(DBG_LESS_INFO, "MIPI BITRATE 1LANE: %dM\n", bitrate_1lane);
	dbg_printf(DBG_LESS_INFO, "MIPI DATA LANE: %d\n", mipi_lnno);
	dbg_printf(DBG_LESS_INFO, "MIPI PIXEL DEPTH: %d\n", pixel_dpp);
	dbg_printf(DBG_LESS_INFO, "MIPI LINE LENGTH: %d\n", line_length);
	dbg_printf(DBG_LESS_INFO, "MIPI FRAME LENGTH: %d\n", frame_length);
	dbg_printf(DBG_LESS_INFO, "MIPI CONTINUOUSOUT: %d\n", continuousout);
	dbg_printf(DBG_LESS_INFO, "MIPI DESKEW: %d\n", deskew_en);

	uint32_t n_preload = 15;
	uint32_t l_header = 4;
	uint32_t l_footer = 2;

	double t_input = (double)(l_header+line_length*pixel_dpp/8+l_footer)/(mipi_lnno*byte_clk)+0.06;
	double t_output = (double)line_length/mipi_pixel_clk;
	double t_preload = (double)(7+(n_preload*4)/mipi_lnno)/mipi_pixel_clk;

	double delta_t = t_input - t_output - t_preload;

	dbg_printf(DBG_LESS_INFO, "t_input: %dns\n", (uint32_t)(t_input*1000));
	dbg_printf(DBG_LESS_INFO, "t_output: %dns\n", (uint32_t)(t_output*1000));
	dbg_printf(DBG_LESS_INFO, "t_preload: %dns\n", (uint32_t)(t_preload*1000));


	uint16_t rx_fifo_fill = 0;
	uint16_t tx_fifo_fill = 0;

	if(delta_t <= 0)
	{
		delta_t = 0 - delta_t;
		tx_fifo_fill = ceil(delta_t*byte_clk*mipi_lnno/4/(pixel_dpp/2))*(pixel_dpp/2);
		rx_fifo_fill = 0;
	}
	else
	{
		rx_fifo_fill = ceil(delta_t*byte_clk*mipi_lnno/4/(pixel_dpp/2))*(pixel_dpp/2);
		tx_fifo_fill = 0;
	}
	dbg_printf(DBG_LESS_INFO, "MIPI RX FIFO FILL: %d\n", rx_fifo_fill);
	dbg_printf(DBG_LESS_INFO, "MIPI TX FIFO FILL: %d\n", tx_fifo_fill);

	/*
	 * Reset CSI RX/TX
	 */
	dbg_printf(DBG_LESS_INFO, "RESET MIPI CSI RX/TX\n");
	SCU_DP_SWRESET_T dp_swrst;
	drv_interface_get_dp_swreset(&dp_swrst);
	dp_swrst.HSC_MIPIRX = 0;
	dp_swrst.HSC_MIPITX = 0;

	hx_drv_scu_set_DP_SWReset(dp_swrst);
	hx_drv_timer_cm55x_delay_us(50, TIMER_STATE_DC);

	dp_swrst.HSC_MIPIRX = 1;
	dp_swrst.HSC_MIPITX = 1;
	hx_drv_scu_set_DP_SWReset(dp_swrst);

    MIPIRX_DPHYHSCNT_CFG_T hscnt_cfg;
    hscnt_cfg.mipirx_dphy_hscnt_clk_en = 0;
    hscnt_cfg.mipirx_dphy_hscnt_ln0_en = 1;
    hscnt_cfg.mipirx_dphy_hscnt_ln1_en = 1;

    if(mipi_pixel_clk == 200) //pll200
    {
		hscnt_cfg.mipirx_dphy_hscnt_clk_val = 0x03;
		hscnt_cfg.mipirx_dphy_hscnt_ln0_val = 0x10;
		hscnt_cfg.mipirx_dphy_hscnt_ln1_val = 0x10;
    }
    else if(mipi_pixel_clk == 300) //pll300
    {
		hscnt_cfg.mipirx_dphy_hscnt_clk_val = 0x03;
		hscnt_cfg.mipirx_dphy_hscnt_ln0_val = 0x18;
		hscnt_cfg.mipirx_dphy_hscnt_ln1_val = 0x18;
    }
    else //rc96
    {
		hscnt_cfg.mipirx_dphy_hscnt_clk_val = 0x03;
		hscnt_cfg.mipirx_dphy_hscnt_ln0_val = 0x06;
		hscnt_cfg.mipirx_dphy_hscnt_ln1_val = 0x06;
    }
    sensordplib_csirx_set_hscnt(hscnt_cfg);

    if(pixel_dpp == 10 || pixel_dpp == 8)
    {
    	sensordplib_csirx_set_pixel_depth(pixel_dpp);
    }
    else
    {
    	dbg_printf(DBG_LESS_INFO, "PIXEL DEPTH fail %d\n", pixel_dpp);
        return;
    }

	sensordplib_csirx_set_deskew(deskew_en);
	sensordplib_csirx_set_fifo_fill(rx_fifo_fill);
    sensordplib_csirx_enable(mipi_lnno);

    dbg_printf(DBG_LESS_INFO, "VMUTE: 0x%08X\n", *(uint32_t*)(SCU_LSC_ADDR+0x408));
    dbg_printf(DBG_LESS_INFO, "0x53061000: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x1000));
    dbg_printf(DBG_LESS_INFO, "0x53061004: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x1004));
    dbg_printf(DBG_LESS_INFO, "0x53061008: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x1008));
    dbg_printf(DBG_LESS_INFO, "0x5306100C: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x100C));
    dbg_printf(DBG_LESS_INFO, "0x53061010: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x1010));

}
#endif // 0

#if 0
// CGP Not used
static void set_mipi_csirx_disable()
{
	dbg_printf(DBG_LESS_INFO, "MIPI CSI Disable\n");

    volatile uint32_t *dphy_reg = (uint32_t *)CSIRX_DPHY_REG;
    volatile uint32_t *csi_static_cfg_reg = (uint32_t *)(CSIRX_REGS_BASE+0x08);
    volatile uint32_t *csi_dphy_lane_control_reg = (uint32_t *)(CSIRX_REGS_BASE+0x40);
    volatile uint32_t *csi_stream0_control_reg = (uint32_t *)(CSIRX_REGS_BASE+0x100);
    volatile uint32_t *csi_stream0_data_cfg = (uint32_t *)(CSIRX_REGS_BASE+0x108);
    // CGP unused volatile uint32_t *csi_stream0_cfg_reg = (uint32_t *)(CSIRX_REGS_BASE+0x10C);

    sensordplib_csirx_disable();

    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", dphy_reg, *dphy_reg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_static_cfg_reg, *csi_static_cfg_reg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_dphy_lane_control_reg, *csi_dphy_lane_control_reg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_stream0_data_cfg, *csi_stream0_data_cfg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_stream0_control_reg, *csi_stream0_control_reg);

}
#endif // 0

#if (CIS_ENABLE_HX_AUTOI2C != 0x00)
static void set_hxautoi2c()
{
	//set to synopsys IP, change to Himax IP
	dbg_printf(DBG_LESS_INFO, "INIT AUTOI2C\n");
	HXAUTOI2CHC_STATIC_CFG_T scfg;
	HXAUTOI2CHC_INT_CFG_T icfg;
	HXAUTOI2CHC_CMD_CFG_T trig_cfg;
	HXAUTOI2CHC_CMD_CFG_T stop_cfg;

	scfg.slaveid = CIS_I2C_ID;

	scfg.clkdiv = HXAUTOI2C_SCL_CLK_DIV_240;
	scfg.cmdtype = HXAUTOI2CHC_CMD_TRIG_STOP;
	scfg.trig_ctrl_sw = HXAUTOI2CHC_CMD_CTRL_BY_HW;
	scfg.stop_ctrl_sw = HXAUTOI2CHC_CMD_CTRL_BY_HW;

	icfg.trig_int_loc = HXAUTOI2CHC_INT_LOC_AFT_CMD1;
	icfg.stop_int_loc = HXAUTOI2CHC_INT_LOC_AFT_CMD1;
	icfg.trig_delay = 0;
	icfg.stop_delay = 0;

	trig_cfg.cmd_num = HXAUTOI2CHC_CMD_NUM_1;
	trig_cfg.cmd1_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	trig_cfg.cmd2_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	trig_cfg.cmd3_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	trig_cfg.cmd4_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	trig_cfg.delay01 = 0;
	trig_cfg.delay12 = 0x100;
	trig_cfg.delay23 = 0x100;
	trig_cfg.delay34 = 0x100;

	stop_cfg.cmd_num = HXAUTOI2CHC_CMD_NUM_1;
	stop_cfg.cmd1_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	stop_cfg.cmd2_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	stop_cfg.cmd3_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	stop_cfg.cmd4_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	stop_cfg.delay01 = 0;
	stop_cfg.delay12 = 0x100;
	stop_cfg.delay23 = 0x100;
	stop_cfg.delay34 = 0x100;

	sensordplib_autoi2c_cfg(scfg, icfg, trig_cfg, stop_cfg);


	HXAUTOI2CHC_CMD_T trig_cmd1, trig_cmd2, trig_cmd3, trig_cmd4;

	trig_cmd1.byte1 = (HM0360_stream_on[0].RegAddree >> 8 & 0xFF);
	trig_cmd1.byte2 = (HM0360_stream_on[0].RegAddree & 0xFF);
	trig_cmd1.byte3 = HM0360_stream_on[0].Value;
	trig_cmd1.byte4 = 0x00;

	trig_cmd2.byte1 = trig_cmd2.byte2 = trig_cmd2.byte3 = trig_cmd2.byte4 = 0x11;
	trig_cmd3.byte1 = trig_cmd3.byte2 = trig_cmd3.byte3 = trig_cmd3.byte4 = 0x22;
	trig_cmd4.byte1 = trig_cmd4.byte2 = trig_cmd4.byte3 = trig_cmd4.byte4 = 0x33;

	sensordplib_autoi2c_trigcmd(trig_cmd1, trig_cmd2, trig_cmd3, trig_cmd4);

	HXAUTOI2CHC_CMD_T stop_cmd1, stop_cmd2, stop_cmd3, stop_cmd4;

	stop_cmd1.byte1 = (HM0360_stream_off[0].RegAddree >> 8 & 0xFF);
	stop_cmd1.byte2 = (HM0360_stream_off[0].RegAddree & 0xFF);
	stop_cmd1.byte3 = HM0360_stream_off[0].Value;
	stop_cmd1.byte4 = 0x00;

	stop_cmd2.byte1 = stop_cmd2.byte2 = stop_cmd2.byte3 = stop_cmd2.byte4 = 0x11;
	stop_cmd3.byte1 = stop_cmd3.byte2 = stop_cmd3.byte3 = stop_cmd3.byte4 = 0x22;
	stop_cmd4.byte1 = stop_cmd4.byte2 = stop_cmd4.byte3 = stop_cmd4.byte4 = 0x33;

	sensordplib_autoi2c_stopcmd(stop_cmd1, stop_cmd2, stop_cmd3, stop_cmd4);
}
#endif

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

    dbg_printf(DBG_LESS_INFO, "Initialising HM0360\r\n");

    /*
     * common CIS init
     */

// CGP - unsure whether we are calling with the correct parameter.
// In an email of 14/4/25 Himax said:
// we use "hx_drv_scu_set_pdlsc_dpclk_cfg(SCU_PDLSC_DPCLK_CFG_T cfg)" to set mclk reference clock, this function can be removed
// However I can't see thsi call anywhere.
#ifdef MCLKUSESRC36M
	hx_drv_dp_set_mclk_src(DP_MCLK_SRC_INTERNAL, DP_MCLK_SRC_INT_SEL_RC36M);
#else
	hx_drv_dp_set_mclk_src(DP_MCLK_SRC_INTERNAL, DP_MCLK_SRC_INT_SEL_XTAL);
#endif	// MCLKUSESRC36M
//	// Himax says these are not needed (14/4/25)
//    hx_drv_cis_init(DEAULT_XHSUTDOWN_PIN, SENSORCTRL_MCLK_DIV1);
//    hx_drv_sensorctrl_set_xSleepCtrl(SENSORCTRL_XSLEEP_BY_CPU);
//    hx_drv_sensorctrl_set_xSleep(1);


    // Note that a comment in the definition of hx_drv_cis_init() syas "use API hx_drv_scu_set_pdlsc_dpclk_cfg() instead"

    hx_drv_cis_set_slaveID(CIS_I2C_ID);

    /*
     * off stream before init sensor
     */
    if(hx_drv_cis_setRegTable(HM0360_stream_off, HX_CIS_SIZE_N(HM0360_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_LESS_INFO, "HM0360 off by app fail\r\n");
        return -1;
    }

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

	if ( sensor_init == true )
	{
		// This is the long list of registers
		if(hx_drv_cis_setRegTable(HM0360_md_init_setting, HX_CIS_SIZE_N(HM0360_md_init_setting, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
		{
			dbg_printf(DBG_LESS_INFO, "HM0360 Init by app fail \r\n");
			return -1;
		}
		else
		{
			dbg_printf(DBG_LESS_INFO, "HM0360 Init by app \n");
		}

		// image orientation
		if(hx_drv_cis_setRegTable(HM0360_mirror_setting, HX_CIS_SIZE_N(HM0360_mirror_setting, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
		{
			dbg_printf(DBG_LESS_INFO, "HM0360 Init Mirror 0x%02X by app fail \r\n", HM0360_mirror_setting[0].Value);
			return -1;
		}
		else
		{
			#if (CIS_MIRROR_SETTING == 0x01)
				dbg_printf(DBG_LESS_INFO, "HM0360 Init Horizontal Mirror by app \n");
			#elif (CIS_MIRROR_SETTING == 0x02)
				dbg_printf(DBG_LESS_INFO, "HM0360 Init Vertical Mirror by app \n");
			#elif (CIS_MIRROR_SETTING == 0x03)
				dbg_printf(DBG_LESS_INFO, "HM0360 Init Horizontal & Vertical Mirror by app \n");
			#else
				dbg_printf(DBG_LESS_INFO, "HM0360 Init Mirror Off by app \n");
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

int cisdp_dp_init(bool inp_init, SENSORDPLIB_PATH_E dp_type, sensordplib_CBEvent_t cb_event, uint32_t jpg_ratio, APP_DP_INP_SUBSAMPLE_E subs)
{
    HW2x2_CFG_T hw2x2_cfg;
    CDM_CFG_T cdm_cfg;
    HW5x5_CFG_T hw5x5_cfg;
    JPEG_CFG_T jpeg_cfg;

    g_subs = subs;
    //HW2x2 Cfg
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

    //CDM Cfg
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

    //HW5x5 Cfg
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

    //JPEG Cfg
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
    if(jpg_ratio == 4)
    	jpeg_cfg.jpeg_encqtable = JPEG_ENC_QTABLE_4X;
    else
    	jpeg_cfg.jpeg_encqtable = JPEG_ENC_QTABLE_10X;

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

	if(inp_init == true) {
		if(subs == APP_DP_RES_RGB640x480_INP_SUBSAMPLE_2X||subs == APP_DP_RES_YUV640x480_INP_SUBSAMPLE_2X)
			sensordplib_set_sensorctrl_inp(SENCTRL_SENSOR_TYPE, SENCTRL_STREAM_TYPE, SENCTRL_SENSOR_WIDTH,
						SENCTRL_SENSOR_HEIGHT, INP_SUBSAMPLE_4TO2);
		else if(subs == APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X||subs == APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X)
			sensordplib_set_sensorctrl_inp(SENCTRL_SENSOR_TYPE, SENCTRL_STREAM_TYPE, SENCTRL_SENSOR_WIDTH,
						SENCTRL_SENSOR_HEIGHT, INP_SUBSAMPLE_DISABLE);
	}

	uint8_t cyclic_buffer_cnt = 1;
	int32_t non_support = 0;

	switch (dp_type)
	{
	case SENSORDPLIB_PATH_INP_WDMA2:
	    sensordplib_set_raw_wdma2(DP_INP_OUT_WIDTH, DP_INP_OUT_HEIGHT,
	    		NULL);
	    break;
	case SENSORDPLIB_PATH_INP_HW2x2_CDM:
	    sensordplib_set_HW2x2_CDM(hw2x2_cfg, cdm_cfg,
	    		NULL);
	    break;
	case SENSORDPLIB_PATH_INP_HW5x5:
	    sensordplib_set_hw5x5_wdma3(hw5x5_cfg,
	    		NULL);
	    break;
	case SENSORDPLIB_PATH_INP_HW5x5_JPEG:
	    sensordplib_set_hw5x5_jpeg_wdma2(hw5x5_cfg
	            , jpeg_cfg,
				cyclic_buffer_cnt,
				NULL);
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
	            cdm_cfg, hw5x5_cfg,
	            NULL);
		break;
	case SENSORDPLIB_PATH_INT3:
		sensordplib_set_int_raw_hw5x5_wdma23(DP_INP_OUT_WIDTH,
				DP_INP_OUT_HEIGHT,
				hw5x5_cfg,
				NULL);
		break;
	case SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG:
		if(hw5x5_cfg.demos_color_mode == DEMOS_COLORMODE_RGB)
		{
			sensordplib_set_int_hw5x5rgb_jpeg_wdma23(hw5x5_cfg,jpeg_cfg,
					cyclic_buffer_cnt,
		            NULL);
		}
		else
		{
			sensordplib_set_int_hw5x5_jpeg_wdma23(hw5x5_cfg,jpeg_cfg,
					cyclic_buffer_cnt,
					NULL);
		}
		break;
	case SENSORDPLIB_PATH_INT_INP_HW2x2_HW5x5_JPEG:
		sensordplib_set_int_hw2x2_hw5x5_jpeg_wdma12(hw2x2_cfg,
	            hw5x5_cfg,jpeg_cfg,
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

	if(non_support == 1)
		return -1;

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
#if (CIS_ENABLE_HX_AUTOI2C == 0x00)
#if (CIS_ENABLE_XSLEEP_TRIG_FRM == 0x01)
    /*
     * XSleep Control Stream On
     */
	if(hx_drv_cis_setRegTable(HM0360_stream_xsleep, HX_CIS_SIZE_N(HM0360_stream_xsleep, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
	{
		dbg_printf(DBG_LESS_INFO, "HM0360 on by app fail\r\n");
		return;
	}
#else
    /*
     * Manual Control Stream On
     */
    if(hx_drv_cis_setRegTable(HM0360_stream_on, HX_CIS_SIZE_N(HM0360_stream_on, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR) {
    	dbg_printf(DBG_LESS_INFO, "HM0360 on by app fail\r\n");
        return;
    }
#endif
#else
    /*
     * AUTO I2C Control Stream On
     */
    sensordplib_autoi2c_enable();
    dbg_printf(DBG_LESS_INFO, "hxauto i2c enable \n");
#endif

//    //Himax says this is not used 14/4/25
//    sensordplib_set_mclkctrl_xsleepctrl_bySCMode();

    // starts data path sensor control block,
   	sensordplib_set_sensorctrl_start();
}

void cisdp_sensor_stop()
{
    sensordplib_stop_capture();
    sensordplib_start_swreset();
    sensordplib_stop_swreset_WoSensorCtrl();

#if (CIS_ENABLE_HX_AUTOI2C == 0x00)
    // Writes to MODE_SELECT to select sleep
    if(hx_drv_cis_setRegTable(HM0360_stream_off, HX_CIS_SIZE_N(HM0360_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)  {
    	dbg_printf(DBG_LESS_INFO, "HM0360 off by app fail\r\n");
    }
    else  {
    	dbg_printf(DBG_LESS_INFO, "HM0360 off by app \n");
    }
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

    dbg_printf(DBG_LESS_INFO, "REDUCED cis_hm0360_md_init\r\n");

    //
    if(hx_drv_cis_setRegTable(HM0360_stream_off, HX_CIS_SIZE_N(HM0360_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR) {
    	dbg_printf(DBG_LESS_INFO, "HM0360 off by app fail\r\n");
        return -1;
    }

    // Set up the motion detect settings
	if (hx_drv_cis_setRegTable(HM0360_md_stream_on, HX_CIS_SIZE_N(HM0360_md_stream_on, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR) {
    	dbg_printf(DBG_LESS_INFO, "HM0360 md on by app fail\r\n");
        return -1;
    }

    // This version has no delay
    dbg_printf(DBG_LESS_INFO, "HM0360 Motion Detection on! (No delay)\r\n");

	return 0;
}


/*
 * Change HM0360 operating mode
 *
 * @param mode - one of 8 modes of MODE_SELECT register
 * @param numFrames - the number of frames to capture before sleeping
 * @param sleepTime - the time to sleep before waking again
 * @return error code
 */
HX_CIS_ERROR_E cisdp_sensor_set_mode(mode_select_t newMode, uint8_t numFrames, uint16_t sleepTime) {
	mode_select_t currentMode;
	HX_CIS_ERROR_E ret;

	ret = hx_drv_cis_get_reg(MODE_SELECT , &currentMode);
	if (ret != HX_CIS_NO_ERROR) {
		return ret;
	}

	xprintf("  Changing mode from %d to %d with nFrames=%d and sleepTime=%d\r\n", currentMode, newMode, numFrames, sleepTime);

	ret = hx_drv_cis_set_reg(MODE_SELECT , MODE_SLEEP, 0);
	if (ret != HX_CIS_NO_ERROR) {
		return ret;
	}

	if (numFrames != 0) {
		// Applies to MODE_SW_NFRAMES_SLEEP, MODE_SW_NFRAMES_STANDBY and MODE_HW_NFRAMES_SLEEP
		ret = hx_drv_cis_set_reg(PMU_CFG_8, numFrames, 0);
		if (ret != HX_CIS_NO_ERROR) {
			return ret;
		}
	}

	if (sleepTime != 0) {
		// Applies to MODE_SW_NFRAMES_SLEEP and MODE_HW_NFRAMES_SLEEP
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

