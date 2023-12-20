/*
 * sensor_dp_cdm.h
 *
 *  Created on: 2018¦~12¤ë10¤é
 *      Author: 902447
 */

#ifndef INTEGRATE_CLI_HM1245_DATAPATH_SENSOR_DP_CDM_H_
#define INTEGRATE_CLI_HM1245_DATAPATH_SENSOR_DP_CDM_H_
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief	datapath D2_Txx CDM setup
 *
 * \param[in]	cdm_cfg	CDM configuration
 * \param[in]	wdma1_cb	WDMA1 callback function
 * \param[in]	rdma_cb	 RDMA callback function
 * \param[in] 	dp_cb	sensor frame len for INP
 * \retval	void
 */
void setup_dp_CDM(CDM_CFG_T cdm_cfg
		, XDMAWDMA1_ISREvent_t wdma1_cb
		, XDMARDMA_ISREvent_t rdma_cb
		, DP_ISREvent_t dp_cb);

#ifdef __cplusplus
}
#endif
#endif /* INTEGRATE_CLI_HM1245_DATAPATH_SENSOR_DP_CDM_H_ */
