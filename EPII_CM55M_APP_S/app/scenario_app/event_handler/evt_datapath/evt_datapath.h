/*
 * evt_datapath.h
 *
 *  Created on: 2019¦~7¤ë30¤é
 *      Author: 902447
 */

#ifndef SCENARIO_APP_EVENT_HANDLER_EVT_DATAPATH_EVT_DATAPATH_H_
#define SCENARIO_APP_EVENT_HANDLER_EVT_DATAPATH_EVT_DATAPATH_H_

//#include "hx_drv_adcc.h"
#include "sensor_dp_lib.h"
#include "event_handler_evt.h"


/**
 * \brief Sensor DP library callback function type definition
 *
 * \param[in] event dp library callback event
 * \retval void.
 */
typedef void (*evthandlerdp_CBEvent_t) (EVT_INDEX_E event);
#if 0
/**
 * \brief GPIO Callback
 *
 * \param[in] event GPIO callback event
 * \retval void.
 */
typedef void (*evthandlerGPIO_CBEvent_t) (uint8_t event);

/**
 * \brief Analog PIR Callback
 *
 * \param[in] event ADC callback event
 * \retval void.
 */
typedef void (*evthandlerADC_CBEvent_t) (uint32_t event);

/**
 * \brief Sensor RTC Callback
 *
 * \param[in] event Sensor RTC callback event
 * \retval void.
 */
typedef void (*evthdlSensorRTC_CBEvent_t) (uint32_t event);

/**
 * \brief ADC RTC Callback
 *
 * \param[in] event ADC RTC callback event
 * \retval void.
 */
typedef void (*evthdlADCRTC_CBEvent_t) (uint32_t event);
#endif

///**
// * \brief	clear datapath related event
// *
// * clear datapath related event
// */
//void evt_dp_clear_all_dpevent();
//
//void evt_dp_clear_sensor_rtc();

/**
 * \brief	Callback function for 1Bit INP Parser Error
 *
 * Callback function for 1Bit INP Parser Error
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_1bitParser_err_cb(void);

/**
 * \brief	Callback function for CDM fifo Error
 *
 * Callback function for CDM fifo Error
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_CDM_fifo_err_cb(void);

/**
 * \brief	Callback function for WDMA1 Error
 *
 * Callback function for WDMA1 Error
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_WDMA1_err_cb(void);

/**
 * \brief	Callback function for WDMA2 Error
 *
 * Callback function for WDMA2 Error
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_WDMA2_err_cb(void);

/**
 * \brief	Callback function for WDMA3 Error
 *
 * Callback function for WDMA3 Error
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_WDMA3_err_cb(void);

/**
 * \brief	Callback function for RDMA Error
 *
 * Callback function for RDMA Error
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_RDMA_err_cb(void);

/**
 * \brief	Callback function for EDM WDT1 Error
 *
 * Callback function for EDM WDT1 Error
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_EDM_WDT1_err_cb(void);

/**
 * \brief	Callback function for EDM WDT2 Error
 *
 * Callback function for EDM WDT2 Error
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_EDM_WDT2_err_cb(void);

/**
 * \brief	Callback function for EDM WDT3 Error
 *
 * Callback function for EDM WDT3 Error
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_EDM_WDT3_err_cb(void);

/**
 * \brief	Callback function for CDM Motion
 *
 * Callback function for CDM Motion
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_CDM_Motion_cb(void);

/**
 * \brief	Callback function for Sensor RTC fire
 *
 * Callback function for Sensor RTC fire
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_Sensor_RTC_Fire_cb(void);

/**
 * \brief	Callback function for HXAUTOI2C Error
 *
 * Callback function for HXAUTOI2C Error
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_hxautoi2c_err_cb(void);

/**
 * \brief	Callback function for XDMA Frame Ready
 *
 * Callback function for XDMA Frame Ready
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_xDMA_FrameReady_cb(void);
#if 0
/**
 * \brief	Callback function for PGPIO0 Interrupt
 *
 * Callback function for PGPIO0 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO0_cb(void);

/**
 * \brief	Callback function for PGPIO1 Interrupt
 *
 * Callback function for PGPIO1 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO1_cb(void);

/**
 * \brief	Callback function for PGPIO2 Interrupt
 *
 * Callback function for PGPIO2 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO2_cb(void);

/**
 * \brief	Callback function for PGPIO3 Interrupt
 *
 * Callback function for PGPIO3 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO3_cb(void);

/**
 * \brief	Callback function for PGPIO4 Interrupt
 *
 * Callback function for PGPIO4 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO4_cb(void);

/**
 * \brief	Callback function for PGPIO5 Interrupt
 *
 * Callback function for PGPIO5 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO5_cb(void);

/**
 * \brief	Callback function for PGPIO6 Interrupt
 *
 * Callback function for PGPIO6 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO6_cb(void);

/**
 * \brief	Callback function for PGPIO7 Interrupt
 *
 * Callback function for PGPIO7 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO7_cb(void);

/**
 * \brief	Callback function for PGPIO8 Interrupt
 *
 * Callback function for PGPIO8 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO8_cb(void);

/**
 * \brief	Callback function for PGPIO9 Interrupt
 *
 * Callback function for PGPIO9 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO9_cb(void);

/**
 * \brief	Callback function for PGPIO10 Interrupt
 *
 * Callback function for PGPIO10 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO10_cb(void);

/**
 * \brief	Callback function for PGPIO11 Interrupt
 *
 * Callback function for PGPIO11 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO11_cb(void);

/**
 * \brief	Callback function for PGPIO12 Interrupt
 *
 * Callback function for PGPIO12 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO12_cb(void);

/**
 * \brief	Callback function for PGPIO13 Interrupt
 *
 * Callback function for PGPIO13 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO13_cb(void);

/**
 * \brief	Callback function for PGPIO14 Interrupt
 *
 * Callback function for PGPIO14 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_PGPIO14_cb(void);

/**
 * \brief	Callback function for SGPIO0 Interrupt
 *
 * Callback function for SGPIO0 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_SGPIO0_cb(void);

/**
 * \brief	Callback function for SGPIO1 Interrupt
 *
 * Callback function for SGPIO1 Interrupt
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_SGPIO1_cb(void);

/**
 * \brief	clear all GPIO Interrupt event
 *
 * clear All GPIO Interrupt related event
 */
