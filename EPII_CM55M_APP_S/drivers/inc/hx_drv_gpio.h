/**
 * @file hx_drv_gpio.h
 * @author himax/903730
 * @brief 
 * @version V1.0.0
 * @date 2021-06-25
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef INC_HX_DRV_GPIO_H_
#define INC_HX_DRV_GPIO_H_
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
* \defgroup   GPIO
* \brief      General-purpose input/output
* \details    a generic pin on an integrated circuit whose behavior, including whether it is an input or output pin
* <pre>
* Usage-1:  Use AON_GPIO0 as an input pin
*    Sample code: Initialize GPIO_GROUP_5_AON
*      /// The pin of AON_GPIO0 is defined by the user application.
*      hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_2, 1);
*   
*      /// Initialize GPIO_GROUP_5_AON
*      hx_drv_gpio_init(GPIO_GROUP_5, HX_GPIO_GROUP_5_BASE);
*
*      /// set AON_GPIO0 as input mode
*      hx_drv_gpio_set_input(AON_GPIO0);
*
*      /// get the external voltage level of AON_GPIO0
*      uint8_t value;
*      hx_drv_gpio_get_in_value(AON_GPIO0, &value);
*
* Usage-2:  Use AON_GPIO0 as an input pin with rising edge to trigger interrupt 
*    Sample code: Initialize GPIO_GROUP_5_AON
*      /// The pin of AON_GPIO0 is defined by the user application.
*      hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_2, 1);
*   
*      /// Initialize GPIO_GROUP_5_AON
*      hx_drv_gpio_init(GPIO_GROUP_5, HX_GPIO_GROUP_5_BASE);
*
*      /// set AON_GPIO0 as input mode
*      hx_drv_gpio_set_input(AON_GPIO0);
*
*      /// register callback function
*      void aon_gpio_0_cb(uint8_t group, uint8_t aIndex)
*      {
*          uint8_t value;
*          GPIO_INDEX_E gpio_idx = (group << 4 | aIndex);
*          
*          hx_drv_gpio_get_in_value(gpio_idx, &value);
*          xprintf("aon_gpio_0_cb(%d, %d, %d)\n", group, aIndex, value);
*      }
*
*      hx_drv_gpio_cb_register(AON_GPIO0, aon_gpio_0_cb);
*
*      /// Set interrupt triggering mode
*      hx_drv_gpio_set_int_type(AON_GPIO0, GPIO_IRQ_TRIG_TYPE_EDGE_RISING);
*
*      /// enable interrupt
*      hx_drv_gpio_set_int_enable(AON_GPIO0, 1);
*
* Usage-3  Use GPIO0 as output mode
*    Sample code: Initialize GPIO_GROUP_0
*      /// The pin of GPIO0 is defined by the user application.
*      hx_drv_scu_set_PB6_pinmux(SCU_PB6_PINMUX_GPIO0_2, 1);
*   
*      /// Initialize GPIO_GROUP_0
*      hx_drv_gpio_init(GPIO_GROUP_0, HX_GPIO_GROUP_0_BASE);
*
*      /// set GPIO0 as output mode with default output low level
*      hx_drv_gpio_set_output(GPIO0, GPIO_OUT_LOW);
*
*      /// set GPIO0 to output high level
*      hx_drv_gpio_set_out_value(GPIO0, GPIO_OUT_HIGH);
*
* </pre>
* @{
*/

/***********************************************
 * CONSTANT DEFINITION
 **********************************************/
#define GPIO_GROUP_4_SB     4
#define GPIO_GROUP_5_AON    5

/***********************************************
 * TYPEDEF DECLARATION
 **********************************************/
typedef void (*gpio_cb_t) (uint8_t group, uint8_t idx);

/***********************************************
 * EMULATOR DECLARATION
 **********************************************/
/**
 * \enum GPIO_DIR_E
 * \brief GPIO Direction enum 
 */
typedef enum GPIO_DIR_S
{
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT,
} GPIO_DIR_E;


/**
 * \enum GPIO_OUT_LEVEL_E
 * \brief GPIO Output level enum 
 */
typedef enum GPIO_OUT_LEVEL_S
{
    GPIO_OUT_LOW = 0,
    GPIO_OUT_HIGH,
} GPIO_OUT_LEVEL_E;


