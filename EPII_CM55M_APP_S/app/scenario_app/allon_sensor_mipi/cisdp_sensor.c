/*
 * cisdp_sensor.c
 *
 *  Created on: 2022�~11��18��
 *      Author: 901912
 */

#include "cisdp_sensor.h"
#include "cisdp_cfg.h"
#include "hx_drv_CIS_common.h"
#include "hx_drv_timer.h"
#include "hx_drv_hxautoi2c_mst.h"
#include "hx_drv_CIS_common.h"

#include "WE2_core.h"
#include "hx_drv_scu_export.h"
#include "memory_manage.h"
#include "hx_drv_swreg_aon.h"
#include "driver_interface.h"

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


#define DYNAMIC_ADDRESS 1
#ifdef DYNAMIC_ADDRESS
//WDMA2 - jpeg data
//WDMA3 - raw data
//jpegsize_autofill


//static uint8_t img_data[4] __attribute__((section(".img_data")));
//#define jpg_data_sz 	DP_JPEG_ENC_WIDTH*DP_JPEG_ENC_HEIGHT/4
//#define raw_data_sz 	DP_INP_OUT_WIDTH*DP_INP_OUT_HEIGHT*SENCTRL_SENSOR_CH

//uint8_t jpg_data[jpg_data_sz] = {0};
//uint8_t raw_data[raw_data_sz] = {0};
//uint8_t jpg_info_data[100] = {0};
//static volatile uint32_t 	g_wdma1_baseaddr = (uint32_t)jpg_data;//(uint32_t)0x34090400;
//static volatile uint32_t 	g_wdma2_baseaddr = (uint32_t)jpg_data;//(uint32_t)0x34090400;
//static volatile uint32_t 	g_wdma3_baseaddr= (uint32_t)raw_data;//(uint32_t)0x34100000;
//static volatile uint32_t 	g_jpegautofill_addr = (uint32_t)jpg_info_data;//(uint32_t)0x34090300;
static volatile uint32_t 	g_wdma1_baseaddr;//(uint32_t)0x34090400;
static volatile uint32_t 	g_wdma2_baseaddr = 0x340a0064;//(uint32_t)0x34090400;
static volatile uint32_t 	g_wdma3_baseaddr = 0x340b2c64;//(uint32_t)0x34100000;
static volatile uint32_t 	g_jpegautofill_addr;//(uint32_t)0x34090300;
#else
static volatile uint32_t 	g_wdma1_baseaddr = SENDPLIB_WDMA1_ADDR;
static volatile uint32_t 	g_wdma2_baseaddr = SENDPLIB_WDMA2_ADDR;
static volatile uint32_t 	g_wdma3_baseaddr= SENDPLIB_WDMA3_ADDR;
static volatile uint32_t 	g_jpegautofill_addr = SENDPLIB_JPEG_YUV400_AUTOFILL_ADDR;
#endif


