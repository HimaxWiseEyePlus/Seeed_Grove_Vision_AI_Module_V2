#ifndef INC_HX_DRV_UART_H_
#define INC_HX_DRV_UART_H_

#include "dev_common.h"
/*The flag for bootloader/bootRom*/
/* no support rs485 & dma*/
#if defined(BOOT_USED)
#define UART_BL_USE
#ifdef SECONDBOOTLOADER
#define UART_2ND_BL_USE
#endif
#endif

/**
 * \defgroup	UART
 * \brief	Universal Asynchronous Receiver/Transmitter
 * \details	WE2 has three sets of UART, but only UART1 supports RS485 & IRDA 1.0
 * <pre>
 *      Sample code: UART0 pin mux configuration and initialization
 *      //The output pin of UART0 is defined by the user application.
 *      hx_drv_scu_set_PB0_pinmux(SCU_PB0_PINMUX_UART0_RX_1, 1);
 *      hx_drv_scu_set_PB1_pinmux(SCU_PB1_PINMUX_UART0_TX_1, 1);
 * 
 *      //initialize UART 0
 *      hx_drv_uart_init(USE_DW_UART_0, HX_UART0_BASE);
 * 
 *      Usage-1: Transmit data using Polling mode with UART 1
 *      #define UART_LOCAL_MSG_LEN 64
 *      uint8_t uart_msg[UART_LOCAL_MSG_LEN] = "U0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTVWXYZ[]";
 *      uint8_t wbuffer[UART_LOCAL_MSG_LEN];
 *      DEV_UART_PTR dev_uart_ptr;
 *      uint32_t data_size = 24;
 *      
 *      memcpy(wbuffer, uart_msg, data_size);
 *      dev_uart_ptr = hx_drv_uart_get_dev(USE_DW_UART_1);
 *      dev_uart_ptr->uart_open(UART_BAUDRATE_115200);
 *      e_no = dev_uart_ptr->uart_write(wbuffer, data_size);
 * 
 *      Usage-2: Transmit data using interrpupt mode with UART 1
 *      void uart_dma_tx_cb(void)
 *      {
 *          //Disable DMA request
 *          cprintf("Transfer : TX Done\n");
 *      } 
 *      #define UART_LOCAL_MSG_LEN 64
 *      uint8_t uart_msg[UART_LOCAL_MSG_LEN] = "U0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTVWXYZ[]";
 *      uint8_t wbuffer[UART_LOCAL_MSG_LEN];
 *      DEV_UART_PTR dev_uart_ptr;
 *      uint32_t data_size = 24;
 *      DEV_BUFFER tx_buffer;
 * 
 *      memcpy(wbuffer, uart_msg, data_size);   
 *      tx_buffer.buf = (void *)wbuffer;
 *      tx_buffer.len = data_size;
 * 
 *      dev_uart_ptr = hx_drv_uart_get_dev(USE_DW_UART_1);
 *      dev_uart_ptr->uart_open(UART_BAUDRATE_115200);
 *      dev_uart_ptr->uart_control(UART_CMD_SET_TXCB,  (UART_CTRL_PARAM)uart_dma_tx_cb);
 *      dev_uart_ptr->uart_control(UART_CMD_SET_TXINT_BUF, (UART_CTRL_PARAM)&tx_buffer);
 *      dev_uart_ptr->->uart_control(UART_CMD_SET_TXINT, (UART_CTRL_PARAM)1);
 * 
 *      Usage-3: Transmit data using DMA mode with UART 1
 *      #define UART_LOCAL_MSG_LEN 64
 *      uint8_t uart_msg[UART_LOCAL_MSG_LEN] = "U0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTVWXYZ[]";
 *      uint8_t wbuffer[UART_LOCAL_MSG_LEN];
 *      DEV_UART_PTR dev_uart_ptr;
 *      uint32_t data_size = 24;
 *      
 *      memcpy(wbuffer, uart_msg, data_size);
 *      hx_CleanDCache_by_Addr((volatile void *)wbuffer, (sizeof(char) * data_size));
 *      dev_uart_ptr = hx_drv_uart_get_dev(USE_DW_UART_1);
 *      dev_uart_ptr->uart_open(UART_BAUDRATE_115200);
 *      e_no = dev_uart_ptr->uart_write_udma(wbuffer, data_size, uart_dma_tx_cb);
 * 
 *      Usage-4: Receive data using interrpupt mode with UART 1
 *      uinr32_t g_datasize = 0;
 *      void uart_dma_rx_cb(void)
 *      {
 *          int i;
 *          cprintf("Transfer : RX Done\n");
 *          cprintf("\nData Received: ");
 *          for (i = 0; i < g_datasize; i++)
 *                  cprintf("0x%02x,", rbuffer[i]);
 *          memset(rbuffer, 0x00, g_datasize);
 *      }
 *      DEV_UART_PTR dev_uart_ptr;
 *      uint8_t rbuffer[64];
 *      uint32_t data_size =32;
 *      DEV_BUFFER rx_buffer;
 * 
 *      memset(rbuffer, 0x00, data_size);
 *      g_datasize = data_size;
 *      rx_buffer.buf = (void *)rbuffer;
 *      rx_buffer.len = g_datasize;
 * 
 *      dev_uart_ptr = hx_drv_uart_get_dev(USE_DW_UART_1);
 *      dev_uart_ptr->uart_open(UART_BAUDRATE_115200);
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RXCB,  (UART_CTRL_PARAM)uart_dma_rx_cb);
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RXINT_BUF, (UART_CTRL_PARAM)&rx_buffer);
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RXINT, (UART_CTRL_PARAM)1);
 * 
 *      Usage5-1: RS485 Full Duplex Mode
 *      //The user can choose when to transmit or when to receive. 
 *      //Both 're' and 'de' can be simultaneously asserted or de-asserted at any time.
 *      DEV_UART_PTR dev_uart_ptr;
 *      UART_RS485_REDE_EN_T config;
 *      uint8_t uart_msg[UART_LOCAL_MSG_LEN] = "U0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTVWXYZ[]";
 *      uint32_t data_size = 24;
 * 
 *      dev_uart_ptr = hx_drv_uart_get_dev(USE_DW_UART_1);
 *      dev_uart_ptr->uart_open(UART_BAUDRATE_115200);
 * 
 *      //set Full deplex mode
 *      dev_uart_ptr->uart_control(UART_CMD_SET_WORK_MODE, (UART_CTRL_PARAM)UART_RS485_FULL_DUPLEX);
 * 
 *      //only De signal enable
 *      config.rede = UART_RS485_RE;
 *      config.enable = 0;
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RS485_REDE_EN, (UART_CTRL_PARAM)&config);
  *     config.rede = UART_RS485_DE;
 *      config.enable = 1;
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RS485_REDE_EN, (UART_CTRL_PARAM)&config);
 *      e_no = dev_uart_ptr->uart_write(uart_msg, data_size); 
 * 
 *      //only Re singel enable
 *      uint8_t rbuffer[64];
 *      config.rede = UART_RS485_RE;
 *      config.enable = 1;
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RS485_REDE_EN, (UART_CTRL_PARAM)&config);
  *     config.rede = UART_RS485_DE;
 *      config.enable = 0;
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RS485_REDE_EN, (UART_CTRL_PARAM)&config);
 *      e_no = dev_uart_ptr->uart_read(rbuffer, data_size); 
 * 
 *      Usage5-2: RS485 set Assertion time and turnaround time
 *      DEV_UART_PTR dev_uart_ptr;
 *      DEV_UART_INFO_PTR uart_info;
 *      uint32_t freq;
 *      UART_RS485_ASSERTTION_TIME_T time_config;
 *      
 *      dev_uart_ptr = hx_drv_uart_get_dev(USE_DW_UART_1);
 *      dev_uart_ptr->uart_open(UART_BAUDRATE_115200);  
 *      uart_info = &(dev_spi_m->spi_info);
 *      freq = uart_info->ref_clk;
 * 
 *      //set Driver Enable assertion time = 700ns
 *      uint32_t DE_Assertion_Time = 700;
 *      uint32_t v_clk_cnt = (DE_Assertion_Time * freq) / 1e9 + 1;
 * 
 *      time_config.det_mode = UART_DE_ASSERTION;
 *      time_config.time = v_clk_cnt;
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RS485_AE_TIME, (UART_CTRL_PARAM)&time_config);
 * 
 *      //set Driver Enable de-assertion time = 12.6 us
 *      uint32_t DE_deAssertion_Time = 12600;
 *      v_clk_cnt = (DE_deAssertion_Time * freq) / 1e9 + 1;
 *      time_config.det_mode = UART_DE_DE_ASSERTION;
 *      time_config.time = v_clk_cnt;
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RS485_AE_TIME, (UART_CTRL_PARAM)&time_config);
 * 
 *      //set RE_to_DE/DE_to_RE TurnAround time = 0.1 s
 *      uint32_t tr_time = 100000;
 *      UART_RS485_TURNAROUND_TIME_T tr_config;
 *      v_clk_cnt = (tr_time * freq) / 1e9;
 *      tr_config.tat_mode = UART_TAT_ALL;
 *      tr_config.time = 100000;
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RS485_TA_TIME, (UART_CTRL_PARAM)&tr_config);
 * 
 *      Usage 5-2: RS485 Software-Controlled Half Duplex Mode
 *      //DE and RE are mutually exclusive
 *      DEV_UART_PTR dev_uart_ptr;
 *      dev_uart_ptr = hx_drv_uart_get_dev(USE_DW_UART_1);
 *      dev_uart_ptr->uart_open(UART_BAUDRATE_115200); 
 *  
 *      //only De signal enable
 *      uint8_t uart_msg[UART_LOCAL_MSG_LEN] = "U0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTVWXYZ[]";
 *      uint32_t data_size = 24;
 *      UART_RS485_REDE_EN_T config; 
 *      config.rede = UART_RS485_RE;
 *      config.enable = 0;
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RS485_REDE_EN, (UART_CTRL_PARAM)&config);
  *     config.rede = UART_RS485_DE;
 *      config.enable = 1;
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RS485_REDE_EN, (UART_CTRL_PARAM)&config);
 *      e_no = dev_uart_ptr->uart_write(uart_msg, data_size); 
 * 
 *      //only Re singel enable
 *      uint8_t rbuffer[64];
 *      config.rede = UART_RS485_RE;
 *      config.enable = 1;
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RS485_REDE_EN, (UART_CTRL_PARAM)&config);
  *     config.rede = UART_RS485_DE;
 *      config.enable = 0;
 *      dev_uart_ptr->uart_control(UART_CMD_SET_RS485_REDE_EN, (UART_CTRL_PARAM)&config);
 *      e_no = dev_uart_ptr->uart_read(rbuffer, data_size); 
 * </pre>
 * @{
 */

