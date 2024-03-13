#ifndef _HX_DRV_DMAC_H
#define _HX_DRV_DMAC_H

#include "dev_common.h"
#include "WE2_device_addr.h"
/**
 * \defgroup	DMAC
 * \brief   Direct Memory Access Controler
 * \details	WE2 has four sets of DMAC, 
 *      DMAC0 & DMAC1 has 2 channels
 *      DMAC2 & DMAC3 has 8 channels
 * 
 * <pre>
 *      Sample code:
 *      Usage-1: memory-to-memory transfer with DMAC 0 channel 0 from SRAM0 to SRAM1 by default configure
 *      static DMAC_MEM_MEM_LLI_T mem_prerollingLLI[CLI_MEM_RAWLLI_NUM];;
 *      void transfer_cb()
 *      {
 *          cprintf("Transfer done\n");
 *      }
 * 
 *      uint32_t* src_addr = (uint32_t*)BASE_ADDR_SRAM0_ALIAS;
 *      uint32_t* dst_addr = (uint32_t*)BASE_ADDR_SRAM0_ALIAS;
 *      ///DMAC default: transfer width: 4 bytes/unit-->  total transfer bytes = number_transfer*4 = 80 bytes.
 *      uint32_t number_transfer = 20;  
 * 
 *      uint32_t channel = 0;
 *      uint32_t protect = (DMAC_PROTECT_USER|DMAC_PROTECT_NON_BUFFERABLE|DMAC_PROTECT_NON_CACHEABLE);
 * 
 *      DEV_DMAC_PTR dev_dmac;
 *      hx_drv_dmac_init(USE_HX_DMAC_0, DMAC0_BASE);
 *      dev_dmac = hx_drv_dmac_get_dev(USE_HX_DMAC_0);
 *      dev_dmac->dmac_open();
 * 
 *      dev_dmac->dmac_control(DMAC_CMD_SET_TRANSFER_CB, (void *)transfer_cb);
 *  	dev_dmac->dmac_memtransfer(channel, protect, number_transfer, src_addr, dst_addr);
 * 
 *      Usage 2: change DMAC 0 trnasfer configure to memory-to-memory transfer.
 *      ///Example conditions, src width: 1 byte, dst width: 1 byte
 *      ///                  src burst size: 8, dst burst size: 8
 * 
 *      DMAC_SRCDST_TRANSFER_CONFIG_T transfer_config;
 * 	    transfer_config.src_width = DMAC_WIDTH_8_BIT;
 *  	transfer_config.dst_width = DMAC_WIDTH_8_BIT;
 *  	transfer_config.src_burst_size = DMAC_BURST_8;
 *  	transfer_config.dst_burst_size = DMAC_BURST_8;
 *      dev_dmac->dmac_control(DMAC_CMD_SET_SRCDSTRANSFER_CONFIG, (void *)&transfer_config);
 * 
 *      uint32_t* src_addr = (uint32_t*)BASE_ADDR_SRAM0_ALIAS;
 *      uint32_t* dst_addr = (uint32_t*)BASE_ADDR_SRAM0_ALIAS;
 *      ///DMAC default: transfer width: 1 bytes/unit-->  total transfer bytes = number_transfer*1 = 20 bytes.
 *      uint32_t number_transfer = 20;  
 * 
 *      uint32_t channel = 0;
 *      uint32_t protect = (DMAC_PROTECT_USER|DMAC_PROTECT_NON_BUFFERABLE|DMAC_PROTECT_NON_CACHEABLE);
 * 
 *      DEV_DMAC_PTR dev_dmac;
 *      hx_drv_dmac_init(USE_HX_DMAC_0, DMAC0_BASE);
 *      dev_dmac = hx_drv_dmac_get_dev(USE_HX_DMAC_0);
 *      dev_dmac->dmac_open();
 * 
 *      dev_dmac->dmac_control(DMAC_CMD_SET_TRANSFER_CB, (void *)transfer_cb);
 * 
 *      //set pointer to prerolling LLI array 
 *      DMAC_MEM_TRANS_CONFIG_T mem_transfer_config;
 * 	    mem_transfer_config.channel = channel;
 * 		mem_transfer_config.prerolling_lli = mem_prerollingLLI;
 * 		mem_transfer_config.LLI_size = CLI_MEM_RAWLLI_NUM;
 * 		mem_transfer_config.LLI_level = CLI_MEM_RAWLLI_LVL;
 * 	    dev_dmac->dmac_control(DMAC_CMD_SET_MEM2MEM_RINGLLI, (void *)&mem_transfer_config);
 * 
 *  	dev_dmac->dmac_memtransfer(channel, protect, number_transfer, src_addr, dst_addr);
 * 
 *      Usage 3: link-list memory-to-memory with DMAC 0 channel 0 from SRAM0 to SRAM1
 *      ///Example conditions, Transfer 4 blocks of memory region data from SRAM 0 to SRAM 1
 *      ///                   Each block contains 16 bytes data.
 *      uint32_t num_lli = 4;
 *      uint32_t num_data = 16;
 *      uint32_t lli_src_addr[4],lli_dst_addr[4];
 *      uint32_t* src_addr = (uint32_t*)BASE_ADDR_SRAM0_ALIAS;
 *      uint32_t* dst_addr = (uint32_t*)BASE_ADDR_SRAM0_ALIAS;
 *      DMAC_MEM_MEM_LLI_T mem_lli[4];
 * 
 *  	for(int i = 0; i < num_lli; i++)
 *  	{
 * 		    lli_src_addr[i] = (uint32_t)((uint32_t)src_addr + (i * 0x1000));
 * 	    	lli_dst_addr[i] = (uint32_t)((uint32_t)dst_addr + (i * 0x1000));
 *  	}
 *  	for(int i = 0; i < num_lli; i++ )
 *      {
 *  		mem_lli[i].num_transfer = num_data;
 *  		mem_lli[i].src_addr  = (void *) lli_src_addr[i];
 *  		mem_lli[i].dst_addr  = (void *) lli_dst_addr[i];    
 *  		if(i == (num_lli-1))
 *  			mem_lli[i].next_lli = NULL;
 *  		else
 *  			mem_lli[i].next_lli     = &mem_lli[i+1];
 *  		/// Fill source block with data 
 *  		buffer_addr = (uint32_t *) mem_lli[i].src_addr;
 *  		for(int j=0 ; j < num_data ; j++)
 *  		{
 *  			*buffer_addr++ = (i+j); 
 *  		}
 *  		mem_lli++;
 *  	}
 *      DEV_DMAC_PTR dev_dmac;
 *      hx_drv_dmac_init(USE_HX_DMAC_0, DMAC0_BASE);
 *      dev_dmac = hx_drv_dmac_get_dev(USE_HX_DMAC_0);
 *      dev_dmac->dmac_open();
 *      uint32_t channel = 0;
 *      uint32_t protect = (DMAC_PROTECT_USER|DMAC_PROTECT_NON_BUFFERABLE|DMAC_PROTECT_NON_CACHEABLE);
 * `    ///Determines whether to jump callback function at each linked-list item.
 *      bool cb_peritem_flag = FALSE;
 *      dev_dmac->dmac_llim2mtransfer(channel, (CB_FUNC_T)transfer_cb, protect, num_lli, &mem_lli[0], cb_peritem_flag);
 * 
 *      Usage 4: memory-to-peripheral tansfer: transmit data from memory to UART1 FIFO
 *      ///Each TX and RX of peripheral has its own assigned DMA and DMA request numbers
 *      DEV_DMAC_PTR dev_dmac;
 *      uint32_t channel = 0;
 *      ///DMAC_PER_UART1_TX = 0x34 , \ref DMAC_DEVICE_E
 *      ///DMA used is DMA 3, DMA request number is 4
 *      uint32_t dev = (uint32_t)DMAC_PER_UART1_TX;
 * 	    uint32_t dma_id = ((dev>>(4)) & (0x0F));
 *  	uint32_t dmac_req = (dev & (0x0F));
 * 
 *      hx_drv_dmac_init(USE_HX_DMAC_3, DMAC3_BASE);
 *      dev_dmac = hx_drv_dmac_get_dev(USE_HX_DMAC_0);
 *      dev_dmac->dmac_open();
 *      dev_dmac->dmac_control(DMAC_CMD_SET_TRANSFER_CB, (void *)transfer_cb);
 * 
 *      uint8_t* mem_addr = (uint8_t*)BASE_ADDR_SRAM0_ALIAS;
 *      uint32_t num_data = 9; 
 *      for(int i = 0; i<num_data; ++i)
 *      {
 *          mem_addr[i] = (0x30+i);
 *      }
 * 
 *      ///set DMA transfer configure of source memory
 *      ///In Mem-to-Peri, src_width & src_burst_size are importanted, while dst_width and dst_burst_size can be ignored.
 *      DMAC_SRCDST_TRANSFER_CONFIG_T transfer_config;
 * 	    transfer_config.src_width = DMAC_WIDTH_8_BIT;
 *  	transfer_config.dst_width = DMAC_WIDTH_8_BIT;
 *  	transfer_config.src_burst_size = DMAC_BURST_1;
 *  	transfer_config.dst_burst_size = DMAC_BURST_1;
 *      dev_dmac->dmac_control(DMAC_CMD_SET_SRCDSTRANSFER_CONFIG, (void *)&transfer_config);
 * 
 *      ///set DMA transfer configure of UART 1 TX
 *      DMAC_PERI_TRANSFER_T peri_config;
 *      ///UART TX FIFO offset: register address + 0x0
 *      peri_config.dev_address = UART_1_BASE;  
 *    	peri_config.dev = dmac_req; 
 *  	peri_config.terminated_cb = (CB_FUNC_T) transfer_cb;
 *  	peri_config.burst_size = DMAC_BURST_1; 
 *  	peri_config.width = DMAC_WIDTH_8_BIT;
 *  	peri_config.sync = DMA_SYNC_EN;
 *  	peri_config.protect = DMAC_PROTECT_USER;
 *  	peri_config.ch_config_but_disable_ch = 0;
 *  	peri_config.using_ring_buffer = 0; ///<disable ring buffer
 *      dev_dmac->dmac_control(DMAC_CMD_SET_PERIPHERAL_CONFIG, (void *)&peri_config);
 * 
 *      dev_dmac->dmac_peritransfer(channel, dmac_req, num_data, DMAC_MEM_TO_PERIPHERAL_DMA_CTRL, (void*)mem_addr);
 * 
 *      Usage 5: peripheral-to-memory tansfer: transmit data from UART1 FIFO to SRAM0
 *      ///Each TX and RX of peripheral has its own assigned DMA and DMA request numbers
 *      DEV_DMAC_PTR dev_dmac;
 *      uint32_t channel = 0;
 *      ///DMAC_PER_UART1_RX = 0x35 , \ref DMAC_DEVICE_E
 *      ///DMA used is DMA 3, DMA request number is 5
 *      uint32_t dev = (uint32_t)DMAC_PER_UART1_RX;
 * 	    uint32_t dma_id = ((dev>>(4)) & (0x0F));
 *  	uint32_t dmac_req = (dev & (0x0F));
 * 
 *      hx_drv_dmac_init(USE_HX_DMAC_3, DMAC3_BASE);
 *      dev_dmac = hx_drv_dmac_get_dev(USE_HX_DMAC_0);
 *      dev_dmac->dmac_open();
 *      dev_dmac->dmac_control(DMAC_CMD_SET_TRANSFER_CB, (void *)transfer_cb);
 * 
 *      uint8_t* mem_addr = (uint8_t*)BASE_ADDR_SRAM0_ALIAS;
 *      uint32_t num_data = 9; 
 *      for(int i = 0; i<num_data; ++i)
 *      {
 *          mem_addr[i] = (0x30+i);
 *      }
 * 
 *      ///set DMA transfer configure of source memory
 *      ///In Peri-to-Mem, src_width & src_burst_size can be ignored, while dst_width and dst_burst_size are importanted.
 *      DMAC_SRCDST_TRANSFER_CONFIG_T transfer_config;
 * 	    transfer_config.src_width = DMAC_WIDTH_8_BIT;
 *  	transfer_config.dst_width = DMAC_WIDTH_8_BIT;
 *  	transfer_config.src_burst_size = DMAC_BURST_1;
 *  	transfer_config.dst_burst_size = DMAC_BURST_1;
 *      dev_dmac->dmac_control(DMAC_CMD_SET_SRCDSTRANSFER_CONFIG, (void *)&transfer_config);
 * 
 *      ///set DMA transfer configure of UART 1 RX
 *      DMAC_PERI_TRANSFER_T peri_config;
 *      ///UART RX FIFO offset: register address + 0x0
 *      peri_config.dev_address = UART_1_BASE;  
 *    	peri_config.dev = dmac_req; 
 *  	peri_config.terminated_cb = (CB_FUNC_T) transfer_cb;
 *  	peri_config.burst_size = DMAC_BURST_1; 
 *  	peri_config.width = DMAC_WIDTH_8_BIT;
 *  	peri_config.sync = DMA_SYNC_EN;
 *  	peri_config.protect = DMAC_PROTECT_USER;
 *  	peri_config.ch_config_but_disable_ch = 0;
 *  	peri_config.using_ring_buffer = 0; ///<disable ring buffer
 *      dev_dmac->dmac_control(DMAC_CMD_SET_PERIPHERAL_CONFIG, (void *)&peri_config);
 * 
 *      dev_dmac->dmac_peritransfer(channel, dmac_req, num_data, DMAC_PERIPHERAL_TO_MEM_DMA_CTRL, (void*)mem_addr);
 * 
 *      Usage 6: prerolling memory-to-peripheral tansfer : transmit data from memory to UART1 FIFO
 * 
 *      static DMAC_MEM_PERI_LLI_T peri_prerollingLLI[2];
 *      DMAC_DEVICE_E dmac_req;
 *      DEV_DMAC_PTR dev_dmac;
 *      uint32_t UART1_TX_req = 0x34;
 *      dmac_req = (UART1_TX_req & (0x0F));
 * 
 *      hx_drv_dmac_init(USE_HX_DMAC_3, DMAC3_BASE);
 *      dev_dmac = hx_drv_dmac_get_dev(USE_HX_DMAC_0);
 *      dev_dmac->dmac_open();
 *      dev_dmac->dmac_control(DMAC_CMD_SET_TRANSFER_CB, (void *)transfer_cb);
 * 
 *      DMAC_PERI_TRANS_LLI_CONFIG_T peri_ll_config;
 *      peri_ll_config.dev = dmac_req;
 *      peri_ll_config.prerolling_lli = peri_prerollingLLI;
 *      peri_ll_config.LLI_level = 1;
 *      peri_ll_config.LLI_size = 2;
 *      dev_dmac->dmac_control(DMAC_CMD_SET_MEM2PERI_RINGLLI, (void *)&peri_ll_config);
 * 
 *      ///set DMA transfer configure of UART 1 TX
 *      DMAC_PERI_TRANSFER_T peri_config;
 *      ///UART TX FIFO offset: register address + 0x0
 *      peri_config.dev_address = UART_1_BASE;  
 *    	peri_config.dev = dmac_req; 
 *  	peri_config.terminated_cb = (CB_FUNC_T) transfer_cb;
 *  	peri_config.burst_size = DMAC_BURST_1; 
 *  	peri_config.width = DMAC_WIDTH_8_BIT;
 *  	peri_config.sync = DMA_SYNC_EN;
 *  	peri_config.protect = DMAC_PROTECT_USER;
 *  	peri_config.ch_config_but_disable_ch = 0;
 *  	peri_config.using_ring_buffer = 0; ///<disable ring buffer
 *      dev_dmac->dmac_control(DMAC_CMD_SET_PERIPHERAL_CONFIG, (void *)&peri_config);
 * 
 *      ///set DMA transfer configure of source memory
 *      ///In Peri-to-Mem, src_width & src_burst_size can be ignored, while dst_width and dst_burst_size are importanted.
 *      DMAC_SRCDST_TRANSFER_CONFIG_T transfer_config;
 * 	    transfer_config.src_width = DMAC_WIDTH_8_BIT;
 *  	transfer_config.dst_width = DMAC_WIDTH_8_BIT;
 *  	transfer_config.src_burst_size = DMAC_BURST_1;
 *  	transfer_config.dst_burst_size = DMAC_BURST_1;
 *      dev_dmac->dmac_control(DMAC_CMD_SET_SRCDSTRANSFER_CONFIG, (void *)&transfer_config);
 * 
 *      uint32_t mem_addr = 0x34000000; //SRAM0
 *      uint32_t num_transfer = 8196;
 *      dev_dmac->dmac_peritransfer_prerolling(channel, dmac_req, DMAC_MEM_TO_PERIPHERAL_DMA_CTRL,
 *  				(UINT32_T *)mem_addr, num_transfer, NULL, 0);
 * 
 * </pre>
 * @{
 */