static HX_CIS_SensorSetting_t IMX681_init_setting[] = {
		{HX_CIS_I2C_Action_W, 0x0136,	0x18},
		{HX_CIS_I2C_Action_W, 0x0137,	0x00},
		{HX_CIS_I2C_Action_W, 0x002C,	0x06},
		{HX_CIS_I2C_Action_W, 0x002D,	0x04},
		{HX_CIS_I2C_Action_W, 0x0111,	0x02},
		{HX_CIS_I2C_Action_W, 0x30EB,	0x05},
		{HX_CIS_I2C_Action_W, 0x30EB,	0x0C},
		{HX_CIS_I2C_Action_W, 0x300A,	0xFF},
		{HX_CIS_I2C_Action_W, 0x300B,	0xFF},
		{HX_CIS_I2C_Action_W, 0x3532,	0xFF},
		{HX_CIS_I2C_Action_W, 0x3533,	0xFF},
		{HX_CIS_I2C_Action_W, 0x051E,	0x00},
		{HX_CIS_I2C_Action_W, 0x0905,	0x04},
		{HX_CIS_I2C_Action_W, 0x2029,	0x01},
		{HX_CIS_I2C_Action_W, 0x202A,	0x11},
		{HX_CIS_I2C_Action_W, 0x20A1,	0x00},
		{HX_CIS_I2C_Action_W, 0x20A2,	0x02},
		{HX_CIS_I2C_Action_W, 0x20A3,	0x03},
		{HX_CIS_I2C_Action_W, 0x20AC,	0x01},
		{HX_CIS_I2C_Action_W, 0x20AD,	0x01},
		{HX_CIS_I2C_Action_W, 0x20AE,	0x01},
		{HX_CIS_I2C_Action_W, 0x20AF,	0x01},
		{HX_CIS_I2C_Action_W, 0x20B0,	0x00},
		{HX_CIS_I2C_Action_W, 0x20B1,	0x01},
		{HX_CIS_I2C_Action_W, 0x20B2,	0x02},
		{HX_CIS_I2C_Action_W, 0x20B3,	0x03},
		{HX_CIS_I2C_Action_W, 0x706F,	0x00},
		{HX_CIS_I2C_Action_W, 0x7130,	0x08},
		{HX_CIS_I2C_Action_W, 0x7131,	0x08},
		{HX_CIS_I2C_Action_W, 0x7408,	0x89},
		{HX_CIS_I2C_Action_W, 0x7437,	0x3D},
		{HX_CIS_I2C_Action_W, 0x7439,	0x29},
		{HX_CIS_I2C_Action_W, 0x7443,	0x38},
		{HX_CIS_I2C_Action_W, 0x7447,	0x55},
		{HX_CIS_I2C_Action_W, 0x744B,	0x00},
		{HX_CIS_I2C_Action_W, 0x7451,	0x8E},
		{HX_CIS_I2C_Action_W, 0x746D,	0x29},
		{HX_CIS_I2C_Action_W, 0x747D,	0x68},
		{HX_CIS_I2C_Action_W, 0x7481,	0x60},
		{HX_CIS_I2C_Action_W, 0x7491,	0x2D},
		{HX_CIS_I2C_Action_W, 0x7493,	0x31},
		{HX_CIS_I2C_Action_W, 0x74A5,	0x52},
		{HX_CIS_I2C_Action_W, 0x74AF,	0x4A},
		{HX_CIS_I2C_Action_W, 0x74B5,	0x1F},
		{HX_CIS_I2C_Action_W, 0x74B7,	0x31},
		{HX_CIS_I2C_Action_W, 0x74BD,	0x75},
		{HX_CIS_I2C_Action_W, 0x74C5,	0x06},
		{HX_CIS_I2C_Action_W, 0x74C9,	0x52},
		{HX_CIS_I2C_Action_W, 0x74D3,	0x4A},
		{HX_CIS_I2C_Action_W, 0x74D9,	0x1F},
		{HX_CIS_I2C_Action_W, 0x74DB,	0x31},
		{HX_CIS_I2C_Action_W, 0x74E1,	0x75},
		{HX_CIS_I2C_Action_W, 0x74E9,	0x06},
		{HX_CIS_I2C_Action_W, 0x74ED,	0x52},
		{HX_CIS_I2C_Action_W, 0x74F7,	0x4A},
		{HX_CIS_I2C_Action_W, 0x74FD,	0x1F},
		{HX_CIS_I2C_Action_W, 0x74FF,	0x31},
		{HX_CIS_I2C_Action_W, 0x7505,	0x75},
		{HX_CIS_I2C_Action_W, 0x750D,	0x06},
		{HX_CIS_I2C_Action_W, 0x7537,	0x38},
		{HX_CIS_I2C_Action_W, 0x753D,	0x4A},
		{HX_CIS_I2C_Action_W, 0x753F,	0x4A},
		{HX_CIS_I2C_Action_W, 0x7541,	0x4A},
		{HX_CIS_I2C_Action_W, 0x7549,	0x8E},
		{HX_CIS_I2C_Action_W, 0x754F,	0x75},
		{HX_CIS_I2C_Action_W, 0x7551,	0x75},
		{HX_CIS_I2C_Action_W, 0x7553,	0x75},
		{HX_CIS_I2C_Action_W, 0x792B,	0x39},
		{HX_CIS_I2C_Action_W, 0x792D,	0x43},
		{HX_CIS_I2C_Action_W, 0x79D3,	0x25},
		{HX_CIS_I2C_Action_W, 0x79D6,	0x8E},
		{HX_CIS_I2C_Action_W, 0x79D7,	0x01},
		{HX_CIS_I2C_Action_W, 0x79D8,	0xE7},
		{HX_CIS_I2C_Action_W, 0x79D9,	0x25},
		{HX_CIS_I2C_Action_W, 0x79DB,	0x76},
		{HX_CIS_I2C_Action_W, 0x79DC,	0x8E},
		{HX_CIS_I2C_Action_W, 0x79DD,	0x01},
		{HX_CIS_I2C_Action_W, 0x79DE,	0xE7},
		{HX_CIS_I2C_Action_W, 0x79DF,	0x25},
		{HX_CIS_I2C_Action_W, 0x79E1,	0x76},
		{HX_CIS_I2C_Action_W, 0x79E2,	0x8E},
		{HX_CIS_I2C_Action_W, 0x79E3,	0x01},
		{HX_CIS_I2C_Action_W, 0x79E4,	0xE7},
		{HX_CIS_I2C_Action_W, 0x79E5,	0x25},
		{HX_CIS_I2C_Action_W, 0x79E7,	0x76},
		{HX_CIS_I2C_Action_W, 0x79E8,	0x8E},
		{HX_CIS_I2C_Action_W, 0x7A01,	0xFF},
		{HX_CIS_I2C_Action_W, 0x7A29,	0x6C},
		{HX_CIS_I2C_Action_W, 0x7A2B,	0xDA},
		{HX_CIS_I2C_Action_W, 0x7A34,	0x6C},
		{HX_CIS_I2C_Action_W, 0x7A37,	0xDA},
		{HX_CIS_I2C_Action_W, 0x7A40,	0x6C},
		{HX_CIS_I2C_Action_W, 0x7A43,	0xDA},
		{HX_CIS_I2C_Action_W, 0x7B08,	0x00},
		{HX_CIS_I2C_Action_W, 0x7B09,	0x01},
		{HX_CIS_I2C_Action_W, 0x7C03,	0x38},
		{HX_CIS_I2C_Action_W, 0x7C09,	0x4A},
		{HX_CIS_I2C_Action_W, 0x7C0B,	0x4A},
		{HX_CIS_I2C_Action_W, 0x7C0D,	0x4A},
		{HX_CIS_I2C_Action_W, 0x7C13,	0x8E},
		{HX_CIS_I2C_Action_W, 0x7C19,	0x75},
		{HX_CIS_I2C_Action_W, 0x7C1B,	0x75},
		{HX_CIS_I2C_Action_W, 0x7C1D,	0x75},
		{HX_CIS_I2C_Action_W, 0x7C90,	0x00},
		{HX_CIS_I2C_Action_W, 0x7C91,	0x00},
		{HX_CIS_I2C_Action_W, 0x7C92,	0x00},
		{HX_CIS_I2C_Action_W, 0x7C9D,	0x01},
		{HX_CIS_I2C_Action_W, 0x7C9E,	0x01},
		{HX_CIS_I2C_Action_W, 0x7C9F,	0x01},
		{HX_CIS_I2C_Action_W, 0x7E9B,	0x07},
		{HX_CIS_I2C_Action_W, 0x7F09,	0x00},
		{HX_CIS_I2C_Action_W, 0x7F36,	0x00},
		{HX_CIS_I2C_Action_W, 0x7F4F,	0x0A},
		{HX_CIS_I2C_Action_W, 0x7F50,	0x0A},
		{HX_CIS_I2C_Action_W, 0x7F51,	0x0A},
		{HX_CIS_I2C_Action_W, 0x7F55,	0x05},
		{HX_CIS_I2C_Action_W, 0x7F56,	0x05},
		{HX_CIS_I2C_Action_W, 0x7F57,	0x05},
		{HX_CIS_I2C_Action_W, 0x7F5B,	0x03},
		{HX_CIS_I2C_Action_W, 0x7F5C,	0x03},
		{HX_CIS_I2C_Action_W, 0x7F5D,	0x03},
		{HX_CIS_I2C_Action_W, 0x7F61,	0x03},
		{HX_CIS_I2C_Action_W, 0x7F62,	0x03},
		{HX_CIS_I2C_Action_W, 0x7F63,	0x03},
		{HX_CIS_I2C_Action_W, 0x7F67,	0x03},
		{HX_CIS_I2C_Action_W, 0x7F68,	0x03},
		{HX_CIS_I2C_Action_W, 0x7F69,	0x03},
		{HX_CIS_I2C_Action_W, 0x7F6A,	0x05},
		{HX_CIS_I2C_Action_W, 0x7F6B,	0x05},
		{HX_CIS_I2C_Action_W, 0x7F6C,	0x05},
		{HX_CIS_I2C_Action_W, 0x7F6D,	0x11},
		{HX_CIS_I2C_Action_W, 0x7F6E,	0x14},
		{HX_CIS_I2C_Action_W, 0x7F6F,	0x14},
		{HX_CIS_I2C_Action_W, 0x7F73,	0x14},
		{HX_CIS_I2C_Action_W, 0x7F74,	0x1C},
		{HX_CIS_I2C_Action_W, 0x7F75,	0x14},
		{HX_CIS_I2C_Action_W, 0x7F76,	0x08},
		{HX_CIS_I2C_Action_W, 0x7F79,	0x14},
		{HX_CIS_I2C_Action_W, 0x7F7A,	0x1C},
		{HX_CIS_I2C_Action_W, 0x7F7B,	0x14},
		{HX_CIS_I2C_Action_W, 0x7F7F,	0x1C},
		{HX_CIS_I2C_Action_W, 0x7F80,	0x1C},
		{HX_CIS_I2C_Action_W, 0x7F81,	0x1C},
		{HX_CIS_I2C_Action_W, 0x7F85,	0x1C},
		{HX_CIS_I2C_Action_W, 0x7F86,	0x1C},
		{HX_CIS_I2C_Action_W, 0x7F87,	0x1C},
		{HX_CIS_I2C_Action_W, 0x7F9D,	0x09},
		{HX_CIS_I2C_Action_W, 0x7F9E,	0x09},
		{HX_CIS_I2C_Action_W, 0x7F9F,	0x09},
		{HX_CIS_I2C_Action_W, 0x7FA3,	0x09},
		{HX_CIS_I2C_Action_W, 0x7FA4,	0x09},
		{HX_CIS_I2C_Action_W, 0x7FA5,	0x09},
		{HX_CIS_I2C_Action_W, 0x7FAC,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FAD,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FAE,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FAF,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FB0,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FB1,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FB2,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FB3,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FB4,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FB5,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FB6,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FB7,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FB8,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FB9,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FBA,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FBB,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FBC,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FBD,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FBE,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FBF,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FC0,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FC1,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FC2,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FC3,	0x00},
		{HX_CIS_I2C_Action_W, 0x7FCB,	0x37},
		{HX_CIS_I2C_Action_W, 0x7FCD,	0x37},
		{HX_CIS_I2C_Action_W, 0x7FCF,	0x37},
		{HX_CIS_I2C_Action_W, 0x7FD7,	0x44},
		{HX_CIS_I2C_Action_W, 0x7FD9,	0x44},
		{HX_CIS_I2C_Action_W, 0x7FDB,	0x44},
		{HX_CIS_I2C_Action_W, 0x7FDD,	0x38},
		{HX_CIS_I2C_Action_W, 0x7FE3,	0x4A},
		{HX_CIS_I2C_Action_W, 0x7FE5,	0x4A},
		{HX_CIS_I2C_Action_W, 0x7FE7,	0x4A},
		{HX_CIS_I2C_Action_W, 0x7FEF,	0x4A},
		{HX_CIS_I2C_Action_W, 0x7FF1,	0x4A},
		{HX_CIS_I2C_Action_W, 0x7FF3,	0x4A},
		{HX_CIS_I2C_Action_W, 0x7FFB,	0x4A},
		{HX_CIS_I2C_Action_W, 0x7FFD,	0x4A},
		{HX_CIS_I2C_Action_W, 0x7FFF,	0x4A},
		{HX_CIS_I2C_Action_W, 0x8007,	0x62},
		{HX_CIS_I2C_Action_W, 0x8009,	0x62},
		{HX_CIS_I2C_Action_W, 0x800B,	0x62},
		{HX_CIS_I2C_Action_W, 0x8013,	0x6F},
		{HX_CIS_I2C_Action_W, 0x8015,	0x6F},
		{HX_CIS_I2C_Action_W, 0x8017,	0x6F},
		{HX_CIS_I2C_Action_W, 0x8019,	0x8E},
		{HX_CIS_I2C_Action_W, 0x801F,	0x75},
		{HX_CIS_I2C_Action_W, 0x8021,	0x75},
		{HX_CIS_I2C_Action_W, 0x8023,	0x75},
		{HX_CIS_I2C_Action_W, 0x802B,	0x75},
		{HX_CIS_I2C_Action_W, 0x802D,	0x75},
		{HX_CIS_I2C_Action_W, 0x802F,	0x75},
		{HX_CIS_I2C_Action_W, 0x8037,	0x75},
		{HX_CIS_I2C_Action_W, 0x8039,	0x75},
		{HX_CIS_I2C_Action_W, 0x803B,	0x75},
		{HX_CIS_I2C_Action_W, 0x803C,	0x13},
		{HX_CIS_I2C_Action_W, 0x803D,	0x17},
		{HX_CIS_I2C_Action_W, 0x803E,	0x15},
		{HX_CIS_I2C_Action_W, 0x803F,	0x11},
		{HX_CIS_I2C_Action_W, 0x8040,	0x0A},
		{HX_CIS_I2C_Action_W, 0x8041,	0x08},
		{HX_CIS_I2C_Action_W, 0x8047,	0x17},
		{HX_CIS_I2C_Action_W, 0x80F0,	0x26},
		{HX_CIS_I2C_Action_W, 0x80F1,	0x1C},
		{HX_CIS_I2C_Action_W, 0x80F2,	0x1C},
		{HX_CIS_I2C_Action_W, 0x80F3,	0x14},
		{HX_CIS_I2C_Action_W, 0x80F4,	0x14},
		{HX_CIS_I2C_Action_W, 0x80F5,	0x12},
		{HX_CIS_I2C_Action_W, 0x80F6,	0x26},
		{HX_CIS_I2C_Action_W, 0x80F7,	0x1C},
		{HX_CIS_I2C_Action_W, 0x80F8,	0x1B},
		{HX_CIS_I2C_Action_W, 0x80F9,	0x18},
		{HX_CIS_I2C_Action_W, 0x80FA,	0x17},
		{HX_CIS_I2C_Action_W, 0x80FB,	0x18},
		{HX_CIS_I2C_Action_W, 0x80FC,	0x28},
		{HX_CIS_I2C_Action_W, 0x80FD,	0x1E},
		{HX_CIS_I2C_Action_W, 0x80FE,	0x1D},
		{HX_CIS_I2C_Action_W, 0x80FF,	0x1C},
		{HX_CIS_I2C_Action_W, 0x8100,	0x1B},
		{HX_CIS_I2C_Action_W, 0x8101,	0x1C},
		{HX_CIS_I2C_Action_W, 0x8102,	0x2A},
		{HX_CIS_I2C_Action_W, 0x8103,	0x1F},
		{HX_CIS_I2C_Action_W, 0x8104,	0x1E},
		{HX_CIS_I2C_Action_W, 0x8105,	0x1E},
		{HX_CIS_I2C_Action_W, 0x8106,	0x1E},
		{HX_CIS_I2C_Action_W, 0x8107,	0x1E},
		{HX_CIS_I2C_Action_W, 0x8108,	0x2A},
		{HX_CIS_I2C_Action_W, 0x8109,	0x1F},
		{HX_CIS_I2C_Action_W, 0x810A,	0x1E},
		{HX_CIS_I2C_Action_W, 0x810B,	0x1E},
		{HX_CIS_I2C_Action_W, 0x810C,	0x1E},
		{HX_CIS_I2C_Action_W, 0x810D,	0x1F},
		{HX_CIS_I2C_Action_W, 0x810E,	0x01},
		{HX_CIS_I2C_Action_W, 0x8168,	0x0A},
		{HX_CIS_I2C_Action_W, 0x8169,	0x0B},
		{HX_CIS_I2C_Action_W, 0x816A,	0x09},
		{HX_CIS_I2C_Action_W, 0x816B,	0x0F},
		{HX_CIS_I2C_Action_W, 0x816C,	0x0F},
		{HX_CIS_I2C_Action_W, 0x816D,	0x0F},
		{HX_CIS_I2C_Action_W, 0x816E,	0x0B},
		{HX_CIS_I2C_Action_W, 0x816F,	0x0B},
		{HX_CIS_I2C_Action_W, 0x8170,	0x0A},
		{HX_CIS_I2C_Action_W, 0x8171,	0x0F},
		{HX_CIS_I2C_Action_W, 0x8172,	0x0F},
		{HX_CIS_I2C_Action_W, 0x8173,	0x0F},
		{HX_CIS_I2C_Action_W, 0x8174,	0x0B},
		{HX_CIS_I2C_Action_W, 0x8175,	0x0B},
		{HX_CIS_I2C_Action_W, 0x8176,	0x09},
		{HX_CIS_I2C_Action_W, 0x8177,	0x0F},
		{HX_CIS_I2C_Action_W, 0x8178,	0x0F},
		{HX_CIS_I2C_Action_W, 0x8179,	0x0F},
		{HX_CIS_I2C_Action_W, 0x817A,	0x0A},
		{HX_CIS_I2C_Action_W, 0x817B,	0x0B},
		{HX_CIS_I2C_Action_W, 0x817C,	0x09},
		{HX_CIS_I2C_Action_W, 0x817D,	0x0F},
		{HX_CIS_I2C_Action_W, 0x817E,	0x0F},
		{HX_CIS_I2C_Action_W, 0x817F,	0x0F},
		{HX_CIS_I2C_Action_W, 0x8180,	0x0A},
		{HX_CIS_I2C_Action_W, 0x8181,	0x0B},
		{HX_CIS_I2C_Action_W, 0x8182,	0x09},
		{HX_CIS_I2C_Action_W, 0x8183,	0x0F},
		{HX_CIS_I2C_Action_W, 0x8184,	0x0F},
		{HX_CIS_I2C_Action_W, 0x8185,	0x0F},
		{HX_CIS_I2C_Action_W, 0x81B0,	0x03},
		{HX_CIS_I2C_Action_W, 0x81E3,	0x04},
		{HX_CIS_I2C_Action_W, 0x81E4,	0x04},
		{HX_CIS_I2C_Action_W, 0x81E9,	0x04},
		{HX_CIS_I2C_Action_W, 0x81EA,	0x04},
		{HX_CIS_I2C_Action_W, 0x81EF,	0x04},
		{HX_CIS_I2C_Action_W, 0x81F0,	0x04},
		{HX_CIS_I2C_Action_W, 0x9186,	0x00},
		{HX_CIS_I2C_Action_W, 0xD030,	0x01},
		{HX_CIS_I2C_Action_W, 0xD04C,	0x10},
		{HX_CIS_I2C_Action_W, 0xD123,	0x75},
		{HX_CIS_I2C_Action_W, 0xD144,	0x10},
		{HX_CIS_I2C_Action_W, 0xD1AF,	0x08},
		{HX_CIS_I2C_Action_W, 0xD1BD,	0x67},
		{HX_CIS_I2C_Action_W, 0xD1D4,	0x04},
		{HX_CIS_I2C_Action_W, 0xD1D5,	0x04},
		{HX_CIS_I2C_Action_W, 0xD1D6,	0x07},
		{HX_CIS_I2C_Action_W, 0xD1D7,	0x07},
		{HX_CIS_I2C_Action_W, 0xD1D9,	0x40},
		{HX_CIS_I2C_Action_W, 0xD1DB,	0x58},
		{HX_CIS_I2C_Action_W, 0xD1DD,	0xD4},
		{HX_CIS_I2C_Action_W, 0xD1DF,	0xD4},
		{HX_CIS_I2C_Action_W, 0xD1E1,	0xD4},
		{HX_CIS_I2C_Action_W, 0xD348,	0x0F},
		{HX_CIS_I2C_Action_W, 0xD357,	0x00},
		{HX_CIS_I2C_Action_W, 0xD3AE,	0x11},
		{HX_CIS_I2C_Action_W, 0xD3AF,	0x44},
		{HX_CIS_I2C_Action_W, 0xD3B1,	0x7D},
		{HX_CIS_I2C_Action_W, 0xD803,	0xF0},
		{HX_CIS_I2C_Action_W, 0xD80B,	0xF0},
		{HX_CIS_I2C_Action_W, 0xD813,	0xF1},
		{HX_CIS_I2C_Action_W, 0xD81B,	0xF0},
		{HX_CIS_I2C_Action_W, 0xD843,	0xF1},
		{HX_CIS_I2C_Action_W, 0xD84F,	0xF0},
		{HX_CIS_I2C_Action_W, 0xD934,	0x23},
		{HX_CIS_I2C_Action_W, 0xD935,	0xC8},
		{HX_CIS_I2C_Action_W, 0xD938,	0x27},
		{HX_CIS_I2C_Action_W, 0xD939,	0x10},
		{HX_CIS_I2C_Action_W, 0xD93A,	0x23},
		{HX_CIS_I2C_Action_W, 0xD93B,	0xC8},
		{HX_CIS_I2C_Action_W, 0xD955,	0x07},
		{HX_CIS_I2C_Action_W, 0xD95A,	0x04},
		{HX_CIS_I2C_Action_W, 0xD95B,	0x0A},
		{HX_CIS_I2C_Action_W, 0xD95C,	0x1E},
		{HX_CIS_I2C_Action_W, 0xD95D,	0x00},
		{HX_CIS_I2C_Action_W, 0xD95E,	0x14},
		{HX_CIS_I2C_Action_W, 0xD95F,	0x21},
		{HX_CIS_I2C_Action_W, 0xD960,	0x00},
		{HX_CIS_I2C_Action_W, 0xD961,	0x00},
		{HX_CIS_I2C_Action_W, 0xD962,	0x0A},
		{HX_CIS_I2C_Action_W, 0xD963,	0x50},
		{HX_CIS_I2C_Action_W, 0xD964,	0x0A},
		{HX_CIS_I2C_Action_W, 0xD965,	0xA0},
		{HX_CIS_I2C_Action_W, 0xD966,	0x00},
		{HX_CIS_I2C_Action_W, 0xD967,	0x28},
		{HX_CIS_I2C_Action_W, 0xD968,	0x0A},
		{HX_CIS_I2C_Action_W, 0xD969,	0x50},
		{HX_CIS_I2C_Action_W, 0xD96A,	0x0A},
		{HX_CIS_I2C_Action_W, 0xD96B,	0xA0},
		{HX_CIS_I2C_Action_W, 0xD96C,	0x00},
		{HX_CIS_I2C_Action_W, 0xD96D,	0x00},
		{HX_CIS_I2C_Action_W, 0xD96E,	0x0A},
		{HX_CIS_I2C_Action_W, 0xD96F,	0x44},
		{HX_CIS_I2C_Action_W, 0xD970,	0x0A},
		{HX_CIS_I2C_Action_W, 0xD971,	0x50},
		{HX_CIS_I2C_Action_W, 0xD972,	0x00},
		{HX_CIS_I2C_Action_W, 0xD973,	0x00},
		{HX_CIS_I2C_Action_W, 0xD974,	0x0A},
		{HX_CIS_I2C_Action_W, 0xD975,	0x44},
		{HX_CIS_I2C_Action_W, 0xD976,	0x0A},
		{HX_CIS_I2C_Action_W, 0xD977,	0x50},
		{HX_CIS_I2C_Action_W, 0xDA10,	0x00},
		{HX_CIS_I2C_Action_W, 0xDA11,	0x14},
		{HX_CIS_I2C_Action_W, 0xDA12,	0x64},
		{HX_CIS_I2C_Action_W, 0xDA13,	0x00},
		{HX_CIS_I2C_Action_W, 0xDA14,	0x14},
		{HX_CIS_I2C_Action_W, 0xDA15,	0xC8},
		{HX_CIS_I2C_Action_W, 0xDA22,	0x00},
		{HX_CIS_I2C_Action_W, 0xDA23,	0x56},
		{HX_CIS_I2C_Action_W, 0xDA24,	0x00},
		{HX_CIS_I2C_Action_W, 0xDA25,	0xB5},
		{HX_CIS_I2C_Action_W, 0xDA26,	0x00},
		{HX_CIS_I2C_Action_W, 0xDA27,	0xE8},
		{HX_CIS_I2C_Action_W, 0xDA28,	0x08},
		{HX_CIS_I2C_Action_W, 0xDA29,	0xA6},
		{HX_CIS_I2C_Action_W, 0xDA2A,	0x00},
		{HX_CIS_I2C_Action_W, 0xDA2B,	0xA2},
		{HX_CIS_I2C_Action_W, 0xDA2F,	0x01},
		{HX_CIS_I2C_Action_W, 0x0110,	0x00},
		{HX_CIS_I2C_Action_W, 0x0112,	0x0A},
		{HX_CIS_I2C_Action_W, 0x0113,	0x0A},
		{HX_CIS_I2C_Action_W, 0x0114,	0x01},

		//HTOTAL
//		{HX_CIS_I2C_Action_W, 0x0342,	0x0D},
//		{HX_CIS_I2C_Action_W, 0x0343,	0xC8},
		{HX_CIS_I2C_Action_W, 0x0342,	0x0F},
		{HX_CIS_I2C_Action_W, 0x0343,	0xC8},

		{HX_CIS_I2C_Action_W, 0x033D,	0x00},
		{HX_CIS_I2C_Action_W, 0x033E,	0x1A},
		{HX_CIS_I2C_Action_W, 0x033F,	0x92},
		{HX_CIS_I2C_Action_W, 0x0344,	0x00},
		{HX_CIS_I2C_Action_W, 0x0345,	0x08},
		{HX_CIS_I2C_Action_W, 0x0346,	0x00},
		{HX_CIS_I2C_Action_W, 0x0347,	0x40},
		{HX_CIS_I2C_Action_W, 0x0348,	0x0F},
		{HX_CIS_I2C_Action_W, 0x0349,	0xC7},
		{HX_CIS_I2C_Action_W, 0x034A,	0x0C},
		{HX_CIS_I2C_Action_W, 0x034B,	0x0F},
		{HX_CIS_I2C_Action_W, 0x017C,	0x02},
		{HX_CIS_I2C_Action_W, 0x017D,	0x04},
		{HX_CIS_I2C_Action_W, 0x017E,	0x00},
		{HX_CIS_I2C_Action_W, 0x017F,	0x02},
		{HX_CIS_I2C_Action_W, 0x0180,	0x00},
		{HX_CIS_I2C_Action_W, 0x038C,	0x13},
		{HX_CIS_I2C_Action_W, 0x038D,	0x33},
		{HX_CIS_I2C_Action_W, 0x2000,	0x01},
		{HX_CIS_I2C_Action_W, 0x0408,	0x00},
		{HX_CIS_I2C_Action_W, 0x0409,	0x00},
		{HX_CIS_I2C_Action_W, 0x040A,	0x00},
		{HX_CIS_I2C_Action_W, 0x040B,	0x00},
		{HX_CIS_I2C_Action_W, 0x040C,	0x03},
		{HX_CIS_I2C_Action_W, 0x040D,	0xF0},
		{HX_CIS_I2C_Action_W, 0x040E,	0x02},
		{HX_CIS_I2C_Action_W, 0x040F,	0xF4},
		{HX_CIS_I2C_Action_W, 0x034C,	0x03},
		{HX_CIS_I2C_Action_W, 0x034D,	0xF0},
		{HX_CIS_I2C_Action_W, 0x034E,	0x02},
		{HX_CIS_I2C_Action_W, 0x034F,	0xF4},
		{HX_CIS_I2C_Action_W, 0x0301,	0x06},
		{HX_CIS_I2C_Action_W, 0x0303,	0x02},
		{HX_CIS_I2C_Action_W, 0x0305,	0x04},
		{HX_CIS_I2C_Action_W, 0x0306,	0x01},
		{HX_CIS_I2C_Action_W, 0x0307,	0x68},
		{HX_CIS_I2C_Action_W, 0x030D,	0x03},
		{HX_CIS_I2C_Action_W, 0x030E,	0x00},
		{HX_CIS_I2C_Action_W, 0x030F,	0xE1},
		{HX_CIS_I2C_Action_W, 0x0323,	0x00},
		{HX_CIS_I2C_Action_W, 0x0229,	0x00},
		{HX_CIS_I2C_Action_W, 0x022A,	0x1A},
		{HX_CIS_I2C_Action_W, 0x022B,	0x80},
		{HX_CIS_I2C_Action_W, 0x0204,	0x00},
		{HX_CIS_I2C_Action_W, 0x0205,	0x00},
		{HX_CIS_I2C_Action_W, 0x020E,	0x01},
		{HX_CIS_I2C_Action_W, 0x020F,	0x00},
		{HX_CIS_I2C_Action_W, 0x0210,	0x01},
		{HX_CIS_I2C_Action_W, 0x0211,	0x00},
		{HX_CIS_I2C_Action_W, 0x0212,	0x01},
		{HX_CIS_I2C_Action_W, 0x0213,	0x00},
		{HX_CIS_I2C_Action_W, 0x0214,	0x01},
		{HX_CIS_I2C_Action_W, 0x0215,	0x00},
		{HX_CIS_I2C_Action_W, 0x6A83,	0x03},
		{HX_CIS_I2C_Action_W, 0xD1CE,	0x00},
		{HX_CIS_I2C_Action_W, 0xDC3C,	0x01},
		{HX_CIS_I2C_Action_W, 0x0368,	0x00},
		{HX_CIS_I2C_Action_W, 0x036A,	0x08},
		{HX_CIS_I2C_Action_W, 0x036B,	0x70},
		{HX_CIS_I2C_Action_W, 0x0182,	0x08},
		{HX_CIS_I2C_Action_W, 0x0183,	0x08},
		{HX_CIS_I2C_Action_W, 0x0184,	0x01},
		{HX_CIS_I2C_Action_W, 0x0186,	0x08},
		{HX_CIS_I2C_Action_W, 0x0187,	0x08},
		{HX_CIS_I2C_Action_W, 0x0364,	0x01},
		{HX_CIS_I2C_Action_W, 0x0365,	0xF8},
		{HX_CIS_I2C_Action_W, 0x0366,	0x01},
		{HX_CIS_I2C_Action_W, 0x0367,	0x7A},
		{HX_CIS_I2C_Action_W, 0xE801,	0x04},
		{HX_CIS_I2C_Action_W, 0xE802,	0x00},
		{HX_CIS_I2C_Action_W, 0xE803,	0x60},
		{HX_CIS_I2C_Action_W, 0xE800,	0x00},
		{HX_CIS_I2C_Action_W, 0xE804,	0x08},
		{HX_CIS_I2C_Action_W, 0xE805,	0x08},
		{HX_CIS_I2C_Action_W, 0xE806,	0x02},
		{HX_CIS_I2C_Action_W, 0xE807,	0x08},
		{HX_CIS_I2C_Action_W, 0xE808,	0x00},
		{HX_CIS_I2C_Action_W, 0xE809,	0x04},
		{HX_CIS_I2C_Action_W, 0xE80A,	0x00},
		{HX_CIS_I2C_Action_W, 0xE80B,	0x00},
		{HX_CIS_I2C_Action_W, 0xE80C,	0x01},
		{HX_CIS_I2C_Action_W, 0xE80D,	0x00},
		{HX_CIS_I2C_Action_W, 0xE80E,	0x01},
		{HX_CIS_I2C_Action_W, 0xE80F,	0x00},
		{HX_CIS_I2C_Action_W, 0xE810,	0x01},
		{HX_CIS_I2C_Action_W, 0xE811,	0x00},
		{HX_CIS_I2C_Action_W, 0xE812,	0x01},
		{HX_CIS_I2C_Action_W, 0xE813,	0x00},
		{HX_CIS_I2C_Action_W, 0xE815,	0x00},
		{HX_CIS_I2C_Action_W, 0xE816,	0x03},
		{HX_CIS_I2C_Action_W, 0xE817,	0xE8},
		{HX_CIS_I2C_Action_W, 0xE819,	0x00},
		{HX_CIS_I2C_Action_W, 0xE81A,	0x1E},
		{HX_CIS_I2C_Action_W, 0xE81B,	0x5C},
		{HX_CIS_I2C_Action_W, 0xE81C,	0x0C},
		{HX_CIS_I2C_Action_W, 0xE81D,	0x10},
		{HX_CIS_I2C_Action_W, 0xE81E,	0x00},
		{HX_CIS_I2C_Action_W, 0xE81F,	0x08},
		{HX_CIS_I2C_Action_W, 0xE820,	0x00},
		{HX_CIS_I2C_Action_W, 0xE821,	0x40},
		{HX_CIS_I2C_Action_W, 0xE822,	0x0F},
		{HX_CIS_I2C_Action_W, 0xE823,	0xC7},
		{HX_CIS_I2C_Action_W, 0xE824,	0x0C},
		{HX_CIS_I2C_Action_W, 0xE825,	0x1F},
		{HX_CIS_I2C_Action_W, 0xE826,	0x01},
		{HX_CIS_I2C_Action_W, 0xE827,	0xF8},
		{HX_CIS_I2C_Action_W, 0xE828,	0x01},
		{HX_CIS_I2C_Action_W, 0xE829,	0x7A},
		{HX_CIS_I2C_Action_W, 0xE82A,	0x00},
		{HX_CIS_I2C_Action_W, 0xE82B,	0x00},
		{HX_CIS_I2C_Action_W, 0xE82C,	0x00},
		{HX_CIS_I2C_Action_W, 0xE82D,	0x00},
		{HX_CIS_I2C_Action_W, 0xE82E,	0x01},
		{HX_CIS_I2C_Action_W, 0xE82F,	0xF8},
		{HX_CIS_I2C_Action_W, 0xE830,	0x01},
		{HX_CIS_I2C_Action_W, 0xE831,	0x7A},
		{HX_CIS_I2C_Action_W, 0xE835,	0x04},
		{HX_CIS_I2C_Action_W, 0xE836,	0x00},
		{HX_CIS_I2C_Action_W, 0xE837,	0x60},
		{HX_CIS_I2C_Action_W, 0xE834,	0x00},
		{HX_CIS_I2C_Action_W, 0xE838,	0x08},
		{HX_CIS_I2C_Action_W, 0xE839,	0x08},
		{HX_CIS_I2C_Action_W, 0xE83E,	0x00},
		{HX_CIS_I2C_Action_W, 0xE83F,	0x00},
		{HX_CIS_I2C_Action_W, 0xE840,	0x01},
		{HX_CIS_I2C_Action_W, 0xE841,	0x00},
		{HX_CIS_I2C_Action_W, 0xE842,	0x01},
		{HX_CIS_I2C_Action_W, 0xE843,	0x00},
		{HX_CIS_I2C_Action_W, 0xE844,	0x01},
		{HX_CIS_I2C_Action_W, 0xE845,	0x00},
		{HX_CIS_I2C_Action_W, 0xE846,	0x01},
		{HX_CIS_I2C_Action_W, 0xE847,	0x00},
		{HX_CIS_I2C_Action_W, 0xE849,	0x00},
		{HX_CIS_I2C_Action_W, 0xE84A,	0x03},
		{HX_CIS_I2C_Action_W, 0xE84B,	0xE8},
		{HX_CIS_I2C_Action_W, 0xE84D,	0x01},
		{HX_CIS_I2C_Action_W, 0xE84E,	0x6C},
		{HX_CIS_I2C_Action_W, 0xE84F,	0x50},
		{HX_CIS_I2C_Action_W, 0xE850,	0x06},
		{HX_CIS_I2C_Action_W, 0xE851,	0x08},
		{HX_CIS_I2C_Action_W, 0xE852,	0x00},
		{HX_CIS_I2C_Action_W, 0xE853,	0x68},
		{HX_CIS_I2C_Action_W, 0xE854,	0x00},
		{HX_CIS_I2C_Action_W, 0xE855,	0x88},
		{HX_CIS_I2C_Action_W, 0xE856,	0x0F},
		{HX_CIS_I2C_Action_W, 0xE857,	0x67},
		{HX_CIS_I2C_Action_W, 0xE858,	0x0B},
		{HX_CIS_I2C_Action_W, 0xE859,	0xC7},
		{HX_CIS_I2C_Action_W, 0xE85A,	0x01},
		{HX_CIS_I2C_Action_W, 0xE85B,	0x00},
		{HX_CIS_I2C_Action_W, 0xE85C,	0x00},
		{HX_CIS_I2C_Action_W, 0xE85D,	0x78},
		{HX_CIS_I2C_Action_W, 0xE85E,	0x00},
		{HX_CIS_I2C_Action_W, 0xE85F,	0x00},
		{HX_CIS_I2C_Action_W, 0xE860,	0x00},
		{HX_CIS_I2C_Action_W, 0xE861,	0x00},
		{HX_CIS_I2C_Action_W, 0xE862,	0x00},
		{HX_CIS_I2C_Action_W, 0xE863,	0xA0},
		{HX_CIS_I2C_Action_W, 0xE864,	0x00},
		{HX_CIS_I2C_Action_W, 0xE865,	0x78},
		{HX_CIS_I2C_Action_W, 0xE869,	0x04},
		{HX_CIS_I2C_Action_W, 0xE86A,	0x00},
		{HX_CIS_I2C_Action_W, 0xE86B,	0x60},
		{HX_CIS_I2C_Action_W, 0xE868,	0x00},
		{HX_CIS_I2C_Action_W, 0xE86C,	0x08},
		{HX_CIS_I2C_Action_W, 0xE86D,	0x08},
		{HX_CIS_I2C_Action_W, 0xE872,	0x00},
		{HX_CIS_I2C_Action_W, 0xE873,	0x00},
		{HX_CIS_I2C_Action_W, 0xE874,	0x01},
		{HX_CIS_I2C_Action_W, 0xE875,	0x00},
		{HX_CIS_I2C_Action_W, 0xE876,	0x01},
		{HX_CIS_I2C_Action_W, 0xE877,	0x00},
		{HX_CIS_I2C_Action_W, 0xE878,	0x01},
		{HX_CIS_I2C_Action_W, 0xE879,	0x00},
		{HX_CIS_I2C_Action_W, 0xE87A,	0x01},
		{HX_CIS_I2C_Action_W, 0xE87B,	0x00},
		{HX_CIS_I2C_Action_W, 0xE87D,	0x00},
		{HX_CIS_I2C_Action_W, 0xE87E,	0x03},
		{HX_CIS_I2C_Action_W, 0xE87F,	0xE8},
		{HX_CIS_I2C_Action_W, 0xE881,	0x00},
		{HX_CIS_I2C_Action_W, 0xE882,	0x52},
		{HX_CIS_I2C_Action_W, 0xE883,	0x86},
		{HX_CIS_I2C_Action_W, 0xE884,	0x0D},
		{HX_CIS_I2C_Action_W, 0xE885,	0x50},
		{HX_CIS_I2C_Action_W, 0xE886,	0x00},
		{HX_CIS_I2C_Action_W, 0xE887,	0x68},
		{HX_CIS_I2C_Action_W, 0xE888,	0x00},
		{HX_CIS_I2C_Action_W, 0xE889,	0x88},
		{HX_CIS_I2C_Action_W, 0xE88A,	0x0F},
		{HX_CIS_I2C_Action_W, 0xE88B,	0x67},
		{HX_CIS_I2C_Action_W, 0xE88C,	0x0B},
		{HX_CIS_I2C_Action_W, 0xE88D,	0xC7},
		{HX_CIS_I2C_Action_W, 0xE88E,	0x01},
		{HX_CIS_I2C_Action_W, 0xE88F,	0x00},
		{HX_CIS_I2C_Action_W, 0xE890,	0x00},
		{HX_CIS_I2C_Action_W, 0xE891,	0x3C},
		{HX_CIS_I2C_Action_W, 0xE892,	0x00},
		{HX_CIS_I2C_Action_W, 0xE893,	0x00},
		{HX_CIS_I2C_Action_W, 0xE894,	0x00},
		{HX_CIS_I2C_Action_W, 0xE895,	0x00},
		{HX_CIS_I2C_Action_W, 0xE896,	0x00},
		{HX_CIS_I2C_Action_W, 0xE897,	0x50},
		{HX_CIS_I2C_Action_W, 0xE898,	0x00},
		{HX_CIS_I2C_Action_W, 0xE899,	0x3C},

		//MIPI CLK SETTING
		{HX_CIS_I2C_Action_W, 0x0115, 0x00},
		{HX_CIS_I2C_Action_W, 0x0847, 0x00},

		//EBD line on
		{HX_CIS_I2C_Action_W, 0x0360, 0x02},
		{HX_CIS_I2C_Action_W, 0x0361, 0x01},

		//DESKEW off
		{HX_CIS_I2C_Action_W, 0x0837, 0x00},
		{HX_CIS_I2C_Action_W, 0x0835, 0x00},

//		{HX_CIS_I2C_Action_W, 0x0100,	0x01},
};

