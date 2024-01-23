/**
 * @file hx_drv_pwm.h
 * @author himax/903730
 * @brief 
 * @version V1.0.0
 * @date 2019-01-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef INC_HX_DRV_PWM_H_
#define INC_HX_DRV_PWM_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
* \defgroup   PWM
* \brief      Pulse-Width Modulation.
* \details    Output PWM signal.
* <pre>
* Usage-1:  PWM0 continuously outputs at a frequency of 5kHz with a duty cycle of 20%.
*
*    Sample code: 
*      /// The output pin of PWM0 is defined by the user application.
*      hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_PWM0, 1);
*   
*      /// initializes the PWM0
*      hx_drv_pwm_init(PWM0, HW_PWM0_BASEADDR);
*
*      /// PWM0 starts outputting according to the set value. (The high period is 20%, and the low period is 80%)
*      pwm_ctrl ctrl; 
*      ctrl.mode = PWM_MODE_CONTINUOUS;
*      ctrl.pol = PWM_POL_NORMAL;
*      ctrl.freq = 5000;
*      ctrl.duty = 20;
*      hx_drv_pwm_start(PWM0, &ctrl);
*
* Usage-2:  PWM outputs 10 pulses with a frequency of 5kHz and a duty cycle of 20%,
*           and triggers an interrupt at the end to execute the registered callback function.
*
*    Sample code: 
*      /// The output pin of PWM0 is defined by the user application.
*      hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_PWM0, 1);
*   
*      /// initializes the PWM0
*      hx_drv_pwm_init(PWM0, HW_PWM0_BASEADDR);
*
*      /// register callback function
*      hx_drv_pwm_register_cb(PWM0, cli_pwm_0_cb); 
*
*      /// PWM0 starts outputting according to the set value. (The low period is 20%, and the high period is 80%) 
*      pwm_ctrl ctrl; 
*      ctrl.mode = PWM_MODE_SINGLE_SHOT;
*      ctrl.pol = PWM_POL_INVERSE;
*      ctrl.freq = 5000;
*      ctrl.duty = 20;
*      ctrl.output_num = 10;
*      hx_drv_pwm_start(PWM0, &ctrl);
* </pre>
* @{
*/


/***********************************************
 * CONSTANT DEFINITION
 **********************************************/


/***********************************************
 * EMULATOR DECLARATION
 **********************************************/
/** @brief	This enum defines the PWM instance index.  */
typedef enum PWM_INDEX_S
{
    PWM0 = 0,
    PWM1,
    PWM2,
    PWM_MAX
} PWM_INDEX_E;


/** @brief	This enum defines the PWM error code.  */
typedef enum PWM_ERROR_S
{
    PWM_NO_ERROR                        = 0,    /**< No ERROR */
    PWM_ERROR_NOT_INIT                  = 1,    /**< ERROR MSG: not init */
    PWM_ERROR_ALREADY_INIT              = 2,    /**< ERROR MSG: already init */
    PWM_ERROR_INVALID_PARAMETERS        = 3,    /**< ERROR MSG: Invalid parameters */
    PWM_ERROR_NULL_POINTER              = 4,    /**< ERROR MSG: NULL pointer */
    PWM_UNKNOWN_ERROR                   = 5,    /**< ERROR MSG: UNKNOWN ERROR*/
    PWM_ERROR_BUSY_STATUS               = 6,    /**< ERROR MSG: BUSY */
    PWM_ERROR_UNDEFINED_INSTANCE        = 7,    /**< ERROR MSG: not compiler PWM IP instance in project */
} PWM_ERROR_E;


/** @brief	This enum defines the PWM pin mux.  */
typedef enum {
	PWM0_PIN_MUX_PB0 = 0,
	PWM0_PIN_MUX_PB3,
	PWM0_PIN_MUX_PB6,
    PWM0_PIN_MUX_PB11,
	PWM1_PIN_MUX_PB1,
	PWM1_PIN_MUX_PB4,
	PWM1_PIN_MUX_PB7,
    PWM1_PIN_MUX_PB9,
	PWM2_PIN_MUX_PB2,
	PWM2_PIN_MUX_PB5,
	PWM2_PIN_MUX_PB8,
    PWM2_PIN_MUX_PB10,
} PWM_PIN_MUX_E;


/** @brief	This enum defines the PWM signal polarity. */
typedef enum {
    PWM_POL_NORMAL        = 0,         /**< PWM outputs square waves in normal polarity. */
    PWM_POL_INVERSE       = 1,         /**< PWM outputs square waves in inverse polarity . */
} PWM_POL_E;


