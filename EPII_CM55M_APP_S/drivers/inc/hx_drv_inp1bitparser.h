/*
 * hx_drv_inp1bitparser.h
 *
 *  Created on: 2019¦~5¤ë2¤é
 *      Author: 902447
 */

#ifndef INC_HX_DRV_INP1BITPARSER_H_
#define INC_HX_DRV_INP1BITPARSER_H_

#include "dev_common.h"
/**
 * \defgroup	INP_1BITPARSER_DRV	INP 1BITPARSER Driver
 * \ingroup		INP_1BITPARSER_DRV
 * \brief	INP 1BITPARSER Driver Declaration
 * \details This driver is for parsing HM11B1 Serial interface protocol. Almost control is handle by datapath library. You may only need use this driver API for tg2ytg setting and debug error reason\n
 * <pre>
 * For Example
 * Ex1: hx_drv_inp1bitparser_set_tg2utg(uint16_t tg2utg);
 *      Toggle to untoggle detection cycle
 *		Setting rule:
 *			reg_inpparser_tg2utg =ceil(8*DP_CLK/freq_SCLK)[clk_h]
 *			ex: DP_CLK=24MHz, freq_SCLK=72MHz, set reg_tg2utg=(8*24/72)=3 cycles of clk_h, but min is 5, so setting 5
 * Ex2: Debug information for when 1bit parser have error
 *		hx_drv_inp1bitparser_get_fsm(&g_hm11B1_info.fsm);
 *		hx_drv_inp1bitparser_get_HW_hvsize(&g_hm11B1_info.hw_hsize, &g_hm11B1_info.hw_vsize);
 *		hx_drv_inp1bitparser_get_hvsize(&g_hm11B1_info.sensor_hsize, &g_hm11B1_info.sensor_vsize);
 *		hx_drv_inp1bitparser_get_framelinelen(&g_hm11B1_info.frame_len, &g_hm11B1_info.line_len);
 *		hx_drv_inp1bitparser_get_again(&g_hm11B1_info.again);
 *		hx_drv_inp1bitparser_get_dgain(&g_hm11B1_info.dgain);
 *		hx_drv_inp1bitparser_get_intg(&g_hm11B1_info.intg);
 *		hx_drv_inp1bitparser_get_intsrc(&g_hm11B1_info.intsrc);
 *		hx_drv_inp1bitparser_get_fstus(&g_hm11B1_info.fstus);
 *		hx_drv_inp1bitparser_get_fc(&g_hm11B1_info.fc);
 *		hx_drv_inp1bitparser_get_crc(&g_hm11B1_info.sensor_crc, &g_hm11B1_info.hw_crc);
 *		hx_drv_inp1bitparser_get_cycle(&g_hm11B1_info.fs_cycle, &g_hm11B1_info.fe_cycle);
 *		hx_drv_inp1bitparser_get_fscycle_err_cnt(&g_hm11B1_info.fs_cycle_err_cnt);
 *		hx_drv_inp1bitparser_get_errstatus(&g_hm11B1_info.err_status);
 *
 * </pre>
 */
/**
 * \defgroup	INP_1BITPARSER_DRV_ENUM	INP 1BITPARSER Driver Enumeration
 * \ingroup	INP_1BITPARSER_DRV
 * \brief	Defines some enumeration of INP 1BITPARSER Driver need.
 * @{
 */
/**
 * \enum INP_1BITPARSER_ERROR_E
 * \brief INP 1bit parser Errors Type
 */
typedef enum
{
	INP_1BITPARSER_NO_ERROR                     = 0,	/**< No ERROR */
	INP_1BITPARSER_ERROR_NOT_INIT               = 1,	/**< ERROR MSG: not init */
	INP_1BITPARSER_ERROR_ALREADY_INIT           = 2,	/**< ERROR MSG: already init */
	INP_1BITPARSER_ERROR_INVALID_PARAMETERS     = 3,	/**< ERROR MSG: Invalid parameters */
	INP_1BITPARSER_UNKNOWN_ERROR      			= 4,	/**< ERROR MSG: UNKNOWN ERROR*/
} INP_1BITPARSER_ERROR_E;

/**
 * \enum INP_1BITPARSER_INTEN_E
 * \brief INP 1bit parser Interrupt Enable
 */
