/*
 * hx_drv_TPG.h
 *
 *  Created on: 2018¦~11¤ë9¤é
 *      Author: 902447
 */

#ifndef INC_HX_DRV_TPG_H_
#define INC_HX_DRV_TPG_H_

#include "dev_common.h"
/**
 * \defgroup	TPG_DRV	Timing and Pattern Generator Driver
 * \ingroup		TPG_DRV
 * \brief	Timing and Pattern Generator Driver Declaration
 */
/**
 * \defgroup	TPG_DRV_ENUM	Timing and Pattern Generator Driver Enumeration
 * \ingroup	TPG_DRV
 * \brief	Defines some enumeration of Timing and Pattern Generator Driver need.
 * @{
 */
/**
 * \enum TPG_ERROR_E
 * \brief TPG Errors Type
 */
typedef enum
{
	TPG_NO_ERROR                     	= 0,	/**< No ERROR */
	TPG_ERROR_NOT_INIT              	= 1,	/**< ERROR MSG: not init */
	TPG_ERROR_ALREADY_INIT              = 2,	/**< ERROR MSG: already init */
	TPG_ERROR_INVALID_PARAMETERS     	= 3,	/**< ERROR MSG: Invalid parameters */
	TPG_UNKNOWN_ERROR      				= 4,	/**< ERROR MSG: UNKNOWN ERROR*/
} TPG_ERROR_E;

/**
 * \enum TPG_CASE_E
 * \brief TPG Pattern Generator case
 */
typedef enum
{
	TPG_CASE_CDM_HL_TH_FROM_RDMA 					= 0, /**< CDM HL TH but no Pixel data case from RDMA */
	TPG_CASE_JPEG_DEC_FROM_RDMA 					= 1, /**< JPEG Decode from TPG RDMA */
	TPG_CASE_NOINP_1CHDATA_FROM_IPG 				= 2, /**< Not through INP from TPG IPG Conventional mode*/
	TPG_CASE_TOINP_1CHDATA_FROM_IPG_PULSE 			= 3, /**< Through INP from TPG IPG Pulse mode*/
	TPG_CASE_NOINP_1CHDATA_FROM_RDMA 				= 4, /**< Not Through INP from TPG RDMA Conventional mode*/
	TPG_CASE_TOINP_1CHDATA_FROM_RDMA_PULSE			= 5, /**< Through INP from TPG RDMA Pulse mode*/
	TPG_CASE_YUV422_JPEG_ENC_FROM_RDMA 				= 6, /**< YUV422 JPEG Encoder from TPG RDMA*/
	TPG_CASE_YUV420_JPEG_ENC_FROM_RDMA 				= 7, /**< YUV420 JPEG Encoder from TPG RDMA*/
	TPG_CASE_NOINP_CDM_PIXEL_HL_TH_FROM_RDMA 		= 8, /**< Not Through INP CDM HL TH and Pixel data case from RDMA */
	TPG_CASE_TOINP_CDM_PIXEL_HL_TH_FROM_RDMA 		= 9, /**< Through INP CDM HL TH and Pixel data case from RDMA */
	TPG_CASE_TO_INP_FROM_IPG_CONVENTION				= 10,/**< Through INP from IPG Convention Timing Mode */
	TPG_CASE_TO_INP_FROM_RDMA_CONVENTION 			= 11,/**< Through INP from RDMA Convention Timing Mode */
	TPG_CASE_NOINP_CDM_PIXELIPG_HL_TH_FROM_RDMA 	= 12,/**< Not Through INP CDM pixel data from IPG and HL from RDMA*/
	TPG_CASE_USERDEF 								= 13,/**< User Defined*/
}TPG_CASE_E;

/**
 * \enum TPG_TG_SEL_E
 * \brief TPG Timing Generator Selection
 */
typedef enum
{
	TPG_TG_SEL_OFF 						= 0,/**< 0 : TG off*/
	TPG_TG_CTRL_FROM_CH1_DE 			= 1,/**< 1 : TG control from Ch1 empty matrix*/
	TPG_TG_CTRL_FROM_CH2_DE 			= 2,/**< 2 : TG control from Ch2 empty matrix*/
	TPG_TG_CTRL_FROM_CH3_DE 			= 3,/**< 3 : TG control from Ch3 empty matrix*/
}TPG_TG_SEL_E;

/**
 * \enum TPG_TG_CH_DE_SEL_E
 * \brief TPG Timing Generator Selection
 */
