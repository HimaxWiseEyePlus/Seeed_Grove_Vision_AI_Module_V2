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
*       hx_drv_scu_set_PA2_pinmux(SCU_PA2_PINMUX_SB_I3C_S_SCL, 1);
*       hx_drv_scu_set_PA3_pinmux(SCU_PA3_PINMUX_SB_I3C_S_SDA, 1);
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
*
*           for (i = 0; i < number_of_samples; i++) {
*               data[i] = (((i * 2) & 0xFF) | ((i * 2 + 1) << 8));
*           }
*       }
*
*       #define MAX_I3CS_EVET_STRING         100
*       static char i3cs_event[][MAX_I3CS_EVET_STRING]={
*           {"[0]:  SDR Read Complete"},
*           {"[1]:  SDR Write Complete"},
*           {"[2]:  DDR Read Complete"},
*           {"[3]:  DDR Write Complete"},
*           {"[4]:  SDR TX FIFO Overflow"},
*           {"[5]:  SDR RX FIFO Underflow"},
*           {"[6]:  DDR TX FIFO Overflow"},
*           {"[7]:  DDR RX FIFO Underflow"},
*           {"[8]:  SDR TX Threshold"},
*           {"[9]:  SDR RX Threshold"},
*           {"[10]: DDR TX Threshold"},
*           {"[11]: DDR RX Threshold"},
*           {"[12]: Master Read Abort"},
*           {"[13]: DDR Fail"},
*           {"[14]: SDR Fail"},
*           {"[15]: Dynamic Address Update"},
*           {"[16]: In-Band Interrupt Done"},
*           {"[17]: In-Band Interrupt NACK"},
*           {"[18]: Hot-Join Done"},
*           {"[19]: Hot-Join NACK"},
*           {"[20]: EVENT UP (DISEC_CCC or ENEC_CCC is received)"},
*           {"[21]: Error (SDR Error is detected? applicable for S0, S1, S2, S4 and S5 Errors)"},
*           {"[22]: Test Mode indicator"},
*           {"[23]: Reserved"},
*           {"[24]: Reserved"},
*           {"[25]: Reserved"},
*           {"[26]: Reserved"},
*           {"[27]: Reserved"},
*           {"[28]: Reserved"},
*           {"[29]: Reserved"},
*           {"[30]: Reserved"},
*           {"[31]: Reserved"},
*       };
*
*       /// I3CS callback function 
*       void I3CS_0_SDR_Rx_Handler(uint32_t size)
*       {   xprintf("\n\n");
*           xprintf("[%s] size:%d bytes\n", __FUNCTION__, size);
*
*           for(uint32_t i = 0; i < size; i++){
*               xprintf("rx_data[%d] = 0x%02x \n", i, i3c_slv_sdr_rx_buf[i]);
*           }
*       }
*
*       void I3CS_0_SDR_Tx_Handler(uint32_t size)
*       {
*           xprintf("\n\n");
*           xprintf("[%s] size:%d bytes\n", __FUNCTION__, size);
*       }
*
*       void I3CS_0_DDR_Rx_Handler(uint32_t size)
*       {
*           xprintf("\n\n");
*           xprintf("[%s] size:%d bytes\n", __FUNCTION__, size);
*           I3CS_ERR_E status;
*           uint32_t num_words = size/2;
*
*           for(uint32_t i = 0; i < num_words; i++){
*               xprintf("rx_data[%d] = 0x%02x \n", i, i3c_slv_ddr_rx_buf[i]);
*           }
*       }
*
*       void I3CS_0_DDR_Tx_Handler(uint32_t size)
*       {
*           xprintf("\n\n");
*           xprintf("[%s] size:%d bytes\n", __FUNCTION__, size);
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
*       void I3CS_0_Evt_Handler(uint32_t evt)
*       {
*           if((evt & (0x0F00)) == 0){
*               /// masked fifo threshold interrupt: bit11/bit10/bit9/bit8
*               xprintf("i3cs_evt:0x%04x \n", evt);
*           }
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
*       i3cs_0_callbacks.i3cs_evt_cb = I3CS_0_EVT_Handler;
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
*       uint8_t rx_data_size = 8;
*       hx_drv_i3cs_sdr_rx(I3CS_ID_0, i3c_slv_sdr_rx_buf, rx_data_size);
*
*   Case4: SDR-TX 8 bytes
*       uint8_t tx_data_size = 8;
*       generate_uint8_test_data(i3c_slv_sdr_tx_buf, tx_data_size);
*       hx_drv_i3cs_sdr_tx(I3CS_ID_0, i3c_slv_sdr_tx_buf, tx_data_size);
*
*   Case5: DDR-RX 8 bytes
*       uint8_t rx_data_size = 8;
*       hx_drv_i3cs_ddr_rx(I3CS_ID_0, i3c_slv_ddr_rx_buf, rx_data_size);
*
*   Case6: DDR-TX 8 bytes
*       uint8_t tx_data_size = 8;
*       generate_uint16_test_data(i3c_slv_ddr_tx_buf, (tx_data_size/2));
*       hx_drv_i3cs_ddr_tx(I3CS_ID_0, i3c_slv_ddr_tx_buf, tx_data_size);
*
*   The controller can participate in I2C transfers (operates as an I2C slave) up until it is
*   assigned a Dynamic Address using either SETDASA CCC or DAA procedure. 
* 
*   When calling hx_drv_i3cs_init(), the initial static address is I3C_SLV_X_DEF_STATIC_ADDR, 
*   which can be further modified by hx_drv_i3cs_set_static_addr() to the user-specified static address 
*   To refer the basic configuration for I3CS initiation mentioned above.
*   
*   Case1: use WE2 I3C slave 0 to read 8 bytes via i2c legacy protocol
*       uint8_t rx_data_size = 8;
*       hx_drv_i3cs_sdr_rx(I3CS_ID_0, i3c_slv_sdr_rx_buf, rx_data_size);
*
*       /// I2C master sends out data following this protocol, where REG_ADDR (0x01) is specific to WE2 RX operation
*       START -- SLV_ADDR + W_BIT -- REG_ADDR (0x01) -- RESTART -- SLV_ADDR + W_BIT -- DATA(N) -- STOP
*
*
*    Case2: use WE2 I3C slave 0 to write 8 bytes via i2c legacy protocol
*       uint8_t tx_data_size = 8;
*       generate_uint8_test_data(i3c_slv_sdr_tx_buf, tx_data_size);
*       hx_drv_i3cs_sdr_tx(I3CS_ID_0, i3c_slv_sdr_tx_buf, tx_data_size);
*
*       /// I2C master receive data following this protocol, where REG_ADDR (0x00) is specific to WE2 TX operation
*       START -- SLV_ADDR + W_BIT -- REG_ADDR (0x00) -- RESTART -- SLV_ADDR + R_BIT -- DATA(N) -- STOP
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
#define I3C_SLV_0_DEF_STATIC_ADDR     0x20
#define I3C_SLV_1_DEF_STATIC_ADDR     0x22
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

#define I3CS_IRQ_SDRWR_COMP ((uint32_t) 1U << 0)  /* Master terminates the SDR Private Write transfer */
#define I3CS_IRQ_SDRRD_COMP ((uint32_t) 1U << 1)  /*  Slave terminates the SDR Private Read transfer */
#define I3CS_IRQ_DDRWR_COMP ((uint32_t) 1U << 2)  /* Master terminates the DDR Write transfer */
#define I3CS_IRQ_DDRRD_COMP ((uint32_t) 1U << 3)  /*  Slave terminates the DDR Read transfer */
#define I3CS_IRQ_SDRTX_OVF  ((uint32_t) 1U << 4)
#define I3CS_IRQ_SDRRX_UNF  ((uint32_t) 1U << 5)
#define I3CS_IRQ_DDRTX_OVF  ((uint32_t) 1U << 6)
#define I3CS_IRQ_DDRRX_UNF  ((uint32_t) 1U << 7)
#define I3CS_IRQ_SDRTX_THR  ((uint32_t) 1U << 8)
#define I3CS_IRQ_SDRRX_THR  ((uint32_t) 1U << 9)
#define I3CS_IRQ_DDRTX_THR  ((uint32_t) 1U << 10)
#define I3CS_IRQ_DDRRX_THR  ((uint32_t) 1U << 11)
#define I3CS_IRQ_M_RD_ABORT ((uint32_t) 1U << 12)
#define I3CS_IRQ_DDR_FAIL   ((uint32_t) 1U << 13)
#define I3CS_IRQ_SDR_FAIL   ((uint32_t) 1U << 14)
#define I3CS_IRQ_DA_UPDATE  ((uint32_t) 1U << 15)
#define I3CS_IRQ_IBI_DONE   ((uint32_t) 1U << 16)
#define I3CS_IRQ_IBI_NACK   ((uint32_t) 1U << 17)
#define I3CS_IRQ_HJ_DONE    ((uint32_t) 1U << 18)
#define I3CS_IRQ_HJ_NACK    ((uint32_t) 1U << 19)
#define I3CS_IRQ_EVENT_UP   ((uint32_t) 1U << 20)
#define I3CS_IRQ_ERROR      ((uint32_t) 1U << 21)
#define I3CS_IRQ_TM         ((uint32_t) 1U << 22)

typedef struct {
    bool sdr_tx_empty;
    bool sdr_rx_empty;
    bool sdr_tx_full;
    bool sdr_rx_full;
    bool ddr_tx_empty;
    bool ddr_rx_empty;
    bool ddr_tx_full;
    bool ddr_rx_full;
    bool has_da;        // has dynamic address
    bool ibi_en;        // in-band interrupt enable
    bool hj_en;         // hot join interrupt enable
    uint8_t da;         // dyanmic address
} hx_drv_i3c_sts1_info_t;
/**********************************************************************
 * Callbacks
 **********************************************************************/
/** Transfer Complete event. */
typedef void (*I3CS_SDR_TransferCompleteCb)(uint32_t size);
typedef void (*I3CS_DDR_TransferCompleteCb)(uint32_t size);

/** Hot Join interrupt occurred. */
typedef void (*I3CS_HotJoinCb)(bool nacked);

/** In-band interrupt occurred. */
typedef void (*I3CS_InbandInterruptCb)(bool nacked);

/** Called when error occues. */
typedef void (*I3CS_ErrorCb)(void);

/** Interrupt Event  */
typedef void (*I3CS_InterruptEventCb)(uint32_t event);

/**********************************************************************
 * Structures and unions
 **********************************************************************/
/** Structure containing pointers to functions defined by user that will be called when specific event occurs. */
typedef struct I3CS_Cus_Callbacks
{
    /** This function will be called when transfer is completed. */
    I3CS_SDR_TransferCompleteCb     i3cs_sdr_rx_cb;
    /** This function will be called when transfer is completed. */
    I3CS_SDR_TransferCompleteCb     i3cs_sdr_tx_cb;
    /** This function will be called when transfer is completed. */
    I3CS_DDR_TransferCompleteCb     i3cs_ddr_rx_cb;
    /** This function will be called when transfer is completed. */
    I3CS_DDR_TransferCompleteCb     i3cs_ddr_tx_cb;
    /** This function will be called when Hot Join event is completed or NACKed. */
    I3CS_HotJoinCb                  i3cs_hot_join_cb;
    /** This function will be called when In-Band Interrupt event is completed or NACKed. */
    I3CS_InbandInterruptCb          i3cs_ibi_cb;
    /** Called when error occurs. */
    I3CS_ErrorCb                    i3cs_err_cb;
    /** Interrupt Event  */
    I3CS_InterruptEventCb           i3cs_evt_cb;
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
 *            Due to the slow speed of the APB, reading and writing to registers inside the ISR (Interrupt Service Routine) takes too much time.
 *            SDR at a speed of 12.5MHz, receiving 640 bytes is no problem. However, data loss occurs when operating at speeds exceeding 12.5MHz
 * @return a value of type I3CS_ERR_E, which is an enumerated type representing different error codes.
 */
I3CS_ERR_E hx_drv_i3cs_sdr_rx(I3CS_ID_E i3cs_id, uint8_t* buf, uint32_t num_bytes);


/**
 * The function hx_drv_i3cs_sdr_tx sends data over an I3C bus using SDR mode.
 * 
 * @param i3cs_id An enumeration representing the I3C slave device ID.
 * @param buf A pointer to the buffer containing the data to be transmitted over the I3CS bus.
 * @param num_bytes The number of bytes to be transmitted over the I3CS bus.
 *            Due to the slow speed of the APB, reading and writing to registers inside the ISR (Interrupt Service Routine) takes too much time.
 *            SDR at a speed of 12.5MHz, transmitting 2048 bytes is no problem. However, data loss occurs when operating at speeds exceeding 12.5MHz
 * @return a value of type I3CS_ERR_E, which is an enumerated type representing different error codes.
 */
I3CS_ERR_E hx_drv_i3cs_sdr_tx(I3CS_ID_E i3cs_id, uint8_t* buf, uint32_t num_bytes);


/**
 * The function hx_drv_i3cs_ddr_rx reads data with HDR-DDR mode.
 * 
 * @param i3cs_id an enumeration representing the I3CS device ID
 * @param buf A pointer to a buffer where the received data will be stored.
 * @param num_bytes The number of bytes to be read with HDR-DDR mode. (max: 508 bytes for data payload)
 *                  cmd(2 bytes) + data(508 bytes) + crc(2 bytes) = 512 bytes (max DDR-RX-FIFO)
 * @return a value of type I3CS_ERR_E, which is an enumerated type representing different error codes.
 */
I3CS_ERR_E hx_drv_i3cs_ddr_rx(I3CS_ID_E i3cs_id, uint16_t *buf, uint32_t num_bytes);


/**
 * This function writes data with HDR-DDR mode.
 * 
 * @param i3cs_id An enumeration representing the I3C slave device ID.
 * @param buf A pointer to the buffer containing the data to be transmitted over I3CS DDR (Double Data
 * Rate).
 * @param num_bytes The number of bytes to be transmitted with the I3CS HDR-DDR mode. (max: 510 bytes for data payload)
 *                  data(510 bytes) + crc(2 bytes) = 512 bytes (max DDR-TX-FIFO)
 * @return a variable of type I3CS_ERR_E, which is an enumerated type representing the error codes that
 * can be returned by this function.
 */
I3CS_ERR_E hx_drv_i3cs_ddr_tx(I3CS_ID_E i3cs_id, uint16_t *buf, uint32_t num_bytes);


/**
 * This function sends an IBI message with a payload to a specified I3CS slave device.
 * 
 * @param i3cs_id An identifier for the I3C slave device.
 * @param buf A pointer to the buffer containing the payload to be transmitted via IBI (In-Band
 * Interrupt).
 * @param num_bytes he number of bytes (max 31 bytes) to be transmitted in the IBI (In-Band Interrupt) message.
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


/**
 * The function hx_drv_i3cs_get_status retrieves the status of an I3CS device.
 * 
 * @param i3cs_id The i3cs_id parameter is an enumeration representing the ID of the I3CS device.
 * @param status A pointer to a hx_drv_i3c_sts1_info_t variable where the status value will be stored.
 * 
 * @return the value of the variable "err", which is of type "I3CS_ERR_E".
 */
I3CS_ERR_E hx_drv_i3cs_get_status(I3CS_ID_E i3cs_id, hx_drv_i3c_sts1_info_t* status);
/** @} */ // end of I3CS group
#ifdef __cplusplus
}
#endif
#endif /* _HX_DRV_I3C_SLV_H_ */