/**
 ********************************************************************************************
 *  @file      evt_cm55mmb.h
 *  @details   This file contains all event handler related function
 *  @author    himax/902452
 *  @version   V1.0.0
 *  @date      14-July-2019
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/
#ifndef EVENT_HANDLER_EVT_CM55MMB_NBAPP_H_
#define EVENT_HANDLER_EVT_CM55MMB_NBAPP_H_
#include <stdint.h>

/**
 * \defgroup	EVT_CM55MMB_NBAPP CM55MMB_NBAPP Event
 * \ingroup	EVENT_HANDLER
 * \brief	CM55MMB_NBAPP EVENT declaration
 */


/****************************************************
 * Constant Definition                              *
 ***************************************************/




/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/
/**
 * \defgroup	CM55MMB_NB_ENUM	CM55MMB_NB Communication Library Enumeration
 * \ingroup	CM55MMB
 * \brief	Defines the required enumeration of CM55MMB_NB library.
 * @{
 */
/**
 * \enum EVT_CM55MMB_NB_ERR_E
 * \brief this enumeration use in CM55MMB library, define the status of CM55MMB_NB process.
 */
typedef enum
{
	EVT_CM55MMB_NBAPP_NO_ERROR	             = 0,	/**< STATUS: No Error*/
	EVT_CM55MMB_NBAPP_ERR_DRVFAIL			 = 1,	/**< STATUS: Driver Not Initialize Correctly */
} EVT_CM55MMB_NBAPP_ERR_E;
/** @} */

/**
 * \enum MB_CM55BIG_EVT_E
 * \brief API Event Code
 */
typedef enum MB_CM55BIG_EVT_S
{
	MB_CM55BIG_EVT_NO                		= 0,	/**< No EVENT */
	MB_CM55BIG_EVT_CM55M_PREROLL			= 0x01,	/**< Trig CM55M PMU VIDEO PREROLLING  */
	MB_CM55BIG_EVT_CM55M_STOP_PREROLL		= 0x02,	/**< Trig CM55M STOP PMU VIDEO PREROLLING  */
	MB_CM55BIG_EVT_CM55M_ALLON_VID  		= 0x04,	/**< Trig CM55M ALLON VIDEO CAPTURE */
	MB_CM55BIG_EVT_CM55M_STOP_ALLON_VID  	= 0x08,	/**< Trig CM55M STOP ALLON VIDEO CAPTURE */
	MB_CM55BIG_EVT_CM55M_CM55S_READY  		= 0x10,	/**< Trig CM55M CM55S READY */

	MB_CM55BIG_EVT_CM55M_SPION  			= 0x1000,	/**< CM55M DEBUG SPI SEND IMAGE */
	MB_CM55BIG_EVT_CM55M_SPIOFF  			= 0x2000,	/**< CM55M DEBUG SPI SEND IMAGE */
	MB_CM55BIG_EVT_RES						= 0x80000000,	/**< RES  */
} MB_CM55BIG_EVT_E;

/**
 * \enum MB_CM55LITTLE_EVT_E
 * \brief API Event Code
 */
typedef enum MB_CM55LITTLE_EVT_S
{
	MB_CM55LITTLE_EVT_NO                		= 0,	/**< No EVENT */
	MB_CM55LITTLE_EVT_CM55M_WARMBOOT_READY  	= 0x01,	/**< Trig CM55S, CM55M WARMBOOT READY */
	MB_CM55LITTLE_EVT_CM55M_WARMBOOT_CDM  		= 0x02,	/**< Trig CM55S, CM55M WARMBOOT CDM */
	MB_CM55LITTLE_EVT_CM55M_ALLON_CDM  			= 0x04,	/**< Trig CM55S, CM55M ALLON CDM */
	MB_CM55LITTLE_EVT_CM55M_PMU_READY  			= 0x08,	/**< Trig CM55S, CM55M PMU READY */
	MB_CM55LITTLE_EVT_CM55M_COLDBOOT_READY   	= 0x10, /**< Trig CM55S, CM55M COLDBOOT READY */
	MB_CM55LITTLE_EVT_CM55M_STOP_ALLON_READY  	= 0x20,	/**< Trig CM55S, CM55M STOP ALLON READY */
	MB_CM55LITTLE_EVT_CM55M_META_READY  		= 0x40,	/**< Trig CM55S, CM55M ALGO META READY */
	MB_CM55LITTLE_EVT_CM55M_ALLON_READY         = 0x80, /**< Trig CM55S, CM55M ALGO META READY */
	MB_CM55LITTLE_EVT_CM55M_CISDP_ERR         	= 0x1000, /**< Trig CM55S, CM55M CISDP ERROR */
} MB_CM55LITTLE_EVT_E;

/**
 * \enum MBLIB_SHAREDATA_IDX_E
 * \brief PM API ERROR Code
 */
