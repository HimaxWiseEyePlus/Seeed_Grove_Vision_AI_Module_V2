/**
 * @file hx_drv_i3c_slv.h
 * @author himax/903730
 * @brief 
 * @version V1.0.0
 * @date 2021-05-09
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */
/**************************************************************************************************/

#ifndef _HX_DRV_I3C_SLV_H_
#define _HX_DRV_I3C_SLV_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>


/**
* \defgroup   I3CS
* \brief      I3C Slave
* \details    I3C (Improved Inter Integrated Circuit) Slave
* <pre>
*   Sample code: basic configuration for I3CS0 initiation
*       /// The output pin of I3CS0 is defined by the user application.
*       hx_drv_scu_set_PA2_pinmux(SCU_PA2_PINMUX_SB_I3C_S_SCL);
*       hx_drv_scu_set_PA3_pinmux(SCU_PA3_PINMUX_SB_I3C_S_SDA);
*
*       /// test pattern
*       #define MAX_FIFO_SIZE           256      //__SCB_DCACHE_LINE_SIZE   
*       #define MAX_IBI_PAYLOAD          32
*       uint8_t i3c_slv_sdr_rx_buf[MAX_FIFO_SIZE] __ALIGNED(__SCB_DCACHE_LINE_SIZE);
*       uint8_t i3c_slv_sdr_tx_buf[MAX_FIFO_SIZE] __ALIGNED(__SCB_DCACHE_LINE_SIZE);
*       uint16_t i3c_slv_ddr_rx_buf[MAX_FIFO_SIZE] __ALIGNED(__SCB_DCACHE_LINE_SIZE);
*       uint16_t i3c_slv_ddr_tx_buf[MAX_FIFO_SIZE] __ALIGNED(__SCB_DCACHE_LINE_SIZE);
*       uint8_t i3c_slv_ibi_tx_buf[MAX_IBI_PAYLOAD] __ALIGNED(__SCB_DCACHE_LINE_SIZE);        
*
*       volatile uint8_t g_i3c_slv_sdr_rx_done = 0;
*
*       void generate_uint8_test_data(uint8_t* data, int number_of_samples)
*       {
*           int i;
*           for (i = 0; i < number_of_samples; i++) {
*               data[i] = i+1;
*           }
*       }
*
*       void generate_uint16_test_data(uint16_t* data, int number_of_samples)
*       {
*           uint16_t i;
*           for (i = 0; i < number_of_samples; i++) {
*               data[i] = ((i * 2) | ((i * 2 + 1) << 8));
*           }
*       }
*
*       /// I3CS callback function 
*       void I3CS_0_SDR_Rx_Handler(void)
*       {   xprintf("\n\n");
*           xprintf("[%s]\n", __FUNCTION__);
*           g_i3c_slv_sdr_rx_done = 1;
*       }
*
*       void I3CS_0_SDR_Tx_Handler(void)
*       {
*           xprintf("\n\n");
*           xprintf("[%s]\n", __FUNCTION__);
*       }
*
*       void I3CS_0_DDR_Rx_Handler(void)
*       {
*           xprintf("\n\n");
*           xprintf("[%s]\n", __FUNCTION__);
*           g_i3c_slv_ddr_rx_done = 1;
*       }
*
*       void I3CS_0_DDR_Tx_Handler(void)
*       {
*           xprintf("\n\n");
*           xprintf("[%s]\n", __FUNCTION__);
*       }
*
*       void I3CS_0_Hot_Join_Handler(bool nacked)
*       {
*           xprintf("\n\n");
*           xprintf("[%s] nacked:%d\n", __FUNCTION__, nacked);
*       }
*
*       void I3CS_0_IBI_Handler(bool nacked)
*       {
*           xprintf("\n\n");
*           xprintf("[%s] nacked:%d\n", __FUNCTION__, nacked);
*       }
*
*       void I3CS_0_Error_Handler(void)
*       {
*           xprintf("\n\n");
*           xprintf("[%s]\n", __FUNCTION__);
*       }       
*
*       /// initializes the I3CS0
*       I3CS_Cus_Callbacks  i3cs_0_callbacks = { 0 };
*       i3cs_0_callbacks.i3cs_sdr_rx_cb = I3CS_0_SDR_Rx_Handler;
*       i3cs_0_callbacks.i3cs_sdr_tx_cb = I3CS_0_SDR_Rx_Handler;
*       i3cs_0_callbacks.i3cs_ddr_rx_cb = I3CS_0_DDR_Rx_Handler;
*       i3cs_0_callbacks.i3cs_ddr_tx_cb = I3CS_0_DDR_Tx_Handler;
*       i3cs_0_callbacks.i3cs_hot_join_cb = I3CS_0_Hot_Join_Handler;
*       i3cs_0_callbacks.i3cs_ibi_cb = I3CS_0_IBI_Handler;
*       i3cs_0_callbacks.i3cs_err_cb = I3CS_0_Error_Handler;
*
*       hx_drv_i3cs_init(I3CS_ID_0, HX_I3C_SLV0_BASE, &i3cs_0_callbacks);
*
*   Case1: Hot-Join request (Dynamic address not assigned)
*       hx_drv_i3cs_hot_join(I3CS_ID_0);
*
*
*   Case2: In-Band interrupt request (Dynamic address assigned) and I3C master enable I3C slave's IBI event.
*       The max payload is 31 bytes.
*
*       /// In-Band interrupt with 8 bytes payload
*       generate_uint8_test_data(i3c_slv_ibi_tx_buf, 8);
*       hx_drv_i3cs_ibi_tx(I3CS_ID_0, i3c_slv_ibi_tx_buf, 8);
*
*
*   Case3: SDR-RX 8 bytes
*       uint32_t retry = 50;
*       uint8_t rx_data_size = 8;
*       g_i3c_slv_sdr_rx_done = 0;
*
*       while(g_i3c_slv_sdr_rx_done == 0){
*           if(retry){
*               hx_drv_timer_delay_ms(TIMER_ID_0, 100, TIMER_STATE_DC);
*               retry--;
*           }else{
*               xprintf("[I3CS] TIMEOUT \n");
*               break;
*           }
*       }
*
*       if(g_i3c_slv_sdr_rx_done == 1)
*       {
*           status = hx_drv_i3cs_sdr_rx(I3CS_ID_0, i3c_slv_sdr_rx_buf, rx_data_size);
*           for(uint32_t i = 0; i < rx_data_size; i++){
*               xprintf("rx_data[%d] = 0x%02x \n", i, i3c_slv_sdr_rx_buf[i]);
*           }
*       }
*
*
*   Case4: SDR-TX 8 bytes
*       uint8_t tx_data_size = 8;
*       generate_uint8_test_data(i3c_slv_sdr_tx_buf, tx_data_size);
*       status = hx_drv_i3cs_sdr_tx(I3CS_ID_0, i3c_slv_sdr_tx_buf, tx_data_size);
*
*
*   Case5: DDR-RX 8 bytes
*       uint32_t retry = 50;
*       uint8_t rx_data_size = 8;
*       g_i3c_slv_ddr_rx_done = 0;
*
*       while(g_i3c_slv_ddr_rx_done == 0){
*           if(retry){
*               hx_drv_timer_delay_ms(TIMER_ID_0, 100, TIMER_STATE_DC);
*               retry--;
*           }else{
*               xprintf("[I3CS] TIMEOUT \n");
*               break;
*           }
*       }
*
*       if(g_i3c_slv_ddr_rx_done == 1)
*       {
*           status = hx_drv_i3cs_ddr_rx(I3CS_ID_0, i3c_slv_ddr_rx_buf, rx_data_size);
*           for(uint32_t i = 0; i < (rx_data_size/2); i++){
*               xprintf("rx_data[%d] = 0x%04x \n", i, i3c_slv_ddr_rx_buf[i]);
*           }
*       }
*
*   Case6: DDR-TX 8 bytes
*       uint8_t tx_data_size = 8;
*       generate_uint16_test_data(i3c_slv_ddr_tx_buf, (tx_data_size/2));
*       status = hx_drv_i3cs_ddr_tx(I3CS_ID_0, i3c_slv_ddr_tx_buf, tx_data_size);
*
*
*
* </pre>
* @{
*/

