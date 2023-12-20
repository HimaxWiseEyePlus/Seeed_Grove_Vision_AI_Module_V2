/**
 * @file hx_drv_vad.h
 * @author himax/902453
 * @brief 
 * @version V1.0.0
 * @date 2021-09-16
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef INC_HX_DRV_VAD_H_
#define INC_HX_DRV_VAD_H_

#include "dev_common.h"

#ifdef IP_pdm
#include "hx_drv_pdm_rx.h"
#endif
#ifdef IP_i2s
#include "hx_drv_i2s.h"
#endif

/**
* \defgroup   HW-VAD
* \brief      HW Voice Vctivity Detection 
* \details    detect the voice signal exist or not
* <pre>
* Usage-1: Using PDM as the hardware VAD (Voice Activity Detection) sound detection source
*
*    Sample code: 
*      /// Specify the HW VAD source as the left channel of PDM channel 0 and initialize the HW VAD
*      VAD_CFG_T g_vad_cfg;
*      hx_drv_vad_get_defCfg(&g_vad_cfg);
*      g_vad_cfg.vad_src = VAD_SRC_PDM;
*      g_vad_cfg.vad_ch = VAD_PCM_DATA_L_CH;
*      g_vad_cfg.pdm_data_path_out = PDM_DATA_PATH_0_OUT_TO_VAD;
*      hx_drv_vad_init(HW_VAD_BASEADDR, &g_vad_cfg);
*   
*      /// register the callback function
*      hx_drv_vad_register_cb(vad_callback);
*
*      /// enable HW-VAD
*      hx_drv_vad_set_enable(1);
*
* </pre>
* @{
*/

/**
 * \enum    VAD_ERROR_E
 * \brief   VAD Errors Type
 * 
 */
typedef enum VAD_ERROR_S
{
	VAD_NO_ERROR						= 0,	/**< No ERROR */
	VAD_ERROR_NOT_INIT					= 1,	/**< ERROR MSG: not init */
	VAD_ERROR_ALREADY_INIT				= 2,	/**< ERROR MSG: already init */
	VAD_ERROR_INVALID_PARAMETERS		= 3,	/**< ERROR MSG: Invalid parameters */
 	VAD_UNKNOWN_ERROR					= 4,	/**< ERROR MSG: UNKNOWN ERROR*/
} VAD_ERROR_E;


/**
 * \enum VAD_HIGH_PASS_FILTER_SEL_E
 * \brief VAD high pass filter Selection
 */
typedef enum {
	VAD_HIGH_PASS_FILTER_BYPASS = 0, 		/**< bypass HPF */ 
	VAD_HIGH_PASS_FILTER_CUTOFF_2K = 1, 	/**< cutoff 2k @ 16k fs */
	VAD_HIGH_PASS_FILTER_CUTOFF_1K = 2, 	/**< cutoff 1k @ 16k fs */ 
} VAD_HIGH_PASS_FILTER_SEL_E;


/**
 * \enum VAD_LUT_SEL_E
 * \brief VAD Look up table Selection
 */
typedef enum {
	VAD_LUT_NO_BYPASS = 0, 					/**< use LUT */ 
	VAD_LUT_BYPASS = 1, 					/**< bypass LUT */
} VAD_LUT_SEL_E;


/**
 * \enum VAD_INVERSE_FILTER_SEL_E
 * \brief VAD inverse filter Selection
 */
typedef enum {
	VAD_INVERSE_FILTER_NO_BYPASS = 0,		/**< use inverse filter */ 
	VAD_INVERSE_FILTER_BYPASS = 1,			/**< bypass inverse filter */
} VAD_INVERSE_FILTER_SEL_E;

/**
 * \enum VAD_SRC_SEL_E
 * \brief VAD Source Selection
 */
typedef enum {
	VAD_SRC_PDM = 0,						/**< VAD source is PDM */ 
	VAD_SRC_I2S_MASTER = 1,					/**< VAD source is I2S Master */ 
	VAD_SRC_I2S_SLAVE = 2,					/**< VAD source is I2S Slave */ 
} VAD_SRC_SEL_E;

/**
 * \enum VAD_PCM_DATA_SEL_E
 * \brief VAD PDM Data Selection
 */
typedef enum {
	VAD_PCM_DATA_R_CH = 0,
	VAD_PCM_DATA_L_CH = 1,
} VAD_PCM_DATA_SEL_E;

/**
 * \struct  VAD_CFG_T
 * \brief   VAD configuration
 */
typedef struct VAD_CFG_S
{
	uint8_t input_gain;								/**<  input_gain[3:0]: 0 ~ 15 */ 
	uint8_t speech_gain;							/**< speech_gain[3:0]: 0 ~ 14 */ 
	uint8_t noise_gain;								/**<  noise_gain[3:0]: 0 ~ 14 */
	VAD_HIGH_PASS_FILTER_SEL_E high_pass;
	VAD_LUT_SEL_E lut;
	VAD_INVERSE_FILTER_SEL_E inverse;
	uint8_t alg_mode;
	uint16_t k_noise;								/**< noise value[15:0] */ 
	VAD_SRC_SEL_E          vad_src;					/**< VAD source is PDM or I2S master or I2S slave */ 
	VAD_PCM_DATA_SEL_E      vad_ch;  				/**< VAD select L or R channel */ 
	PDM_DATA_PATH_OUT_TO_VAD_E pdm_data_path_out;	/**< Specify which data path of PDM to output to the VAD */ 
} VAD_CFG_T;