typedef enum MB_SHAREDATA_IDX_S
{
    MB_SHAREDATA_IDX_ALGOMETA_ADDR   = 0,    /**< ALGOMETA ADDR */
    MB_SHAREDATA_IDX_RES2        ,/**< RES2 */
    MB_SHAREDATA_IDX_RES3        ,/**< RES3 */
    MB_SHAREDATA_IDX_RES4        ,/**< RES4 */
    MB_SHAREDATA_IDX_RES5        ,/**< RES5 */
    MB_SHAREDATA_IDX_RES6        ,/**< RES6 */
    MB_SHAREDATA_IDX_RES7        ,/**< RES7 */
    MB_SHAREDATA_IDX_RES8        ,/**< RES8 */
    MB_SHAREDATA_IDX_RES9        ,/**< RES9 */
    MB_SHAREDATA_IDX_RES10       ,/**< RES10 */
    MB_SHAREDATA_IDX_RES11       ,/**< RES11 */
    MB_SHAREDATA_IDX_RES12       ,/**< RES12 */
    MB_SHAREDATA_IDX_RES13       ,/**< RES13 */
    MB_SHAREDATA_IDX_RES14       ,/**< RES14 */
    MB_SHAREDATA_IDX_RES15       ,/**< RES15 */
    MB_SHAREDATA_IDX_RES16       ,/**< RES16 */
    MB_SHAREDATA_IDX_RES17       ,/**< RES17 */
    MB_SHAREDATA_IDX_RES18       ,/**< RES18 */
    MB_SHAREDATA_IDX_RES19       ,/**< RES19 */
    MB_SHAREDATA_IDX_RES20       ,/**< RES20 */
    MB_SHAREDATA_IDX_RES21       ,/**< RES21 */
    MB_SHAREDATA_IDX_RES22       ,/**< RES22 */
    MB_SHAREDATA_IDX_RES23       ,/**< RES23 */
    MB_SHAREDATA_IDX_RES24       ,/**< RES24 */
} MB_SHAREDATA_IDX_E;

/**
 * \enum MBLIB_MUTEX_IDX_E
 * \brief PM API ERROR Code
 */
typedef enum MB_MUTEX_IDX_S
{
    MB_MUTEX_IDX_ALGOMETA_UPDATE      = 0,   /**< ALGOMETA_UPDATE */
    MB_MUTEX_IDX_RES2         ,/**< VADBUF2 */
    MB_MUTEX_IDX_RES3         ,/**< RES3 */
    MB_MUTEX_IDX_RES4         ,/**< RES4 */
    MB_MUTEX_IDX_RES5         ,/**< RES5 */
    MB_MUTEX_IDX_RES6         ,/**< RES6 */
    MB_MUTEX_IDX_RES7         ,/**< RES7 */
    MB_MUTEX_IDX_RES8         ,/**< RES8 */
    MB_MUTEX_IDX_RES9         ,/**< RES9 */
    MB_MUTEX_IDX_RES10        ,/**< RES10 */
    MB_MUTEX_IDX_RES11        ,/**< RES11 */
    MB_MUTEX_IDX_RES12        ,/**< RES12 */
    MB_MUTEX_IDX_RES13        ,/**< RES13 */
    MB_MUTEX_IDX_RES14        ,/**< RES14 */
    MB_MUTEX_IDX_RES15        ,/**< RES15 */
    MB_MUTEX_IDX_RES16        ,/**< RES16 */
    MB_MUTEX_IDX_RES17        ,/**< RES17 */
    MB_MUTEX_IDX_RES18        ,/**< RES18 */
    MB_MUTEX_IDX_RES19        ,/**< RES19 */
    MB_MUTEX_IDX_RES20        ,/**< RES20 */
    MB_MUTEX_IDX_RES21        ,/**< RES21 */
    MB_MUTEX_IDX_RES22        ,/**< RES22 */
    MB_MUTEX_IDX_RES23        ,/**< RES23 */
    MB_MUTEX_IDX_RES24        /**< RES24 */
} MB_MUTEX_IDX_E;


typedef enum MB_CM55M_STATE_S
{
    MB_CM55M_STATE_INIT             = 0,        /**< CM55M INIT STATE */
    MB_CM55M_STATE_COLDBOOT_DONE          = 1,       /**< CM55M COLDBOOT DONE */
    MB_CM55M_STATE_SWITCH_TO_PMU_REROLL   = 2,       /**< CM55M SWITCH TO PMU PREROLL */
    MB_CM55M_STATE_PMU_REROLL             = 3,       /**< CM55M PMU PREROLL */
    MB_CM55M_STATE_SWITCH_TO_ALLON_REROLL = 4,       /**< CM55M SWITCH TO ALLON PREROLL */
    MB_CM55M_STATE_ALLON_REROLL           = 5,       /**< CM55M ALLON PREROLL */
    MB_CM55M_STATE_STOP_ALLON             = 6,       /**< CM55M STOP ALLON */
    MB_CM55M_STATE_STOP_PMU               = 7,       /**< CM55M STOP PMU */
} MB_CM55M_STATE_E;
/****************************************************
 * Function Declaration                             *
 ***************************************************/
/**
 * \defgroup	EVT_CM55MMB_FUNCDLR	CM55M MAILBOX Event Function Declaration
 * \ingroup	EVENT_HANDLER
 * \brief	Contains declarations of CM55M MAILBOX Event functions.
 * @{
 */

/**
 * \brief   Init CM55M MB
 *
 * Init CM55MMB process
 * \retval  EVT_CM55MMB_NO_ERROR    no error
 * \retval  others  error
 */
EVT_CM55MMB_NBAPP_ERR_E evt_cm55mmb_nbapp_init(void);

/**
 * \brief MAILBOX user define callback function type definition
 *
 * \param[in] event MAILBOX user define callback event
 * \retval void.
 */
typedef void (*evthandlermbox_CBEvent_t) (MB_CM55BIG_EVT_E event);

void evt_cm55mmb_setup_cb(evthandlermbox_CBEvent_t cb);

uint8_t evt_cm55mmb_preroll_cb(void);
uint8_t evt_cm55mmb_stop_preroll_cb(void);
uint8_t evt_cm55mmb_allon_vid_cb(void);
uint8_t evt_cm55mmb_stop_allon_vid_cb(void);
uint8_t evt_cm55mmb_cm55s_rdy_cb(void);
uint8_t evt_cm55mmb_spion_cb(void);
uint8_t evt_cm55mmb_spioff_cb(void);




/** @} */	//EVT_CM55MMB_FUNCDLR

#endif /* EVENT_HANDLER_EVT_CM55MMB_NBAPP_H_ */
