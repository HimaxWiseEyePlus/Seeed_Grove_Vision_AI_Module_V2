/**
 ********************************************************************************************
 *  @file      hx_drv_cdm.h
 *  @details   This file contains cdm driver function
 *  @author    himax/902447
 *  @version   V1.0.0
 *  @date      14-May-2019
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/

#ifndef _HX_DRV_CDM_H_
#define _HX_DRV_CDM_H_

#include "dev_common.h"

/**
 * \defgroup	HWACC_CDM_DRV	CDM Hardware accelerator Driver
 * \ingroup		HWACC_CDM_DRV
 * \brief		CDM Hardware accelerator Driver Declaration
 * \details Application can use CDM driver API to control CDM IP by application\n
 * <pre>
 * For Example
 * Ex1: Control CDM init map on/off. if init map on, it will reset HL information
 * 		hx_drv_cdm_set_initmapFlag(CDM_INIMAP_FLAG_E flag);
 * Ex2: Let CDM know CPU will be active or Sleep.
 * 		hx_drv_cdm_set_CPUActiveFlag(CDM_CPU_ACTFLAG_E flag);
 * Ex3: CDM_CFG_T: All CDM configuration for application to use datapath library control CDM
 * </pre>
 */

/**
 * \defgroup	HWACC_CDM_DRV_ENUM	CDM Hardware accelerator Driver Enumeration
 * \ingroup	HWACC_CDM_DRV
 * \brief	Defines some enumeration of Hardware accelerator CDM Driver need.
 * @{
 */
/**
 * \enum CDM_ERROR_E
 * \brief CDM Errors Type
 */
typedef enum
{
	CDM_NO_ERROR                     	= 0,	/**< No ERROR */
	CDM_ERROR_NOT_INIT              	= 1,	/**< ERROR MSG: not init */
	CDM_ERROR_ALREADY_INIT     			= 2,	/**< ERROR MSG: Already init */
	CDM_ERROR_INVALID_PARAMETERS     	= 3,	/**< ERROR MSG: Invalid parameters */
    CDM_ERROR_INVALID_TIMING_RANGE     	= 4,	/**< ERROR MSG: Invalid TIMING Range */
    CDM_ERROR_INVALID_TIMING_DIVISIBLE  = 5,	/**< ERROR MSG: Invalid TIMING Divisble */
	CDM_UNKNOWN_ERROR      				= 6,	/**< ERROR MSG: UNKNOWN ERROR*/
} CDM_ERROR_E;

/**
 * \enum CDM_ENABLE_E
 * \brief enable change detection module(CDM)
 */
typedef enum
{
	CDM_ENABLE_OFF 			= 0,/**< turn-off CDM */
	CDM_ENABLE_ON 			= 1,/**< turn-on CDM */
} CDM_ENABLE_E;

/**
 * \enum CDM_INIMAP_FLAG_E
 * \brief initial frame map flag
 */
typedef enum
{
	CDM_INIMAP_FLAG_OFF 					= 0,/**< turn-off initial map flag */
	CDM_INIMAP_FLAG_ON 						= 1,/**< turn-on and keep initial map flag */
	CDM_INIMAP_FLAG_ON_ONEFRAME_ONLY		= 2,/**< turn-on initial map on 1st frame and trun off on 2nd frame */
} CDM_INIMAP_FLAG_E;

/**
 * \enum CDM_CPU_ACTFLAG_E
 * \brief cpu active flag
 */
typedef enum
{
	CDM_CPU_ACTFLAG_SLEEP 					= 0,/**< CPU Sleep */
	CDM_CPU_ACTFLAG_ACTIVE 					= 1,/**< CPU keep Active */
	CDM_CPU_ACTFLAG_ACTIVE_ONEFRAME_ONLY 	= 2,/**< CPU Active one frame then sleep*/
} CDM_CPU_ACTFLAG_E;


/**
 * \enum CDM_ENABLE_MATA_DUMP_E
 * \brief enable dump metadata to SRAM
 */
typedef enum
{
	CDM_ENABLE_MATA_DUMP_OFF 			= 0,/**< disable dump metadata to SRAM */
	CDM_ENABLE_MATA_DUMP_ON 			= 1,/**< enable dump metadata to SRAM */
} CDM_ENABLE_MATA_DUMP_E;

/**
 * \enum CDM_ENABLE_HT_PACKING_E
 * \brief enable packing eroded hot-pixel map
 */
typedef enum
{
	CDM_ENABLE_HT_PACKING_OFF 			= 0,/**< dump original eroded hot-pixel map (320x240x1bit) */
	CDM_ENABLE_HT_PACKING_ON 			= 1,/**< dump packing eroded hot-pixel map (40x240x1byte) */
} CDM_ENABLE_HT_PACKING_E;
/** @} */

