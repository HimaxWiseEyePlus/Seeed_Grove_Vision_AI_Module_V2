/*
 * hx_drv_inpovparser.h
 *
 *  Created on: 2022¦~6¤ë30¤é
 *      Author: 901912
 */

#ifndef INC_HX_DRV_INPOVPARSER_H_
#define INC_HX_DRV_INPOVPARSER_H_

#include "dev_common.h"
/**
 * \defgroup	INP_OVPARSER_DRV	INP OVPARSER Driver
 * \ingroup		INP_OVPARSER_DRV
 * \brief	INP OVPARSER Driver Declaration
 * \details This driver is for parsing OV02C10 Serial interface protocol. Almost control is handle by datapath library.\n
 * <pre>
 * Ex:  Debug information for when ov parser have error
 *		hx_drv_inpovparser_get_fsm(&g_hm11B1_info.fsm);
 *		hx_drv_inpovparser_get_HW_hvsize(&g_hm11B1_info.hw_hsize, &g_hm11B1_info.hw_vsize);
 *		hx_drv_inpovparser_get_hvsize(&g_hm11B1_info.sensor_hsize, &g_hm11B1_info.sensor_vsize);
 *		hx_drv_inpovparser_get_framelinelen(&g_hm11B1_info.frame_len, &g_hm11B1_info.line_len);
 *		hx_drv_inpovparser_get_again(&g_hm11B1_info.again);
 *		hx_drv_inpovparser_get_dgain(&g_hm11B1_info.dgain);
 *		hx_drv_inpovparser_get_intg(&g_hm11B1_info.intg);
 *		hx_drv_inpovparser_get_intsrc(&g_hm11B1_info.intsrc);
 *		hx_drv_inpovparser_get_fstus(&g_hm11B1_info.fstus);
 *		hx_drv_inpovparser_get_fc(&g_hm11B1_info.fc);
 *		hx_drv_inpovparser_get_crc(&g_hm11B1_info.sensor_crc, &g_hm11B1_info.hw_crc);
 *		hx_drv_inpovparser_get_cycle(&g_hm11B1_info.fs_cycle, &g_hm11B1_info.fe_cycle);
 *		hx_drv_inpovparser_get_fscycle_err_cnt(&g_hm11B1_info.fs_cycle_err_cnt);
 *		hx_drv_inpovparser_get_errstatus(&g_hm11B1_info.err_status);
 *
 * </pre>
 */
/**
 * \defgroup	INP_OVPARSER_DRV_ENUM	INP OVPARSER Driver Enumeration
 * \ingroup	INP_OVPARSER_DRV
 * \brief	Defines some enumeration of INP OVPARSER Driver need.
 * @{
 */
/**
 * \enum INP_OVPARSER_ERROR_E
 * \brief INP ov parser Errors Type
 */
typedef enum
{
	INP_OVPARSER_NO_ERROR                     = 0,	/**< No ERROR */
	INP_OVPARSER_ERROR_NOT_INIT               = 1,	/**< ERROR MSG: not init */
	INP_OVPARSER_ERROR_ALREADY_INIT           = 2,	/**< ERROR MSG: already init */
	INP_OVPARSER_ERROR_INVALID_PARAMETERS     = 3,	/**< ERROR MSG: Invalid parameters */
	INP_OVPARSER_UNKNOWN_ERROR      			= 4,	/**< ERROR MSG: UNKNOWN ERROR*/
} INP_OVPARSER_ERROR_E;

/**
 * \enum INP_OVPARSER_BITORDER_E
 * \brief INP ov parser BIT ORDER
 */
typedef enum
{
	INP_OVPARSER_SOF_GOTO_PKT  			= 0x100,			/**< FSM from PARA_SOF goto SIZE_PKT state  */
	INP_OVPARSER_SYNCxSOL_GOTO_PKT  	= 0x1000,			/**< FSM from SYNCxSOL goto SIZE_PKT state */
	INP_OVPARSER_SOL_GOTO_PKT  			= 0x10000,			/**< FSM from PARA_SOL goto SIZE_PKT state */
	INP_OVPARSER_SYNCxSOD_GOTO_PKT  	= 0x100000,			/**< FSM from SYNCxSOD goto SIZE_PKT state */
	INP_OVPARSER_PKT_SIZE_SEL_SW_HSIZE  = 0x1000000,		/**< PKT_SIZE is decided by REG_OV02C_SW_HSIZE */
	INP_OVPARSER_NO_CRC  				= 0x10000000,		/**< NO CRC for each PKT_SIZE of data */
} INP_OVPARSER_FSM_SETTING_E;