/**
 * \defgroup UART DMA information
 * \brief DMA default setting:
 * UART0 : use DMA3, TX channel = 0, RX channel = 1
 * UART1 : use DMA3, TX channel = 2, RX channel = 3
 * UART2 : use DMA3, TX channel = 4, RX channel = 5
 * 
 * \note Each UART corresponds to a specific DMA and cannot be changed.
 * \note Change DMA channel used by UART, use \ref UART_DMA_CHANNELS_S to set
 */

/**
 * \brief  defines DW UART Object Number
 *
 */
#define DW_UART_NUM (3) /*!< The number of WE2 UART*/

/**
 * \brief  defines DW UART Object ID Macros
 *
 */
#define DW_UART_0_ID 0             /*!< uart 0 id macro */
#define DW_UART_1_ID 1             /*!< uart 1 id macro, supprot RS485 & IRDA 1.0*/
#define DW_UART_2_ID 2             /*!< uart 2 id macro */
#define DW_UART_all_ID DW_UART_NUM /*!< uart All id macro */

#define SUPPORT_RS485 1
#define UART_UDMA 1

/**
 * \brief UART write/read dma function support Linked-list 
 * if there flag are defined, the Max size of uart_write_udma/uart_read_udma can up to 1 MB.
 * otherwise, max size only is up to 4095.
 */