/***********************************************
 * CONSTANT DEFINITION
 **********************************************/


/***********************************************
 * TYPEDEF DECLARATION
 **********************************************/


/***********************************************
 * EMULATOR DECLARATION
 **********************************************/
/**
 * \enum I3CS_ID_S
 * \brief I3C Slave ID enum 
 */
typedef enum I3CS_ID_S
{
    I3CS_ID_0 = 0,
    I3CS_ID_1,
} I3CS_ID_E;

typedef enum I3CS_ERR_S
{
    I3CS_ERR_EOK           =  0U,       /* no error */
    I3CS_ERR_EPERM         =  1U,       /* Operation not permitted */
    I3CS_ERR_ENOENT        =  2U,       /* No such file or directory */
    I3CS_ERR_EIO           =  5U,       /* I/O error */
    I3CS_ERR_ENOEXEC       =  8U,       /* Exec format error */
    I3CS_ERR_EAGAIN        =  11U,      /* Try again */
    I3CS_ERR_ENOMEM        =  12U,      /* Out of memory */
    I3CS_ERR_EFAULT        =  14U,      /* Bad address */
    I3CS_ERR_EBUSY         =  16U,      /* Device or resource busy */
    I3CS_ERR_EINVAL        =  22U,      /* Invalid argument */
    I3CS_ERR_ENOSPC        =  28U,      /* No space left on device */
    I3CS_ERR_EBADSLT       =  57U,      /* Invalid slot */
    I3CS_ERR_EPROTO        =  71U,      /* Protocol error */
    I3CS_ERR_EOVERFLOW     =  75U,      /* Value too large for defined data type */
    I3CS_ERR_EOPNOTSUPP    =  95U,      /* Operation not supported */
    I3CS_ERR_ETIMEDOUT     =  110U,     /* Connection timed out */
    I3CS_ERR_EINPROGRESS   =  115U,     /* Operation now in progress */
    I3CS_ERR_EDQUOT        =  122U,     /* Quota exceeded */
    I3CS_ERR_ECANCELED     =  126U,     /* Cancelled */
} I3CS_ERR_E;