static HX_CIS_SensorSetting_t IMX681_stream_on[] = {
        {HX_CIS_I2C_Action_W, 0x0100, 0x01},
};

static HX_CIS_SensorSetting_t IMX681_stream_off[] = {
        {HX_CIS_I2C_Action_W, 0x0100, 0x00},
};


/*
 * bootloader use RC96M
 */
static void imx681_change_dp_clk_96m()
{
	SCU_PDHSC_DPCLK_CFG_T cfg;
	hx_drv_scu_get_pdhsc_dpclk_cfg(&cfg);
	cfg.dpclk = SCU_HSCDPCLKSRC_RC96M48M;
	hx_drv_scu_set_pdhsc_dpclk_cfg(cfg, 0, 1);

	SCU_PDLSC_DPCLK_CFG_T lsccfg;
	hx_drv_scu_get_pdlsc_dpclk_cfg(&lsccfg);

	lsccfg.scsenmclk.lscmclkdiv = 3;
	lsccfg.scsenmclk.lscscmclksrc = SCU_LSCSCCLKSRC_RC96M48M;

	hx_drv_scu_set_pdlsc_dpclk_cfg(lsccfg);


	uint32_t pixel_clk = 96;
	uint32_t mclk = 24;
	uint32_t lsc_pclk = 96;
	hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_HSC_DP_PCLK, &pixel_clk);
	pixel_clk = pixel_clk / 1000000;

	hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_LSC_MCLK, &mclk);
	mclk = mclk / 1000000;

	hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_LSC_SCCLK, &lsc_pclk);
	lsc_pclk = lsc_pclk / 1000000;

	dbg_printf(DBG_LESS_INFO, "DP PIXEL CLK: %dM\n", pixel_clk);
	dbg_printf(DBG_LESS_INFO, "DP MCLK: %dM\n", mclk);
	dbg_printf(DBG_LESS_INFO, "DP LSC PIXEL CLK: %dM\n", lsc_pclk);
}


