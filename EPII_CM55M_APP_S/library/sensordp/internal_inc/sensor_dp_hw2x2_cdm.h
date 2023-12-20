/*
 * sensor_dp_hw2x2_cdm.h
 *
 *  Created on: 2018¦~11¤ë29¤é
 *      Author: 902447
 */

#ifndef INTEGRATE_CLI_SENSOR_DATAPATH_SENSORB_DP_HW2x2_CDM_H_
#define INTEGRATE_CLI_SENSOR_DATAPATH_SENSORB_DP_HW2x2_CDM_H_

/**
 * \brief	datapath U2_Txx HW2x2 and CDM setup
 *
 *
 * \param[in]	hw2x2_cfg	HW2x2 configuration
 * \param[in]	cdm_cfg	CDM configuration
 * \param[in]	wdma1_cb	 WDMA1 callback function
 * \param[in] 	rdma_cb	RDMA callback function
 * \param[in] 	dp_cb	Datapath callback function
 * \retval	void
 */
void setup_dp_HW2x2_CDM(
		HW2x2_CFG_T hw2x2_cfg, CDM_CFG_T cdm_cfg
		, XDMAWDMA1_ISREvent_t wdma1_cb
		, XDMARDMA_ISREvent_t rdma_cb
		, DP_ISREvent_t dp_cb);

/**
 * \brief	datapath U9_Txx HXCSC and CDM setup
 *
 *
 * \param[in]	hxcsc_cfg	HXCSC configuration
 * \param[in]	cdm_cfg	CDM configuration
 * \param[in]	wdma1_cb	 WDMA1 callback function
 * \param[in] 	rdma_cb	RDMA callback function
 * \param[in] 	dp_cb	Datapath callback function
 * \retval	void
 */
void setup_dp_HXCSC_CDM(
		HXCSC_CFG_T hxcsc_cfg, CDM_CFG_T cdm_cfg
		, XDMAWDMA1_ISREvent_t wdma1_cb
		, XDMARDMA_ISREvent_t rdma_cb
		, DP_ISREvent_t dp_cb);

#endif /* INTEGRATE_CLI_SENSOR_DATAPATH_SENSORB_DP_HW2x2_CDM_H_ */
