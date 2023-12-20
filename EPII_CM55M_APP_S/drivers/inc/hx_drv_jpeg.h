/*
 * hx_drv_jpeg.h
 *
 *  Created on: 2018¦~11¤ë15¤é
 *      Author: 902447
 */

#ifndef INC_HX_DRV_JPEG_H_
#define INC_HX_DRV_JPEG_H_

#include "dev_common.h"

/**
 * \defgroup	HWACC_JPEG_DRV	JPEG Hardware accelerator Driver
 * \ingroup		HWACC_JPEG_DRV
 * \brief	JPEG Hardware accelerator Driver Declaration
 * \details Application can use JPEG driver API to control JPEG IP by application\n
 * <pre>
 * For Example
 * Ex1: get JPEG frame File Size from another memory
 * 		hx_drv_jpeg_get_FillFileSizeToMem(uint8_t frame_no, uint32_t start_addr, uint32_t *size);
 * Ex2: get JPEG Frame Memory address in cyclic buffer
 * 		hx_drv_jpeg_get_MemAddrByFrameNo(uint8_t frame_no, uint32_t wdma2_start_addr, uint32_t *frame_start_addr);
 * Ex3: JPEG_CFG_T: All JPEG configuration for application to use datapath library control JPEG
 * </pre>
 */
/**
 * \defgroup	HWACC_JPEG_DRV_ENUM	JPEG Driver Enumeration
 * \ingroup	HWACC_JPEG_DRV
 * \brief	Defines some enumeration of JPEG Driver need.
 * @{
 */
/**
 * \enum JPEG_ERROR_E
 * \brief JPEG Errors Type
 */
typedef enum
{
	JPEG_NO_ERROR                     		= 0,	/**< No ERROR */
	JPEG_ERROR_NOT_INIT              	    = 1,	/**< ERROR MSG: not init */
	JPEG_ERROR_ALREADY_INIT                	= 2,	/**< ERROR MSG: already init */
	JPEG_ERROR_INVALID_PARAMETERS     		= 3,	/**< ERROR MSG: Invalid parameters */
	JPEG_ERROR_INVALID_TIMING_RANGE     	= 4,	/**< ERROR MSG: Invalid TIMING Range */
	JPEG_ERROR_INVALID_TIMING_DIVISIBLE     = 5,	/**< ERROR MSG: Invalid TIMING Divisble */
	JPEG_UNKNOWN_ERROR      				= 6,	/**< ERROR MSG: UNKNOWN ERROR*/
} JPEG_ERROR_E;

/**
 * \enum JPEG_PATH_E
 * \brief JPEG Path Selection
 */
typedef enum
{
	JPEG_PATH_DISABLE 	    	= 0,/**< 00: Disable. */
	JPEG_PATH_ENCODER_EN		= 1,/**< 01: Encoder Enable. */
	JPEG_PATH_DECODER_EN		= 2,/**< 10: Decoder Enable. */
}JPEG_PATH_E;

/**
 * \enum JPEG_ENC_TYPE_T
 * \brief JPEG Encoding Color format Selection
 */
typedef enum
{
	JPEG_ENC_TYPE_YUV420 	    = 0,/**< Encoding Color format YUV420. */
	JPEG_ENC_TYPE_YUV400		= 1,/**< Encoding Color format YUV400. */
	JPEG_ENC_TYPE_YUV422		= 2,/**< Encoding Color format YUV422. */
}JPEG_ENC_TYPE_T;

/**
 * \enum JPEG_ENC_QTABLE_T
 * \brief JPEG Encoding quantization table Selection
 */
typedef enum
{
	JPEG_ENC_QTABLE_4X 	    = 0,/**< 4x quantization table*/
	JPEG_ENC_QTABLE_10X		= 1,/**< 10x quantization table*/
}JPEG_ENC_QTABLE_T;

/**
 * \enum JPEG_DEC_TYPE_T
 * \brief JPEG Decoding color format information
 */
typedef enum
{
	JPEG_DEC_TYPE_YUV420 	    = 0,/**< JPEG Decoding Color format YUV420*/
	JPEG_DEC_TYPE_YUV400		= 1,/**< JPEG Decoding Color format YUV400*/
	JPEG_DEC_TYPE_YUV422		= 2,/**< JPEG Decoding Color format YUV422*/
}JPEG_DEC_TYPE_T;

/**
 * \enum JPEG_DEC_QTABLE_T
 * \brief JPEG Decoding quantization table information
 */
typedef enum
{
	JPEG_DEC_QTABLE_4X 	    = 0,/**< JPEG Decoding 4x quantization table*/
	JPEG_DEC_QTABLE_10X		= 1,/**< JPEG Decoding 10x quantization table*/
}JPEG_DEC_QTABLE_T;
/** @} */

