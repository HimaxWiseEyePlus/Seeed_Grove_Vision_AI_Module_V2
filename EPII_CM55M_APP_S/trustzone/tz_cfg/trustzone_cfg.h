/*
 * trustzone_cfg.h
 *
 *  Created on: 2021�~7��24��
 *      Author: 902447
 */

#ifndef TRUSTZONE_TRUSTZONE_CFG_H_
#define TRUSTZONE_TRUSTZONE_CFG_H_

#include "WE2_device.h"

#if !defined(HX_TFM)
#ifdef TRUSTZONE
#ifndef TRUSTZONE_SEC_ONLY
#include "CM55M_TrustZone_FW_mem.h"
#endif
#endif
#endif

void TZ_Set_ALL_Secure();
void TZ_Set_Secure_ByCFG();
void TZ_Set_ITS_byIRQNo(IRQn_Type irq);
void TZ_Set_ITNS_byIRQNo(IRQn_Type irq);
void TZ_SAU_Setup(void);

#endif /* TRUSTZONE_TRUSTZONE_CFG_H_ */
