/*
 * hx_drv_HM2140.h
 *
 *  Created on: 2018/10/18
 *      Author: 902447
 */

#ifndef HX_DRV_HM2140_H_
#define HX_DRV_HM2140_H_

#include "board.h"
#include "hx_drv_iic.h"
#include "hx_drv_CIS_common.h"

//for EM9D no need shift slave ID, for CX3 need shift slave ID
#define HM2140_SLAVE_ID        BOARD_CIS_IIC_SLVADDR//0x48 //slave ID << 1
#define IIC_M_ID_HM2140        BOARD_CIS_SENSOR_IIC_ID
#define HM2140_MCLK_MHZ        24

/** HM2140 Streaming Type **/
typedef enum HM2140_STREAMING_S
{
  HM2140_STANDY        = 0,	/**< Standby Mode */
  HM2140_STREAMING     = 1,	/**< Streaming */
  HM2140_STOP          = 2,	/**< Stop Mode*/
} HM2140_STREAMING_E;

/** HM2140 TestPattern Type **/
typedef enum HM2140_TP_TYPE_S
{
  HM2140_TP_DISABLE              = 0,	/**< 0: Test Pattern Disable */
  HM2140_SOLID_COLOR             = 1,	/**< 1: Solid Color*/
  HM2140_COLORBAR                = 2,	/**< 2: Colorbar*/
  HM2140_COLORBAR_BLENDING       = 3,	/**< 3: Colorbar Blending*/
  HM2140_PN9                     = 4,	/**< 4: PN9*/
} HM2140_TP_TYPE_E;

/** HM2140 Output Port Type **/
typedef enum HM2140_OUT_TYPE_S
{
	HM2140_OUT_TYPE_PARALLEL_ONLY    = 0,	/**< PARALLEL Output ONLY */
	HM2140_OUT_TYPE_MIPI_ONLY        = 1,	/**< MIPI Output ONLY */
	HM2140_OUT_TYPE_BOTH             = 2,	/**< PARALLEL and MIPI Output */
} HM2140_OUT_TYPE_E;

/** HM2140 Resolution Type **/
typedef enum HM2140_RESOLUTION_S
{
  HM2140_RESOLUTION_960X540        = 0,	/**< 960x540 */
  HM2140_RESOLUTION_1920X1080      = 1,	/**< 1920x1080 */
  HM2140_RESOLUTION_1920X1080_PCLK18M   = 2,   /**< 1920x1080 PCLK 24M */
} HM2140_RESOLUTION_E;


typedef struct HM2140_TPSolidCol_S{
  uint16_t m_TPBlue; /*Test data BLUE (PS:10bit)*/
  uint16_t m_TPGB; /*Test data greenB (PS:10bit)*/
  uint16_t m_TPRed; /*Test data RED (PS:10bit)*/
  uint16_t m_TPGR; /*Test data greenR (PS:10bit)*/
}HM2140_TPSolidCol_T;

HX_CIS_ERROR_E hx_drv_hm2140_get_ModelID(uint16_t *modelID);
HX_CIS_ERROR_E hx_drv_hm2140_get_FrameCounter(uint8_t *frameCnt);
/** HM1245 all configuration without streaming on **/
HX_CIS_ERROR_E hx_drv_hm2140_set_allCfg(HM2140_RESOLUTION_E resolution);
HX_CIS_ERROR_E hx_drv_hm2140_set_Timing(uint16_t m_lineLen, uint16_t m_frameLen);
HX_CIS_ERROR_E hx_drv_hm2140_get_Timing(uint16_t *m_lineLen, uint16_t *m_frameLen);
HX_CIS_ERROR_E hx_drv_hm2140_set_stream(HM2140_STREAMING_E m_mode);
HX_CIS_ERROR_E hx_drv_hm2140_get_stream(HM2140_STREAMING_E *m_mode);

HX_CIS_ERROR_E hx_drv_hm2140_set_TP(HM2140_TP_TYPE_E m_pattern);
HX_CIS_ERROR_E hx_drv_hm2140_set_TPColor(HM2140_TPSolidCol_T m_color);
/*m_seed: 10 bit*/
HX_CIS_ERROR_E hx_drv_hm2140_set_PN9Seed(uint16_t m_seed);

/*
 * D-Gain:10bit
 * 2.6 fixed point number 2 integer bits,6 fractional bits)
 * 2 integer bits 0x020E[1:0],
 * 6 fractional bits 0x020F[7:2]
 * m_digitgain == register value
 */
HX_CIS_ERROR_E hx_drv_hm2140_set_DGain(uint16_t m_digitgain);
HX_CIS_ERROR_E hx_drv_hm2140_get_DGain(uint16_t *m_digitgain);
/*(A-Gain:8bit)
 * Analog gain = (x + 16) / 16,
 * m_analoggain == x == register value
 */
HX_CIS_ERROR_E hx_drv_hm2140_set_AGain(uint8_t m_analoggain);
HX_CIS_ERROR_E hx_drv_hm2140_get_AGain(uint8_t *m_analoggain);
//(Coarse Integration:16bit)
HX_CIS_ERROR_E hx_drv_hm2140_set_CoarseINT(uint16_t m_coarseInt);
HX_CIS_ERROR_E hx_drv_hm2140_get_CoarseINT(uint16_t *m_coarseInt);
HX_CIS_ERROR_E hx_drv_hm2140_SWReset();

HX_CIS_ERROR_E hx_drv_hm2140_set_OutputType(HM2140_OUT_TYPE_E m_outtype);


#endif /* HX_DRV_HM2140_H_ */
