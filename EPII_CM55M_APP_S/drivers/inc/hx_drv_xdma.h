/*
 * hx_drv_xdma.h
 *
 *  Created on: 2018¦~11¤ë7¤é
 *      Author: 902447
 */

#ifndef SYNOPSYS_PA8535_EM9D_DFSS_DEFAULT_PLATFORM_INC_HX_DRV_XDMA_H_
#define SYNOPSYS_PA8535_EM9D_DFSS_DEFAULT_PLATFORM_INC_HX_DRV_XDMA_H_

#include "dev_common.h"
/**
 * \defgroup	XDMA_DRV	xDMA-WDMA1,2,3 and RDMA Driver
 * \ingroup		XDMA_DRV
 * \brief	xDMA-WDMA1,2,3 and RDMA  Driver Declaration
 * \details Application can use xDMA driver API to get Cyclic buffer related information\n
 * <pre>
 * Please notify if you use sensordplib_start_swreset(); or sensordplib_stop_swreset_WoSensorCtrl();, the following API information are reset
 * For Example
 * Ex1: get xDMA WDMA2 loop flag (1: cyclic buffer overwrite, 0: cyclic buffer not overwrite)
 * 		hx_drv_xdma_get_WDMA2_loopflag(uint8_t *loop_flag);
 * Ex2: get xDMA WDMA2 Cyclic mode next Frame buffer index
 * 		hx_drv_xdma_get_WDMA2NextFrameIdx(uint8_t *number);
 * Ex3: get xDMA WDMA2 get total cyclic buffer number (register + 1), EX: 10 Cyclic Slot, output parameter is 10
 * 		hx_drv_xdma_get_WDMA2_bufferNo(uint8_t high);
 * Ex4: get xDMA WDMA2 Cyclic mode First frame already capture or not
 * 		hx_drv_xdma_get_WDMA2FirstFrameCapflag(uint8_t *firstframe_cap);
 * </pre>
 */
/**
 * \defgroup	XDMA_DRV_ENUM	xDMA-WDMA1,2,3 and RDMA Driver Enumeration
 * \ingroup	XDMA_DRV
 * \brief	Defines some enumeration of xDMA-WDMA1,2,3 and RDMA Driver need.
 * @{
 */
/**
 * \enum XDMA_ERROR_E
 * \brief xDMA Errors Type
 */
typedef enum
{
	XDMA_NO_ERROR                     		= 0,	/**< No ERROR */
	XDMA_ERROR_NOT_INIT              	    = 1,	/**< ERROR MSG: not init */
	XDMA_ERROR_ALREADY_INIT                 = 2,	/**< ERROR MSG: already init */
	XDMA_ERROR_INVALID_PARAMETERS     		= 3,	/**< ERROR MSG: Invalid parameters */
	XDMA_UNKNOWN_ERROR      				= 4,	/**< ERROR MSG: UNKNOWN ERROR*/
} XDMA_ERROR_E;

/**
 * \enum XDMA_WDMA1INT_E
 * \brief xDMA WDMA1 Interrupt bit offset
 */
typedef enum
{
	XDMA_WDMA1INT_NORMAL_FINISH  				= 0,	/**< [0] : normal, data count reach THR */
	XDMA_WDMA1INT_ERR_FE_COUNT_NOT_REACH   		= 1,	/**< [1] : abnormal, Frame end but data count not reach THR */
	XDMA_WDMA1INT_ERR_DIS_BEFORE_FINISH         = 2,	/**< [2] : abnormal, DMA disable before finish */
	XDMA_WDMA1INT_ERR_CH1_FIFO_MISMATCH         = 3,	/**< [3] : abnormal FIFO error, Channel 1 Pointer mismatch*/
	XDMA_WDMA1INT_ERR_CH2_FIFO_MISMATCH	  	    = 4,	/**< [4] : abnormal FIFO error, Channel 2 Pointer mismatch*/
	XDMA_WDMA1INT_ERR_CH3_FIFO_MISMATCH	   	    = 5,	/**< [5] : abnormal FIFO error, Channel 3 Pointer mismatch*/
	XDMA_WDMA1INT_ERR_FIFO_CH1_OVERFLOW	  	    = 6,	/**< [6] : abnormal FIFO error, Channel 1 overflow*/
	XDMA_WDMA1INT_ERR_FIFO_CH2_OVERFLOW	  	    = 7,	/**< [7] : abnormal FIFO error, Channel 2 overflow*/
	XDMA_WDMA1INT_ERR_FIFO_CH3_OVERFLOW	  	    = 8,	/**< [8] : abnormal FIFO error, Channel 3 overflow*/
	XDMA_WDMA1INT_ERR_BUS	  	    		    = 9,	/**< [9] : abnormal BUS error,*/
} XDMA_WDMA1INT_E;

/**
 * \enum XDMA_WDMA2INT_E
 * \brief xDMA WDMA2 Interrupt bit offset
 */
typedef enum
{
	XDMA_WDMA2INT_NORMAL_FINISH           	  = 0,	/**< [0] : normal, data count reach THR */
	XDMA_WDMA2INT_ERR_FE_COUNT_NOT_REACH      = 1,	/**< [1] : abnormal, Frame end but data count not reach THR */
	XDMA_WDMA2INT_ERR_DIS_BEFORE_FINISH        = 2,	/**< [2] : abnormal, DMA disable before finish */
	XDMA_WDMA2INT_ERR_FIFO_MISMATCH            = 3,	/**< [3] : abnormal FIFO error, Pointer mismatch,*/
	XDMA_WDMA2INT_ERR_FIFO_OVERFLOW	  	       = 4,	/**< [4] : abnormal FIFO error, overflow*/
	XDMA_WDMA2INT_ERR_BUS	  	    		   = 5,	/**< [5] : abnormal BUS error,*/
} XDMA_WDMA2INT_E;

/**
 * \enum XDMA_WDMA3INT_E
 * \brief xDMA WDMA3 Interrupt bit offset
 */
