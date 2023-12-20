/*
 * sensor_dp_hw2x2.h
 *
 *  Created on: 2018¦~12¤ë10¤é
 *      Author: 902447
 */

#ifndef INTEGRATE_CLI_HM1245_DATAPATH_SENSOR_DP_HW2X2_H_
#define INTEGRATE_CLI_HM1245_DATAPATH_SENSOR_DP_HW2X2_H_

/**
 * \brief	datapath U2_Txx CDM setup
 *
 *
 * \param[in]	hw2x2_cfg	HW2x2 configuration
 * \param[in]	wdma1_cb	 WDMA1 callback function
 * \retval	void
 */
void setup_dp_HW2x2_wdma1(
		HW2x2_CFG_T hw2x2_cfg
		, XDMAWDMA1_ISREvent_t wdma1_cb);

#endif /* INTEGRATE_CLI_HM1245_DATAPATH_SENSOR_DP_HW2X2_H_ */
