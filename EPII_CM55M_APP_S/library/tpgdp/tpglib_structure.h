/*
 * tpglib_structure.h
 *
 *  Created on: 2018¦~12¤ë22¤é
 *      Author: 902447
 */

#ifndef LIBRARY_TPGDP_TPGLIB_STRUCTURE_H_
#define LIBRARY_TPGDP_TPGLIB_STRUCTURE_H_

typedef struct TPGDPLIB_TPG_TIMING_CFG_S
{
	uint16_t tpg_width;
	uint16_t tpg_height;
	uint8_t tpg_HFP;
	uint8_t tpg_HSY;
	uint8_t tpg_HBP;
	uint8_t tpg_VFP;
	uint8_t tpg_VSY;
	uint8_t tpg_VBP;
	uint8_t blanking_autoset; // 1: auto set by lib, 0: by app setting
} TPGDPLIB_TPG_TIMING_CFG_T;

typedef struct TPGDPLIB_RDMA_CFG_S
{
	uint32_t rdma_ch1_startaddr;
	uint32_t rdma_ch1_size;
	uint32_t rdma_ch2_startaddr;
	uint32_t rdma_ch2_size;
	uint32_t rdma_ch3_startaddr;
	uint32_t rdma_ch3_size;
	XDMA_RDMA_FINSRC_SEL_E finmode;
} TPGDPLIB_RDMA_CFG_T;

typedef struct TPGDPLIB_WDMA1_CFG_S
{
	uint32_t wdma1_ch1_startaddr;
	uint32_t wdma1_ch2_startaddr;
	uint32_t wdma1_ch3_startaddr;
} TPGDPLIB_WDMA1_CFG_T;

typedef struct TPGDPLIB_WDMA2_CFG_S
{
	uint32_t wdma2_startaddr;
	uint32_t wdma2_offset;
	uint8_t wdma2_startcnt;
	uint8_t wdma2_tgtloop;
	XDMA_WDMA2_DMAMODE_E dma_mode;
	XDMA_WDMA2_SNAPSHOT_FINMODE_E fin_mode;
} TPGDPLIB_WDMA2_CFG_T;

typedef struct TPGDPLIB_WDMA3_CFG_S
{
	uint32_t wdma3_ch1_startaddr;
	uint32_t wdma3_ch2_startaddr;
	uint32_t wdma3_ch3_startaddr;
	XDMA_WDMA3_FINSRC_SEL_E finmode;
} TPGDPLIB_WDMA3_CFG_T;


#endif /* LIBRARY_TPGDP_TPGLIB_STRUCTURE_H_ */
