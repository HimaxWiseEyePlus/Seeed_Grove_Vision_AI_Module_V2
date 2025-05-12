/*
 * cvapp.h
 *
 *  Created on: 2018�~12��4��
 *      Author: 902452
 */

#ifndef APP_WW130_CLI_CVAPP_
#define APP_WW130_CLI_CVAPP_

#include "spi_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

int cv_init(bool security_enable, bool privilege_enable, uint32_t model_addr);

int cv_run();

int cv_deinit();
#ifdef __cplusplus
}
#endif

#endif /* APP_WW130_CLI_CVAPP_ */
