/*
 * veneer_u55_ctrl.c
 *
 *  Created on: 2023/05/03
 *      Author: 904207
 */

#if !defined(HX_TFM)
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif

#include <stdio.h>
#include "stdint.h"
#include "arm_cmse.h"

#include "hx_drv_u55.h"
// #include "FaceDet_inference.h"
#include "tflm_dl_algo.h"
#include "hx_drv_scu.h"
#include "hx_drv_scu_export.h"
#include "xprintf.h"
#include "veneer_u55_ctrl.h"
#include "WE2_core.h"
#include "tflm_dl_algo_s_priv.h"

__attribute__((cmse_nonsecure_entry)) VENEER_U55_ERROR_E veneer_YOLO_FaceDetection_inference(int8_t* input_img, void* output_result, uint32_t img_width, uint32_t img_height)
{
    VENEER_U55_ERROR_E err = VENEER_U55_NO_ERROR;
    tflm_dl_algo_set_memery_trustzone(1);
    tflm_dl_algo_set_U55_trustzone(true);
    hx_drv_u55_init(true, true, HX_U55_BASE);

	err = (VENEER_U55_ERROR_E)YOLO_FaceDetection_inference(input_img, output_result, img_width, img_height);

    hx_drv_u55_deinit();
    tflm_dl_algo_set_U55_trustzone(false);
    return err;

}

__attribute__((cmse_nonsecure_entry)) VENEER_U55_ERROR_E veneer_YOLO_HumanDet_inference(int8_t* input_img, void* output_result, uint32_t img_width, uint32_t img_height)
{
    VENEER_U55_ERROR_E err = VENEER_U55_NO_ERROR;
    tflm_dl_algo_set_memery_trustzone(1);
    tflm_dl_algo_set_U55_trustzone(true);
    hx_drv_u55_init(true, true, HX_U55_BASE); 

    err = (VENEER_U55_ERROR_E)YOLO_HumanDet_inference(input_img, output_result, img_width, img_height);

    hx_drv_u55_deinit();
    tflm_dl_algo_set_U55_trustzone(false);
    return err;
}

__attribute__((cmse_nonsecure_entry)) VENEER_U55_ERROR_E veneer_TFLM_model_close()
{
    YOLO_FaceDetection_close();
    YOLO_HumanDet_close();
    tflm_dl_algo_set_memery_trustzone(0);
    return VENEER_U55_NO_ERROR;
}

#endif //#if !defined(HX_TFM)