typedef enum
{
	TPG_TG_CH_DE_SEL_OFF 						= 0,/**< 0 : Off*/
	TPG_TG_CH_DE_SEL_CONTINOUS_DIS 				= 1,/**< 1 : TG Continues / Discontinues*/
	TPG_TG_CH_DE_SEL_RERG_YUV 					= 2,/**< 2 : TG DERG YUV422/420*/
	TPG_TG_CH_DE_SEL_CTRL_FROM_HANDSHAKE 		= 3,/**< 3 : Control from handshake w/o TG*/
}TPG_TG_CH_DE_SEL_E;

/**
 * \enum TPG_CH_EMPTY_CTRL_E
 * \brief RDMA empty input control
 */
typedef enum
{
	TPG_CH_EMPTY_CTRL_BYPASS 			= 0,/**< 0 : By pass empty*/
	TPG_CH_EMPTY_CTRL_TGDE 				= 1,/**< 1 : Control from TG DE*/
	TPG_CH_EMPTY_CTRL_TGDERG 			= 2,/**< 2 : Control from TG DERG*/
	TPG_CH_EMPTY_CTRL_OFF 				= 3,/**< 3 : off*/
}TPG_CH_EMPTY_CTRL_E;

/**
 * \enum TPG_CH1_EMPTY_SEL_E
 * \brief Ch1 RDMA empty mix
 */
typedef enum
{
	TPG_CH1_EMPTY_SEL_1 			= 0,/**< 0 : empty 1*/
	TPG_CH1_EMPTY_SEL_12 			= 1,/**< 1 : empty 1+2*/
	TPG_CH1_EMPTY_SEL_13 			= 2,/**< 2 : empty 1+3*/
	TPG_CH1_EMPTY_SEL_123 			= 3,/**< 3 : empty 1+2+3*/
}TPG_CH1_EMPTY_SEL_E;

/**
 * \enum TPG_CH2_EMPTY_SEL_E
 * \brief Ch2 RDMA empty mix
 */
typedef enum
{
	TPG_CH2_EMPTY_SEL_2 			= 0,/**< 0 : empty 2*/
	TPG_CH2_EMPTY_SEL_12 			= 1,/**< 1 : empty 1+2*/
	TPG_CH2_EMPTY_SEL_23 			= 2,/**< 2 : empty 2+3*/
	TPG_CH2_EMPTY_SEL_123 			= 3,/**< 3 : empty 1+2+3*/
}TPG_CH2_EMPTY_SEL_E;

/**
 * \enum TPG_CH3_EMPTY_SEL_E
 * \brief Ch3 RDMA empty mix
 */
typedef enum
{
	TPG_CH3_EMPTY_SEL_3 			= 0,/**< 0 : empty 3*/
	TPG_CH3_EMPTY_SEL_13 			= 1,/**< 1 : empty 1+3*/
	TPG_CH3_EMPTY_SEL_23 			= 2,/**< 2 : empty 2+3*/
	TPG_CH3_EMPTY_SEL_123 			= 3,/**< 3 : empty 1+2+3*/
}TPG_CH3_EMPTY_SEL_E;

/**
 * \enum TPG_RDY_CH_SEL_E
 * \brief REG_TPG_CH1_RDY_SEL
 */
typedef enum
{
	TPG_RDY_CH_SEL_NOT_CARE 		= 0,/**< 0,3 : Don¡¦t care ready signal*/
	TPG_RDY_CH_SEL_CDM_READY 		= 1,/**< 1 : CDM Ready*/
	TPG_RDY_CH_SEL_JPEGDEC_READY 	= 2,/**< 2 : JPEG Decoder Ready*/
}TPG_RDY_CH_SEL_E;

/**
 * \enum TPG_CH_PG_SEL_E
 * \brief REG_TPG_CH1_PG_SEL IPG or RDMA selection
 */
typedef enum
{
	TPG_CH_PG_SEL_IPG 		= 0,/**< 0 : Internal pattern*/
	TPG_CH_PG_SEL_RDMA 		= 1,/**< 1 : External pattern (RDMA)*/
}TPG_CH_PG_SEL_E;

/**
 * \enum TPG_TG_MS_E
 * \brief REG_TPG_TG_V_MS
 */
typedef enum
{
	TPG_TG_V_MS_LINE 		= 0,/**< 0 : Line mode*/
	TPG_TG_V_MS_PIXEL 		= 1,/**< 1 : Pixel mode*/
}TPG_TG_MS_E;

/**
 * \enum TPG_TG_V_POL_E
 * \brief Vertical Timing Polarity
 */
