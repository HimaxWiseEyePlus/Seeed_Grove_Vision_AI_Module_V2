/*
 * hx_drv_inp.h
 *
 *  Created on: 2018¦~11¤ë10¤é
 *      Author: 902447
 */

#ifndef INC_HX_DRV_INP_H_
#define INC_HX_DRV_INP_H_

#include "dev_common.h"

/**
 * \defgroup	INP_DRV	INP Driver
 * \ingroup		INP_DRV
 * \brief	INP Driver Declaration
 */
/**
 * \defgroup	INP_DRV_ENUM	INP Driver Enumeration
 * \ingroup	INP_DRV
 * \brief	Defines some enumeration of INP Driver need.
 * @{
 */
/**
 * \enum INP_ERROR_E
 * \brief INP Errors Type
 */
typedef enum
{
	INP_NO_ERROR                     		= 0,	/**< No ERROR */
	INP_ERROR_NOT_INIT              	    = 1,	/**< ERROR MSG: not init */
	INP_ERROR_ALREADY_INIT                  = 2,	/**< ERROR MSG: already init */
	INP_ERROR_INVALID_PARAMETERS     		= 3,	/**< ERROR MSG: Invalid parameters */
 	INP_UNKNOWN_ERROR      				    = 4,	/**< ERROR MSG: UNKNOWN ERROR*/
} INP_ERROR_E;

/**
 * \enum INP_IOBIT_E
 * \brief RX(sensor) input valid data IO bit
 */
typedef enum
{
	INP_IOBIT_8BIT 		= 0,/**< IO 8 bit */
	INP_IOBIT_4BIT 		= 1,/**< IO 4 bit */
	INP_IOBIT_1BIT 		= 2,/**< IO 1 bit */
} INP_IOBIT_E;

/**
 * \enum INP_DATADEPTH_E
 * \brief RX(sensor) input valid data depth per pixel
 */
typedef enum
{
	INP_DATADEPTH_8BIT 		= 0,/**< data depth 8 bit */
	INP_DATADEPTH_4BIT 		= 1,/**< data depth 4 bit */
} INP_DATADEPTH_E;

/**
 * \enum INP_PIXELORDER_E
 * \brief RX(sensor) input pixel order
 */
typedef enum
{
	INP_PIXELORDER_P0_P1 		= 0,/**< even pixel(P0, P2, ¡K) in rx_data[3:0], odd pixel(P1, P3, ¡K) in rx_data[7:4].*/
	INP_PIXELORDER_P1_P0 		= 1,/**< even pixel(P1, P3, ¡K) in rx_data[3:0], odd pixel(P0, P2, ¡K) in rx_data[7:4].*/
} INP_PIXELORDER_E;

/**
 * \enum INP_DATAENDIAN_E
 * \brief RX(sensor) input data endian
 */
typedef enum
{
	INP_DATAENDIAN_LS_FIRST 		= 0,/**< LS bit first*/
	INP_DATAENDIAN_MS_FIRST 		= 1,/**< MS bit first*/
} INP_DATAENDIAN_E;

/**
 * \enum INP_DATABUS_E
 * \brief Data bus reverse.
 */
typedef enum
{
	INP_DATABUS_NORMAL 			= 0,/**< Data bus normal*/
	INP_DATABUS_REVERSE 		= 1,/**< Data bus reverse*/
} INP_DATABUS_E;

/**
 * \enum INP_HVSYNC_CHANGE_E
 * \brief vsync/hsync input change.
 */
typedef enum
{
	INP_HVSYNC_CHANGE_NORMAL 		= 0,/**< vsync/hsync not exchange*/
	INP_HVSYNC_CHANGE_EXCHANGE 		= 1,/**< vsync/hsync exchange*/
} INP_HVSYNC_CHANGE_E;

/**
 * \enum INP_VSYNC_INVERSE_E
 * \brief vsync inverse
 */
typedef enum
{
	INP_VSYNC_INVERSE_NORMAL 		= 0,/**< vsync normal*/
	INP_VSYNC_INVERSE_INVERSE 		= 1,/**< vsync inverse*/
} INP_VSYNC_INVERSE_E;


/**
 * \enum INP_HSYNC_INVERSE_E
 * \brief hsync inverse
 */
typedef enum
{
	INP_HSYNC_INVERSE_NORMAL 		= 0,/**< hsync normal*/
	INP_HSYNC_INVERSE_INVERSE 		= 1,/**< hsync inverse*/
} INP_HSYNC_INVERSE_E;

