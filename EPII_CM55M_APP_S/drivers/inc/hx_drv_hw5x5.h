#ifndef _HX_DRV_HW5X5_H_
#define _HX_DRV_HW5X5_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
/**
 * \defgroup	HWACC_HW5X5_DRV	HW5x5 Hardware accelerator Driver
 * \ingroup		HWACC_HW5X5_DRV
 * \brief	HW5x5 Hardware accelerator Driver Declaration
 * \details Application can use HW5x5 Structure and datapath library to control Hardware\n
 * <pre>
 * For Example
 * Ex1: HW5x5_CFG_T: All HW5x5 configuration for application to use datapath library control HW5x5
 * </pre>
 */
/**
 * \defgroup	HWACC_HW5X5_DRV_ENUM	HW5x5 Hardware accelerator Driver Enumeration
 * \ingroup	HWACC_HW5X5_DRV
 * \brief	Defines some enumeration of HW5x5 Hardware accelerator Driver need.
 * @{
 */
/**
 * \enum HW5x5_ERROR_E
 * \brief HW5x5 Errors Type
 */
typedef enum {
	HW5x5_NO_ERROR = 0, /**< No ERROR */
	HW5x5_ERROR_NOT_INIT = 1, /**< ERROR MSG: not init */
	HW5x5_ERROR_ALREADY_INIT = 2, /**< ERROR MSG: already init */
	HW5x5_ERROR_INVALID_PARAMETERS = 3, /**< ERROR MSG: Invalid parameters */
	HW5x5_ERROR_INVALID_TIMING_RANGE = 4, /**< ERROR MSG: Invalid TIMING Range */
	HW5x5_ERROR_INVALID_TIMING_DIVISIBLE = 5, /**< ERROR MSG: Invalid TIMING Divisble */
	HW5x5_UNKNOWN_ERROR = 6, /**< ERROR MSG: UNKNOWN ERROR*/
} HW5x5_ERROR_E;
/**
 * \enum HW5x5_PATH_E
 * \brief HW5x5 Path Selection
 */
typedef enum {
	HW5x5_PATH_BYPASS_5X5 = 0,/**< HW5x5 Internal Bypass*/
	HW5x5_PATH_THROUGH_DEMOSAIC = 1,/**< HW5x5 Demosaic Path*/
	HW5x5_PATH_THROUGH_FIR = 2,/**< HW5x5 FIR Path*/
	HW5x5_PATH_THROUGH_LPF = 3, /**< HW5x5 LPF Path*/
} HW5x5_PATH_E;
/**
 * \enum DEMOS_COLORMODE_E
 * \brief HW5x5 Demosaic output Color Selection
 */
typedef enum {
	DEMOS_COLORMODE_YUV422 = 0,/**< HW5x5 Demosaic output Color Mode YUV422*/
	DEMOS_COLORMODE_YUV420 = 1,/**< HW5x5 Demosaic output Color Mode YUV420*/
	DEMOS_COLORMODE_YUV400 = 2,/**< HW5x5 Demosaic output Color Mode YUV400*/
	DEMOS_COLORMODE_RGB = 3, /**< HW5x5 Demosaic output Color Mode RGB only for debug*/
} DEMOS_COLORMODE_E;

/**
 * \enum DEMOS_PATTENMODE_E
 * \brief Bayer Sensor layout Selection for HW5x5
 */
typedef enum {
	DEMOS_PATTENMODE_BGGR = 0,/**< Sensor Bayer Layout BGGR*/
	DEMOS_PATTENMODE_GBRG = 1,/**< Sensor Bayer Layout GBRG*/
	DEMOS_PATTENMODE_GRBG = 2,/**< Sensor Bayer Layout GRBG*/
	DEMOS_PATTENMODE_RGGB = 3, /**< Sensor Bayer Layout RGGB*/
} DEMOS_PATTENMODE_E;

/**
 * \enum DEMOS_BNDODE_E
 * \brief HW5x5 Demosaic Boundary process mode
 */
