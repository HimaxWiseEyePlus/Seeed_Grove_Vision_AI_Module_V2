/*
 * hx_drv_HW2x2.h
 *
 *  Created on: 2018�~10��2��
 *      Author: 902447
 */

#ifndef _HX_DRV_HW2X2_H_
#define _HX_DRV_HW2X2_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
/**
 * \defgroup	HWACC_HW2X2_DRV	HW2x2 Hardware accelerator Driver
 * \ingroup		HWACC_HW2X2_DRV
 * \brief	HW2x2 Hardware accelerator Driver Declaration
 * \details Application can use HW2x2 Structure and datapath library to control Hardware\n
 * <pre>
 * For Example
 * Ex1: HW2x2_CFG_T: All HW2x2 configuration for application to use datapath library control HW2x2
 * </pre>
 */
/**
 * \defgroup	HWACC_HW2X2_DRV_ENUM	HW2x2 Hardware accelerator Driver Enumeration
 * \ingroup	HWACC_HW2X2_DRV
 * \brief	Defines some enumeration of HW2x2 Hardware accelerator Driver need.
 * @{
 */
/**
 * \enum HW2x2_ERROR_E
 * \brief HW2x2 Errors Type
 */
typedef enum {
	HW2x2_NO_ERROR = 0, /**< No ERROR */
	HW2x2_ERROR_NOT_INIT = 1, /**< ERROR MSG: not init */
	HW2x2_ERROR_ALREADY_INIT = 2, /**< ERROR MSG: already init */
	HW2x2_ERROR_INVALID_PARAMETERS = 3, /**< ERROR MSG: Invalid parameters */
	HW2x2_ERROR_INVALID_TIMING_RANGE = 4, /**< ERROR MSG: Invalid TIMING Range */
	HW2x2_ERROR_INVALID_TIMING_DIVISIBLE = 5, /**< ERROR MSG: Invalid TIMING Divisble */
	HW2x2_UNKNOWN_ERROR = 6, /**< ERROR MSG: UNKNOWN ERROR*/
} HW2x2_ERROR_E;
/**
 * \enum HW2x2_PATH_E
 * \brief HW2x2 Through or Internal Bypass
 */
typedef enum {
	HW2x2_PATH_THROUGH = 0,/**< HW2x2 Through*/
	HW2x2_PATH_BYPASS = 1, /**< HW2x2 Internal Bypass*/
} HW2x2_PATH_E;
/**
 * \enum HW2x2_MODE_E
 * \brief HW2x2 Subsample Mode
 */
typedef enum {
	HW2x2_MODE_UNITY = 0,/**< HW2x2 Unity Sample*/
	HW2x2_MODE_SUB4_LT = 1,/**< HW2x2 Sub-Sample LT 1 by 4*/
	HW2x2_MODE_SUB4_RT = 2,/**< HW2x2 Sub-Sample RT 1 by 4*/
	HW2x2_MODE_SUB4_LB = 3,/**< HW2x2 Sub-Sample LB 1 by 4*/
	HW2x2_MODE_SUB4_RB = 4,/**< HW2x2 Sub-Sample RB 1 by 4*/
	HW2x2_MODE_BINNING = 5,/**< HW2x2 Binning 1 by 4*/
	HW2x2_MODE_SUB9_X0Y0 = 6,/**< HW2x2 Sub-Sample X0Y0 1 by 9*/
	HW2x2_MODE_SUB9_X1Y0 = 7,/**< HW2x2 Sub-Sample X1Y0 1 by 9*/
	HW2x2_MODE_SUB9_X2Y0 = 8,/**< HW2x2 Sub-Sample X2Y0 1 by 9*/
	HW2x2_MODE_SUB9_X0Y1 = 9,/**< HW2x2 Sub-Sample X0Y1 1 by 9*/
	HW2x2_MODE_SUB9_X1Y1 = 10,/**< HW2x2 Sub-Sample X1Y1 1 by 9*/
	HW2x2_MODE_SUB9_X2Y1 = 11,/**< HW2x2 Sub-Sample X2Y1 1 by 9*/
	HW2x2_MODE_SUB9_X0Y2 = 12,/**< HW2x2 Sub-Sample X0Y2 1 by 9*/
	HW2x2_MODE_SUB9_X1Y2 = 13,/**< HW2x2 Sub-Sample X1Y2 1 by 9*/
	HW2x2_MODE_SUB9_X2Y2 = 14, /**< HW2x2 Sub-Sample X2Y2 1 by 9*/
} HW2x2_MODE_E;
/**
 * \enum HW2x2_ROUNDMODE_E
 * \brief HW2x2 Rounding Mode
 */
