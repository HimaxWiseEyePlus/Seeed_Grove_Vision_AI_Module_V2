/*
 * sensor_dp_intnojpeg.h
 *
 *  Created on: 2018¦~12¤ë12¤é
 *      Author: 902447
 */

#ifndef INTEGRATE_CLI_HM1245_DATAPATH_SENSOR_DP_INTNOJPEG_H_
#define INTEGRATE_CLI_HM1245_DATAPATH_SENSOR_DP_INTNOJPEG_H_

/**
 * \brief	datapath INTNoJPEG HW2x2 --> CDM --> WDMA1, HW5x5 --> WDMA3 setup
 *
 *
 * \param[in]	hw2x2_cfg	HW2x2 configuration
 * \param[in]	cdm_cfg	CDM configuration
 * \param[in]	hw5x5_cfg	HW5x5 configuration
 * \param[in]	wdma1_cb	 WDMA1 callback function
 * \param[in]	wdma3_cb	 WDMA3 callback function
 * \param[in]	rdma_cb	 RDMA callback function
 * \param[in]	dp_cb	 DP callback function
 * \retval	void
 */
void setup_sensor_dp_INTNoJPEG_HWACC(
		HW2x2_CFG_T hw2x2_cfg,
		CDM_CFG_T cdm_cfg,
		HW5x5_CFG_T hw5x5_cfg,
		XDMAWDMA1_ISREvent_t wdma1_cb,
	    XDMAWDMA3_ISREvent_t wdma3_cb,
		XDMARDMA_ISREvent_t rdma_cb,
		DP_ISREvent_t dp_cb);

/**
 * \brief	datapath INTCSCNoJPEG HXCSC --> CDM --> WDMA1, HXCSC --> WDMA3 setup
 *
 *
 * \param[in]	hxcsc_cfg	HXCSC configuration
 * \param[in]	cdm_cfg	CDM configuration
 * \param[in]	wdma1_cb	 WDMA1 callback function
 * \param[in]	wdma3_cb	 WDMA3 callback function
 * \param[in]	rdma_cb	 RDMA callback function
 * \param[in]	dp_cb	 DP callback function
 * \retval	void
 */
void setup_sensor_dp_INTCSCNoJPEG_HWACC(
		HXCSC_CFG_T hxcsc_cfg,
		CDM_CFG_T cdm_cfg,
		XDMAWDMA1_ISREvent_t wdma1_cb,
	    XDMAWDMA3_ISREvent_t wdma3_cb,
		XDMARDMA_ISREvent_t rdma_cb,
		DP_ISREvent_t dp_cb);

#endif /* INTEGRATE_CLI_HM1245_DATAPATH_SENSOR_DP_INTNOJPEG_H_ */
