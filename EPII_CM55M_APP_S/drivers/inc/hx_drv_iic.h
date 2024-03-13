#ifndef INC_HX_DRV_IIC_M_H_
#define INC_HX_DRV_IIC_M_H_
#include "dev_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \defgroup   I2C
* \brief      Inter-Integrated Circuit
* \details    WE2 has two sets of I2C masters and two sets of I2C slaves
* <pre>
*    Pin mux configuration
*      I2C Master 0 (for general use): PA2/PA3/PB0/PB1/PB2/PB3/PB4/PB5/PB6/PB7/PB8/PB9/PB10/PB11
*      I2C Master 1 (for image sensor use): SEN_I2CM_SDA(PAD_SEN_I2C_MST_SDA)/SEN_I2CM_SCL(PAD_SEN_I2C_MST_SCL)
*      I2C Slave 0: PA2/PA3
*      I2C Slave 1: PA2/PA3/PB0/PB1/PB2/PB3/PB4/PB5/PB6/PB7/PB8/PB9/PB10/PB11
*
*    Sample code: I2C master 0 pin mux configuration and initialization
*      /// The output pin of I2C Master 0 is defined by the user application.
*      hx_drv_scu_set_PB7_pinmux(SCU_PB7_PINMUX_I2C_M_SCL, 1);
*      hx_drv_scu_set_PB8_pinmux(SCU_PB8_PINMUX_I2C_M_SDA, 1);
*   
*      /// initializes the I2C Master 0 with SCL speed of 400 KHz
*      hx_drv_i2cm_init(USE_DW_IIC_0, HX_I2C_HOST_MST_0_BASE, DW_IIC_SPEED_FAST);
*
*    Usage-1: Transmit data using interrupt mode with I2C master 0
*      void i2cm_0_tx_cb(void *param)
*      {
*          xprintf("[%s] \n", __FUNCTION__);
*      }
*
*      void i2cm_0_err_cb(void *param)
*      {
*          HX_DRV_DEV_IIC *iic_obj = param;
*          HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
*          HX_DRV_DEV_IIC_ERROR_STATE err_state = iic_info_ptr->err_state;
*          xprintf("[%s] err:%d \n", __FUNCTION__, err_state);
*      }
*
*      uint8_t wbuffer[32] = {0};
*      uint32_t i, data_size = 32;
*      for(i = 0; i < data_size; i++)
*      {
*          wbuffer[i] = i;
*          xprintf("wbuffer[%d]:0x%02x \n", i, wbuffer[i]);
*      }

