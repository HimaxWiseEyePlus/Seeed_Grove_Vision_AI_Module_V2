/*
 * hx_drv_hm11b1_fpga.h
 *
 *  Created on: 2018¦~12¤ë5¤é
 *      Author: 902447
 */

#ifndef EXTERNAL_CIS_HM11B1_FPGA_
#define EXTERNAL_CIS_HM11B1_FPGA_

#include "board.h"
#include "hx_drv_iic.h"
#include "hx_drv_CIS_common.h"
#include "hx_drv_gpio.h"

//for EM9D no need shift slave ID, for CX3 need shift slave ID
#define HM11B1_SLAVE_ID        0x24//0x48 //slave ID << 1

#define IIC_M_ID_HM11B1        BOARD_CIS_SENSOR_IIC_ID

#define HM11B1_MCLK_MHZ     	24
#define SUPPORT_PCLK_MHZ    	36


typedef enum HM11B1_RES_S
{
	HM11B1_RES_648x408        = 0,	/**< 648x408 */
	HM11B1_RES_648x368     	  = 1,	/**< 648x368 */
	HM11B1_RES_640x400        = 2,	/**< 640x400*/
	HM11B1_RES_640x360        = 3,	/**< 640x360*/
	HM11B1_RES_324x204        = 4,	/**< 324x204*/
	HM11B1_RES_320x200        = 5,	/**< 320x200*/
	HM11B1_RES_324x184        = 6,	/**< 324x184*/
	HM11B1_RES_320x180        = 7,	/**< 320x180*/
	HM11B1_RES_162x102        = 8,	/**< 162x102*/
	HM11B1_RES_162x92         = 9,	/**< 162x92*/
	HM11B1_RES_160x100        = 10,	/**< 160x100*/
	HM11B1_RES_160x90         = 11,	/**< 160x90*/
} HM11B1_RES_E;

/** HM01B0 IO Bit **/
typedef enum HM11B1_IOBIT_ORDER_S
{
	HM11B1_IOBIT_ORDER_1BIT_LSB     = 0,	/**< 1 BIT IO LSB*/
	HM11B1_IOBIT_ORDER_1BIT_MSB     = 1,	/**< 1 BIT IO MSB */
} HM11B1_IOBIT_ORDER_E;


/** HM01B0 Streaming Type **/
typedef enum HM11B1_STREAMING_S
{
  HM11B1_STANDY                    = 0,	/**< Standby Mode */
  HM11B1_STREAMING1_CONTINUOUS     = 1,	/**< Streaming1 CONTINUOUS */
  HM11B1_STREAMING2_XSLEEP          = 2,	/**< Streaming2 xSleep*/
} HM11B1_STREAMING_E;

typedef enum HM11B1_TESTPATTEN_S
{
	HM11B1_TESTPATTERN_OFF        		= 0,	/**< TP OFF */
	HM11B1_TESTPATTERN_COLORBAR   		= 1,	/**< TP Colorbar*/
} HM11B1_TESTPATTEN_E;

typedef struct HM11B1_TPCOLOR_CFG_S
{
	uint16_t blue;
	uint16_t gb;
	uint16_t gr;
	uint16_t red;
} HM11B1_TPCOLOR_CFG_T;


typedef struct HM11B1Setting_S{
	HX_CIS_I2C_ActionType_e I2C_ActionType;
    uint8_t slaveID;
    uint16_t RegAddree; //if I2C_ActionType == SENSOR_S, RegAddree = msec
    uint8_t Value;
} HM11B1Setting_t;

//set MCLK & toogle xshutdown
HX_CIS_ERROR_E hx_drv_hm11b1_init(GPIO_INDEX_E xshutdown_pin);
HX_CIS_ERROR_E hx_drv_hm11b1_xshutdown(GPIO_INDEX_E xshutdown_pin, uint8_t high);

//set all configure without streaming
HX_CIS_ERROR_E hx_drv_hm11b1_set_allcfg(HM11B1_RES_E res, HM11B1_IOBIT_ORDER_E endian, HM11B1_STREAMING_E stream_mode);
//get all configure without streaming
HX_CIS_ERROR_E hx_drv_hm11b1_dump_allcfg(HM11B1_RES_E res, HM11B1_IOBIT_ORDER_E endian, HM11B1_STREAMING_E stream_mode);

//set test Pattern
HX_CIS_ERROR_E hx_drv_hm11b1_set_tp(HM11B1_TESTPATTEN_E tp);
//get test Pattern
HX_CIS_ERROR_E hx_drv_hm11b1_get_tp(HM11B1_TESTPATTEN_E *tp);

//set HM11B1 standby
HX_CIS_ERROR_E hx_drv_hm11b1_set_stream(HM11B1_STREAMING_E m_mode);
//get HM11B1 stream
HX_CIS_ERROR_E hx_drv_hm11b1_get_stream(HM11B1_STREAMING_E *m_mode);

HX_CIS_ERROR_E hx_drv_hm11b1_get_ModelID(uint8_t slave_addr, uint16_t *modelID);
HX_CIS_ERROR_E hx_drv_hm11b1_get_FrameCounter(uint8_t slave_addr, uint8_t *frameCnt);

#endif /* SYNOPSYS_PA8535_EM9D_DFSS_SVNHM11B1_EXTERNAL_CIS_HM11B1_FPGA_HX_DRV_HM11B1_FPGA_H_ */
