/*
 * cvapp.h
 *
 *  Created on: 2018�~12��4��
 *      Author: 902452
 */

#ifndef SCENARIO_TFLM_2IN1_FD_FL_PL_CVAPP_PL_
#define SCENARIO_TFLM_2IN1_FD_FL_PL_CVAPP_PL_

#include "spi_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

int cv_yolov8_gender_cls_init(bool security_enable, bool privilege_enable, uint32_t fd_model_addr, uint32_t gender_cls_model_addr );

int cv_yolov8_gender_cls_run(struct_yolov8_gender_cls_algoResult *algoresult_yolov8_gender_cls);

int cv_yolov8_gender_cls_deinit();
#ifdef __cplusplus
}
#endif

#endif /* SCENARIO_TFLM_2IN1_FD_FL_PL_CVAPP_PL_ */