typedef enum
{
	INP_1BITPARSER_INTEN_FS_ERR    	 		= 0x1,	/**< [0] reg reg_inpparser_fs_cycle_error_int_en */
	INP_1BITPARSER_INTEN_HSIZE_ERR  		= 0x2,  /**< [1] reg reg_inpparser_hsize_error_int_en */
	INP_1BITPARSER_INTEN_FE_ERR     		= 0x4,	/**< [2] reg reg_inpparser_fe_cycle_error_int_en */
	INP_1BITPARSER_INTEN_CRC_ERROR      	= 0x08,	/**< [3] reg_inpparser_crc_error_int_en*/
	INP_1BITPARSER_INTEN_BLANK_TOGGLE_ERROR = 0x10,	/**< [4] reg_inpparser_blank_toggle_error_int_en*/
	INP_1BITPARSER_INTEN_FS_TOGGLE_ERROR    = 0x20,	/**< [5] reg_inpparser_wait_fs_toggle_error_int_en*/
	INP_1BITPARSER_INTEN_FD_TOGGLE_ERROR    = 0x40,	/**< [6] reg_inpparser_wait_fd_toggle_error_int_en*/
	INP_1BITPARSER_INTEN_FE_TOGGLE_ERROR    = 0x80,	/**< [7] reg_inpparser_wait_fe_toggle_error_int_en*/
	INP_1BITPARSER_INTEN_FS_HVSIZE_ERROR    = 0x100,/**< [8] reg_inpparser_fs_hsize_vsize_error_int_en*/
} INP_1BITPARSER_INTEN_E;

/**
 * \enum INP_1BITPARSER_INTSTATUS_E
 * \brief INP 1bit parser Interrupt Status
 */
typedef enum
{
	INP_1BITPARSER_INTSTATUS_NO	            = 0x0,  /**< No Interrupt Status */
	INP_1BITPARSER_INTSTATUS_FS_ERR    	 	= 0x1,	/**< [0] reg reg_inpparser_fs_cycle_error_int_en */
	INP_1BITPARSER_INTSTATUS_HSIZE_ERR  	= 0x2,  /**< [1] reg reg_inpparser_hsize_error_int_en */
	INP_1BITPARSER_INTSTATUS_FE_ERR     	= 0x4,	/**< [2] reg reg_inpparser_fe_cycle_error_int_en */
	INP_1BITPARSER_INTSTATUS_CRC_ERROR      = 0x8,	/**< [3] reg_inpparser_crc_error_int_en*/
	INP_1BITPARSER_INTSTATUS_BLANK_TOGGLE_ERROR    = 0x10,	/**< [4] reg_inpparser_blank_toggle_error_int_en*/
	INP_1BITPARSER_INTSTATUS_FS_TOGGLE_ERROR      	= 0x20,	/**< [5] reg_inpparser_wait_fs_toggle_error_int_en*/
	INP_1BITPARSER_INTSTATUS_FD_TOGGLE_ERROR      	= 0x40,	/**< [6] reg_inpparser_wait_fd_toggle_error_int_en*/
	INP_1BITPARSER_INTSTATUS_FE_TOGGLE_ERROR      	= 0x80,	/**< [7] reg_inpparser_wait_fe_toggle_error_int_en*/
	INP_1BITPARSER_INTSTATUS_FS_HVSIZE_ERROR      	= 0x100,/*[8] reg_inpparser_fs_hsize_vsize_error_int_en*/
} INP_1BITPARSER_INTSTATUS_E;

/**
 * \enum INP_1BITPARSER_ORDER_E
 * \brief INP 1bit parser endian
 */
typedef enum
{
	INP_1BITPARSER_ORDER_MSB    = 0x0,	/**<  MSB*/
	INP_1BITPARSER_ORDER_LSB  	= 0x1,  /**<  LSB*/
} INP_1BITPARSER_ORDER_E;

/**
 * \enum INP_1BITPARSER_ORDER_E
 * \brief INP 1bit parser endian
 */
typedef enum
{
	INP_1BITPARSER_FE_BYTE_SWAP_DISABLE    = 0x0,	/**<  SWAP Disable*/
	INP_1BITPARSER_FE_BYTE_SWAP_ENABLE  	= 0x1,  /**<  SWAP Enable*/
} INP_1BITPARSER_FE_SWAP_E;


/**
 * \enum INP_1BITPARSER_FSM_E
 * \brief INP 1bit parser Finite State Machine
 */
typedef enum
{
	INP_1BITPARSER_FSM_IDLE    		   = 0x0,  /**<  IDEL*/
	INP_1BITPARSER_FSM_DLY     		   = 0x1,  /**<  DLY*/
	INP_1BITPARSER_FSM_WAIT_TOGGLE     = 0x2,  /**<  FSM_WAIT_TOGGLE*/
	INP_1BITPARSER_FSM_FS     		   = 0x3,  /**<  FSM_FS*/
	INP_1BITPARSER_FSM_FD     		   = 0x4,  /**<  FSM_FD*/
	INP_1BITPARSER_FSM_FE     		   = 0x5,  /**<  FSM_FE*/
	INP_1BITPARSER_FSM_RST     		   = 0x6,  /**<  FSM_RST*/
} INP_1BITPARSER_FSM_E;
/** @} */