#define MEM_TO_MEM_RELATED
#define MEM_TO_PERI_RELATED

#if defined(BOOT_USED)
#define DMA_BL_USE
#endif
#if defined(BOOTROM) || defined(BOOTLOADER)
#undef MEM_TO_MEM_RELATED
#endif

typedef void (*CB_FUNC_T)(void);

#define DMAC_NUM (4)

#define DMAC_0_ID 0
#define DMAC_1_ID 1
#define DMAC_2_ID 2
#define DMAC_3_ID 3
#define DMAC_all_ID DMAC_NUM

/**
 * \name	DMA Object Control Macros
 * if not use then not define
 * @{
 */
#if defined(IP_INST_DMA0) || defined(IP_INST_NS_DMA0)
#define USE_DMA_0
#endif
#if defined(IP_INST_DMA1) || defined(IP_INST_NS_DMA1)
#define USE_DMA_1
#endif
#if defined(IP_INST_DMA2) || defined(IP_INST_NS_DMA2)
#define USE_DMA_2
#endif
#if defined(IP_INST_DMA3) || defined(IP_INST_NS_DMA3)
#ifndef DMA_BL_USE
#define USE_DMA_3
#endif
#endif
/** @} end of name */

/**
 * \brief The number of DMAC objcet channel.
 * 
 */
