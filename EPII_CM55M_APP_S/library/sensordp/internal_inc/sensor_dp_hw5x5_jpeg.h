/*
 * sensor_dp_u4.h
 *
 *  Created on: 2018¦~11¤ë29¤é
 *      Author: 902447
 */

#ifndef INTEGRATE_CLI_SENSOR_DATAPATH_SENSORB_DP_U4_H_
#define INTEGRATE_CLI_SENSOR_DATAPATH_SENSORB_DP_U4_H_

/**
 * \brief	datapath HW5x5 and JPEG Enc setup
 *
 *
 * \param[in]	hw5x5_cfg	HW5x5 configuration
 * \param[in]	jpeg_cfg	JPEG configuration
 * \param[in]	cyclic_buffer_cnt	cyclic buffer count
 * \param[in]	wdma2_cb	 WDMA2 callback function
 * \retval	void
 */
void setup_dp_hw5x5_jpeg_wdma2(
		HW5x5_CFG_T hw5x5_cfg
		, JPEG_CFG_T jpeg_cfg
		, uint8_t cyclic_buffer_cnt
		,XDMAWDMA2_ISREvent_t wdma2_cb);

/**
 * \brief	datapath HXCSC and JPEG Enc setup
 *
 *
 * \param[in]	hxcsc_cfg	HXCSC configuration
 * \param[in]	jpeg_cfg	JPEG configuration
 * \param[in]	cyclic_buffer_cnt	cyclic buffer count
 * \param[in]	wdma2_cb	 WDMA2 callback function
 * \retval	void
 */
void setup_dp_hxcsc_jpeg_wdma2(
		HXCSC_CFG_T hxcsc_cfg
		, JPEG_CFG_T jpeg_cfg
		, uint8_t cyclic_buffer_cnt
		,XDMAWDMA2_ISREvent_t wdma2_cb);

/**
 * \brief	datapath HW5x5 and JPEG Enc setup, with specify cyclic buffer start number.
 *
 *
 * \param[in]	hw5x5_cfg	HW5x5 configuration
 * \param[in]	jpeg_cfg	JPEG configuration
 * \param[in]	cyclic_buffer_cnt	cyclic buffer count
 * \param[in]	cyclic_buffer_start_no: Specific the cyclic buffer index to start putting images.
 *										Use hx_drv_xdma_get_WDMA2NextFrameIdx to get the start index
 *										before re-starting the datapath.
 * \param[in]	wdma2_cb	 WDMA2 callback function
 * \retval	void
 */
void setup_dp_hw5x5_jpeg_wdma2_start_no(
		HW5x5_CFG_T hw5x5_cfg
		, JPEG_CFG_T jpeg_cfg
		, uint8_t cyclic_buffer_cnt
		, uint8_t cyclic_buffer_start_no
		,XDMAWDMA2_ISREvent_t wdma2_cb);

#endif /* INTEGRATE_CLI_SENSOR_DATAPATH_SENSORB_DP_U4_H_ */
