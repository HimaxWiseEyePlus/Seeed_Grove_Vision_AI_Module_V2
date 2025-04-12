/*
 * cvapp.h
 *
 *  Created on: 2018�~12��4��
 *      Author: 902452
 */

#ifndef APP_SCENARIO_ALLON_SENSOR_TFLM_CVAPP_
#define APP_SCENARIO_ALLON_SENSOR_TFLM_CVAPP_

#include "spi_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

int cv_init(bool security_enable, bool privilege_enable);

int cv_run();

int cv_deinit();
#ifdef __cplusplus
}
#endif

#endif /* APP_SCENARIO_ALLON_SENSOR_TFLM_CVAPP_ */