static void IMX681_dp_wdma_addr_init()
{

//	g_wdma1_baseaddr = (uint32_t)jpg_data;
//	g_wdma2_baseaddr = (uint32_t)jpg_data;
//	g_wdma3_baseaddr= (uint32_t)raw_data;
//	g_jpegautofill_addr = (uint32_t)jpg_info_data;

	g_jpegautofill_addr = mm_reserve(100);
	g_wdma1_baseaddr = mm_reserve(76800); //640*480/4
	if(g_wdma1_baseaddr!=0)
		g_wdma2_baseaddr = g_wdma1_baseaddr;
	else
		return ;

	g_wdma3_baseaddr= mm_reserve(921600); //640*480*3

	xprintf("WD1[%x], WD2_J[%x], WD3_RAW[%x], JPAuto[%x]\n",g_wdma1_baseaddr,g_wdma2_baseaddr,
			g_wdma3_baseaddr, g_jpegautofill_addr);


    sensordplib_set_xDMA_baseaddrbyapp(g_wdma1_baseaddr, g_wdma2_baseaddr, g_wdma3_baseaddr);
    sensordplib_set_jpegfilesize_addrbyapp(g_jpegautofill_addr);
}

#if (CIS_ENABLE_MIPI_INF != 0x00)

static void set_mipi_csi_enable()
{
	/*
	 * Set PLL200
	 */
	SCU_PDHSC_DPCLK_CFG_T cfg;

	hx_drv_scu_get_pdhsc_dpclk_cfg(&cfg);

	uint32_t pllfreq;
	hx_drv_swreg_aon_get_pllfreq(&pllfreq);

	if(pllfreq == 400000000)
	{
		cfg.mipiclk.hscmipiclksrc = SCU_HSCMIPICLKSRC_PLL;
		cfg.mipiclk.hscmipiclkdiv = 1;
	}
	else
	{
		cfg.mipiclk.hscmipiclksrc = SCU_HSCMIPICLKSRC_PLL;
		cfg.mipiclk.hscmipiclkdiv = 0;
	}


	hx_drv_scu_set_pdhsc_dpclk_cfg(cfg, 0, 1);


	uint32_t mipi_pixel_clk = 96;
	hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_HSC_MIPI_RXCLK, &mipi_pixel_clk);
	mipi_pixel_clk = mipi_pixel_clk / 1000000;

	dbg_printf(DBG_LESS_INFO, "MIPI CLK change t0 PLL freq:(%d / %d)\n", pllfreq, (cfg.mipiclk.hscmipiclkdiv+1));
	dbg_printf(DBG_LESS_INFO, "MIPI TX CLK: %dM\n", mipi_pixel_clk);


	/*
	 * Init CSI RX/TX
	 */
	mipi_pixel_clk = 96;
	hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_HSC_MIPI_RXCLK, &mipi_pixel_clk);

	mipi_pixel_clk = mipi_pixel_clk / 1000000;

	uint32_t bitrate_1lane = SENDPLIB_MIPIRX_DATARATE;
	uint32_t mipi_lnno = SENDPLIB_MIPIRX_LANE_NB;
	uint32_t pixel_dpp = SENDPLIB_MIPIRX_PIXEL_DEPTH;
	uint32_t line_length = SENCTRL_SENSOR_WIDTH;
	uint32_t frame_length = SENCTRL_SENSOR_HEIGHT;
	uint32_t byte_clk = bitrate_1lane/8;
	uint32_t continuousout = SENDPLIB_MIPITX_CONTINUOUS;
	uint32_t deskew_en = SENDPLIB_MIPI_DESKEW;

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
#if(IC_VERSION >= 30)
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
#else //VerB
	hscnt_cfg.mipirx_dphy_hscnt_clk_val = 0x03;
	hscnt_cfg.mipirx_dphy_hscnt_ln0_val = 0x10;
	hscnt_cfg.mipirx_dphy_hscnt_ln1_val = 0x10;