typedef enum {
	DEMOS_BNDODE_EXTEND0 = 0,/**< HW5x5 Demosaic boundary extend 0*/
	DEMOS_BNDODE_REFLECT = 1,/**< HW5x5 Demosaic boundary reflect*/
	DEMOS_BNDODE_INPUTEXTEND = 2, /**< HW5x5 Demosaic input with boundary extend*/
} DEMOS_BNDODE_E;

/**
 * \enum DEMOSLPF_ROUNDMODE_E
 * \brief HW5x5 Demosaic and LPF Rounding mode
 */
typedef enum {
	DEMOSLPF_ROUNDMODE_FLOOR = 0, /**< HW5x5 Demosaic and LPF floor*/
	DEMOSLPF_ROUNDMODE_ROUNDING = 1, /**< HW5x5 Demosaic and LPF rounding*/
} DEMOSLPF_ROUNDMODE_E;

/**
 * \enum FIR_PROCMODE_E
 * \brief HW5x5 FIR mode
 */
typedef enum {
	FIR_PROCMODE_LBP1 = 0, /**< HW5x5 LBP1*/
	FIR_PROCMODE_LBP2 = 1, /**< HW5x5 LBP2*/
} FIR_PROCMODE_E;

/**
 * \enum FIRLPF_BNDODE_E
 * \brief HW5x5 FIR and LPF boundary extend mode
 */
typedef enum {
	FIRLPF_BNDODE_EXTEND0 = 0, /**< HW5x5 FIR and LPF boundary extend 0*/
	FIRLPF_BNDODE_REPEAT = 1, /**< HW5x5 FIR and LPF boundary repeat*/
} FIRLPF_BNDODE_E;
/** @} */

/**
 * \defgroup	HWACC_HW5X5_DRV_REGSTRUCT	HW5x5 Hardware accelerator Driver Structure
 * \ingroup	HWACC_HW5X5_DRV
 * \brief	contains definitions of HW5x5 Hardware accelerator Driver structure.
 * @{
 */
/**
 * \struct HW5x5_CFG_T
 * \brief HW5x5 configuration by HW5x5 Driver
 */
typedef struct {
	HW5x5_PATH_E hw5x5_path;/**< HW5x5 Path Selection*/
	DEMOS_COLORMODE_E demos_color_mode;/**< HW5x5 Demosaic output Color Selection*/
	DEMOS_PATTENMODE_E demos_pattern_mode;/**< Bayer Sensor layout Selection for HW5x5*/
	DEMOS_BNDODE_E demos_bndmode;/**< HW5x5 Demosaic Boundary process mode*/
	DEMOSLPF_ROUNDMODE_E demoslpf_roundmode;/**< HW5x5 Demosaic and LPF Rounding mode*/

	FIR_PROCMODE_E fir_procmode;/**< HW5x5 FIR mode*/
	FIRLPF_BNDODE_E firlpf_bndmode;/**< HW5x5 FIR and LPF boundary extend mode*/
	int16_t fir_lbp_th;/**< HW5x5 FIR LBP Threshold*/
	uint16_t hw55_crop_stx;/**< HW5x5 ROI Start x-position*/
	uint16_t hw55_crop_sty;/**< HW5x5 ROI Start y-position*/
	uint16_t hw55_in_width;/**< HW5x5 ROI width*/
	uint16_t hw55_in_height;/**< HW5x5 ROI height*/
} HW5x5_CFG_T;
/** @} */

/**
 * \defgroup	HWACC_HW5X5_DRV_MACRO	HW5x5 Hardware accelerator Driver MACRO
 * \ingroup	HWACC_HW5X5_DRV
 * \brief	contains definitions of HW5x5 Hardware accelerator Driver MACRO.
 * @{
 */