/**
 * \enum GPIO_ERROR_E
 * \brief GPIO Errors Type
 */
typedef enum GPIO_ERROR_S
{
    GPIO_NO_ERROR                          = 0,     /**< No ERROR */
    GPIO_ERROR_NOT_INIT                    = -1,    /**< ERROR MSG: not init */
    GPIO_ERROR_ALREADY_INIT                = -2,    /**< ERROR MSG: already init */
    GPIO_ERROR_INVALID_PARAMETERS          = -3,    /**< ERROR MSG: Invalid parameters */
    GPIO_ERROR_NULL_POINTER                = -4,    /**< ERROR MSG: NULL pointer */
    GPIO_UNKNOWN_ERROR                     = -5,    /**< ERROR MSG: UNKNOWN ERROR*/
    GPIO_UNDEFINED_INSTANCE                = -6,    /**< ERROR MSG: not compiler GPIO IP instance in project */
} GPIO_ERROR_E;


/**
 * \enum GPIO_GROUP_E
 * \brief GPIO Group 
 */
typedef enum GPIO_GROUP_S
{
    GPIO_GROUP_0 = 0,
    GPIO_GROUP_1,
    GPIO_GROUP_2,
    GPIO_GROUP_3,
    GPIO_GROUP_4 = GPIO_GROUP_4_SB,
    GPIO_GROUP_5 = GPIO_GROUP_5_AON,
    GPIO_GROUP_MAX,
} GPIO_GROUP_E;


/**
 * \enum GPIO_SUB_INDEX_E
 * \brief GPIO Sub index
 */ 
typedef enum GPIO_SUB_INDEX_S
{
    SUB_GPIO0 = 0,
    SUB_GPIO1,
    SUB_GPIO2,
    SUB_GPIO3,
    SUB_GPIO4,
    SUB_GPIO5,
    SUB_GPIO6,
    SUB_GPIO7,
    SUB_GPIO_INDEX_MAX
}GPIO_SUB_INDEX_E;


/**
 * \enum GPIO_INDEX_E
 * \brief GPIO index
 */
typedef enum {
    // GPIO_GROUP_0
    GPIO0 = (GPIO_GROUP_0 << 4 | SUB_GPIO0),
    GPIO1 = (GPIO_GROUP_0 << 4 | SUB_GPIO1),
    GPIO2 = (GPIO_GROUP_0 << 4 | SUB_GPIO2),
    GPIO3 = (GPIO_GROUP_0 << 4 | SUB_GPIO3),
    GPIO4 = (GPIO_GROUP_0 << 4 | SUB_GPIO4),
    GPIO5 = (GPIO_GROUP_0 << 4 | SUB_GPIO5),
    GPIO6 = (GPIO_GROUP_0 << 4 | SUB_GPIO6),
    GPIO7 = (GPIO_GROUP_0 << 4 | SUB_GPIO7),
    // GPIO_GROUP_1
    GPIO8 = (GPIO_GROUP_1 << 4 | SUB_GPIO0),
    GPIO9 = (GPIO_GROUP_1 << 4 | SUB_GPIO1),
    GPIO10 = (GPIO_GROUP_1 << 4 | SUB_GPIO2),
    GPIO11 = (GPIO_GROUP_1 << 4 | SUB_GPIO3),
    GPIO12 = (GPIO_GROUP_1 << 4 | SUB_GPIO4),
    GPIO13 = (GPIO_GROUP_1 << 4 | SUB_GPIO5),
    GPIO14 = (GPIO_GROUP_1 << 4 | SUB_GPIO6),
    GPIO15 = (GPIO_GROUP_1 << 4 | SUB_GPIO7),
    // GPIO_GROUP_2
    GPIO16 = (GPIO_GROUP_2 << 4 | SUB_GPIO0),
    GPIO17 = (GPIO_GROUP_2 << 4 | SUB_GPIO1),
    GPIO18 = (GPIO_GROUP_2 << 4 | SUB_GPIO2),
    GPIO19 = (GPIO_GROUP_2 << 4 | SUB_GPIO3),
    GPIO20 = (GPIO_GROUP_2 << 4 | SUB_GPIO4),
    GPIO21 = (GPIO_GROUP_2 << 4 | SUB_GPIO5),
    GPIO22 = (GPIO_GROUP_2 << 4 | SUB_GPIO6),
    GPIO23 = (GPIO_GROUP_2 << 4 | SUB_GPIO7),
    // GPIO_GROUP_3
    GPIO24 = (GPIO_GROUP_3 << 4 | SUB_GPIO0),
    GPIO25 = (GPIO_GROUP_3 << 4 | SUB_GPIO1),
    GPIO26 = (GPIO_GROUP_3 << 4 | SUB_GPIO2),
    GPIO27 = (GPIO_GROUP_3 << 4 | SUB_GPIO3),
    GPIO28 = (GPIO_GROUP_3 << 4 | SUB_GPIO4),
    GPIO29 = (GPIO_GROUP_3 << 4 | SUB_GPIO5),
    // GPIO_GROUP_4_SB
    SB_GPIO0 = (GPIO_GROUP_4_SB << 4 | SUB_GPIO0),
    SB_GPIO1 = (GPIO_GROUP_4_SB << 4 | SUB_GPIO1),
    // GPIO_GROUP_5_AON
    AON_GPIO0 = (GPIO_GROUP_5_AON << 4 | SUB_GPIO0),
    AON_GPIO1 = (GPIO_GROUP_5_AON << 4 | SUB_GPIO1),
    AON_GPIO2 = (GPIO_GROUP_5_AON << 4 | SUB_GPIO2),
} GPIO_INDEX_E;


