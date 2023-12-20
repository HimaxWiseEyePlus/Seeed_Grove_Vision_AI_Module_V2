/*
 * tpgdp_lib.h
 *
 *  Created on: 2018¦~12¤ë22¤é
 *      Author: 902447
 */

#ifndef LIBRARY_TPGDP_TPGDP_LIB_H_
#define LIBRARY_TPGDP_TPGDP_LIB_H_

//#include "hx_drv_ckgen.h"
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
#include <tpgdp_common.h>


//disable all xDMA1, TPG
void setup_tpgdp_disable_xDMA_TP();
//disable all DP Mux, xDMA, TPG, CDM
void setup_tpgdp_disable_all_DP();

void setup_tpgdp_enblestream(uint8_t wdma1_en, uint8_t wdma2_en, uint8_t wdma3_en, uint8_t rdma_en, uint8_t tpg_en);


//export API datapath TPG, HW2x2 setup
void setup_tpgdp_HW2x2_wdma1(
		HW2x2_CFG_T hw2x2_cfg,
		TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg,
		TPGDPLIB_WDMA1_CFG_T tpglib_wdma1_cfg,
		TPGDPLIB_RDMA_CFG_T tpglib_rdma_cfg,
		XDMAWDMA1_ISREvent_t wdma1_cb,
		XDMARDMA_ISREvent_t rdma_cb);

//export API datapath TPG CDM setup
void setup_tpgdp_CDM(CDM_CFG_T cdm_cfg,
		TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg,
		TPGDPLIB_WDMA1_CFG_T tpglib_wdma1_cfg,
		TPGDPLIB_RDMA_CFG_T tpglib_rdma_cfg,
		XDMAWDMA1_ISREvent_t wdma1_cb,
		XDMARDMA_ISREvent_t rdma_cb,
		DP_ISREvent_t dp_cb);

//export API datapath TPG HW2x2 and CDM setup
void setup_tpgdp_HW2x2_CDM(
		HW2x2_CFG_T hw2x2_cfg, CDM_CFG_T cdm_cfg,
		TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg,
		TPGDPLIB_WDMA1_CFG_T tpglib_wdma1_cfg,
		TPGDPLIB_RDMA_CFG_T tpglib_rdma_cfg
		, XDMAWDMA1_ISREvent_t wdma1_cb
		, XDMARDMA_ISREvent_t rdma_cb
		, DP_ISREvent_t dp_cb);

//export API datapath TPG and HW5x5 setup
void setup_tpgdp_hw5x5_wdma3(
		HW5x5_CFG_T hw5x5_cfg,
		TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg,
		TPGDPLIB_WDMA3_CFG_T tpglib_wdma3_cfg,
		TPGDPLIB_RDMA_CFG_T tpglib_rdma_cfg,
		XDMAWDMA3_ISREvent_t wdma3_cb,
		XDMARDMA_ISREvent_t rdma_cb);

//export API datapath TPG and JPEG Enc setup
void setup_tpgdp_jpegenc_wdma2(
		JPEG_CFG_T jpeg_cfg
		,TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg
		,TPGDPLIB_WDMA2_CFG_T tpglib_wdma2_cfg
		,TPGDPLIB_RDMA_CFG_T tpglib_rdma_cfg
		,XDMAWDMA2_ISREvent_t wdma2_cb,
		XDMARDMA_ISREvent_t rdma_cb);

//export API datapath JPEG & RDMA & WDMA3 setup
void setup_tpgdp_jpegdec(JPEG_CFG_T jpegdec_cfg,
		TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg,
		TPGDPLIB_WDMA3_CFG_T tpglib_wdma3_cfg,
		TPGDPLIB_RDMA_CFG_T tpglib_rdma_cfg,
		XDMAWDMA3_ISREvent_t wdma3_cb,
		XDMARDMA_ISREvent_t rdma_cb);

//export API datapath JPEG & WDMA2 setup
void setup_tpgdp_hw5x5_jpeg_wdma2(
		HW5x5_CFG_T hw5x5_cfg
		, JPEG_CFG_T jpeg_cfg
		,TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg
		,TPGDPLIB_WDMA2_CFG_T tpglib_wdma2_cfg
		,TPGDPLIB_RDMA_CFG_T tpglib_rdma_cfg
		,XDMAWDMA2_ISREvent_t wdma2_cb,
		XDMARDMA_ISREvent_t rdma_cb);

//export API datapath TPG and WDMA2 setup with out start
/*
 * tpg_case must be TPG_CASE_TOINP_1CHDATA_FROM_IPG_PULSE or TPG_CASE_TO_INP_FROM_IPG_CONVENTION
 */
void setup_tpgdp_bypass_wdma2_wiinp_ipg(
		TPG_CASE_E tpg_case,
		INP_SUBSAMPLE_E subsample,
		TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg
		,TPGDPLIB_WDMA2_CFG_T tpglib_wdma2_cfg
		,XDMAWDMA2_ISREvent_t wdma2_cb);

//export API datapath TPG and WDMA2 setup with out start
/*
 * tpg_case must be TPG_CASE_NOINP_1CHDATA_FROM_IPG
 */
void setup_tpgdp_bypass_wdma2_woinp_ipg(
		TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg
		,TPGDPLIB_WDMA2_CFG_T tpglib_wdma2_cfg
		,XDMAWDMA2_ISREvent_t wdma2_cb);

//export API datapath TPG and WDMA2 setup with out start
/*
 * tpg_case must be TPG_CASE_NOINP_1CHDATA_FROM_RDMA
 */
void setup_tpgdp_bypass_wdma2_woinp_rdma(
		TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg
		,TPGDPLIB_WDMA2_CFG_T tpglib_wdma2_cfg
		,TPGDPLIB_RDMA_CFG_T tpglib_rdma_cfg
		,XDMAWDMA2_ISREvent_t wdma2_cb,
		XDMARDMA_ISREvent_t rdma_cb);

#if 0 //RD Said INP not support discontinous
//export API datapath TPG and WDMA2 setup with out start
/*
 * tpg_case must be TPG_CASE_TO_INP_FROM_RDMA_CONVENTION
 * or TPG_CASE_TOINP_1CHDATA_FROM_RDMA_PULSE
 */
void setup_tpgdp_bypass_wdma2_wiinp_rdma(
		TPG_CASE_E tpg_case,
		INP_SUBSAMPLE_E subsample,
		TPGDPLIB_TPG_TIMING_CFG_T tpg_cfg
		,TPGDPLIB_WDMA2_CFG_T tpglib_wdma2_cfg
		,TPGDPLIB_RDMA_CFG_T tpglib_rdma_cfg
		,XDMAWDMA2_ISREvent_t wdma2_cb,
		XDMARDMA_ISREvent_t rdma_cb);
#endif
#endif /* LIBRARY_TPGDP_TPGDP_LIB_H_ */
