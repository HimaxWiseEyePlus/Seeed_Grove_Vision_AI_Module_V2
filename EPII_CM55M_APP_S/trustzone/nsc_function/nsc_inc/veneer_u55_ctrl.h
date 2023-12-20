/*
 * u55_ctrl.h
 *
 *  Created on: 20230503
 *      Author: 904207
 */
#ifndef TRUSTZONE_NSC_FUNCTION_NSC_INC_VENEER_U55_CTRL_H_
#define TRUSTZONE_NSC_FUNCTION_NSC_INC_VENEER_U55_CTRL_H_
#include "stdbool.h"

/**
 * \enum U55_ERROR_E
 * \brief U55 Errors Type
 */
typedef enum {
	VENEER_U55_NO_ERROR = 0, /**< No ERROR */
	VENEER_U55_INVALID_PARAMETERS, /**< ERROR MSG: Invalid parameters */
	VENEER_U55_NOT_INITIAL, /**< ERROR MSG: U55 not initial yet */
	VENEER_U55_MODEL_VERSION_INCORRECT, /**< ERROR MSG: incorrect model version */
	VENEER_U55_OUTPUT_DATA_ERROR, /**< ERROR MSG: output data after invoke are incorrect*/
	VENEER_U55_ERROR, /**< ERROR MSG: ERROR happen */
} VENEER_U55_ERROR_E;

#ifdef __cplusplus
extern "C" {
#endif

VENEER_U55_ERROR_E veneer_YOLO_FaceDetection_inference(int8_t* input_img, void* output_result, uint32_t img_width, uint32_t img_height);

VENEER_U55_ERROR_E veneer_YOLO_HumanDet_inference(int8_t* input_img, void* output_result, uint32_t img_width, uint32_t img_height);

VENEER_U55_ERROR_E veneer_TFLM_model_close();

#ifdef __cplusplus
}
#endif
#endif /*TRUSTZONE_NSC_FUNCTION_NSC_INC_VENEER_U55_CTRL_H_*/