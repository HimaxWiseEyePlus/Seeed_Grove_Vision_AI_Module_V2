/*
 * sensor_dp_common.h
 *
 *  Created on: 2018¦~11¤ë29¤é
 *      Author: 902447
 */

#ifndef INTEGRATE_CLI_SENSOR_DATAPATH_SENSOR_DP_COMMON_H_
#define INTEGRATE_CLI_SENSOR_DATAPATH_SENSOR_DP_COMMON_H_

#include "hx_drv_hw2x2.h"
#include "hx_drv_hw5x5.h"
#include "hx_drv_cdm.h"
#include "hx_drv_jpeg.h"
#include "hx_drv_dp.h"
#include "hx_drv_xdma.h"


#include "hx_drv_tpg.h"
#include "hx_drv_inp.h"
#include "hx_drv_sensorctrl.h"

void print_wdma1_int_status(XDMA_WDMA1STATUS_E event);
void print_wdma2_int_status(XDMA_WDMA2STATUS_E event);
void print_wdma3_int_status(XDMA_WDMA3STATUS_E event);
void print_rdma_int_status(XDMA_RDMASTATUS_E event);
//void print_hogdma_int_status(HOGDMA_INTSTATUS_E event);
//void print_rsdma_int_status(RSDMA_INTSTATUS_E event);
void print_dp_int_status(DP_INT_STATUS_E event);

//disable all DP Mux
void setup_dp_disable_Mux();
//disable all xDMA1, TPG
void setup_dp_disable_xDMA_TP();
//disable all DP Mux, xDMA, TPG, CDM
void setup_dp_disable_all_DP();

//datapath WDMA1, WDMA2, WDMA3, RDMA & TPG enable
void setup_dp_enblestream(uint8_t wdma1_en, uint8_t wdma2_en, uint8_t wdma3_en, uint8_t rdma_en, uint8_t tpg_en);
///datapath TPG case setup
void setup_dp_TPG_wo_enable(uint32_t width, uint32_t height, TPG_CASE_E tpg_case);


//setup xDMA1 WDMA1 configuration & RDMA for H/L and register CB for WDMA1
void setup_sensor_dp_cdm_xdma1(CDM_CFG_T cdm_cfg,
		XDMAWDMA1_ISREvent_t  wdma1_cb,
		XDMARDMA_ISREvent_t  rdma_cb);

//setup xDMA1 WDMA1 configuration register CB for WDMA1
void setup_sensor_dp_hw2x2_xdma1(HW2x2_CFG_T hw2x2_cfg,
		uint32_t dp_wdma1_addr,
		XDMAWDMA1_ISREvent_t  wdma1_cb);


//setup xDMA1 WDMA2 configuration register CB for WDMA2
void setup_sensor_dp_jpegenc_xdma1(JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		XDMAWDMA2_ISREvent_t wdma2_cb);

//setup xDMA1 WDMA2 configuration register CB for WDMA2, with specify cyclic buffer start number.
void setup_sensor_dp_jpegenc_xdma1_start_no(JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		uint8_t cyclic_buffer_start_no,
		XDMAWDMA2_ISREvent_t wdma2_cb);

//setup xDMA1 WDMA3 configuration register CB for WDMA3
void setup_sensor_dp_hw5x5_xdma1(HW5x5_CFG_T hw55_cfg
		,XDMAWDMA3_ISREvent_t wdma3_cb);

// datapath TPG case setup
void setup_sensordp_TPG_wo_enable(uint8_t time_auto_cal, uint16_t in_width, uint16_t in_height, TPG_CASE_E tpg_case, TPG_TIMING_T timing);

void setup_sensordp_rdma_ch1_wo_enable(uint32_t rdma_ch1_startaddr,
		uint32_t rdma_ch1_size,
		XDMARDMA_ISREvent_t rdma_cb);

#endif /* INTEGRATE_CLI_SENSOR_DATAPATH_SENSOR_DP_COMMON_H_ */