/**
 * \defgroup	HWACC_CDM_DRV_REGSTRUCT	CDM Hardware accelerator Driver Structure
 * \ingroup	HWACC_CDM_DRV
 * \brief	contains definitions of CDM Hardware accelerator Driver structure.
 * @{
 */
/**
 * \struct CDM_CFG_T
 * \brief CDM configuration by CDM Driver
 */
typedef struct
{
	CDM_ENABLE_E  cdm_enable;				/*!< CDM enable */
	CDM_INIMAP_FLAG_E init_map_flag;		/*!< CDM initial map flag */
	CDM_CPU_ACTFLAG_E cpu_activeflag;		/*!< CDM cpu active flag */
	CDM_ENABLE_MATA_DUMP_E meta_dump;		/*!< CDM meta data dump enable or disable */
	CDM_ENABLE_HT_PACKING_E ht_packing;		/*!< CDM meta data dump packing or unpacking */
	uint8_t cdm_min_allow_dis;				/*!< CDM algorithm minimum allowed distance between pixel and threshold setting*/
	uint8_t cdm_tolerance;					/*!< CDM algorithm tolerance setting*/
	uint8_t cdm_reactance;					/*!< CDM algorithm reactance setting*/
	uint8_t cdm_relaxation;					/*!< CDM algorithm relaxation setting*/
	uint8_t cdm_eros_th;					/*!< CDM algorithm erosion threshold setting*/
	uint16_t cdm_num_ht_th;					/*!< CDM algorithm number of hot pixels threshold setting*/

	uint8_t cdm_num_ht_vect_th_x;			/*!< CDM algorithm threshold for number of hot-pixels in each bin of the x-projection vector setting*/
	uint8_t cdm_num_ht_vect_th_y;			/*!< CDM algorithm threshold for number of hot-pixels in each bin of the y-projection vector setting*/
	uint8_t cdm_num_cons_ht_bin_th_x;		/*!< CDM algorithm threshold for number of consecutive hot bins of the x-projection vector setting*/
	uint8_t cdm_num_cons_ht_bin_th_y;		/*!< CDM algorithm threshold for number of consecutive hot bins of the y-projection vector setting*/

	uint16_t cdm_crop_stx;	/*!< CDM input ROI start x position*/
	uint16_t cdm_crop_sty;	/*!< CDM input ROI start y position*/
	uint16_t cdm_in_width;	/*!< CDM input ROI width*/
	uint16_t cdm_in_height;	/*!< CDM input ROI height*/
} CDM_CFG_T;
/** @} */

/**
 * \defgroup	HWACC_CDM_DRV_MACRO	CDM Hardware accelerator Driver MACRO
 * \ingroup	HWACC_CDM_DRV
 * \brief	contains definitions of CDM Hardware accelerator Driver MACRO.
 * @{
 */