/**
 * \defgroup	HWACC_JPEG_DRV_REGSTRUCT	JPEG Hardware accelerator Driver Structure
 * \ingroup	HWACC_JPEG_DRV
 * \brief	contains definitions of JPEG Hardware accelerator Driver structure.
 * @{
 */
/**
 * \struct JPEG_CFG_T
 * \brief JPEG configuration by JPEG Driver
 */
typedef struct
{
	JPEG_PATH_E jpeg_path;/**< JPEG Path Selection*/
	uint16_t enc_width; /**< JPEG encoding width(divide by 16)*/
	uint16_t enc_height; /**< JPEG encoding height(divide by 16)*/
	JPEG_ENC_TYPE_T jpeg_enctype;/**< JPEG Encoding Color format Selection*/
	JPEG_ENC_QTABLE_T jpeg_encqtable;/**< JPEG Encoding quantization table Selection*/
	uint16_t dec_roi_stx; /**< JPEG Decoding ROI Start x-position(divide by 16)*/
	uint16_t dec_roi_sty; /**< JPEG Decoding ROI Start y-position(divide by 16)*/
	uint16_t dec_width; /**< JPEG Decoding ROI width(divide by 16)*/
	uint16_t dec_height; /**< JPEG Decoding ROI height(divide by 16)*/// divide by 16
} JPEG_CFG_T;
/** @} */

/**
 * \defgroup	HWACC_JPEG_DRV_MACRO	JPEG Hardware accelerator Driver MACRO
 * \ingroup	HWACC_JPEG_DRV
 * \brief	contains definitions of JPEG Hardware accelerator Driver MACRO.
 * @{
 */
