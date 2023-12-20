/*
 * hx_drv_HM01B0.h
 *
 *  Created on: 2018/10/11
 *      Author: 902447
 */

#ifndef HX_DRV_HM01B0_H_
#define HX_DRV_HM01B0_H_

#include "board.h"
//#include "board_config.h"
#include "hx_drv_iic.h"
#include "hx_drv_CIS_common.h"
/**
 * \defgroup	HM01B0_SENSOR_DRV	HM01B0 Sensor Driver
 * \ingroup	HM01B0_SENSOR_DRV
 * \brief	HM01B0 Sensor Driver Declaration
 */
/**
 * \defgroup	HM01B0_COMMON_SENSOR_DRV_MACRO	HM01B0 Sensor Driver Macro
 * \ingroup	HM01B0_SENSOR_DRV
 * \brief	Defines some Macro of HM01B0 Sensor Driver need.
 * @{
 */
#define HM01B0_MCLK_MHZ        24//12		/**< WE-1 output MCLK frequency. It affects HM01B0 PLL Setting in HM01B0 Driver*/
/** @} */

/**
 * \defgroup	HM01B0_SENSOR_DRV_ENUM	HM01B0 Sensor Driver Enumeration
 * \ingroup	HM01B0_SENSOR_DRV
 * \brief	Defines some enumeration of HM01B0 Sensor Driver need.
 * @{
 */
/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/
/**
 * \enum HM01B0_RES_TYPE_E
 * \brief HM01B0 Sensor type and resolution
 */
typedef enum
{
  HM01B0_RES_TYPE_QVGA_MONO     = 0,	/**< QVGA Mono */
  HM01B0_RES_TYPE_QVGA_BAYER     = 1,	/**< QVGA Bayer */
  HM01B0_RES_TYPE_QQVGA_MONO     = 2,	/**< QQVGA MONO */
} HM01B0_RES_TYPE_E;

/**
 * \enum HM01B0_IOBIT_ORDER_E
 * \brief HM01B0 Sensor IO Bit
 */
typedef enum
{
  HM01B0_IOBIT_ORDER_1BIT_MSB     = 0,	/**< 1 BIT IO MSB*/
  HM01B0_IOBIT_ORDER_1BIT_LSB     = 1,	/**< 1 BIT IO LSB */
  HM01B0_IOBIT_ORDER_4BIT_MSB     = 2,	/**< 4 BIT IO MSB*/
  HM01B0_IOBIT_ORDER_4BIT_LSB     = 3,	/**< 4 BIT IO LSB */
  HM01B0_IOBIT_ORDER_8BIT_MSB     = 4,	/**< 8 BIT IO MSB */
  HM01B0_IOBIT_ORDER_8BIT_LSB     = 5,	/**< 8 BIT IO LSB */
} HM01B0_IOBIT_ORDER_E;

/**
 * \enum HM01B0_STREAMING_E
 * \brief HM01B0 Streaming Type
 */
typedef enum HM01B0_STREAMING_S
{
  HM01B0_STANDY                    = 0,	/**< Standby Mode */
  HM01B0_STREAMING1_CONTINUOUS     = 1,	/**< Streaming1 CONTINUOUS */
  HM01B0_STREAMING2_HW_TRIGGER     = 2,	/**< Streaming3 HW Trigger*/
} HM01B0_STREAMING_E;

/**
 * \enum HM01B0_TP_TYPE_E
 * \brief HM01B0 TestPattern Type
 */
typedef enum
{
  HM01B0_TP_OFF                 = 0,	/**< 0: TP OFF */
  HM01B0_COLORBAR               = 1,	/**< 1: Colorbar */
  HM01B0_WALKING_1              = 2,	/**< 2: Walking 1 */
} HM01B0_TP_TYPE_E;

/**
 * \defgroup	HM01B0_SENSOR_DRV_REGSTRUCT		HM01B0 Sensor Driver Structure
 * \ingroup	HM01B0_SENSOR_DRV
 * \brief	contains definitions of HM01B0 Sensor Driver structure.
 * @{
 */
/**
 * \struct HM01B0_AE_CTRL_T
 * \brief HM01B0 AE control related setting
 */
typedef struct {
  uint8_t m_HM01B0_AE_CTRL;			/**< AE Control Enable */
  uint8_t m_HM01B0_AE_TARGET_MEAN;	/**< AE Target Mean */
  uint8_t m_HM01B0_AE_MIN_MEAN;		/**< AE minimum Mean */
  uint16_t m_HM01B0_MAX_INTG;		/**< AE Maximum Integration time */
  uint8_t m_HM01B0_MIN_INTG;		/**< AE Minimum Integration time */
  uint8_t m_HM01B0_MAX_AGAIN_FULL;	/**< AE Maximum A-Gain in full frame mode */
  uint8_t m_HM01B0_MAX_AGAIN_BIN2;	/**< AE Maximum A-Gain in Bin2 mode */
  uint8_t m_HM01B0_MIN_AGAIN;		/**< AE Minimum A-Gain*/
  uint8_t m_HM01B0_MAX_DGAIN;		/**< AE Maximum D-Gain*/
  uint8_t m_HM01B0_MIN_DGAIN;		/**< AE Minimum D-Gain*/
} HM01B0_AE_CTRL_T;
/** @} */

