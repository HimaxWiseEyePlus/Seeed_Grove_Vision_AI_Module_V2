/*
 * cvapp_fd_fm.h
 *
 *  Created on: Feb 15, 2023
 *      Author: bigcat-himax
 */

#ifndef APP_SCENARIO_APP_TFLM_FD_FM_CVAPP_FD_FM_H_
#define APP_SCENARIO_APP_TFLM_FD_FM_CVAPP_FD_FM_H_

#include "spi_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

int cv_fd_fm_init(bool security_enable, bool privilege_enable, uint32_t fd_model_addr, uint32_t fm_model_addr, uint32_t il_model_addr);

int cv_fd_fm_run(struct_algoResult *alg_result, struct_fm_algoResult_with_fps *alg_fm_result);

int cv_fd_fm_deinit();

#ifdef __cplusplus
}
#endif

#endif /* APP_SCENARIO_APP_TFLM_FD_FM_CVAPP_FD_FM_H_ */
