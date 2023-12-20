/*
 * hx_drv_edm.h
 *
 *  Created on: 2019¦~1¤ë13¤é
 *      Author: 902447
 */

#ifndef INC_HX_DRV_EDM_H_
#define INC_HX_DRV_EDM_H_
#include "dev_common.h"
/**
 * \defgroup	EDM_DRV	Error Detection Module Driver
 * \ingroup		EDM_DRV
 * \brief	Error Detection Module Driver Declaration
 */
/**
 * \defgroup	EDM_DRV_ENUM	Error Detection Module Driver Enumeration
 * \ingroup	EDM_DRV
 * \brief	Defines some enumeration of Error Detection Module Driver need.
 * @{
 */
/**
 * \enum EDM_CRC_MUX_CH0_E
 * \brief CRC Mux for Channel 0
 */
typedef enum
{
	EDM_CRC_MUX_CH0_INP_OUT			= 0,/**< Channel 0 CRC Mux INP out*/
	EDM_CRC_MUX_CH0_WDMA1_IN_CH0 	= 1,/**< Channel 0 CRC Mux WDMA1 channel 0*/
	EDM_CRC_MUX_CH0_WDMA2_IN 		= 2,/**< Channel 0 CRC Mux WDMA2*/
	EDM_CRC_MUX_CH0_WDMA3_IN_CH0	= 3,/**< Channel 0 CRC Mux WDMA3 channel 0*/
	EDM_CRC_MUX_CH0_RDMA_IN_CH0		= 4,/**< Channel 0 CRC Mux RDMA channel 0*/
	EDM_CRC_MUX_CH0_TPG_OUTPUT		= 5,/**< Channel 0 CRC Mux TPG output*/
}EDM_CRC_MUX_CH0_E;
/**
 * \enum EDM_CRC_MUX_CH1_E
 * \brief CRC Mux for Channel 1
 */
typedef enum
{
	EDM_CRC_MUX_CH1_WDMA1_IN_CH1 	= 1,/**< Channel 1 CRC Mux WDMA1 input*/
	EDM_CRC_MUX_CH1_WDMA3_IN_CH1	= 3,/**< Channel 1 CRC Mux WDMA3 input*/
	EDM_CRC_MUX_CH1_RDMA_IN_CH1		= 4,/**< Channel 1 CRC Mux RDMA output*/
	EDM_CRC_MUX_CH1_TPG_OUTPUT		= 5,/**< Channel 1 CRC Mux TPG output*/
}EDM_CRC_MUX_CH1_E;

/**
 * \enum EDM_CRC_MUX_CH2_E
 * \brief CRC Mux for Channel 2
 */
typedef enum
{
	EDM_CRC_MUX_CH2_WDMA1_IN_CH2 	= 1,/**< Channel 2 CRC Mux WDMA1 input*/
	EDM_CRC_MUX_CH2_WDMA3_IN_CH2	= 3,/**< Channel 2 CRC Mux WDMA3 input*/
	EDM_CRC_MUX_CH2_RDMA_IN_CH2		= 4,/**< Channel 2 CRC Mux RDMA output*/
	EDM_CRC_MUX_CH2_TPG_OUTPUT		= 5,/**< Channel 2 CRC Mux TPG output*/
}EDM_CRC_MUX_CH2_E;

/**
 * \enum EDM_CRC_UPDATE_E
 * \brief CRC update method vsync or cycle base
 */
typedef enum
{
	EDM_CRC_UPDATE_VSYNC 		= 0,/**< CRC update method vsync update*/
	EDM_CRC_UPDATE_CYCLEBASE	= 1,/**< CRC update method cycle update*/
}EDM_CRC_UPDATE_E;

/**
 * \enum EDM_NODE_SEL_E
 * \brief EDM monitor node selection
 */
