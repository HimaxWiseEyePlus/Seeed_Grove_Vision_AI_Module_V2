/**
 * @file hx_drv_adcc_hv.h
 * @author himax/902453
 * @brief 
 * @version V1.0.0
 * @date 2018-11-28
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef INC_HX_DRV_ADCC_HV_H_
#define INC_HX_DRV_ADCC_HV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_adcc_common.h"

/**
* \defgroup   ADC-HV
* \brief      ADC-HV Modulation.
* \details    Analog-to-Digital (8bit) Converter Controller module 
* <pre>
* Usage-1: Using channel 0 to obtain ADC value, with a maximum input voltage selected as 1.8V.  
*    Sample code: 
*      uint16_t data;
*      ADCC_CFG_T g_adcc_hv_cfg;
*      hx_drv_adcc_hv_get_defCfg(&g_adcc_hv_cfg);
*
*      g_adcc_hv_cfg.analog_vol = ADCC_VOL_1P8V;
*      hx_drv_adcc_hv_init(HW_ADCC_HV_BASEADDR, &g_adcc_hv_cfg);
*
*      /// unmask channel 0
*      hx_drv_adcc_hv_set_channel_mask(~ADCC_CH0); 
*
*      /// enable ADC module
*      hx_drv_adcc_hv_set_enable(1);
*
*      /// get channel 0 ADC data
*      hx_drv_adcc_hv_get_sample_data(ADCC_CHANNEL0, &data);
*
*
* Usage-2: Using channel 0 interrupt, with a maximum input voltage selected as 1.8V.  
*    Sample code: 
*      static void adcc_hv_callback_ch_0(UINT32_T status)
*      {
*          xprintf("ADCC_HV CB_FUN_CH0 : 0x%x\n", status);
*           
*          /// The data, from low to high, exceeded the high threshold
*          if ( status & ADCC_CH0_HIGH_OUT_INT_BIT )
*              xprintf("ADCC_HV CH0 High OUT\n");
*          
*          /// The data, from high to low, below the high threshold
*          if ( status & ADCC_CH0_HIGH_IN_INT_BIT )
*              xprintf("ADCC_HV CH0 High IN\n");
*
*          /// The data, from high to low, below the low threshold
*          if ( status & ADCC_CH0_LOW_OUT_INT_BIT )
*              xprintf("ADCC_HV CH0 Low OUT\n");
*
*          /// The data, from low to high, exceeded the low threshold
*          if ( status & ADCC_CH0_LOW_IN_INT_BIT )
*              xprintf("ADCC_HV CH0 Low IN\n");
*
*          /// The first data is above the high threshold
*          if ( status & ADCC_CH0_HIGH_INT_BIT )
*              xprintf("ADCC_HV CH0 Init HIGH INT\n");
*
*          /// The first data is below the low threshold
*          if ( status & ADCC_CH0_LOW_INT_BIT )
*              xprintf("ADCC_HV CH0 Init LOW INT\n");
*      }
*
*      ADCC_CFG_T g_adcc_hv_cfg;
*      hx_drv_adcc_hv_get_defCfg(&g_adcc_hv_cfg);
*
*      g_adcc_hv_cfg.analog_vol = ADCC_VOL_1P8V;
*      hx_drv_adcc_hv_init(HW_ADCC_HV_BASEADDR, &g_adcc_hv_cfg);
*
*      /// Setting the high/low threshold for channel 0
*      hx_drv_adcc_hv_set_high_threshold(ADCC_CHANNEL0, 176);
*      hx_drv_adcc_hv_set_low_threshold(ADCC_CHANNEL0, 32);
*
*      /// register channel 0 callback function
*      hx_drv_adcc_hv_register_cb(ADCC_CH0_CB, adcc_hv_callback_ch_0);
*
*      /// unmask channel 0 interrupt
*      hx_drv_adcc_hv_set_int_mask(~(ADCC_CH1_HIGH_INT_BIT | ADCC_CH1_LOW_INT_BIT |ADCC_CH0_HIGH_OUT_INT_BIT | ADCC_CH0_HIGH_IN_INT_BIT | ADCC_CH0_LOW_OUT_INT_BIT | ADCC_CH0_LOW_IN_INT_BIT); 
*
*      /// unmask channel 0
*      hx_drv_adcc_hv_set_channel_mask(~ADCC_CH0); 
*
*      /// enable ADC module
*      hx_drv_adcc_hv_set_enable(1);
*
*
* </pre>
* @{
*/



