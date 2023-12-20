/**
 * @file hx_drv_anti_tamp.h
 * @author himax/904207
 * @brief 
 * @version V1.0.0
 * @date 2022-04-19
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */
#ifndef INC_HX_DRV_ANTI_TAMPER_H_
#define INC_HX_DRV_ANTI_TAMPER_H_

#include "dev_common.h"

/** 
 * \defgroup Anti Tamper
 * \details The Anti tamper features 5 external tamper events, configurable in active or passive mode, 
 *          and 4 internal monitoring tamper events.
 *  Internal monitoring tamper events, including:
 *  >Temperature
 *  >Voltage
 *  >RTC
 *  >Clock monitor
 *  When a tamper condition is detected, a notification event is notified to the anti tamper controller
 *  through one of a number of dedicated control lines. Voltage and temperature can be digitally monitored through another fabric interface. 
 *  See Temperature and Voltage Sensors for details.
 * 
 *  [TAMPER_DETECTION]
 *  The external tamper detection takes TAMP_IO pins to detect detect whether the device encounters unauthorized opneing or tampering.
 *  External tamper events, up to 5, can be configured in either passive or active.
 *  >Passive: when the desired signal(TAMP_OUT) is fixed.
 *  >Active: when the desird signal(TAMP_OUT) level is toggling time.
 *  the received signal(TAMP_IN) is compared with the desired singal level. Once unequal, tamper event is trggered.
 * 
 * for example:
 *      static void anti_tamp_callback(UINT32_T status) 
 *      {
 *          xprintf("ANTI TAMPER Callback (Done) \n");
 *      }
 * 
 *      hx_drv_anti_tamp_init(SCU_AON_ADDR);
 *      hx_drv_anti_tamp_register_cb(anti_tamp_callback);
 * 
 *      ///set TAM_DET = passive mode, and TAM_OUT4 is HIGH level.
 *      uint32_t TAMP_mode = 0x10;
 *      hx_drv_anti_tamp_set_tam_mode(0);
 *      hx_drv_anti_tamp_set_tam_data(TAMP_mode);
 * 
 *      ///unmask TAM_IO4 interrupt
 *      hx_drv_anti_tamp_unmask_tam_det(TAMP_mode);
 * 
 *      ///unmask TAM_DET interrupt of anti tamper
 *      uint32_t anti_int_mask = 0x10;
 *      hx_drv_anti_tamp_unmask_interrupt(anti_int_mask);
 *      ///connect intput of TAM IO 4 to ground, and then interrupt will be triggered.
 * 
 * [Clock monitor]
 * The clock monitor will monitor whether  osc32k is detected abnormal, 
 * this module will set the corresponding flag to indicate clock tampering detected.
 * 
 * CLK_MONI_COUNT = (clock monitor source clock)/(LSC_32K_CLK)
 * using \ref hx_drv_scu_set_pdlsc_ckmonsrcclk_cfg to select clock monitor source clock  
 * using \ref hx_drv_scu_set_pdlsc_32k_cfg to select 
 * 
 */


/**
 * \enum    ANTI_TAMP_ERROR_E
 * \brief   ANTI_TAMP Errors Type
 * 
 */
typedef enum {
	ANTI_TAMP_NO_ERROR					    	= 0,	/**< No ERROR */
	ANTI_TAMP_ERROR_NOT_INIT					= 1,	/**< ERROR MSG: not init */
	ANTI_TAMP_ERROR_ALREADY_INIT				= 2,	/**< ERROR MSG: already init */
	ANTI_TAMP_ERROR_INVALID_PARAMETERS	    	= 3,	/**< ERROR MSG: Invalid parameters */
    ANTI_TAMP_UNKNOWN_ERROR					    = 4,	/**< ERROR MSG: UNKNOWN ERROR*/
} ANTI_TAMP_ERROR_E;

typedef enum  {
    PASSIVE_MODE = 0,
    ACTIVE_MODE = 1,
} ANTI_TAMP_TAM_MODE_E;

typedef enum {
    TEMPERATURE          = 0x0,
    VOLTAGE              = 0x1,
    CLOCK_MONITOR        = 0x2,
    RTC                  = 0x3,
    TAMPER_DETECTION     = 0x4,
    MONITOR_EVENT_ALL    = 0x31,
} ANTI_TAMP_MONITOR_EVENT_E;