typedef enum GPIO_IRQ_TRIG_TYPE_S
{
    GPIO_IRQ_TRIG_TYPE_EDGE_RISING    = 1 << 0,
    GPIO_IRQ_TRIG_TYPE_EDGE_FALLING   = 1 << 1,
    GPIO_IRQ_TRIG_TYPE_EDGE_BOTH      = (GPIO_IRQ_TRIG_TYPE_EDGE_FALLING | GPIO_IRQ_TRIG_TYPE_EDGE_RISING),
    GPIO_IRQ_TRIG_TYPE_LEVEL_HIGH     = 1 << 2,
    GPIO_IRQ_TRIG_TYPE_LEVEL_LOW      = 1 << 3,
}GPIO_IRQ_TRIG_TYPE_E;


/** GPIO configuration **/
typedef struct GPIO_CFG_S
{
    gpio_cb_t cb;
} GPIO_CFG_T;


/**
 * \enum GPIO_PIN_MUX_E
 * \brief GPIO PIN Mux Type
 */
typedef enum {
    // GPIO_GROUP_0
    GPIO0_PIN_MUX_PB6_F1 = 0,
    GPIO0_PIN_MUX_PB6_F2,
    GPIO0_PIN_MUX_PB9_F2,
    GPIO0_PIN_MUX_SRSTN_F2,
    GPIO1_PIN_MUX_PB7_F1,
    GPIO1_PIN_MUX_PB7_F2,
    GPIO1_PIN_MUX_PB10_F2,
    GPIO1_PIN_MUX_SWCLK_F2,
    GPIO2_PIN_MUX_PB8_F1,
    GPIO2_PIN_MUX_PB8_F2,
    GPIO2_PIN_MUX_PB11_F2,
    GPIO2_PIN_MUX_SWDIO_F2,
    GPIO3_PIN_MUX_SEN_GPIO_F2,
    GPIO4_PIN_MUX_SEN_I2CM_SDA_F2,
    GPIO5_PIN_MUX_SEN_I2CM_SCL_F2,
    GPIO6_PIN_MUX_SEN_FAE_F2,
    GPIO7_PIN_MUX_SEN_TRIG_F2,
    // GPIO_GROUP_1
    GPIO8_PIN_MUX_SEN_CSW0_F2,
    GPIO9_PIN_MUX_SEN_CSW1_F2,
    GPIO10_PIN_MUX_SEN_INT_F2,
    GPIO11_PIN_MUX_PB0_F2,
    GPIO12_PIN_MUX_PB1_F2,
    GPIO13_PIN_MUX_PB2_F2,
    GPIO14_PIN_MUX_PB3_F2,
    GPIO15_PIN_MUX_PB4_F2,
    // GPIO_GROUP_2
    GPIO16_PIN_MUX_PB5_F2,
    GPIO17_PIN_MUX_SEN_PCLK_F2,
    GPIO18_PIN_MUX_SEN_D0_F2,
    GPIO19_PIN_MUX_SEN_D1_F2,
    GPIO20_PIN_MUX_SEN_D2_F2,
    GPIO21_PIN_MUX_SEN_D3_F2,
    GPIO22_PIN_MUX_SEN_D4_F2,
    GPIO23_PIN_MUX_SEN_D5_F2,
    // GPIO_GROUP_3
    GPIO24_PIN_MUX_SEN_D6_F2,
    GPIO25_PIN_MUX_SEN_D7_F2,
    GPIO26_PIN_MUX_SEN_FVALID_F2,
    GPIO27_PIN_MUX_SEN_LVALID_F2,
    GPIO28_PIN_MUX_SEN_XSLEEP_F2,
    GPIO29_PIN_MUX_SEN_MCLK_F2,	
    // GPIO_GROUP_4_SB
    SB_GPIO0_PIN_MUX_PA2_F2,
    SB_GPIO1_PIN_MUX_PA3_F2,
    // GPIO_GROUP_5_AON
    AON_GPIO0_PIN_MUX_PA0_F0,
    AON_GPIO0_PIN_MUX_PA0_F2,
    AON_GPIO1_PIN_MUX_PA1_F2,
    AON_GPIO2_PIN_MUX_SEN_GPIO_F0,
    AON_GPIO2_PIN_MUX_SEN_GPIO_F1,
    AON_GPIO2_PIN_MUX_SEN_XSLEEP_F1,
    AON_GPIO2_PIN_MUX_SEN_MCLK_F1
} GPIO_PIN_MUX_E;
/***********************************************
 * FUNCTION DECLARATION
 **********************************************/