typedef enum
{
	XDMA_WDMA3INT_NORMAL_FINISH                = 0,	/**< [0] : normal, data count reach THR */
	XDMA_WDMA3INT_ERR_FE_COUNT_NOT_REACH       = 1,	/**< [1] : abnormal, Frame end but data count not reach THR */
	XDMA_WDMA3INT_ERR_DIS_BEFORE_FINISH        = 2,	/**< [2] : abnormal, DMA disable before finish */
	XDMA_WDMA3INT_ERR_FIFO_CH1_MISMATCH        = 3,	/**< [3] : abnormal FIFO error, Channel 1 Pointer mismatch*/
	XDMA_WDMA3INT_ERR_FIFO_CH2_MISMATCH	  	   = 4,	/**< [4] : abnormal FIFO error, Channel 2 Pointer mismatch*/
	XDMA_WDMA3INT_ERR_FIFO_CH3_MISMATCH	  	   = 5,	/**< [5] : abnormal FIFO error, Channel 3 Pointer mismatch*/
	XDMA_WDMA3INT_ERR_FIFO_CH1_OVERFLOW	  	   = 6,	/**< [6] : abnormal FIFO error, Channel 1 overflow*/
	XDMA_WDMA3INT_ERR_FIFO_CH2_OVERFLOW	  	   = 7,	/**< [7] : abnormal FIFO error, Channel 2 overflow*/
	XDMA_WDMA3INT_ERR_FIFO_CH3_OVERFLOW	  	   = 8,	/**< [8] : abnormal FIFO error, Channel 2 overflow*/
	XDMA_WDMA3INT_ERR_BUS	  	    		   = 9,	/**< [9] : abnormal BUS error*/
} XDMA_WDMA3INT_E;

/**
 * \enum XDMA_RDMAINT_E
 * \brief xDMA RDMA Interrupt bit offset
 */
typedef enum
{
	XDMA_RDMAINT_NORMAL_FINISH           = 0,	/**< [0] : normal, finish */
	XDMA_RDMAINT_ERR_DIS_BEFORE_FINISH   = 1,	/**< [1] : abnormal, DMA Enable release before finish*/
	XDMA_RDMAINT_ERR_BUS	  	    	 = 2,	/**< [2] : HREADY but HRESP not be OKAY(2¡¦b00)*/
} XDMA_RDMAINT_E;

/**
 * \enum XDMA_INTMASK_E
 * \brief xDMA Interrupt Mask
 */
typedef enum
{
	XDMA_INTMASK_OFF       = 0,	/*Not Mask Interrupt*/
	XDMA_INTMASK_ON        = 1,	/*Mask Interrupt*/
} XDMA_INTMASK_E;

/**
 * \enum XDMA_WDMA1STATUS_E
 * \brief xDMA WDMA1 Interrupt Status
 */
typedef enum
{
	XDMA_WDMA1STATUS_NO				        		= 0,	/**< No */
	XDMA_WDMA1STATUS_NORMAL_FINISH  				= 0x1,	/**< [0] : normal, data count reach THR */
	XDMA_WDMA1STATUS_ERR_FE_COUNT_NOT_REACH   		= 0x2,	/**< [1] : abnormal, Frame end but data count not reach THR */
	XDMA_WDMA1STATUS_ERR_DIS_BEFORE_FINISH         = 0x4,	/**< [2] : abnormal, DMA disable before finish */
	XDMA_WDMA1STATUS_ERR_FIFO_CH1_MISMATCH         = 0x8,	/**< [3] : abnormal FIFO error, Channel 1 Pointer mismatch*/
	XDMA_WDMA1STATUS_ERR_FIFO_CH2_MISMATCH	  	    = 0x10,	/**< [4] : abnormal FIFO error, Channel 2 Pointer mismatch*/
	XDMA_WDMA1STATUS_ERR_FIFO_CH3_MISMATCH	   	    = 0x20,	/**< [5] : abnormal FIFO error, Channel 3 Pointer mismatch*/
	XDMA_WDMA1STATUS_ERR_FIFO_CH1_OVERFLOW	  	    = 0x40,	/**< [6] : abnormal FIFO error, Channel 1 overflow*/
	XDMA_WDMA1STATUS_ERR_FIFO_CH2_OVERFLOW	  	    = 0x80,	/**< [7] : abnormal FIFO error, Channel 2 overflow*/
	XDMA_WDMA1STATUS_ERR_FIFO_CH3_OVERFLOW	  	    = 0x100,	/**< [8] : abnormal FIFO error, Channel 3 overflow*/
	XDMA_WDMA1STATUS_ERR_BUS	  	    		    = 0x200,	/**< [9] : abnormal BUS error,*/
} XDMA_WDMA1STATUS_E;

/**
 * \enum XDMA_WDMA2STATUS_E
 * \brief xDMA WDMA2 Interrupt Status
 */
typedef enum
{
	XDMA_WDMA2STATUS_NO				         	 = 0,		/**< No */
	XDMA_WDMA2STATUS_NORMAL_FINISH           	 = 0x1,		/**< [0] : normal, data count reach THR */
	XDMA_WDMA2STATUS_ERR_FE_COUNT_NOT_REACH      = 0x2,		/**< [1] : abnormal, Frame end but data count not reach THR */
	XDMA_WDMA2STATUS_ERR_DIS_BEFORE_FINISH       = 0x4,		/**< [2] : abnormal, DMA disable before finish */
	XDMA_WDMA2STATUS_ERR_FIFO_MISMATCH           = 0x8,		/**< [3] : abnormal FIFO error, Pointer mismatch,*/
	XDMA_WDMA2STATUS_ERR_FIFO_OVERFLOW	  	     = 0x10,	/**< [4] : abnormal FIFO error, overflow*/
	XDMA_WDMA2STATUS_ERR_BUS	  	    		 = 0x20,	/**< [5] : abnormal BUS error,*/
} XDMA_WDMA2STATUS_E;