/**
 * \enum INP_OVPARSER_BITORDER_E
 * \brief INP ov parser BIT ORDER
 */
typedef enum
{
	INP_OVPARSER_BITORDER_MSB_FIRST  	= 0,	/**< MSB FIRST BIT ORDER */
	INP_OVPARSER_BITORDER_LSB_FIRST  	= 1		/**< LSB FIRST BIT ORDER */
} INP_OVPARSER_BITORDER_E;

/**
 * \enum INP_OVPARSER_ACT_VS_E
 * \brief INP ov parser SOF/SOD ACT VS
 */
typedef enum
{
	INP_OVPARSER_SOD_ACT_VS  	= 0,	/**< SOD ACT VS */
	INP_OVPARSER_SOF_ACT_VS  	= 1		/**< SOF ACT VS */
} INP_OVPARSER_ACT_VS_E;

/**
 * \enum INP_OVPARSER_FSM_STS_E
 * \brief INP ov parser FSM STATUS
 */
typedef enum
{
	INP_OVPARSER_FSM_LSH_IDLE  	= 0,		/**< Idle state */
	INP_OVPARSER_FSM_SYNC_SOF 	= 1,		/**< [SYNC][SOF_OP] */
	INP_OVPARSER_FSM_PARA_SOF 	= 2,		/**< [DATA_ID][WIDTH][HEIGHT]  //SOI_1BIT: [EXPOSURE][GAIN] */
	INP_OVPARSER_FSM_SYNC_XXX 	= 3,		/**< [SYNC][XXX_OP] */
	INP_OVPARSER_FSM_PARA_SOL 	= 4,		/**< [LINE_ID] */
	INP_OVPARSER_FSM_PARA_SOD 	= 5,		/**< [PKT_SIZE] */
	INP_OVPARSER_FSM_SIZE_PKT 	= 6,		/**< [DATA] */
	INP_OVPARSER_FSM_SIZE_CRC 	= 7			/**< [CRC] */
} INP_OVPARSER_FSM_STS_E;

/**
 * \enum INP_OVPARSER_ERR_STS_E
 * \brief INP ov parser ERROR STATUS
 */
typedef enum
{
	INP_OVPARSER_ERR_PKT  	= 0x01,		/**< OV02C_ERR_PKT */
	INP_OVPARSER_ERR_HSIZE 	= 0x02,		/**< OV02C_ERR_HSIZE */
	INP_OVPARSER_ERR_VSIZE 	= 0x04,		/**< OV02C_ERR_VSIZE */
} INP_OVPARSER_ERR_STS_E;
/** @} */

/**
 * \defgroup	INP_OVPARSER_DRV_REGSTRUCT	INP OVPARSER Driver Structure
 * \ingroup	INP_OVPARSER_DRV
 * \brief	contains definitions of INP OVPARSER Driver structure.
 * @{
 */
/**
 * \struct INPOVPARSER_CFG_T
 * \brief INP OVPARSER configuration by INP OVTPARSER Driver
 */
typedef struct
{
	uint8_t inpovparser_enable;						/**<  inp ov parser enable*/
	INP_OVPARSER_BITORDER_E inpovparser_lsb_first;  /**<  inp ov parser lsb first*/
	INP_OVPARSER_ACT_VS_E inpovparser_act_vs;  		/**<  inp ov parser SOF ACT VS*/
#if(IC_VERSION >= 30)
	uint8_t  inpovparser_SOI_enable;				/**<  inp ov parser enable SOI SENSOR mode*/
	uint16_t inpovparser_sw_hsize;					/**<  inp ov parser SW setting Hsize*/
	uint16_t inpovparser_sw_vsize;					/**<  inp ov parser SW setting Vsize*/
	uint8_t  inpovparser_sts2int_en;				/**<  inp ov parser error status to interrupt enable*/
	uint16_t inpovparser_OP_SOF;					/**<  inp ov parser OPCODE for SOF*/
	uint16_t inpovparser_OP_SOL;					/**<  inp ov parser OPCODE for SOL*/
	uint16_t inpovparser_OP_SOD;					/**<  inp ov parser OPCODE for SOD*/
	uint16_t inpovparser_OP_EOF;					/**<  inp ov parser OPCODE for EOF*/
	uint8_t  inpovparser_presync_byte;				/**<  inp ov parser SYNC_SOF pre-1byte zero check bit-mask*/
	uint8_t  inpovparser_SOF_bytenum;				/**<  inp ov parser SOF byte number (Max 5)*/
	uint32_t inpovparser_FSM;						/**<  inp ov parser FSM setting*/
	uint8_t	 inpovparser_dbg_rec4byte;				/**<  inp ov parser debug record 4 byte*/
#endif
} INPOVPARSER_CFG_T;

