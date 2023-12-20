/*
 * sensor_dp_int_hw2x2_hw5x5jpeg.h
 *
 *  Created on: 2019¦~2¤ë24¤é
 *      Author: 902447
 */

#ifndef LIBRARY_SENSORDP_SENSOR_DP_INT_HW2x2_HW5X5JPEG_H_
#define LIBRARY_SENSORDP_SENSOR_DP_INT_HW2x2_HW5X5JPEG_H_

/**
 * \brief	datapath  setup
 * \Sensor --> INP --> HW2x2 --> WDMA1
 * \Sensor --> INP --> HW5x5 --> JPEG Enc --> WDMA2
 *
 *
 * \param[in]	hw2x2_cfg	HW2x2 configuration
 * \param[in]	hw5x5_cfg	HW5x5 configuration
 * \param[in]	jpeg_cfg	JPEG configuration
 * \param[in]	cyclic_buffer_cnt	cyclic buffer count configuration
 * \param[in]	wdma1_cb	 WDMA1 callback function
 * \param[in]	wdma2_cb	 WDMA2 callback function
 * \retval	void
 */

void 	setup_dp_int_hw2x2_hw5x5jpeg(
		HW2x2_CFG_T hw2x2_cfg,
		HW5x5_CFG_T hw5x5_cfg,
		JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		XDMAWDMA1_ISREvent_t wdma1_cb,
	    XDMAWDMA2_ISREvent_t wdma2_cb);

#endif /* LIBRARY_SENSORDP_SENSOR_DP_INT_HW2x2_HW5X5JPEG_H_ */
