

#ifndef HXMB_HXEVENT_H_
#define HXMB_HXEVENT_H_

#include "stdint.h"
#include "hx_drv_mb.h"

/**
 * \enum MBLIB_CM55BIG_EVT_E
 * \brief PM API ERROR Code
 */
typedef enum MBLIB_ERR_S
{
	MBLIB_ERR_NO_ERROR                = 0,	/**< No Error */
	MBLIB_ERR_MUTEX_ACQUIRE_FAIL      = 1,	/**< MUTEX_ACQUIRE_FAIL */
	MBLIB_ERR_VAD_BUF_FULL_ERR		  = 2,	/**< VAD BUFF FULL  */
	MBLIB_ERR_VAD_BUF_COPY_ERR		  = 3,	/**< VAD COPYs  */
	MBLIB_ERR_UNKNOWN_ERR			  = 0xFF,	/**< Unknown FAIL  */
} MBLIB_ERR_E;

/**
 * \enum MBLIB_CM55BIG_EVT_E
 * \brief PM API Event Code
 */
typedef enum MBLIB_CM55BIG_EVT_S
{
	MBLIB_CM55BIG_EVT_NO                = 0,	/**< No EVENT */
	MBLIB_CM55BIG_EVT_VAD_BUF1_RDY      = 0x01,	/**< VAD BUF1 Ready */
	MBLIB_CM55BIG_EVT_VAD_BUF2_RDY		= 0x02,	/**< VAD BUF2 Ready  */
	MBLIB_CM55BIG_EVT_CM55S_RDY			= 0x04,	/**< CM55S Ready  */
	MBLIB_CM55BIG_EVT_CM55M_DC			= 0x08,	/**< CM55M Ready for Dual Core  */
	MBLIB_CM55BIG_EVT_CM55S_AUDIO_STOP  = 0x10,	/**< CM55S STOP AUDIO */
	MBLIB_CM55BIG_EVT_RES		= 0x80000000,	/**< RES  */
} MBLIB_CM55BIG_EVT_E;


/**
 * \enum MBLIB_MUTEX_SHAREDATA_IDX_E
 * \brief PM API ERROR Code
 */
typedef enum MBLIB_MUTEX_SHAREDATA_IDX_S
{
	MBLIB_MUTEX_SHAREDATA_IDX_VADBUF1      = 0,	/**< VADBUF1 */
	MBLIB_MUTEX_SHAREDATA_IDX_VADBUF2      ,/**< VADBUF2 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES3        ,/**< RES3 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES4        ,/**< RES4 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES5        ,/**< RES5 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES6        ,/**< RES6 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES7        ,/**< RES7 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES8        ,/**< RES8 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES9        ,/**< RES9 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES10       ,/**< RES10 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES11       ,/**< RES11 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES12       ,/**< RES12 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES13        ,/**< RES13 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES14        ,/**< RES14 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES15        ,/**< RES15 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES16        ,/**< RES16 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES17        ,/**< RES17 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES18        ,/**< RES18 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES19        ,/**< RES19 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES20       ,/**< RES20 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES21       ,/**< RES21 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES22       ,/**< RES22 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES23        ,/**< RES23 */
	MBLIB_MUTEX_SHAREDATA_IDX_RES24        /**< RES24 */
} MBLIB_MUTEX_SHAREDATA_IDX_E;

//#ifdef CM55_BIG
/**
 * \brief	mb lib init
 *
 * \param[in]	cb_event	CM55M MB Interrupt callback function
 * \return	MBLIB_ERR_E.
 */
MBLIB_ERR_E hx_lib_mb_cm55m_init(CM55M_MB_ISREvent_t  cb_event);

/**
 * \brief	register CM55M MB Interrupt Status callback
 *
 * \param[in]	cb_event	CM55M MB Interrupt callback function
 * \return	MBLIB_ERR_E.
 */
MBLIB_ERR_E hx_lib_mb_register_cm55m_cb(CM55M_MB_ISREvent_t  cb_event);

/**
 * \brief	copy vad share buffer information and copy data to target memory
 *
 * \param[in]	buf_no	VAD Buffer Number Count
 * \param[in]	start_addr	VAD target start address for CM55M
 * \param[out]	size	VAD memory size for CM55M
 * \return	MBLIB_ERR_E.
 */
MBLIB_ERR_E hx_lib_mb_copy_vad_data(uint8_t buf_no, uint32_t start_addr, uint32_t *size);

/**
 * \brief	get cm55m event
 *
 * \param[out]	evt	CM555M MB Event CB
 * \return	void.
 */
void hx_lib_mb_cm55m_get_event(MBLIB_CM55BIG_EVT_E *evt);

/**
 * \brief	set CM55M DC RDY
 *
 * \return	void.
 */
void hx_lib_mb_set_cm55m_dc_rdy();
//#else
/**
 * \brief	cm55s mb lib init
 *
 * \param[in]	cb_event	CM55S MB Interrupt callback function
 * \return	MBLIB_ERR_E.
 */
MBLIB_ERR_E hx_lib_mb_cm55s_init(CM55S_MB_ISREvent_t  cb_event);

/**
 * \brief	register CM55S MB Interrupt Status callback
 *
 * \param[in]	cb_event	CM55S MB Interrupt callback function
 * \return	MBLIB_ERR_E.
 */
MBLIB_ERR_E hx_lib_mb_register_cm55s_cb(CM55S_MB_ISREvent_t  cb_event);

/**
 * \brief	set CM55S RDY
 *
 * \return	void.
 */
void hx_lib_mb_set_cm55s_rdy();


/**
 * \brief	set vad event
 *
 * \param[in]	start_addr	VAD buffer start address
 * \param[in]	end_addr	VAD buffer end address
 * \param[in]	force_replace	Force replace even buffer not consume
 * \return	MBLIB_ERR_E.
 */
MBLIB_ERR_E hx_lib_mb_set_vad_event(uint32_t start_addr, uint32_t end_addr, uint8_t force_replace);


//#endif

#endif /* HXMB_HXEVENT_H_ */