/**
 * \defgroup	INP_1BITPARSER_DRV_REGSTRUCT	INP 1BITPARSER Driver Structure
 * \ingroup	INP_1BITPARSER_DRV
 * \brief	contains definitions of INP 1BITPARSER Driver structure.
 * @{
 */
/**
 * \struct INP1BITPARSER_CFG_T
 * \brief INP 1BITPARSER configuration by INP 1BITPARSER Driver
 */
typedef struct
{
	uint8_t inpparser_enable;/**<  inp 1bit parser enable*/
	INP_1BITPARSER_ORDER_E inpparser_FS_order_sel;/**<  FS INP 1bit parser data endian*/
	INP_1BITPARSER_ORDER_E inpparser_FD_order_sel;/**<  FD INP 1bit parser data endian*/
	INP_1BITPARSER_ORDER_E inpparser_FE_order_sel;/**<  FE INP 1bit parser data endian*/
	INP_1BITPARSER_FE_SWAP_E fe_swap;
	uint16_t inpparser_tg2utg;/**<  FS INP 1bit parser Toggle to untoggle detection cycle */
	uint32_t inpparser_blank_timeout_th;/**<  FS INP 1bit parser Time out threshold (data stalling) */
	uint16_t inpparser_error_status2int_en;/**<  inp 1bit parser error interrupt enable*/
	uint16_t inpparser_swhsize;/**<  inp 1bit parser hsize configuration*/
	uint16_t inpparser_swvsize;/**<  inp 1bit parser vsize configuration*/
	uint16_t inpparser_fs_cycle_err_th;/**<  inp 1bit parser Threshold of FS cycle error counter*/
} INP1BITPARSER_CFG_T;
/** @} */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \defgroup	INP_1BITPARSER_DRV_FUNCDLR	INP 1BITPARSER Driver Function Declaration
 * \ingroup	INP_1BITPARSER_DRV
 * \brief	Contains declarations of INP 1BITPARSER Driver functions.
 * @{
 */
/**
 * \brief	INP1BITPARSER callback function
 *
 * \return	void.
 */
typedef void (*INP1BITPARSER_ISREvent_t) (uint16_t event);

/**
 * \brief	initial INP1BITPARSER Driver
 *
 * \param[in]	reg_baseaddr	 	inp reg base address
 * \param[in]	reg_lsc_baseaddr	inp LSC reg base address
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_init(uint32_t reg_baseaddr, uint32_t reg_lsc_baseaddr);

/**
 * \brief	set all inp1bitparser related Setting
 *
 * \param[in]	inpparser_cfg	 inp parser configuration
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_set_allCfg(INP1BITPARSER_CFG_T inpparser_cfg);

/**
 * \brief	get default inp1bitparser related Setting
 *
 * \param[out]	inpparser_cfg	 inp parser configuration
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_defCfg(INP1BITPARSER_CFG_T *inpparser_cfg);

/**
 * \brief	set inp1bitparser enable
 *
 * \param[in]	enable	 inp parser enable
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_set_enable(uint8_t enable);

/**
 * \brief	get inp1bitparser enable
 *
 * \param[out]	enable	 inp parser enable
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_enable(uint8_t *enable);

/**
 * \brief	set inp1bitparser data order
 *
 * \param[in]	fs	 FS data endian
 * \param[in]	fd	 FD data endian
 * \param[in]	fe	 FE data endian
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_set_order(INP_1BITPARSER_ORDER_E fs, INP_1BITPARSER_ORDER_E fd, INP_1BITPARSER_ORDER_E fe);

/**
 * \brief	get inp1bitparser data order
 *
 * \param[out]	fs	 FS data endian
 * \param[out]	fd	 FD data endian
 * \param[out]	fe	 FE data endian
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_order(INP_1BITPARSER_ORDER_E *fs, INP_1BITPARSER_ORDER_E *fd, INP_1BITPARSER_ORDER_E *fe);

/**
 * \brief	set inp1bitparser FE Byte Swap
 *
 * \param[in]	fe	 FE Byte Swap
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_set_fe_swap(INP_1BITPARSER_FE_SWAP_E fe_swap);

/**
 * \brief	get inp1bitparser FE Byte SWAP
 *
 * \param[out]	fe_swap	 FE Byte Swap
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_fe_swap(INP_1BITPARSER_FE_SWAP_E *fe_swap);

/**
 * \brief	set inp1bitparser tg2utg
 *
 * \param[in]	tg2utg	 Toggle to untoggle detection cycle
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_set_tg2utg(uint16_t tg2utg);

/**
 * \brief	get inp1bitparser tg2utg
 *
 * \param[out]	tg2utg	 Toggle to untoggle detection cycle
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_tg2utg(uint16_t *tg2utg);

/**
 * \brief	set inp1bitparser blank timeout
 *
 * \param[in]	blanktimeout	 Time out threshold (data stalling)
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_set_blanktimeout(uint32_t blanktimeout);

/**
 * \brief	get inp1bitparser blank timeout
 *
 * \param[in]	blanktimeout	 Time out threshold (data stalling)
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_blanktimeout(uint32_t *blanktimeout);

/**
 * \brief	set inp1bitparser errinten
 *
 * \param[in]	errinten	 error interrupt enable
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_set_errinten(uint16_t errinten);

/**
 * \brief	get inp1bitparser errinten
 *
 * \param[out]	errinten	 error interrupt enable
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_errinten(uint16_t *errinten);

/**
 * \brief	get inp1bitparser error status
 *
 * \param[out]	errstatus	 error interrupt status
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_errstatus(uint16_t *errstatus);

/**
 * \brief	get inp1bitparser fsm
 *
 * \param[out]	fsm	 Finite state machine state
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_fsm(INP_1BITPARSER_FSM_E *fsm);

/**
 * \brief	get inp1bitparser HW calculation hvsize
 *
 * \param[out]	hsize	hardware calculation input width
 * \param[out]	vsize	hardware calculation input height
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_HW_hvsize(uint16_t *hsize, uint16_t *vsize);

/**
 * \brief	get inp1bitparser header hvsize
 *
 * \param[out]	hsize	header input width
 * \param[out]	vsize	header input height
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_hvsize(uint16_t *hsize, uint16_t *vsize);

/**
 * \brief	get inp1bitparser header frame length, line length
 *
 * \param[out]	frame_len	header frame length
 * \param[out]	line_len	header line length
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_framelinelen(uint16_t *frame_len, uint16_t *line_len);

/**
 * \brief	get inp1bitparser header again
 *
 * \param[out]	again	header again
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_again(uint8_t *again);

/**
 * \brief	get inp1bitparser header dgain
 *
 * \param[out]	dgain	header dgain
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_dgain(uint16_t *dgain);

/**
 * \brief	get inp1bitparser integration time
 *
 * \param[out]	intg	header integration time
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_intg(uint16_t *intg);

/**
 * \brief	get inp1bitparser interrupt source
 *
 * \param[out]	intsrc	header interrupt source
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_intsrc(uint8_t *intsrc);

/**
 * \brief	get inp1bitparser fstus
 *
 * \param[out]	fstus	header fstus information
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_fstus(uint16_t *fstus);

/**
 * \brief	get inp1bitparser frame counter
 *
 * \param[out]	fc	header frame counter information
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_fc(uint16_t *fc);

/**
 * \brief	get inp1bitparser crc
 *
 * \param[out]	sensor_crc	sensor header crc
 * \param[out]	hw_crc	hardware calculation crc
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_crc(uint16_t *sensor_crc,uint16_t *hw_crc);

/**
 * \brief	inp1bitparser register callback function
 *
 * \param[in]	cb_event	interrupt callback function
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_err_register_cb(INP1BITPARSER_ISREvent_t  cb_event);

/**
 * \brief	inp1bitparser clear interrupt
 *
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_clear_int();

/**
 * \brief	inp1bitparser get detect fs cycle and fe cycle(debug purpose)
 *
 * \param[out]	fs_cycle	fs cycle
 * \param[out]	fe_cycle	fe cycle
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_cycle(uint16_t *fs_cycle, uint16_t *fe_cycle);

/**
 * \brief	inp1bitparser set inp1bitparser sw hsize, vsize
 *
 * \param[in]	sw_hsize	hsize setting
 * \param[in]	sw_vsize	vsize setting
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_set_sw_hvsize(uint16_t sw_hsize, uint16_t sw_vsize);

/**
 * \brief	inp1bitparser get inp1bitparser sw_hsize, vsize
 *
 * \param[out]	sw_hsize	hsize setting
 * \param[out]	sw_vsize	vsize setting
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_sw_hvsize(uint16_t *sw_hsize, uint16_t *sw_vsize);

/**
 * \brief	inp1bitparser set inp1bitparser reg_inpparser_fs_cycle_error_cnt_th
 *
 * \param[in]	fs_cycle_err_th	  FS cycle error counter
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_set_fscycle_err_th(uint16_t fs_cycle_err_th);

/**
 * \brief	inp1bitparser get inp1bitparser reg_inpparser_fs_cycle_error_cnt_th
 *
 * \param[out]	fs_cycle_err_cnt	  FS cycle error counter
 * \return	INP_1BITPARSER_ERROR_E.
 */
INP_1BITPARSER_ERROR_E hx_drv_inp1bitparser_get_fscycle_err_cnt(uint16_t *fs_cycle_err_cnt);
/** @} */
#ifdef __cplusplus
}
#endif
#endif /* INC_HX_DRV_INP1BITPARSER_H_ */
