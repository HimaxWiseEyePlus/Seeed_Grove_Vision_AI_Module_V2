/*
 * hm1245_dp_u3.h
 *
 *  Created on: 2018¦~11¤ë29¤é
 *      Author: 902447
 */

#ifndef INTEGRATE_CLI_HM1245_DATAPATH_SENSOR_DP_HW5X5DEMOS_WDMA3_H_
#define INTEGRATE_CLI_HM1245_DATAPATH_SENSOR_DP_HW5X5DEMOS_WDMA3_H_

/**
 * \brief	datapath HW5x5 setup
 *
 *
 * \param[in]	hw5x5_cfg	HW5x5 configuration
 * \param[in]	wdma3_cb	 WDMA3 callback function
 * \retval	void
 */
void setup_dp_hw5x5_wdma3(
		HW5x5_CFG_T hw5x5_cfg
		, XDMAWDMA3_ISREvent_t wdma3_cb);

/**
 * \brief	datapath HXCSC setup
 *
 * \param[in]   width		 INPUT width for HWACC
 * \param[in]   height		 INPUT height for HWACC
 * \param[in]	hxcsc_cfg	 HXCSC configuration
 * \param[in]	wdma3_cb	 WDMA3 callback function
 * \retval	void
 */
void setup_dp_hxcscYUV_wdma3(
		uint16_t width,
		uint16_t height,
		HXCSC_CFG_T hxcsc_cfg,
		XDMAWDMA3_ISREvent_t wdma3_cb);

/**
 * \brief	datapath HXCSC RGB setup
 *
 * \param[in]   width		 INPUT width for HWACC
 * \param[in]   height		 INPUT height for HWACC
 * \param[in]	hxcsc_cfg	 HXCSC configuration
 * \param[in]	wdma3_cb	 WDMA3 callback function
 * \retval	void
 */
void setup_dp_hxcsc_wdma3(
		uint16_t width,
		uint16_t height,
		HXCSC_CFG_T hxcsc_cfg,
		XDMAWDMA3_ISREvent_t wdma3_cb);
#endif /* INTEGRATE_CLI_HM1245_DATAPATH_SENSOR_DP_HW5X5DEMOS_WDMA3_H_ */
