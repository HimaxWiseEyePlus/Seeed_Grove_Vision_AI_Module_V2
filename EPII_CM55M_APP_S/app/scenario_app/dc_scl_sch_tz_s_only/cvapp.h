/*
 * cvapp.h
 *
 *  Created on: 2018�~12��4��
 *      Author: 902452
 */

#ifndef APP_SCENARIO_DC_SCL_SCH_TZ_S_ONLY_CVAPP_H_
#define APP_SCENARIO_DC_SCL_SCH_TZ_S_ONLY_CVAPP_H_

#ifdef __cplusplus
extern "C" {
#endif

int cvapp_init(bool security_enable, bool privilege_enable);

int cvapp_run(int *score);
#ifdef __cplusplus
}
#endif

#endif /* APP_SCENARIO_DC_SCL_SCH_TZ_S_ONLY_CVAPP_H_ */
