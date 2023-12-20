/*
 * sensor_dp_raw_wdma2.h
 *
 *  Created on: 2018¦~11¤ë29¤é
 *      Author: 902447
 */

#ifndef INTEGRATE_CLI_SENSOR_DATAPATH_SENSOR_DP_WDMA2_H_
#define INTEGRATE_CLI_SENSOR_DATAPATH_SENSOR_DP_WDMA2_H_

/**
 * \brief	datapath INP --> WDMA2 setup
 *
 *
 * \param[in]	width	width
 * \param[in]	height	Height
 * \param[in]	wdma2_cb	 WDMA2 callback function
 * \retval	void
 */
void setup_dp_raw_wdma2(
		uint16_t width, uint16_t height
		, XDMAWDMA2_ISREvent_t wdma2_cb);

/**
 * \brief	datapath INP --> WDMA2 setup
 *
 *
 * \param[in]	width	width
 * \param[in]	height	Height
 * \param[in]	cyclic_buffer_cnt	cyclic buffer count
 * \param[in]	cyclic_buffer_start_no: Specific the cyclic buffer index to start putting images.
 *										Use hx_drv_xdma_get_WDMA2NextFrameIdx to get the start index
 *										before re-starting the datapath.
 * \param[in]	wdma2_cb	 WDMA2 callback function
 * \retval	void
 */
void setup_dp_raw_wdma2_start_no(
		uint16_t width, uint16_t height
		, uint8_t cyclic_buffer_cnt
		, uint8_t cyclic_buffer_start_no
		, XDMAWDMA2_ISREvent_t wdma2_cb);

#endif /* INTEGRATE_CLI_HM1245_DATAPATH_SENSOR_DP_WDMA2_H_ */