/**
 * \enum I3CS_PIN_MUX_E
 * \brief I3C Slave PIN Mux enum
 */
typedef enum I3CS_PIN_MUX_S{
    // I3CS_ID_0
    I3CS0_PIN_MUX_PA2PA3_F5_SCLSDA = 0,
    
    // I3CS_ID_1
    I3CS1_PIN_MUX_PB0_F5_SDA,
    I3CS1_PIN_MUX_PB1_F5_SCL,
    I3CS1_PIN_MUX_PB2_F5_SDA,
    I3CS1_PIN_MUX_PB3_F5_SCL,
    I3CS1_PIN_MUX_PB4_F5_SDA,
    I3CS1_PIN_MUX_PB5_F5_SCL,
    I3CS1_PIN_MUX_PB6_F5_SDA,
    I3CS1_PIN_MUX_PB7_F5_SCL,
    I3CS1_PIN_MUX_PB8_F5_SDA,
    I3CS1_PIN_MUX_PB9_F5_SCL,
    I3CS1_PIN_MUX_PB10_F5_SDA,
    I3CS1_PIN_MUX_PB11_F5_SDA,
    I3CS1_PIN_MUX_PA2PA3_F5_SCLSDA
} I3CS_PIN_MUX_E;

/**********************************************************************
 * Callbacks
 **********************************************************************/
/** Transfer Complete event. */
typedef void (*I3CS_TransferCompleteCb)(void);

/** Hot Join interrupt occurred. */
typedef void (*I3CS_HotJoinCb)(bool nacked);

/** In-band interrupt occurred. */
typedef void (*I3CS_InbandInterruptCb)(bool nacked);

/** Called when error occues. */
typedef void (*I3CS_ErrorCb)(void);

/**********************************************************************
 * Structures and unions
 **********************************************************************/
/** Structure containing pointers to functions defined by user that will be called when specific event occurs. */
typedef struct I3CS_Cus_Callbacks
{
    /** This function will be called when transfer is completed. */
    I3CS_TransferCompleteCb     i3cs_sdr_rx_cb;
    /** This function will be called when transfer is completed. */
    I3CS_TransferCompleteCb     i3cs_sdr_tx_cb;
    /** This function will be called when transfer is completed. */
    I3CS_TransferCompleteCb     i3cs_ddr_rx_cb;
    /** This function will be called when transfer is completed. */
    I3CS_TransferCompleteCb     i3cs_ddr_tx_cb;
    /** This function will be called when Hot Join event is completed or NACKed. */
    I3CS_HotJoinCb              i3cs_hot_join_cb;
    /** This function will be called when In-Band Interrupt event is completed or NACKed. */
    I3CS_InbandInterruptCb      i3cs_ibi_cb;
    /** Called when error occurs. */
    I3CS_ErrorCb                i3cs_err_cb;
}  I3CS_Cus_Callbacks;
/***********************************************
 * FUNCTION DECLARATION
 **********************************************/

/**
 * This function initializes an I3CS device with the given ID, base address, and callbacks.
 * 
 * @param i3cs_id An enumeration representing the I3CS interface ID (I3CS_ID_0 or I3CS_ID_1).
 * @param base_addr The base address of the I3C slave device's registers.
 * @param callbacks A pointer to a structure containing callback functions for various I3CS events,
 * such as SDR and DDR transfers, hot join completion, in-band interrupts, and errors.
 */
