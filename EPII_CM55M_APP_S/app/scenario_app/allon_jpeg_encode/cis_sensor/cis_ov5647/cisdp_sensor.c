/*
 * cisdp_sensor.c
 *
 *  Created on: 20240122
 *      Author: 901912
 */

#include "cisdp_sensor.h"
#include "cisdp_cfg.h"
#include "hx_drv_CIS_common.h"
#include "hx_drv_timer.h"
#include "hx_drv_hxautoi2c_mst.h"
#include "hx_drv_CIS_common.h"

#include "WE2_core.h"
#include "hx_drv_swreg_aon.h"
#include "hx_drv_scu_export.h"
#include "driver_interface.h"
#include "hx_drv_scu.h"
#include "math.h"
#include "tpgdp_lib.h"

#define GROVE_VISION_AI

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


extern uint8_t raw_buff[];
extern uint8_t quater_raw_buff[];
extern uint8_t jpg_buff[];
extern uint8_t jpegfilesizebuf[];
extern uint8_t yuv_buff[];

static volatile uint8_t flag = 0;
static volatile uint32_t g_wdma1_baseaddr = SENDPLIB_WDMA1_ADDR;
static volatile uint32_t g_wdma2_baseaddr = (uint32_t)raw_buff;
static volatile uint32_t g_wdma3_baseaddr = (uint32_t)yuv_buff;
static volatile uint32_t g_jpegautofill_addr = (uint32_t)jpegfilesizebuf;

static uint32_t g_raw_width = 1280, g_raw_height = 960;
static volatile uint8_t hw5x5_jpeg_flag =0, hw5x5_flag =0, jpg_flag = 0;

static HX_CIS_SensorSetting_t OV5647_init_setting[] = {
#include "OV5647_mipi_2lane_1280x960.i"
};

static HX_CIS_SensorSetting_t OV5647_stream_on[] = {
		{HX_CIS_I2C_Action_W, 0x4800, OV5647_MIPI_CTRL_ON},
		{HX_CIS_I2C_Action_W, 0x4202, 0x00},
};

static HX_CIS_SensorSetting_t OV5647_stream_off[] = {
		{HX_CIS_I2C_Action_W, 0x4800, OV5647_MIPI_CTRL_OFF},
	    {HX_CIS_I2C_Action_W, 0x4202, 0x0F},
};


static void cisdp_wdma_addr_init()
{
	dbg_printf(DBG_MORE_INFO, "wdma1_addr=0x%x,wdma2_addr=0x%x,wdma3_addr=0x%x\n", g_wdma1_baseaddr, g_wdma2_baseaddr, g_wdma3_baseaddr);
	dbg_printf(DBG_MORE_INFO, "g_jpegautofill_addr=0x%x, JPEG_BUFSIZE=0x%x, RAW_BUFSIZE=0x%x\n", g_jpegautofill_addr, 115856, 1280*960);

    sensordplib_set_xDMA_baseaddrbyapp(g_wdma1_baseaddr, g_wdma2_baseaddr, g_wdma3_baseaddr);
    sensordplib_set_jpegfilesize_addrbyapp(g_jpegautofill_addr);
}


void ov5647_set_dp_rc96()
{
	SCU_PDHSC_DPCLK_CFG_T cfg;

	hx_drv_scu_get_pdhsc_dpclk_cfg(&cfg);

	cfg.mipiclk.hscmipiclksrc = SCU_HSCMIPICLKSRC_RC96M48M;
	cfg.mipiclk.hscmipiclkdiv = 0;
	cfg.dpclk = SCU_HSCDPCLKSRC_RC96M48M;

	hx_drv_scu_set_pdhsc_dpclk_cfg(cfg, 0, 1);

	uint32_t mipi_pixel_clk = 96;
	hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_HSC_MIPI_RXCLK, &mipi_pixel_clk);
	mipi_pixel_clk = mipi_pixel_clk / 1000000;

	dbg_printf(DBG_MORE_INFO, "MIPI TX CLK: %dM\n", mipi_pixel_clk);

	SCU_PDLSC_DPCLK_CFG_T lsccfg;
	hx_drv_scu_get_pdlsc_dpclk_cfg(&lsccfg);

	lsccfg.scsenmclk.lscmclkdiv = 3;
	lsccfg.scsenmclk.lscscmclksrc = SCU_LSCSCCLKSRC_RC96M48M;

	hx_drv_scu_set_pdlsc_dpclk_cfg(lsccfg);

}