/**
 * \enum INP_FS_REGEN_E
 * \brief frame start re-generation enable
 */
typedef enum
{
	INP_FS_REGEN_DISABLE 		= 0,/**< frame start regen disable*/
	INP_FS_REGEN_ENABLE 		= 1,/**< frame start regen enable*/
} INP_FS_REGEN_E;

/**
 * \enum INP_SUBSAMPLE_E
 * \brief disable or 4to2 or 8to2 subsampling for HM2140
 */
typedef enum
{
	INP_SUBSAMPLE_DISABLE 	= 0,		/**< subsample disable*/
	INP_SUBSAMPLE_4TO2 		= 1,		/**< 4 to 2 subsample Bayer */
	INP_SUBSAMPLE_8TO2 		= 0x100,	/**< 8 to 2 subsample Bayer */
#if (IC_VERSION >= 30)
	INP_SUBSAMPLE_16TO2 	= 0x1000,	/**< 16 to 2 subsample Bayer */
	INP_SUBSAMPLE_4TO2_B    = 0x1003,	/**<  6 to 2 subsample Bayer */
	INP_SUBSAMPLE_6TO2_B    = 0x1005,	/**<  6 to 2 subsample Bayer */
	INP_SUBSAMPLE_8TO2_B    = 0x1007,	/**<  6 to 2 subsample Bayer */
	INP_SUBSAMPLE_10TO2_B	= 0x1009,	/**< 10 to 2 subsample Bayer */
	INP_SUBSAMPLE_12TO2_B   = 0x100B,	/**< 12 to 2 subsample Bayer */
	INP_SUBSAMPLE_14TO2_B   = 0x100D,   /**< 14 to 2 subsample Bayer */
	INP_SUBSAMPLE_16TO2_B 	= 0x100F,	/**< 16 to 2 subsample Bayer */
	INP_SUBSAMPLE_4TO2_M    = 0x1002,	/**<  4 to 2 subsample Mono */
	INP_SUBSAMPLE_6TO2_M    = 0x1004,	/**<  6 to 2 subsample Mono */
	INP_SUBSAMPLE_8TO2_M    = 0x1006,	/**<  8 to 2 subsample Mono */
	INP_SUBSAMPLE_10TO2_M	= 0x1008,	/**< 10 to 2 subsample Mono */
	INP_SUBSAMPLE_12TO2_M   = 0x100A,	/**< 12 to 2 subsample Mono */
	INP_SUBSAMPLE_14TO2_M   = 0x100C,   /**< 14 to 2 subsample Mono */
	INP_SUBSAMPLE_16TO2_M 	= 0x100E,	/**< 16 to 2 subsample Mono */
#endif
} INP_SUBSAMPLE_E;

/**
 * \enum INP_VSYNCMODE_E
 * \brief vsync mode option
 */
typedef enum
{
	INP_VSYNCMODE_CONVENTIONAL 		= 0,/**< vsync conventional mode*/
	INP_VSYNCMODE_PULSE 			= 1,/**< vsync pulse mode(re-gen frame end)*/
} INP_VSYNCMODE_E;

#if (IC_VERSION >= 30)
/**
 * \enum INP_BINNING_E
 * \brief vsync mode option
 */
typedef enum
{
	INP_BINNING_DISABLE 	= 0,		/**< subsample disable*/
	INP_BINNING_4TO2_B 		= 0x03,		/**<  4 to 2 binning Bayer */
	INP_BINNING_6TO2_B    	= 0x05,		/**<  6 to 2 binning Bayer */
	INP_BINNING_8TO2_B 		= 0x07,		/**<  8 to 2 binning Bayer */
	INP_BINNING_10TO2_B		= 0x09,		/**< 10 to 2 binning Bayer */
	INP_BINNING_12TO2_B     = 0x0B,		/**< 12 to 2 binning Bayer */
	INP_BINNING_14TO2_B    	= 0x0D,   	/**< 14 to 2 binning Bayer */
	INP_BINNING_16TO2_B 	= 0x0F,		/**< 16 to 2 binning Bayer */
	INP_BINNING_4TO2_M    	= 0x02,		/**<  4 to 2 binning Mono */
	INP_BINNING_6TO2_M    	= 0x04,		/**<  6 to 2 binning Mono */
	INP_BINNING_8TO2_M    	= 0x06,		/**<  8 to 2 binning Mono */
	INP_BINNING_10TO2_M		= 0x08,		/**< 10 to 2 binning Mono */
	INP_BINNING_12TO2_M   	= 0x0A,		/**< 12 to 2 binning Mono */
	INP_BINNING_14TO2_M   	= 0x0C,   	/**< 14 to 2 binning Mono */
	INP_BINNING_16TO2_M 	= 0x0E,		/**< 16 to 2 binning Mono */
} INP_BINNING_E;
#endif
/** @} */

