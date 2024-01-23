/*
 * customer_seeed.h
 *
 *  Created on: 2023¦~12¤ë6¤é
 *      Author: 902447
 */

#ifndef CUSTOMER_SEEED_CUSTOMER_SEEED_H_
#define CUSTOMER_SEEED_CUSTOMER_SEEED_H_

#include <stdio.h>
#include <string.h>
#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"

/**
 * \defgroup CUSTOMER SEEED
 * \brief  CUSTOMER Specific API
 * \details CUSTOMER Specific API
 * <pre>
 *
 *
 * </pre>
 * @}
 */

#define OTP_USER_DATA_CNT	4
/**
 * \struct OTP_USER_DATA_T
 * \brief OTP User data
 */
typedef struct {
	uint32_t data[OTP_USER_DATA_CNT];
} OTP_USER_DATA_T;

/**
 * \brief	Set User Data to OTP
 *
 * \param[in]	user_data	user data to OTP
 * \return	void.
 */
void cust_seeed_set_otp_user_data(OTP_USER_DATA_T *user_data);

/**
 * \brief	Get User Data to OTP
 *
 * \param[out]	user_data	user data to OTP
 * \return	void.
 */
void cust_seeed_get_otp_user_data(OTP_USER_DATA_T *user_data);

#endif /* CUSTOMER_SEEED_CUSTOMER_SEEED_H_ */
