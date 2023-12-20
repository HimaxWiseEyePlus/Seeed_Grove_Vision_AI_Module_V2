#ifndef _AWB_H_
#define _AWB_H_

#include <stdint.h>
#include <stdbool.h>
#ifndef true
#define true	1
#endif

#ifndef false
#define false	0
#endif

/** only AWB_rgb support Helium optimize
 * if enable Helium optimize, image Width is must multiples of eight
 * and 	night mode can't be used
*/
#define AWB_HELIUM_OPTIMIZE 


/** only  AWB_rgb_v2 support
 * if define IMG_RGB_ORDER_FORWARD, 
 * color order is RGB
 *  --------------
 *  | red plane  |
 *  --------------
 *  | green plane|
 *  --------------
 *  | blue plane |
 *  --------------
 * else
 * color ordeer is BGR
 *  --------------
 *  | blue plane  |
 *  --------------
 *  | green plane|
 *  --------------
 *  | red plane |
 *  --------------
*/
// #define IMG_RGB_ORDER_FORWARD

#include "hx_drv_jpeg.h"
#ifdef __cplusplus
extern "C"
{
#endif
// Main functions
/** auto white balance
 * @brief calculates an average color temperature for the entire scene, and then uses this as the white balance
 * @property image format: YUV domain
 * 
 * @param[in] yuvBuffer input RGB image buffer   
 * @param[in] imgWidth  the width of input image 
 * @param[in] imgHeight the height of input image 
 * @param[in] jpeg_enc jpeg encode type (420/422)  
 * @param[out] outputBuffer output RGB image buffer     
 * @param[in] night_mode night mode detect result (1/0)
 * @param[in] CCM_EN (I) CCM function enable/disable (1/0) 
 * @param[in] GAMMA_EN GAMMA function enable/disable (1/0) 
 */
void AWB(
		uint8_t* yuvBuffer,        /* (I) input YUV image buffer              */
		uint16_t imgWidth,         /* (I) the width of input image            */
		uint16_t imgHeight,        /* (I) the height of input image           */
		JPEG_ENC_TYPE_T jpeg_enc,  /* (I) jpeg encode type (420/422)          */
		uint8_t* outputBuffer,     /* (O) output YUV image buffer             */
		uint8_t night_mode,        /* (I) night mode detect result (1/0)      */
		uint8_t CCM_EN,            /* (I) CCM function enable/disable (1/0)   */
		uint8_t GAMMA_EN);         /* (I) GAMMA function enable/disable (1/0) */



/** auto white balance
 * @brief calculates an average color temperature for the entire scene, and then uses this as the white balance
 * @property image format:
 *    For M(# Row) x N(#Column) RGB image, data are assumed to be stored in the following order.
 *      R_11 R_12....R_1N G_11 G_12...G_1N B_11 B_12...B_1N
 *      ......
 *      R_M1 R_M2...R_MN G_M1 B_M2...G_MN B_M1 B_M2...B_MN 
 * 
 * @param[in] rgbBuffer input RGB image buffer   
 * @param[in] imgWidth  the width of input image 
 * @param[in] imgHeight the height of input image 
 * @param[out] outputBuffer output RGB image buffer     
 * @param[in] night_mode night mode detect result(1/0)
 * @param[in] CCM_EN (I) CCM function enable/disable (1/0) 
 * @param[in] GAMMA_EN GAMMA function enable/disable (1/0) 
 */
extern int AWB_rgb_v1(
		uint8_t* rgbBuffer,        /* (I) input RGB image buffer              */
		uint16_t imgWidth,         /* (I) the width of input image            */
		uint16_t imgHeight,        /* (I) the height of input image           */
		uint8_t* outputBuffer,     /* (O) output RGB image buffer             */
		uint8_t night_mode,        /* (I) night mode detect result (1/0)      */
		uint8_t CCM_EN,            /* (I) CCM function enable/disable (1/0)   */
		uint8_t GAMMA_EN);         /* (I) GAMMA function enable/disable (1/0) */


/** auto white balance
 * @brief calculates an average color temperature for the entire scene, and then uses this as the white balance
 * @property input image format: data are assumed to be stored in the following order.
 *  --------------
 *  | red plane  |
 *  --------------
 *  | green plane|
 *  --------------
 *  | blue plane |
 *  --------------
 * @param[in] rgbBuffer input RGB image buffer   
 * @param[in] imgWidth  the width of input image 
 * @param[in] imgHeight the height of input image 
 * @param[out] outputBuffer output RGB image buffer     
 * @param[in] night_mode night mode detect result (1/0)
 * @param[in] CCM_EN (I) CCM function enable/disable (1/0) 
 * @param[in] GAMMA_EN GAMMA function enable/disable (1/0) 
 */
extern int AWB_rgb_v2(
		uint8_t* rgbBuffer,        /* (I) input RGB image buffer              */
		uint16_t imgWidth,         /* (I) the width of input image            */
		uint16_t imgHeight,        /* (I) the height of input image           */
		uint8_t* outputBuffer,     /* (O) output RGB image buffer             */
		uint8_t night_mode,        /* (I) night mode detect result (1/0)      */
		uint8_t CCM_EN,            /* (I) CCM function enable/disable (1/0)   */
		uint8_t GAMMA_EN);         /* (I) GAMMA function enable/disable (1/0) */		

extern void apply_gain_ccm_gamma_helium(uint8_t* rgbBuffer, uint16_t imgWidth, uint16_t imgHeight,
		uint16_t rGain, uint16_t gGain, uint16_t bGain, uint8_t* outputBuffer);

#ifdef __cplusplus
}
#endif
#endif