#define ANTI_TAMP_EVENT_TEMPERTURE          (0x1)
#define ANTI_TAMP_EVENT_VOLTAGE             (0x2)
#define ANTI_TAMP_EVENT_CLOCK_MONITOR       (0x4)
#define ANTI_TAMP_EVENT_RTC                 (0x8)
#define ANTI_TAMP_EVENT_TAMPER_IO           (0x10)
#define ANTI_TAMP_EVENT_ALL                 (0x1f)

typedef void (*ANTI_TAMP_CB) (uint32_t int_status);

/**
 * @brief initialize anti-tamper interface with a given base address.
 * 
 * @param scu_aon_addr The base address of the SCU AON register.
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_init(uint32_t scu_aon_addr);

/**
 * @brief deinitialize anti-tamper interface, disable anti-tamper
 * 
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_deinit();

/**
 * @brief Enable/disable interrupt of anti-tamper.
 * 
 * @param[in] enable set 1 or 0 to enable/disable IRQ.
 * @return ANTI_TAMP_ERROR_E 
 */
ANTI_TAMP_ERROR_E hx_drv_anti_tamp_setIRQ(uint32_t enable);

/**
 * @brief Enable/disable anti-tamper
 * 
 * @param[in] enable set 0 or 1 to enable/disable anti-tamper
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_enable(uint32_t enable);

/**
 * @brief Enable/disable software-controller for event
 * 
 * @param[in] event Set a 1 to the corresponding register bit to enable software-controller of specified event
 * [0] : TS_CTRL_INT
 * [1] : BOR18_INT
 * [2] : CLK_MONITOR_INT
 * [3] : RTC_INT
 * [4] : TAM_DET_INT
 * 
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_ena_interrupt_sw_input(uint32_t event);

/**
 * @brief Set interrupt of software-controller for event
 * 
 * @param[in] value Set a 1 to the corresponding register bit to generate interrupt of specified event
 * [0] : TS_CTRL_INT
 * [1] : BOR18_INT
 * [2] : CLK_MONITOR_INT
 * [3] : RTC_INT
 * [4] : TAM_DET_INT
 * 
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_set_interrupt_sw_input(uint32_t value);

/**
 * @brief Masks all interrupts generated by the anti-tamper
 * 
 * @param[in] event  Set a 1 to the corresponding register bit to mask interrupt of specified event.
 * [0] : TS_CTRL_INT
 * [1] : BOR18_INT
 * [2] : CLK_MONITOR_INT
 * [3] : RTC_INT
 * [4] : TAM_DET_INT
 * 
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_mask_interrupt(uint32_t event);

/**
 * @brief Unmasks all interrupts generated by the anti-tamper
 * 
 * @param event Set a 1 to the corresponding register bit to unmask interrupt of specified event.
 * [0] : TS_CTRL_INT
 * [1] : BOR18_INT
 * [2] : CLK_MONITOR_INT
 * [3] : RTC_INT
 * [4] : TAM_DET_INT
 * 
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_unmask_interrupt(uint32_t event);

/**
 * @brief Clear interrupt status of anti-tamper
 * 
 * @param[in] event each data bit that is HIGH causes the corresponding bit in the Status Register to be cleared.
 * [0] : TS_CTRL_INT
 * [1] : BOR18_INT
 * [2] : CLK_MONITOR_INT
 * [3] : RTC_INT
 * [4] : TAM_DET_INT
 * 
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_clr_interrupt(uint32_t event);

/**
 * @brief A HIGH bit indicates that a specific DMA channel interrupt request is active.
 * 
 * @param[out] status Pointer to a uint32_t variable where the interrupt status of the Anti tamper will be stored.
 * [0] : TS_CTRL_INT
 * [1] : BOR18_INT
 * [2] : CLK_MONITOR_INT
 * [3] : RTC_INT
 * [4] : TAM_DET_INT
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_get_interrupt(uint32_t *status);

/**
 * @brief Enable/Dsiable tamper detection module
 * 
 * @param[in] enable Tamper datection enable setting
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_ena_tam_det(uint32_t enable);

/**
 * @brief Set work mode for tamper detection
 * 
 * @param[in] tam_mode tamper work mode
 * 0: passive mode. The passive tamper detection just checks a static level.
 * 1: active mode. Tamper active mode is based on a comparison between a TAMP_OUTx pin and a TAMP_INx pin, 
 *                  which occurs continuously at a configurable sampling frequency.
 * 
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_set_tam_mode(uint32_t tam_mode);

/**
 * @brief Mask tampers interrupt generated by tamper detection
 * 
 * @param[in] mask tamper events interrupt mask setting. Mask for compare data bit by bit.
 * [0]: TAMP_IO0
 * [1]: TAMP_IO1
 * [2]: TAMP_IO2
 * [3]: TAMP_IO3
 * [4]: TAMP_IO4
 * 
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_mask_tam_det(uint32_t mask);

/**
 * @brief ENable tampers interrupt generated by tamper detection
 * 
 * @param[in] mask tamper events interrupt enable setting. Mask for compare data bit by bit.
 * [0]: TAMP_IO0
 * [1]: TAMP_IO1
 * [2]: TAMP_IO2
 * [3]: TAMP_IO3
 * [4]: TAMP_IO4
 * 
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_unmask_tam_det(uint32_t mask);

/**
 * @brief set fix value for compare with TAM_IN[4:0] when passive mode
 * 
 * @param[in] value set value of TAMP_IO.
 * [0]: TAMP_IO0
 * [1]: TAMP_IO1
 * [2]: TAMP_IO2
 * [3]: TAMP_IO3
 * [4]: TAMP_IO4
 * 
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_set_tam_data(uint32_t value);

/**
 * @brief Get the level of TAMP_IN[4:0]
 * 
 * @param[out] value Pointer to a uint32_t variable where the input value of the TAMP_IO will be stored.
 * [0]: TAMP_IO0
 * [1]: TAMP_IO1
 * [2]: TAMP_IO2
 * [3]: TAMP_IO3
 * [4]: TAMP_IO4
 * 
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_get_tam_in(uint32_t *value);

/**
 * @brief Get the level of TAMP_OUT[4:0]
 * 
 * @param[out] value Pointer to a uint32_t variable where the output value of the TAMP_IO will be stored.
 * [0]: TAMP_IO0
 * [1]: TAMP_IO1
 * [2]: TAMP_IO2
 * [3]: TAMP_IO3
 * [4]: TAMP_IO4
 * 
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_get_tam_out(uint32_t *value);

/**
 * @brief Get interrput staus of tamper detection
 * 
 * @param[out] value Pointer to a uint32_t variable where the interrupt stauts of the TAMP_IO will be stored.
 * [0]: TAMP_IO0
 * [1]: TAMP_IO1
 * [2]: TAMP_IO2
 * [3]: TAMP_IO3
 * [4]: TAMP_IO4
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_get_tam_interrupt(uint32_t *value);

/**
 * @brief Enable clock monitor module
 * 
 * @param[in]  enable Clock monitor enable setting. 0 or 1
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_ena_clk_moni(uint32_t enable);

/**
 * @brief Get interrupt status of clock monitor
 * 
 * @param[out] status Pointer to a uint32_t variable where the interrupt stauts of the TAMP_IO will be stored. 
 * *status = 1 --> clock monitor interrupt is active after masking.
 * *status = 0 --> clock monitor interrupt is not active after masking.
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_get_clk_moni_interrupt(uint32_t *status);

/**
 * @brief Get real counter value when interrupt happens 
 * 
 * @param[out] count Pointer to a uint32_t variable where the counter value of clock monitor will be stored. 
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_get_clk_moni_interrupt_count(uint32_t *count);

/**
 * @brief set upper threshold and lower threshold for clock monitor.
 * Issue interrupt when counter value > upper or < lower threshold
 * 
 * @param[in] upper_thr  The value of the high threshold to be set for the clock monitor
 * @param[in] lower_thr  The value of the low threshold to be set for the clock monitor
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_set_clk_moni_thr(uint32_t upper_thr, uint32_t lower_thr);

/**
 * @brief This function registers a callback function for the anti-tamper.
 * 
 * @param[in] cb_func A function pointer to the callback function that will be registered for the anti-tamper.
 * @return ANTI_TAMP_ERROR_E 
 */
extern ANTI_TAMP_ERROR_E hx_drv_anti_tamp_register_cb(ANTI_TAMP_CB cb_func);
#endif /*INC_HX_DRV_ANTI_TAMPER_H_*/ 
