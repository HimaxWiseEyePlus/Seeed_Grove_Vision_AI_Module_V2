#ifndef _LIB_IMG_PROC_HELIUM_H_
#define _LIB_IMG_PROC_HELIUM_H_
#include <stdint.h>
#include "WE2_core.h"

// #define IMG_PROC_HELIUM_OPTIMIZE 1

#define LOCAL_FRAQ_BITS (8)
#define SC(A, B) ((A<<8)/B)
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef IMG_PROC_HELIUM_OPTIMIZE
void hx_lib_image_resize(uint8_t* im, uint8_t* resized, int input_w, int input_h, int channels, int output_w, int output_h);       
void hx_lib_crop_image(uint8_t* im, uint8_t* cropped, int input_w, int input_h, int input_c, int dx, int dy, int w, int h);
void hx_lib_pad_image(uint8_t* im, uint8_t* pad_img, int input_w, int input_h, int channels,  int pad_top, int pad_bottom , int pad_left, int pad_right, int output_pad_w, int output_pad_h);
void hx_lib_rotate_image(uint8_t* im, uint8_t* rotate_img, int input_w, int input_h, int channels, int output_w, int output_h,float rotate_angle);
void hx_lib_wrap_affine(uint8_t* im, uint8_t* affine_img,int input_w, int input_h, int channels, int output_w, int output_h, float T[]);
void hx_lib_image_rescale(
        const uint8_t*in_image,
        const int32_t width,
        const int32_t height,
        const int32_t nwidth,
        const int32_t nheight,
        uint8_t*out_image,
        const int32_t nxfactor,
        const int32_t nyfactor);
#endif        

/**
 * @brief Scale an input image to the desired dimension. using Helium optimize.
 * It's better to use this function to zoom out image.
 * @limit: The weight of resized image must be multiples of 4.
 * @suggest: 
 *          float w_scale = (float)(input_w - 1) / (output_w - 1);
 *          float h_scale = (float)(input_h - 1) / (output_h - 1);
 * 
 * @param[in] im input image address
 * @param[out] resized  output image address
 * @param[in] input_w  the weight of input image
 * @param[in] input_h  the hight of input image
 * @param[in] channels the channels of input image
 * @param[in] output_w  the weight of output image
 * @param[in] output_h the hight of output image
 * @param[in] w_scale scale factor along the horizontal axis; must be non-zero
 * @param[in] h_scale scale factor along the vertical axis; must be non-zero
 */
void hx_lib_image_resize_helium(uint8_t* im, uint8_t* resized, int input_w, int input_h, int channels, int output_w, int output_h, float w_scale, float h_scale);

/**
 * @brief Scale an input image to the desired dimension. using Helium optimize.
 * It's better to use this function to zoom out image.
 * Input image format: [[BBB...],[GGG...],[BBB...]]
 * Output image format: [[rgb], [rgb], [rgb],...]
 * @limit: The weight of resized image must be multiples of 4.
 * @suggest: 
 *          float w_scale = (float)(input_w - 1) / (output_w - 1);
 *          float h_scale = (float)(input_h - 1) / (output_h - 1);
 * 
 * @param[in] im input image address
 * @param[out] resized  output image address
 * @param[in] input_w  the weight of input image
 * @param[in] input_h  the hight of input image
 * @param[in] channels the channels of input image
 * @param[in] output_w  the weight of output image
 * @param[in] output_h the hight of output image
 * @param[in] w_scale scale factor along the horizontal axis; must be non-zero
 * @param[in] h_scale scale factor along the vertical axis; must be non-zero
 */
void hx_lib_image_resize_BGR8U3C_to_RGB24_helium(uint8_t* im, uint8_t* resized, 
                                        int input_w, int input_h, int channels, 
                                        int output_w, int output_h, float w_scale, float h_scale);

/**
 * @brief Extract a region from a given image. using Helium optimize.
 * 
 * @param[in] im input image address
 * @param[out] cropped Cropped image address
 * @param[in] input_w  the weight of input image
 * @param[in] input_h  the hight of input image
 * @param[in] input_c the channels of input image
 * @param[in] dx The x-coordinate, in logical units of the crop offset.
 * @param[in] dy The y-coordinate, in logical units of the crop offset.
 * @param[in] w The width of the crop.
 * @param[in] h The height of the crop.
 */
void hx_lib_image_copy_helium(uint8_t* im, uint8_t* cropped, int input_w, int input_h, int input_c, int dx, int dy, int w, int h);

/**
 * @brief Scale an input image to the desired dimension. using Helium optimize.
 * This function is suitable for scale down, and the execution speed is fast.
 * 
 * @limit: The weight of resized image must be multiples of 4.
 *        only support 1 channel.
 * @suggest: 
 *          int32_t nxfactor = SC(input_w, output_w);
 *          int32_t nyfactor = SC(input_h, output_h);
 * 
 * @param[in] in_image input image address
 * @param[in] width  the weight of input image
 * @param[in] height  the hight of input image
 * @param[in] channels the channels of input image
 * @param[in] nheight  the weight of output image
 * @param[in] nheight the hight of output image
 * @param[out] out_image  output image address
 * @param[in] nxfactor scale factor along the horizontal axis; must be non-zero
 * @param[in] nyfactor scale factor along the vertical axis; must be non-zero
 */
void hx_lib_image_rescale_helium(
        const uint8_t*in_image,
        const int32_t width,
        const int32_t height,
        const int32_t nwidth,
        const int32_t nheight,
        uint8_t*out_image,
        const int32_t nxfactor,
        const int32_t nyfactor);

void hx_lib_image_rescale_color_helium(
        const uint8_t*in_image,
        const int32_t width,
        const int32_t height,
        const int32_t nwidth,
        const int32_t nheight,
        uint8_t*out_image,
        const int32_t nxfactor,
        const int32_t nyfactor);        
#ifdef __cplusplus
}
#endif

#endif /*_LIB_IMG_PROC_HELIUM_H_*/