typedef enum {
	HW2x2_ROUNDMODE_FLOOR = 0,/**< HW2x2 Floor mode*/
	HW2x2_ROUNDMODE_ROUNDING = 1, /**< HW2x2 Rounding mode*/
} HW2x2_ROUNDMODE_E;
/** @} */

/**
 * \defgroup	HWACC_HW2X2_DRV_REGSTRUCT	HW2x2 Hardware accelerator Driver Structure
 * \ingroup	HWACC_HW2X2_DRV
 * \brief	contains definitions of HW2x2 Hardware accelerator Driver structure.
 * @{
 */
/**
 * \struct HW2x2_CFG_T
 * \brief HW2x2 configuration by HW2x2 Driver
 */
typedef struct {
	HW2x2_PATH_E hw2x2_path;/**< HW2x2 Through or Internal Bypass*/
	HW2x2_MODE_E hw_22_process_mode;/**< HW2x2 Subsample Mode*/
	HW2x2_ROUNDMODE_E hw_22_mono_round_mode;/**< HW2x2 Rounding Mode*/
	uint16_t hw_22_crop_stx; /**< HW2x2 ROI Start X Position*/
	uint16_t hw_22_crop_sty; /**< HW2x2 ROI Start Y Position*/
	uint16_t hw_22_in_width; /**< HW2x2 ROI Width*/
	uint16_t hw_22_in_height;/**< HW2x2 ROI height*/
} HW2x2_CFG_T;
/** @} */

/**
 * \defgroup	HWACC_HW2X2_DRV_MACRO	HW2x2 Hardware accelerator Driver MACRO
 * \ingroup	HWACC_HW2X2_DRV
 * \brief	contains definitions of HW2x2 Hardware accelerator Driver MACRO.
 * @{
 */
#define MAX_HW2x2_SUB2_CROP_WIDTH_CONSTRAINT 640	/**< HW2x2 Timing Constraint of subsample 1/4 max ROI input width*/
#define MAX_HW2x2_SUB2_CROP_HEIGHT_CONSTRAINT 480	/**< HW2x2 Timing Constraint of subsample 1/4 max ROI input height*/
#define MAX_HW2x2_SUB3_CROP_WIDTH_CONSTRAINT 639	/**< HW2x2 Timing Constraint of subsample 1/9 max ROI input width*/
#define MAX_HW2x2_SUB3_CROP_HEIGHT_CONSTRAINT 480	/**< HW2x2 Timing Constraint of subsample 1/9 max ROI input height*/
#define HW2x2_SUB2_DIVISIBLE  2	/**< HW2x2 Timing Constraint of subsample 1/4 width and hieght multiple*/
#define HW2x2_SUB3_DIVISIBLE  3	/**< HW2x2 Timing Constraint of subsample 1/9 width and hieght multiple*/
#define MIN_HW2x2_SUB2_CROP_WIDTH_CONSTRAINT 2	/**< HW2x2 Timing Constraint of subsample 1/4 min ROI input width*/
#define MIN_HW2x2_SUB2_CROP_HEIGHT_CONSTRAINT 2	/**< HW2x2 Timing Constraint of subsample 1/4 min ROI input height*/
#define MIN_HW2x2_SUB3_CROP_WIDTH_CONSTRAINT 3	/**< HW2x2 Timing Constraint of subsample 1/9 min ROI input width*/
#define MIN_HW2x2_SUB3_CROP_HEIGHT_CONSTRAINT 3	/**< HW2x2 Timing Constraint of subsample 1/9 min ROI input height*/
/** @} */

/**
 * \defgroup	HWACC_HW2X2_DRV_FUNCDLR	Hardware accelerator HW2x2 Driver Function Declaration
 * \ingroup	HWACC_HW2X2_DRV
 * \brief	Contains declarations of Hardware accelerator HW2x2 Driver functions.
 * @{
 */
/**
 * \brief	initial HW2x2 Driver
 *
 *\param[in]	reg_baseaddr	 HW2x2 reg base address
 * \return	HW2x2_ERROR_E.
 */
HW2x2_ERROR_E hx_drv_hw2x2_init(uint32_t reg_baseaddr);

/**
 * \brief	set all HW2x2 related Setting
 *
 * \param[in]	hw2x2_cfg	 HW2x2 configuration
 * \return	HW2x2_ERROR_E.
 */
HW2x2_ERROR_E hx_drv_hw2x2_set_allCfg(HW2x2_CFG_T hw2x2_cfg);