typedef enum
{
	EDM_NODE_SEL_A 		= 0,/**< EDM monitor node A(after INP)*/
	EDM_NODE_SEL_B		= 1,/**< EDM monitor node B(WDMA1 input)*/
	EDM_NODE_SEL_C		= 2,/**< EDM monitor node C(WDMA2 input)*/
	EDM_NODE_SEL_D		= 3,/**< EDM monitor node D(WDMA3 input)*/
	EDM_NODE_SEL_F		= 4,/**< EDM monitor node F(TPG output)*/
}EDM_NODE_SEL_E;

/**
 * \enum EDM_STATUS_BIT_E
 * \brief EDM interrupt status bit
 */
typedef enum
{
	EDM_STATUS_BIT_VSYNC			= 0,/**< [0]:vsync_active: vsync cycles > REG_EDM_VTH*/
	EDM_STATUS_BIT_HSYNC			= 1,/**< [1]:hsync_active: hsync cycles > REG_EDM_HTH*/
	EDM_STATUS_BIT_OVERLAP_VH		= 2,/**< [2]:overlap_vh : vsync overlap with hsync*/
	EDM_STATUS_BIT_OVERLAP_HD		= 3,/**< [3]:overlap_hd : hsync overlap with date enable*/
	EDM_STATUS_BIT_OVERLAP_DF		= 4,/**< [4]:overlap_df : data enable overlap with frame end*/
	EDM_STATUS_BIT_HSYNC_FP			= 5,/**< [5]:hsync_front_porch: fp < REG_EDM_HFP_TH*/
	EDM_STATUS_BIT_HSYNC_BP			= 6,/**< [6]:hsync_back_porch: bp < REG_EDM_HBP_TH*/
	EDM_STATUS_BIT_FE				= 7,/**< [7]:frame_end_timing: frame end cycles > 255*/
	EDM_STATUS_BIT_CON_VSYNC		= 8,/**< [8]:con_vsync_active:Conventional vsync cycles > REG_EDM _CON_VTH*/
	EDM_STATUS_BIT_CON_HSYNC		= 9,/**< [9]:con_hsync_active:Conventional hsync cycles > REG_EDM _CON_HTH*/
	EDM_STATUS_BIT_CH0_DE_LESS		= 10,/**<[10]:ch0_de_less: de < REG_EDM_CH0_DE_TH*/
	EDM_STATUS_BIT_CH0_DE_MORE		= 11,/**<[11]:ch0_de_more:de > REG_EDM_CH0_DE_TH*/
	EDM_STATUS_BIT_CH1_DE_LESS		= 12,/**<[12]:ch1_de_less: de < REG_EDM_CH1_DE_TH*/
	EDM_STATUS_BIT_CH1_DE_MORE		= 13,/**<[13]:ch1_de_more: de > REG_EDM_CH1_DE_TH*/
	EDM_STATUS_BIT_CH2_DE_LESS		= 14,/**<[14]:ch2_de_less: de < REG_EDM_CH2_DE_TH*/
	EDM_STATUS_BIT_CH2_DE_MORE		= 15,/**<[15]:ch2_de_more: de > REG_EDM_CH2_DE_TH*/
	EDM_STATUS_BIT_CONV_DE_LESS		= 16,/**<[16]:con_de_more: total de > REG_EDM_CH0_DE_TH*/
	EDM_STATUS_BIT_CONV_DE_MORE		= 17,/**<[17]:con_de_more: total de < REG_EDM_CH0_DE_TH*/
	EDM_STATUS_BIT_WDT1				= 18,/**<[18]:WDT1 time out*/
	EDM_STATUS_BIT_WDT2				= 19,/**<[19]:WDT2 time out*/
	EDM_STATUS_BIT_WDT3				= 20,/**<[20]:WDT3 time out*/
}EDM_STATUS_BIT_E;

/**
 * \enum EDM_STATUS_E
 * \brief EDM interrupt status
 */