/**
 * \struct INPOVPARSER_SOFPARA_T
 * \brief INP OVPARSER SOF parameters by INP OVTPARSER Driver
 */
typedef struct
{
	uint8_t dataid;							/**<  DATAID of SOF*/
	uint16_t width;							/**<  IMAGE WIDTH of SOF*/
	uint16_t height;						/**<  IMAGE HEIGHT of SOF*/
} INPOVPARSER_SOFPARA_T;

/**
 * \struct INPOVPARSER_SOLPARA_T
 * \brief INP OVPARSER SOL parameters by INP OVTPARSER Driver
 */
typedef struct
{
	uint16_t lineid;							/**<  LINEID of SOL*/
} INPOVPARSER_SOLPARA_T;

/**
 * \struct INPOVPARSER_SODPARA_T
 * \brief INP OVPARSER SOD parameters by INP OVTPARSER Driver
 */
typedef struct
{
	uint16_t packet_size;							/**<  PACKET SIZE of SOD*/
} INPOVPARSER_SODPARA_T;
/** @} */

/**
 * \defgroup	INP_OVPARSER_DRV_FUNCDLR	INP OVPARSER Driver Function Declaration
 * \ingroup	INP_OVPARSER_DRV
 * \brief	Contains declarations of INP OVPARSER Driver functions.
 * @{
 */
/**
 * \brief	INPOVPARSER callback function
 *
 * \return	void.
 */
typedef void (*INPOVPARSER_ISREvent_t) (uint16_t event);