#define DMAC_0_CHANNEL_NUM (2)
#define DMAC_1_CHANNEL_NUM (2)
#define DMAC_2_CHANNEL_NUM (8)
#define DMAC_3_CHANNEL_NUM (8)

/**
 * \enum USE_HX_DMAC_S
 * \brief DMAC Object ID
 */
typedef enum 
{
    USE_HX_DMAC_0 = DMAC_0_ID,
    USE_HX_DMAC_1 = DMAC_1_ID,
    USE_HX_DMAC_2 = DMAC_2_ID,
    USE_HX_DMAC_3 = DMAC_3_ID,
    USE_HX_DMAC_ALL = DMAC_all_ID,
} USE_HX_DMAC_E;

/**
 * \enum HX_DMAC_REGISTER_ADDR_E
 * \brief HX DMAC REGISTER BASE ADDRESS
 */
typedef enum 
{
    HX_DMAC_0_REG_ADDR = DMAC0_BASE,
    HX_DMAC_1_REG_ADDR = DMAC1_BASE,
    HX_DMAC_2_REG_ADDR = DMAC2_BASE,
    HX_DMAC_3_REG_ADDR = DMAC3_BASE,
} HX_DMAC_REGISTER_ADDR_E;
/**
 * \brief DMA reqest list
 * [1]: DMA ID
 * [0]: DMA request number
 * 
 * for example:
 * DMAC_PER_UART1_TX = 0x34
 *  3: DMA 3
 *  4: DMA reguest number
 */