/*Timing Constraint of input*/
#define MAX_HW5x5_CROP_WIDTH_CONSTRAINT 	644	/**< HW5x5 Timing Constraint of max ROI input width*/
#define MAX_HW5x5_CROP_HEIGHT_CONSTRAINT 	484/**< HW5x5 Timing Constraint of max ROI input height*/
#define HW5x5_WIDTH_DIVISIBLE  				16/**< HW5x5 with JPEG Timing Constraint of width multiple*/
#define HW5x5_HEIGHT_DIVISIBLE  		    16/**< HW5x5 with JPEG Timing Constraint of height multiple*/
#define HW5x5_FIR_WIDTH_DIVISIBLE  			8/**< HW5x5 FIR Timing Constraint of width multiple*/
#define HW5x5_FIR_HEIGHT_DIVISIBLE  		4/**< HW5x5 FIR Timing Constraint of height multiple*/
#define HW5x5_LPF_WIDTH_DIVISIBLE  			4/**< HW5x5 LPF Timing Constraint of width multiple*/
#define HW5x5_LPF_HEIGHT_DIVISIBLE  		4/**< HW5x5 LPF Timing Constraint of height multiple*/
#define MIN_HW5x5_CROP_WIDTH_CONSTRAINT 	8/**< HW5x5 Timing Constraint of min ROI input width*/
#define MIN_HW5x5_CROP_HEIGHT_CONSTRAINT 	8/**< HW5x5 Timing Constraint of min ROI input height*/
/** @} */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \defgroup	HWACC_HW5X5_DRV_FUNCDLR	Hardware accelerator HW5x5 Driver Function Declaration
 * \ingroup	HWACC_HW5X5_DRV
 * \brief	Contains declarations of Hardware accelerator HW5x5 Driver functions.
 * @{
 */
/**
 * \brief	initial HW5x5 Driver
 *
 * \param[in]	reg_baseaddr	 HW5x5 reg base address
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_init(uint32_t reg_baseaddr);

/**
 * \brief	set all HW5x5 related Setting
 *
 * \param[in]	hw5x5_cfg	 HW5x5 configuration
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_set_allCfg(HW5x5_CFG_T hw5x5_cfg);

/**
 * \brief	get default HW5x5 related Setting
 *
 * \param[out]	hw5x5_cfg	 HW5x5 default configuration
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_get_defCfg(HW5x5_CFG_T *hw5x5_cfg);

/**
 * \brief	get HW5x5 available Width and Height to avoid HW Constraint
 *
 * \param[in]	path	 HW5x5 path
 * \param[in]	bndmode	 HW5x5 boundary mode
 * \param[in]	in_width	 HW5x5 input width
 * \param[in]	in_height	 HW5x5 input height
 * \param[out]	crop_width	 HW5x5 ROI width according to setting
 * \param[out]	crop_height	 HW5x5 ROI height according to setting
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_get_available_HV(HW5x5_PATH_E path,
		DEMOS_BNDODE_E bndmode, uint16_t in_width, uint16_t in_height,
		uint16_t *crop_width, uint16_t *crop_height);

/**
 * \brief	set HW5x5 Demosaic filter path
 *
 * \param[in]	path	 HW5x5 Path Selection
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_set_path(HW5x5_PATH_E path);

/**
 * \brief	get HW5x5 Demosaic filter path
 *
 * \param[out]	path	 HW5x5 Path Selection
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_get_path(HW5x5_PATH_E *path);

/**
 * \brief	set HW5x5 Demosaic filter Color Format
 *
 * \param[in]	mode	 HW5x5 Demosaic output Color Selection
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_set_demosColor(DEMOS_COLORMODE_E mode);

/**
 * \brief	get HW5x5 Demosaic filter Color Format
 *
 * \param[out]	mode	 HW5x5 Demosaic output Color Selection
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_get_demosColor(DEMOS_COLORMODE_E *mode);

/**
 * \brief	set HW5x5 Demosaic filter input Pattern layout
 *
 * \param[in]	mode	 Bayer Sensor layout Selection for HW5x5
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_set_demosPattern(DEMOS_PATTENMODE_E mode);
/**
 * \brief	get HW5x5 Demosaic filter input Pattern layout
 *
 * \param[out]	mode	 Bayer Sensor layout Selection for HW5x5
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_get_demosPattern(DEMOS_PATTENMODE_E *mode);

/**
 * \brief	set HW5x5 Demosaic filter BND mode
 *
 * \param[in]	mode	 HW5x5 Demosaic Boundary process mode
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_set_demosBND(DEMOS_BNDODE_E mode);

/**
 * \brief	get HW5x5 Demosaic filter BND mode
 *
 * \param[out]	mode	 HW5x5 Demosaic Boundary process mode
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_get_demosBND(DEMOS_BNDODE_E *mode);

/**
 * \brief	set HW5x5 Demosaic or LPF filter rounding mode
 *
 * \param[in]	mode	 HW5x5 Demosaic and LPF Rounding mode
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_set_DemosLPFRND(DEMOSLPF_ROUNDMODE_E mode);
/**
 * \brief	get HW5x5 Demosaic or LPF filter rounding mode
 *
 * \param[out]	mode	 HW5x5 Demosaic and LPF Rounding mode
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_get_DemosLPFRND(DEMOSLPF_ROUNDMODE_E *mode);

/**
 * \brief	set HW5x5set HW5x5 FIR Process Mode LBP1 or LBP2
 *
 * \param[in]	mode	 HW5x5 FIR mode
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_set_firProc(FIR_PROCMODE_E mode);
/**
 * \brief	get HW5x5set HW5x5 FIR Process Mode LBP1 or LBP2
 *
 * \param[out]	mode	 HW5x5 FIR mode
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_get_firProc(FIR_PROCMODE_E *mode);

/**
 * \brief	set HW5x5 FIR or LPF boundary extend mode
 *
 * \param[in]	mode	HW5x5 FIR and LPF boundary extend mode
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_set_firLPFBND(FIRLPF_BNDODE_E mode);
/**
 * \brief	get HW5x5 FIR or LPF boundary extend mode
 *
 * \param[out]	mode	HW5x5 FIR and LPF boundary extend mode
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_get_firLPFBND(FIRLPF_BNDODE_E *mode);

/**
 * \brief	set HW5x5 threshold of LBP
 *
 * \param[in]	fir_lbp_th	HW5x5 LBP Threshold
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_set_firlbpTH(int16_t fir_lbp_th);

/**
 * \brief	set HW5x5 threshold of LBP
 *
 * \param[out]	fir_lbp_th	HW5x5 LBP Threshold
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_get_firlbpTH(int16_t *fir_lbp_th);

/**
 * \brief	set HW5x5 cropping related setting
 *
 * \param[in]	crop_stx	HW5x5 ROI Start x position
 * \param[in]	crop_sty	HW5x5 ROI Start y position
 * \param[in]	crop_width	HW5x5 ROI width
 * \param[in]	crop_height	HW5x5 ROI height
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_set_crop(uint16_t crop_stx, uint16_t crop_sty,
		uint16_t crop_width, uint16_t crop_height);

/**
 * \brief	get HW5x5 cropping related setting
 *
 * \param[out]	crop_stx	HW5x5 ROI Start x position
 * \param[out]	crop_sty	HW5x5 ROI Start y position
 * \param[out]	crop_width	HW5x5 ROI width
 * \param[out]	crop_height	HW5x5 ROI height
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_get_crop(uint16_t *crop_stx, uint16_t *crop_sty,
		uint16_t *crop_width, uint16_t *crop_height);
/*get HW5x5 output Memory Size for DMAC*/
/**
 * \brief	get HW5x5 output Memory Size for DMAC
 *
 * \param[in]	hw55_cfg	HW5x5 configuration
 * \param[out]	ch1_mem_size	HW5x5 output memory size for channel 1
 * \param[out]	ch2_mem_size	HW5x5 output memory size for channel 2
 * \param[out]	ch3_mem_size	HW5x5 output memory size for channel 3
 * \return	HW5x5_ERROR_E.
 */
HW5x5_ERROR_E hx_drv_hw5x5_get_OutMEMSize(HW5x5_CFG_T hw55_cfg,
		uint32_t *ch1_mem_size, uint32_t *ch2_mem_size, uint32_t *ch3_mem_size);
#ifdef __cplusplus
}
#endif
/** @} */

#endif