/**
 * \defgroup	INP_DRV_REGSTRUCT	INP Driver Structure
 * \ingroup	INP_DRV
 * \brief	contains definitions of INP Driver structure.
 * @{
 */
/**
 * \struct INP_RXFORMAT_T
 * \brief INP RX Format configuration by INP Driver
 */
typedef struct
{
	INP_IOBIT_E iobit;		/**< RX(sensor) input valid data IO bit*/
	INP_DATADEPTH_E depth;	/**< RX(sensor) input valid data depth per pixel*/
	INP_PIXELORDER_E pixelorder;/**< RX(sensor) input pixel order*/
	INP_DATAENDIAN_E endian;/**< RX(sensor) input data endian*/
} INP_RXFORMAT_T;

/**
 * \struct INP_DELAY_T
 * \brief INP Re-gen delay time configuration by INP Driver
 */
typedef struct
{
	uint8_t vsync_delay;/**< Regen Vsync Delay Cycle*/
	uint8_t hsync_delay;/**< Regen Hsync Delay Cycle*/
	uint8_t de_delay;/**< Regen DE Delay Cycle*/
	uint8_t lineend_delay;/**< Regen LineEnd Delay Cycle*/
	uint8_t frameend_delay;/**< Regen FrameEnd Delay Cycle*/
} INP_DELAY_T;

/**
 * \struct INP_CROP_T
 * \brief INP set crop by INP Driver
 */
typedef struct
{
    uint16_t start_x;/**< INP CROP start x (min 0)*/
    uint16_t start_y;/**< INP CROP start y (min 0)*/
    uint16_t last_x;/**< INP CROP last x (max HSIZE - 1)*/
    uint16_t last_y;/**< INP CROP last y (max VSIZE - 1)*/
} INP_CROP_T;

/**
 * \struct INP_CFG_T
 * \brief INP configuration by INP Driver
 */
typedef struct
{
	uint8_t inp_enable;/**< INP enable*/
	INP_RXFORMAT_T rxformat;/**< INP RX Format configuration by INP Driver*/
	INP_DATABUS_E databus_reverse;/**< Data bus reverse*/
	INP_HVSYNC_CHANGE_E hvsyncExchange;/**< vsync/hsync input change.*/
	INP_VSYNC_INVERSE_E vsyncInverse;/**< vsync inverse.*/
	INP_HSYNC_INVERSE_E hsyncInverse;/**< hsync inverse.*/
	INP_FS_REGEN_E fs_regen;/**< frame start re-generation enable.*/
	INP_DELAY_T delay;/**< re-generation timing delay.*/
	INP_SUBSAMPLE_E subsample;/**< INP subsample*/
	INP_VSYNCMODE_E vsyncmode;/**< INP Vsync mode*/
	uint16_t framelen;/**< Sensor Frame Length for INP*/
	uint8_t inp_rx_crop_enable;/**< INP CROP enable*/
	uint8_t inp_rx_lbuff_enable;/**< INP RX LBUFF enable*/
	uint32_t inp_rx_crop_start_xy;/**< INP RX CROP START XY enable*/
	uint32_t inp_rx_crop_last_xy;/**< INP RX CROP LAST XY enable*/
	uint16_t inp_in_hsize;  /**< Sensor Hsize for INP*/
	uint16_t inp_rx_lbuf_insert_h_fporch; /**< INSERT H_FPORCH for INP*/
#if (IC_VERSION >= 30)
	INP_BINNING_E binmode;	/**< BINNING for INP*/
#endif
} INP_CFG_T;
/** @} */

/**
 * \defgroup	INP_DRV_FUNCDLR	INP Driver Function Declaration
 * \ingroup	INP_DRV
 * \brief	Contains declarations of INP Driver functions.
 * @{
 */
/**
 * \brief INP Interrupt callback function
 *
 * \return	void.
 */
typedef void (*INP_ISREvent_t) (uint8_t event);