#define UART_0_DMA_LLI_SUPPORT
#define UART_1_DMA_LLI_SUPPORT
#define UART_2_DMA_LLI_SUPPORT

/**
 * \enum USE_DW_UART_E
 * \brief DW UART Object ID
 */
typedef enum USE_DW_UART_S
{
    USE_DW_UART_0 = DW_UART_0_ID, /*!< enable use datafusion subsystem uart 0 */
#if !defined(UART_BL_USE) || defined(UART_2ND_BL_USE)
    USE_DW_UART_1 = DW_UART_1_ID, /*!< enable use datafusion subsystem uart 1 */
    USE_DW_UART_2 = DW_UART_2_ID, /*!< enable use datafusion subsystem uart 2 */
#endif
    USE_DW_UART_ALL = DW_UART_all_ID, /*!< enable use datafusion subsystem uart all */
    USE_DW_UART_MAX
} USE_DW_UART_E;

/**
 * \defgroup	DEVICE_HAL_UART_BAUD	UART Device Baudrate Definitions
 * \ingroup	DEVICE_HAL_UART
 * \brief	Macros for uart baudrate.
 * \details	Definitions for baudrate from 4800 to 115200bps.
 * @{
 */
/**
 * \brief  defines for uart baudrates
 *
 */
#define UART_BAUDRATE_110 (110)           /*!< uart baudrate 110bps */
#define UART_BAUDRATE_300 (300)           /*!< uart baudrate 300bps */
#define UART_BAUDRATE_600 (600)           /*!< uart baudrate 600bps */
#define UART_BAUDRATE_1200 (1200)         /*!< uart baudrate 1200bps */
#define UART_BAUDRATE_2400 (2400)         /*!< uart baudrate 2400bps */
#define UART_BAUDRATE_4800 (4800)         /*!< uart baudrate 4800bps */
#define UART_BAUDRATE_9600 (9600)         /*!< uart baudrate 9600bps */
#define UART_BAUDRATE_14400 (14400)       /*!< uart baudrate 14400bps */
#define UART_BAUDRATE_19200 (19200)       /*!< uart baudrate 19200bps */
#define UART_BAUDRATE_38400 (38400)       /*!< uart baudrate 38400bps */
#define UART_BAUDRATE_57600 (57600)       /*!< uart baudrate 57600bps */
#define UART_BAUDRATE_115200 (115200)     /*!< uart baudrate 115200bps */
#define UART_BAUDRATE_230400 (230400)     /*!< uart baudrate 230400bps */
#define UART_BAUDRATE_460800 (460800)     /*!< uart baudrate 460800bps */
#define UART_BAUDRATE_921600 (921600)     /*!< uart baudrate 921600bps */

/** @} */

/**
 * \defgroup	DEVICE_HAL_UART_FORMAT	UART Device Format Definitions
 * \ingroup	DEVICE_HAL_UART
 * \brief	Macros for uart format.
 * \details	Definitions for uart format like databits, parity, stopbits.
 * @{
 */
/**
 * \enum UART_PARITY
 * \brief UART Device Parity Types Enum
 */
typedef enum
{
    UART_PARITY_NONE = 0, /*!< no parity bit */
    UART_PARITY_ODD = 1,  /*!< odd parity bit */
    UART_PARITY_EVEN = 2, /*!< even parity bit */
    UART_PARITY_MARK = 3, /*!< mark parity bit, always logical 1 */
    UART_PARITY_SPACE = 4 /*!< space parity bit, always logical 0 */
} UART_PARITY;

/**
 * \enum UART_STOPBITS
 * \brief UART Device Stop Bits Enum
 */
typedef enum
{
    UART_STPBITS_ONE = 0,     /*!< 1 stop bit */
    UART_STPBITS_ONEHALF = 1, /*!< 1.5 stop bits */
    UART_STPBITS_TWO = 2      /*!< 2 stop bits */
} UART_STOPBITS;

/**
 * \enum UART_TRANSFER_MODE
 * \brief UART Transfer Mode
 */
typedef enum uart_transfer_mode
{
    UART_TRANSFER_INT = 0, /*!< Use DMA mode */
    UART_TRANSFER_DMA = 1  /*!< Use INTERRUPT mode */
} UART_TRANSFER_MODE;

/**
 * \struct UART_DPS_FORMAT
 * \brief UART DPS Format: databits/parity/stopbits
 * databits: 5 bits~8 bits
 */
typedef struct uart_dps_format
{
    uint32_t databits;      /*!< data bits */
    UART_PARITY parity;     /*!< parity bit type */
    UART_STOPBITS stopbits; /*!< stop bits */
} UART_DPS_FORMAT;
#define UART_DATABITS_DEFAULT 8 /*!< default data bits */

/** Default UART DPS format */
static const UART_DPS_FORMAT dps_format_default = {UART_DATABITS_DEFAULT, UART_PARITY_NONE, UART_STPBITS_ONE};
/** @} */

/**
 * \enum UART_HW_FLOW_CONTROL
 * \brief UART Device Hardware Flow Control Types Enum
 */