typedef enum
{
	TPG_TG_V_POL_VSYNC_HIGH 		= 2,  /**< Vertical Timing VSync High*/
	TPG_TG_V_POL_VACT_HIGH 			= 0x8,/**< Vertical Timing V-Active High*/
	TPG_TG_V_POL_VFP_VBP_VACT_HIGH 	= 0xD,/**< Vertical Timing VFP, VBP and V-Active High*/
	TPG_TG_V_POL_ALL_HIGH 			= 0xF,/**< Vertical Timing Vsync, VFP, VBP and V-Active High*/
}TPG_TG_V_POL_E;


/**
 * \enum TPG_TG_V_WH_E
 * \brief Vertical Timing Polarity
 */
typedef enum
{
	TPG_TG_V_WH_NOHSYNC 	= 0,/**< 0 : VSYNC without HSYNC*/
	TPG_TG_V_WH_HSYNC 		= 1,/**< 1 : VSYNC with HSYNC*/
}TPG_TG_V_WH_E;


/**
 * \enum TPG_TG_H_POL_E
 * \brief Horizontal Timing Polarity
 */
typedef enum
{
	TPG_TG_H_POL_HSYNC_HIGH 		= 2, /**< Horizontal Timing HSync High*/
	TPG_TG_H_POL_HACT_HIGH 			= 0x8,/**< Horizontal Timing H-Active High*/
	TPG_TG_H_POL_HFP_HBP_HACT_HIGH 	= 0xD,/**< Horizontal Timing HFP, HBP and H-Active High*/
	TPG_TG_H_POL_ALL_HIGH 			= 0xF,/**< Horizontal Timing Hsync, HFP, HBP and H-Active High*/
}TPG_TG_H_POL_E;

/**
 * \enum TPG_TG_DERG_MS_E
 * \brief REG_TPG_TG_DERG_MS
 */
typedef enum
{
	TPG_TG_DERG_OFF 					= 0,/**< 0 : off*/
	TPG_TG_DERG_YUV422_P_EVEN 			= 1,/**< 1 : YUV422 : Pixel(even)*/
	TPG_TG_DERG_YUV420_P_EVEN_L_EVEN 	= 3,/**< 3 : YUV420 : Pixel(even), Line(even)*/
	TPG_TG_DERG_YUV422_P_ODD 			= 5,/**< 5 : YUV422 : Pixel(odd)*/
	TPG_TG_DERG_YUV420_P_ODD_L_EVEN 	= 7,/**< 7 : YUV420 : Pixel(odd), Line(even)*/
	TPG_TG_DERG_YUV420_P_EVEN_L_ODD 	= 0xB,/**< B : YUV420 : Pixel(even), Line(odd)*/
	TPG_TG_DERG_YUV420_P_ODD_L_ODD 		= 0xF,/**< F : YUV420 : Pixel(odd), Line(odd)*/
}TPG_TG_DERG_MS_E;

/**
 * \enum TPG_IPG_TYPE_E
 * \brief Internal pattern
 */
typedef enum
{
	TPG_IPG_TYPE_PIXEL_COUNT 		= 0,/**< 0 : Pixel : Count Mode*/
	TPG_IPG_TYPE_PIXEL_WIDTH 		= 1,/**< 1 : Pixel : Width Mode*/
	TPG_IPG_TYPE_LINE_COUNT 		= 2,/**< 2 : Line : Count Mode*/
	TPG_IPG_TYPE_LINE_WIDTH 		= 3,/**< 3 : Line Width Mode*/
	TPG_IPG_TYPE_SOLID_COLOR 		= 4,/**< 4 : Frame Solid Color*/
	TPG_IPG_TYPE_FRAME_COUNT 		= 5,/**< 5 : Frame Count Mode*/
	TPG_IPG_TYPE_XOR_ZOOMOUT 		= 6,/**< 6 : XOR Zoom-Out Mode*/
	TPG_IPG_TYPE_XOR_ZOOMIN 		= 7,/**< 7 : XOR Zoom-In Mode*/
}TPG_IPG_TYPE_E;
/** @} */

/**
 * \defgroup	TPG_DRV_REGSTRUCT	Timing and Pattern Generator Driver Structure
 * \ingroup	TPG_DRV
 * \brief	contains definitions of Timing and Pattern Generator Driver structure.
 * @{
 */
/**
 * \struct TPG_CTRL_CFG_T
 * \brief TPG Control configuration by TPG Driver
 */