/**
 * \brief	initial INP Driver
 *
 * \param[in]	reg_baseaddr	 	INP reg base address
 * \param[in]	reg_hsc_baseaddr	INP HSC reg base address
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_init(uint32_t reg_baseaddr, uint32_t reg_hsc_baseaddr);

/**
 * \brief	set all INP related Setting
 *
 * \param[in]	inp_cfg	 INP configuration
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_allCfg(INP_CFG_T inp_cfg);


/**
 * \brief	get default INP related Setting
 *
 * \param[out]	inp_cfg	 INP default configuration
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_defCfg(INP_CFG_T *inp_cfg);


/**
 * \brief	set INP enable
 *
 * \param[in]	enable	 INP enable
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_enable(uint8_t enable);

/**
 * \brief	get INP enable
 *
 * \param[out]	enable	 INP enable
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_enable(uint8_t *enable);


/**
 * \brief	set INP RX format
 *
 * \param[in]	format	 INP RX Format configuration by INP Driver
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_rxformat(INP_RXFORMAT_T format);

/**
 * \brief	get INP RX format
 *
 * \param[out]	format	 INP RX Format configuration by INP Driver
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_rxformat(INP_RXFORMAT_T *format);

/**
 * \brief	set INP data bus reverse
 *
 * \param[in]	reverse	 Data bus reverse.
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_databusreverse(INP_DATABUS_E reverse);

/**
 * \brief	get INP data bus reverse
 *
 * \param[out]	reverse	 Data bus reverse.
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_databusreverse(INP_DATABUS_E *reverse);


/**
 * \brief	set INP HVSync change
 *
 * \param[in]	hv_change	vsync/hsync input change.
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_HVSyncchange(INP_HVSYNC_CHANGE_E hv_change);

/**
 * \brief	get INP HVSync change
 *
 * \param[out]	hv_change	vsync/hsync input change.
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_HVSyncchange(INP_HVSYNC_CHANGE_E *hv_change);

/**
 * \brief	set INP VSync Inverse
 *
 * \param[in]	v_inverse	vsync inverse
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_VSyncInverse(INP_VSYNC_INVERSE_E v_inverse);

/**
 * \brief	get INP VSync Inverse
 *
 * \param[out]	v_inverse	vsync inverse
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_VSyncInverse(INP_VSYNC_INVERSE_E *v_inverse);


/**
 * \brief	set INP HSync Inverse
 *
 * \param[in]	h_inverse	hsync inverse
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_HSyncInverse(INP_HSYNC_INVERSE_E h_inverse);

/**
 * \brief	get INP HSync Inverse
 *
 * \param[out]	h_inverse	hsync inverse
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_HSyncInverse(INP_HSYNC_INVERSE_E *h_inverse);

/**
 * \brief	set INP Frame Start Regen
 *
 * \param[in]	fs_regen	frame start re-generation enable
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_FSRegen(INP_FS_REGEN_E fs_regen);

/**
 * \brief	get INP Frame Start Regen
 *
 * \param[out]	fs_regen	frame start re-generation enable
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_FSRegen(INP_FS_REGEN_E *fs_regen);


/**
 * \brief	set INP Timing delay
 *
 * \param[in]	delay	INP Re-gen delay time configuration by INP Driver
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_Delay(INP_DELAY_T delay);

/**
 * \brief	get INP Timing delay
 *
 * \param[out]	delay	INP Re-gen delay time configuration by INP Driver
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_Delay(INP_DELAY_T *delay);


/**
 * \brief	set INP subsample
 *
 * \param[in]	subsample	disable or 4to2 or 8to2 subsampling for HM2140
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_subsample(INP_SUBSAMPLE_E subsample);

/**
 * \brief	get INP subsample
 *
 * \param[out]	subsample	disable or 4to2 or 8to2 subsampling for HM2140
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_subsample(INP_SUBSAMPLE_E *subsample);

#if (IC_VERSION >= 30)
/**
 * \brief	set INP rxsub th
 *
 * \param[in]	subsampleth	disable or Nto2 subsampling for HM2140
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_rxsub_th(uint8_t subsampleth);

/**
 * \brief	get INP rxsub th
 *
 * \param[out]	subsampleth	disable or Nto2 subsampling
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_rxsub_th(uint8_t *subsampleth);

/**
 * \brief	set INP rxsub hsize
 *
 * \param[in]	subhsize	HSIZE for subsample 2
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_rxsub_hsize(uint16_t subhsize);

/**
 * \brief	get INP rxsub hsize
 *
 * \param[out]	subhsize	HSIZE for subsample 2
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_rxsub_hsize(uint16_t *subhsize);

/**
 * \brief	set INP rxbin th
 *
 * \param[in]	binning	disable or Nto2 binning
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_rxbin_th(uint8_t rxbinth);

/**
 * \brief	get INP rxbin th
 *
 * \param[out]	binning	disable or Nto2 binning40
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_rxbin_th(uint8_t *rxbinth);

/**
 * \brief	set INP rxbin hsize
 *
 * \param[in]	subhsize	HSIZE for binning
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_rxbin_hsize(uint16_t rxbinhsize);

/**
 * \brief	get INP rxbin hsize
 *
 * \param[out]	subhsize	HSIZE for binning
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_rxbin_hsize(uint16_t *rxbinhsize);
#endif

/**
 * \brief   set INP HSIZE
 *
 * \param[in]   hsize   sensor input hsize
 * \return  INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_cropin_hsize(uint32_t hsize);

/**
 * \brief   get INP HSIZE
 *
 * \param[out]  hsize   sensor input hsize
 * \return  INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_cropin_hsize(uint32_t *hsize);

/**
 * \brief	set INP RX VsyncMode
 *
 * \param[in]	vsyncmode	vsync mode option
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_VsyncMode(INP_VSYNCMODE_E vsyncmode);

/**
 * \brief	get INP RX VsyncMode
 *
 * \param[out]	vsyncmode	vsync mode option
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_VsyncMode(INP_VSYNCMODE_E *vsyncmode);

/**
 * \brief	set INP RX Pulse Mode FrameLen
 *
 * \param[in]	framelen	Sensor VSize
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_PulseFrameLen(uint16_t framelen);

/**
 * \brief	get INP RX Pulse Mode FrameLen
 *
 * \param[out]	framelen	Sensor VSize
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_PulseFrameLen(uint16_t *framelen);

/**
 * \brief	Register INP Vsync Input Interrupt callback function(debug purpose)
 *
 * \param[in]	cb_event	Interrupt callback function
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_vsin_register_cb(INP_ISREvent_t  cb_event);
/**
 * \brief	Register INP Vsync Output Interrupt callback function(debug purpose)
 *
 * \param[in]	cb_event	Interrupt callback function
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_vsout_register_cb(INP_ISREvent_t  cb_event);
/**
 * \brief	Register INP Hsync Input Interrupt callback function(debug purpose)
 *
 * \param[in]	cb_event	Interrupt callback function
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_hsin_register_cb(INP_ISREvent_t  cb_event);
/**
 * \brief	Register INP Hsync Output Interrupt callback function(debug purpose)
 *
 * \param[in]	cb_event	Interrupt callback function
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_hsout_register_cb(INP_ISREvent_t  cb_event);

/**
 * \brief	Register INP rxdehcnterr Interrupt callback function (debug purpose)
 *
 * \param[in]	cb_event	Interrupt callback function
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_rxdehcnterr_register_cb(INP_ISREvent_t  cb_event);
/**
 * \brief	Register INP rx2outdehcnterr Interrupt callback function (debug purpose)
 *
 * \param[in]	cb_event	Interrupt callback function
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_rx2outdehcnterr_register_cb(INP_ISREvent_t  cb_event);



/**
 * \brief	set INP swreset 1:SWRESET 0:Normal
 *
 * \param[in]	swreset	(1: SWRESET mode, 0: Normal mode)
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_swreset(uint8_t swreset);

/**
 * \brief	get INP RX HSYNC and VSYNC Cnt
 *
 * \param[out]	vsync_cnt	INP out Vsync cnt
 * \param[out]	hsync_cnt	INP out Hsync cnt
 * \param[out]	de_cnt	INP out DE cnt
 * \param[out]	le_cnt	INP out LE cnt
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_get_timing(uint16_t *vsync_cnt, uint16_t *hsync_cnt, uint32_t *de_cnt, uint16_t *le_cnt);

/**
 * \brief	set INP CROP Area
 *
 * \param[in]	start_point	cropped area start point
 * \param[in]	end_point	cropped area end point
 * \return	INP_ERROR_E.
 */
INP_ERROR_E hx_drv_inp_set_crop_area(uint32_t start_point, uint32_t end_point);
/** @} */

#endif /* SYNOPSYS_PA8535_EM9D_DFSS_SVNFORHM01B0MONO_PLATFORM_SYNOPSYS_PA8535_EM9D_DFSS_DEFAULT_PLATFORM_INC_HX_DRV_INP_H_ */