#endif


    if(pixel_dpp == 10 || pixel_dpp == 8)
    {
    	sensordplib_csirx_set_pixel_depth(pixel_dpp);
    }
    else
    {
    	dbg_printf(DBG_LESS_INFO, "PIXEL DEPTH fail %d\n", pixel_dpp);
        return;
    }


//	sensordplib_csirx_set_vcnum(0);
	sensordplib_csirx_set_deskew(deskew_en);
	sensordplib_csirx_set_fifo_fill(rx_fifo_fill);
    sensordplib_csirx_enable(mipi_lnno);

    CSITX_DPHYCLKMODE_E clkmode;
    if(continuousout)
    {
    	clkmode = CSITX_DPHYCLOCK_CONT;
    }
    else
    {
    	clkmode = CSITX_DPHYCLOCK_NON_CONT;
    }
    sensordplib_csitx_set_dphy_clkmode(clkmode);

    if(pixel_dpp == 10 || pixel_dpp == 8)
    {
    	sensordplib_csitx_set_pixel_depth(pixel_dpp);
    }
    else
    {
    	dbg_printf(DBG_LESS_INFO, "PIXEL DEPTH fail %d\n", pixel_dpp);
        return;
    }

	sensordplib_csitx_set_deskew(deskew_en);
    sensordplib_csitx_set_fifo_fill(tx_fifo_fill);
    sensordplib_csitx_enable(mipi_lnno, bitrate_1lane, line_length, frame_length);

    /*
     * //VMUTE setting: Enable VMUTE
     * W:0x52001408:0x0000000D:4:4
     */
    SCU_VMUTE_CFG_T ctrl;
    ctrl.timingsrc = SCU_VMUTE_CTRL_TIMING_SRC_VMUTE;
    ctrl.txphypwr = SCU_VMUTE_CTRL_TXPHY_PWR_DISABLE;
    ctrl.ctrlsrc = SCU_VMUTE_CTRL_SRC_SW;
    ctrl.swctrl = SCU_VMUTE_CTRL_SW_ENABLE;
    hx_drv_scu_set_vmute(&ctrl);


    dbg_printf(DBG_LESS_INFO, "VMUTE: 0x%08X\n", *(uint32_t*)(SCU_LSC_ADDR+0x408));
    dbg_printf(DBG_LESS_INFO, "0x53061000: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x1000));
    dbg_printf(DBG_LESS_INFO, "0x53061004: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x1004));
    dbg_printf(DBG_LESS_INFO, "0x53061008: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x1008));
    dbg_printf(DBG_LESS_INFO, "0x5306100C: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x100C));
    dbg_printf(DBG_LESS_INFO, "0x53061010: 0x%08X\n", *(uint32_t*)(CSITX_REGS_BASE+0x1010));

}

static void set_mipi_csi_disable()
{
	dbg_printf(DBG_LESS_INFO, "MIPI CSI DeInit IMX681\n");

    volatile uint32_t *csi_static_cfg_reg = (volatile uint32_t *)(CSIRX_REGS_BASE+0x08);
    volatile uint32_t *csi_dphy_lane_control_reg = (volatile uint32_t *)(CSIRX_REGS_BASE+0x40);
    volatile uint32_t *csi_stream0_control_reg = (volatile uint32_t *)(CSIRX_REGS_BASE+0x100);
    volatile uint32_t *csi_stream0_data_cfg = (volatile uint32_t *)(CSIRX_REGS_BASE+0x108);
    volatile uint32_t *csi_stream0_cfg_reg = (volatile uint32_t *)(CSIRX_REGS_BASE+0x10C);

    sensordplib_csirx_disable();
    sensordplib_csitx_disable();

    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_static_cfg_reg, *csi_static_cfg_reg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_dphy_lane_control_reg, *csi_dphy_lane_control_reg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_stream0_data_cfg, *csi_stream0_data_cfg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_stream0_control_reg, *csi_stream0_control_reg);

}
#endif

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

	trig_cmd1.byte1 = (IMX681_stream_on[0].RegAddree >> 8 & 0xFF);
	trig_cmd1.byte2 = (IMX681_stream_on[0].RegAddree & 0xFF);
	trig_cmd1.byte3 = IMX681_stream_on[0].Value;
	trig_cmd1.byte4 = 0x00;

	trig_cmd2.byte1 = trig_cmd2.byte2 = trig_cmd2.byte3 = trig_cmd2.byte4 = 0x11;
	trig_cmd3.byte1 = trig_cmd3.byte2 = trig_cmd3.byte3 = trig_cmd3.byte4 = 0x22;
	trig_cmd4.byte1 = trig_cmd4.byte2 = trig_cmd4.byte3 = trig_cmd4.byte4 = 0x33;

	sensordplib_autoi2c_trigcmd(trig_cmd1, trig_cmd2, trig_cmd3, trig_cmd4);

	HXAUTOI2CHC_CMD_T stop_cmd1, stop_cmd2, stop_cmd3, stop_cmd4;

	stop_cmd1.byte1 = (IMX681_stream_off[0].RegAddree >> 8 & 0xFF);
	stop_cmd1.byte2 = (IMX681_stream_off[0].RegAddree & 0xFF);
	stop_cmd1.byte3 = IMX681_stream_off[0].Value;
	stop_cmd1.byte4 = 0x00;

	stop_cmd2.byte1 = stop_cmd2.byte2 = stop_cmd2.byte3 = stop_cmd2.byte4 = 0x11;
	stop_cmd3.byte1 = stop_cmd3.byte2 = stop_cmd3.byte3 = stop_cmd3.byte4 = 0x22;
	stop_cmd4.byte1 = stop_cmd4.byte2 = stop_cmd4.byte3 = stop_cmd4.byte4 = 0x33;

	sensordplib_autoi2c_stopcmd(stop_cmd1, stop_cmd2, stop_cmd3, stop_cmd4);
}
#endif

int cisdp_sensor_init()
{
	DP_MCLK_SRC_INT_EXT_E clk_int_ext;
	DP_MCLK_SRC_INT_SEL_E clk_int_src;

    dbg_printf(DBG_LESS_INFO, "cis_imx681_init \r\n");

	hx_drv_dp_get_mclk_src(&clk_int_ext, &clk_int_src);
	xprintf("ori_mclk_int_ext=%d,ori_mclk_int_src=%d\n",clk_int_ext, clk_int_src);
	hx_drv_dp_set_mclk_src(DP_MCLK_SRC_INTERNAL, DP_MCLK_SRC_INT_SEL_XTAL);
	hx_drv_dp_get_mclk_src(&clk_int_ext, &clk_int_src);
	xprintf("mclk_int_ext=%d,mclk_int_src=%d\n",clk_int_ext, clk_int_src);

    /*
     * common CIS init
     */
    hx_drv_cis_init(DEAULT_XHSUTDOWN_PIN, SENSORCTRL_MCLK_DIV1);
    dbg_printf(DBG_LESS_INFO, "mclk DIV1, xshutdown_pin=%d\n",DEAULT_XHSUTDOWN_PIN);
    hx_drv_sensorctrl_set_xSleepCtrl(SENSORCTRL_XSLEEP_BY_CPU);
    hx_drv_sensorctrl_set_xSleep(1);
    dbg_printf(DBG_LESS_INFO, "hx_drv_sensorctrl_set_xSleep(1)\n");

    /*
     * IMX681 special setting
     */
    uint8_t level = 1;
    dbg_printf(DBG_LESS_INFO,"Set SEN_D2	--->	GPIO20:%d \n", level);
    hx_drv_scu_set_SEN_D2_pinmux(SCU_SEN_D2_PINMUX_GPIO20);
    hx_drv_gpio_set_output(GPIO20, GPIO_OUT_LOW);
    hx_drv_gpio_set_out_value(GPIO20, (GPIO_OUT_LEVEL_E) level);

    dbg_printf(DBG_LESS_INFO, "Change I2C ID to: 0x%02X\n", CIS_I2C_ID);
    hx_drv_cis_set_slaveID(CIS_I2C_ID);

    /*
     * off stream before init sensor
     */
    if(hx_drv_cis_setRegTable(IMX681_stream_off, HX_CIS_SIZE_N(IMX681_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_LESS_INFO, "IMX681 off by app fail\r\n");
        return -1;
    }


    if(hx_drv_cis_setRegTable(IMX681_init_setting, HX_CIS_SIZE_N(IMX681_init_setting, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_LESS_INFO, "IMX681 Init by app fail \r\n");
        return -1;
    }
    else
    {
    	dbg_printf(DBG_LESS_INFO, "IMX681 Init by app \n");
    }

    return 0;
}

int cisdp_dp_init(bool inp_init, SENSORDPLIB_PATH_E dp_type, evthandlerdp_CBEvent_t cb_event, uint32_t jpg_ratio)
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
    hw5x5_cfg.demos_color_mode = DP_HW5X5_DEMOS_COLORMODE;

    hw5x5_cfg.demos_pattern_mode = DP_HW5X5_DEMOS_PATTERN;
    hw5x5_cfg.demoslpf_roundmode = DP_HW5X5_DEMOSLPF_ROUNDMODE;
    hw5x5_cfg.hw55_crop_stx = DP_HW5X5_CROP_START_X;
    hw5x5_cfg.hw55_crop_sty = DP_HW5X5_CROP_START_X;
    hw5x5_cfg.hw55_in_width = DP_HW5X5_CROP_WIDTH;
    hw5x5_cfg.hw55_in_height = DP_HW5X5_CROP_HEIGHT;

    //JPEG Cfg
    jpeg_cfg.jpeg_path = DP_JPEG_PATH;
    jpeg_cfg.enc_width = DP_JPEG_ENC_WIDTH;
    jpeg_cfg.enc_height = DP_JPEG_ENC_HEIGHT;
    jpeg_cfg.jpeg_enctype = DP_JPEG_ENCTYPE;

    if(jpg_ratio == 4)
    	jpeg_cfg.jpeg_encqtable = JPEG_ENC_QTABLE_4X;
    else
    	jpeg_cfg.jpeg_encqtable = JPEG_ENC_QTABLE_10X;

    IMX681_dp_wdma_addr_init();

    /*
     * Change DP source CLK
     */
    imx681_change_dp_clk_96m();


#if (CIS_ENABLE_MIPI_INF != 0x00)
    //setup MIPI RXTX
	set_mipi_csi_enable();
#endif

    INP_CROP_T crop;
    crop.start_x = DP_INP_CROP_START_X;
    crop.start_y = DP_INP_CROP_START_Y;

    if(DP_INP_CROP_WIDTH >= 1)
    	crop.last_x = DP_INP_CROP_WIDTH - 1;
    else
    	crop.last_x = 0;

    if(DP_INP_CROP_HEIGHT >= 1)
    	crop.last_y = DP_INP_CROP_HEIGHT - 1;
    else
    	crop.last_y = 0;


	sensordplib_set_sensorctrl_inp_wi_crop_bin(SENCTRL_SENSOR_TYPE, SENCTRL_STREAM_TYPE,
			SENCTRL_SENSOR_WIDTH, SENCTRL_SENSOR_HEIGHT, DP_INP_SUBSAMPLE, crop, DP_INP_BINNING);

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

    hx_dplib_evthandler_register_cb(cb_event, SENSORDPLIB_CB_FUNTYPE_DP);

	return 0;
}

void cisdp_sensor_start()
{
#if (CIS_ENABLE_HX_AUTOI2C == 0x00)

    /*
     * Manual Control Stream On
     */
    if(hx_drv_cis_setRegTable(IMX681_stream_on, HX_CIS_SIZE_N(IMX681_stream_on, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_LESS_INFO, "IMX681 on by app fail\r\n");
        return;
    }

#else
    /*
     * AUTO I2C Control Stream On
     */
    sensordplib_autoi2c_enable();
    dbg_printf(DBG_LESS_INFO, "hxauto i2c enable \n");
#endif

    sensordplib_set_mclkctrl_xsleepctrl_bySCMode();

   	sensordplib_set_sensorctrl_start();
}

void cisdp_sensor_stop()
{
    sensordplib_stop_capture();
    sensordplib_start_swreset();
    sensordplib_stop_swreset_WoSensorCtrl();

#if (CIS_ENABLE_HX_AUTOI2C == 0x00)
    /*
     * Default Stream On
     */
    if(hx_drv_cis_setRegTable(IMX681_stream_off, HX_CIS_SIZE_N(IMX681_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_LESS_INFO, "IMX681 off by app fail\r\n");
    }
    else
    {
    	dbg_printf(DBG_LESS_INFO, "IMX681 off by app \n");
    }
#else
    //disable hxauotoi2c
    sensordplib_autoi2c_disable();
    dbg_printf(DBG_LESS_INFO, "hxauto i2c disable \n");
#endif

#if (CIS_ENABLE_MIPI_INF != 0x00)
    set_mipi_csi_disable();
#endif
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
    //hx_drv_jpeg_get_EncOutRealMEMSize(jpeg_enc_filesize);
    //dbg_printf(DBG_LESS_INFO, "current jpeg_size=0x%x\n", *jpeg_enc_filesize);
    hx_drv_jpeg_get_EncOutRealMEMSize(&reg_val);
    dbg_printf(DBG_LESS_INFO, "current jpeg_size=0x%x\n", reg_val);

    //hx_drv_jpeg_get_FillFileSizeToMem(frame_no, (uint32_t)g_jpegautofill_addr, jpeg_enc_filesize);
    //hx_drv_jpeg_get_MemAddrByFrameNo(frame_no, g_wdma2_baseaddr, jpeg_enc_addr);
    hx_drv_jpeg_get_FillFileSizeToMem(frame_no, (uint32_t)g_jpegautofill_addr, &mem_val);
    hx_drv_jpeg_get_MemAddrByFrameNo(frame_no, g_wdma2_baseaddr, jpeg_enc_addr);

    //*jpeg_enc_filesize = mem_val > reg_val?reg_val:mem_val;
    *jpeg_enc_filesize = mem_val == reg_val?mem_val:reg_val;

    dbg_printf(DBG_LESS_INFO, "current frame_no=%d, jpeg_size=0x%x,addr=0x%x\n",frame_no,*jpeg_enc_filesize,*jpeg_enc_addr);
}

uint32_t app_get_jpeg_addr()
{
    //EPII_InvalidateDCache_by_Addr(g_wdma2_baseaddr, 4);
	return g_wdma2_baseaddr;
}

uint32_t app_get_jpeg_sz()
{
    //EPII_InvalidateDCache_by_Addr(g_jpegautofill_addr, 4);
    hx_InvalidateDCache_by_Addr((volatile void *)g_jpegautofill_addr, 32);
	return *((uint32_t*)g_jpegautofill_addr);
}

uint32_t app_get_raw_addr()
{
	//raw data area BBBBBB/GGGGGG/RRRRRR
	return g_wdma3_baseaddr;//return b for use
}

uint32_t app_get_raw_channels() {
	return SENCTRL_SENSOR_CH;
}