typedef struct
{
	TPG_TG_SEL_E tg_sel;/**< TPG Timing Generator Selection*/
	TPG_TG_CH_DE_SEL_E ch1_de_sel;/**< TPG Channel 1 Timing Generator Selection*/
	TPG_TG_CH_DE_SEL_E ch2_de_sel;/**< TPG Channel 2 Timing Generator Selection*/
	TPG_TG_CH_DE_SEL_E ch3_de_sel;/**< TPG Channel 3 Timing Generator Selection*/
	TPG_CH_EMPTY_CTRL_E ch1_empty_ctrl;/**< RDMA empty input control channel 1*/
	TPG_CH_EMPTY_CTRL_E ch2_empty_ctrl;/**< RDMA empty input control channel 2*/
	TPG_CH_EMPTY_CTRL_E ch3_empty_ctrl;/**< RDMA empty input control channel 3*/
	TPG_CH1_EMPTY_SEL_E ch1_empty_sel;/**< Ch1 RDMA empty mix*/
	TPG_CH2_EMPTY_SEL_E ch2_empty_sel;/**< Ch2 RDMA empty mix*/
	TPG_CH3_EMPTY_SEL_E ch3_empty_sel;/**< Ch3 RDMA empty mix*/
	TPG_RDY_CH_SEL_E ch1_rdy_sel;/**< REG_TPG_CH1_RDY_SEL*/
	TPG_RDY_CH_SEL_E ch2_rdy_sel;/**< REG_TPG_CH2_RDY_SEL*/
	TPG_RDY_CH_SEL_E ch3_rdy_sel;/**< REG_TPG_CH3_RDY_SEL*/
	TPG_TG_DERG_MS_E derg_mode_sel;/**< REG_TPG_TG_DERG_MS*/
}TPG_CTRL_CFG_T;

/**
 * \struct TPG_IPG_RDMA_CFG_T
 * \brief TPG IPG or RDMA Selection by TPG Driver
 */
typedef struct
{
	TPG_CH_PG_SEL_E ch1_pg_sel;/**< channel 1 IPG or RDMA selection*/
	TPG_CH_PG_SEL_E ch2_pg_sel;/**< channel 2 IPG or RDMA selection*/
	TPG_CH_PG_SEL_E ch3_pg_sel;/**< channel 3 IPG or RDMA selection*/
}TPG_IPG_RDMA_CFG_T;

/**
 * \struct TPG_TIMING_TYPE_T
 * \brief TPG Timing type configuration by TPG Driver
 */
typedef struct
{
	TPG_TG_MS_E v_ms_sel;/**< REG_TPG_TG_V_MS line mode or pixel mode*/
	TPG_TG_V_POL_E v_pol;/**< Vertical Timing Polarity*/
	TPG_TG_V_WH_E v_wo_hsync;/**< Vertical Timing Polarity*/
	TPG_TG_H_POL_E h_pol;/**< Horizontal Timing Polarity*/
}TPG_TIMING_TYPE_T;

/**
 * \struct TPG_TIMING_T
 * \brief TPG Timing configuration by TPG Driver
 */
typedef struct
{
	uint8_t H_FP;/**< Horizontal Front Porch*/
	uint8_t H_SY;/**< Horizontal Sync*/
	uint8_t H_BP;/**< Horizontal Back Porch*/
	uint16_t H_AC;/**< Horizontal Active*/
	uint8_t V_FP;/**< Vertical Front Porch*/
	uint8_t V_SY;/**< Vertical Sync*/
	uint8_t V_BP;/**< Vertical Back Porch*/
	uint16_t V_AC;/**< Vertical Active*/
}TPG_TIMING_T;

/**
 * \struct TPG_IPG_CFG_T
 * \brief TPG IPG Pattern configuration by TPG Driver
 */
typedef struct
{
	TPG_IPG_TYPE_E ch1_ipg_pattern;/**< Channel 1 Internal pattern type*/
	uint8_t ch1_ipg_val;/**< Channel 1 Internal pattern value*/
	TPG_IPG_TYPE_E ch2_ipg_pattern;/**< Channel 2 Internal pattern type*/
	uint8_t ch2_ipg_val;/**< Channel 2 Internal pattern value*/
	TPG_IPG_TYPE_E ch3_ipg_pattern;/**< Channel 3 Internal pattern type*/
	uint8_t ch3_ipg_val;/**< Channel 3 Internal pattern value*/
} TPG_IPG_CFG_T;

/**
 * \struct TPG_CFG_T
 * \brief TPG configuration by TPG Driver
 */