typedef enum
{
    UART_FC_NONE = 0, /*!< Non hardware flow control */
    UART_FC_RTS = 1,  /*!< Request To Send */
    UART_FC_CTS = 2,  /*!< Clear To Send */
    UART_FC_BOTH = 3  /*!< Both hardware flow control methods */
} UART_HW_FLOW_CONTROL;
/** Default hardware flow control method */
static const UART_HW_FLOW_CONTROL hwfc_default = UART_FC_NONE;

/** 
 * \enum UART_WORK_MODE_T
 * \brief UART model select enum
 * UART_NORMAL: RS232,  max baudrate: UART_BAUDRATE_921600
 * UART_IRDA: IrDA 1.0 SIR Protocol,max baudrate: 115200
 * 
 * UART_RS485_FULL_DUPLEX:
 *      >both transmit and receive transfers simultaneously
 *      >default setting:
 *          >>DE signal: enable; RE signal: enable
 *          >>DE signal polarity: high; RE signal polarity: low
 *          >>UART doesn't impose any turnaround time(re to de / de to re).
 *          >>de-assertion time: 8; assertion time:8 (unit type: reference clock periods)
 * 
 * UART_RS485_SW_HALF_DUPLEX:
 *      >either transmit or receive transfers at a time but not both simultaneously.
 *          >>DE signal: enable; RE signal: disable
 *          >>DE signal polarity: high; RE signal polarity: low
 *          >>de to re turnaround time:8; re to de turnaround time:8. (unit type: reference clock periods)
 *          >>de-assertion time: 8; assertion time:8 (unit type: reference clock periods)
 * 
 * UART_RS485_HW_HALF_DUPLEX:
 *      >either transmit or receive transfers at a time but not both simultaneously.
 *          >>DE signal: disable; RE signal: enable
 *          >>DE signal polarity: high; RE signal polarity: low
 *          >>de to re turnaround time:8; re to de turnaround time:8. (unit type: reference clock periods)
 *          >>de-assertion time: 8; assertion time:8 (unit type: reference clock periods)
 */
typedef enum uart_work_mode
{
    UART_NORMAL,
    UART_IRDA,
    UART_RS485_FULL_DUPLEX,
    UART_RS485_SW_HALF_DUPLEX,
    UART_RS485_HW_HALF_DUPLEX,
} UART_WORK_MODE_T;

/**
 * \enum UART_RS485_REDE_T
 * \brief UART RE/DE select for RS485
 */
typedef enum uart_rs485_rede
{
    UART_RS485_DE,
    UART_RS485_RE,
    UART_RS485_REDE,
} UART_RS485_REDE_T;

/**
 * \enum UART_REDE_POLARITY_T;
 * \brief UART signal Polarity.
 */
typedef enum uart_rede_polarity
{
    UART_LOW,
    UART_HIGH,
} UART_REDE_POLARITY_T;

/**
 * \struct UART_RS485_REDE_POL_T
 * \brief RE_EN/DE_EN singal Polarity struct
 */
typedef struct uart_rs485_rede_pol_s
{
    UART_RS485_REDE_T rede;
    UART_REDE_POLARITY_T pol;
} UART_RS485_REDE_POL_T;

/**
 * \struct UART_RS485_REDE_EN_T
 * \brief RE_EN/DE_EN singal enable 
 */
typedef struct uart_rs485_rede_en_s
{
    UART_RS485_REDE_T rede;
    uint32_t enable;
} UART_RS485_REDE_EN_T;

/**
 * \enum UART_DET_MODE_T
 * \brief DE_assertion DE_de-assertion time enum
 * 
 * de-assertion time: 
 *      >The time between the end of stop bit on the sout to the falling edge of Driver output enable signal.
 * assertion time:
 *      >timebetween the assertion of rising edge of Driver output enable signal to serial transmit enable.
 */
typedef enum uart_det_mode
{
    UART_DE_ASSERTION,
    UART_DE_DE_ASSERTION,
    UART_DE_ALL,
} UART_DET_MODE_T;

/**
 * \struct uart_rs485_assertion_time_s
 * \brief DE assertion and de-assertion timings of 'de' signal.
 * 
 * \param det_mode: UART_DE_ASSERTION/UART_DE_DE_ASSERTION/UART_DE_ALL
 * \param time: number of reference clock periods. max valid value: 255
 */
typedef struct uart_rs485_assertion_time_s
{
    UART_DET_MODE_T det_mode;
    uint32_t time;
} UART_RS485_ASSERTTION_TIME_T;

/**
 * \enum UART_TAT_MODE_T
 * \brief Turnaround Time type
 * 
 * UART_DE_TO_RE: DE to RE Turnaround Time
 *      >wait state before switching to receive mode from transmit mode
 * UART_RE_TO_DE: RE to DE Turnaround Time
 *      >wait state before switching to transmit mode from receive mode
 */
typedef enum uart_tat_mode
{
    UART_DE_TO_RE,
    UART_RE_TO_DE,
    UART_TAT_ALL,
} UART_TAT_MODE_T;

/**
 * \struct UART_RS485_TURNAROUND_TIME_T
 * \brief RS4885 trunaround time config. struct
 * 
 * \param tat_mode: DE__to_RE\ RE_to_DE\ both
 * \param time: number of reference clock periods max valid value: 65535
 * 
 * for RE_to_DE:
 *      if DE assertion time == 0:
 *          actual value = the programmed value + 3
 *      else if DE assertion time == 1
 *          actual value = the programmed value + 2
 *      else if DE assertion time > 1
 *          actual value = the programmed value + 1
 * 
 * for DE_to_RE:
 *      actual time = the programmed value + 1.
 */
typedef struct uart_rs485_turnaround_time_s
{
    UART_TAT_MODE_T tat_mode;
    uint32_t time;
} UART_RS485_TURNAROUND_TIME_T;


typedef struct uart_dma_channels_s
{
    uint8_t uart_id;    /*< UART id */
    uint8_t tx_dma;     /*< dma channel of Tx */
    uint8_t rx_dma;     /*< dma channel of Rx */
} UART_DMA_CHANNELS_S;