void hx_drv_i3cs_init(I3CS_ID_E i3cs_id, uint32_t base_addr, I3CS_Cus_Callbacks *callbacks);


/**
 * This function sets the static address of an I3CS device.
 * 
 * @param i3cs_id An enumeration representing the I3CS device ID.
 * @param static_addr The static address is a unique address assigned to an I3C slave device that
 * allows the master device to communicate with it directly without having to perform an address
 * discovery process. This parameter is the value of the static address that is being set for the
 * specified I3C slave device.
 */
void hx_drv_i3cs_set_static_addr(I3CS_ID_E i3c_slv_id, uint8_t static_addr);


/**
 * This function reads data from an I3C device in SDR mode and returns an error code if unsuccessful.
 * 
 * @param i3cs_id An enumeration representing the I3C slave device ID.
 * @param buf A pointer to a buffer where the received data will be stored.
 * @param num_bytes The number of bytes to be read from the I3C device.
 * 
 * @return a value of type I3CS_ERR_E, which is an enumerated type representing different error codes.
 */
I3CS_ERR_E hx_drv_i3cs_sdr_rx(I3CS_ID_E i3cs_id, uint8_t* buf, uint16_t num_bytes);


/**
 * The function hx_drv_i3cs_sdr_tx sends data over an I3C bus using SDR mode.
 * 
 * @param i3cs_id An enumeration representing the I3C slave device ID.
 * @param buf A pointer to the buffer containing the data to be transmitted over the I3CS bus.
 * @param num_bytes The number of bytes to be transmitted over the I3CS bus.
 * 
 * @return a value of type I3CS_ERR_E, which is an enumerated type representing different error codes.
 */
I3CS_ERR_E hx_drv_i3cs_sdr_tx(I3CS_ID_E i3cs_id, uint8_t* buf, uint16_t num_bytes);


/**
 * The function hx_drv_i3cs_ddr_rx reads data with HDR-DDR mode.
 * 
 * @param i3cs_id an enumeration representing the I3CS device ID
 * @param buf A pointer to a buffer where the received data will be stored.
 * @param num_bytes The number of bytes to be read with HDR-DDR mode.
 * 
 * @return a value of type I3CS_ERR_E, which is an enumerated type representing different error codes.
 */
I3CS_ERR_E hx_drv_i3cs_ddr_rx(I3CS_ID_E i3cs_id, uint16_t *buf, uint16_t num_bytes);


/**
 * This function writes data with HDR-DDR mode.
 * 
 * @param i3cs_id An enumeration representing the I3C slave device ID.
 * @param buf A pointer to the buffer containing the data to be transmitted over I3CS DDR (Double Data
 * Rate).
 * @param num_bytes The number of bytes to be transmitted with the I3CS HDR-DDR mode.
 * 
 * @return a variable of type I3CS_ERR_E, which is an enumerated type representing the error codes that
 * can be returned by this function.
 */
I3CS_ERR_E hx_drv_i3cs_ddr_tx(I3CS_ID_E i3cs_id, uint16_t *buf, uint16_t num_bytes);


/**
 * This function sends an IBI message with a payload to a specified I3CS slave device.
 * 
 * @param i3cs_id An identifier for the I3C slave device.
 * @param buf A pointer to the buffer containing the payload to be transmitted via IBI (In-Band
 * Interrupt).
 * @param num_bytes The number of bytes to be transmitted in the IBI (In-Band Interrupt) message.
 * 
 * @return a value of type I3CS_ERR_E, which is an enumerated type representing different error codes.
 * The specific error code being returned depends on the outcome of the function's execution.
 */
I3CS_ERR_E hx_drv_i3cs_ibi_tx(I3CS_ID_E i3cs_id, uint8_t* buf, uint8_t num_bytes);


/**
 * This function requests a hot join for a specific I3CS device and returns an error code if
 * unsuccessful.
 * 
 * @param i3cs_id an enumeration representing the I3C device ID
 * 
 * @return a value of type I3CS_ERR_E, which is an enumerated type representing the error codes that
 * can be returned by the function.
 */
I3CS_ERR_E hx_drv_i3cs_hot_join(I3CS_ID_E i3cs_id);

/** @} */ // end of I3CS group
#ifdef __cplusplus
}
#endif
#endif /* _HX_DRV_I3C_SLV_H_ */