/**
 * \enum XDMA_WDMA3STATUS_E
 * \brief xDMA WDMA3 Interrupt Status
 */
typedef enum
{
	XDMA_WDMA3STATUS_NO						        = 0x0,   /**< No */
	XDMA_WDMA3STATUS_NORMAL_FINISH           		= 0x1,	/**< [0] : normal, data count reach THR */
	XDMA_WDMA3STATUS_ERR_FE_COUNT_NOT_REACH         = 0x2,	/**< [1] : abnormal, Frame end but data count not reach THR */
	XDMA_WDMA3STATUS_ERR_DIS_BEFORE_FINISH        	= 0x4,	/**< [2] : abnormal, DMA disable before finish */
	XDMA_WDMA3STATUS_ERR_FIFO_CH1_MISMATCH        	= 0x8,	/**< [3] : abnormal FIFO error, Channel 1 Pointer mismatch*/
	XDMA_WDMA3STATUS_ERR_FIFO_CH2_MISMATCH	  	   	= 0x10,	/**< [4] : abnormal FIFO error, Channel 2 Pointer mismatch*/
	XDMA_WDMA3STATUS_ERR_FIFO_CH3_MISMATCH	  	   	= 0x20,	/**< [5] : abnormal FIFO error, Channel 3 Pointer mismatch*/
	XDMA_WDMA3STATUS_ERR_FIFO_CH1_OVERFLOW	  	   	= 0x40,	/**< [6] : abnormal FIFO error, Channel 1 overflow*/
	XDMA_WDMA3STATUS_ERR_FIFO_CH2_OVERFLOW	  	   	= 0x80,	/**< [7] : abnormal FIFO error, Channel 2 overflow*/
	XDMA_WDMA3STATUS_ERR_FIFO_CH3_OVERFLOW	  	   	= 0x100,	/**< [8] : abnormal FIFO error, Channel 3 overflow*/
	XDMA_WDMA3STATUS_ERR_BUS	  	    		   	= 0x200,	/**< [9] : abnormal BUS error*/
} XDMA_WDMA3STATUS_E;

/**
 * \enum XDMA_RDMASTATUS_E
 * \brief xDMA RDMA Interrupt Status
 */
typedef enum
{
	XDMA_RDMASTATUS_NO				        = 0,	/**< No */
	XDMA_RDMASTATUS_NORMAL_FINISH           = 0x1,	/**< [0] : normal, finish */
	XDMA_RDMASTATUS_ERR_DIS_BEFORE_FINISH   = 0x2,	/**< [1] : abnormal, DMA Enable release before finish*/
	XDMA_RDMASTATUS_ERR_BUS	  	    	 = 0x4,	/**< [2] : HREADY but HRESP not be OKAY(2¡¦b00)*/
} XDMA_RDMASTATUS_E;

/**
 * \enum XDMA_WDMA2_DMAMODE_E
 * \brief xDMA WDMA2 Snapshot mode or cyclic mode
 */
typedef enum
{
	XDMA_WDMA2_DMAMODE_SNAPSHOT         = 0,/**< 0 : snapshot mode DMA stop by DMA FINISH Mode*/
	XDMA_WDMA2_DMAMODE_CYCLIC       	= 1,/**< 1 : cyclic mode DMA stop by frame_end*/
} XDMA_WDMA2_DMAMODE_E;

/**
 * \enum XDMA_WDMA2_SNAPSHOT_FINMODE_E
 * \brief WDMA2 FINISH Mode (only support snapshot mode)
 */
typedef enum
{
	XDMA_WDMA2_SNAPSHOT_FINMODE_THR         = 0,/**< 0 : snapshot mode DMA stop by THR*/
	XDMA_WDMA2_SNAPSHOT_FINMODE_FE       	= 1,/**< 1 : snapshot mode DMA stop by frame_end*/
} XDMA_WDMA2_SNAPSHOT_FINMODE_E;

/**
 * \enum XDMA_WDMA3_FINSRC_SEL_E
 * \brief WDMA3 FINISH by threshold or frame_end
 */
typedef enum
{
	XDMA_WDMA3_FINSRC_THR         = 0,/**< 0 : Finish by THR*/
	XDMA_WDMA3_FINSRC_FE     	 = 1,/**< 1 : Finish by frame_end_in*/
} XDMA_WDMA3_FINSRC_SEL_E;

/**
 * \enum XDMA_RDMA_FINSRC_SEL_E
 * \brief RDMA FINISH by threshold, WDMA1 frame_end, WDMA2 frame_end, WDMA3 frame_end
 */
typedef enum
{
	XDMA_RDMA_FINSRC_THR         = 0,/**< 0 : Finish by THR*/
	XDMA_RDMA_FINSRC_WDMA1FE     = 1,/**< 1 : Finish by WDMA1_frame_end_in*/
	XDMA_RDMA_FINSRC_WDMA2FE     = 2,/**< 2 : Finish by WDMA2_frame_end_in*/
	XDMA_RDMA_FINSRC_WDMA3FE     = 3,/**< 3 : Finish by WDMA3_frame_end_in*/
} XDMA_RDMA_FINSRC_SEL_E;
/** @} */


/**
 * \defgroup	XDMA_DRV_REGSTRUCT	xDMA-WDMA1,2,3 and RDMA Driver Structure
 * \ingroup	XDMA_DRV
 * \brief	contains definitions of xDMA-WDMA1,2,3 and RDMA Driver structure.
 * @{
 */
/**
 * \struct xDMA_WDMA1_CFG_T
 * \brief WDMA1 configuration by xDMA Driver
 */