typedef enum
{
	EDM_STATUS_VSYNC			= 0x1,/**< [0]:vsync_active: vsync cycles > REG_EDM_VTH*/
	EDM_STATUS_HSYNC			= 0x2,/**< [1]:hsync_active: hsync cycles > REG_EDM_HTH*/
	EDM_STATUS_OVERLAP_VH		= 0x4,/**< [2]:overlap_vh : vsync overlap with hsync*/
	EDM_STATUS_OVERLAP_HD		= 0x8,/**< [3]:overlap_hd : hsync overlap with date enable*/
	EDM_STATUS_OVERLAP_DF		= 0x10,/**< [4]:overlap_df : data enable overlap with frame end*/
	EDM_STATUS_HSYNC_FP			= 0x20,/**< [5]:hsync_front_porch: fp < REG_EDM_HFP_TH*/
	EDM_STATUS_HSYNC_BP			= 0x40,/**< [6]:hsync_back_porch: bp < REG_EDM_HBP_TH*/
	EDM_STATUS_FE				= 0x80,/**< [7]:frame_end_timing: frame end cycles > 255*/
	EDM_STATUS_CON_VSYNC		= 0x100,/**< [8]:con_vsync_active:Conventional vsync cycles > REG_EDM _CON_VTH*/
	EDM_STATUS_CON_HSYNC		= 0x200,/**< [9]:con_hsync_active:Conventional hsync cycles > REG_EDM _CON_HTH*/
	EDM_STATUS_CH0_DE_LESS		= 0x400,/**<[10]:ch0_de_less: de < REG_EDM_CH0_DE_TH*/
	EDM_STATUS_CH0_DE_MORE		= 0x800,/**<[11]:ch0_de_more:de > REG_EDM_CH0_DE_TH*/
	EDM_STATUS_CH1_DE_LESS		= 0x1000,/**<[12]:ch1_de_less: de < REG_EDM_CH1_DE_TH*/
	EDM_STATUS_CH1_DE_MORE		= 0x2000,/**<[13]:ch1_de_more: de > REG_EDM_CH1_DE_TH*/
	EDM_STATUS_CH2_DE_LESS		= 0x4000,/**<[14]:ch2_de_less: de < REG_EDM_CH2_DE_TH*/
	EDM_STATUS_CH2_DE_MORE		= 0x8000,/**<[15]:ch2_de_more: de > REG_EDM_CH2_DE_TH*/
	EDM_STATUS_CONV_DE_LESS		= 0x10000,/**<[16]:con_de_more: total de > REG_EDM_CH0_DE_TH*/
	EDM_STATUS_CONV_DE_MORE		= 0x20000,/**<[17]:con_de_more: total de < REG_EDM_CH0_DE_TH*/
	EDM_STATUS_WDT1				= 0x40000,/**<[18]:WDT1 time out*/
	EDM_STATUS_WDT2				= 0x80000,/**<[19]:WDT2 time out*/
	EDM_STATUS_WDT3				= 0x100000,/**<[20]:WDT3 time out*/
}EDM_STATUS_E;
/** @} */

/**
 * \defgroup	EDM_DRV_REGSTRUCT	Error Detection Module Driver Structure
 * \ingroup	EDM_DRV
 * \brief	contains definitions of Error Detection Module Driver structure.
 * @{
 */
/**
 * \struct EDM_CRC_CFG_T
 * \brief EDM CRC configuration by EDM Driver
 */
typedef struct
{
	uint8_t crc_ch0_en;/**< CRC channel 0 enable*/
	uint8_t crc_ch1_en;/**< CRC channel 1 enable*/
	uint8_t crc_ch2_en;/**< CRC channel 2 enable*/
	uint8_t crc_conv_en;/**< CRC Conventional enable*/
	EDM_CRC_MUX_CH0_E ch0_mux;/**< CRC channel 0 mux*/
	EDM_CRC_MUX_CH1_E ch1_mux;/**< CRC channel 1 mux*/
	EDM_CRC_MUX_CH2_E ch2_mux;/**< CRC channel 2 mux*/
} EDM_CRC_CFG_T;