/**
 * \brief	initial INPOVPARSER Driver
 *
 * \param[in]	reg_baseaddr	 	inpov reg base address
 * \param[in]	reg_lsc_baseaddr	inpov lsc reg base address
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_init(uint32_t reg_baseaddr, uint32_t reg_lsc_baseaddr);

/**
 * \brief	set all inpovparser related Setting
 *
 * \param[in]	inpparser_cfg	 inpov parser configuration
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_set_allCfg(INPOVPARSER_CFG_T inpovparser_cfg);

/**
 * \brief	get default inpovparser related Setting
 *
 * \param[out]	inpparser_cfg	 inpov parser configuration
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_defCfg(INPOVPARSER_CFG_T *inpovparser_cfg);

/**
 * \brief	set inpovparser enable
 *
 * \param[in]	enable	 inpov parser enable
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_set_enable(uint8_t enable);

/**
 * \brief	get inpovparser enable
 *
 * \param[out]	enable	 inpov parser enable
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_enable(uint8_t *enable);

/**
 * \brief	set inpovparser bit order
 *
 * \param[in]	bitorder	bit order
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_set_bitorder(INP_OVPARSER_BITORDER_E bitorder);

/**
 * \brief	get inpovparser bit order
 *
 * \param[out]	bitorder	bit order
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_bitorder(INP_OVPARSER_BITORDER_E *bitorder);

/**
 * \brief	set inpovparser ACT VS
 *
 * \param[in]	actvs	SOD/SOF ACT VS
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_set_actvs(INP_OVPARSER_ACT_VS_E actvs);

/**
 * \brief	get inpovparser ACT VS
 *
 * \param[out]	actvs	SOD/SOF ACT VS
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_actvs(INP_OVPARSER_ACT_VS_E *actvs);

/**
 * \brief	get inpovparser SOF PARAMETERS
 *
 * \param[out]	sof	SOF parameters
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_sof_para(INPOVPARSER_SOFPARA_T *sof);

/**
 * \brief	get inpovparser SOL PARAMETERS
 *
 * \param[out]	sof	SOL parameters
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_sol_para(INPOVPARSER_SOLPARA_T *sol);


/**
 * \brief	get inpovparser SOD PARAMETERS
 *
 * \param[out]	sof	SOD parameters
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_sod_para(INPOVPARSER_SODPARA_T *sod);

#if(IC_VERSION >= 30)
/**
 * \brief	get default inpovparser SOI related Setting
 *
 * \param[out]	inpparser_cfg	 inpov parser SOI configuration
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_SOI_defCfg(INPOVPARSER_CFG_T *inpovparser_cfg);

/**
 * \brief	set inpovparser SOI enable
 *
 * \param[in]	enable	 inpov parser SOI enable
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_set_SOI_enable(uint8_t enable);

/**
 * \brief	get inpovparser SOI enable
 *
 * \param[out]	enable	 inpov parser SOI enable
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_SOI_enable(uint8_t *enable);

/**
 * \brief	set inpovparser SW H/Z size
 *
 * \param[in]	sw_hsize	 SW hsize
 * \param[in]	sw_vsize	 SW vsize
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_set_sw_hvsize(uint16_t sw_hsize, uint16_t sw_vsize);

/**
 * \brief	get inpovparser SW H/Z size
 *
 * \param[out]	sw_hsize	 SW hsize
 * \param[out]	sw_vsize	 SW vsize
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_sw_hvsize(uint16_t *sw_hsize, uint16_t *sw_vsize);

/**
 * \brief	set inpovparser STS2INT enable
 *
 * \param[in]	enable	 inpov parser STS2INT enable
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_set_sts2int_enable(uint8_t enable);

/**
 * \brief	get inpovparser STS2INT enable
 *
 * \param[out]	enable	 inpov parser STS2INT enable
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_sts2int_enable(uint8_t *enable);

/**
 * \brief	set inpovparser OP code
 *
 * \param[in]	sof_code	 SOF OPCODE
 * \param[in]	sol_code	 SOL OPCODE
 * \param[in]	sod_code	 SOD OPCODE
 * \param[in]	eof_code	 EOF OPCODE
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_set_opcode(uint8_t sof_code, uint8_t sol_code, uint8_t sod_code, uint8_t eof_code);

/**
 * \brief	get inpovparser OP code
 *
 * \param[out]	sof_code	 SOF OPCODE
 * \param[out]	sol_code	 SOL OPCODE
 * \param[out]	sod_code	 SOD OPCODE
 * \param[out]	eof_code	 EOF OPCODE
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_opcode(uint8_t *sof_code, uint8_t *sol_code, uint8_t *sod_code, uint8_t *eof_code);

/**
 * \brief	set inpovparser PRESYNC BYTE
 *
 * \param[in]	presync_byte	 inpov parser PRESYNC BYTE MASK
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_set_presync_byte(uint8_t presync_byte);

/**
 * \brief	get inpovparser PRESYNC BYTE
 *
 * \param[out]	presync_byte	 inpov parser PRESYNC BYTE MASK
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_presync_byte(uint8_t *presync_byte);

/**
 * \brief	set inpovparser SOF byte number
 *
 * \param[in]	bytenum	 inpov parser SOF byte number
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_set_sof_bytenum(uint8_t bytenum);

/**
 * \brief	get inpovparser SOF byte number
 *
 * \param[out]	bytenum	 inpov parser SOF byte number
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_sof_bytenum(uint8_t *bytenum);

/**
 * \brief	set inpovparser FSM setting
 *
 * \param[in]	fsm	 inpov parser FSM setting
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_set_fsm_setting(uint32_t fsm);

/**
 * \brief	get inpovparser FSM setting
 *
 * \param[out]	fsm	 inpov parser FSM setting
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_fsm_setting(uint32_t *fsm);

/**
 * \brief	set inpovparser DEBUG record 4 byte enable
 *
 * \param[in]	enable	 inpov parser debug record 4 byte enable
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_set_dbg_rec4byte_enable(uint8_t enable);

/**
 * \brief	get inpovparser DEBUG record 4 byte enable
 *
 * \param[out]	enable	 inpov parser debug record 4 byte enable
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_dbg_rec4byte_enable(uint8_t *enable);

/**
 * \brief	get inpovparser debug record 4 byte
 *
 * \param[out]	data	 inpov parser debug record 4 byte
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_rec4byte(uint32_t *data);

/**
 * \brief	get inpovparser HW V size
 *
 * \param[out]	hw_vsize	 inpov parser HW V size
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_hw_vsize(uint16_t *hw_vsize);

/**
 * \brief	get inpovparser FSM status
 *
 * \param[out]	fsm_sts	 inpov parser FSM status
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_fsm_sts(uint8_t *fsm_sts);

/**
 * \brief	get inpovparser ERROR status
 *
 * \param[out]	err_sts	 inpov parser ERROR status
 * \return	INP_OVPARSER_ERROR_E.
 */
INP_OVPARSER_ERROR_E hx_drv_inpovparser_get_err_sts(uint8_t *err_sts);
#endif

/** @} */

#endif /* INC_HX_DRV_INPOVPARSER_H_ */
