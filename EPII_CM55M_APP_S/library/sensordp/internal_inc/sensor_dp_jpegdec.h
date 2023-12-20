/*
 * sensor_dp_jpegdec.h
 *
 *  Created on: 2018¦~11¤ë29¤é
 *      Author: 902447
 */

#ifndef INTEGRATE_CLI_SENSOR_DATAPATH_SENSOR_DP_JPEGDEC_H_
#define INTEGRATE_CLI_SENSOR_DATAPATH_SENSOR_DP_JPEGDEC_H_


/**
 * \brief	datapath RDMA --> TPG --> JPEG Decode --> WDMA3 setup
 *
 *
 * \param[in]	jpegdec_cfg	JPEG Decoder configuration
 * \param[in]	in_width	width
 * \param[in]	in_height	height
 * \param[in]	frame_no	decode frame no(start from 0)
 * \param[in]	wdma3_cb	 WDMA3 callback function
 * \param[in]	rdma_cb	 RDMA callback function
 * \retval	void
 */
void setup_dp_jpegdec(JPEG_CFG_T jpegdec_cfg,
		uint16_t in_width, uint16_t in_height,
		uint32_t frame_no,
		XDMAWDMA3_ISREvent_t wdma3_cb,
		XDMARDMA_ISREvent_t rdma_cb);

/**
 * \brief	datapath RDMA --> TPG --> JPEG Decode --> WDMA3 setup
 *
 *
 * \param[in]	jpegdec_cfg	JPEG Decoder configuration
 * \param[in]	in_width	width
 * \param[in]	in_height	height
 * \param[in]	frame_no	decode frame no(start from 0)
 * \param[in]	wdma3_cb	 WDMA3 callback function
 * \param[in]	rdma_cb	 RDMA callback function
 * \retval	void
 */
void setup_dp_jpegdec_1stframe(JPEG_CFG_T jpegdec_cfg,
		uint16_t in_width, uint16_t in_height,
		XDMAWDMA3_ISREvent_t wdma3_cb,
		XDMARDMA_ISREvent_t rdma_cb);

#endif /* INTEGRATE_CLI_SENSOR_DATAPATH_SENSOR_DP_JPEGDEC_H_ */
