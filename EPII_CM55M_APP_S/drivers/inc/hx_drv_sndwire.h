/*
 * hx_drv_sndwiare.h
 *
 *  Created on: 2023�~2��20��
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_SNDWIRE_H_
#define DRIVERS_INC_HX_DRV_SNDWIRE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
/**
 * \defgroup	SNDWIRE_DRV	SNDWIRE Driver
 * \ingroup		SNDWIRE_DRV
 * \brief	SNDWIRE Driver Declaration
 * \details Application can use SNDWIRE Structure and library to control Hardware\n
 * <pre>
 * For Example
 *
 * </pre>
 */
/**
 * \defgroup	SNDWIRE_DRV_ENUM	SNDWIRE Hardware accelerator Driver Enumeration
 * \ingroup	SNDWIRE_DRV
 * \brief	Defines some enumeration of SNDWIRE Hardware accelerator Driver need.
 * @{
 */

/**
 * \enum SNDWIRE_ERROR_E
 * \brief SNDWIRE Errors Type
 */
typedef enum
{
    SNDWIRE_NO_ERROR                           = 0,    /**< No ERROR */
    SNDWIRE_ERROR_UNKNOWN                     = -1,    /**< ERROR MSG:  */
} SNDWIRE_ERROR_E;

typedef enum
{
    /** Slave is disabled. */
    CSSI_OM_OFF = 0,
    /** Normal slave operation. */
    CSSI_OM_NORMAL = 1,
    /** Slave actively drives data lanes to the low state. */
    CSSI_OM_DATA_LOW = 2,
    /** Slave actively drives data lanes to the high state. */
    CSSI_OM_DATA_HIGH = 6,
} CSSI_OperationMode;

typedef enum
{
    /** Slave is ready for Clock Stop. */
    CSSI_CSR_READY = 1,
    /** Slave is not ready for Clock Stop. */
    CSSI_CSR_NOT_READY = 0,
} CSSI_ClockStopReady;

typedef enum
{
    /** Channel (De)Preparation finished. */
    CSSI_CP_FINISHED = 0,
    /** Channel (De)Preparation is not finished. */
    CSSI_CP_NOT_FINISHED = 1,
} CSSI_ChannelPrepare;

typedef enum
{
    /** Clock Stop Mode 1 - In this mode Slave might enter a deeper power-saving mode that does not retain state while the Clock is stopped, so is not capable of continuing operation seamlessly when the Clock restarts. */
    CSSI_CSM_1 = 1,
    /** Clock Stop Mode 0 - In this mode Slave is capable of continuing operation seamlessly when the Clock is restarted, as though time had stood still while the Clock was stopped. */
    CSSI_CSM_0 = 0,
} CSSI_ClockStopMode;

typedef enum
{
    /** Slave is out of synchronization. */
    CSSI_SS_OUT_OF_SYNC = 0,
    /** Slave is in synchronization. */
    CSSI_SS_IN_SYNC = 1,
} CSSI_SyncStatus;

typedef enum
{
    /** Slave is not enumerated. */
    CSSI_ES_NOT_ENUMERATED = 0,
    /** Master enumerated the Slave. */
    CSSI_ES_ENUMERATED = 1,
} CSSI_EnumeratedStatus;

typedef enum
{
    /** Slave is not enumerated. */
    CSSI_WUE_DISABLED = 0,
    /** Master enumerated the Slave. */
    CSSI_WUE_ENABLED = 1,
} CSSI_WakeUpEnable;

typedef enum
{
    /** SoundWire bus is not in Clock Stop State. */
    CSSI_CSA_DISABLED = 0,
    /** SoundWire bus is in Clock Stop State. */
    CSSI_CSA_ENABLED = 1,
} CSSI_ClockStopActive;

typedef enum
{
    /** Clock Stop Preparation is not active. */
    CSSI_CSP_DISABLED = 0,
    /** Master wants to enter Clock Stop State. */
    CSSI_CSP_ENABLED = 1,
} CSSI_ClockStopPrepare;

typedef enum
{
    /** Slave is under Hard Reset. */
    CSSI_HRS_IN_PROGRESS = 1,
    /** Slave is operating normally. */
    CSSI_HRS_FINISHED = 0,
} CSSI_HardResetStatus;