typedef struct
{
	uint8_t wdma1_en;/**< WDMA1 block enable*/
	uint8_t wdma1_ch1_en;/**< WDMA1 channel 1 enable*/
	uint8_t wdma1_ch2_en;/**< WDMA1 channel 2 enable*/
	uint8_t wdma1_ch3_en;/**< WDMA1 channel 3 enable*/
	uint32_t wdma1_ch1_startaddr;/**< WDMA1 channel 1 output start address*/
	uint32_t wdma1_ch1_size;/**< WDMA1 channel 1 output size*/
	uint32_t wdma1_ch2_startaddr;/**< WDMA1 channel 2 output start address*/
	uint32_t wdma1_ch2_size;/**< WDMA1 channel 2 output size*/
	uint32_t wdma1_ch3_startaddr;/**< WDMA1 channel 3 output start address*/
	uint32_t wdma1_ch3_size;/**< WDMA1 channel 3 output size*/
	uint16_t mask;/**< WDMA1 interrupt mask*/
} xDMA_WDMA1_CFG_T;

/**
 * \struct xDMA_WDMA2_CFG_T
 * \brief WDMA2 configuration by xDMA Driver
 */
typedef struct
{
	uint8_t wdma2_en;/**< WDMA2 block enable*/
	XDMA_WDMA2_DMAMODE_E dma_mode;/**< xDMA WDMA2 Snapshot mode or cyclic mode*/
	XDMA_WDMA2_SNAPSHOT_FINMODE_E fin_mode;/**< WDMA2 FINISH Mode (only support snapshot mode)*/
    uint8_t auto_en;/**< WDMA2 HW auto fill jpeg size to another memory enable*/
    uint32_t auto_fill_startaddr;/**< WDMA2 HW auto fill jpeg size to another memory start address*/
	uint32_t wdma2_startaddr;/**< WDMA2 output start address*/
	uint32_t wdma2_size;/**< WDMA2 output size*/
	uint32_t wdma2_offset;/**< WDMA2 next frame offset in cyclic mode (next frame jump offset(next frame start address))*/
	uint8_t wdma2_targetloopCnt;/**< WDMA2 the end count of loop in cyclic mode*/
	uint8_t wdma2_startloopCnt;/**< WDMA2 the start index of loop in cyclic mode*/
	uint8_t mask;/**< WDMA2 interrupt mask*/
} xDMA_WDMA2_CFG_T;

/**
 * \struct xDMA_WDMA3_CFG_T
 * \brief WDMA3 configuration by xDMA Driver
 */
typedef struct
{
	uint8_t wdma3_en;/**< WDMA3 block enable*/
	uint8_t wdma3_ch1_en;/**< WDMA3 channel 1 enable*/
	uint8_t wdma3_ch2_en;/**< WDMA3 channel 2 enable*/
	uint8_t wdma3_ch3_en;/**< WDMA3 channel 3 enable*/
	uint32_t wdma3_ch1_startaddr;/**< WDMA3 channel 1 output start address*/
	uint32_t wdma3_ch1_size;/**< WDMA3 channel 1 output size*/
	uint32_t wdma3_ch2_startaddr;/**< WDMA3 channel 2 output start address*/
	uint32_t wdma3_ch2_size;/**< WDMA3 channel 2 output size*/
	uint32_t wdma3_ch3_startaddr;/**< WDMA3 channel 3 output start address*/
	uint32_t wdma3_ch3_size;/**< WDMA3 channel 3 output size*/
	uint16_t mask;/**< WDMA3 interrupt mask*/
	XDMA_WDMA3_FINSRC_SEL_E finish_mode;/**< WDMA3 FINISH by threshold or frame_end*/
} xDMA_WDMA3_CFG_T;

/**
 * \struct xDMA_RDMA_CFG_T
 * \brief RDMA configuration by xDMA Driver
 */
typedef struct
{
	uint8_t rdma_en;/**< RDMA block enable*/
	uint8_t rdma_ch1_en;/**< RDMA channel 1 enable*/
	uint8_t rdma_ch2_en;/**< RDMA channel 2 enable*/
	uint8_t rdma_ch3_en;/**< RDMA channel 3 enable*/
	XDMA_RDMA_FINSRC_SEL_E finSrcSel;/**< RDMA FINISH by threshold, WDMA1 frame_end, WDMA2 frame_end, WDMA3 frame_end*/
	uint32_t rdma_ch1_startaddr;/**< RDMA channel 1 output start address*/
	uint32_t rdma_ch1_size;/**< RDMA channel 1 output size*/
	uint32_t rdma_ch2_startaddr;/**< RDMA channel 2 output start address*/
	uint32_t rdma_ch2_size;/**< RDMA channel 2 output size*/
	uint32_t rdma_ch3_startaddr;/**< RDMA channel 3 output start address*/
	uint32_t rdma_ch3_size;/**< RDMA channel 3 output size*/
	uint8_t mask;/**< RDMA Interrupt mask*/
} xDMA_RDMA_CFG_T;
/** @} */

/**
 * \defgroup	XDMA_DRV_FUNCDLR	xDMA-WDMA1,2,3 and RDMA Driver Function Declaration
 * \ingroup	XDMA_DRV
 * \brief	Contains declarations of xDMA-WDMA1,2,3 and RDMA Driver functions.
 * @{
 */

