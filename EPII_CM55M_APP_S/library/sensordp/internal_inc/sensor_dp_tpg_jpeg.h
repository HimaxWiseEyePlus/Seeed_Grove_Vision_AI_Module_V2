/*
 * sensor_dp_tpg_jpeg.h
 *
 *  Created on: 2019¦~6¤ë18¤é
 *      Author: 902447
 */

#ifndef LIBRARY_SENSORDP_INTERNAL_INC_SENSOR_DP_TPG_JPEG_H_
#define LIBRARY_SENSORDP_INTERNAL_INC_SENSOR_DP_TPG_JPEG_H_


/**
 * \brief	set source memory address --> RDMA --> TPG --> JPEG Enc --> WDMA --> Destination memory address Snapshot
 *
 *
 * \param[in]	jpeg_cfg	JPEG configuration
 * \param[in]	wdma2_startaddr	WDMA2 Start address
 * \param[in]	rdma_ch1_startaddr	 RDMA Channel 1 Start Address
 * \param[in]	rdma_ch2_startaddr	 RDMA Channel 2 Start Address
 * \param[in]	rdma_ch3_startaddr	 RDMA Channel 3 Start Address
 * \param[in]	wdma2_cb	 WDMA2 callback function
 * \param[in]	rdma_cb	 RDMA callback function
 * \retval	void
 */
void setup_sensordp_tpg_jpegenc_wdma2(
		JPEG_CFG_T jpeg_cfg
		,uint32_t wdma2_startaddr
		,uint32_t rdma_ch1_startaddr
		,uint32_t rdma_ch2_startaddr
		,uint32_t rdma_ch3_startaddr
		,XDMAWDMA2_ISREvent_t wdma2_cb
		, XDMARDMA_ISREvent_t rdma_cb);



#endif /* LIBRARY_SENSORDP_INTERNAL_INC_SENSOR_DP_TPG_JPEG_H_ */