typedef enum
{
    /** Bulk Register Access */
    CSSI_BPT_BULK_REGISTER_ACCESS = 0,
    /** Implementation defined */
    CSSI_BPT_IMPLEMENTATION_DEFINED = 1,
} CSSI_BptPayloadType;

typedef enum
{
    /** Port Enable Interrupt */
    CSSI_DI_PORT_ENABLE = 2,
    /** Channel Prepare Interrupt */
    CSSI_DI_CHANNEL_PREPARE = 4,
    /** Underrun/Overrun Interrupt */
    CSSI_DI_XRUN = 8,
} CSSI_DpInterrupts;
/** @} */

/**
 * \defgroup	SNDWIRE_DRV_REGSTRUCT	SNDWIRE Driver Structure
 * \ingroup	SNDWIRE_DRV
 * \brief	contains definitions of SNDWIRE Driver structure.
 * @{
 */
/**********************************************************************
 * Forward declarations
 **********************************************************************/
struct CSSI_SlaveStatus;
struct CSSI_ClockStopStatus;
struct CSSI_IpVersion;
struct CSSI_Callbacks;

/**
 * \brief	SNDWIRE interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*Sndwire_ISREvent_t) (uint32_t event);
/**
 * \brief	SNDWIRE PDI0 interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*Sndwire_PDI0_ISREvent_t) (uint32_t event);
/**
 * \brief	SNDWIRE PDI1 interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*Sndwire_PDI1_ISREvent_t) (uint32_t event);
/**
 * \brief	SNDWIRE PDI2 interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*Sndwire_PDI2_ISREvent_t) (uint32_t event);



/**********************************************************************
 * Callbacks
 **********************************************************************/
/** Called when Hard Reset has occurred on SoundWire bus. */
typedef void (*CSSI_HardResetInterruptHandler)(void* pD);

/** Called when Soft Reset has occurred. */
typedef void (*CSSI_SoftResetInterruptHandler)(void* pD);

/** Called when SoundWire bus has entered Clock Stop. */
typedef void (*CSSI_ClockStopActiveInterruptHandler)(void* pD);

/** Called when SoundWire bus has been Waked Up (exited Clock Stop). */
typedef void (*CSSI_WakeUpInterruptHandler)(void* pD);

/** Called when Clock Stop Prepare has been activated. If CSSI_CSR_READY is returned, then Slave will report that it's ready for Clock Stop. Otherwise setClockStopReady can be called to inform the Master that Slave prepared for Clock Stop. If this callback is not being used, then Slave will always report that it's ready for Clock Stop. */
typedef CSSI_ClockStopReady (*CSSI_ClockStopPrepareInterruptHandler)(void* pD);

/** Called when Clock Stop De-Prepare has been activated. */
typedef void (*CSSI_ClockStopDeprepareInterruptHandler)(void* pD);

/** Called when master has enumerated the Slave. */
typedef void (*CSSI_EnumeratedInterruptHandler)(void* pD);

/** Called when BPT Packet has been received. */
typedef void (*CSSI_BptPacketReceivedInterruptHandler)(void* pD);

/** Called when Underrun or Overrun has occurred on a Data Port. */
typedef void (*CSSI_DpXrunInterruptHandler)(void* pD, uint8_t dataPortNumber);

/** Called when Channel Preparation has been activated in a Data Port. setDataPortReady function can be called to finish preparation later. */
typedef CSSI_ChannelPrepare (*CSSI_DpChannelPrepareInterruptHandler)(void* pD, uint8_t dataPortNumber, uint8_t prepareChannelMask);

/** Called when Channel De-Preparation has been activated in a Data Port. setDataPortReady function can be called to finish de-preparation later. */
typedef CSSI_ChannelPrepare (*CSSI_DpChannelDeprepareInterruptHandler)(void* pD, uint8_t dataPortNumber);

/** Called when Channels in Data Port have been enabled */
typedef void (*CSSI_DpChannelEnableInterruptHandler)(void* pD, uint8_t dataPortNumber, uint8_t dataPortEnableMask);

/** Called when Channels in Data Port have been disabled */
typedef void (*CSSI_DpChannelDisableInterruptHandler)(void* pD, uint8_t dataPortNumber);