/**
 * \defgroup	DEVICE_HAL_UART_CTRLCMD		UART Device Control Commands
 * \ingroup	DEVICE_HAL_UART
 * \brief	Definitions for uart control command, used in \ref dev_uart::uart_control "UART IO Control"
 * \details	These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - \ref E_OK,	Control device successfully
 *   - \ref E_CLSED,	Device is not opened
 *   - \ref E_OBJ,	Device object is not valid or not exists
 *   - \ref E_PAR,	Parameter is not valid for current control command
 *   - \ref E_SYS,	Control device failed, due to hardware issues such as device is disabled
 *   - \ref E_CTX,	Control device failed, due to different reasons like in transfer state
 *   - \ref E_NOSPT,	Control command is not supported or not valid
 * @{
 */
/**
 * 
 * \brief Set \ref dev_uart_info::baudrate "uart baudrate".
 * \param type : uint32_t
 * \param usage : uart baudrate, must above zero. Here is a list of \ref DEVICE_HAL_UART_BAUD "possible baudrates"
 * - Return value explanation :
 */
#define UART_CMD_SET_BAUD DEV_SET_SYSCMD(0)
/**
 * Get \ref dev_uart_info::status "current device status"
 * \param type : uint32_t *
 * \param usage : store result of current status
 * - Return value explanation :
 */
#define UART_CMD_GET_STATUS DEV_SET_SYSCMD(1)
/**
 * \brief Enable uart device
 * \param type : NULL
 * \param usage : param is not required
 * - Return value explanation :
 */
#define UART_CMD_ENA_DEV DEV_SET_SYSCMD(2)
/**
 * \brief Disable uart device, when device is disabled,
 * only \ref UART_CMD_ENA_DEV, \ref UART_CMD_DIS_DEV and
 * \ref UART_CMD_GET_STATUS commands can be executed,
 * other commands will return \ref E_SYS
 * \param type : NULL
 * \param usage : param is not required
 * - Return value explanation :
 */
#define UART_CMD_DIS_DEV DEV_SET_SYSCMD(3)
/**
 * \brief Flush uart device output
 * \param type : NULL
 * \param usage : param is not required
 * - Return value explanation :
 */
#define UART_CMD_FLUSH_OUTPUT DEV_SET_SYSCMD(4)
/**
 * \brief Get how many bytes space in uart are available to transmit,
 * this can be used in interrupt callback functions,
 * cooperate with \ref dev_uart::uart_write "uart_write" API to realize non-blocked write
 * \param type : int32_t *
 * \param usage : store the write available bytes, > 0 for available bytes, 0 for not available
 * - Return value explanation :
 */
#define UART_CMD_GET_TXAVAIL DEV_SET_SYSCMD(5)
/**
 * \brief Get how many bytes in uart are available to receive,
 * this can be used in interrupt callback functions,
 * cooperate with \ref dev_uart::uart_read "uart_read" API to realize non-blocked read
 * \param type : int32_t *
 * \param usage : store the read available bytes, > 0 for available bytes, 0 for not available
 * - Return value explanation :
 */
#define UART_CMD_GET_RXAVAIL DEV_SET_SYSCMD(6)
/**
 * \brief Cause a break condition to be transmitted to the receiving device
 * \param type : NULL
 * \param usage : param is not required
 * - Return value explanation :
 */
#define UART_CMD_BREAK_SET DEV_SET_SYSCMD(7)
/**
 * Clear break condition and return to normal
 * \param type : NULL
 * \param usage : param is not required
 * - Return value explanation :
 */
#define UART_CMD_BREAK_CLR DEV_SET_SYSCMD(8)
/**
 * \brief Change uart \ref dev_uart_info::dps_format "D/P/S(Data/Parity/Stop) format"
 * \param type : \ref UART_DPS_FORMAT *
 * \param usage : uart dps format including databits, parity and stopbits
 * - Return value explanation :
 */
#define UART_CMD_SET_DPS_FORMAT DEV_SET_SYSCMD(9)
/**
 * \brief Set uart device \ref dev_uart_info::hwfc "hardware flow control"
 * \param type : \ref UART_HW_FLOW_CONTROL
 * \param usage : uart dps format including databits, parity and stopbits
 * - Return value explanation :
 */
#define UART_CMD_SET_HWFC DEV_SET_SYSCMD(10)
/**
 * \brief Set \ref dev_uart_cbs::tx_cb "uart transmit success callback" function
 * when all required bytes are transmitted for interrupt method
 * 
 * If tx_buffer == NULL and \retval DW_UART_IIR_XMIT_EMPTY flag is trigged:
 * then TX callback function will be trigged.
 * 
 * IF tx_buffer != NULL :
 * TX callback function will be trigged after the data of tx buffer is sended
 * 
 * \param type : \ref DEV_CALLBACK * or NULL
 * \param usage : transmit success callback function for uart
 * - Return value explanation :
 */
#define UART_CMD_SET_TXCB DEV_SET_SYSCMD(11)
/**
 * \brief Set \ref dev_uart_cbs::rx_cb "uart receive success callback" function
 * when all required bytes are received for interrupt method
 * 
 * If rx_buffer == NULL and \retval DW_UART_IIR_DATA_AVAIL flag is trigged:
 * then RX callback function will be trigged.
 * 
 * IF rx_buffer != NULL :
 * RX callback function will be trigged after the aomount of data speciifed by re_buffer is received.
 * 
 * \param type : \ref DEV_CALLBACK * or NULL
 * \param usage : receive success callback function for uart
 * - Return value explanation :
 */