typedef enum DMAC_DEVICE
{
    DMAC_PER_I3C_HC = 0x00,
    DMAC_PER_QSPI_TX = 0x10,
    DMAC_PER_QSPI_RX = 0x11,
    DMAC_PER_OSPI_TX = 0x12,
    DMAC_PER_OSPI_RX = 0x13,
    DMAC_PER_PDM_RX = 0x20,
    DMAC_PER_HIMAX_PDM_RX = 0x21,
    DMAC_PER_I2S_HOST_TX = 0x22,
    DMAC_PER_I2S_HOST_RX = 0x23,
    DMAC_PER_I2S_SLAVE_TX = 0x24,
    DMAC_PER_I2S_SLAVE_RX = 0x25,
    DMAC_PER_SSPI_HOST_TX = 0x26,
    DMAC_PER_SSPI_HOST_RX = 0x27,
    DMAC_PER_SSPI_SLAVE_TX = 0x28,
    DMAC_PER_SSPI_SLAVE_RX = 0x29,
    DMAC_PER_I2C_SLAVE_TX = 0x30,
    DMAC_PER_I2C_SLAVE_RX = 0x31,
    DMAC_PER_UART0_TX = 0x32,
    DMAC_PER_UART0_RX = 0x33,
    DMAC_PER_UART1_TX = 0x34,
    DMAC_PER_UART1_RX = 0x35,
    DMAC_PER_I2C_MASTER_TX = 0x36,
    DMAC_PER_I2C_MASTER_RX = 0x37,
    DMAC_PER_UART2_TX = 0x38,
    DMAC_PER_UART2_RX = 0x39,
    DMAC_PER_I2C_MASTER_SENSOR_TX = 0x3A,
    DMAC_PER_I2C_MASTER_SENSOR_RX = 0x3B,
    DMAC_PER_I2C_SLAVE1_TX = 0x3C,
    DMAC_PER_I2C_SLAVE1_RX = 0x3D
} DMAC_DEVICE_E;

#define DMA_DEVICE_REQ_NUM_MASK (0xf)
#define DMA_DEVICE_DMA_SEL_MASK (0xf0)

/**
 * @enum DMAC_FLOW_CTRL_E
 * @brief DMA transfer flow controller type
 */
typedef enum DMAC_FLOW_CTRL
{
    DMAC_MEM_TO_MEM_DMA_CTRL = 0,
    DMAC_MEM_TO_PERIPHERAL_DMA_CTRL = 1,
    DMAC_PERIPHERAL_TO_MEM_DMA_CTRL = 2,
    DMAC_PERIPHERAL_TO_PERIPHERAL_DMA_CTRL = 3, //no support
    DMAC_PERIPHERAL_TO_PERIPHERAL_DEST_PERIPHERAL_CTRL = 4,
    DMAC_MEM_TO_PERIPHERAL_PERIPHERAL_CTRL = 5,
    DMAC_PERIPHERAL_TO_MEM_PERIPHERAL_CTRL = 6,
    DMAC_PERIPHERAL_TO_PERIPHERAL_SRC_PERIPHERAL_CTRL = 7  //no support

} DMAC_FLOW_CTRL_E;

/**
 * \enum  DMAC_BURST_SIZE_E
 * \brief burst transfer request size
 */
typedef enum DMAC_BURST_SIZE
{
    DMAC_BURST_1 = 0,   /**< 1 transfer per burst */
    DMAC_BURST_4 = 1,   /**< 4 transfers per burst */
    DMAC_BURST_8 = 2,   /**< 8 transfers per burst */
    DMAC_BURST_16 = 3,  /**< 16 transfers per burst */
    DMAC_BURST_32 = 4,  /**< 32 transfers per burst */
    DMAC_BURST_64 = 5,  /**< 64 transfers per burst */
    DMAC_BURST_128 = 6, /**< 128 transfers per burst */
    DMAC_BURST_256 = 7  /**< 256 transfers per burst */

} DMAC_BURST_SIZE_E;

/**
 * \enum  DMAC_WIDTH_E
 * \brief DMA transfer width
 */
typedef enum DMAC_WIDTH
{
    DMAC_WIDTH_8_BIT = 0,  /**< 8 Bits per transfer */
    DMAC_WIDTH_16_BIT = 1, /**< 16 Bits per transfer */
    DMAC_WIDTH_32_BIT = 2  /**< 32 Bits per transfer */
} DMAC_WIDTH_E;


/**
 * \enum  DMAC_WIDTH_E
 * \brief DMA protection and access information configuration
 */