/**
 * \brief	VAD Interrupt callback function
 *
 * \return	void
 */
typedef void (*VAD_ISR_CB) (uint32_t int_status);


/**
 * This function initializes the VAD (Voice Activity Detection) module with the provided configuration.
 * 
 * @param base_addr The base address of the VAD hardware module.
 * @param vad_cfg A pointer to a structure containing the configuration settings for the VAD (Voice
 * Activity Detection) module. The structure includes the following fields:
 * 
 * @return a VAD_ERROR_E enumeration value, which could be VAD_NO_ERROR if the initialization is
 * successful.
 */
VAD_ERROR_E hx_drv_vad_init(uint32_t base_addr, VAD_CFG_T *vad_cfg);


/**
 * This function deinitializes the VAD module and disables its functionality.
 * 
 * @param vad_cfg A pointer to a VAD_CFG_T structure that contains the configuration settings for the
 * VAD (Voice Activity Detection) module. This structure typically includes parameters such as the
 * sampling rate, frame size, and detection threshold.
 * 
 * @return a VAD_ERROR_E enumeration value, specifically VAD_NO_ERROR.
 */
VAD_ERROR_E hx_drv_vad_deinit();


/**
 * The function sets default values for a VAD configuration structure and copies it to the provided
 * structure.
 * 
 * @param vad_cfg A pointer to a VAD_CFG_T structure that will be filled with default VAD configuration
 * values.
 */
VAD_ERROR_E hx_drv_vad_get_defCfg(VAD_CFG_T *vad_cfg);


/**
 * This function sets the enable status of the VAD (Voice Activity Detection) module and clears any
 * interrupts.
 * 
 * @param enable A boolean value indicating whether to enable or disable the VAD (Voice Activity
 * Detection) feature. If set to 1, the VAD will be enabled, and if set to 0, it will be disabled.
 * 
 * @return a VAD_ERROR_E enum value, which in this case is VAD_NO_ERROR.
 */
VAD_ERROR_E hx_drv_vad_set_enable(uint8_t enable);


/**
 * This function sets the gain values for input, speech, and noise in a VAD system.
 * 
 * @param input_gain The gain value for the input signal to the VAD (Voice Activity Detection) module.
 * This parameter controls the amplification of the input signal before it is processed by the VAD
 * algorithm.
 * @param speech_gain The speech gain parameter is used to set the gain level for speech signals in the
 * VAD (Voice Activity Detection) module. 
 * @param noise_gain The gain value for the noise signal in the VAD (Voice Activity Detection)
 * algorithm. It determines how much the noise signal is amplified before being processed by the VAD
 * algorithm.
 * 
 * @return a VAD_ERROR_E enum value, specifically VAD_NO_ERROR.
 */
VAD_ERROR_E hx_drv_vad_set_gain(uint8_t input_gain, uint8_t speech_gain, uint8_t noise_gain);


/**
 * The function sets the datapath for the VAD module based on the specified high pass filter, lookup
 * table, and inverse filter.
 * 
 * @param high_pass This parameter selects the type of high-pass filter to be used in the VAD datapath.
 * It is of type VAD_HIGH_PASS_FILTER_SEL_E, which is an enumeration of possible filter types.
 * @param lut VAD_LUT_SEL_E is an enumeration type that specifies the type of lookup table to be used
 * in the VAD datapath. It can take one of the following values:
 * @param inverse VAD_INVERSE_FILTER_SEL_E is an enumeration type that specifies the type of inverse
 * filter to be used in the VAD datapath. It can take one of the following values:
 * 
 * @return a VAD_ERROR_E enumeration value. In this case, it is returning VAD_NO_ERROR.
 */
VAD_ERROR_E hx_drv_vad_set_datapath(VAD_HIGH_PASS_FILTER_SEL_E high_pass, VAD_LUT_SEL_E lut, VAD_INVERSE_FILTER_SEL_E inverse);


/**
 * This function sets the value of k_noise in the VAD register.
 * 
 * @param k_noise The parameter k_noise is a uint16_t variable that represents the noise threshold for
 * the Voice Activity Detection (VAD) algorithm. The VAD algorithm is used to detect the presence or
 * absence of human speech in an audio signal. The k_noise parameter is used to set the threshold level
 * 
 * @return The function `hx_drv_vad_set_k_noise` is returning a value of type `VAD_ERROR_E`. In this
 * case, it is returning `VAD_NO_ERROR`.
 */
VAD_ERROR_E hx_drv_vad_set_k_noise(uint16_t k_noise);


/**
 * This function registers a callback function for the VAD interrupt and enables or disables the
 * interrupt accordingly.
 * 
 * @param cb_func A function pointer to the user-defined callback function that will be called when a
 * VAD interrupt occurs.
 * 
 * @return a VAD_ERROR_E enumeration value.
 */
VAD_ERROR_E hx_drv_vad_register_cb(VAD_ISR_CB cb_func);

/** @} */ // end of VAD group
#endif /* INC_HX_DRV_VAD_H_ */