void evt_dp_clear_allgpio();


/**
 * \brief	Callback function for ADC Channel 0
 *
 * Callback function for ADC Channel 0
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_ADC_Ch0_cb(void);

/**
 * \brief	Callback function for ADC Channel 1
 *
 * Callback function for ADC Channel 1
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_ADC_Ch1_cb(void);

/**
 * \brief	Callback function for ADC Channel 2
 *
 * Callback function for ADC Channel 2
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_ADC_Ch2_cb(void);

/**
 * \brief	Callback function for ADC Channel 3
 *
 * Callback function for ADC Channel 3
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_ADC_Ch3_cb(void);

/**
 * \brief	Callback function for ADC CFG
 *
 * Callback function for ADC CFG
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_ADC_CFG_cb(void);

/**
 * \brief	Callback function for Sensor RTC
 *
 * Callback function for Sensor RTC
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_SensorRTC_cb(void);

/**
 * \brief	Callback function for ADC RTC
 *
 * Callback function for ADC RTC
 * \retval	INFRA_EVT_RTN_NONE	to keep event alive
 * \retval	INFRA_EVT_RTN_CLEAR	to clear event
 */
uint8_t evt_ADCRTC_cb(void);
#endif
/*
 * @brief register DP lib DataPath Status callback
 *
 * register DP lib DataPath Status callback
 * you can use different callback for DP, HOG and RS
 * @param cb_event dp library callback event function
 * @param type register library callback function type (Sensor DP, HOG, RS, JPEG_DEC)
 * @return void.
 * */
void hx_dplib_evthandler_register_cb(evthandlerdp_CBEvent_t  cb_event, SENSORDPLIB_CB_FUNTYPE_E cb_type);

SENSORDPLIB_STATUS_E hx_dplib_evthandler_get_dplib_event();
#if 0
/*
 * @brief register GPIO Interrupt callback
 *
 * register GPIO Interrupt callback
 * @param aIndex  GPIO Interrupt pin index
 * @param cb_event GPIO Interrupt callback event function
 * @return void.
 * */
void hx_gpio_evthandler_register_cb(IOMUX_INDEX_E aIndex, evthandlerGPIO_CBEvent_t  cb_event);

/*
 * @brief register ADC init
 *
 * register ADC Interrupt callback
 * @param cb0_event ADC Channel 0 callback event function
 * @param cb1_event ADC Channel 1 callback event function
 * @param cb2_event ADC Channel 2 callback event function
 * @param cb3_event ADC Channel 3 callback event function
 * @param cfg_event ADC configure callback event function
 * @return void.
 * */
void hx_ADC_evthandler_init(evthandlerADC_CBEvent_t  ch0_event
		,evthandlerADC_CBEvent_t  ch1_event
		,evthandlerADC_CBEvent_t  ch2_event
		,evthandlerADC_CBEvent_t  ch3_event
		,evthandlerADC_CBEvent_t  cfg_event
		,evthandlerADC_CBEvent_t  sg_event);

/*
 * @brief register ADC Interrupt callback
 *
 * register ADC Interrupt callback
 * @param cbxx ADC Channel xx
 * @param cbxx_event ADC Channel xx callback event function
 * @return void.
 * */
void hx_ADCCh_evthandler_register_cb(ADCC_CALLBACK_E  chxx
		,evthandlerADC_CBEvent_t  chxx_event);

/*
 * @brief register Sensor RTC Interrupt callback
 *
 * register Sensor RTC Interrupt callback
 * @param rtc_interval Sensor RTC interval(ms)
 * @param cb_event Sensor RTC Interrupt callback event function
 * @return void.
 * */
void hx_SensorRTC_evthdl_register_cb(uint32_t rtc_interval, evthdlSensorRTC_CBEvent_t  cb_event);

/*
 * @brief register ADC RTC Interrupt callback
 *
 * register ADC RTC Interrupt callback
 * @param rtc_interval ADC RTC interval(ms)
 * @param cb_event ADC RTC Interrupt callback event function
 * @return void.
 * */
void hx_ADCRTC_evthdl_register_cb(uint32_t rtc_interval, evthdlADCRTC_CBEvent_t  cb_event);
#endif

#endif /* SCENARIO_APP_EVENT_HANDLER_EVT_DATAPATH_EVT_DATAPATH_H_ */
