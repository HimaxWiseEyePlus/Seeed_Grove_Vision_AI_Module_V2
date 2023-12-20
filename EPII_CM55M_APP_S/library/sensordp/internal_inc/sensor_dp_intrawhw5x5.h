/*
 * sensor_dp_intrawhw5x5.h
 *
 *  Created on: 2018¦~12¤ë17¤é
 *      Author: 902447
 */

#ifndef INTEGRATE_CLI_HM1245_DATAPATH_DPLIB_SENSOR_DP_INTRAWHW5X5_H_
#define INTEGRATE_CLI_HM1245_DATAPATH_DPLIB_SENSOR_DP_INTRAWHW5X5_H_

/**
 * \brief	datapath INP --> WDMA2, INP --> HW5x5 --> WDMA3 setup
 *
 *
 * \param[in]	width	Width
 * \param[in]	height	Height
 * \param[in]	hw5x5_cfg	HW5x5 configuration
 * \param[in]	wdma2_cb	 WDMA2 callback function
 * \param[in]	wdma3_cb	 WDMA3 callback function
 * \retval	void
 */
void setup_dp_int_raw_hw5x5_wdma23(
		uint16_t width, uint16_t height,
		HW5x5_CFG_T hw5x5_cfg
		, XDMAWDMA2_ISREvent_t wdma2_cb
		, XDMAWDMA3_ISREvent_t wdma3_cb);

/**
 * \brief	datapath INP --> WDMA2, INP --> HXCSC --> WDMA3 setup
 *
 *
 * \param[in]	width	Width
 * \param[in]	height	Height
 * \param[in]	hxcsc_cfg	HXCSC configuration
 * \param[in]	wdma2_cb	 WDMA2 callback function
 * \param[in]	wdma3_cb	 WDMA3 callback function
 * \retval	void
 */
void setup_dp_int_raw_hxcsc_wdma23(
		uint16_t width, uint16_t height,
		HXCSC_CFG_T hxcsc_cfg
		, XDMAWDMA2_ISREvent_t wdma2_cb
		, XDMAWDMA3_ISREvent_t wdma3_cb);

#endif /* INTEGRATE_CLI_HM1245_DATAPATH_DPLIB_SENSOR_DP_INTRAWHW5X5_H_ */