typedef enum DMAC_PROTECT_BITS
{
    DMAC_PROTECT_USER = 0x00,  /**< User access */
    DMAC_PROTECT_SUPER = 0x01, /**< Supervisor access */

    DMAC_PROTECT_NON_BUFFERABLE = 0x00, /**< Non bufferable data */
    DMAC_PROTECT_BUFFERABLE = 0x02,     /**< Bufferable data */

    DMAC_PROTECT_NON_CACHEABLE = 0x00, /**< Non cacheable data */
    DMAC_PROTECT_CACHEABLE = 0x04,     /**< Cacheable data */

    DMAC_PROTECT_VALID = (DMAC_PROTECT_CACHEABLE | DMAC_PROTECT_BUFFERABLE | DMAC_PROTECT_SUPER)
} DMAC_PROTECT_BITS_E;

/**
 * \enum  DMAC_ERROR_E
 * \brief DMAC Error status
 */
typedef enum DMAC_ERROR
{
    DMAC_ERR_NONE = 0,
    DMAC_ERR_BAD_PARAMETER,
    DMAC_ERR_BUSY,
    DMAC_ERR_CTRLR_NOT_INITIALISED,
    DMAC_ERR_PERIPHERAL_NOT_INITIALISED,
    DMAC_ERR_TRANSFER_ERROR,
    DMAC_ERR_TRANSFER_ABORTED,
    DMAC_ERR_TRANSFER_LLI_COMPLETE,
    DMAC_ERR_TRANSFER_COMPLETE,
    DMAC_ERR_CTRLR_ACTIVE,
    DMAC_ERR_CTRLR_DISABLED,
    
} DMAC_ERROR_E;

/**
 * \enum  DMAC_CHANNEL_ID_E
 * \brief DMAC device channel ID
 * 
 * \note: no all DMA has 8 channels.
 */
typedef enum DMAC_CHANNEL_ID
{
    DMAC_CHANNEL0 = 0,
    DMAC_CHANNEL1 = 1,
    DMAC_CHANNEL2 = 2,
    DMAC_CHANNEL3 = 3,
    DMAC_CHANNEL4 = 4,
    DMAC_CHANNEL5 = 5,
    DMAC_CHANNEL6 = 6,
    DMAC_CHANNEL7 = 7
} DMAC_CHANNEL_ID_E;

/**
 * \enum  DMAC_SYNC_E
 * \brief DMA sync flag
 */
typedef enum DMAC_SYNC
{
    DMA_SYNC_EN = 0,
    DMA_SYNC_DIS = 1
} DMAC_SYNC_E;

/**
 * \enum  DMAC_ENDIAN_E
 * \brief DMA endian config
 */
typedef enum DMAC_ENDIAN
{
    DMAC_LILLLE_ENDIAN = 0,
    DMAC_BIG_ENDIAN = 1
} DMAC_ENDIAN_E;

/** 
 * \struct DMAC_AHB_ENDIAN_S
 * \brief DMA AHB Master 2 endianness configuration:
 * ahb: 0 or 1
 * endian: DMAC_LILLLE_ENDIAN or DMAC_BIG_ENDIAN
 * 
 * \note:
 * DMA0/1: only 1 ahb port
 * DMA2/3: 2 ahb port.
 */
typedef struct DMAC_AHB_ENDIAN_S
{
    uint32_t ahb; 
    DMAC_ENDIAN_E endian;
}  DMAC_AHB_ENDIAN_T;

/**
 * \struct  DMAC_RAWLLI_T
 * \brief DMAC raw transfer configure
 */
typedef struct DMAC_RAWLLI_S
{
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t next_lli;
    uint32_t transfer_ctrl;

} DMAC_RAWLLI_T;

/**
 * \struct DMAC_MEM_MEM_LLI_T
 * \brief DMA Link list Data Structure for  Memory to Memory
 */
typedef struct DMAC_MEM_MEM_LLI_S
{
    DMAC_RAWLLI_T raw_lli;

    uint32_t num_transfer;               /**< number unit of transfers to perform */
    void *src_addr;                      /**< Source address */
    void *dst_addr;                      /**< Destination address */
    struct DMAC_MEM_MEM_LLI_S *next_lli; /**< Ptr to next LLI or NULL if this is the last */

} DMAC_MEM_MEM_LLI_T;

/**
 * \struct DMAC_MEM_PERI_LLI_T
 * \brief DMA Link list Data Structure for  Memory to Peripheral/Peripheral to Memory
 */
typedef struct DMAC_MEM_PERI_LLI_S
{
    DMAC_RAWLLI_T raw_lli;

    uint32_t num_transfer;                /**< number unit of transfers to perform */
    void *mem_addr;                       /**< memory address */
    struct DMAC_MEM_PERI_LLI_S *next_lli; /**< Ptr to next LLI or aNULL if this is the last */

} DMAC_MEM_PERI_LLI_T;

/**
 * \struct DMAC_SRCDST_TRANSFER_CONFIG_T
 * \brief Set DMAC soucer & destination transfer configure
 */
typedef struct DMAC_SRCDST_TRANSFER_CONFIG_S
{
    DMAC_WIDTH_E src_width;             /**< Source transfer width type*/
    DMAC_BURST_SIZE_E src_burst_size;   /**< Source burst size type*/

    DMAC_WIDTH_E dst_width;             /**< Destination transfer width type*/
    DMAC_BURST_SIZE_E dst_burst_size;   /**< Destination burst size type*/

} DMAC_SRCDST_TRANSFER_CONFIG_T;

/**
 * \struct DMAC_CHANNEL_INTERRUPT_MASK_T
 * \brief Set interrput mask of specific channel in DMAC
 */
typedef struct DMAC_CHANNEL_INTERRUPT_MASK_S
{
    DMAC_CHANNEL_ID_E channel;   /**< DMAC channel ID */
    uint32_t int_tc_mask;        /**< 0:mask, 1:non-mask */
    uint32_t int_err_mask;       /**< 0:mask, 1:non-mask */
} DMAC_CHANNEL_INTERRUPT_MASK_T;