#define MAX_CDM_CROP_WIDTH_CONSTRAINT 	    320		/*!< Timing Constraint of input max width*/
#define MAX_CDM_CROP_HEIGHT_CONSTRAINT 		240		/*!< Timing Constraint of input max height*/
#define CDM_WIDTH_DIVISIBLE  				8		/*!< Timing Constraint of input width*/
#define CDM_HEIGHT_DIVISIBLE  				4		/*!< Timing Constraint of input height*/
#define MIN_CDM_CROP_WIDTH_CONSTRAINT 		8		/*!< Timing Constraint of input min width*/
#define MIN_CDM_CROP_HEIGHT_CONSTRAINT 		3		/*!< Timing Constraint of input min height*/
/** @} */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \defgroup	HWACC_CDM_DRV_FUNCDLR	CDM Hardware accelerator Driver Function Declaration
 * \ingroup	HWACC_CDM_DRV
 * \brief	Contains declarations of CDM Hardware accelerator Driver functions.
 * @{
 */
/**
 * \brief	initial CDM Driver
 *
 * \param[in]	reg_baseaddr	 CDM reg base address
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_init(uint32_t reg_baseaddr);

/**
 * \brief	set all CDM related Setting
 *
 * \param[in]	cdm_cfg	 CDM configuration
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_allCfg(CDM_CFG_T cdm_cfg);

/**
 * \brief	get default CDM related Setting
 *
 * \param[out]	cdm_cfg	default CDM configuration
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_defCfg(CDM_CFG_T *cdm_cfg);

/**
 * \brief	get CDM available Width and Height to avoid HW Constraint
 *
 * \param[in]	in_width		CDM input width
 * \param[in]	in_height		CDM input height
 * \param[out]	crop_width		CDM ROI width by timing constraint
 * \param[out]	crop_height	CDM ROI height by timing constraint
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_available_HV(uint16_t in_width,uint16_t in_height,uint16_t *crop_width,uint16_t *crop_height);


/**
 * \brief	set CDM enable selection
 *
 * \param[in]	path		CDM enable or disable
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_enable(CDM_ENABLE_E path);

/**
 * \brief	get CDM enable selection
 *
 * \param[out]	path		CDM enable or disable
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_enable(CDM_ENABLE_E *path);


/**
 * \brief	set CDM initial frame flag
 *
 * \param[in]	flag		CDM inital map flag settings
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_initmapFlag(CDM_INIMAP_FLAG_E flag);
/**
 * \brief	get CDM initial frame flag
 *
 * \param[out]	flag		CDM inital map flag settings
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_initmapFlag(CDM_INIMAP_FLAG_E *flag);


/**
 * \brief	set CDM cpu active flag
 *
 * \param[in]	flag		CDM cpu active flag settings
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_CPUActiveFlag(CDM_CPU_ACTFLAG_E flag);

/**
 * \brief	get CDM cpu active flag
 *
 * \param[out]	flag		CDM cpu active flag settings
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_CPUActiveFlag(CDM_CPU_ACTFLAG_E *flag);

/**
 * \brief	set CDM Meta Data Dump Enable
 *
 * \param[in]	meta_dump		CDM meta data dump enable/disable
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_MetaDumpEn(CDM_ENABLE_MATA_DUMP_E meta_dump);

/**
 * \brief	get CDM Meta Data Dump Enable
 *
 * \param[out]	meta_dump		CDM meta data dump enable/disable
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_MetaDumpEn(CDM_ENABLE_MATA_DUMP_E *meta_dump);

/**
 * \brief	set CDM enable packing eroded hot-pixel map
 *
 * \param[in]	ht_packing		CDM hot pixels packing/unpacking
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_HTPacking(CDM_ENABLE_HT_PACKING_E ht_packing);

/**
 * \brief	set CDM enable packing eroded hot-pixel map
 *
 * \param[out]	ht_packing		CDM hot pixels packing/unpacking
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_HTPacking(CDM_ENABLE_HT_PACKING_E *ht_packing);


/**
 * \brief	set CDM minimum allowed distance between pixel and threshold
 *
 * \param[in]	cdm_min_allow_dis		CDM minimum allowed distance between pixel and threshold
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_minAllowDis(uint8_t cdm_min_allow_dis);

/**
 * \brief	get CDM minimum allowed distance between pixel and threshold
 *
 * \param[out]	cdm_min_allow_dis		CDM minimum allowed distance between pixel and threshold
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_minAllowDis(uint8_t *cdm_min_allow_dis);

/**
 * \brief	set CDM tolerance
 *
 * \param[in]	cdm_tolerance		CDM tolerance
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_tolerance(uint8_t cdm_tolerance);

/**
 * \brief	get CDM tolerance
 *
 * \param[out]	cdm_tolerance		CDM tolerance
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_tolerance(uint8_t *cdm_tolerance);

/**
 * \brief	set CDM reactance
 *
 * \param[in]	cdm_reactance		CDM reactance
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_reactance(uint8_t cdm_reactance);

/**
 * \brief	get CDM reactance
 *
 * \param[out]	cdm_reactance		CDM reactance
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_reactance(uint8_t *cdm_reactance);


/**
 * \brief	set CDM relaxation
 *
 * \param[in]	cdm_relaxation		CDM relaxation
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_relaxation(uint8_t cdm_relaxation);

/**
 * \brief	get CDM relaxation
 *
 * \param[out]	cdm_relaxation		CDM relaxation
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_relaxation(uint8_t *cdm_relaxation);

/**
 * \brief	set CDM erosion threshold
 *
 * \param[in]	cdm_eros_th		erosion threshold
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_erosionTH(uint8_t cdm_eros_th);

/**
 * \brief	get CDM erosion threshold
 *
 * \param[out]	cdm_eros_th		CDM erosion threshold
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_erosionTH(uint8_t *cdm_eros_th);

/**
 * \brief	set CDM threshold number of hot-pixels
 *
 * \param[in]	cdm_num_ht_th	threshold number of hot-pixels
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_THNumOfHT(uint16_t cdm_num_ht_th);

/**
 * \brief	get CDM threshold number of hot-pixels
 *
 * \param[out]	cdm_num_ht_th	threshold number of hot-pixels
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_THNumOfHT(uint16_t *cdm_num_ht_th);

/**
 * \brief	set CDM threshold for number of hot-pixels in each bin of the x-projection vector (vector size: width x 1)
 *
 * \param[in]	cdm_num_ht_vect_th_x	threshold for number of hot-pixels in each bin of the x-projection vector
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_ht_vect_th_x(uint8_t cdm_num_ht_vect_th_x);

/**
 * \brief	get CDM threshold for number of hot-pixels in each bin of the x-projection vector (vector size: width x 1)
 *
 * \param[out]	cdm_num_ht_vect_th_x	threshold for number of hot-pixels in each bin of the x-projection vector
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_ht_vect_th_x(uint8_t *cdm_num_ht_vect_th_x);

/**
 * \brief	set CDM threshold for number of hot-pixels in each bin of the y-projection vector (vector size: height x 1)
 *
 * \param[in]	cdm_num_ht_vect_th_y	threshold for number of hot-pixels in each bin of the y-projection vector
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_ht_vect_th_y(uint8_t cdm_num_ht_vect_th_y);

/**
 * \brief	get CDM threshold for number of hot-pixels in each bin of the y-projection vector (vector size: height x 1)
 *
 * \param[out]	cdm_num_ht_vect_th_y	threshold for number of hot-pixels in each bin of the y-projection vector
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_ht_vect_th_y(uint8_t *cdm_num_ht_vect_th_y);

/**
 * \brief	set CDM threshold for number of consecutive hot bins of the x-projection vector (vector size: width x 1)
 *
 * \param[in]	cdm_num_cons_ht_bin_th_x	threshold for number of consecutive hot bins of the x-projection vector (vector size: width x 1)
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_cons_ht_bin_th_x(uint8_t cdm_num_cons_ht_bin_th_x);
/**
 * \brief	get CDM threshold for number of consecutive hot bins of the x-projection vector (vector size: width x 1)
 *
 * \param[out]	cdm_num_cons_ht_bin_th_x	threshold for number of consecutive hot bins of the x-projection vector
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_cons_ht_bin_th_x(uint8_t *cdm_num_cons_ht_bin_th_x);

/**
 * \brief	set CDM threshold for number of consecutive hot bins of the y-projection vector (vector size: height x 1)
 *
 * \param[in]	cdm_num_cons_ht_bin_th_y	threshold for number of consecutive hot bins of the y-projection vector
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_cons_ht_bin_th_y(uint8_t cdm_num_cons_ht_bin_th_y);

/**
 * \brief	get CDM threshold for number of consecutive hot bins of the y-projection vector (vector size: height x 1)
 *
 * \param[out]	cdm_num_cons_ht_bin_th_y	threshold for number of consecutive hot bins of the y-projection vector
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_cons_ht_bin_th_y(uint8_t *cdm_num_cons_ht_bin_th_y);


/**
 * \brief	set CDM croping related setting
 *
 * \param[in]	crop_stx	ROI start x position
 * \param[in]	crop_sty	ROI start y position
 * \param[in]	crop_width	ROI width
 * \param[in]	crop_height	ROI height
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_crop(uint16_t crop_stx,uint16_t crop_sty, uint16_t crop_width, uint16_t crop_height);

/**
 * \brief	get CDM croping related setting
 *
 * \param[out]	crop_stx	ROI start x position
 * \param[out]	crop_sty	ROI start y position
 * \param[out]	crop_width	ROI width
 * \param[out]	crop_height	ROI height
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_crop(uint16_t *crop_stx,uint16_t *crop_sty, uint16_t *crop_width, uint16_t *crop_height);


/**
 * \brief	get CDM output Memory Size for DMAC
 *
 * \param[in]	cdm_cfg	 CDM configuration
 * \param[out]	HP_Meta_size	Hotpixel and mata data memory size
 * \param[out]	high_mem_size	High threshold memory size
 * \param[out]	low_mem_size	Low threshold memory size
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_get_OutMEMSize(CDM_CFG_T cdm_cfg, uint32_t *HP_Meta_size, uint32_t *high_mem_size, uint32_t *low_mem_size);

/**
 * \brief	set CDM clear MD Interrupt
 *
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_clear_int_md();

/**
 * \brief	set CDM clear timing illegal interrupt
 *
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_clear_int_ckt();

/**
 * \brief	set CDM FIFO ready signal Threshold
 *
 * \param[in]	cdm_fifo_minus_upper_th	FIFO ready signal upper threshold (uth)= (reg_emza_cdm_in_width-reg_emza_cdm_fifo_minus_upper_th)
 * \param[in]	cdm_fifo_minus_lower_th	FIFO ready signal lower threshold (lth)= (reg_emza_cdm_in_width-reg_emza_cdm_fifo_minus_lower_th)
 * \return	CDM_ERROR_E.
 */
CDM_ERROR_E hx_drv_cdm_set_fifoTH(uint8_t cdm_fifo_minus_upper_th, uint8_t cdm_fifo_minus_lower_th);

/** @} */
#ifdef __cplusplus
}
#endif
#endif
