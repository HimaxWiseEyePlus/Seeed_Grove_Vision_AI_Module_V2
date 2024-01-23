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

int cv_yolov8n_ob_init(bool security_enable, bool privilege_enable, uint32_t model_addr);

int cv_yolov8n_ob_run(struct_yolov8_ob_algoResult *algoresult_yolov8n_ob);

int cv_yolov8n_ob_deinit();
#ifdef __cplusplus
}
#endif

#endif /* SCENARIO_TFLM_2IN1_FD_FL_PL_CVAPP_PL_ */