*      hx_drv_i2cm_set_err_cb(USE_DW_IIC_0, i2cm_0_err_cb);
*      hx_drv_i2cm_interrupt_write(USE_DW_IIC_0, 0x24, wbuffer, data_size, i2cm_0_tx_cb);
*
*
*    Usage-2: Receive data using interrupt mode with I2C master 0
*      uint8_t rbuffer[32] = {0};
*      uint32_t i, data_size = 32;
*
*      void i2cm_0_rx_cb(void *param)
*      {
*          xprintf("[%s] \n", __FUNCTION__);
*          HX_DRV_DEV_IIC *iic_obj = param;
*          HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
*          uint32_t len, ofs;
*          uint8_t *rbuffer;
*
*          len = iic_info_ptr->rx_buf.len;
*          ofs = iic_info_ptr->rx_buf.ofs;
*          rbuffer = iic_info_ptr->rx_buf.buf;
*
*          xprintf("[%s] ofs:%d \n", __FUNCTION__, ofs);
*          for ( int i = 0; i < ofs; i++ )
*          {
*              xprintf("0x%x ", *(rbuffer+i));
*          }
*      }
*
*      void i2cm_0_err_cb(void *param)
*      {
*          HX_DRV_DEV_IIC *iic_obj = param;
*          HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
*          HX_DRV_DEV_IIC_ERROR_STATE err_state = iic_info_ptr->err_state;
*          xprintf("[%s] err:%d \n", __FUNCTION__, err_state);
*      }
*
*      hx_drv_i2cm_set_err_cb(USE_DW_IIC_0, i2cm_0_err_cb);
*      hx_drv_i2cm_interrupt_read(USE_DW_IIC_0, 0x24, rbuffer, data_size, i2cm_0_rx_cb);
*
*
*    Usage-3: Write data to register address using I2C master 0
*      uint8_t regAddr[2] = {0x12, 0x34};
*      uint8_t regLen = 2;
*      uint8_t wBuffer[2] = {0x56, 0x78};
*      uint8_t dataLen = 2;
*
*      hx_drv_i2cm_write_data(USE_DW_IIC_0, 0x24, regAddr, regLen, wBuffer, dataLen);
*
*
*    Usage-4: reads data from a specified I2C slave device using the I2C Master 0
*      uint8_t rBuffer[2] = {0};
*      uint8_t dataLen = 2;
*       
*      hx_drv_i2cm_read_data(USE_DW_IIC_0, 0x24, rBuffer, dataLen);
*
*
*    Usage-5: writes data to an I2C device and then reads data from the device using a restart condition.
*      uint8_t regAddr[2] = {0x12, 0x34};
*      uint8_t regLen = 2;
*      uint8_t ruffer[2] = {0};
*      uint8_t dataLen = 2;
*
*      hx_drv_i2cm_write_restart_read(USE_DW_IIC_0, 0x24, regAddr, regLen, ruffer, dataLen);
*
*
*    Usage-6: writes data to an I2C device and then reads data from the device using a stop condition.
*      uint8_t regAddr[2] = {0x12, 0x34};
*      uint8_t regLen = 2;
*      uint8_t ruffer[2] = {0};
*      uint8_t dataLen = 2;
*
*      hx_drv_i2cm_write_stop_read(USE_DW_IIC_0, 0x24, regAddr, regLen, ruffer, dataLen);
*
*
*
*
*    Sample code: I2C slave 0 pin mux configuration and initialization
*      /// The output pin of I2C slave 0 is defined by the user application.
*      hx_drv_scu_set_PA2_pinmux(SCU_PA2_PINMUX_SB_I2C_S_SCL_0, 1);
*      hx_drv_scu_set_PA3_pinmux(SCU_PA3_PINMUX_SB_I2C_S_SDA_0, 1);
*   
*      /// initializes the I2C slave 0 
*       hx_drv_i2cs_init(USE_DW_IIC_SLV_0, HX_I2C_HOST_SLV_0_BASE);
*
*    Usage-1: Transmit data using interrupt mode with I2C slave 0
*      void i2cs_0_tx_cb(void *param)
*      {
*          xprintf("[%s] \n", __FUNCTION__);
*      }
*
*      void i2cs_0_err_cb(void *param)
*      {
*          HX_DRV_DEV_IIC *iic_obj = param;
*          HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
*          HX_DRV_DEV_IIC_ERROR_STATE err_state = iic_info_ptr->err_state;
*          xprintf("[%s] err:%d \n", __FUNCTION__, err_state);
*
*          if(err_state == DEV_IIC_ERR_TX_DATA_UNREADY){
*              // I2C slave transfer old data or prepare new data 
*          }
*      }
*
*      uint8_t wbuffer[32] = {0};
*      uint32_t i, data_size = 32;
*      for(i = 0; i < data_size; i++)
*      {
*          wbuffer[i] = i;
*          xprintf("wbuffer[%d]:0x%02x \n", i, wbuffer[i]);
*      }
*
*      hx_drv_i2cs_set_err_cb(USE_DW_IIC_0, i2cs_0_err_cb);
*      hx_drv_i2cs_interrupt_write(USE_DW_IIC_SLV_0, 0x62, wbuffer, data_size, i2cs_0_tx_cb);
*
*    Usage-2: Receive data using interrupt mode with I2C slave 0
*      uint8_t rbuffer[32] = {0};
*      uint32_t i, data_size = 32;
*
*      void i2cs_0_rx_cb(void *param)
*      {
*          xprintf("[%s] \n", __FUNCTION__);
*          HX_DRV_DEV_IIC *iic_obj = param;
*          HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
*          uint32_t len, ofs;
*          uint8_t *rbuffer;
*
*          len = iic_info_ptr->rx_buf.len;
*          ofs = iic_info_ptr->rx_buf.ofs;
*          rbuffer = iic_info_ptr->rx_buf.buf;
*
*          xprintf("[%s] ofs:%d \n", __FUNCTION__, ofs);
*          for ( int i = 0; i < ofs; i++ )
*          {
*              xprintf("0x%x ", *(rbuffer+i));
*          }
*      }
*
*      void i2cs_0_err_cb(void *param)
*      {
*          HX_DRV_DEV_IIC *iic_obj = param;
*          HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
*          HX_DRV_DEV_IIC_ERROR_STATE err_state = iic_info_ptr->err_state;
*          xprintf("[%s] err:%d \n", __FUNCTION__, err_state);
*
*          if(err_state == DEV_IIC_ERR_TX_DATA_UNREADY){
*              // I2C slave transfer old data or prepare new data
*          }
*      }
*
*      hx_drv_i2cs_set_err_cb(USE_DW_IIC_0, i2cs_0_err_cb);
*      hx_drv_i2cs_interrupt_read(USE_DW_IIC_SLV_0, 0x62, rbuffer, data_size, i2cs_0_rx_cb);
*
*
*    Usage-3: Receive data using DMA mode with I2C slave 0
*      #define I2C_SLV_RX_DMA_NUM          1024
*      uint8_t i2c_slv_rx_buf[I2C_SLV_RX_DMA_NUM];
*      uint8_t g_i2cs_dma_ch = 0;  // ch: 0 ~ 7
*      
*      void i2c_slv_rx_dma_cb()
*      {
*          
*      }
*      
*      void i2cs_0_err_cb(void *param)
*      {
*      
*          HX_DRV_DEV_IIC *iic_obj = param;
*          HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
*          HX_DRV_DEV_IIC_ERROR_STATE err_state = iic_info_ptr->err_state;
*          
*          if(err_state == DEV_IIC_ERR_TX_DATA_UNREADY){
*              // I2C slave transfer old data or prepare new data 
*          }if(err_state == DEV_IIC_ERR_RX_DMA_UNREADY){
*              // I2C Master send data but I2C slave dma mode is unready
*              hx_drv_i2cs_dma_read(USE_DW_IIC_SLV_0, 0x62, g_i2cs_dma_ch, i2c_slv_rx_buf, I2C_SLV_RX_DMA_NUM, i2c_slv_rx_dma_cb);
*          }
*      }
*      
*      hx_drv_i2cs_set_err_cb(USE_DW_IIC_0, i2cs_0_err_cb);
*      hx_drv_i2cs_dma_read(USE_DW_IIC_SLV_0, 0x62, g_i2cs_dma_ch, i2c_slv_rx_buf, I2C_SLV_RX_DMA_NUM, i2c_slv_rx_dma_cb);
*
*
*    Usage-4: Transmit data using DMA mode with I2C slave 0
*      #define I2C_SLV_TX_DMA_NUM          1024
*      uint16_t i2c_slv_tx_buf[I2C_SLV_TX_DMA_NUM];
*      uint8_t g_i2cs_dma_ch = 0;  // ch: 0 ~ 7
*
*      void i2c_slv_tx_dma_cb()
*      {
*          
*      }
*      
*      void i2cs_0_err_cb(void *param)
*      {
*          HX_DRV_DEV_IIC *iic_obj = param;
*          HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
*          HX_DRV_DEV_IIC_ERROR_STATE err_state = iic_info_ptr->err_state;
*          
*          if(err_state == DEV_IIC_ERR_TX_DATA_UNREADY){
*              // I2C slave transfer old data or prepare new data 
*              hx_drv_i2cs_dma_write(USE_DW_IIC_SLV_0, 0x62, g_i2cs_dma_ch, i2c_slv_tx_buf, I2C_SLV_TX_DMA_MAX_NUM, i2c_slv_tx_dma_cb);
*          }if(err_state == DEV_IIC_ERR_RX_DMA_UNREADY){
*              // I2C Master send data but I2C slave dma mode is unready
*          }
*      }
*
*      // an array of 16-bit values that the 8-bit Most Significant Bit (MSB) must be zero
*      // as it is reserved for filling the control bits in the data FIFO buffer.
*      for(uint32_t i=0; i<I2C_SLV_TX_DMA_MAX_NUM; i++)
*      {
*          i2c_slv_tx_buf[i] = i & 0x00FF;
*      }
*
*      hx_drv_i2cs_set_err_cb(USE_DW_IIC_0, i2cs_0_err_cb);
*      hx_drv_i2cs_dma_write(USE_DW_IIC_SLV_0, 0x62, g_i2cs_dma_ch, i2c_slv_tx_buf, I2C_SLV_TX_DMA_MAX_NUM, i2c_slv_tx_dma_cb);
*
* </pre>
* @{
*/


#define DW_IIC_M_NUM                            (2)    /*!< The number of WE2 I2C master */
#define DW_IIC_S_NUM                            (2)    /*!< The number of WE2 I2C slave */


typedef enum USE_DW_IIC_S
{
    USE_DW_IIC_0 = 0,                   /*!< I2C master 0 */
    USE_DW_IIC_1 = 1,                   /*!< I2C master 1 */
    USE_DW_IIC_ALL = DW_IIC_M_NUM,
} USE_DW_IIC_E;

#define DW_IIC_0_SLV_ID         0
#define DW_IIC_1_SLV_ID         1

typedef enum USE_DW_IIC_SLV_S
{
    USE_DW_IIC_SLV_0 = DW_IIC_0_SLV_ID, /*!< I2C slave 0 */
    USE_DW_IIC_SLV_1 = DW_IIC_1_SLV_ID, /*!< I2C slave 1 */
    USE_DW_IIC_SLV_ALL = DW_IIC_S_NUM,
} USE_DW_IIC_SLV_E;


/** IIC Bus possible speed modes */
typedef enum DW_IIC_SPEED_MODE_S 
{
    DW_IIC_SPEED_STANDARD = 0, /*!< Bidirectional, Standard-mode (Sm), with a bit rate up to 100 kbit/s */
    DW_IIC_SPEED_FAST     = 1, /*!< Bidirectional, Fast-mode (Fm), with a bit rate up to 400 kbit/s */
    DW_IIC_SPEED_FASTPLUS = 2  /*!< Bidirectional, Fast-mode Plus (Fm+), with a bit rate up to 1 Mbit/s */
} DW_IIC_SPEED_MODE_E;


/**
 * mapping from dw_iic_hal.h
 * To implement user functions for the customer, please refer to cli_i2c_mst.c and cli_i2c_slv.c
 */
/** Define IIC control commands for common usage */
#define DW_SET_IIC_SYSCMD(cmd)     DEV_SET_SYSCMD((cmd))

/** Define IIC control commands for master usage */
#define DW_SET_IIC_MST_SYSCMD(cmd) DEV_SET_SYSCMD(0x00004000|(cmd))

/** Define IIC control commands for slave usage */
#define DW_SET_IIC_SLV_SYSCMD(cmd) DEV_SET_SYSCMD(0x00008000|(cmd))

#define DW_IIC_CMD_SET_BUS_CLEAR_EN                       DEV_SET_USRCMD(10)
#define DW_IIC_CMD_SET_SCL_STUCK_AT_LOW_TIMEOUT           DEV_SET_USRCMD(11)

#define DW_IIC_CMD_SET_TXCB                               DW_SET_IIC_SYSCMD(2)
#define DW_IIC_CMD_SET_RXCB                               DW_SET_IIC_SYSCMD(3)
#define DW_IIC_CMD_SET_ERRCB                              DW_SET_IIC_SYSCMD(4)
#define DW_IIC_CMD_SET_TXINT_BUF                          DW_SET_IIC_SYSCMD(5)
#define DW_IIC_CMD_SET_RXINT_BUF                          DW_SET_IIC_SYSCMD(6)
#define DW_IIC_CMD_SET_TXINT                              DW_SET_IIC_SYSCMD(7)
#define DW_IIC_CMD_SET_RXINT                              DW_SET_IIC_SYSCMD(8)
#define DW_IIC_CMD_FLUSH_TX                               DW_SET_IIC_SYSCMD(12)
#define DW_IIC_CMD_FLUSH_RX                               DW_SET_IIC_SYSCMD(13)
#define DW_IIC_CMD_SET_STACB                              DW_SET_IIC_SYSCMD(18)
#define DW_IIC_CMD_SET_DMA_ERRCB                          DW_SET_IIC_SYSCMD(19)
     
#define DW_IIC_CMD_MST_SET_SPEED_MODE                     DW_SET_IIC_MST_SYSCMD(0)
#define DW_IIC_CMD_MST_SET_NEXT_COND                      DW_SET_IIC_MST_SYSCMD(1)
#define DW_IIC_CMD_MST_SET_TAR_ADDR                       DW_SET_IIC_MST_SYSCMD(2)
										               
#define DW_IIC_CMD_SLV_SET_SLV_ADDR                       DW_SET_IIC_SLV_SYSCMD(0)
#define DW_IIC_CMD_SLV_GET_SLV_STATE                      DW_SET_IIC_SLV_SYSCMD(1)
#define DW_IIC_CMD_SLV_SET_RX_DMA_DONE                    DW_SET_IIC_SLV_SYSCMD(5)
#define DW_IIC_CMD_SLV_SET_TX_DMA_DONE                    DW_SET_IIC_SLV_SYSCMD(6)
/* mapping from dw_iic_hal.h */


/**
 * \name	I2C Error Code Definitions
 * @{
 */
/* mapping from dev_common.h*/
typedef enum IIC_ERR_CODE_S
{
    IIC_ERR_OK                   = 0,         /*!< ok */
	IIC_ERR_SYS                  = -5,        /*!< system error */
	IIC_ERR_OPNED                = -6,        /*!< device is opened */
	IIC_ERR_CLSED                = -7,        /*!< device is closed */
	IIC_ERR_NOSPT                = -9,        /*!< unsupported features */
	IIC_ERR_RSFN                 = -10,       /*!< reserved function code */
	IIC_ERR_RSATR                = -11,       /*!< reserved attribute */
	IIC_ERR_PAR                  = -17,       /*!< parameter error */
	IIC_ERR_ID                   = -18,       /*!< invalid ID number */
	IIC_ERR_CTX                  = -25,       /*!< context error */
	IIC_ERR_MACV                 = -26,       /*!< memory access violation */
	IIC_ERR_OACV                 = -27,       /*!< object access violation */
	IIC_ERR_ILUSE                = -28,       /*!< illegal service call use */
	IIC_ERR_NOMEM                = -33,       /*!< insufficient memory */
	IIC_ERR_NOID                 = -34,       /*!< no ID number available */
	IIC_ERR_NORES                = -35,       /*!< no resource available */
	IIC_ERR_OBJ                  = -41,       /*!< object state error */
	IIC_ERR_NOEXS                = -42,       /*!< non-existent object */
	IIC_ERR_QOVR                 = -43,       /*!< queue overflow */
	IIC_ERR_RLWAI                = -49,       /*!< forced release from waiting */
	IIC_ERR_TMOUT                = -50,       /*!< polling failure or timeout */
	IIC_ERR_DLT                  = -51,       /*!< waiting object deleted */
	IIC_ERR_CLS                  = -52,       /*!< waiting object state changed */
	IIC_ERR_WBLK                 = -57,       /*!< non-blocking accepted */
	IIC_ERR_BOVR                 = -58,       /*!< buffer overflow */
	IIC_ERR_DEV_I2C_ADDR_NO_ACK  = -60,       /*!< Slave address is sent but not addressed by any slave devices */
	IIC_ERR_DEV_I2C_DATA_NO_ACK  = -61,       /*!< Data in transfer is not acked when it should be acked */
	IIC_ERR_DEV_I2C_TIMEOUT      = -62,       /*!< Timeout, no more data is received or sent */
	IIC_ERR_DEV_I2C_MSTSTOP      = -63,       /*!< Slave received a STOP condition from master device */
	IIC_ERR_DEV_I2C_LOST_BUS     = -64        /*!< Master or Slave-Transmitter lost arbitration */
}IIC_ERR_CODE_E;
/* mapping from dev_common.h*/

/**
 * \enum I2C_MST_PIN_MUX_E
 * \brief I2C Master PIN Mux Type
 */
typedef enum {
    // IIC_HOST
    IIC_HOST_PIN_MUX_I2CM_SCLSDA_F1_SCLSDA = 0,
    IIC_HOST_PIN_MUX_PA2_F6_SCL,
    IIC_HOST_PIN_MUX_PA3_F6_SDA,
    IIC_HOST_PIN_MUX_PB0_F6_SDA,
    IIC_HOST_PIN_MUX_PB1_F6_SCL,
    IIC_HOST_PIN_MUX_PB2_F6_SDA,
    IIC_HOST_PIN_MUX_PB3_F6_SCL,
    IIC_HOST_PIN_MUX_PB4_F6_SDA,
    IIC_HOST_PIN_MUX_PB5_F6_SCL,
    IIC_HOST_PIN_MUX_PB6_F6_SDA,
    IIC_HOST_PIN_MUX_PB7_F6_SCL,
    IIC_HOST_PIN_MUX_PB8_F6_SDA,
    IIC_HOST_PIN_MUX_PB9_F6_SCL,
    IIC_HOST_PIN_MUX_PB10_F6_SDA,
    IIC_HOST_PIN_MUX_PB11_F6_SDA,

    // IIC_HOST_SENSOR
    IIC_HOST_SENSOR_PIN_MUX_SEN_I2CM_F0_SCLSDA,
    IIC_HOST_SENSOR_PIN_MUX_SEN_I2CM_F1_SCLSDA
} I2C_MST_PIN_MUX_E;

/**
 * \enum I2C_SLV_PIN_MUX_E
 * \brief I2C Slave PIN Mux Type
 */
typedef enum {
    // IIC_SLAVE0
    IIC_SLAVE0_PIN_MUX_PA2PA3_F0_SCLSDA = 0,
    IIC_SLAVE0_PIN_MUX_PA2PA3_F1_SCLSDA,
    IIC_SLAVE0_PIN_MUX_PA2PA3_F4_SCLSDA,
    
    // IIC_SLAVE1
    IIC_SLAVE1_PIN_MUX_PB0_F4_SDA,
    IIC_SLAVE1_PIN_MUX_PB1_F4_SCL,
    IIC_SLAVE1_PIN_MUX_PB2_F4_SDA,
    IIC_SLAVE1_PIN_MUX_PB3_F4_SCL,
    IIC_SLAVE1_PIN_MUX_PB4_F4_SDA,
    IIC_SLAVE1_PIN_MUX_PB5_F4_SCL,
    IIC_SLAVE1_PIN_MUX_PB6_F4_SDA,
    IIC_SLAVE1_PIN_MUX_PB7_F4_SCL,
    IIC_SLAVE1_PIN_MUX_PB8_F4_SDA,
    IIC_SLAVE1_PIN_MUX_PB9_F4_SCL,
    IIC_SLAVE1_PIN_MUX_PB10_F4_SDA,
    IIC_SLAVE1_PIN_MUX_PB11_F4_SDA,
    IIC_SLAVE1_PIN_MUX_PA2PA3_SCLSDA
} I2C_SLV_PIN_MUX_E;

/**
 * \defgroup    DEVICE_HAL_IIC_CALLBACK IIC Interrupt callback functions
 * \ingroup DEVICE_HAL_IIC
 * \brief   callback function structure for IIC device
 * @{
 */
typedef struct hx_drv_dev_iic_cbs {
    DEV_CALLBACK tx_cb;         /*!< iic data transmit success required bytes callback */
    DEV_CALLBACK rx_cb;         /*!< iic data receive success required bytes callback */
    DEV_CALLBACK err_cb;        /*!< iic error callback */
    DEV_CALLBACK sta_cb;        /*!< iic start condition callback */
    DEV_CALLBACK dma_err_cb;    /*!< iic dma error callback */
} HX_DRV_DEV_IIC_CBS, *HX_DRV_DEV_IIC_CBS_PTR;
/* dw_iic_hal.h :: DEV_IIC_CBS*/
/** @} */

/** IIC Error State */
typedef enum hx_drv_dev_iic_error_state {
    DEV_IIC_ERR_NONE                = 0, /*!< Currently in iic device free state */
    DEV_IIC_ERR_LOST_BUS            = 1, /*!< Master or slave lost bus during operation */
    DEV_IIC_ERR_ADDR_NOACK          = 2, /*!< Slave address is sent but not addressed by any slave devices */
    DEV_IIC_ERR_DATA_NOACK          = 3, /*!< Data in transfer is not acked when it should be acked */
    DEV_IIC_ERR_TIMEOUT             = 4, /*!< Transfer timeout, no more data is received or sent */
    DEV_IIC_ERR_MSTSTOP             = 5, /*!< Slave received a STOP condition from master device */
    DEV_IIC_ERR_UNDEF               = 6, /*!< Undefined error cases */
    DEV_IIC_ERR_TX_DATA_UNREADY     = 7, /*!< I2C Master read request but I2C slave tx data is unready (clock stretching by i2c slave) */
    DEV_IIC_ERR_RX_DMA_UNREADY      = 8, /*!< I2C Master send data but I2C slave dma mode is unready (clock stretching by i2c slave) */
} HX_DRV_DEV_IIC_ERROR_STATE;

/**
 * \brief   iic information struct definition
 * \details informations about iic open state, working state,
 *  baurate, iic registers, working method, interrupt number
 */
typedef struct hx_drv_dev_iic_info {
    void *iic_ctrl;                 /*!< iic control related pointer, implemented by bsp developer, and this should be set during iic object implementation, refer to DW_IIC_CTRL_PTR */
    uint32_t opn_cnt;               /*!< iic open count, open it will increase 1, close it will decrease 1, 0 for close, >0 for open */
    uint32_t status;                /*!< current working status, refer to \ref DEVICE_HAL_COMMON_DEVSTATUS, this should be \ref DEV_ENABLED for first open */
    uint32_t mode;                  /*!< current working mode, which can be \ref DEV_MASTER_MODE "master mode" or \ref DEV_SLAVE_MODE "slave mode" */
    uint32_t speed_mode;            /*!< current working \ref IIC_SPEED_MODE "iic speed mode" */
    uint32_t cur_state;             /*!< \ref IIC_WORKING_STATE "current working state for iic device", this should be \ref IIC_FREE for first open */
    uint32_t err_state;             /*!< \ref HX_DRV_DEV_IIC_ERROR_STATE "current error state for iic device", this should be \ref IIC_ERR_NONE for first open */
    uint32_t addr_mode;             /*!< \ref IIC_ADDRESS_MODE "current addressing mode", this should be \ref IIC_7BIT_ADDRESS for first open */
    uint32_t slv_addr;              /*!< slave address when working as slave iic device, this should be 0 for first open */
    uint32_t tar_addr;              /*!< target slave device address when addressing that slave device, this should be 0 for first open */
    uint32_t next_cond;             /*!< \ref IIC_NEXT_CONDTION "next condition for master transmit or receive", \
                                        possible values are STOP or RESTART, it should be STOP for first open */
    uint32_t op_flag;               /*!< 0: interrupt mode, 1: dma mode */                                        
    DEV_BUFFER tx_buf;              /*!< transmit buffer via interrupt, this should be all zero for first open */
    DEV_BUFFER rx_buf;              /*!< receive buffer via interrupt, this should be all zero for first open */
    HX_DRV_DEV_IIC_CBS iic_cbs;     /*!< iic callbacks, for both master and slave mode, this should be all NULL for first open */
    void *extra;                    /*!< a extra pointer to get hook to applications which should not used by bsp developer,
                                        this should be NULL for first open and you can \ref DEV_IIC_INFO_SET_EXTRA_OBJECT "set"
                                        or \ref DEV_IIC_INFO_GET_EXTRA_OBJECT "get" the extra information pointer */
} HX_DRV_DEV_IIC_INFO, * HX_DRV_DEV_IIC_INFO_PTR;
/*dw_iic_hal.h :: DEV_IIC_INFO*/

/**
 * \brief   himax driver iic device interface definition
 * \details define iic device interface, like iic information structure,
 *  fuctions to get iic info, open/close/control iic, send/receive data by iic
 * \note    all this details are implemented by user in user porting code
 */
typedef struct hx_drv_dev_iic {
    HX_DRV_DEV_IIC_INFO iic_info;                                   /*!< iic device information */
    int32_t (*iic_open) (uint32_t mode, uint32_t param);            /*!< open iic device in master/slave mode, \
                                                                        when in master mode, param stands for speed mode, \
                                                                        when in slave mode, param stands for slave address */
    int32_t (*iic_close) (void);                                    /*!< close iic device */
    int32_t (*iic_control) (uint32_t ctrl_cmd, void *param);        /*!< control iic device */
    int32_t (*iic_write) (const void *data, uint32_t len);          /*!< send data by iic device */
    int32_t (*iic_read) (void *data, uint32_t len);                 /*!< read data from iic device */
} HX_DRV_DEV_IIC, * HX_DRV_DEV_IIC_PTR;


/**
 * This function initializes a specified I2C interface with a given base address and speed mode.
 * 
 * @param iic_id The ID of the I2C interface to be initialized. It can be either USE_DW_IIC_0 or
 * USE_DW_IIC_1.
 * @param base_addr The base address of the I2C controller. This is the memory-mapped address of the
 * I2C controller hardware in the system.
 * @param speed_mode The speed mode of the I2C interface, which can be set to one of the following
 * values: DW_IIC_SPEED_STANDARD, DW_IIC_SPEED_FAST, or DW_IIC_SPEED_HIGH.
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cm_init(USE_DW_IIC_E iic_id, uint32_t base_addr, DW_IIC_SPEED_MODE_E speed_mode);


/**
 * This function deinitializes a specified I2C interface.
 * 
 * @param iic_id The I2C interface ID to be deinitialized. It is of type USE_DW_IIC_E.
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cm_deinit(USE_DW_IIC_E iic_id);


/**
 * \brief   get an \ref dev_iic "iic device" by iic device id.
 *  For how to use iic device hal refer to \ref DEVICE_HAL_IIC_DEVSTRUCT "Functions in iic device structure"
 * \param[in]   iic_id  id of iic, defined by user
 * \retval  !NULL   pointer to an \ref dev_iic "iic device structure"
 * \retval  NULL    failed to find the iic device by \ref iic_id
 * \note    need to implemented by user in user code
 */

HX_DRV_DEV_IIC_PTR hx_drv_i2cm_get_dev(USE_DW_IIC_E iic_id);


/**
 * This function sets the speed mode for a specified I2C interface.
 * 
 * @param iic_id The ID of the I2C interface to be configured. It can be either USE_DW_IIC_0 or
 * USE_DW_IIC_1 depending on the specific I2C interface being used.
 * @param speed_mode The speed mode to be set for the I2C interface. It is of type DW_IIC_SPEED_MODE_E,
 * which is an enumeration that defines the different speed modes supported by the I2C interface.
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cm_set_speed(USE_DW_IIC_E iic_id, DW_IIC_SPEED_MODE_E speed_mode);


/**
 * The function `hx_drv_i2cm_set_err_cb` sets the error callback function for a specified I2C module.
 * 
 * @param iic_id The iic_id parameter is an enumeration that specifies the I2C interface to use. It can
 * have the values USE_DW_IIC_0 or USE_DW_IIC_1, which correspond to the first and second I2C
 * interfaces respectively.
 * @param err_cb The parameter "err_cb" is a pointer to a callback function that will be called when an
 * error occurs during I2C communication. This callback function should have the following signature:
 * 
 * @return The function `hx_drv_i2cm_set_err_cb` returns an error code of type `IIC_ERR_CODE_E`.
 */
IIC_ERR_CODE_E hx_drv_i2cm_set_err_cb(USE_DW_IIC_E iic_id, void * err_cb);


/**
 * This function writes data to an I2C device with interrupt support.
 * 
 * @param iic_id The ID of the I2C interface to use (either USE_DW_IIC_0 or USE_DW_IIC_1).
 * @param slave_addr_sft The slave address shifted by 1 bit to the left (7-bits). This is because the least
 * significant bit of the slave address is used to indicate whether the I2C transaction is a read or
 * write operation.
 * @param data Pointer to the data buffer to be written to the I2C slave device.
 * @param data_len The length of the data buffer to be written to the I2C slave device.
 * @param tx_cb tx_cb is a pointer to a callback function that will be called when the write operation
 * is completed.
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cm_interrupt_write(USE_DW_IIC_E iic_id, uint8_t slave_addr_sft, uint8_t *data, uint32_t data_len, void * tx_cb);


/**
 * The function performs an interrupt-based read operation on a specified I2C slave device using a
 * designated I2C interface.
 * 
 * @param iic_id The ID of the I2C interface to use (either USE_DW_IIC_0 or USE_DW_IIC_1).
 * @param slave_addr_sft The slave address shifted by 1 bit to the left (7-bits). This is because the least
 * significant bit of the slave address is used to indicate whether the I2C transaction is a read or
 * write operation.
 * @param data Pointer to the buffer where the received data will be stored.
 * @param data_len The length of the data buffer that will be read from the I2C slave device.
 * @param rx_cb rx_cb is a pointer to a callback function that will be called when the I2C read
 * operation is complete. The callback function should take a single argument, which is a pointer to
 * the data buffer where the received data is stored.
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cm_interrupt_read(USE_DW_IIC_E iic_id, uint8_t slave_addr_sft, uint8_t *data, uint32_t data_len, void * rx_cb);


/**
 * The function hx_drv_i2cm_write_data writes data to an I2C device using the specified I2C interface
 * and slave address.
 * 
 * @param iic_id The ID of the I2C interface to use (either USE_DW_IIC_0 or USE_DW_IIC_1).
 * @param slave_addr_sft The slave address shifted by 1 bit to the left (7-bits). This is because the least
 * significant bit of the slave address is used to indicate whether the I2C transaction is a read or
 * write operation.
 * @param addr An array of bytes representing the address to write to on the I2C device.
 * @param addr_len The length of the address array.
 * @param data An array of bytes containing the data to be written to the I2C slave device.
 * @param data_len The length of the data array that contains the data to be written to the I2C slave
 * device.
 * 
 * Notice: 
 *    addr_len + data_len <= 32 bytes
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cm_write_data(USE_DW_IIC_E iic_id, uint8_t slave_addr_sft, uint8_t addr[], uint32_t addr_len, uint8_t data[], uint32_t data_len);


/**
 * This function reads data from a specified I2C slave device using the I2C Master
 * interface.
 * 
 * @param iic_id The ID of the I2C interface to use for the read operation.
 * @param slave_addr_sft The slave address shifted by 1 bit to the left (7-bits). This is because the least
 * significant bit of the slave address is used to indicate whether the I2C transaction is a read or
 * write operation.
 * @param data An array of uint8_t where the read data will be stored.
 * @param len The length of the data buffer to be read from the I2C slave device.
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cm_read_data(USE_DW_IIC_E iic_id, uint8_t slave_addr_sft, uint8_t data[], uint32_t len);


/**
 * This function writes data to an I2C device and then reads data from the same device using a restart
 * condition.
 * 
 * @param iic_id The ID of the I2C interface to use (either USE_DW_IIC_0 or USE_DW_IIC_1).
 * @param slave_addr_sft The slave address shifted by 1 bit to the left (7-bits). This is because the least
 * significant bit of the slave address is used to indicate whether the I2C transaction is a read or
 * write operation.
 * @param addr The address of the register or memory location to write to on the I2C slave device. It
 * is an array of uint8_t type.
 * @param addr_len The length of the address array.
 * @param data The data array is the buffer that will hold the data read from the I2C slave device. The
 * function will write the address bytes to the slave device, then restart the communication and read
 * the specified number of bytes into the data buffer.
 * @param data_len The length of the data buffer to be read from the I2C slave device.
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cm_write_restart_read(
    USE_DW_IIC_E iic_id,
    uint8_t slave_addr_sft,
    uint8_t addr[],
    uint32_t addr_len,
    uint8_t data[],
    uint32_t data_len);


/**
 * This function writes data to an I2C device and then reads data from the same device using stop mode.
 * 
 * @param iic_id The ID of the I2C interface to use (either USE_DW_IIC_0 or USE_DW_IIC_1).
 * @param slave_addr_sft The slave address shifted by 1 bit to the left  (7-bits). This is because the least
 * significant bit of the slave address is used to indicate whether the I2C transaction is a read or
 * write operation.
 * @param addr The address of the register to write to on the I2C slave device.
 * @param addr_len The length of the address array (addr[]) in bytes.
 * @param data The data parameter is a pointer to an array of bytes that will be filled with the data
 * read from the I2C slave device. The length of this array is specified by the data_len parameter.
 * @param data_len The length of the data buffer to be read from the I2C slave device.
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cm_write_stop_read(
    USE_DW_IIC_E iic_id,
    uint8_t slave_addr_sft,
    uint8_t addr[],
    uint32_t addr_len,
    uint8_t data[],
    uint32_t data_len);

/**
 * This function initializes the I2C slave device and returns an error code.
 * 
 * @param iic_id The ID of the I2C slave interface to be initialized. It can be either USE_DW_IIC_SLV_0
 * or USE_DW_IIC_SLV_1.
 * @param base_addr The base address of the I2C slave device.
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cs_init(USE_DW_IIC_SLV_E iic_id, uint32_t base_addr);


/**
 * The function deinitializes a specified I2C slave interface.
 * 
 * @param iic_id The I2C slave interface ID to be deinitialized. It can be either USE_DW_IIC_SLV_0 or
 * USE_DW_IIC_SLV_1.
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cs_deinit(USE_DW_IIC_SLV_E iic_id);

/**
 * The function hx_drv_i2cs_reset resets the specified I2C slave module.
 * 
 * @param iic_id The parameter `iic_id` is of type `USE_DW_IIC_SLV_E` and is used to specify the I2C
 * slave interface to reset. 
 */
void hx_drv_i2cs_reset(USE_DW_IIC_SLV_E iic_id);

/**
 * The function `hx_drv_i2cs_set_err_cb` sets the error callback function for a specified I2C slave
 * device.
 * 
 * @param iic_id The iic_id parameter is an enumeration value that specifies the I2C slave device to
 * set the error callback for. It can have the values USE_DW_IIC_SLV_0 or USE_DW_IIC_SLV_1.
 * @param err_cb err_cb is a pointer to a callback function that will be called when an error occurs
 * during I2C communication.
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cs_set_err_cb(USE_DW_IIC_SLV_E iic_id, void * err_cb);

/**
 * The function returns a pointer to a device structure for a specified I2C slave interface.
 * 
 * @param iic_id An enumeration type variable that specifies which I2C slave device to use. It can be
 * either USE_DW_IIC_SLV_0 or USE_DW_IIC_SLV_1.
 * 
 * @return a pointer to a structure of type DEV_IIC_PTR, which is cast to a pointer of type
 * HX_DRV_DEV_IIC_PTR.
 */
HX_DRV_DEV_IIC_PTR hx_drv_i2cs_get_dev(USE_DW_IIC_SLV_E iic_id);


/**
 * This function writes data to an I2C master device with interrupt handling.
 * 
 * @param iic_id The ID of the I2C slave device. It can be either USE_DW_IIC_SLV_0 or
 * USE_DW_IIC_SLV_1.
 * @param slv_addr The address of the I2C slave device (7-bits) that the master wants to communicate with.
 * @param data A pointer to the data buffer to be written to the I2C master device.
 * @param num num is the number of bytes to be written to the I2C master device.
 * @param tx_cb tx_cb is a pointer to a callback function that will be called when the I2C write
 * operation is complete.
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cs_interrupt_write(USE_DW_IIC_SLV_E iic_id, uint32_t slv_addr, const uint8_t *data, uint32_t num, void * tx_cb);


/**
 * This function reads data from an I2C master device with interrupt support.
 * 
 * @param iic_id The ID of the I2C slave device. It can be either USE_DW_IIC_SLV_0 or
 * USE_DW_IIC_SLV_1.
 * @param slv_addr The address of the I2C slave device (7-btis) that the master wants to communicate with.
 * @param data Pointer to the buffer where the received data will be stored.
 * @param num The number of bytes to be read from the I2C master device.
 * @param rx_cb rx_cb is a pointer to a callback function that will be called when the I2C read
 * operation is complete. This function should be defined by the user and will be passed the received
 * data as a parameter.
 * 
 * @return an error code of type IIC_ERR_CODE_E.
 */
IIC_ERR_CODE_E hx_drv_i2cs_interrupt_read(USE_DW_IIC_SLV_E iic_id, uint32_t slv_addr, uint8_t *data, uint32_t num, void * rx_cb);

#ifndef BOOT_USED
#ifdef IP_INST_DMA3
/**
 * The function `hx_drv_i2cs_dma_write` is used to perform a DMA write operation on an I2C slave
 * device.
 * 
 * @param iic_id The I2C slave ID. It can be either USE_DW_IIC_SLV_0 or USE_DW_IIC_SLV_1.
 * @param slv_addr The slave address of the I2C device you want to communicate with.
 * @param dma_ch The dma_ch parameter is the DMA channel number to be used for the I2C DMA write
 * operation.
 * @param data A pointer to the data to be written to the I2C slave device. The data should be in the
 * form of an array of 16-bit values that the 8-bit Most Significant Bit (MSB) must be zero, 
 * as it is reserved for filling the control bits in the data FIFO buffer.
 * @param num The parameter "num" represents the number of data elements to be written via DMA.
 *            and the max single transfer size is 4095.
 * @param tx_cb The parameter "tx_cb" is a void pointer to a callback function that will be called when
 * the DMA transfer is complete.
 * 
 * @return The function `hx_drv_i2cs_dma_write` returns an error code of type `IIC_ERR_CODE_E`.
 */
IIC_ERR_CODE_E hx_drv_i2cs_dma_write(USE_DW_IIC_SLV_E iic_id, uint32_t slv_addr, uint8_t dma_ch, const uint16_t *data, uint32_t num, void * tx_cb);

/**
 * The function `hx_drv_i2cs_dma_read` reads data from an I2C slave device using DMA.
 * 
 * @param iic_id The iic_id parameter is the ID of the I2C slave device to be used. It can be either
 * USE_DW_IIC_SLV_0 or USE_DW_IIC_SLV_1.
 * @param slv_addr The slave address of the I2C device you want to communicate with.
 * @param dma_ch The parameter "dma_ch" is the DMA channel number to be used for the I2C DMA read
 * operation.
 * @param data A pointer to the 8-bit buffer where the received data will be stored.
 * @param num The parameter "num" represents the number of bytes to be read from the I2C slave device.
 *            and the max single transfer size is 4095.
 * @param rx_cb The parameter "rx_cb" is a callback function that will be called when the DMA transfer
 * is complete. It is used to notify the user that the data transfer has finished.
 * 
 * @return The function `hx_drv_i2cs_dma_read` returns an error code of type `IIC_ERR_CODE_E`.
 */
IIC_ERR_CODE_E hx_drv_i2cs_dma_read(USE_DW_IIC_SLV_E iic_id, uint32_t slv_addr, uint8_t dma_ch, uint8_t *data, uint32_t num, void * rx_cb);
#endif
#endif
#ifdef __cplusplus
}
#endif
/** @} */
#endif /*INC_HX_DRV_IIC_H_*/