/**
 * \brief	WDMA1 interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*XDMAWDMA1_ISREvent_t) (XDMA_WDMA1STATUS_E event);

/**
 * \brief	WDMA2 interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*XDMAWDMA2_ISREvent_t) (XDMA_WDMA2STATUS_E event);

/**
 * \brief	WDMA3 interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*XDMAWDMA3_ISREvent_t) (XDMA_WDMA3STATUS_E event);

/**
 * \brief	RDMA interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*XDMARDMA_ISREvent_t) (XDMA_RDMASTATUS_E event);

/**
 * \brief	xDMA hSync/Vsync interrupt status callback function(debug purpose)
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*XDMA_HS_VS_ISREvent_t) (uint8_t event);

/**
 * \brief	initial xDMA Driver
 *
 * \param[in]	reg_baseaddr	 	wdma1 reg base address
 * \param[in]	reg_hsc_baseaddr	 wdma1 HSC reg base address
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_init(uint32_t reg_baseaddr, uint32_t reg_hsc_baseaddr);

/**
 * \brief	set xDMA WDMA1, WDMA2, WDMA3 and RDMA block enable Setting
 *
 * \param[in]	wdma1_en	 wdma1 block enable
 * \param[in]	wdma2_en	 wdma2 block enable
 * \param[in]	wdma3_en	 wdma3 block enable
 * \param[in]	rdma_en	 rdma block enable
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_enable(uint8_t wdma1_en, uint8_t wdma2_en, uint8_t wdma3_en, uint8_t rdma_en);

/**
 * \brief	set xDMA WDMA1 block enable Setting
 *
 * \param[in]	wdma1_en	 wdma1 block enable
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_wdma1_enable(uint8_t wdma1_en);

/**
 * \brief	set xDMA WDMA2 block enable Setting
 *
 * \param[in]	wdma2_en	 wdma2 block enable
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_wdma2_enable(uint8_t wdma2_en);

/**
 * \brief	set xDMA WDMA3 block enable Setting
 *
 * \param[in]	wdma3_en	 wdma3 block enable
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_wdma3_enable(uint8_t wdma3_en);

/**
 * \brief	set xDMA RDMA block enable Setting
 *
 * \param[in]	rdma_en	 rdma block enable
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_rdma_enable(uint8_t rdma_en);

/**
 * \brief	get xDMA RDMA block enable Setting
 *
 * \param[out]	rdma_en	 rdma block enable
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_rdma_enable(uint8_t *rdma_en);

/**
 * \brief	get xDMA xDMA WDMA1, WDMA2, WDMA3 and RDMA block enable Setting
 *
 * \param[out]	wdma1_en	 wdma1 block enable
 * \param[out]	wdma2_en	 wdma2 block enable
 * \param[out]	wdma3_en	 wdma3 block enable
 * \param[out]	rdma_en	 rdma block enable
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_enable(uint8_t *wdma1_en, uint8_t *wdma2_en, uint8_t *wdma3_en, uint8_t *rdma_en);

/**  WDMA1 **/
/**
 * \brief	set all xDMA WDMA1 related Setting without enable & callback because after enable it will start in most of case xDMA should run Simultaneously
 *
 * \param[in]	wdma1_cfg	 wdma1 configuration
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA1allCfg(xDMA_WDMA1_CFG_T wdma1_cfg);

/**
 * \brief	get default xDMA WDMA1 related Setting
 *
 * \param[out]	wdma1_cfg	 wdma1 configuration
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA1defCfg(xDMA_WDMA1_CFG_T *wdma1_cfg);

/**
 * \brief	set xDMA WDMA1 channel enable Setting
 *
 * \param[in]	ch1enable	 wdma1 channel1 enable
 * \param[in]	ch2enable	 wdma1 channel2 enable
 * \param[in]	ch3enable	 wdma1 channel3 enable
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA1Chenable(uint8_t ch1enable, uint8_t ch2enable, uint8_t ch3enable);

/**
 * \brief	get xDMA WDMA1 channel enable Setting
 *
 * \param[out]	ch1enable	 wdma1 channel1 enable
 * \param[out]	ch2enable	 wdma1 channel2 enable
 * \param[out]	ch3enable	 wdma1 channel3 enable
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA1Chenable(uint8_t *ch1enable, uint8_t *ch2enable, uint8_t *ch3enable);

/**
 * \brief	set xDMA WDMA1 Channel 1 output start Address and Size Setting
 *
 * \param[in]	addr	 Channel 1 output start Address
 * \param[in]	size	 Channel 1 output size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA1Ch1AddrSize(uint32_t addr, uint32_t size);
/**
 * \brief	get xDMA WDMA1 Channel 1 output start Address and Size Setting
 *
 * \param[out]	addr	 Channel 1 output start Address
 * \param[out]	size	 Channel 1 output size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA1Ch1AddrSize(uint32_t *addr, uint32_t *size);

/**
 * \brief	set xDMA WDMA1 Channel 2 output start Address and Size Setting
 *
 * \param[in]	addr	 Channel 2 output start Address
 * \param[in]	size	 Channel 2 output size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA1Ch2AddrSize(uint32_t addr, uint32_t size);

/**
 * \brief	get xDMA WDMA1 Channel 2 output start Address and Size Setting
 *
 * \param[out]	addr	 Channel 2 output start Address
 * \param[out]	size	 Channel 2 output size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA1Ch2AddrSize(uint32_t *addr, uint32_t *size);

/**
 * \brief	set xDMA WDMA1 Channel 3 output start Address and Size Setting
 *
 * \param[in]	addr	 Channel 3 output start Address
 * \param[in]	size	 Channel 3 output size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA1Ch3AddrSize(uint32_t addr, uint32_t size);

/**
 * \brief	get xDMA WDMA1 Channel 3 output start Address and Size Setting
 *
 * \param[out]	addr	 Channel 3 output start Address
 * \param[out]	size	 Channel 3 output size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA1Ch3AddrSize(uint32_t *addr, uint32_t *size);

/**
 * \brief	set xDMA WDMA1 Clear Normal Interrupt
 *
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA1_clear_Normalint();

/**
 * \brief	set xDMA WDMA1 Clear AbNormal Interrupt
 *
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA1_clear_AbNormalint();

/**
 * \brief	set xDMA WDMA1 Interrupt Mask
 *
 * \param[in]	mask	 WDMA1 Interrupt Mask
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA1INTMask(uint16_t mask);

/**
 * \brief	get xDMA WDMA1 Interrupt Mask
 *
 * \param[out]	mask	 WDMA1 Interrupt Mask
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA1INTMask(uint16_t *mask);

/**
 * \brief	register xDMA WDMA1 Interrupt Status callback
 *
 * \param[in]	cb_event	 WDMA1 Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA1_register_cb(XDMAWDMA1_ISREvent_t  cb_event);

/**
 * \brief	get xDMA WDMA1 Interrupt Status
 *
 * \param[out]	status	 WDMA1 Interrupt status
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA1INTStatus(XDMA_WDMA1STATUS_E *status);

/**  WDMA2 **/
/**
 * \brief	set xDMA WDMA2 Cyclic mode Start Frame count related Setting
 *
 * \param[in]	number	 Cyclic mode Start Frame count(start from 0)
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA2StartFrame(uint8_t number);
/**
 * \brief	get xDMA WDMA2 Cyclic mode Start Frame count related Setting
 *
 * \param[out]	number	 Cyclic mode Start Frame count(start from 0)
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA2StartFrame(uint8_t *number);

/**
 * \brief	get xDMA WDMA2 Cyclic mode next Frame buffer index
 *
 * \param[out]	number	 WDMA2 Cyclic mode next Frame buffer index(start from 0)
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA2NextFrameIdx(uint8_t *number);

/**
 * \brief	get xDMA WDMA2 Cyclic mode First frame already capture or not
 *
 * \param[out]	firstframe_cap	 WDMA2 Cyclic mode 1: first frame already capture, 0: first frame not capture
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA2FirstFrameCapflag(uint8_t *firstframe_cap);

/**
 * \brief	clear xDMA WDMA2 Cyclic mode First frame already capture or not flag
 *
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_clear_WDMA2FirstFrameCapflag();


/**
 * \brief	set all xDMA WDMA2 related Setting without enable & callback because after enable it will start in most of case xDMA should run Simultaneously
 *
 * \param[in]	wdma2_cfg	 wdma2 configuration
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA2allCfg(xDMA_WDMA2_CFG_T wdma2_cfg);

/**
 * \brief	get default xDMA WDMA2 related Setting
 *
 * \param[out]	wdma2_cfg	 wdma2 default configuration
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA2defCfg(xDMA_WDMA2_CFG_T *wdma2_cfg);

/**
 * \brief	set xDMA WDMA2 DMA Mode Setting
 *
 * \param[in]	mode	xDMA WDMA2 Snapshot mode or cyclic mode
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA2DMAMode(XDMA_WDMA2_DMAMODE_E mode);

/**
 * \brief	get xDMA WDMA2 DMA Mode Setting
 *
 * \param[out]	mode	xDMA WDMA2 Snapshot mode or cyclic mode
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA2DMAMode(XDMA_WDMA2_DMAMODE_E *mode);

/**
 * \brief	set xDMA WDMA2 Snapshot Mode Finish Setting
 *
 * \param[in]	mode	WDMA2 FINISH Mode (only support snapshot mode)
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA2FinMode(XDMA_WDMA2_SNAPSHOT_FINMODE_E mode);

/**
 * \brief	get xDMA WDMA2 Snapshot Mode Finish Setting
 *
 * \param[out]	mode	WDMA2 FINISH Mode (only support snapshot mode)
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA2FinMode(XDMA_WDMA2_SNAPSHOT_FINMODE_E *mode);

/**
 * \brief	set xDMA WDMA2 Address, Size, offset, target loop count and start index Setting
 *
 * \param[in]	addr	WDMA2 output start address
 * \param[in]	size	WDMA2 output size
 * \param[in]	offset	WDMA2 next frame offset in cyclic mode
 * \param[in]	targetloopCnt	WDMA2 the end count of loop in cyclic mode
 * \param[in]	startloopCnt	WDMA2 the start index of loop in cyclic mode
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA2AddrSizeCnt(uint32_t addr, uint32_t size,uint32_t offset, uint8_t targetloopCnt, uint8_t startloopCnt);

/**
 * \brief	get xDMA WDMA2 Address, Size, offset, target loop count and start index Setting
 *
 * \param[out]	addr	WDMA2 output start address
 * \param[out]	size	WDMA2 output size
 * \param[out]	offset	WDMA2 next frame offset in cyclic mode
 * \param[out]	targetloopCnt	WDMA2 the end count of loop in cyclic mode
 * \param[out]	startloopCnt	WDMA2 the start index of loop in cyclic mode
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA2AddrSizeCnt(uint32_t *addr, uint32_t *size,uint32_t *offset, uint8_t *targetloopCnt, uint8_t *startloopCnt);
/**
 * \brief	get xDMA WDMA2 offset Setting
 *
 * \param[out]	offset	WDMA2 next frame offset in cyclic mode
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA2Offset(uint32_t *offset);

/**
 * \brief	set xDMA WDMA2 Write FileSize Enable and Write FileSize Setting
 *
 * \param[in]	auto_en	WDMA2 auto fill jpeg file size to memory feature enable/disable
 * \param[in]	start_filladdr	WDMA2 auto fill jpeg file size memory start address
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA2AutoFileSize(uint32_t auto_en, uint32_t start_filladdr);


/**
 * \brief	set xDMA WDMA2 Clear Normal Interrupt
 *
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA2_clear_Normalint();

/**
 * \brief	set xDMA WDMA2 Clear AbNormal Interrupt
 *
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA2_clear_AbNormalint();

/**
 * \brief	set xDMA WDMA2 Interrupt Mask
 *
 * \param[in]	mask	WDMA2 Interrupt Mask
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA2INTMask(uint8_t mask);

/**
 * \brief	get xDMA WDMA2 Interrupt Mask
 *
 * \param[out]	mask	WDMA2 Interrupt Mask
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA2INTMask(uint8_t *mask);

/**
 * \brief	register xDMA WDMA2 Interrupt Status callback
 *
 * \param[in]	cb_event	WDMA2 Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA2_register_cb(XDMAWDMA2_ISREvent_t  cb_event);

/**
 * \brief	get xDMA WDMA2 Interrupt Status
 *
 * \param[out]	status	 WDMA2 Interrupt status
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA2INTStatus(XDMA_WDMA2STATUS_E *status);

/**
 * \brief	set xDMA WDMA2 clear loop flag. (Hardware store cyclic mode is loop or not in loop flag)
 *
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA2_clearloopflag();

/**
 * \brief	get xDMA WDMA2 loop flag (1: cyclic buffer overwrite, 0: cyclic buffer not overwrite)
 *
 * \param[out]	loop_flag	1: cyclic buffer overwrite, 0: cyclic buffer not overwrite
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA2_loopflag(uint8_t *loop_flag);

/**
 * \brief	get xDMA WDMA2 get total cyclic buffer number (register + 1), EX: 10 Cyclic Slot, output parameter is 10
 *
 * \param[out]	buffer_cnt	 get total cyclic buffer number (register + 1)
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA2_bufferNo(uint8_t *buffer_cnt);


/**  WDMA3 **/
/**
 * \brief	set all xDMA WDMA3 related Setting without enable & callback because after enable it will start in most of case xDMA should run Simultaneously
 *
 * \param[in]	wdma3_cfg	 wdma3 configuration
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA3allCfg(xDMA_WDMA3_CFG_T wdma3_cfg);

/**
 * \brief	get default xDMA WDMA3 related Setting
 *
 * \param[out]	wdma3_cfg	 wdma3 default configuration
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA3defCfg(xDMA_WDMA3_CFG_T *wdma3_cfg);

/**
 * \brief	set xDMA WDMA3 channel enable Setting
 *
 * \param[in]	ch1enable	 wdma3 channel 1 enable
 * \param[in]	ch2enable	 wdma3 channel 2 enable
 * \param[in]	ch3enable	 wdma3 channel 3 enable
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA3Chenable(uint8_t ch1enable, uint8_t ch2enable, uint8_t ch3enable);

/**
 * \brief	get xDMA WDMA3 channel enable Setting
 *
 * \param[out]	ch1enable	 wdma3 channel 1 enable
 * \param[out]	ch2enable	 wdma3 channel 2 enable
 * \param[out]	ch3enable	 wdma3 channel 3 enable
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA3Chenable(uint8_t *ch1enable, uint8_t *ch2enable, uint8_t *ch3enable);

/**
 * \brief	set xDMA WDMA3 Finish Mode
 *
 * \param[in]	finmode	 WDMA3 FINISH by threshold or frame_end
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA3FinMode(XDMA_WDMA3_FINSRC_SEL_E finmode);

/**
 * \brief	set xDMA WDMA3 Channel 1 output start Address and Size Setting
 *
 * \param[in]	addr	 Channel 1 output start Address
 * \param[in]	size	 Channel 1 output size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA3Ch1AddrSize(uint32_t addr, uint32_t size);

/**
 * \brief	get xDMA WDMA3 Channel 1 output start Address and Size Setting
 *
 * \param[out]	addr	 Channel 1 output start Address
 * \param[out]	size	 Channel 1 output size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA3Ch1AddrSize(uint32_t *addr, uint32_t *size);

/**
 * \brief	set xDMA WDMA3 Channel 2 output start Address and Size Setting
 *
 * \param[in]	addr	 Channel 2 output start Address
 * \param[in]	size	 Channel 2 output size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA3Ch2AddrSize(uint32_t addr, uint32_t size);

/**
 * \brief	get xDMA WDMA3 Channel 2 output start Address and Size Setting
 *
 * \param[out]	addr	 Channel 2 output start Address
 * \param[out]	size	 Channel 2 output size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA3Ch2AddrSize(uint32_t *addr, uint32_t *size);

/**
 * \brief	set xDMA WDMA3 Channel 3 output start Address and Size Setting
 *
 * \param[in]	addr	 Channel 3 output start Address
 * \param[in]	size	 Channel 3 output size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA3Ch3AddrSize(uint32_t addr, uint32_t size);

/**
 * \brief	set xDMA WDMA3 Channel 3 output start Address and Size Setting
 *
 * \param[out]	addr	 Channel 3 output start Address
 * \param[out]	size	 Channel 3 output size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA3Ch3AddrSize(uint32_t *addr, uint32_t *size);

/**
 * \brief	set xDMA WDMA3 Clear Normal Interrupt
 *
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA3_clear_Normalint();

/**
 * \brief	set xDMA WDMA3 Clear AbNormal Interrupt
 *
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA3_clear_AbNormalint();

/**
 * \brief	set xDMA WDMA3 Interrupt Mask
 *
 * \param[in]	mask	 Interrupt Mask
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA3INTMask(uint16_t mask);

/**
 * \brief	get xDMA WDMA3 Interrupt Mask
 *
 * \param[out]	mask	 Interrupt Mask
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA3INTMask(uint16_t *mask);

/**
 * \brief	register xDMA WDMA3 Interrupt Status callback
 *
 * \param[in]	cb_event	 Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA3_register_cb(XDMAWDMA3_ISREvent_t  cb_event);

/**
 * \brief	get xDMA WDMA3 Interrupt Status
 *
 * \param[out]	status	 WDMA3 Interrupt status
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_WDMA3INTStatus(XDMA_WDMA3STATUS_E *status);

/**  RDMA **/
/**
 * \brief	set all xDMA RDMA related Setting without enable & callback because after enable it will start in most of case xDMA should run Simultaneously
 *
 * \param[in]	rdma_cfg	 RDMA configuration
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_RDMAallCfg(xDMA_RDMA_CFG_T rdma_cfg);

/**
 * \brief	get default xDMA RDMA related Setting
 *
 * \param[out]	rdma_cfg	 RDMA default configuration
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_RDMAdefCfg(xDMA_RDMA_CFG_T *rdma_cfg);

/**
 * \brief	set xDMA RDMA channel enable Setting
 *
 * \param[in]	ch1enable	 RDMA channel 1 enable
 * \param[in]	ch2enable	 RDMA channel 2 enable
 * \param[in]	ch3enable	 RDMA channel 3 enable
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_RDMAChenable(uint8_t ch1enable, uint8_t ch2enable, uint8_t ch3enable);

/**
 * \brief	get xDMA RDMA channel enable Setting
 *
 * \param[out]	ch1enable	 RDMA channel 1 enable
 * \param[out]	ch2enable	 RDMA channel 2 enable
 * \param[out]	ch3enable	 RDMA channel 3 enable
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_RDMAChenable(uint8_t *ch1enable, uint8_t *ch2enable, uint8_t *ch3enable);

/**
 * \brief	set xDMA RDMA Finish SRC Sel Setting
 *
 * \param[in]	finsrcsel	 RDMA FINISH by threshold, WDMA1 frame_end, WDMA2 frame_end, WDMA3 frame_end
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_RDMAFinSrcSel(XDMA_RDMA_FINSRC_SEL_E finsrcsel);

/**
 * \brief	get xDMA RDMA Finish SRC Sel Setting
 *
 * \param[out]	finsrcsel	 RDMA FINISH by threshold, WDMA1 frame_end, WDMA2 frame_end, WDMA3 frame_end
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_RDMAFinSrcSel(XDMA_RDMA_FINSRC_SEL_E *finsrcsel);

/**
 * \brief	set xDMA RDMA Ch1 Address and Size Setting
 *
 * \param[in]	addr	 RDMA channel1 input start address
 * \param[in]	size	 RDMA channel1 input size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_RDMACh1AddrSize(uint32_t addr, uint32_t size);

/**
 * \brief	get xDMA RDMA Ch1 Address and Size Setting
 *
 * \param[out]	addr	 RDMA channel1 input start address
 * \param[out]	size	 RDMA channel1 input size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_RDMACh1AddrSize(uint32_t *addr, uint32_t *size);

/**
 * \brief	set xDMA RDMA Ch2 Address and Size Setting
 *
 * \param[in]	addr	 RDMA channel2 input start address
 * \param[in]	size	 RDMA channel2 input size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_RDMACh2AddrSize(uint32_t addr, uint32_t size);

/**
 * \brief	get xDMA RDMA Ch2 Address and Size Setting
 *
 * \param[out]	addr	 RDMA channel2 input start address
 * \param[out]	size	 RDMA channel2 input size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_RDMACh2AddrSize(uint32_t *addr, uint32_t *size);

/**
 * \brief	set xDMA RDMA Ch3 Address and Size Setting
 *
 * \param[in]	addr	 RDMA channel3 input start address
 * \param[in]	size	 RDMA channel3 input size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_RDMACh3AddrSize(uint32_t addr, uint32_t size);

/**
 * \brief	get xDMA RDMA Ch3 Address and Size Setting
 *
 * \param[out]	addr	 RDMA channel3 input start address
 * \param[out]	size	 RDMA channel3 input size
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_RDMACh3AddrSize(uint32_t *addr, uint32_t *size);

/**
 * \brief	set xDMA RDMA Clear Normal Interrupt
 *
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_RDMA_clear_Normalint();

/**
 * \brief	set xDMA RDMA Clear AbNormal Interrupt
 *
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_RDMA_clear_AbNormalint();

/**
 * \brief	set xDMA RDMA Interrupt Mask
 *
 * \param[in]	mask	 RDMA interrupt mask
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_RDMAINTMask(uint8_t mask);

/**
 * \brief	get xDMA RDMA Interrupt Mask
 *
 * \param[out]	mask	 RDMA interrupt mask
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_RDMAINTMask(uint8_t *mask);

/**
 * \brief	register xDMA RDMA Interrupt Status callback
 *
 * \param[in]	cb_event	 RDMA interrupt callback function
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_RDMA_register_cb(XDMARDMA_ISREvent_t  cb_event);

/**
 * \brief	get xDMA RDMA Interrupt Status
 *
 * \param[out]	status	 RDMA Interrupt status
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_get_RDMAINTStatus(XDMA_RDMASTATUS_E *status);

/**
 * \brief	set xDMA mask Setting 1: mask all INT, 0: not mask all INT
 *
 * \param[in]	mask	 (1: mask all INT, 0: not mask all INT)
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_mask(uint8_t mask);

/**
 * \brief	register xDMA WDMA1 VS Interrupt Status callback(debug purpose)
 *
 * \param[in]	cb_event	 Interrupt callback
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA1_VS_register_cb(XDMA_HS_VS_ISREvent_t  cb_event);

/**
 * \brief	register xDMA WDMA1 HS Interrupt Status callback(debug purpose)
 *
 * \param[in]	cb_event	 Interrupt callback
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA1_HS_register_cb(XDMA_HS_VS_ISREvent_t  cb_event);


/**
 * \brief	register xDMA WDMA2 VS Interrupt Status callback(debug purpose)
 *
 * \param[in]	cb_event	 Interrupt callback
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA2_VS_register_cb(XDMA_HS_VS_ISREvent_t  cb_event);

/**
 * \brief	register xDMA WDMA2 HS Interrupt Status callback(debug purpose)
 *
 * \param[in]	cb_event	 Interrupt callback
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA2_HS_register_cb(XDMA_HS_VS_ISREvent_t  cb_event);


/**
 * \brief	register xDMA WDMA3 VS Interrupt Status callback(debug purpose)
 *
 * \param[in]	cb_event	 Interrupt callback
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA3_VS_register_cb(XDMA_HS_VS_ISREvent_t  cb_event);

/**
 * \brief	register xDMA WDMA3 HS Interrupt Status callback(debug purpose)
 *
 * \param[in]	cb_event	 Interrupt callback
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_WDMA3_HS_register_cb(XDMA_HS_VS_ISREvent_t  cb_event);

/**
 * \brief	set WDMA2 Start Address
 *
 * \param[in]	addr	 WDMA2 address
 * \return	XDMA_ERROR_E.
 */
XDMA_ERROR_E hx_drv_xdma_set_WDMA2Addr(uint32_t addr);
/** @} */

#endif /* INC_HX_DRV_DMA_H_ */