/**
 * This function initializes the GPIO module and sets up the interrupt handling for a specified GPIO
 * group.
 * 
 * @param group GPIO group number, which can be GPIO_GROUP_0, GPIO_GROUP_1, GPIO_GROUP_2,
 * GPIO_GROUP_3, GPIO_GROUP_4 (GPIO_GROUP_4_SB) or GPIO_GROUP_5 (GPIO_GROUP_5_AON)
 * @param base_addr The base address of the GPIO group to be initialized. This is the memory address
 * where the GPIO registers for the specified group are located.
 * 
 * @return a GPIO_ERROR_E enumeration value, which could be either GPIO_NO_ERROR or
 * GPIO_ERROR_INVALID_PARAMETERS depending on the input parameters.
 */
GPIO_ERROR_E hx_drv_gpio_init(GPIO_GROUP_E, uint32_t base_addr);


/**
 * This function deinitializes all GPIO groups and clears their interrupt status and callback
 * functions.
 * 
 * @return GPIO_NO_ERROR is being returned.
 */
GPIO_ERROR_E hx_drv_gpio_deinit(void);


/**
 * This function sets a GPIO pin as an input and returns an error if the GPIO group is undefined.
 * 
 * @param gpio_idx The index of the GPIO pin to set as input.
 * 
 * @return either GPIO_NO_ERROR if the GPIO input is set successfully or GPIO_UNDEFINED_INSTANCE if the
 * GPIO group is undefined.
 */
GPIO_ERROR_E hx_drv_gpio_set_input(GPIO_INDEX_E gpio_idx);


/**
 * This function sets the output level of a GPIO pin and returns an error code if the GPIO group is
 * undefined.
 * 
 * @param gpio_idx The GPIO index number that identifies the specific GPIO pin to be set as output.
 * @param def_val The default output level to set for the specified GPIO pin. It can be either
 * GPIO_OUT_LOW or GPIO_OUT_HIGH, indicating whether the pin should be set to a low or high output
 * level, respectively.
 * 
 * @return either GPIO_NO_ERROR or GPIO_UNDEFINED_INSTANCE depending on the condition inside the
 * if-else statement.
 */
GPIO_ERROR_E hx_drv_gpio_set_output(GPIO_INDEX_E gpio_idx, GPIO_OUT_LEVEL_E def_val);


