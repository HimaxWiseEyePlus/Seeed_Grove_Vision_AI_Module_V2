/*
 * tpg_common.h
 *
 *  Created on: 2018¦~12¤ë22¤é
 *      Author: 902447
 */

#ifndef LIBRARY_TPGDP_TPGDP_COMMON_H_
#define LIBRARY_TPGDP_TPGDP_COMMON_H_

#include "hx_drv_timer.h"
#ifdef IP_timer
#include "timer_interface.h"
#endif
#include "hx_drv_hw2x2.h"
#include "hx_drv_hw5x5.h"
#include "hx_drv_cdm.h"
#include "hx_drv_jpeg.h"
#include "hx_drv_dp.h"
#include "hx_drv_xdma.h"
#include "hx_drv_tpg.h"
#include "hx_drv_inp.h"
#include "hx_drv_sensorctrl.h"
#include <tpglib_structure.h>

//disable all DP Mux
void setup_tpgdp_disable_Mux();


///datapath TPG case setup
void setup_tpgdp_TPG_wo_enable(TPGDPLIB_TPG_TIMING_CFG_T tpg_timing, TPG_CASE_E tpg_case);

//setup RDMA
void setup_tpgdp_rdma_wo_enable(TPGDPLIB_RDMA_CFG_T tpglib_rdma_cfg,
				XDMARDMA_ISREvent_t rdma_cb);


//setup xDMA1 WDMA1 configuration register CB for WDMA1
void setup_tpgdp_hw2x2_xdma1(HW2x2_CFG_T hw2x2_cfg,
		TPGDPLIB_WDMA1_CFG_T tpglib_wdma1_cfg,
		XDMAWDMA1_ISREvent_t  wdma1_cb);

//setup xDMA1 WDMA1 configuration register CB for WDMA1
void setup_tpgdp_cdm_xdma1(CDM_CFG_T cdm_cfg,
		TPGDPLIB_WDMA1_CFG_T tpglib_wdma1_cfg,
		XDMAWDMA1_ISREvent_t  wdma1_cb);

//setup xDMA1 WDMA3 configuration register CB for WDMA3
void setup_tpgdp_hw5x5_xdma1(HW5x5_CFG_T hw5x5_cfg,
		TPGDPLIB_WDMA3_CFG_T tpglib_wdma3_cfg,
		XDMAWDMA3_ISREvent_t wdma3_cb);

//Setup xDMA1 WDMA2 configuration register CB for WDMA2
void setup_tpgdp_wdma2_xdma1(
		TPGDPLIB_WDMA2_CFG_T tpglib_wdma2_cfg,
		XDMAWDMA2_ISREvent_t wdma2_cb);

#endif /* LIBRARY_TPGDP_TPGDP_COMMON_H_ */