/**
 * \struct DMAC_PERI_TRANSFER_T
 * \brief Set peripheral transfer configure
 * 
 * \note:
 * sync: synchronization logic
 *  >sync = DMA_SYNC_EN when the peripheral generating the DMA request runs on a different clock to the DMAC.
 *  >sync = DMA_SYNC_DIS to improve the DMA request response time (only when peripherals running on the same clock as the DMAC)
 */
typedef struct DMAC_PERI_TRANSFER_S
{
    uint32_t dev;                       /**< DMA request number */
    uint32_t dev_address;               /**< peripheral TX/RX FIFO address */
    CB_FUNC_T terminated_cb;            /**< callback function wheh transfer finished for Mem-to-Peri/Peri-to-Mem */
    DMAC_WIDTH_E width;                 /**< transfer width for DMA-to-Peri */
    DMAC_BURST_SIZE_E burst_size;       /**< transfer burst size for DMA-to-Peri */
    DMAC_SYNC_E sync;                   /**< synchronization logic for DMA request signals enabled or disabled */
    uint32_t protect;                   /**< protection and access configuration type */
    uint32_t ch_config_but_disable_ch;  /**< only set 1 for iic slave, otherwire set 0 */
    uint32_t using_ring_buffer;         /**< set 1 when enable ring buffer*/
} DMAC_PERI_TRANSFER_T;

/**
 * \struct DMAC_DEV_TRANSFER_T
 * \brief Set register address & callback function for Peripheral Transfer Configuration
 */
typedef struct DMAC_DEV_TRANSFER_S
{
    uint32_t dev;               /**< DMA request number */
    uint32_t dev_address;       /**< peripheral TX/RX FIFO address */
    CB_FUNC_T terminated_cb;     /**< callback function wheh transfer finished for Mem-to-Peri/Peri-to-Mem */
} DMAC_DEV_TRANSFER_T;

/**
 * \struct DMAC_TRANSFER_CONFIG_T
 * \brief Set transfer configure for Mem-to-Peri/Peri-to-Mem
 */
typedef struct DMAC_TRANSFER_CONFIG_S
{
    uint32_t dev;                   /**< DMA request number */

    DMAC_WIDTH_E width;             /**< transfer width for DMA-to-Peri */
    DMAC_BURST_SIZE_E burst_size;   /**< transfer burst size for DMA-to-Peri */
    DMAC_SYNC_E sync;               /**< synchronization logic for DMA request signals enabled or disabled */
    uint32_t protect;               /**< protection and access configuration type */
} DMAC_TRANSFER_CONFIG_T;

typedef struct DMAC_MEM_TRANS_CONFIG_S
{
    DMAC_CHANNEL_ID_E channel;
    DMAC_MEM_MEM_LLI_T *prerolling_lli;
    uint32_t LLI_size;
    uint32_t LLI_level;
} DMAC_MEM_TRANS_CONFIG_T;

typedef struct DMAC_PERI_TRANS_LLI_CONFIG_S
{
    uint32_t dev;  
    DMAC_MEM_PERI_LLI_T *prerolling_lli;  
    uint32_t LLI_size;    
    uint32_t LLI_level;
} DMAC_PERI_TRANS_LLI_CONFIG_T;

typedef void* DMAC_CTRL_PARAM;

/**
 * enable/disable DMA
 * \param type : uint32_t
 * \param usage : 0 or 1
 * \return value explanation :
 */
#define DMAC_CMD_SET_CONTROLLER DEV_SET_SYSCMD(0)

/**
 * enable/disable print Terminal Count Status & Error Status when transfer done
 * \param type : uint32_t
 * \param usage : 0 or 1
 * \return value explanation :
 */
#define DMAC_CMD_SET_DISPALY DEV_SET_SYSCMD(1)

/**
 * Set peripheral address and callback function
 * \param type : DMAC_DEV_TRANSFER_T*
 * \param usage :
 * \return value explanation :
 */
#define DMAC_CMD_SET_PERIPHERAL_ADDR DEV_SET_SYSCMD(2)

/** burst size & width of peripheral transfer phase
 * \param type : DMAC_TRANSFER_CONFIG_T*
 * \param usage :
 * \return value explanation :
 */
#define DMAC_CMD_SET_DMAC_CONFIG DEV_SET_SYSCMD(3)

/**
 * Set \ref dmac_info::cb "DMAC transmit success callback" function
 * when all required bytes are transmitted for interrupt method
 * \param type : \ref DEV_CALLBACK * or NULL
 * \param usage : transmit success callback function for dma
 * \return value explanation :
 */
#define DMAC_CMD_SET_TRANSFER_CB DEV_SET_SYSCMD(4)

/**
 * Set \ref dmac_info::cb "DMAC transmit error callback" function
 * \param type : \ref DEV_CALLBACK * or NULL
 * \param usage : transmit success callback function for dma
 * \return value explanation :
 */
#define DMAC_CMD_SET_ERROR_CB DEV_SET_SYSCMD(5)

/**
 * Set \ref dmac_ctrl::DMAC_PERIPHERIAL_T " peripherial configuration
 * \param type : \ref DMAC_PERI_TRANSFER_T
 * \param usage : peripherial configuration of dma
 * \return value explanation :
 */
#define DMAC_CMD_SET_PERIPHERAL_CONFIG DEV_SET_SYSCMD(6)

/**
 * Select DMA big/little endian
 * \param type : uint32_t
 * \param usage : 0:little-endian 1:big-endian
 * \return value explanation :
 */
#define DMAC_CMD_SET_ENDIAN_CONFIG DEV_SET_SYSCMD(7)

/**
 * Select DMA big/little endian
 * \param type : uint32_t
 * \param usage : 0:little-endian 1:big-endian
 * \return value explanation :
 */
#define DMAC_CMD_SET_SRCDSTRANSFER_CONFIG DEV_SET_SYSCMD(8)

/**
 * Set burst size & width of memory transfer phase
 * \param type : \ref DMAC_SRCDST_TRANSFER_CONFIG_T*
 * \param usage :
 * \return value explanation :
 */
#define DMAC_CMD_SWITCH_DEFAULT_TRANSFER DEV_SET_SYSCMD(9)