#define MAX_JPEG_WIDTH_CONSTRAINT 		1023	/**< JPEG Timing Constraint of max input width*/
#define MAX_JPEG_HEIGHT_CONSTRAINT 		1023	/**< JPEG Timing Constraint of max input height*/
#define JPEG_WIDTH_DIVISIBLE  			16		/**< JPEG Timing Constraint of input width multiple*/
#define JPEG_HEIGHT_DIVISIBLE  			16		/**< JPEG Timing Constraint of input height multiple*/
#define JPEG_XPOS_DIVISIBLE  			16		/**< JPEG Timing Constraint of decoder ROI x-position multiple*/
#define JPEG_YPOS_DIVISIBLE  			16		/**< JPEG Timing Constraint of decoder ROI y-position multiple*/
#define MIN_JPEG_WIDTH_CONSTRAINT 		16		/**< JPEG Timing Constraint of min input width*/
#define MIN_JPEG_HEIGHT_CONSTRAINT 		16		/**< JPEG Timing Constraint of min input height*/
/** @} */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \defgroup	HWACC_JPEG_DRV_FUNCDLR	JPEG Hardware accelerator Driver Function Declaration
 * \ingroup	HWACC_JPEG_DRV
 * \brief	Contains declarations of JPEG Hardware accelerator Driver functions.
 * @{
 */
/**
 * \brief	initial JPEG Driver
 *
 * \param[in]	reg_baseaddr	 	JPEG reg base address
 * \param[in]	reg_hsc_baseaddr	JPEG HSC reg base address
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_init(uint32_t reg_baseaddr, uint32_t reg_hsc_baseaddr);

/**
 * \brief	set all JPEG related Setting
 *
 * \param[in]	jpeg_cfg	 JPEG configuration
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_set_allCfg(JPEG_CFG_T jpeg_cfg);

/**
 * \brief	get all JPEG related Setting
 *
 * \param[out]	jpeg_cfg	 JPEG default configuration
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_defCfg(JPEG_CFG_T *jpeg_cfg);

/**
 * \brief	set JPEG enable selection
 *
 * \param[in]	path	 JPEG Path Selection
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_set_path(JPEG_PATH_E path);

/**
 * \brief	get JPEG enable selection
 *
 * \param[out]	path	 JPEG Path Selection
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_path(JPEG_PATH_E *path);

/**
 * \brief	set JPEG Encoder Width and Height selection
 *
 * \param[in]	width	 JPEG Encoder Width
 * \param[in]	height	 JPEG Encoder Height
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_set_encWH(uint16_t width, uint16_t height);

/**
 * \brief	get JPEG Encoder Width and Height selection
 *
 * \param[out]	width	 JPEG Encoder Width
 * \param[out]	height	 JPEG Encoder Height
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_encWH(uint16_t *width, uint16_t *height);

/**
 * \brief	set JPEG Encoder Color Format selection
 *
 * \param[in]	type	 JPEG Encoding Color format Selection
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_set_encFormat(JPEG_ENC_TYPE_T type);

/**
 * \brief	get JPEG Encoder Color Format selection
 *
 * \param[out]	type	 JPEG Encoding Color format Selection
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_encFormat(JPEG_ENC_TYPE_T *type);


/**
 * \brief	set JPEG Encoder Q Table selection
 *
 * \param[in]	table	 JPEG Encoding quantization table Selection
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_set_encQTable(JPEG_ENC_QTABLE_T table);
/**
 * \brief	get JPEG Encoder Q Table selection
 *
 * \param[out]	table	 JPEG Encoding quantization table Selection
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_encQTable(JPEG_ENC_QTABLE_T *table);

/**
 * \brief	set JPEG Decoder ROI selection
 *
 * \param[in]	stx	 JPEG Decoder start x-position
 * \param[in]	sty	 JPEG Decoder start x-position
 * \param[in]	width	 JPEG Decoder ROI width
 * \param[in]	height	 JPEG Decoder ROI height
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_set_decROI(uint16_t stx, uint16_t sty, uint16_t width, uint16_t height);

/**
 * \brief	get JPEG Decoder ROI selection
 *
 * \param[out]	stx	 JPEG Decoder start x-position
 * \param[out]	sty	 JPEG Decoder start x-position
 * \param[out]	width	 JPEG Decoder ROI width
 * \param[out]	height	 JPEG Decoder ROI height
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_decROI(uint16_t *stx, uint16_t *sty, uint16_t *width, uint16_t *height);

/**
 * \brief	get JPEG Decoder Header Width and Height Information
 *
 * \param[out]	width	 JPEG Decoder Header Width information
 * \param[out]	height	 JPEG Decoder Header height information
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_decHeaderWH(uint16_t *width, uint16_t *height);
/**
 * \brief	get JPEG Decoder Header Sample Type Information
 *
 * \param[out]	type	 JPEG Decoding color format information
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_decHeaderFormat(JPEG_DEC_TYPE_T *type);

/**
 * \brief	get JPEG Decoder Header Q Table Information
 *
 * \param[out]	table	 JPEG Decoding quantization table information
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_decHeaderQtable(JPEG_DEC_QTABLE_T *table);

/**
 * \brief	get JPEG Encoder output Max Memory Size for DMAC
 *
 * \param[in]	jpeg_cfg	 JPEG Configuration
 * \param[out]	ch0_mem_size	 JPEG Encoder out max memory size
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_EncOutMaxMEMSize(JPEG_CFG_T jpeg_cfg, uint32_t *ch0_mem_size);

/**
 * \brief	get JPEG Decoder output Max Memory Size for DMAC
 *
 * \param[in]	jpeg_cfg	 JPEG Configuration
 * \param[out]	ch1_mem_size	 JPEG Decoder channel 1 out max memory size
 * \param[out]	ch2_mem_size	 JPEG Decoder channel 2 out max memory size
 * \param[out]	ch3_mem_size	 JPEG Decoder channel 3 out max memory size
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_DecOutMaxMEMSize(JPEG_CFG_T jpeg_cfg, uint32_t *ch1_mem_size, uint32_t *ch2_mem_size, uint32_t *ch3_mem_size);

/**
 * \brief	get JPEG Encoder output Real Memory Size for DMAC from JPEG IP
 *
 * \param[out]	mem_size	 JPEG Encoder real output memory size
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_EncOutRealMEMSize(uint32_t *mem_size);

/**
 * \brief	set JPEG Encoder output File Size To Mem by Software(For Sensor case, Hardware will auto fill)
 *
 * \param[in]	frame_no	 JPEG Encoder frame number
 * \param[in]	start_addr	 JPEG Encoder fill jpeg file size to another memory to record JPEG buffer JPEG size
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_set_FillFileSizeToMem(uint8_t frame_no, uint32_t start_addr);


/**
 * \brief	get JPEG frame File Size from another memory
 *
 * \param[in]	frame_no	 JPEG Encoder frame number(start from 0)
 * \param[in]	start_addr	 JPEG Encoder fill jpeg file size to another memory to record JPEG buffer JPEG size
 * \param[out]	size	 JPEG Encoder file size for jpeg image
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_FillFileSizeToMem(uint8_t frame_no, uint32_t start_addr, uint32_t *size);

/**
 * \brief	get JPEG Frame Memory address in cyclic buffer
 *
 * \param[in]	frame_no	 JPEG Encoder frame number(start from 0)
 * \param[in]	wdma2_start_addr	 WDMA2 base address for JPEG encoder
 * \param[out]	frame_start_addr	 JPEG encoder image output start address in cyclic buffer
 * \return	JPEG_ERROR_E.
 */
JPEG_ERROR_E hx_drv_jpeg_get_MemAddrByFrameNo(uint8_t frame_no, uint32_t wdma2_start_addr, uint32_t *frame_start_addr);

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* INC_HX_DRV_JPEG_H_ */
