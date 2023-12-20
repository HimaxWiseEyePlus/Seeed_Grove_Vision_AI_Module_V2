/**
 * @file hx_drv_ts.h
 * @author himax/903730
 * @brief 
 * @version V1.0.0
 * @date 2023-02-28
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef INC_HX_DRV_TS_H_
#define INC_HX_DRV_TS_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
* \defgroup   TS (Temperature Sensor)
* \brief      Temperature Sensor
* \details    a low power analog temperature sensor for internal IC sensing
* <pre>
* Usage-1:  get internal IC temperature
*    Sample code: 
*      /// Initialize temperature sensor
*      hx_drv_ts_ctrl_init(TS_CTRL_BASEADDR);
*
*      /// enable temperature sensor
*      hx_drv_ts_ctrl_set_enable(1);
*
*      /// get temperature sensor ADC data
*      int8_t temp_val;
*      temp_val = hx_drv_ts_analog_get_temperature();
*
*
* Usage-2:  set high/low threshold to trigger interrupt
*    Sample code: 
*      /// Initialize temperature sensor
*      hx_drv_ts_ctrl_init(TS_CTRL_BASEADDR);
*
*      /// set high/low threshold
*      /// high threshold = 100 degree + 40 = 0x8C
*      /// low threshold =    0 degree + 40 = 0x28 
*      hx_drv_ts_ctrl_set_thr(0x8C, 0x28);
*
*      /// register callback function
*      void ts_callback(uint8_t status)
*      {
*          xprintf("\n");
*          if ( status & TS_INT_WIN_HI_OUT )
*              xprintf("TS: High-OUT \n");
*          if ( status & TS_INT_WIN_HI_IN )
*              xprintf("TS: High-IN \n");
*          if ( status & TS_INT_WIN_LOW_OUT )
*              xprintf("TS: Low-OUT \n");
*          if ( status & TS_INT_WIN_LOW_IN )
*              xprintf("TS: Low-IN \n");
*          if ( status & TS_INT_WIN_HI )
*              xprintf("TS: HIGH \n");
*          if ( status & TS_INT_WIN_LOW )
*              xprintf("TS: LOW \n");
*      }
*      hx_drv_ts_ctrl_register_cb(ts_callback);
*
*      /// unmask interrupt event
*      hx_drv_ts_ctrl_set_int_mask(~(TS_INT_WIN_LOW | TS_INT_WIN_HI | TS_INT_WIN_LOW_IN | TS_INT_WIN_LOW_OUT | TS_INT_WIN_HI_IN | TS_INT_WIN_HI_OUT));
*
*      /// enable temperature sensor
*      hx_drv_ts_ctrl_set_enable(1);
*
* </pre>
* @{
*/

/**
 * \enum TS_INT_E
 * \brief Temperature Sensor interrupt enum
 */
typedef enum {
    TS_INT_WIN_HI_OUT  = 0x01, /**< Window High out interrupt*/
    TS_INT_WIN_HI_IN   = 0x02, /**< Window High in interrupt */
    TS_INT_WIN_LOW_OUT = 0x04, /**< Window Low out interrupt */
    TS_INT_WIN_LOW_IN  = 0x08, /**< Window Low in interrupt */
    TS_INT_WIN_HI      = 0x10, /**< Window High interrupt */
    TS_INT_WIN_LOW     = 0x20, /**< Window Low interrupt */
} TS_INT_E;

/**
 * \brief   Temperature Sensor Interrupt callback function
 *
 * \return  void
 */
typedef void (*TS_ISR_CB) (uint8_t int_status);


/**
 * \defgroup    Temperature Sensor Driver
 * \ingroup     TS_DRV
 * \brief       Temperature Sensor Driver  Declaration
 */

/**
 * \brief   Temperature Sensor initialization
 *
 * \param[in]   none
 * \return  
 */
void hx_drv_ts_ctrl_init(uint32_t base_addr);


/**
 * \brief   Temperature Sensor deinitialization
 *
 * \return  
 */
void hx_drv_ts_ctrl_deinit();

/**
 * \brief   set Temperature Sensor enable
 *
 * \param[in]   enable     enable Temperature Sensor setting
 * \return  
 */
void hx_drv_ts_ctrl_set_enable(uint8_t enable);


/**
 * \brief     set Temperature Sensor threshold
 *
 * \param[in]   high_thr     high threshold
 * \param[in]   low_thr      low threshold
 * \return  
 */
void hx_drv_ts_ctrl_set_thr(uint8_t high_thr, uint8_t low_thr);


/**
 * \brief     set Temperature Sensor interrupt mask
 *
 * \param[in] int_mask           interrupt mask
 * \return  
 */
void hx_drv_ts_ctrl_set_int_mask(TS_INT_E int_mask);

/**
 * \brief     get Temperature 
 *
 * \return  temperature value
 */
int8_t hx_drv_ts_analog_get_temperature();

/**
 * This function registers a callback function for the temperature sensor interrupt 
 * 
 * @param cb_func A function pointer to the callback function that will be registered to handle the
 * temperature sensor interrupt.
 */
void hx_drv_ts_ctrl_register_cb(TS_ISR_CB cb_func);

/** @} */ // end of Temperature sensor group
#ifdef __cplusplus
}
#endif
#endif /* INC_HX_DRV_TS_H_ */
