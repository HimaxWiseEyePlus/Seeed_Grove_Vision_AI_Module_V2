/*
 * event_handler_evt.h
 *
 *  Created on: 2019/7/19
 *      Author: 902452
 */

#ifndef EVENT_HANDLER_EVENT_HANDLER_EVT_H_
#define EVENT_HANDLER_EVENT_HANDLER_EVT_H_

#include "hxevent.h"
#include "event_handler_cfg.h"

/****************************************************
 * FEATURE Definition                              *
 ***************************************************/

/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/
/**
 * \defgroup	EVENT_HANDLER_ENUM	Event Handler Framework Enumeration
 * \ingroup	EVENT_HANDLER
 * \brief	Defines the required enumeration of Event Handler Framework.
 * @{
 */

/**
 * \enum EVT_INDEX_E
 * \brief this enumeration use in event handler framework, define the supported event.
 */
typedef enum EVT_INDEX_S
{
#ifdef EVT_I2CS_0_CMD
    EVT_INDEX_I2CS_0_RX,
    EVT_INDEX_I2CS_0_TX,
    EVT_INDEX_I2CS_0_ERR,
#endif
#ifdef EVT_I2CS_1_CMD
    EVT_INDEX_I2CS_1_RX,
    EVT_INDEX_I2CS_1_TX,
    EVT_INDEX_I2CS_1_ERR,
#endif
#ifdef EVT_DATAPATH
    EVT_INDEX_1BITPARSER_ERR,
    EVT_INDEX_CDM_FIFO_ERR,
    EVT_INDEX_XDMA_WDMA1_ABNORMAL,
    EVT_INDEX_XDMA_WDMA2_ABNORMAL,
    EVT_INDEX_XDMA_WDMA3_ABNORMAL,
    EVT_INDEX_XDMA_RDMA_ABNORMAL,
    EVT_INDEX_EDM_WDT1_TIMEOUT,
    EVT_INDEX_EDM_WDT2_TIMEOUT,
    EVT_INDEX_EDM_WDT3_TIMEOUT,
    EVT_INDEX_CDM_MOTION,
    EVT_INDEX_XDMA_FRAME_READY,
    EVT_INDEX_SENSOR_RTC_FIRE,
	EVT_INDEX_HXAUTOI2C_ERR,
#endif
#ifdef EVT_DATAPATH_EXT
    EVT_INDEX_PGPIO0,
    EVT_INDEX_PGPIO1,
    EVT_INDEX_PGPIO2,
    EVT_INDEX_PGPIO3,
    EVT_INDEX_PGPIO4,
    EVT_INDEX_PGPIO5,
    EVT_INDEX_PGPIO6,
    EVT_INDEX_PGPIO7,
    EVT_INDEX_PGPIO8,
    EVT_INDEX_PGPIO9,
    EVT_INDEX_PGPIO10,
    EVT_INDEX_PGPIO11,
    EVT_INDEX_PGPIO12,
    EVT_INDEX_PGPIO13,
    EVT_INDEX_PGPIO14,
    EVT_INDEX_SGPIO0,
    EVT_INDEX_SGPIO1,
    EVT_INDEX_ADC_CH0,
    EVT_INDEX_ADC_CH1,
    EVT_INDEX_ADC_CH2,
    EVT_INDEX_ADC_CH3,
    EVT_INDEX_ADC_CFG_INT,
    EVT_INDEX_ADC_SG_INT,
    EVT_INDEX_SENSOR_RTC,
    EVT_INDEX_ADC_RTC,
#endif
#ifdef EVT_SPIMCOMM
    EVT_INDEX_SPIM1_RX,
    EVT_INDEX_SPIM1_TX,
#endif
#ifdef EVT_SPISCOMM
    EVT_INDEX_SPIS_RX,
#endif
#ifdef EVT_AUDIO
    EVT_INDEX_AUDIO_RX,
#endif
#ifdef EVT_ISPCOMM
    EVT_INDEX_ISP_RX,
#endif
#ifdef EVT_UARTCOMM
    EVT_INDEX_UART_RX,
#endif
#ifdef EVT_CM55STIMER
    EVT_INDEX_CM55STIMER,
#endif
#ifdef EVT_CM55MTIMER
    EVT_INDEX_CM55MTIMER,
#endif
#ifdef EVT_CM55MMB
	EVT_INDEX_CM55MMB,
#endif
#ifdef EVT_CM55MMB_NBAPP
	EVT_INDEX_CM55MMB_PREROLL,
	EVT_INDEX_CM55MMB_STOP_PREROLL,
	EVT_INDEX_CM55MMB_ALLON_VID,
	EVT_INDEX_CM55MMB_STOP_ALLON_VID,
	EVT_INDEX_CM55MMB_CM55S_RDY,
	EVT_INDEX_CM55MMB_SPION,
	EVT_INDEX_CM55MMB_SPIOFF,
#endif
    EVT_INDEX_MAX	= HX_EVENTQUE_MAXSIZE
}EVT_INDEX_E;

/** @} */

#endif /* EVENT_HANDLER_EVENT_HANDLER_EVT_H_ */
