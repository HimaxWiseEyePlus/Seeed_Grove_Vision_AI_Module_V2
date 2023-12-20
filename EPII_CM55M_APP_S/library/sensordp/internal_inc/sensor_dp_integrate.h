/*
 * sensor_dp_integrate.h
 *
 *  Created on: 2018¦~11¤ë29¤é
 *      Author: 902447
 */

#ifndef INTEGRATE_CLI_SENSOR_DATAPATH_SENSORB_DP_INTEGRATE_H_
#define INTEGRATE_CLI_SENSOR_DATAPATH_SENSORB_DP_INTEGRATE_H_

/**
 * \brief	datapath INT1_Txx HW2x2 --> CDM --> WDMA1, HW5x5(YUV) --> WDMA3, HW5x5(YUV) --> JPEG --> WDMA2 setup
 *
 *
 * \param[in]	hw2x2_cfg	HW2x2 configuration
 * \param[in]	cdm_cfg	CDM configuration
 * \param[in]	hw5x5_cfg	HW5x5 configuration
 * \param[in]	jpeg_cfg	JPEG configuration
 * \param[in]	cyclic_buffer_cnt	cyclic buffer count configuration
 * \param[in]	wdma1_cb	 WDMA1 callback function
 * \param[in]	wdma2_cb	 WDMA2 callback function
 * \param[in]	wdma3_cb	 WDMA3 callback function
 * \param[in]	rdma_cb	 RDMA callback function
 * \param[in]	dp_cb	 DP callback function
 * \retval	void
 */
void setup_sensor_dp_INT1_HWACC(
		HW2x2_CFG_T hw2x2_cfg,
		CDM_CFG_T cdm_cfg,
		HW5x5_CFG_T hw5x5_cfg,
		JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		XDMAWDMA1_ISREvent_t wdma1_cb,
		XDMAWDMA2_ISREvent_t wdma2_cb,
	    XDMAWDMA3_ISREvent_t wdma3_cb,
		XDMARDMA_ISREvent_t rdma_cb,
		DP_ISREvent_t dp_cb);

/**
 * \brief	datapath INT1_Txx HW2x2 --> CDM --> WDMA1, HW5x5(YUV) --> WDMA3, HW5x5(YUV) --> JPEG --> WDMA2 setup
 *
 *
 * \param[in]	hw2x2_cfg	HW2x2 configuration
 * \param[in]	cdm_cfg	CDM configuration
 * \param[in]	hw5x5_cfg	HW5x5 configuration
 * \param[in]	jpeg_cfg	JPEG configuration
 * \param[in]	cyclic_buffer_cnt	cyclic buffer count configuration
 * \param[in]	cyclic_buffer_start_no	cyclic buffer start index
 * \param[in]	wdma1_cb	 WDMA1 callback function
 * \param[in]	wdma2_cb	 WDMA2 callback function
 * \param[in]	wdma3_cb	 WDMA3 callback function
 * \param[in]	rdma_cb	 RDMA callback function
 * \param[in]	dp_cb	 DP callback function
 * \retval	void
 */
void setup_sensor_dp_INT1_HWACC_start_no(
		HW2x2_CFG_T hw2x2_cfg,
		CDM_CFG_T cdm_cfg, HW5x5_CFG_T hw5x5_cfg,
		JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		uint8_t cyclic_buffer_start_no,
		XDMAWDMA1_ISREvent_t wdma1_cb,
		XDMAWDMA2_ISREvent_t wdma2_cb,
		XDMAWDMA3_ISREvent_t wdma3_cb,
		XDMARDMA_ISREvent_t rdma_cb,
		DP_ISREvent_t dp_cb);

/**
 * \brief	datapath INT3_Txx HW5x5(YUV) --> WDMA3, HW5x5(YUV) --> JPEG --> WDMA2 setup
 *
 *
 * \param[in]	hw5x5_cfg	HW5x5 configuration
 * \param[in]	jpeg_cfg	JPEG configuration
 * \param[in]	cyclic_buffer_cnt	cyclic buffer count configuration
 * \param[in]	cyclic_buffer_start_no	cyclic buffer start index
 * \param[in]	wdma2_cb	 WDMA2 callback function
 * \param[in]	wdma3_cb	 WDMA3 callback function
 * \retval	void
 */
void setup_dp_int_hw5x5jpeg_start_no(
		HW5x5_CFG_T hw5x5_cfg,
		JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		uint8_t cyclic_buffer_start_no,
		XDMAWDMA2_ISREvent_t wdma2_cb,
		XDMAWDMA3_ISREvent_t wdma3_cb);

/**
 * \brief   datapath INT1_Txx HW2x2 --> CDM --> WDMA1, HW5x5(RGB) --> WDMA3, HW5x5(RGB) --> HXRGB2YUV --> JPEG --> WDMA2 setup
 *
 *
 * \param[in]   hw2x2_cfg   HW2x2 configuration
 * \param[in]   cdm_cfg CDM configuration
 * \param[in]   hw5x5_cfg   HW5x5 configuration
 * \param[in]   jpeg_cfg    JPEG configuration
 * \param[in]   cyclic_buffer_cnt   cyclic buffer count configuration
 * \param[in]   wdma1_cb     WDMA1 callback function
 * \param[in]   wdma2_cb     WDMA2 callback function
 * \param[in]   wdma3_cb     WDMA3 callback function
 * \param[in]   rdma_cb  RDMA callback function
 * \param[in]   dp_cb    DP callback function
 * \retval  void
 */
void setup_sensor_dp_INT1RGB_HWACC(
        HW2x2_CFG_T hw2x2_cfg,
        CDM_CFG_T cdm_cfg,
        HW5x5_CFG_T hw5x5_cfg,
        JPEG_CFG_T jpeg_cfg,
        uint8_t cyclic_buffer_cnt,
        XDMAWDMA1_ISREvent_t wdma1_cb,
        XDMAWDMA2_ISREvent_t wdma2_cb,
        XDMAWDMA3_ISREvent_t wdma3_cb,
        XDMARDMA_ISREvent_t rdma_cb,
        DP_ISREvent_t dp_cb);

/**
 * \brief   datapath INT1_Txx HXCSC --> CDM --> WDMA1, HXCSC(RGB) --> WDMA3, HXCSC --> HXRGB2YUV --> JPEG --> WDMA2 setup
 *
 *
 * \param[in]   hxcsc_cfg   HXCSC configuration
 * \param[in]   cdm_cfg CDM configuration
 * \param[in]   jpeg_cfg    JPEG configuration
 * \param[in]   cyclic_buffer_cnt   cyclic buffer count configuration
 * \param[in]   wdma1_cb     WDMA1 callback function
 * \param[in]   wdma2_cb     WDMA2 callback function
 * \param[in]   wdma3_cb     WDMA3 callback function
 * \param[in]   rdma_cb  RDMA callback function
 * \param[in]   dp_cb    DP callback function
 * \retval  void
 */
void setup_sensor_dp_INT1CSC_HWACC(
		HXCSC_CFG_T hxcsc_cfg,
		CDM_CFG_T cdm_cfg,
		JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		XDMAWDMA1_ISREvent_t wdma1_cb,
		XDMAWDMA2_ISREvent_t wdma2_cb,
		XDMAWDMA3_ISREvent_t wdma3_cb,
		XDMARDMA_ISREvent_t rdma_cb,
		DP_ISREvent_t dp_cb);

#endif /* INTEGRATE_CLI_HM1245_DATAPATH_SENSORB_DP_INTEGRATE_H_ */