/**
 * This function initializes the ADC controller with the given configuration.
 * 
 * @param base_addr The base address of the ADC LV controller.
 * @param adcc_cfg A pointer to a structure containing the configuration settings for the ADC
 * controller.
 * 
 * @return an ADCC_ERROR_E value, which is an enumerated type defined in the code. The possible values
 * that can be returned are ADCC_NO_ERROR or ADCC_ERROR_INVALID_PARAMETERS.
 */
ADCC_ERROR_E hx_drv_adcc_hv_init(uint32_t base_addr, ADCC_CFG_T *adcc_cfg);


/**
 * This function deinitializes the ADC controller and returns an error code.
 * 
 * @return The function `hx_drv_adcc_deinit()` is returning an `ADCC_ERROR_E` enum value, specifically
 * `ADCC_NO_ERROR`.
 */
ADCC_ERROR_E hx_drv_adcc_hv_deinit();


/**
 * The function copies the default ADCC configuration to the provided ADCC configuration structure.
 * 
 * @param adcc_cfg A pointer to an ADCC_CFG_T structure that will be filled with the default
 * configuration values for the ADC controller.
 * 
 * @return an ADCC_ERROR_E enum value, which in this case is ADCC_NO_ERROR.
 */
ADCC_ERROR_E hx_drv_adcc_hv_get_defCfg(ADCC_CFG_T *adcc_cfg);


/**
 * The function sets the enable status of the ADCC and returns an error if it has not been initialized.
 * 
 * @param enable A boolean value indicating whether to enable or disable the ADCC (Analog-to-Digital
 * Converter Controller) module. If set to true, the ADCC module will be enabled, and if set to false,
 * it will be disabled.
 * 
 * @return an ADCC_ERROR_E enum value, which could be ADCC_ERROR_NOT_INIT if g_adcc_init is false, or
 * ADCC_NO_ERROR if the function executes successfully.
 */
ADCC_ERROR_E hx_drv_adcc_hv_set_enable(uint8_t enable);


/**
 * The function sets the ADCC to single shot mode if it is enabled.
 * 
 * @param enable A boolean value indicating whether to enable or disable single-shot mode for the ADC.
 * If set to true, the ADC will perform a single conversion and then stop. If set to false, the ADC
 * will continuously perform conversions until disabled.
 * 
 * @return an ADCC_ERROR_E enum value, which could be ADCC_ERROR_NOT_INIT if the ADCC module has not
 * been initialized, or ADCC_NO_ERROR if the function executes successfully.
 */
ADCC_ERROR_E hx_drv_adcc_hv_set_single_shot(uint8_t enable);


/**
 * This function sets the channel mask for the ADC and returns an error if the ADC is not initialized.
 * 
 * @param channel_mask an 8-bit value representing the channels to be enabled or disabled in the ADC.
 * Each bit in the value corresponds to a specific channel, with a value of 1 indicating that the
 * channel should be enabled and a value of 0 indicating that it should be disabled.
 * 
 *           #define ADCC_CH0                    (1)
 *           #define ADCC_CH1                    (2)
 *           #define ADCC_CH2                    (4)
 *           #define ADCC_CH3                    (8)
 *
 * @return an ADCC_ERROR_E enum value, which could be either ADCC_ERROR_NOT_INIT or ADCC_NO_ERROR.
 */
ADCC_ERROR_E hx_drv_adcc_hv_set_channel_mask(uint8_t channel_mask);


/**
 * This function sets the sample rate for a specified ADCC channel.
 * 
 * @param channel The ADC channel for which the sample rate is being set. It can be ADCC_CHANNEL0,
 * ADCC_CHANNEL1, ADCC_CHANNEL2, or ADCC_CHANNEL3.
 * @param sample_rate The sampling rate to be set for the specified ADCC channel.
 * 
 * @return an ADCC_ERROR_E enum value, which could be ADCC_ERROR_NOT_INIT,
 * ADCC_ERROR_INVALID_PARAMETERS, or ADCC_NO_ERROR.
 */
ADCC_ERROR_E hx_drv_adcc_hv_set_sample_rate(ADCC_CHANNEL channel, uint16_t sample_rate);