void set_mipi_csirx_enable()
{
	uint32_t bitrate_1lane = OV5647_MIPI_CLOCK_FEQ*2;
	uint32_t mipi_lnno = OV5647_MIPI_LANE_CNT;
	uint32_t pixel_dpp = OV5647_MIPI_DPP;
	uint32_t line_length = OV5647_SENSOR_WIDTH;
	uint32_t frame_length = OV5647_SENSOR_HEIGHT;
	uint32_t byte_clk = bitrate_1lane/8;
	uint32_t continuousout = OV5647_MIPITX_CNTCLK_EN;
	uint32_t deskew_en = 0;
	uint32_t mipi_pixel_clk = 96;

	ov5647_set_dp_rc96();

	hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_HSC_MIPI_RXCLK, &mipi_pixel_clk);

	mipi_pixel_clk = mipi_pixel_clk / 1000000;

	dbg_printf(DBG_MORE_INFO, "MIPI CSI Init Enable\n");
	dbg_printf(DBG_MORE_INFO, "MIPI TX CLK: %dM\n", mipi_pixel_clk);
	dbg_printf(DBG_MORE_INFO, "MIPI BITRATE 1LANE: %dM\n", bitrate_1lane);
	dbg_printf(DBG_MORE_INFO, "MIPI DATA LANE: %d\n", mipi_lnno);
	dbg_printf(DBG_MORE_INFO, "MIPI PIXEL DEPTH: %d\n", pixel_dpp);
	dbg_printf(DBG_MORE_INFO, "MIPI LINE LENGTH: %d\n", line_length);
	dbg_printf(DBG_MORE_INFO, "MIPI FRAME LENGTH: %d\n", frame_length);
	dbg_printf(DBG_MORE_INFO, "MIPI CONTINUOUSOUT: %d\n", continuousout);
	dbg_printf(DBG_MORE_INFO, "MIPI DESKEW: %d\n", deskew_en);

	uint32_t n_preload = 15;
	uint32_t l_header = 4;
	uint32_t l_footer = 2;

	double t_input = (double)(l_header+line_length*pixel_dpp/8+l_footer)/(mipi_lnno*byte_clk)+0.06;
	double t_output = (double)line_length/mipi_pixel_clk;
	double t_preload = (double)(7+(n_preload*4)/mipi_lnno)/mipi_pixel_clk;

	double delta_t = t_input - t_output - t_preload;

	dbg_printf(DBG_MORE_INFO, "t_input: %dns\n", (uint32_t)(t_input*1000));
	dbg_printf(DBG_MORE_INFO, "t_output: %dns\n", (uint32_t)(t_output*1000));
	dbg_printf(DBG_MORE_INFO, "t_preload: %dns\n", (uint32_t)(t_preload*1000));


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
	dbg_printf(DBG_MORE_INFO, "MIPI RX FIFO FILL: %d\n", rx_fifo_fill);
	dbg_printf(DBG_MORE_INFO, "MIPI TX FIFO FILL: %d\n", tx_fifo_fill);

	/*
	 * Reset CSI RX/TX
	 */
	dbg_printf(DBG_MORE_INFO, "RESET MIPI CSI RX/TX\n");
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
    	dbg_printf(DBG_MORE_INFO, "PIXEL DEPTH fail %d\n", pixel_dpp);
        return;
    }

	sensordplib_csirx_set_deskew(deskew_en);
	sensordplib_csirx_set_fifo_fill(rx_fifo_fill);
    sensordplib_csirx_enable(mipi_lnno);

    dbg_printf(DBG_MORE_INFO, "VMUTE: 0x%08X\n", *(uint32_t*)(SCU_LSC_ADDR+0x408));
    dbg_printf(DBG_MORE_INFO, "0x53061000: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x1000));
    dbg_printf(DBG_MORE_INFO, "0x53061004: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x1004));
    dbg_printf(DBG_MORE_INFO, "0x53061008: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x1008));
    dbg_printf(DBG_MORE_INFO, "0x5306100C: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x100C));
    dbg_printf(DBG_MORE_INFO, "0x53061010: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x1010));

}


static void set_mipi_csirx_disable()
{
	dbg_printf(DBG_MORE_INFO, "MIPI CSI Disable\n");

    volatile uint32_t *dphy_reg = (uint32_t *)CSIRX_DPHY_REG;
    volatile uint32_t *csi_static_cfg_reg = (uint32_t *)(CSIRX_REGS_BASE+0x08);
    volatile uint32_t *csi_dphy_lane_control_reg = (uint32_t *)(CSIRX_REGS_BASE+0x40);
    volatile uint32_t *csi_stream0_control_reg = (uint32_t *)(CSIRX_REGS_BASE+0x100);
    volatile uint32_t *csi_stream0_data_cfg = (uint32_t *)(CSIRX_REGS_BASE+0x108);
    volatile uint32_t *csi_stream0_cfg_reg = (uint32_t *)(CSIRX_REGS_BASE+0x10C);

    sensordplib_csirx_disable();

    dbg_printf(DBG_MORE_INFO, "0x%08X = 0x%08X\n", dphy_reg, *dphy_reg);
    dbg_printf(DBG_MORE_INFO, "0x%08X = 0x%08X\n", csi_static_cfg_reg, *csi_static_cfg_reg);
    dbg_printf(DBG_MORE_INFO, "0x%08X = 0x%08X\n", csi_dphy_lane_control_reg, *csi_dphy_lane_control_reg);
    dbg_printf(DBG_MORE_INFO, "0x%08X = 0x%08X\n", csi_stream0_data_cfg, *csi_stream0_data_cfg);
    dbg_printf(DBG_MORE_INFO, "0x%08X = 0x%08X\n", csi_stream0_control_reg, *csi_stream0_control_reg);

}


int cisdp_sensor_init()
{
    dbg_printf(DBG_LESS_INFO, "cis_OV5647_init \r\n");

    /*
     * common CIS init
     */
    hx_drv_cis_init((CIS_XHSHUTDOWN_INDEX_E)DEAULT_XHSUTDOWN_PIN, SENSORCTRL_MCLK_DIV3);
    dbg_printf(DBG_MORE_INFO, "mclk DIV3, xshutdown_pin=%d\n",DEAULT_XHSUTDOWN_PIN);

#ifdef GROVE_VISION_AI
	//OV5647 Enable
	hx_drv_gpio_set_output(AON_GPIO1, GPIO_OUT_LOW);
    hx_drv_scu_set_PA1_pinmux(SCU_PA1_PINMUX_AON_GPIO1, 1);
	hx_drv_gpio_set_out_value(AON_GPIO1, GPIO_OUT_LOW);
    hx_drv_timer_cm55x_delay_ms(10, TIMER_STATE_DC);
    hx_drv_gpio_set_out_value(AON_GPIO1, GPIO_OUT_HIGH);
    hx_drv_timer_cm55x_delay_ms(10, TIMER_STATE_DC);
	dbg_printf(DBG_MORE_INFO, "Set PA1(AON_GPIO1) to High\n");
#else
    hx_drv_sensorctrl_set_xSleepCtrl(SENSORCTRL_XSLEEP_BY_CPU);
    hx_drv_sensorctrl_set_xSleep(1);
    dbg_printf(DBG_MORE_INFO, "hx_drv_sensorctrl_set_xSleep(1)\n");
#endif

    hx_drv_cis_set_slaveID(CIS_I2C_ID);
    dbg_printf(DBG_MORE_INFO, "hx_drv_cis_set_slaveID(0x%02X)\n", CIS_I2C_ID);
    /*
     * off stream before init sensor
     */
    if(hx_drv_cis_setRegTable(OV5647_stream_off, HX_CIS_SIZE_N(OV5647_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_MORE_INFO, "OV5647 off by app fail\r\n");
        return -1;
    }

	if(hx_drv_cis_setRegTable(OV5647_init_setting, HX_CIS_SIZE_N(OV5647_init_setting, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
	{
		dbg_printf(DBG_MORE_INFO, "OV5647 Init Stream by app fail \r\n");
		return -1;
	}
	else
	{
		dbg_printf(DBG_MORE_INFO, "OV5647 Init Stream by app \n");
	}

    return 0;
}

uint32_t copy_mem_to_mem(uint32_t src_addr, uint32_t dst_addr, uint32_t src_w, uint32_t src_h, uint32_t start_x, uint32_t start_y, uint32_t len_x, uint32_t len_y) {
	
	dbg_printf(DBG_MORE_INFO, "copy_mem_to_mem, src=0x%x, dst=0x%x\n", src_addr, dst_addr);
	dbg_printf(DBG_MORE_INFO, "src w=%d, h=%d, start_x=%d, y=%d\n", src_w, src_h, start_x, start_y);
	dbg_printf(DBG_MORE_INFO, "dst w=%d, h=%d\n", len_x, len_y);
	uint8_t *cur_src = src_addr+start_y*src_w;
	uint8_t *cur_dst = dst_addr;
	hx_InvalidateDCache_by_Addr((volatile void *)src_addr, src_w*src_h);

	for(uint32_t j=0;j<len_y;j++)
	{	
		memcpy(cur_dst, cur_src+start_x, len_x);
		cur_src += src_w;
		cur_dst += len_x;		
	}

	hx_CleanDCache_by_Addr((volatile void *)dst_addr, len_x*len_y);
	return 0;
}

void HW5x5JPEGbyTPG_RDMA_CB(XDMA_RDMASTATUS_E event)
{

	if(event == XDMA_RDMASTATUS_NORMAL_FINISH)
	{
	    if(hx_drv_xdma_RDMA_clear_Normalint() != XDMA_NO_ERROR)
	    {
			dbg_printf(DBG_MORE_INFO, "hx_drv_xdma_RDMA_clear_Normalint fail\n");
	    }
	}else{
	    if(hx_drv_xdma_RDMA_clear_AbNormalint() != XDMA_NO_ERROR)
	    {
			dbg_printf(DBG_MORE_INFO, "hx_drv_xdma_RDMA_clear_AbNormalint fail\n");
	    }
	}

	dbg_printf(DBG_MORE_INFO, "RDMA_CB event=0x%x\r\n", event);
}

void HW5x5JPEGEncbyTPG_WDMA2_CB(XDMA_WDMA2STATUS_E event)
{
	uint8_t frame_no;
    uint8_t buffer_no = 0;

	dbg_printf(DBG_MORE_INFO, "WDMA2 event=0x%x\r\n", event);

	if (event == XDMA_WDMA2STATUS_NORMAL_FINISH)
	{
	    if(hx_drv_xdma_WDMA2_clear_Normalint() != XDMA_NO_ERROR)
	    {
			dbg_printf(DBG_MORE_INFO, "hx_drv_xdma_WDMA2_clear_Normalint fail\n");
	    }
	} else {
		if(hx_drv_xdma_WDMA2_clear_AbNormalint() != XDMA_NO_ERROR)
	    {
			dbg_printf(DBG_MORE_INFO, "hx_drv_xdma_WDMA2_clear_AbNormalint fail\n");
	    }
	}
	hw5x5_jpeg_flag++;
}

void JPEGEncbyTPG_WDMA2_CB(XDMA_WDMA2STATUS_E event)
{
	dbg_printf(DBG_MORE_INFO, "WDMA2 event=0x%x\r\n", event);

	if(event == XDMA_WDMA2STATUS_NORMAL_FINISH)
	{
	    if(hx_drv_xdma_WDMA2_clear_Normalint() != XDMA_NO_ERROR)
	    {
			dbg_printf(DBG_MORE_INFO, "hx_drv_xdma_WDMA2_clear_Normalint fail\n");
	    }

	}else{
	    if(hx_drv_xdma_WDMA2_clear_AbNormalint() != XDMA_NO_ERROR)
	    {
			dbg_printf(DBG_MORE_INFO, "hx_drv_xdma_WDMA2_clear_AbNormalint fail\n");
	    }
	}
	jpg_flag++;	
}

void HW5x5byTPG_WDMA3_CB(XDMA_WDMA3STATUS_E event)
{

   dbg_printf(DBG_MORE_INFO, "WDMA3 event=0x%x\r\n", event);

    if(event == XDMA_WDMA3STATUS_NORMAL_FINISH)
    {
        if(hx_drv_xdma_WDMA3_clear_Normalint() != XDMA_NO_ERROR)
        {
            dbg_printf(DBG_MORE_INFO, "hx_drv_xdma_WDMA3_clear_Normalint fail\n");
        }
        //dbg_printf(DBG_MORE_INFO, "You can dump SRAM data to compare\r\n");
    }else{
        if(hx_drv_xdma_WDMA3_clear_AbNormalint() != XDMA_NO_ERROR)
        {
            dbg_printf(DBG_MORE_INFO, "hx_drv_xdma_WDMA3_clear_AbNormalint fail\n");
        }
    }

	hw5x5_flag++;
}

uint32_t app_mem_to_hw5x5(uint32_t raw_addr, uint32_t yuv_addr, uint32_t raw_w, uint32_t raw_h) 
{
	HW5x5_CFG_T hw5x5_cfg;
    TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg;
    TPGDPLIB_WDMA3_CFG_T tpglib_wdma3_cfg;
    TPGDPLIB_RDMA_CFG_T tpglib_rdma_cfg;
    uint32_t wdma3_ch1_startaddr = 0;
    uint32_t wdma3_ch1_size = 0;
    uint32_t wdma3_ch2_startaddr = 0;
    uint32_t wdma3_ch2_size = 0;
    uint32_t wdma3_ch3_startaddr = 0;
    uint32_t wdma3_ch3_size = 0;
    uint32_t rdma_ch1_startaddr = 0;
    uint32_t rdma_ch1_size = 0;

    //setup HW5x5 block
    if(hx_drv_hw5x5_get_defCfg(&hw5x5_cfg) != HW5x5_NO_ERROR)
    {
       dbg_printf(DBG_MORE_INFO, "hx_drv_hw5x5_get_defCfg fail\n");
    }

	hw5x5_cfg.hw5x5_path = HW5x5_PATH_THROUGH_DEMOSAIC;
	hw5x5_cfg.demos_bndmode = DEMOS_BNDODE_REFLECT;
	hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV420;
	hw5x5_cfg.demos_pattern_mode = DP_HW5X5_DEMOS_PATTERN;	//DEMOS_PATTENMODE_RGGB;
	hw5x5_cfg.demoslpf_roundmode = DEMOSLPF_ROUNDMODE_FLOOR;
	hw5x5_cfg.hw55_crop_stx = 0;
	hw5x5_cfg.hw55_crop_sty = 0;
	hw5x5_cfg.hw55_in_width = raw_w;
	hw5x5_cfg.hw55_in_height = raw_h;


    //get WDMA3 size from HW5x5 block
    if(hx_drv_hw5x5_get_OutMEMSize(hw5x5_cfg, &wdma3_ch1_size, &wdma3_ch2_size, &wdma3_ch3_size) != HW5x5_NO_ERROR)
    {
        dbg_printf(DBG_MORE_INFO, "hx_drv_hw5x5_get_OutMEMSize fail\n");
    }
    dbg_printf(DBG_MORE_INFO, "output wdma3_ch1 size= 0x%08x\n", wdma3_ch1_size);
    dbg_printf(DBG_MORE_INFO, "output wdma3_ch2 size= 0x%08x\n", wdma3_ch2_size);
    dbg_printf(DBG_MORE_INFO, "output wdma3_ch3 size= 0x%08x\n", wdma3_ch3_size);
    wdma3_ch1_startaddr = yuv_addr;
    wdma3_ch2_startaddr = wdma3_ch1_startaddr + wdma3_ch1_size;
    wdma3_ch3_startaddr = wdma3_ch2_startaddr + wdma3_ch2_size;
    rdma_ch1_startaddr = raw_addr;
    dbg_printf(DBG_MORE_INFO, "output wdma3_ch1 addr= 0x%08x\n", wdma3_ch1_startaddr);
    dbg_printf(DBG_MORE_INFO, "output wdma3_ch2 addr= 0x%08x\n", wdma3_ch2_startaddr);
    dbg_printf(DBG_MORE_INFO, "output wdma3_ch3 addr= 0x%08x\n", wdma3_ch3_startaddr);
    dbg_printf(DBG_MORE_INFO, "input rdma_ch1 addr= 0x%08x\n", rdma_ch1_startaddr);

    tpg_cfg.tpg_width = raw_w;
    tpg_cfg.tpg_height = raw_h;
    tpg_cfg.blanking_autoset = 1;
    tpglib_wdma3_cfg.wdma3_ch1_startaddr = wdma3_ch1_startaddr;
    tpglib_wdma3_cfg.wdma3_ch2_startaddr = wdma3_ch2_startaddr;
    tpglib_wdma3_cfg.wdma3_ch3_startaddr = wdma3_ch3_startaddr;
    tpglib_wdma3_cfg.finmode = XDMA_WDMA3_FINSRC_THR;
    tpglib_rdma_cfg.rdma_ch1_startaddr = rdma_ch1_startaddr;
    tpglib_rdma_cfg.rdma_ch1_size = raw_w*raw_h;
    tpglib_rdma_cfg.finmode = XDMA_RDMA_FINSRC_THR;
    // init_wait_start_process();
	hw5x5_flag = 0;

	setup_tpgdp_hw5x5_wdma3(
		hw5x5_cfg,
		tpg_cfg,
		tpglib_wdma3_cfg,
		tpglib_rdma_cfg,
		HW5x5byTPG_WDMA3_CB,
		HW5x5JPEGbyTPG_RDMA_CB);

	return 0;
}

uint32_t app_mem_to_jpg(uint32_t yuv_addr, uint32_t jpg_addr, uint32_t w, uint32_t h)
{
	/*
     * YUV420
     */
    JPEG_CFG_T jpeg_cfg;
    TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg;
    TPGDPLIB_WDMA2_CFG_T tpglib_wdma2_cfg;
    TPGDPLIB_RDMA_CFG_T tpglib_rdma_cfg;
    uint32_t wdma2_size = 0;

    uint32_t rdma_ch1_size = w*h;
    uint32_t rdma_ch2_size = rdma_ch1_size >> 2;
    uint32_t rdma_ch3_size = rdma_ch1_size >> 2;

    //setup JPEG Enc block
    if(hx_drv_jpeg_get_defCfg(&jpeg_cfg) != JPEG_NO_ERROR)
    {
       dbg_printf(DBG_MORE_INFO, "hx_drv_jpeg_get_defCfg fail\n");
    }

    jpeg_cfg.jpeg_path = JPEG_PATH_ENCODER_EN;
    jpeg_cfg.enc_width = w;
    jpeg_cfg.enc_height = h;
    jpeg_cfg.jpeg_enctype = JPEG_ENC_TYPE_YUV420;
    jpeg_cfg.jpeg_encqtable = JPEG_ENC_QTABLE_4X;

    if(hx_drv_jpeg_get_EncOutMaxMEMSize(jpeg_cfg, &wdma2_size)!= JPEG_NO_ERROR)
    {
       dbg_printf(DBG_MORE_INFO, "hx_drv_jpeg_get_EncOutMaxMEMSize fail\n");
    }
    else
        dbg_printf(DBG_MORE_INFO, "wdma2_size: %d\n", wdma2_size);

    tpg_cfg.tpg_width = w;
    tpg_cfg.tpg_height = h;
    tpg_cfg.blanking_autoset = 1;
    tpglib_wdma2_cfg.dma_mode = XDMA_WDMA2_DMAMODE_SNAPSHOT;
    tpglib_wdma2_cfg.wdma2_startaddr = jpg_addr;
    tpglib_wdma2_cfg.wdma2_offset = wdma2_size;
    tpglib_wdma2_cfg.wdma2_startcnt = 0;
    tpglib_wdma2_cfg.wdma2_tgtloop = 10;
    tpglib_wdma2_cfg.fin_mode = XDMA_WDMA2_SNAPSHOT_FINMODE_FE;

    tpglib_rdma_cfg.rdma_ch1_startaddr = yuv_addr;
    tpglib_rdma_cfg.rdma_ch1_size = rdma_ch1_size;
    tpglib_rdma_cfg.rdma_ch2_startaddr = (yuv_addr+rdma_ch1_size);
    tpglib_rdma_cfg.rdma_ch2_size = rdma_ch2_size;
    tpglib_rdma_cfg.rdma_ch3_startaddr = (yuv_addr+rdma_ch1_size+rdma_ch2_size);
    tpglib_rdma_cfg.rdma_ch3_size = rdma_ch3_size;
    tpglib_rdma_cfg.finmode = XDMA_RDMA_FINSRC_THR;
    
	jpg_flag = 0;

    setup_tpgdp_jpegenc_wdma2(
            jpeg_cfg
            , tpg_cfg
            , tpglib_wdma2_cfg
            , tpglib_rdma_cfg
            , JPEGEncbyTPG_WDMA2_CB
            , HW5x5JPEGbyTPG_RDMA_CB);

	return 0;

}

uint32_t app_mem_to_hw5x5_jpg(uint32_t raw_addr, uint32_t jpg_addr, uint32_t raw_w, uint32_t raw_h)
{
	JPEG_CFG_T jpeg_cfg;
	HW5x5_CFG_T hw5x5_cfg;
	TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg;
	TPGDPLIB_WDMA2_CFG_T tpglib_wdma2_cfg;
	TPGDPLIB_RDMA_CFG_T tpglib_rdma_cfg;
	uint32_t wdma2_size = 0;

	uint8_t frame_no;
    uint8_t buffer_no = 0;

	dbg_printf(DBG_MORE_INFO, "raw_addr:0x%x, jpg_addr:0x%x\n", raw_addr, jpg_addr);

	//setup HW5x5 block
	if(hx_drv_hw5x5_get_defCfg(&hw5x5_cfg) != HW5x5_NO_ERROR)
	{
 	   dbg_printf(DBG_MORE_INFO, "hx_drv_hw5x5_get_defCfg fail\n");
	}
	hw5x5_cfg.hw5x5_path = HW5x5_PATH_THROUGH_DEMOSAIC;
	hw5x5_cfg.demos_bndmode = DEMOS_BNDODE_REFLECT;
	hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV420;
	hw5x5_cfg.demos_pattern_mode = DEMOS_PATTENMODE_RGGB;
	hw5x5_cfg.demoslpf_roundmode = DEMOSLPF_ROUNDMODE_FLOOR;
	hw5x5_cfg.hw55_crop_stx = 0;
	hw5x5_cfg.hw55_crop_sty = 0;
	hw5x5_cfg.hw55_in_width = raw_w;
	hw5x5_cfg.hw55_in_height = raw_h;

	//setup JPEG Enc block
	if(hx_drv_jpeg_get_defCfg(&jpeg_cfg) != JPEG_NO_ERROR)
	{
 	   dbg_printf(DBG_MORE_INFO, "hx_drv_jpeg_get_defCfg fail\n");
	}
	jpeg_cfg.jpeg_path = JPEG_PATH_ENCODER_EN;
	jpeg_cfg.enc_width = raw_w;
	jpeg_cfg.enc_height = raw_h;
	jpeg_cfg.jpeg_enctype = JPEG_ENC_TYPE_YUV420;
	jpeg_cfg.jpeg_encqtable = JPEG_ENC_QTABLE_4X;

	if(hx_drv_jpeg_get_EncOutMaxMEMSize(jpeg_cfg, &wdma2_size)!= JPEG_NO_ERROR)
	{
		dbg_printf(DBG_MORE_INFO, "hx_drv_jpeg_get_EncOutMaxMEMSize fail\n");
	}
	else
		dbg_printf(DBG_MORE_INFO, "wdma2_size: %d\n", wdma2_size);


	tpg_cfg.tpg_width = raw_w;
	tpg_cfg.tpg_height = raw_h;
	tpg_cfg.blanking_autoset = 1;
	tpglib_wdma2_cfg.dma_mode = XDMA_WDMA2_DMAMODE_SNAPSHOT;
	tpglib_wdma2_cfg.wdma2_startaddr = jpg_addr;
	tpglib_wdma2_cfg.wdma2_offset = wdma2_size;
	tpglib_wdma2_cfg.wdma2_startcnt = 0;
	tpglib_wdma2_cfg.wdma2_tgtloop = 20;
	tpglib_wdma2_cfg.fin_mode = XDMA_WDMA2_SNAPSHOT_FINMODE_FE;

	tpglib_rdma_cfg.rdma_ch1_startaddr = raw_addr;
	tpglib_rdma_cfg.rdma_ch1_size = raw_w*raw_h;
	tpglib_rdma_cfg.finmode = XDMA_RDMA_FINSRC_THR;
	//TPG Htotal
	//wdma2 startaddr
	hw5x5_jpeg_flag =0;

	setup_tpgdp_hw5x5_jpeg_wdma2(
			hw5x5_cfg
			,jpeg_cfg
			,tpg_cfg
			,tpglib_wdma2_cfg
			,tpglib_rdma_cfg
			,HW5x5JPEGEncbyTPG_WDMA2_CB
			,HW5x5JPEGbyTPG_RDMA_CB);

	return 0;
}

int cisdp_dp_init(bool inp_init, CISDP_INIT_TYPE_E type, SENSORDPLIB_PATH_E dp_type, sensordplib_CBEvent_t dplib_cb, uint32_t jpg_ratio)
{
    HW2x2_CFG_T hw2x2_cfg;
    CDM_CFG_T cdm_cfg;
    HW5x5_CFG_T hw5x5_cfg;
    JPEG_CFG_T jpeg_cfg;

    //HW2x2 Cfg
    hw2x2_cfg.hw2x2_path = DP_HW2X2_PATH;
    hw2x2_cfg.hw_22_process_mode = DP_HW2X2_PROCESS_MODE;
    hw2x2_cfg.hw_22_crop_stx = DP_HW2X2_CROP_START_X;
    hw2x2_cfg.hw_22_crop_sty = DP_HW2X2_CROP_START_Y;
    hw2x2_cfg.hw_22_in_width = DP_HW2X2_CROP_WIDTH;
    hw2x2_cfg.hw_22_in_height = DP_HW2X2_CROP_HEIGHT;
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
   	hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV420;
    hw5x5_cfg.demos_pattern_mode = DP_HW5X5_DEMOS_PATTERN;
    hw5x5_cfg.demoslpf_roundmode = DP_HW5X5_DEMOSLPF_ROUNDMODE;
    hw5x5_cfg.hw55_crop_stx = 0;
    hw5x5_cfg.hw55_crop_sty = 0;
    hw5x5_cfg.hw55_in_width = 640;
    hw5x5_cfg.hw55_in_height = 480;

    //JPEG Cfg
    jpeg_cfg.jpeg_path = DP_JPEG_PATH;
    jpeg_cfg.enc_width = 640;
    jpeg_cfg.enc_height = 480;
	jpeg_cfg.jpeg_enctype = JPEG_ENC_TYPE_YUV420;	
	if(jpg_ratio == 4)
    	jpeg_cfg.jpeg_encqtable = JPEG_ENC_QTABLE_4X;
    else
    	jpeg_cfg.jpeg_encqtable = JPEG_ENC_QTABLE_10X;

    cisdp_wdma_addr_init();

    //setup MIPI RX
	set_mipi_csirx_enable();

    INP_CROP_T crop;

	if (type == CISDP_INIT_TYPE_INP_CROP_1280x960_RAW) {
		// 1280x960--> (0~1279)x(0~959)
		crop.start_x = 0;
		crop.start_y = 0;
		crop.last_x = g_raw_width - 1;
		crop.last_y = g_raw_height - 1;
		sensordplib_set_sensorctrl_inp_wi_crop(SENCTRL_SENSOR_TYPE, SENCTRL_STREAM_TYPE,
			SENCTRL_SENSOR_WIDTH, SENCTRL_SENSOR_HEIGHT, INP_SUBSAMPLE_DISABLE, crop);
	}
	else if(type == CISDP_INIT_TYPE_INP_CROP_1280x960_CH_BINNING_2X_JPG) {
		crop.start_x = 0; //  1280x960
		crop.start_y = 0;
		crop.last_x = 0;
		crop.last_y = 0;
		sensordplib_set_sensorctrl_inp_wi_crop_bin(SENCTRL_SENSOR_TYPE, SENCTRL_STREAM_TYPE,
			SENCTRL_SENSOR_WIDTH, SENCTRL_SENSOR_HEIGHT, INP_SUBSAMPLE_DISABLE, crop, INP_BINNING_4TO2_B);		
	}

	uint8_t cyclic_buffer_cnt = 1;

	int32_t non_support = 0;
	switch (dp_type)
	{
	case SENSORDPLIB_PATH_INP_WDMA2:
	    sensordplib_set_raw_wdma2(g_raw_width, g_raw_height,
	    		dplib_cb);
	    break;
	case SENSORDPLIB_PATH_INP_HW2x2_CDM:
	    sensordplib_set_HW2x2_CDM(hw2x2_cfg, cdm_cfg,
	    		dplib_cb);
	    break;
	case SENSORDPLIB_PATH_INP_HW5x5:
	    sensordplib_set_hw5x5_wdma3(hw5x5_cfg,
	    		dplib_cb);
	    break;
	case SENSORDPLIB_PATH_INP_HW5x5_JPEG:
	    sensordplib_set_hw5x5_jpeg_wdma2(hw5x5_cfg
	            , jpeg_cfg,
				cyclic_buffer_cnt,
				dplib_cb);
	    break;
	case SENSORDPLIB_PATH_INP_HW2x2:
		sensordplib_set_HW2x2_wdma1(hw2x2_cfg, dplib_cb);
		break;
	case SENSORDPLIB_PATH_INP_CDM:
		sensordplib_set_CDM(cdm_cfg, dplib_cb);
		break;
	case SENSORDPLIB_PATH_INT1:
	    sensordplib_set_INT1_HWACC(hw2x2_cfg,
	            cdm_cfg, hw5x5_cfg,jpeg_cfg,
				cyclic_buffer_cnt,
	            dplib_cb);
	    break;
	case SENSORDPLIB_PATH_INTNOJPEG:
		sensordplib_set_INTNoJPEG_HWACC(hw2x2_cfg,
	            cdm_cfg, hw5x5_cfg,
	            dplib_cb);
		break;
	case SENSORDPLIB_PATH_INT3:
		sensordplib_set_int_raw_hw5x5_wdma23(DP_INP_OUT_WIDTH,
				DP_INP_OUT_HEIGHT,
				hw5x5_cfg,
				dplib_cb);
		break;
	case SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG:
		if(hw5x5_cfg.demos_color_mode == DEMOS_COLORMODE_RGB)
		{
			sensordplib_set_int_hw5x5rgb_jpeg_wdma23(hw5x5_cfg,jpeg_cfg,
					cyclic_buffer_cnt,
		            dplib_cb);
		}
		else
		{
			sensordplib_set_int_hw5x5_jpeg_wdma23(hw5x5_cfg,jpeg_cfg,
					cyclic_buffer_cnt,
					dplib_cb);
		}
		break;
	case SENSORDPLIB_PATH_INT_INP_HW2x2_HW5x5_JPEG:
		sensordplib_set_int_hw2x2_hw5x5_jpeg_wdma12(hw2x2_cfg,
	            hw5x5_cfg,jpeg_cfg,
				cyclic_buffer_cnt,
	            dplib_cb);
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
		dbg_printf(DBG_MORE_INFO, "Not support case \r\n");
		non_support = 1;
		break;
	}

	if (non_support == 1)
		return -1;

	return 0;
}

void cisdp_sensor_start()
{
    /*
     * Default Stream On
     */
    if(hx_drv_cis_setRegTable(OV5647_stream_on, HX_CIS_SIZE_N(OV5647_stream_on, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_MORE_INFO, "OV5647 on by app fail\r\n");
        return;
    }
    else
    {
    	dbg_printf(DBG_MORE_INFO, "OV5647 on by app done\r\n");
    }

    sensordplib_set_mclkctrl_xsleepctrl_bySCMode();

    sensordplib_set_sensorctrl_start();
}

void cisdp_sensor_stop()
{
    sensordplib_stop_capture();
    sensordplib_start_swreset();
    sensordplib_stop_swreset_WoSensorCtrl();

    /*
     * Stream Off
     */
    if(hx_drv_cis_setRegTable(OV5647_stream_off, HX_CIS_SIZE_N(OV5647_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_MORE_INFO, "OV5647 off by app fail\r\n");
    }
    else
    {
    	dbg_printf(DBG_MORE_INFO, "OV5647 off by app \n");
    }

    set_mipi_csirx_disable();
}

void cisdp_get_jpginfo(uint32_t *jpeg_enc_filesize, uint32_t *jpeg_enc_addr)
{
    uint8_t frame_no;
    uint8_t buffer_no = 0;
    hx_drv_xdma_get_WDMA2_bufferNo(&buffer_no);
    hx_drv_xdma_get_WDMA2NextFrameIdx(&frame_no);
    uint32_t reg_val=0, mem_val=0;
    if(frame_no == 0)
    {
        frame_no = buffer_no - 1;
    }else{
        frame_no = frame_no - 1;
    }

    hx_drv_jpeg_get_EncOutRealMEMSize(&reg_val);
    dbg_printf(DBG_MORE_INFO, "current jpeg_size=0x%x\n", reg_val);

    hx_drv_jpeg_get_FillFileSizeToMem(frame_no, (uint32_t)g_jpegautofill_addr, &mem_val);
    hx_drv_jpeg_get_MemAddrByFrameNo(frame_no, g_wdma2_baseaddr, jpeg_enc_addr);

    //*jpeg_enc_filesize = mem_val > reg_val?reg_val:mem_val;
    *jpeg_enc_filesize = mem_val == reg_val?mem_val:reg_val;

    dbg_printf(DBG_MORE_INFO, "current frame_no=%d, jpeg_size=0x%x,addr=0x%x\n",frame_no,*jpeg_enc_filesize,*jpeg_enc_addr);
}

uint32_t app_get_jpeg_addr()
{
	return (uint32_t)&jpg_buff[0];
}

uint32_t app_get_jpeg_sz()
{
    hx_InvalidateDCache_by_Addr((volatile void *)g_jpegautofill_addr, 32);
	return *((uint32_t*)g_jpegautofill_addr);
}

uint32_t app_get_raw_addr()
{
	return g_wdma3_baseaddr;
}

uint32_t app_get_raw_channels() {
	return SENCTRL_SENSOR_CH;
}

uint32_t cisdp_get_wdma1_sram_addr()
{
	return g_wdma1_baseaddr;
}

uint32_t cisdp_get_wdma2_sram_addr()
{
	return g_wdma2_baseaddr;
}

uint32_t cisdp_get_wdma3_sram_addr()
{
	return g_wdma3_baseaddr;
}

uint32_t cisdp_get_jpegautofill_sram_addr()
{
	return g_jpegautofill_addr;
}

uint32_t cisdp_get_raw_addr()
{
	return (uint32_t)&raw_buff[0];
}

uint32_t cisdp_get_quater_raw_addr()
{
	return (uint32_t)&quater_raw_buff[0];
}

uint32_t cisdp_get_yuv_addr()
{
	return (uint32_t)&yuv_buff[0];
}

uint32_t cisdp_get_raw_width()
{
	return g_raw_width;
}

uint32_t cisdp_get_raw_height()
{
	return g_raw_height;
}

uint8_t cisdp_get_hw5x5_jpeg_flag() {
	return hw5x5_jpeg_flag;
}

uint8_t cisdp_get_hw5x5_flag() {
	return hw5x5_flag;
}

uint8_t cisdp_get_jpg_flag() {
	return jpg_flag;
}