/** Slave Status */
typedef struct CSSI_SlaveStatus
{
    /** Synchronization Status */
    CSSI_SyncStatus synchronized;
    /** Enumerated Status */
    CSSI_EnumeratedStatus enumerated;
    /** Wake Up Enable */
    CSSI_WakeUpEnable wakeUpEnable;
    /** Bulk Transport Protocol Payload Type, unused if CSSI_BBP_EN = 0 */
    CSSI_BptPayloadType bptPayloadType;
}  CSSI_SlaveStatus;

/** Clock Stop Status */
typedef struct CSSI_ClockStopStatus
{
    /** Clock Stop Mode */
    CSSI_ClockStopMode mode;
    /** Clock Stop Prepare Status */
    CSSI_ClockStopPrepare prepare;
    /** Clock Stop Active Status */
    CSSI_ClockStopActive active;
}  CSSI_ClockStopStatus;

/** IP Version. Can be used by driver to determine version of IP implemented. */
typedef struct CSSI_IpVersion
{
    /** The value is hardwired as a MAJOR_IPVERSION_PARAM parameter. */
    uint16_t majorVersion;
    /** The value is hardwired as a MINOR_IPVERSION_PARAM parameter. */
    uint16_t minorVersion;
}  CSSI_IpVersion;

/** Structure containing pointers to functions defined by user that will be called when specific event occurs. */
typedef struct CSSI_Callbacks
{
    /** Callback for Hard Reset Interrupt. Not used if NULL. */
    CSSI_HardResetInterruptHandler onHardResetInterrupt;
    /** Callback for Soft Reset Interrupt. Not used if NULL. */
    CSSI_SoftResetInterruptHandler onSoftResetInterrupt;
    /** Callback for Clock Stop Active. Not used if NULL. */
    CSSI_ClockStopActiveInterruptHandler onClockStopActiveInterrupt;
    /** Callback for Wake Up. Not used if NULL. */
    CSSI_WakeUpInterruptHandler onWakeUpInterrupt;
    /** Callback for Clock Stop Prepare. Not used if NULL. */
    CSSI_ClockStopPrepareInterruptHandler onClockStopPrepareInterrupt;
    /** Callback for Clock Stop Deprepare. Not used if NULL. */
    CSSI_ClockStopDeprepareInterruptHandler onClockStopDeprepareInterrupt;
    /** Callback for Enumerated. Not used if NULL. */
    CSSI_EnumeratedInterruptHandler onEnumeratedInterrupt;
    /** Callback for BPT Packet Received. Not used if NULL. */
    CSSI_BptPacketReceivedInterruptHandler onBptPacketReceivedInterrupt;
    /** Callback for Data Port Underrun / Overrun. Not used if NULL. */
    CSSI_DpXrunInterruptHandler onDpXrunInterrupt;
    /** Callback for Data Port Channel Preparation. Not used if NULL. */
    CSSI_DpChannelPrepareInterruptHandler onDpChannelPrepareInterrupt;
    /** Callback for Data Port Channel De-Preparation. Not used if NULL. */
    CSSI_DpChannelDeprepareInterruptHandler onDpChannelDeprepareInterrupt;
    /** Callback for Data Port Enable. Not used if NULL. */
    CSSI_DpChannelEnableInterruptHandler onDpChannelEnableInterrupt;
    /** Callback for Data Port Disable. Not used if NULL. */
    CSSI_DpChannelDisableInterruptHandler onDpChannelDisableInterrupt;
}  CSSI_Callbacks;

/**
 * \struct SNDWIRE_PDI_INT_DRV_CB_T
 * \brief SNDWIRE PDI Interrupt configuration by SNDWIRE Driver
 */
typedef struct {
	Sndwire_PDI0_ISREvent_t sndwire_pdi0_cb;						/**< SNDWIRE PDI00 Interrupt CB */
	Sndwire_PDI1_ISREvent_t sndwire_pdi1_cb;						/**< SNDWIRE PDI01 Interrupt CB */
	Sndwire_PDI1_ISREvent_t sndwire_pdi2_cb;						/**< SNDWIRE PDI02 Interrupt CB */
} SNDWIRE_PDI_INT_DRV_CB_T;
/** @} */