typedef struct
{
	uint8_t tpg_en;/**< TPG enable*/
	TPG_CASE_E tpg_case;/**< TPG Case*/
	TPG_CTRL_CFG_T ctrl_cfg;/**< TPG Control configuration*/
	TPG_IPG_RDMA_CFG_T src_cfg;/**< TPG Source Control configuration*/
	TPG_TIMING_TYPE_T timing_type;/**< TPG Timing Type configuration*/
	TPG_TIMING_T timing_info;/**< TPG Timing configuration*/
	TPG_IPG_CFG_T ipg_cfg;/**< TPG IPG configuration*/
} TPG_CFG_T;
/** @} */

/**
 * \defgroup	TPG_DRV_MACRO	Timing and Pattern Generator Driver MACRO
 * \ingroup	TPG_DRV
 * \brief	contains definitions of Timing and Pattern Generator Driver MACRO.
 * @{
 */
#define MAX_TPG_WIDTH_CONSTRAINT 		1023/**< TPG timing constraint of max width*/
#define MAX_TPG_HEIGHT_CONSTRAINT 		1023/**< TPG timing constraint of max height*/
#define MIN_TPG_WIDTH_CONSTRAINT 		1/**< TPG timing constraint of min width*/
#define MIN_TPG_HEIGHT_CONSTRAINT 		1/**< TPG timing constraint of min height*/
/** @} */

/**
 * \defgroup	TPG_DRV_FUNCDLR	Timing and Pattern Generator Driver Function Declaration
 * \ingroup	TPG_DRV
 * \brief	Contains declarations of Timing and Pattern Generator Driver functions.
 * @{
 */

/**
 * \brief	initial TPG Driver reg base address
 *
 * \param[in]	reg_addr	 TPG reg base address
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_init_reg_addr(uint32_t reg_addr);

/**
 * \brief	initial TPG Driver
 *
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_init();

/**
 * \brief	set all TPG related Setting
 *
 * \param[in]	tpg_cfg	 TPG configuration
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_set_allCfg(TPG_CFG_T tpg_cfg);

/**
 * \brief	get default TPG related Setting
 *
 * \param[out]	tpg_cfg	 TPG default configuration
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_get_defCfg(TPG_CFG_T *tpg_cfg);

/**
 * \brief	set TPG enable selection
 *
 * \param[in]	enable	 TPG enable
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_set_enable(uint8_t enable);

/**
 * \brief	get TPG enable selection
 *
 * \param[out]	enable	 TPG enable
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_get_enable(uint8_t *enable);

/**
 * \brief	set TPG case selection
 *
 * \param[in]	tpg_case	 TPG case selection
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_set_case(TPG_CASE_E tpg_case);

/**
 * \brief	set TPG control setting
 *
 * \param[in]	ctrl	 TPG control selection
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_set_ctrl(TPG_CTRL_CFG_T ctrl);

/**
 * \brief	get TPG control setting
 *
 * \param[out]	ctrl	 TPG control selection
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_get_ctrl(TPG_CTRL_CFG_T *ctrl);

/**
 * \brief	set TPG source selection
 *
 * \param[in]	srcctrl	 TPG source selection
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_set_srcctrl(TPG_IPG_RDMA_CFG_T srcctrl);

/**
 * \brief	get TPG source selection
 *
 * \param[out]	srcctrl	 TPG source selection
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_get_srcctrl(TPG_IPG_RDMA_CFG_T *srcctrl);

/**
 * \brief	set TPG Timing type selection
 *
 * \param[in]	type	 TPG timing type selection
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_set_timingType(TPG_TIMING_TYPE_T type);

/**
 * \brief	get TPG Timing type selection
 *
 * \param[out]	type	 TPG timing type selection
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_get_timingType(TPG_TIMING_TYPE_T *type);

/**
 * \brief	set TPG Timing selection
 *
 * \param[in]	timing	 TPG timing configuration
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_set_timing(TPG_TIMING_T timing);

/**
 * \brief	get TPG Timing selection
 *
 * \param[out]	timing	 TPG timing configuration
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_get_timing(TPG_TIMING_T *timing);

/**
 * \brief	set IPG Configuration
 *
 * \param[in]	ipg_cfg	 IPG Configuration
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_set_IPG(TPG_IPG_CFG_T ipg_cfg);

/**
 * \brief	get IPG Configuration
 *
 * \param[out]	ipg_cfg	 IPG Configuration
 * \return	TPG_ERROR_E.
 */
TPG_ERROR_E hx_drv_tpg_get_IPG(TPG_IPG_CFG_T *ipg_cfg);
/** @} */

#endif /* INC_HX_DRV_TPG_H_ */