/**
 * \struct EDM_ABCDF_CFG_T
 * \brief EDM Timing monitor configuration by EDM Driver
 */
typedef struct
{
	uint8_t edm_ch0_en;/**< edm timing detect channel 0 enable*/
	uint8_t edm_ch1_en;/**< edm timing detect channel 1 enable*/
	uint8_t edm_ch2_en;/**< edm timing detect channel 2 enable*/
	uint8_t edm_firstframe_switch;/**< edm timing detect first frame detect switch*/
    EDM_NODE_SEL_E node_sel;/**< edm timing node selection*/
    uint16_t max_edm_vth;/**< edm timing The maximum of Vsync acitive cycles*/
    uint16_t max_edm_hth;/**< edm timing The maximum of Hsync acitive cycles*/
    uint32_t max_edm_ch0_de_th;/**< edm timing Total cycles of ch0 data enable per frame*/
    uint32_t max_edm_ch1_de_th;/**< edm timing Total cycles of ch1 data enable per frame*/
    uint32_t max_edm_ch2_de_th;/**< edm timing Total cycles of ch2 data enable per frame*/
    uint32_t max_edm_hfp_th;/**< edm timing The minimum of Hsync front porch cycles*/
    uint32_t max_edm_hbp_th;/**< edm timing The minimum of Hsync back porch cycles*/
} EDM_ABCDF_CFG_T;

/**
 * \struct EDM_G_CFG_T
 * \brief EDM Timing monitor configuration by EDM Driver
 */
typedef struct
{
	uint8_t edm_conv_en;/**< edm timing detect conventional enable*/
	uint8_t edm_firstframe_switch;/**< edm timing detect first frame detect only enable*/
    uint32_t max_edm_con_vth;/**< edm timing detect The maximum of conventional Vsync acitive cycles, equal to (Vtotal * Htotal)*/
    uint16_t max_edm_con_hth;/**< edm timing detect The number of conventional Hsync acitive cycles, equal to Hsize.*/
    uint32_t max_edm_ch0_de_th;/**< edm timing Total cycles of ch0 data enable per frame*/
} EDM_G_CFG_T;

/**
 * \struct EDM_WDT_CFG_T
 * \brief EDM Watch Dog configuration by EDM Driver
 */
typedef struct
{
	uint8_t edm_wdt1_en;/**< edm watch dog for WDMA1 enable*/
	uint8_t edm_wdt2_en;/**< edm watch dog for WDMA2 enable*/
	uint8_t edm_wdt3_en;/**< edm watch dog for WDMA3 enable*/
    uint32_t wdt1_th;/**< edm watch dog threshold for WDMA1 (unit:ms)*/
    uint32_t wdt2_th;/**< edm watch dog threshold for WDMA2 (unit:ms)*/
    uint32_t wdt3_th;/**< edm watch dog threshold for WDMA3 (unit:ms)*/
} EDM_WDT_CFG_T;
/** @} */

/**
 * \defgroup	EDM_DRV_FUNCDLR	Error Detection Module Driver Function Declaration
 * \ingroup	EDM_DRV
 * \brief	Contains declarations of Error Detection Module Driver functions.
 * @{
 */
/**
 * \brief	EDM interrupt callback function
 *
 * \return	void.
 */
typedef void (*EDM_ISREvent_t) (uint32_t event);

/**
 * \brief	initial EDM Driver Reg base address
 *
 * \param[in]	reg_baseaddr	 	EDM reg base address
 * \param[in]	reg_hsc_baseaddr	EDM HSC reg base address
 * \return	void.
 */
void hx_drv_edm_init_regbase(uint32_t reg_baseaddr, uint32_t reg_hsc_baseaddr);

/**
 * \brief	initial EDM Driver
 *
 * \return	void.
 */
void hx_drv_edm_init();

/**
 * \brief	set all EDM related Setting
 *
 * \param[in]	edm_cfg	 EDM configuration
 * \return	void.
 */
