/*
 * hx_drv_hm0360.h
 *
 *  Created on: 2019¦~4¤ë29¤é
 *      Author: 902447
 */

#ifndef CIS_HM0360_HX_DRV_HM0360_H_
#define CIS_HM0360_HX_DRV_HM0360_H_

#include "board.h"
#include "hx_drv_CIS_common.h"

//for EM9D no need shift slave ID, for CX3 need shift slave ID
#define HM0360_SLAVE_ID        BOARD_CIS_IIC_SLVADDR//0x48 //slave ID << 1
#define IIC_M_ID_HM0360        BOARD_CIS_SENSOR_IIC_ID
#define SUPPORT_OSC_PCLK_MAX
#define HM0360_MCLK_MHZ        24 //12 or 24M
#define SUPPORT_LINE_GATED

/** HM0360 Streaming Type **/
typedef enum HM0360_STREAMING_S
{
  HM0360_STANDY       			= 0,	/**< Standby Mode */
  HM0360_STREAMING_I2C_CONT     = 1,	/**< I2C Continous Streaming */
  HM0360_STREAMING_HW_CONT      = 2,	/**< HW Continous Streaming */
  HM0360_STREAMING_HW_BURST     = 3,	/**< HW Burst Streaming */
  HM0360_STREAMING_SC_MODE4     = 4,	/**< Sensor MD Detect CTX Streaming */
  HM0360_STREAMING_SC_MODE5     = 5,	/**< Sensor Automatic wake-up mode Streaming */
  HM0360_STOP          			= 6,	/**< Stop Mode*/
} HM0360_STREAMING_E;

typedef enum HM0360_DATAFORMAT_S
{
	HM0360_DATAFORMAT_MODE1        = 0,	/**< Mode1 Data format */
	HM0360_DATAFORMAT_MODE2        = 1,	/**< Mode2 Data format */
	HM0360_DATAFORMAT_MODE3        = 2,	/**< Mode3 Data format*/
	HM0360_DATAFORMAT_MODE5        = 4,	/**< Mode5 Data format*/
} HM0360_DATAFORMAT_E;

typedef enum HM0360_RES_S
{
	HM0360_RES_656x496        = 0,	/**< 656x496 */
	HM0360_RES_640x480     	  = 1,	/**< 640x480 */
	HM0360_RES_328x248        = 2,	/**< 328x248*/
	HM0360_RES_320x240        = 3,	/**< 320x240*/
	HM0360_RES_164x124        = 4,	/**< 164x124*/
	HM0360_RES_160x120        = 5,	/**< 160x120*/
	HM0360_RES_82x62          = 6,	/**< 82x62*/
	HM0360_RES_80x60          = 7,	/**< 80x60*/
} HM0360_RES_E;

typedef enum HM0360_TESTPATTEN_S
{
	HM0360_TESTPATTERN_OFF        		= 0,	/**< TP OFF */
	HM0360_TESTPATTERN_COLORBAR   		= 1,	/**< TP Colorbar*/
	HM0360_TESTPATTERN_GRAYFADE   		= 2,	/**< TP GRAY FADE*/
	HM0360_TESTPATTERN_WALKING1   		= 3,	/**< TP Walking1*/
	HM0360_TESTPATTERN_SOLID_COLOR      = 4,	/**< TP Solid Color*/
	HM0360_TESTPATTERN_PN9		        = 5,	/**< TP PN9*/
} HM0360_TESTPATTEN_E;

typedef struct HM0360_TPCOLOR_CFG_S
{
	uint16_t blue;
	uint16_t gb;
	uint16_t gr;
	uint16_t red;
} HM0360_TPCOLOR_CFG_T;

HX_CIS_ERROR_E hx_drv_hm0360_get_ModelID(uint16_t *modelID, uint8_t *rev_id);
HX_CIS_ERROR_E hx_drv_hm0360_get_FrameCounter(uint16_t *frameCnt);
/** HM0360 all configuration without streaming on **/
HX_CIS_ERROR_E hx_drv_hm0360_set_allCfg(HM0360_RES_E res, HM0360_DATAFORMAT_E dataformat, HM0360_STREAMING_E stream);
HX_CIS_ERROR_E hx_drv_hm0360_dump_allCfg();
HX_CIS_ERROR_E hx_drv_hm0360_set_Timing(uint16_t m_lineLen, uint16_t m_frameLen);
HX_CIS_ERROR_E hx_drv_hm0360_get_Timing(uint16_t *m_lineLen, uint16_t *m_frameLen);
HX_CIS_ERROR_E hx_drv_hm0360_set_stream(HM0360_STREAMING_E m_mode);
HX_CIS_ERROR_E hx_drv_hm0360_get_stream(uint8_t *m_mode);

HX_CIS_ERROR_E hx_drv_hm0360_set_DGain(uint16_t m_digitgain);
HX_CIS_ERROR_E hx_drv_hm0360_get_DGain(uint16_t *m_digitgain);
HX_CIS_ERROR_E hx_drv_hm0360_set_AGain(uint8_t m_analoggain);
HX_CIS_ERROR_E hx_drv_hm0360_get_AGain(uint8_t *m_analoggain);
HX_CIS_ERROR_E hx_drv_hm0360_set_CoarseINT(uint16_t m_coarseInt);
HX_CIS_ERROR_E hx_drv_hm0360_get_CoarseINT(uint16_t *m_coarseInt);
HX_CIS_ERROR_E hx_drv_hm0360_SWReset();

HX_CIS_ERROR_E hx_drv_hm0360_set_reg(uint16_t addr, uint8_t val, uint8_t cmu_update);
HX_CIS_ERROR_E hx_drv_hm0360_get_reg(uint16_t addr, uint8_t *val);
HX_CIS_ERROR_E hx_drv_hm0360_CMU_Update();

HX_CIS_ERROR_E hx_drv_hm0360_set_dataformat(HM0360_DATAFORMAT_E format);
HX_CIS_ERROR_E hx_drv_hm0360_dump_dataformat();

HX_CIS_ERROR_E hx_drv_hm0360_set_tp(HM0360_TESTPATTEN_E pattern, HM0360_TPCOLOR_CFG_T color);

/*0 ~ 7*/
HX_CIS_ERROR_E hx_drv_hm0360_set_driving(uint8_t pclk_driv, uint8_t data_driv, uint8_t hvsync_driv,HM0360_DATAFORMAT_E format);

#ifdef USE_HUB616
HX_CIS_ERROR_E hx_drv_hm0360_set_hub616(HM0360_DATAFORMAT_E format);
HX_CIS_ERROR_E hx_drv_hm0360_dump_hub616();
#endif
#endif /* CIS_HM0360_HX_DRV_HM0360_H_ */