/**
 * \brief   SNDWIRE init
 *
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_sndwire_Init(uint8_t *pDM, CSSI_Callbacks *callbacks);

/**
 * \brief   Start the CSSI driver, enabling interrupts.
 *
 * \param[in]	isrcb	Interrupt CB for PDI
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_sndwire_Start(SNDWIRE_PDI_INT_DRV_CB_T pdi_isrcb);

/**
 * \brief   The client may call this to disable the hardware (disabling its IRQ at the source and disconnecting it if applicable).
 *
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_sndwire_Stop();

/**
 * \brief   SNDWIRE This performs an automatic stop and then de-initializes the driver.
 * The client may not make further requests on this instance.
 *
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_sndwire_Destroy();


/**
 * \brief   SNDWIRE Get required memory
 *
 * \param[out]	requiredMemory	required memory
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_sndwire_Probe(uint16_t* requiredMemory);

/**
 * \brief   SNDWIRE Sets Slave's Operation Mode.
 *
 * \param[in]	operationMode	operation
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_sndwire_SetOperationMode(CSSI_OperationMode operationMode);

/**
 * \brief   Enables or disables interrupts for selected Data Port.
 *
 * \param[in]	dataPortNumber	data port number
 * \param[in]	dpInterruptMask	data port interrupt mask
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_sndwire_SetDataPortInterruptMask(uint8_t dataPortNumber, uint32_t dpInterruptMask);

/**
 * \brief   SNDWIRE Set Informs the Master if Slave is Ready for Clock Stop.
 *
 * \param[in]	clockStopReady	Clock Stop Ready Status
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_SetClockStopReady(CSSI_ClockStopReady clockStopReady);

/**
 * \brief   Set Informs the Master that Data Port Preparation or De-Preparation has finished.
 *
 * \param[in]	dataPortNumber	Number of a Data Port.
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_SetDataPortFinishedPreparation(uint8_t dataPortNumber);

/**
 * \brief   Tries to wake up the SoundWire Master while bus is in Clock Stop State.
 *
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_RequestWakeUp();

/**
 * \brief   Sets Slew Rate for the PHY for all SoundWire Lanes.
 *
 * \param[in]	slewRate	New 4-bit Slew rate value.
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_SetSlewRate(uint8_t slewRate);

/**
 * \brief   Calls User Interrupt (ImpDef1 in SCP_IntStat1) with specific vector (SCP_UserIntCode).
 *
 * \param[in]	vector	4-bit User Interrupt Vector.
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_SetUserInterrupt(uint8_t vector);

/**
 * \brief   Informs if User Interrupt (ImpDef1 in SCP_IntStat1) has been processed and cleared by the Master.
 *
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_GetUserInterruptStatus();

/**
 * \brief   Calls ImpDef1 Data Port Interrupt.
 *
 * \param[in]	dataPortNumber	Number of a Data Port.
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_SetDataPortInterrupt(uint8_t dataPortNumber);

/**
 * \brief  Informs if Data Port Interrupt has been processed and cleared by the Master.
 *
 * \param[in]	dataPortNumber	Number of a Data Port.
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_GetDataPortInterruptStatus(uint8_t dataPortNumber);

/**
 * \brief  Returns Clock Stop status.
 *
 * \param[out]	clockStopStatus
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_GetClockStopStatus(CSSI_ClockStopStatus* clockStopStatus);

/**
 * \brief  Returns Slave's status.
 *
 * \param[out]	slaveStatus
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_GetSlaveStatus(CSSI_SlaveStatus* slaveStatus);

/**
 * \brief  Returns Slave's status.
 *
 * \param[out]	slaveStatus
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_GetHardResetStatus(CSSI_HardResetStatus* hardResetStatus);

/**
 * \brief   SNDWIRE Get IP Version
 *
 * \param[in]	ipVersion	IP Version
 * \return  SNDWIRE_ERROR_E.
 */
SNDWIRE_ERROR_E hx_drv_sndwire_getIpVersion(CSSI_IpVersion* ipVersion);

#endif /* DRIVERS_INC_HX_DRV_SNDWIRE_H_ */