void hx_drv_edm_set_CRC_Cfg(EDM_CRC_CFG_T edm_cfg);

/**
 * \brief	get EDM CRC Ch0 CRC
 *
 * \param[in]	method	 crc update method vsync or cycle base
 * \param[out]	ch0_crc	 channel 0 crc value
 * \return	void.
 */
void hx_drv_edm_get_ch0_CRC(EDM_CRC_UPDATE_E method, uint16_t *ch0_crc);

/**
 * \brief	get EDM CRC Ch1 CRC
 *
 * \param[in]	method	 crc update method vsync or cycle base
 * \param[out]	ch1_crc	 channel 1 vcrc alue
 * \return	void.
 */
void hx_drv_edm_get_ch1_CRC(EDM_CRC_UPDATE_E method, uint16_t *ch1_crc);

/**
 * \brief	get EDM CRC Ch2 CRC
 *
 * \param[in]	method	 crc update method vsync or cycle base
 * \param[out]	ch2_crc	 channel 2 crc value
 * \return	void.
 */
void hx_drv_edm_get_ch2_CRC(EDM_CRC_UPDATE_E method, uint16_t *ch2_crc);

/**
 * \brief	get EDM CRC Conventional CRC
 *
 * \param[in]	method	 crc update method vsync or cycle base
 * \param[out]	conv_crc	Conventional crc value
 * \return	void.
 */
void hx_drv_edm_get_Conv_CRC(EDM_CRC_UPDATE_E method, uint16_t *conv_crc);

/**
 * \brief	set EDM timing detect enable module
 *
 * \param[in]	ch0_en	 edm channel 0 timing detect enable
 * \param[in]	ch1_en	 edm channel 1 timing detect enable
 * \param[in]	ch2_en	 edm channel 2 timing detect enable
 * \param[in]	conv_en	 edm conventional timing detect enable
 * \return	void.
 */
void hx_drv_edm_set_enable(uint8_t ch0_en, uint8_t ch1_en,uint8_t ch2_en,uint8_t conv_en);
/**
 * \brief	set EDM timing detect first frame only
 *
 * \param[in]	first_frame	 edm first_frame detect enable
 * \return	void.
 */
void hx_drv_edm_set_firstframeonly(uint8_t first_frame);
/**
 * \brief	set EDM timing detect node selection
 *
 * \param[in]	sel	 EDM monitor node selection
 * \return	void.
 */
void hx_drv_edm_set_node_sel(EDM_NODE_SEL_E sel);
/**
 * \brief	set EDM Pulse Timing monitor configuration
 *
 * \param[in]	edm_cfg	 EDM pulse Timing monitor configuration by EDM Driver
 * \return	void.
 */
void hx_drv_edm_set_pulse_Cfg(EDM_ABCDF_CFG_T edm_cfg);
/**
 * \brief	set EDM Conventional Timing monitor configuration
 *
 * \param[in]	edm_cfg	 EDM Conventional Timing monitor configuration by EDM Driver
 * \return	void.
 */
void hx_drv_edm_set_conv_Cfg(EDM_G_CFG_T edm_cfg);
/**
 * \brief	set EDM Watch Dog configuration
 *
 * \param[in]	edm_cfg	 EDM Watch Dog configuration by EDM Driver
 * \return	void.
 */
void hx_drv_edm_set_wdt_Cfg(EDM_WDT_CFG_T edm_cfg);
/**
 * \brief	get EDM Interrupt status
 *
 * \param[out]	status	 EDM Interrupt status
 * \return	void.
 */
void hx_drv_edm_get_status(uint32_t *status);
/**
 * \brief	set EDM Interrupt mask
 *
 * \param[in]	mask	 EDM Interrupt mask
 * \return	void.
 */
void hx_drv_edm_set_mask(uint32_t mask);
/**
 * \brief	get EDM Interrupt mask
 *
 * \param[out]	mask	 EDM Interrupt mask
 * \return	void.
 */