/**
 * This function sets the data type for the Analog-to-Digital Converter (ADC) and returns an error code
 * if the ADC is not initialized.
 * 
 * @param data_type The data type to be set for the ADC converter. It is of type ADCC_DATA_TYPE, which
 * is likely an enumeration or a typedef defined elsewhere in the code.
 *              [0] ch1 data type select (bypass/average)
 *              [1] ch2 data type select (bypass/average)
 *              [2] ch3 data type select (bypass/average)
 *              [3] ch4 data type select (bypass/average)
 * @return an ADCC_ERROR_E enum value, which could be ADCC_ERROR_NOT_INIT if the ADCC module is not
 * initialized or ADCC_NO_ERROR if the data type is set successfully.
 */
ADCC_ERROR_E hx_drv_adcc_hv_set_data_type(uint16_t data_type);


/**
 * The function sets the data offset for a specified ADCC channel.
 * 
 * @param channel The ADC channel number (0-3) for which the data offset needs to be set.
 * @param offset The offset value to be set for the specified ADCC channel. This value is added to the
 * raw ADC value before it is converted to a voltage value.
 * 
 * @return an ADCC_ERROR_E enum value, which could be ADCC_ERROR_NOT_INIT,
 * ADCC_ERROR_INVALID_PARAMETERS, or ADCC_NO_ERROR.
 */
ADCC_ERROR_E hx_drv_adcc_hv_set_data_offset(ADCC_CHANNEL channel, uint16_t offset);


/**
 * This function sets the high threshold value for a specified ADCC channel.
 * 
 * @param channel The ADC channel number (0-3) for which the high threshold value is being set.
 * @param high_threshold The value of the high threshold to be set for the specified ADCC channel. This
 * threshold is used to trigger an interrupt when the ADC value exceeds this
 * threshold.
 * 
 * @return an ADCC_ERROR_E enum value, which could be ADCC_ERROR_NOT_INIT,
 * ADCC_ERROR_INVALID_PARAMETERS, or ADCC_NO_ERROR.
 */
ADCC_ERROR_E hx_drv_adcc_hv_set_high_threshold(ADCC_CHANNEL channel, uint16_t high_threshold);


/**
 * This function sets the low threshold value for a specified ADC channel.
 * 
 * @param channel The ADC channel number (0-3) for which the low threshold needs to be set.
 * @param low_threshold The value of the low threshold to be set for the specified ADC channel. This
 * threshold is used to trigger an interrupt or event when the ADC value falls below this threshold.
 * 
 * @return an ADCC_ERROR_E enum value, which could be ADCC_ERROR_NOT_INIT,
 * ADCC_ERROR_INVALID_PARAMETERS, or ADCC_NO_ERROR.
 */
ADCC_ERROR_E hx_drv_adcc_hv_set_low_threshold(ADCC_CHANNEL channel, uint16_t low_threshold);


/**
 * This function sets the interrupt delay for the ADC controller.
 * 
 * @param int_delay The parameter int_delay is an unsigned 16-bit integer that represents the delay
 * time in ADC clock cycles for the ADCC interrupt.
 *        0:There will be no delay 1,2,1023 : 5ms, 10ms, 5.115s (max)
 * 
 * @return an ADCC_ERROR_E enum value, which could be ADCC_ERROR_NOT_INIT if the ADCC module is not
 * initialized, or ADCC_NO_ERROR if the function executes successfully.
 */
ADCC_ERROR_E hx_drv_adcc_hv_set_int_delay(uint16_t int_delay);