#define UART_CMD_SET_RXCB DEV_SET_SYSCMD(12)
/**
 * Set \ref dev_uart_cbs::err_cb "uart transfer error callback" function
 * when something error happened for interrupt method
 * \param type : \ref DEV_CALLBACK * or NULL
 * \param usage : transfer error callback function for uart
 * - Return value explanation :
 */
#define UART_CMD_SET_ERRCB DEV_SET_SYSCMD(13)
/**
 * \brief Set transmit buffer via interrupt, and it will set \ref dev_uart_info::tx_buf "tx_buf"
 * \param type : DEV_BUFFER * or NULL
 * \param usage : buffer structure pointer, if param is NULL, then it will set tx_buf to NULL
 * - Return value explanation :
 */
#define UART_CMD_SET_TXINT_BUF DEV_SET_SYSCMD(14)
/**
 * \brief Set receive buffer via interrupt, and it will set \ref dev_uart_info::rx_buf "rx_buf"
 * \param type : DEV_BUFFER * or NULL
 * \param usage : buffer structure pointer, if param is NULL, then it will set rx_buf to NULL
 * - Return value explanation :
 */
#define UART_CMD_SET_RXINT_BUF DEV_SET_SYSCMD(15)
/**
 * \brief Enable or disable transmit interrupt
 * \param type : uint32_t
 * \param usage : enable(none-zero) or disable(zero) flag
 * - Return value explanation :
 */
#define UART_CMD_SET_TXINT DEV_SET_SYSCMD(16)
/**
 * \brief Enable or disable receive interrupt
 * \param type : uint32_t
 * \param usage : enable(none-zero) or disable(zero) flag
 * - Return value explanation :
 */
#define UART_CMD_SET_RXINT DEV_SET_SYSCMD(17)
/**
 * \brief Abort current interrupt transmit operation if tx interrupt enabled,
 * it will disable transmit interrupt, and set \ref DEV_IN_TX_ABRT
 * in \ref dev_uart_info::status "status" variable,
 * and call the transmit callback function, when tx callback is finished,
 * it will clear \ref DEV_IN_TX_ABRT and return
 * \param type : NULL
 * \param usage :
 * - Return value explanation :
 */
#define UART_CMD_ABORT_TX DEV_SET_SYSCMD(18)
/**
 * \brief Abort current interrupt receive operation if rx interrupt enabled,
 * it will disable receive interrupt, and set \ref DEV_IN_TX_ABRT
 * in \ref dev_uart_info::status "status" variable,
 * and call the receive callback function, when rx callback is finished,
 * it will clear \ref DEV_IN_TX_ABRT and return
 * \param type : NULL
 * \param usage :
 * - Return value explanation :
 */
#define UART_CMD_ABORT_RX DEV_SET_SYSCMD(19)

/**
 * \brief Flush uart device RX buffer
 * \param type : NULL
 * \param usage : param is not required
 * - Return value explanation :
 */
#define UART_CMD_FLUSH_RX_BUF DEV_SET_SYSCMD(20)

/**
 * \brief Set UART work mode
 * \param type : uint32_t
 * \param usage :
 * 0: uart
 * 1: infrared
 * 2: full duplex RS485, control RE and DE manually
 * 3: software controlled half duplex mode. When receiving data, write DE_EN to '0' otherwise ''1'
 * 4: hardware controlled half duplex mode
 * - Return value explanation :
 */
#define UART_CMD_SET_WORK_MODE DEV_SET_SYSCMD(21)

/**
 * \brief Set RE or DE driver enable polarity
 * \param type : UART_RS485_REDE_POL_T*
 * \param usage : param is not required
 * - Return value explanation :
 */
#define UART_CMD_SET_RS485_REDE_POL DEV_SET_SYSCMD(22)

/**
 * \brief Set RS485 DE and RE signal driver output enable
 * \param type : UART_RS485_REDE_EN_T*
 * \param usage :
 * - Return value explanation :
 */
#define UART_CMD_SET_RS485_REDE_EN DEV_SET_SYSCMD(23)

/**
 * \brief Set assertion and de-assertion time of the DE signal.
 * \param type : UART_RS485_ASSERTTION_TIME_T*
 * \param usage :
 * - Return value explanation :
 */
#define UART_CMD_SET_RS485_AE_TIME DEV_SET_SYSCMD(24)

/**
 * \brief Set turnaround time between switching of RE and DE signals.
 * \param type : UART_RS485_TURNAROUND_TIME_T*
 * \param usage :
 * - Return value explanation :
 */
#define UART_CMD_SET_RS485_TA_TIME DEV_SET_SYSCMD(25)

/**
 * \brief Set the DMA channel of UART.
 * \param type : UART_DMA_CHANNELS_S*
 * \param usage :
 * - Return value explanation :
 */
#define UART_CMD_SET_DMA_CHANNEL DEV_SET_SYSCMD(26)

/**
 * \brief Get the DMA channel of UART.
 * \param type : UART_DMA_CHANNELS_S*
 * \param usage : 
 * - Return value explanation :
 */
#define UART_CMD_GET_DMA_CHANNEL DEV_SET_SYSCMD(27)

/**
 * \brief Update system clock of UART
 * \param type : NULL
 * \param usage :
 * - Return value explanation :
 */
#define UART_CMD_UPDATE_REF_CLK DEV_SET_SYSCMD(28)

/**
 * Get done flag pointer of DMA for UART RX
 * \param type: int32_t
 *  value = -1 -> DMA channel of UART Rx is busy status
 *  value = 1  -> DMA channel of UART Rx is IDEL status
 *  \return value explanation :
 */
#define UART_CMD_GET_RX_DMA_DONE DEV_SET_SYSCMD(29)

/**
 * Get done flag pointer of DMA for UART TX
 * \param type: int32_t
 *  value = -1 -> DMA channel of UART Tx is busy status
 *  value = 1  -> DMA channel of UART Tx is IDEL status
 *  \return value explanation :
 */