/** @brief	This enum defines the PWM mode.  */
typedef enum {
    PWM_MODE_SINGLE_SHOT  = 0,         /**< The specified number of square wave outputs only occurs once. */
    PWM_MODE_CONTINUOUS   = 1,         /**< PWM always outputs square waves. */
} PWM_OUTPUT_MODE_E;

/***********************************************
 * TYPEDEF DECLARATION
 **********************************************/
typedef void (*pwm_cb_t) (void* param);


/**
 * \struct pwm_ctrl
 * \brief the structure of PWM control
 */
typedef struct pwm_ctrl_s{
    PWM_OUTPUT_MODE_E  mode;        /**< PWM_OUTPUT_MODE_E */    
    PWM_POL_E  pol;                 /**< PWM_POL_E */
    uint8_t  duty;                  /**< min:1 ~ max:99 */
    uint32_t freq;                  /**< 1Hz ~ (lsc_ref_clk/2) */
    uint32_t output_num;            /**< output the uumber of square waves: 1 ~ 1023 */
}pwm_ctrl, *pwm_ctrl_ptr;

/***********************************************
 * FUNCTION DECLARATION
 **********************************************/
/**
 * This function initializes a PWM module and sets its default values.
 * 
 * @param aIndex an enumeration value representing the PWM index (PWM0, PWM1, or PWM2)
 * @param base_addr The base address of the PWM module. It is a uint32_t type variable that specifies
 * the memory address where the PWM module is located.
 * 
 * @return a value of type PWM_ERROR_E, which is an enumerated type defined in the code. The possible
 * values that can be returned are PWM_NO_ERROR, PWM_ERROR_INVALID_PARAMETERS, and
 * PWM_ERROR_ALREADY_INIT.
 */
PWM_ERROR_E hx_drv_pwm_init(PWM_INDEX_E aIndex, uint32_t base_addr);


/**
 * This function deinitializes a PWM channel and sets its initialization status to false.
 * 
 * @param aIndex The index of the PWM module to be deinitialized. It is of type PWM_INDEX_E, which is
 * an enumeration type defined in the code.
 * 
 * @return a value of type PWM_ERROR_E, which is an enumerated type defined in the code. The possible
 * values that can be returned are PWM_NO_ERROR or PWM_ERROR_INVALID_PARAMETERS.
 */
PWM_ERROR_E hx_drv_pwm_deinit(PWM_INDEX_E aIndex);


/**
 * This function registers a callback function for a specified PWM index and returns an error code if
 * the index or callback function is invalid.
 * 
 * @param aIndex an enumeration value representing the PWM channel index
 * @param cb_fun cb_fun is a function pointer to the callback function that will be registered for the
 * specified PWM index. This function will be called when the PWM signal reaches the end of a cycle.
 * 
 * @return The function `hx_drv_pwm_register_cb` is returning an error code of type `PWM_ERROR_E`. The
 * possible return values are `PWM_NO_ERROR` if the function executes successfully,
 * `PWM_ERROR_INVALID_PARAMETERS` if the `aIndex` parameter is out of range, and
 * `PWM_ERROR_NULL_POINTER` if the `cb_fun` parameter is NULL.
 */
PWM_ERROR_E hx_drv_pwm_register_cb(PWM_INDEX_E aIndex, pwm_cb_t  cb_fun);


/**
 * This function starts the PWM output with the specified configuration.
 * 
 * @param aIndex The index of the PWM module to be used. It is of type PWM_INDEX_E, which is an
 * enumeration of all available PWM modules.
 * @param aPwmCtrl a pointer to a structure containing the PWM control parameters such as frequency,
 * duty cycle, output number, polarity, and mode.
 * 
 * @return a PWM_ERROR_E value, which is an enumerated type indicating the status of the PWM operation.
 */
PWM_ERROR_E hx_drv_pwm_start(PWM_INDEX_E aIndex, pwm_ctrl_ptr aCtrl);


/**
 * The function hx_drv_pwm_stop stops the PWM signal for a specified PWM index.
 * 
 * @param aIndex The index of the PWM instance to stop.
 * 
 * @return a variable of type PWM_ERROR_E, which is an enumerated type representing different error
 * codes related to PWM operations. The specific error code being returned depends on the outcome of
 * the function's execution.
 */
PWM_ERROR_E hx_drv_pwm_stop(PWM_INDEX_E aIndex);
/** @} */ // end of PWM group

#ifdef __cplusplus
}
#endif
#endif /* INC_HX_DRV_PWM_H_ */
