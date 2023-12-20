/*
 * sensor_dp_tpg_hw2x2.h
 *
 *  Created on: 2019¦~7¤ë4¤é
 *      Author: 902447
 */

#ifndef LIBRARY_SENSORDP_INTERNAL_INC_SENSOR_DP_TPG_HW2X2_H_
#define LIBRARY_SENSORDP_INTERNAL_INC_SENSOR_DP_TPG_HW2X2_H_

void setup_sensordp_tpg_HW2x2_wdma1(
		uint16_t input_width,
		uint16_t input_height,
		HW2x2_CFG_T hw2x2_cfg,
		uint32_t wdma1_ch1_startaddr,
		uint32_t rdma_ch1_startaddr,
		XDMAWDMA1_ISREvent_t wdma1_cb,
		XDMARDMA_ISREvent_t rdma_cb);


#endif /* LIBRARY_SENSORDP_INTERNAL_INC_SENSOR_DP_TPG_HW2X2_H_ */