#define UART_CMD_GET_TX_DMA_DONE DEV_SET_SYSCMD(30)

/**
 * \defgroup	DEVICE_HAL_UART_CALLBACK	UART Interrupt callback functions
 * \ingroup	DEVICE_HAL_UART
 * \brief	callback function structure for UART device
 * @{
 */
typedef struct dev_uart_cbs
{
    DEV_CALLBACK tx_cb;  /*!< uart data transmit success required bytes callback */
    DEV_CALLBACK rx_cb;  /*!< uart data receive success required bytes callback */
    DEV_CALLBACK err_cb; /*!< uart error callback */
} DEV_UART_CBS, *DEV_UART_CBS_PTR;
/** @} */

/**
 * \defgroup	DEVICE_HAL_UART_DEVSTRUCT	UART Device Interface Definition
 * \ingroup	DEVICE_HAL_UART
 * \brief	Contains definitions of uart device interface structure.
 * \details	This structure will be used in user implemented code, which was called
 *     \ref DEVICE_IMPL "Device Driver Implement Layer" for uart to use in implementation code.
 *     Application developer should use the UART API provided here to access to UART devices.
 *     BSP developer should follow the API definition to implement UART device drivers.
 * @{
 */
/**
 * \brief	UART information struct definition
 * \details	informations about uart open count, working status,
 *     baudrate, uart registers and ctrl structure, uart dps format
 */
typedef struct dev_uart_info
{
    void *uart_ctrl; /*!< uart control related pointer, implemented by bsp developer, and this should be set during uart
                        object implementation */
    uint32_t dev_id;
    uint32_t
        opn_cnt; /*!< uart open count, open it will increase 1, close it will decrease 1, 0 for close, >0 for open */
    uint32_t status;   /*!< current working status, refer to \ref DEVICE_HAL_COMMON_DEVSTATUS, this should be \ref
                          DEV_ENABLED for first open */
    uint32_t baudrate; /*!< uart baud rate, this should be the value of baud passing by uart_open if first successfully
                          opened */
    uint32_t ref_clk;                          
    UART_TRANSFER_MODE transfer_mode; /*!< current transfer \ref UART_TRANSFER_MODE "uart transfer mode" */
    UART_DPS_FORMAT dps_format; /*!< D/P/S format settings for uart device, here is \ref dps_format_default "default
                                   settings for first open" */
    UART_HW_FLOW_CONTROL hwfc; /*!< UART hardware flow control, here is \ref hwfc_default "default hardware flow control
                                  settings for first open" */
    DEV_BUFFER tx_buf;         /*!< transmit buffer via interrupt, this should be all zero for first open */
    DEV_BUFFER rx_buf;         /*!< receive buffer via interrupt, this should be all zero for first open */
    DEV_UART_CBS uart_cbs; /*!< uart callbacks, callback arguments should be \ref DEV_UART * or NULL, this should be all
                              NULL for first open */
    void *extra;           /*!< a extra pointer to get hook to applications which should not used by bsp developer,
                       this should be NULL for first open and you can \ref DEV_UART_INFO_SET_EXTRA_OBJECT "set"
                       or \ref DEV_UART_INFO_GET_EXTRA_OBJECT "get" the extra information pointer */
} DEV_UART_INFO, *DEV_UART_INFO_PTR;

/** Set extra information pointer of uart info */
#define DEV_UART_INFO_SET_EXTRA_OBJECT(uart_info_ptr, extra_info) (uart_info_ptr)->extra = (void *)(extra_info)
/** Get extra information pointer of uart info */
#define DEV_UART_INFO_GET_EXTRA_OBJECT(uart_info_ptr) ((uart_info_ptr)->extra)

/**
 * \brief	UART device interface definition
 * \details	Define uart device interface, like uart information structure,
 * 	provide functions to open/close/control uart, send/receive data by uart
 * \note	All this details are implemented by user in user porting code
 */
typedef struct dev_uart
{
    DEV_UART_INFO uart_info;                                 /*!< UART device information */
    int32_t (*uart_open)(uint32_t baud);                     /*!< Open uart device */
    int32_t (*uart_close)(void);                             /*!< Close uart device */
    int32_t (*uart_control)(uint32_t ctrl_cmd, void *param); /*!< Control uart device */
    int32_t (*uart_write)(const void *data, uint32_t len);   /*!< Send data by uart device(blocked) */
    int32_t (*uart_read)(void *data, uint32_t len);          /*!< Read data from uart device(blocked) */
    int32_t (*uart_read_nonblock)(void *data, uint32_t len); /*!< Read data from uart device(non-blocked) */
    int32_t (*uart_write_udma)(const void *data, uint32_t len,
                               void *cb);                          /*!< Read data from uart device(non-blocking) */
    int32_t (*uart_read_udma)(void *data, uint32_t len, void *cb); /*!< Read data from uart device(non-blocking) */
} DEV_UART, *DEV_UART_PTR;

typedef void* UART_CTRL_PARAM;

/**
 * \fn		int32_t (* dev_uart::uart_open) (uint32_t baud)
 * \details	open an uart device with defined baudrate
 * \param[in]	baud	\ref DEVICE_HAL_UART_BAUD "initial baudrate of uart", must > 0
 * \retval	E_OK	Open successfully without any issues
 * \retval	E_OPNED	If device was opened before with different parameters,
 *			then just increase the \ref dev_uart_info::opn_cnt "opn_cnt" and return \ref E_OPNED
 * \retval	E_OBJ	Device object is not valid
 * \retval	E_PAR	Parameter is not valid
 * \retval	E_NOSPT	Open settings are not supported
 */