/**
 * This function sets the output value of a GPIO pin and returns an error code if the GPIO group is
 * undefined.
 * 
 * @param gpio_idx The index of the GPIO pin being set.
 * @param aValue The value to be set on the GPIO pin, either GPIO_OUT_HIGH or GPIO_OUT_LOW.
 * 
 * @return an error code of type GPIO_ERROR_E. It could be either GPIO_NO_ERROR if the operation was
 * successful or GPIO_UNDEFINED_INSTANCE if the GPIO group is not defined.
 */
GPIO_ERROR_E hx_drv_gpio_set_out_value(GPIO_INDEX_E gpio_idx, GPIO_OUT_LEVEL_E aValue);


/**
 * This function gets the input value of a specified GPIO pin and returns an error code if there is an
 * issue.
 * 
 * @param gpio_idx The index of the GPIO pin being accessed.
 * @param aValue a pointer to a uint8_t variable where the input value of the GPIO pin will be stored.
 * 
 * @return either GPIO_NO_ERROR, GPIO_ERROR_NULL_POINTER, or GPIO_UNDEFINED_INSTANCE depending on the
 * conditions met in the code.
 */
GPIO_ERROR_E hx_drv_gpio_get_in_value(GPIO_INDEX_E gpio_idx, uint8_t * aValue);


/**
 * This function registers a callback function for a specific GPIO pin.
 * 
 * @param gpio_idx GPIO index number, which identifies the specific GPIO pin to register the callback
 * function for.
 * @param cb_fun A function pointer to the callback function that will be registered for the specified
 * GPIO index.
 * 
 * @return a GPIO_ERROR_E enumeration value, which could be either GPIO_ERROR_NULL_POINTER or
 * GPIO_NO_ERROR.
 */
GPIO_ERROR_E hx_drv_gpio_cb_register(GPIO_INDEX_E gpio_idx, gpio_cb_t cb_fun);


/**
 * This function sets the interrupt type for a specified GPIO pin.
 * 
 * @param gpio_idx The GPIO index number that identifies the specific GPIO pin being configured.
 * @param aValue The trigger type for the GPIO interrupt. It can be one of the following values:
 * 
 * @return an enumerated value of type GPIO_ERROR_E. The possible values that can be returned are
 * GPIO_NO_ERROR or GPIO_UNDEFINED_INSTANCE.
 */
GPIO_ERROR_E hx_drv_gpio_set_int_type(GPIO_INDEX_E gpio_idx, GPIO_IRQ_TRIG_TYPE_E aValue);


/**
 * This function sets the interrupt enable status for a specified GPIO pin.
 * 
 * @param gpio_idx The GPIO index number that identifies the specific GPIO pin being configured.
 * @param aValue a boolean value indicating whether to enable or disable the interrupt for the
 * specified GPIO pin. True means enable, false means disable.
 * 
 * @return an error code of type GPIO_ERROR_E. It could be either GPIO_NO_ERROR if the function
 * executes successfully or GPIO_UNDEFINED_INSTANCE if the GPIO group is undefined.
 */
GPIO_ERROR_E hx_drv_gpio_set_int_enable(GPIO_INDEX_E gpio_idx, uint8_t aValue);


/**
 * This function returns the interrupt status of a specified GPIO pin.
 * 
 * @param gpio_idx The index of the GPIO pin for which the interrupt status is being checked.
 * 
 * @return a boolean value indicating the interrupt status of the specified GPIO pin.
 */
bool hx_drv_gpio_get_int_status(GPIO_INDEX_E gpio_idx);


/**
 * This function clears the interrupt status of a specified GPIO pin.
 * 
 * @param gpio_idx The index of the GPIO pin to clear the interrupt status for.
 * 
 * @return a value of type GPIO_ERROR_E, which could be either GPIO_NO_ERROR or GPIO_UNDEFINED_INSTANCE
 * depending on the execution path.
 */
GPIO_ERROR_E hx_drv_gpio_clr_int_status(GPIO_INDEX_E gpio_idx);

/** @} */ // end of GPIO group

#ifdef __cplusplus
}
#endif
#endif /* INC_HX_DRV_GPIO_H_ */
