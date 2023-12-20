/*
 * WE2_device.h
 *
 *  Created on: 2023�~5��15��
 *      Author: 902447
 */

#ifndef DEVICE_INC_WE2_DEVICE_H_
#define DEVICE_INC_WE2_DEVICE_H_

#if defined(_RTE_)
#include "RTE_Components.h"
#include "RTE_Device.h"
#else
#define CMSIS_device_header "WE2_ARMCM55.h"
#endif //#if defined(_RTE_)

#include "WE2_device_addr.h"
#include CMSIS_device_header


#endif /* DEVICE_INC_WE2_DEVICE_H_ */