/**
 * \defgroup	HM01B0_SENSOR_DRV_FUNCDLR	HM01B0 Sensor Driver Function Declaration
 * \ingroup	HM01B0_SENSOR_DRV
 * \brief	Contains declarations of HM01B0 Sensor Driver functions.
 * @{
 */
/*
 * \brief get HM01B0 Model ID by I2C Master
 *
 * \param[out] modelID  Sensor Model ID
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_get_ModelID(uint16_t *modelID);
/*
 * \brief get HM01B0 Frame Counter
 *
 * \param[out] frameCnt  Sensor Frame Counter by I2C Master
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_get_FrameCounter(uint8_t *frameCnt);

/*
 * \brief set HM01B0 all configuration without streaming on according pre-defined setting in driver
 *
 * \param[in] res_type  HM01B0 Sensor type and resolution
 * \param[in] io_bitOrder  HM01B0 Sensor IO Bit
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_set_allCfg(HM01B0_RES_TYPE_E res_type, HM01B0_IOBIT_ORDER_E io_bitOrder);
/*
 * \brief set HM01B0 timing line length and frame length by I2C Master
 *
 * \param[in] m_lineLen  HM01B0 line length setting
 * \param[in] m_frameLen  HM01B0 frame length setting
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_set_Timing(uint16_t m_lineLen, uint16_t m_frameLen);
/*
 * \brief get HM01B0 timing line length and frame length by I2C Master
 *
 * \param[out] m_lineLen  HM01B0 line length setting
 * \param[out] m_frameLen  HM01B0 frame length setting
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_get_Timing(uint16_t *m_lineLen, uint16_t *m_frameLen);
/*
 * \brief set HM01B0 Streaming Type by I2C Master
 *
 * \param[in] m_mode  HM01B0 stream type
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_set_stream(HM01B0_STREAMING_E m_mode);
/*
 * \brief get HM01B0 Streaming Type by I2C Master
 *
 * \param[out] m_mode  HM01B0 stream type
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_get_stream(HM01B0_STREAMING_E *m_mode);
/*
 * \brief set HM01B0 test pattern by I2C Master
 *
 * \param[in] m_pattern  HM01B0 TestPattern Type
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_set_TP(HM01B0_TP_TYPE_E m_pattern);
/*
 * \brief set HM01B0 D-Gain in None-AE mode by I2C Master
 *
 * \param[in] m_digitgain  HM01B0 Digital Gain
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_set_DGain(uint16_t m_digitgain);
/*
 * \brief get HM01B0 D-Gain in None-AE mode by I2C Master
 *
 * \param[out] m_digitgain  HM01B0 Digital Gain
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_get_DGain(uint16_t *m_digitgain);
/*
 * \brief set HM01B0 A-Gain in None-AE mode by I2C Master
 *
 * \param[in] m_analoggain  HM01B0 analog Gain
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_set_AGain(uint8_t m_analoggain);
/*
 * \brief get HM01B0 A-Gain in None-AE mode by I2C Master
 *
 * \param[out] m_analoggain  HM01B0 analog Gain
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_get_AGain(uint8_t *m_analoggain);
/*
 * \brief set HM01B0 Coarse Integration time in None-AE mode by I2C Master
 *
 * \param[in] m_coarseInt  HM01B0 coarse integration time
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_set_CoarseINT(uint16_t m_coarseInt);
/*
 * \brief get HM01B0 Coarse Integration time in None-AE mode by I2C Master
 *
 * \param[out] m_coarseInt  HM01B0 coarse integration time
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_get_CoarseINT(uint16_t *m_coarseInt);
/*
 * \brief set HM01B0 Software Reset by I2C Master
 *
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_SWReset();

/*
 * \brief set HM01B0 AE enable and AE target mean by I2C Master
 *
 * \param[in] m_AEEnable  HM01B0 AE enable
 * \param[in] m_TargetMean  HM01B0 AE Target Mean
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_set_AE_TMEAN(uint8_t m_AEEnable, uint8_t m_TargetMean);
/*
 * \brief get HM01B0 AE enable and AE target mean by I2C Master
 *
 * \param[out] m_AEEnable  HM01B0 AE enable
 * \param[out] m_TargetMean  HM01B0 AE Target Mean
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_get_AE_TMEAN(uint8_t *m_AEEnable, uint8_t *m_TargetMean);
/*
 * \brief set HM01B0 AE control related setting by I2C Master
 *
 * \param[in] ae_ctrl  HM01B0 AE control related setting
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_set_AE_Ctrl(HM01B0_AE_CTRL_T ae_ctrl);
/*
 * \brief get HM01B0 AE control related setting by I2C Master
 *
 * \param[out] ae_ctrl  HM01B0 AE control related setting
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_get_AE_Ctrl(HM01B0_AE_CTRL_T *ae_ctrl);

/*
 * \brief dump HM01B0 related register to UART message (debug purpose)
 *
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_hm01b0_dump_reg();
/** @} */

#endif /* HX_DRV_HM01B0_H_ */