/**
 * \fn		int32_t (* dev_uart::uart_close) (void)
 * \details	close an uart device, just decrease the \ref dev_uart_info::opn_cnt "opn_cnt",
 *      if \ref dev_uart_info::opn_cnt "opn_cnt" equals 0, then close the device
 * \retval	E_OK	Close successfully without any issues(including scenario that device is already closed)
 * \retval	E_OPNED	Device is still opened, the device \ref dev_uart_info::opn_cnt "opn_cnt" decreased by 1
 * \retval	E_OBJ	Device object is not valid
 */

/**
 * \fn		int32_t (* dev_uart::uart_control) (uint32_t ctrl_cmd, void *param)
 * \details	control an uart device by \ref ctrl_cmd, with passed \ref param.
 * 	you can control uart device using predefined uart control commands defined using \ref DEV_SET_SYSCMD
 * 	(which must be implemented by bsp developer), such as \ref UART_CMD_SET_BAUD "change baudrate",
 * 	\ref UART_CMD_FLUSH_OUTPUT "flush output" and \ref DEVICE_HAL_UART_CTRLCMD "more".
 * 	And you can also control uart device using your own specified commands defined using \ref DEV_SET_USRCMD,
 * 	but these specified commands should be defined in your own uart device driver implementation.
 * \param[in]		ctrl_cmd	\ref DEVICE_HAL_UART_CTRLCMD "control command", to change or get some thing related to
 * uart \param[in,out]	param		parameters that maybe argument of the command, or return values of the command
 * \retval	E_OK	Control device successfully
 * \retval	E_CLSED	Device is not opened
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	Parameter is not valid for current control command
 * \retval	E_SYS	Control device failed, due to hardware issues, such as device is disabled
 * \retval	E_CTX	Control device failed, due to different reasons like in transfer state
 * \retval	E_NOSPT	Control command is not supported or not valid
 */

/**
 * \fn		int32_t (* dev_uart::uart_write) (const void *data, uint32_t len)
 * \details	send \ref data through uart with defined \ref len(blocked).
 * \param[in]	data	pointer to data need to send by uart, must not be NULL
 * \param[in]	len	length of data to be sent, must > 0
 * \retval	>0	Byte count that was successfully sent for poll method
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	Parameter is not valid
 * \retval	E_SYS	Can't write data to hardware due to hardware issues, such as device is disabled
 */

/**
 * \fn		int32_t (* dev_uart::uart_read) (void *data, uint32_t len)
 * \details	receive \ref data of defined \ref len through uart(blocked).
 * \param[out]	data	pointer to data need to received by uart, must not be NULL
 * \param[in]	len	length of data to be received, must > 0
 * \retval	>0	Byte count that was successfully received for poll method
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	Parameter is not valid
 * \retval	E_SYS	Can't receive data from hardware due to hardware issues, such as device is disabled
 */

/**
 * \fn		int32_t (* dev_uart::uart_read_nonblock) (void *data, uint32_t len)
 * \details	receive \ref data of defined \ref len through uart(non-block). 
 * \param[out]	data	pointer to data need to received by uart, must not be NULL
 * \param[in]	len	length of data to be received, must > 0
 * \retval	>0	Byte count that was successfully received for poll method
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	Parameter is not valid
 * \retval	E_SYS	Can't receive data from hardware due to hardware issues, such as device is disabled
 */

/**
 * \fn		int32_t (*uart_write_udma)(const void *data, uint32_t len, void *cb)
 * \details	send \ref data through uart with defined \ref len(blocked). 
 *          max size: 4095 (if UART_x_DMA_LLI_SUPPORT is defined, max size is 1 MB)
 * \param[in]	data	pointer to data need to send by uart, must not be NULL
 * \param[in]	len	length of data to be sent, must > 0
 * \param[out]  cb  callback function after operation is done
 * \retval	>0	Byte count that was successfully sent for poll method
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	Parameter is not valid
 * \retval	E_SYS	Can't write data to hardware due to hardware issues, such as device is disabled
 */

/**
 * \fn		int32_t (*uart_read_udma)(void *data, uint32_t len, void *cb)
 * \details	receive \ref data of defined \ref len through uart(blocked). 
 *         max size: 4095 (if UART_x_DMA_LLI_SUPPORT is defined, max size is 1 MB)
 * \param[out]	data	pointer to data need to received by uart, must not be NULL
 * \param[in]	len	length of data to be received, must > 0
 * \param[out]  cb  callback function after operation is done
 * \retval	>0	Byte count that was successfully received for poll method
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	Parameter is not valid
 * \retval	E_SYS	Can't receive data from hardware due to hardware issues, such as device is disabled
 */
/** @} */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * \brief	get an \ref dev_uart "uart device" by uart device id.
     * 	For how to use uart device hal refer to \ref DEVICE_HAL_UART_DEVSTRUCT "Functions in uart device structure"
     * \param[in]	uart_id	id of uart, defined by user
     * \retval	!NULL	pointer to an \ref dev_uart "uart device structure"
     * \retval	NULL	failed to find the uart device by \ref uart_id
     * \note	need to implemented by user in user code.
     */
    extern DEV_UART_PTR hx_drv_uart_get_dev(USE_DW_UART_E uart_id);

    /**
     * @brief This function initializes a specified UART interface with a given base address.
     * 
     * @param uart_id The ID of the UART interface to be initialized. \ref USE_DW_UART_E
     * @param base_addr The base address of the UART controller. This is the memory-mapped address of the
     * UART controller hardware in the system.
     * @return int an error code of type Main Error Code Definitions
     */
    extern int hx_drv_uart_init(USE_DW_UART_E uart_id, uint32_t base_addr);

    /**
     * @brief This function deinitializes a specified UART interface.
     * 
     * @param uart_id The ID of the UART interface to be initialized. \ref USE_DW_UART_E
     * @return int an error code of type Main Error Code Definitions.
     */
    extern int hx_drv_uart_deinit(USE_DW_UART_E uart_id);
#ifdef __cplusplus
}
#endif

#endif /*INC_HX_DRV_UART_H_*/