/**
 * Set incrementing or non-incrementing addressing for source and destination
 * Only vaild for memory transfer port.
 * 0: address auto increment
 * 1: address non-increment
 * \param type : uint32_t
 * \param usage :
 * \return value explanation :
 */
#define DMAC_CMD_SET_ADDR_INCREMENT_FLAGE DEV_SET_SYSCMD(10)

/**
 * set channel enable bit
 * \param type : uint32_t
 * \param usage : should < DMA max channel number
 * \return value explanation :
 */
#define DMAC_CMD_SET_CHANNEL_ENABLE DEV_SET_SYSCMD(11)

/**
 * Get peripheral identification
 * \param type : uint32_t*
 * \param usage : should < DMA max channel number
 * \return value explanation :
 * Configuration[31:24]
 * Revision[23:20]
 * Designer ID[19:12]
 * PartNumber[11:0]
 */
#define DMAC_CMD_GET_PERIPHI_ID DEV_SET_SYSCMD(12)

/**
 * Get PrimeCell Identification
 * \param type : uint32_t*
 * \param usage : should < DMA max channel number
 * \return value explanation :
 * DMACPCellID3[31:24]
 * DMACPCellID2[23:20]
 * DMACPCellID1[19:12]
 * DMACPCellID0[11:0]
 */
#define DMAC_CMD_GET_PCELL_ID DEV_SET_SYSCMD(13)

/**
 * Set DMA channel interrupt mask.
 * \param type : DMAC_CHANNEL_INTERRUPT_MASK_T*
 * \param usage :
 * \return value explanation :
 */
#define DMAC_CMD_SET_CHANNEL_INT_MASK DEV_SET_SYSCMD(14)

/**
 * Get flag which DMA channel are enabled
 * \param type : uitn32_t*
 * \param usage :
 * \return value explanation :
 */
#define DMAC_CMD_GET_CHANNEL_STATUS DEV_SET_SYSCMD(15)

/**
 * Get DMA register address
 * \param type : uitn32_t*
 * \param usage :
 * \return value explanation :
 */
#define DMAC_CMD_GET_DMAC_BASE_ADDR DEV_SET_SYSCMD(16)

/**
 * Set DMA channel mode to select auto-polling or manual mode
 * \param type : uitn32_t*
 * \param usage : should 0 or 1
 * \return value explanation :
 * \note: if no channel can be use, dma transfer function will retrun DMAC_ERROR_E::DMAC_ERR_BUSY
 */
#define DMAC_CMD_SET_AUTO_POLLING DEV_SET_SYSCMD(17)

/**
 * Set big/littel endian for AHB port. only DMA2/3 is supported.
 * \param type : \ref DMAC_AHB_ENDIAN_T * 
 * \param usage : 
 * \return value explanation :
 */
#define DMAC_CMD_SET_AHB_ENDIAN DEV_SET_SYSCMD(18)

//#if defined(HX_LIB_SPIM_DMA_POLL_STATUS_NO_IRQ)
//YX 20230222 added. Polling DMA RX/TX and SPIM TX status without DMA and SPI IRQ
/**
 * Get DMA Channel Interrupt Status
 * \param type : uitn32_t*
 * \param usage :
 * \return value explanation :
 */
#define DMAC_CMD_GET_CHANNEL_INT_STATUS      DEV_SET_SYSCMD(19)

/**
 * Get DMA Channel Interrupt Error Status
 * \param type : uitn32_t*
 * \param usage :
 * \return value explanation :
 */
#define DMAC_CMD_GET_CHANNEL_INT_ERR_STATUS  DEV_SET_SYSCMD(20)

/**
 * Set DMA Transfer Stop
 * \param type : uint32_t
 * \param usage : should < DMA max channel number
 * \return value explanation :
 * \note  vaild for HX_LIB_SPIM_DMA_POLL_STATUS_NO_IRQ is defined
 */
#define DMAC_CMD_STOP_TRANSFER               DEV_SET_SYSCMD(21)

/**
 * Set DMA IRQ Enable
 * \param type : uint32_t
 * \param usage : 0:Disable 1:Enable
 * \return value explanation :
 * \note  vaild for HX_LIB_SPIM_DMA_POLL_STATUS_NO_IRQ is defined
 */
#define DMAC_CMD_ENA_IRQ                     DEV_SET_SYSCMD(22)

/**
 * Set pointer to circular linked list for prerolling mem-to-mem transfer 
 * \param type : DMAC_MEM_TRANS_CONFIG_T
 * \return value explanation :
 */
#define DMAC_CMD_SET_MEM2MEM_RINGLLI         DEV_SET_SYSCMD(23)

/**
 * Set pointer to circular linked list for prerolling mem-to-peri transfer
 * \param type : DMAC_PERI_TRANS_LLI_CONFIG_T
 * \return value explanation :
 */
#define DMAC_CMD_SET_MEM2PERI_RINGLLI        DEV_SET_SYSCMD(24)
//#endif

/**
 * \brief	DMAC information struct definition
 * \details	informations about DMAC.
 */
typedef struct dev_dmac_info
{
    void *dmac_ctrl;            /**< DMAC control related pointer, and this should be set during DMA object implementation, refer to DMAC_CONTROLLER_T */
    uint32_t id;                /**< dmac id */
    uint32_t opn_cnt;           /**<dmac open count, open it will increase 1, close it will decrease 1, 0 for close, >0 for open*/
    uint32_t status;            /**< no used variable */
    uint32_t channel_mode;      /**< 0:manual mode, 1: auto-polling mode */
    CB_FUNC_T cb;               /**< callback function when transfer done*/
    CB_FUNC_T error_cb;         /**< callback function when transfer error*/
} DEV_DMAC_INFO, *DEV_DMAC_INFO_PTR;

/**
 * \brief	DMA controller interface definition
 * \details	Define MA controller interface, like dma information structure,
 * 	provide functions to open/close/control dmac, transfer data by dmac
 * \note	All this details are implemented by user in user porting code
 */