/**
 * \brief	get default HW2x2 related Setting
 *
 * \param[out]	hw2x2_cfg	 HW2x2 default configuration
 * \return	HW2x2_ERROR_E.
 */
HW2x2_ERROR_E hx_drv_hw2x2_get_defCfg(HW2x2_CFG_T *hw2x2_cfg);

/**
 * \brief	get HW2x2 available Width and Height to avoid HW Constraint
 *
 * \param[in]	scale	 HW2x2 Subsample Mode
 * \param[in]	in_width	 HW2x2 input width
 * \param[in]	in_height	 HW2x2 input height
 * \param[out]	crop_width	 HW2x2 crop width according to Timing Constraint
 * \param[out]	crop_height	 HW2x2 crop height according to Timing Constraint
 * \return	HW2x2_ERROR_E.
 */
HW2x2_ERROR_E hx_drv_hw2x2_get_available_HV(HW2x2_MODE_E scale,
		uint16_t in_width, uint16_t in_height, uint16_t *crop_width,
		uint16_t *crop_height);

/**
 * \brief	set HW2x2 enable selection
 *
 * \param[in]	path	 HW2x2 Through or Internal Bypass
 * \return	HW2x2_ERROR_E.
 */
HW2x2_ERROR_E hx_drv_hw2x2_set_path(HW2x2_PATH_E path);

/**
 * \brief	get HW2x2 enable selection
 *
 * \param[out]	path	 HW2x2 Through or Internal Bypass
 * \return	HW2x2_ERROR_E.
 */
HW2x2_ERROR_E hx_drv_hw2x2_get_path(HW2x2_PATH_E *path);

/**
 * \brief	set HW2x2 Subsmaple mode selection
 *
 * \param[in]	m_mode	 HW2x2 Subsample Mode
 * \return	HW2x2_ERROR_E.
 */
HW2x2_ERROR_E hx_drv_hw2x2_set_process(HW2x2_MODE_E m_mode);

/**
 * \brief	get HW2x2 Subsmaple mode selection
 *
 * \param[out]	m_mode	 HW2x2 Subsample Mode
 * \return	HW2x2_ERROR_E.
 */
HW2x2_ERROR_E hx_drv_hw2x2_get_process(HW2x2_MODE_E *m_mode);

/**
 * \brief	set HW2x2 round mode selection
 *
 * \param[in]	m_mode	 HW2x2 round Mode
 * \return	HW2x2_ERROR_E.
 */
HW2x2_ERROR_E hx_drv_hw2x2_set_round(HW2x2_ROUNDMODE_E m_mode);

/**
 * \brief	get HW2x2 round mode selection
 *
 * \param[out]	m_mode	 HW2x2 round Mode
 * \return	HW2x2_ERROR_E.
 */
HW2x2_ERROR_E hx_drv_hw2x2_get_round(HW2x2_ROUNDMODE_E *m_mode);

/**
 * \brief	set HW2x2 cropping related setting
 *
 * \param[in]	crop_stx	 HW2x2 Crop start x position
 * \param[in]	crop_sty	 HW2x2 Crop start y position
 * \param[in]	crop_width	 HW2x2 Crop width
 * \param[in]	crop_height	 HW2x2 Crop height
 * \return	HW2x2_ERROR_E.
 */
HW2x2_ERROR_E hx_drv_hw2x2_set_crop(uint16_t crop_stx, uint16_t crop_sty,
		uint16_t crop_width, uint16_t crop_height);

/**
 * \brief	get HW2x2 cropping related setting
 *
 * \param[out]	crop_stx	 HW2x2 Crop start x position
 * \param[out]	crop_sty	 HW2x2 Crop start y position
 * \param[out]	crop_width	 HW2x2 Crop width
 * \param[out]	crop_height	 HW2x2 Crop height
 * \return	HW2x2_ERROR_E.
 */
HW2x2_ERROR_E hx_drv_hw2x2_get_crop(uint16_t *crop_stx, uint16_t *crop_sty,
		uint16_t *crop_width, uint16_t *crop_height);

/*get HW2x2 output Memory Size for DMAC*/
/**
 * \brief	get HW2x2 output Memory Size for DMAC
 *
 * \param[in]	hw2x2_cfg	 HW2x2 Configuration
 * \param[out]	mem_size	 According to HW2x2 configuration, calculate memory size
 * \return	HW2x2_ERROR_E.
 */
HW2x2_ERROR_E hx_drv_hw2x2_get_OutMEMSize(HW2x2_CFG_T hw2x2_cfg,
		uint32_t *mem_size);
/** @} */
#endif