void hx_drv_edm_get_mask(uint32_t *mask);
/**
 * \brief	set EDM Interrupt clear bit
 *
 * \param[in]	clear	 EDM Interrupt clear bit
 * \return	void.
 */
void hx_drv_edm_set_clear(uint32_t clear);
/**
 * \brief	register EDM watchdog timeout callback
 *
 * \param[in]	cb_event	 watchdog timeout callback function
 * \return	void.
 */
void hx_drv_edm_register_wdt_cb(EDM_ISREvent_t  cb_event);
/**
 * \brief	register EDM timing violation callback
 *
 * \param[in]	cb_event	 timing violation callback function
 * \return	void.
 */
void hx_drv_edm_register_timing_cb(EDM_ISREvent_t  cb_event);

/**
 * \brief	get EDM Conventional timing H-FPorch, H-BPorch
 *
 * \param[out]	fp	 Conventional timing detect for H-Fporch
 * \param[out]	bp	 Conventional timing detect for H-Bporch
 * \return	void.
 */
void hx_drv_edm_get_Conv_Porch(uint16_t *fp, uint16_t *bp);

/**
 * \brief	get EDM V-Blanking, H-Blanking
 *
 * \param[out]	h_blanking	 timing detect for h_blanking
 * \param[out]	v_blanking	 timing detect for v_blanking
 * \return	void.
 */
void hx_drv_edm_get_Conv_Blanking(uint16_t *h_blanking, uint32_t *v_blanking);

/**
 * \brief	get EDM detect DE count
 *
 * \param[in]	ch012	 channel 0, 1 or 2 selection
 * \param[out]	de_cnt	 timing detect for DE count
 * \return	void.
 */
void hx_drv_edm_get_de_count(uint8_t ch012, uint32_t *de_cnt);

/**
 * \brief	get EDM Conv DE count
 *
 * \param[out]	de_cnt	 timing detect for Conventional DE count
 * \return	void.
 */
void hx_drv_edm_get_conv_de_count(uint32_t *de_cnt);

/**
 * \brief	get EDM WDT Timer count
 *
 * \param[in]	wdt123	 WDT 1, 2 or 3 selection
 * \param[out]	wdt_cnt	 WDT Timer count
 * \return	void.
 */
void hx_drv_edm_get_wdt_count(uint8_t wdt123, uint32_t *wdt_cnt);

/**
 * \brief	get EDM Timing detect for frame counter
 *
 * \param[out]	af_framecnt	 After INP frame counter detection
 * \param[out]	be_framecnt	 Before INP frame counter detection
 * \return	void.
 */
void hx_drv_edm_get_frame_count(uint16_t *af_framecnt, uint16_t *be_framecnt);

/**
 * \brief	get EDM Channel 0 H-Front Porch, Channel 0, 1 or 2 H-Back Porch
 *
 * \param[in]	ch012_hbp	 Channel 0, 1 or 2 H-Back Porch selection
 * \param[out]	ch0_hfp	 channel 0 H-Front Porch
 * \param[out]	hbp	 Channel 0, 1 or 2 H-Back Porch
 * \return	void.
 */
void hx_drv_edm_get_pulse_hporch(uint8_t ch012_hbp,uint16_t *ch0_hfp, uint16_t *hbp);

/**
 * \brief	get EDM CH0 Htotal
 *
 * \param[out]	h_total	 channel 0 HTotal
 * \return	void.
 */
void hx_drv_edm_get_htotal(uint16_t *h_total);

/**
 * \brief	get EDM CON VTH/HTH
 *
 * \param[out]	con_vth	 CON VTH
 * \param[out]	con_hth	 CON HTH
 * \return	void.
 */
void hx_drv_edm_get_con_vth_hth(uint32_t *con_vth, uint32_t *con_hth);
/** @} */

#endif /* INC_HX_DRV_EDM_H_ */