typedef struct dev_dmac
{
    /**< DMAC device information*/
    DEV_DMAC_INFO dmac_info;    

    /**
     * open DMA device 
     */
    DMAC_ERROR_E (*dmac_open)(void); 
    
    /**
     * close DMA device
     */
    DMAC_ERROR_E (*dmac_close)(void);
    /**
     * control DMA device
     * 
     * \param[in] ctrl_cmd  \ref DEV_SET_SYSCMD "control command", to change or get some thing related to DMA
     * \param[in, out] param parameters that maybe argument of the command,
     * 					or return values of the command, must not be NULL
     */
    DMAC_ERROR_E (*dmac_control)(uint32_t ctrl_cmd, void *param); 

    /**
     * halt assigned channel transfer, and get the number of untransmitted data
     * 
     * \param[in] channel specific DMA channel ID
     * \param[out] count Pointer to the number of untransmitted data be stored
     */
    DMAC_ERROR_E (*dmac_halttransfer)(DMAC_CHANNEL_ID_E channel, uint32_t *count); 

    /**
     * transfer data for mem-to-mem transfer
     * \note  Before using this function, using Contrl commad "DMAC_CMD_SET_MEM2MEM_RINGLLI" to set circular linked list.
     * 
     * \param[in] channel specific DMA channel ID
     * \param[in] channel_protect transfer information for protection and access
     * \param[in] length the number of transfer unit, max value: 265980
     * \param[in] src_addr Pointer to the source data buffer to be transferred
     * \param[out] dst_addr Pointer to the target data buffer where the transmitted data from source data buffer
     * 
     * if transfer width is word, MAX 1MB of data can be transmitted at a time.
     * 265980(unit)*4(bytes) > 1024*1024 bytes
     */
    DMAC_ERROR_E (*dmac_memtransfer)
    (DMAC_CHANNEL_ID_E channel, DMAC_PROTECT_BITS_E channel_protect, uint32_t length, void *src_addr, void *dst_addr); 

    /**
     * linked-list transfer data for mem-to-mem transfer
     * \param[in] channel specific DMA channel ID
     * \param[in] channel_protect transfer information for protection and access
     * \param[in] num_lli the lenght of linked_list table
     * \param[in] lli Pointer is the starting address of the linked list
     * \param[in] cb_peritem Flag to determine whether to jump callback every transfer linked-list node is done
     */
    DMAC_ERROR_E (*dmac_llim2mtransfer)
    (DMAC_CHANNEL_ID_E channel, void *cb, DMAC_PROTECT_BITS_E channel_protect, uint32_t num_lli,
    DMAC_MEM_MEM_LLI_T *lli, bool cb_peritem); 

    /**
     * transfer data for mem-to-peri transfer single time
     * \param[in] channel specific DMA channel ID
     * \param[in] dev DMA request number
     * \param[in] num_transfer the number of transfer unit, max num_transfer is 4095
     * \param[in] flow Flow control and transfer type
     * \param[in, out] mem_addr point to memory address to receive/transmit data with peripheral.
     */
    DMAC_ERROR_E (*dmac_peritransfer)
    (DMAC_CHANNEL_ID_E channel, DMAC_DEVICE_E dev, uint32_t num_transfer, DMAC_FLOW_CTRL_E flow, void *mem_addr); 

    /**
     * linked-list transfer data for mem-to-peri transfer
     * \param[in] channel specific DMA channel ID
     * \param[in] dev DMA request number
     * \param[in] flow Flow control and transfer type
     * \param[in] num_lli the lenght of linked_list table
     * \param[in] lli_item Pointer is the starting address of the linked list
     * \param[in] cb_peritem Flag to determine whether to jump callback every transfer linked-list node is done
     */
    DMAC_ERROR_E (*dmac_lliperitransfer)
    (DMAC_CHANNEL_ID_E channel, DMAC_DEVICE_E dev, DMAC_FLOW_CTRL_E flow, uint32_t num_lli,
     DMAC_MEM_PERI_LLI_T *lli_item, bool cb_peritem); /**< linked-list transfer data for mem-to-peri transfer */

    /**
     * Memory to peripherial transfer using circular Linked List.
     * If header is not used, header should be set to NULL and hdr should be set to 0.
     * \note  Before using this function, using Contrl commad "DMAC_CMD_SET_MEM2PERI_RINGLLI" to set circular linked list.
     * 
     * \param[in] channel specific DMA channel ID
     * \param[in] dev DMA request number
     * \param[in] flow Flow control and transfer type
     * \param[in] data_addr point to data address to receive/transmit data with peripheral.
     * \param[in] data_len the number of data transfer unit
     * \param[in] header point to header address to receive/transmit data with peripheral (opt.)
     * \param[in] hdr_len the number of header transfer unit(opt.)
     * 
     */
    DMAC_ERROR_E (*dmac_peritransfer_prerolling)
    (DMAC_CHANNEL_ID_E channel, DMAC_DEVICE_E dev, DMAC_FLOW_CTRL_E flow, void *data_addr, uint32_t data_len, void *header, uint32_t hdr_len); 
} DEV_DMAC, *DEV_DMAC_PTR;

/**
 * @brief This function initializes a specified DMAC interface with a given base address.
 * 
 * @param dmac_id specified DMAC ID
 * @param base_addr  The base address of the DMA controller. This is the memory-mapped address of the
 * DMA controller hardware in the system.
 * @return int an error code of type \ref DMAC_ERROR_E.
 */
int hx_drv_dmac_init(USE_HX_DMAC_E dmac_id, uint32_t base_addr);

/**
 * \brief This function deinitializes a specified DMAC interface.
 * 
 * \param dmac_id specified DMAC ID. using USE_HX_DMAC_ALL to deinit all DMAC interface at same time. 
 * \return int an error code of type \ref DMAC_ERROR_E 
 */
int hx_drv_dmac_deinit(USE_HX_DMAC_E dmac_id);
/**
 * @brief get an \ref DEV_DMAC "dma controller" by dma controller id.
 * 
 * @param dmac_id  specified DMAC ID
 * @return DEV_DMAC_PTR pointer to an \ref dev_uart "dma controler structure"
 */
DEV_DMAC_PTR hx_drv_dmac_get_dev(USE_HX_DMAC_E dmac_id);

#endif /*_HX_DRV_DMAC_H*/