/**
 * The function sets the interrupt mask for the ADC controller and returns an error code if the
 * controller is not initialized.
 * 
 * @param int_mask The int_mask parameter is a 32-bit integer that represents the interrupt mask for
 * the ADC controller. It is used to enable or disable specific ADC interrupts.
 * 
 *       ADCC_CH0_INT_MASK = (ADCC_CH0_LOW_INT_BIT | ADCC_CH0_HIGH_INT_BIT | ADCC_CH0_LOW_IN_INT_BIT | ADCC_CH0_LOW_OUT_INT_BIT | ADCC_CH0_HIGH_IN_INT_BIT | ADCC_CH0_HIGH_OUT_INT_BIT)      
 *       ADCC_CH1_INT_MASK = (ADCC_CH1_LOW_INT_BIT | ADCC_CH1_HIGH_INT_BIT | ADCC_CH1_LOW_IN_INT_BIT | ADCC_CH1_LOW_OUT_INT_BIT | ADCC_CH1_HIGH_IN_INT_BIT | ADCC_CH1_HIGH_OUT_INT_BIT)           
 *       ADCC_CH2_INT_MASK = (ADCC_CH2_LOW_INT_BIT | ADCC_CH2_HIGH_INT_BIT | ADCC_CH2_LOW_IN_INT_BIT | ADCC_CH2_LOW_OUT_INT_BIT | ADCC_CH2_HIGH_IN_INT_BIT | ADCC_CH2_HIGH_OUT_INT_BIT)           
 *       ADCC_CH3_INT_MASK = (ADCC_CH3_LOW_INT_BIT | ADCC_CH3_HIGH_INT_BIT | ADCC_CH3_LOW_IN_INT_BIT | ADCC_CH3_LOW_OUT_INT_BIT | ADCC_CH3_HIGH_IN_INT_BIT | ADCC_CH3_HIGH_OUT_INT_BIT)           
 *       ADCC_CONSTRAIN_INT_MASK
 *       ADCC_SG_INT_MASK       
 *       ADCC_ALL_INT_MASK = (ADCC_SG_INT_MASK | ADCC_CONSTRAIN_INT_MASK | ADCC_CH3_INT_MASK | ADCC_CH2_INT_MASK | ADCC_CH1_INT_MASK | ADCC_CH0_INT_MASK)
 * 
 * @return an ADCC_ERROR_E enum value, which could be ADCC_ERROR_NOT_INIT if the ADCC module is not
 * initialized or ADCC_NO_ERROR if the function executes successfully.
 */
ADCC_ERROR_E hx_drv_adcc_hv_set_int_mask(uint32_t int_mask);


/**
 * This function sets the data select for the ADCC and returns an error if the ADCC is not initialized.
 * 
 * @param data_select an enum type variable that specifies the type of data to be selected for ADC
 * conversion. 
 *              [0] ch0 sample data, (0: Data Mux / 1: Data Offset)
 *              [1] ch2 sample data, (0: Data Mux / 1: Data Offset)
 *              [2] ch3 sample data, (0: Data Mux / 1: Data Offset)
 *              [3] ch4 sample data, (0: Data Mux / 1: Data Offset)
 * 
 * @return an ADCC_ERROR_E enum value, which could be ADCC_ERROR_NOT_INIT if the ADCC module is not
 * initialized or ADCC_NO_ERROR if the data select is set successfully.
 */
ADCC_ERROR_E hx_drv_adcc_hv_set_data_select(uint16_t data_select);


/**
 * This function gets the sample data from a specified ADCC channel and applies a trim offset and gain
 * if they have been previously set.
 * 
 * @param channel The ADC channel from which to get the sample data. It can be ADCC_CHANNEL0,
 * ADCC_CHANNEL1, ADCC_CHANNEL2, or ADCC_CHANNEL3.
 * @param channel_data A pointer to a uint16_t variable where the sample data (8bit)will be stored.
 * 
 * @return an ADCC_ERROR_E enum value, which could be ADCC_ERROR_NOT_INIT,
 * ADCC_ERROR_INVALID_PARAMETERS, or ADCC_NO_ERROR.
 */
ADCC_ERROR_E hx_drv_adcc_hv_get_sample_data(ADCC_CHANNEL channel, uint16_t *channel_data);


/**
 * This function registers a callback function for the ADCC module and enables/disables the
 * corresponding interrupt.
 * 
 * @param cb_type An enumeration type that specifies the type of callback function being registered.
 * @param cb_func A function pointer to the callback function that will be registered for the specified
 * cb_type.
 * 
 * @return an ADCC_ERROR_E enumeration value, which could be ADCC_NO_ERROR or one of the error codes
 * defined in the enumeration.
 */
ADCC_ERROR_E hx_drv_adcc_hv_register_cb(ADCC_CALLBACK_E cb_type, ADCC_ISR_CB cb_func);

/** @} */ // end of ADC-HV group
#ifdef __cplusplus
}
#endif
#endif /* INC_HX_DRV_ADCC_HV_H_ */
