/*
 * epii_device_addr.h
 *
 *  Created on: 2021/03/27
 *      Author: 902447
 */

#ifndef DEVICE_INC_EPII_DEVICE_ADDR_H_
#define DEVICE_INC_EPII_DEVICE_ADDR_H_


#ifdef FVP_SIM
#define BASE_ADDR_SRAM0                 		(0x10010000)
#define BASE_ADDR_SRAM1                 		(0x10060000)
#define BASE_ADDR_SRAM2                 		(0x10080000)
#define BASE_ADDR_FLASH1_R              		(0x20090000)
#define BASE_ADDR_FLASH1_W              		(0x20090000)
#define BASE_ADDR_FLASH2                		(0x20100000)
#define BASE_ADDR_DTCM_                 		(0x200C0000)
#define BASE_ADDR_FLASH1_R_ALIAS        		(0x20090000)
#define BASE_ADDR_FLASH1_W_ALIAS        		(0x20090000)
#define BASE_ADDR_FLASH2_ALIAS          		(0x20100000)
#define BASE_ADDR_DTCM_ALIAS            		(0x200C0000)

#else
#define BASE_ADDR_ITCM                          (0x00000000)
#define BASE_ADDR_BOOTROM                       (0x08000000)
#define BASE_ADDR_ITCM_ALIAS                    (0x10000000)
#define BASE_ADDR_BOOTROM_ALIAS                 (0x18000000)
#define BASE_ADDR_DTCM                          (0x20000000)
#define BASE_ADDR_SRAM0                         (0x24000000)
#define BASE_ADDR_SRAM1                         (0x24100000)
#define BASE_ADDR_SRAM2                         (0x26000000)
#define BASE_ADDR_FLASH1_W                      (0x28000000)  //denote as flash isp
#define BASE_ADDR_FLASH1_R                      (0x2A000000)  //denote as spi master 0 Quad spi master
#define BASE_ADDR_FLASH2                        (0x2C000000)  //denote as spi master 1 Octa spi master
#define BASE_ADDR_DTCM_ALIAS                    (0x30000000)
#define BASE_ADDR_SRAM0_ALIAS                   (0x34000000)
#define BASE_ADDR_SRAM1_ALIAS                   (0x34100000)
#define BASE_ADDR_SRAM2_ALIAS                   (0x36000000)
#define BASE_ADDR_FLASH1_W_ALIAS                (0x38000000)
#define BASE_ADDR_FLASH1_R_ALIAS                (0x3A000000)
#define BASE_ADDR_FLASH2_ALIAS                  (0x3C000000)

#define BASE_ADDR_DMA0_PL081                    (0x40000000)
#define BASE_ADDR_DMA0_PL081_ALIAS              (0x50000000)
#define BASE_ADDR_AHB_TEST                      (0x40030000)
#define BASE_ADDR_AHB_TEST_ALIAS                (0x50030000)
#define BASE_ADDR_AHB_SSPI                      (0x40800000)  //denote as spi master 2 single spi master
#define BASE_ADDR_AHB_SSPI_ALIAS                (0x50800000)
#define BASE_ADDR_AHB_SPI_S                     (0x40810000)
#define BASE_ADDR_AHB_SPI_S_ALIAS               (0x50810000)
#define BASE_ADDR_DMA1_PL081                    (0x41000000)
#define BASE_ADDR_DMA1_PL081_ALIAS              (0x51000000)
#define BASE_ADDR_SDIO                          (0x41010000)
#define BASE_ADDR_SDIO_ALIAS                    (0x51010000)
#define BASE_ADDR_DMA2_PL080                    (0x41800000)
#define BASE_ADDR_DMA2_PL080_ALIAS              (0x51800000)
#define BASE_ADDR_DMA3_PL080                    (0x41810000)
#define BASE_ADDR_DMA3_PL080_ALIAS              (0x51810000)

#define BASE_ADDR_APB_UART1                     (0x42000000)
#define BASE_ADDR_APB_UART1_ALIAS               (0x52000000)
#define BASE_ADDR_SCU_LSC                       (0x42001000)
#define BASE_ADDR_SCU_LSC_ALIAS                 (0x52001000)
#define BASE_ADDR_APB_I3C_SLAVE1                (0x42002000)
#define BASE_ADDR_APB_I3C_SLAVE1_ALIAS          (0x52002000)
#define BASE_ADDR_PWM                           (0x42003000)
#define BASE_ADDR_PWM_ALIAS                     (0x52003000)
#define BASE_ADDR_PWM_1                         (0x42003040)
#define BASE_ADDR_PWM_1_ALIAS                   (0x52003040)
#define BASE_ADDR_PWM_2                         (0x42003080)
#define BASE_ADDR_PWM_2_ALIAS                   (0x52003080)
#define BASE_ADDR_APB_I2C_MST_1                 (0x42004000)
#define BASE_ADDR_APB_I2C_MST_1_ALIAS           (0x52004000)
#define BASE_ADDR_APB_SND_WIRE                  (0x42005000)
#define BASE_ADDR_APB_SND_WIRE_ALIAS            (0x52005000)
#define BASE_ADDR_APB_SND_WIRE_PDI0             (0x42006000)
#define BASE_ADDR_APB_SND_WIRE_PDI0_ALIAS       (0x52006000)
#define BASE_ADDR_APB_SND_WIRE_PDI1             (0x42007000)
#define BASE_ADDR_APB_SND_WIRE_PDI1_ALIAS       (0x52007000)
#define BASE_ADDR_APB_SND_WIRE_PDI2             (0x42008000)
#define BASE_ADDR_APB_SND_WIRE_PDI2_ALIAS       (0x52008000)
#define BASE_ADDR_APB_VAD_D_TOP                 (0x42009F00)
#define BASE_ADDR_APB_VAD_D_TOP_ALIAS           (0x52009F00)
#define BASE_ADDR_APB_VAD_A_TOP                 (0x4200A000)
#define BASE_ADDR_APB_VAD_A_TOP_ALIAS           (0x5200A000)
#define BASE_ADDR_APB_ROPD_TOP                  (0x4200B000)
#define BASE_ADDR_APB_ROPD_TOP_ALIAS            (0x5200B000)
#define BASE_ADDR_APB_ADC_AIP_LV                (0x4200C000)
#define BASE_ADDR_APB_ADC_AIP_LV_ALIAS          (0x5200C000)
#define BASE_ADDR_APB_ADC_CTRL_LV               (0x4200C800)
#define BASE_ADDR_APB_ADC_CTRL_LV_ALIAS         (0x5200C800)
#define BASE_ADDR_APB_IMG_SC_STATUS           	(0x4200E000)
#define BASE_ADDR_APB_IMG_SC_STATUS_ALIAS     	(0x5200E000)
#define BASE_ADDR_APB_SWREG_LSC                 (0x4200F000)
#define BASE_ADDR_APB_SWREG_LSC_ALIAS           (0x5200F000)

#define BASE_ADDR_APB_SDIO                      (0x43000000)
#define BASE_ADDR_APB_SDIO_ALIAS                (0x53000000)
#define BASE_ADDR_APB_MPC_OSPI                  (0x43010000)
#define BASE_ADDR_APB_MPC_OSPI_ALIAS            (0x53010000)
#define BASE_ADDR_APB_U55_CTRL                  (0x43020000)
#define BASE_ADDR_APB_U55_CTRL_ALIAS            (0x53020000)
#define BASE_ADDR_APBS_CC312_CTRL               (0x43030000)
#define BASE_ADDR_APBS_CC312_CTRL_ALIAS         (0x53030000)
#define BASE_ADDR_APBSC_CC312_CTRL              (0x43040000)
#define BASE_ADDR_APBSC_CC312_CTRL_ALIAS        (0x53040000)
#define BASE_ADDR_APB_PUFRT_DEBUG               (0x43050000)
#define BASE_ADDR_APB_PUFRT_DEBUG_ALIAS         (0x53050000)
#define BASE_ADDR_APB_MIPITX_CTRL               (0x43060000)
#define BASE_ADDR_APB_MIPITX_CTRL_ALIAS         (0x53060000)
#define BASE_ADDR_APB_SCU_HSC                   (0x43070000)
#define BASE_ADDR_APB_SCU_HSC_ALIAS             (0x53070000)
#define BASE_ADDR_APB_DBG_AUTHENTICATE          (0x43080000)
#define BASE_ADDR_APB_DBG_AUTHENTICATE_ALIAS    (0x53080000)
#define BASE_ADDR_APB_SDC                       (0x43090000)
#define BASE_ADDR_APB_SDC_ALIAS                 (0x53090000)
#define BASE_ADDR_APB_DBG_PD_POWER_CTRL         (0x430A0000)
#define BASE_ADDR_APB_DBG_PD_POWER_CTRL_ALIAS   (0x530A0000)
#define BASE_ADDR_APB_SYS_ASYNC_BRIDGE          (0x430B0000)
#define BASE_ADDR_APB_SYS_ASYNC_BRIDGE_ALIAS    (0x530B0000)
#define BASE_ADDR_APB_MPC_SRAM0                 (0x430C0000)
#define BASE_ADDR_APB_MPC_SRAM0_ALIAS           (0x530C0000)
#define BASE_ADDR_APB_MPC_SRAM1                 (0x430D0000)
#define BASE_ADDR_APB_MPC_SRAM1_ALIAS           (0x530D0000)
#define BASE_ADDR_APB_MPC_qspicache             (0x430E0000)
#define BASE_ADDR_APB_MPC_qspicache_ALIAS       (0x530E0000)
#define BASE_ADDR_I3C_HC                        (0x430F0000)
#define BASE_ADDR_I3C_HC_ALIAS                  (0x530F0000)
#define BASE_ADDR_MIPI_RX_CTRL                  (0x430F8000)
#define BASE_ADDR_MIPI_RX_CTRL_ALIAS            (0x530F8000)
#define BASE_ADDR_IMG_PATH                  	(0x43100000)
#define BASE_ADDR_IMG_PATH_ALIAS            	(0x53100000)

#define BASE_ADDR_APB_I2C_SLAVE                 (0x44000000)
#define BASE_ADDR_APB_I2C_SLAVE_ALIAS           (0x54000000)
#define BASE_ADDR_APB_I2C_MST_0                 (0x44001000)
#define BASE_ADDR_APB_I2C_MST_0_ALIAS           (0x54001000)
#define BASE_ADDR_APB_I2S_HOST                  (0x44002000)
#define BASE_ADDR_APB_I2S_HOST_ALIAS            (0x54002000)
#define BASE_ADDR_APB_UART0                     (0x44003000)
#define BASE_ADDR_APB_UART0_ALIAS               (0x54003000)
#define BASE_ADDR_APB_UART2                     (0x44004000)
#define BASE_ADDR_APB_UART2_ALIAS               (0x54004000)
#define BASE_ADDR_APB_PDM                       (0x44005000)
#define BASE_ADDR_APB_PDM_ALIAS                 (0x54005000)
#define BASE_ADDR_APB_GPIO0                     (0x44006000)
#define BASE_ADDR_APB_GPIO0_ALIAS               (0x54006000)
#define BASE_ADDR_APB_GPIO1                     (0x44007000)
#define BASE_ADDR_APB_GPIO1_ALIAS               (0x54007000)
#define BASE_ADDR_APB_GPIO2                     (0x44008000)
#define BASE_ADDR_APB_GPIO2_ALIAS               (0x54008000)
#define BASE_ADDR_APB_GPIO3                     (0x44009000)
#define BASE_ADDR_APB_GPIO3_ALIAS               (0x54009000)
#define BASE_ADDR_APB_MAILBOX                   (0x4400A000)
#define BASE_ADDR_APB_MAILBOX_ALIAS             (0x5400A000)
#define BASE_ADDR_APB_I2S_SLAVE                 (0x4400B000)
#define BASE_ADDR_APB_I2S_SLAVE_ALIAS           (0x5400B000)
#define BASE_ADDR_APB_MPC_SRAM2                 (0x4400C000)
#define BASE_ADDR_APB_MPC_SRAM2_ALIAS           (0x5400C000)
#define BASE_ADDR_APB_I3C_SLAVE0                (0x4400D000)
#define BASE_ADDR_APB_I3C_SLAVE0_ALIAS          (0x5400D000)
#define BASE_ADDR_APB_I2C_SLAVE1                (0x4400E000)
#define BASE_ADDR_APB_I2C_SLAVE1_ALIAS          (0x5400E000)
#define BASE_ADDR_APB_HIMAXPDM                  (0x4400F000)
#define BASE_ADDR_APB_HIMAXPDM_ALIAS            (0x5400F000)

#define BASE_ADDR_APB_WDT0                      (0x45000000)
#define BASE_ADDR_APB_WDT0_ALIAS                (0x55000000)
#define BASE_ADDR_APB_WDT1                      (0x45001000)
#define BASE_ADDR_APB_WDT1_ALIAS                (0x55001000)
#define BASE_ADDR_APB_1_NS_TESTREG              (0x45002000)
#define BASE_ADDR_APB_1_NS_TESTREG_ALIAS        (0x55002000)
#define BASE_ADDR_APB_MIPITX_AIP                (0x45003000)
#define BASE_ADDR_APB_MIPITX_AIP_ALIAS          (0x55003000)
#define BASE_ADDR_APB_INP                       (0x45004000)
#define BASE_ADDR_APB_INP_ALIAS                 (0x55004000)
#define BASE_ADDR_APB_SENSOR_CTRL               (0x45005000)
#define BASE_ADDR_APB_SENSOR_CTRL_ALIAS         (0x55005000)
#define BASE_ADDR_APB_MIPI_RX_PHY               (0x45006000)
#define BASE_ADDR_APB_MIPI_RX_PHY_ALIAS         (0x55006000)
#define BASE_ADDR_APB_TIMER0                    (0x4500A000)
#define BASE_ADDR_APB_TIMER0_ALIAS              (0x5500A000)
#define BASE_ADDR_APB_TIMER1                    (0x4500B000)
#define BASE_ADDR_APB_TIMER1_ALIAS              (0x5500B000)
#define BASE_ADDR_APB_TIMER2                    (0x4500C000)
#define BASE_ADDR_APB_TIMER2_ALIAS              (0x5500C000)
#define BASE_ADDR_APB_TIMER3                    (0x4500D000)
#define BASE_ADDR_APB_TIMER3_ALIAS              (0x5500D000)
#define BASE_ADDR_APB_TIMER4                    (0x4500E000)
#define BASE_ADDR_APB_TIMER4_ALIAS              (0x5500E000)
#define BASE_ADDR_APB_TIMER5                    (0x4500F000)
#define BASE_ADDR_APB_TIMER5_ALIAS              (0x5500F000)

#define BASE_ADDR_APB_GPIO4_SB                  (0x46002000)
#define BASE_ADDR_APB_GPIO4_SB_ALIAS            (0x56002000)
#define BASE_ADDR_APB_TSCTRL                    (0x46004000)
#define BASE_ADDR_APB_TSCTRL_ALIAS              (0x56004000)
#define BASE_ADDR_APB_ADC_AIP_HV                (0x46005000)
#define BASE_ADDR_APB_ADC_AIP_HV_ALIAS          (0x56005000)
#define BASE_ADDR_APB_ADCCTRL_HV                (0x46005800)
#define BASE_ADDR_APB_ADCCTRL_HV_ALIAS          (0x56005800)
#define BASE_ADDR_APB_SCU_SB                    (0x46006000)
#define BASE_ADDR_APB_SCU_SB_ALIAS              (0x56006000)
#define BASE_ADDR_APB_I2CM_MPIP                 (0x46008000)
#define BASE_ADDR_APB_I2CM_MPIP_ALIAS           (0x56008000)
#define BASE_ADDR_APB_TIMER6                    (0x46009000)
#define BASE_ADDR_APB_TIMER6_ALIAS              (0x56009000)
#define BASE_ADDR_APB_TIMER7                    (0x4600A000)
#define BASE_ADDR_APB_TIMER7_ALIAS              (0x5600A000)
#define BASE_ADDR_APB_TIMER8                    (0x4600B000)
#define BASE_ADDR_APB_TIMER8_ALIAS              (0x5600B000)
#define BASE_ADDR_APB_1_PS_TEST                 (0x4600F000)
#define BASE_ADDR_APB_1_PS_TEST_ALIAS           (0x5600F000)

#define BASE_ADDR_APB_SCU_AON                   (0x46100000)
#define BASE_ADDR_APB_SCU_AON_ALIAS             (0x56100000)
#define BASE_ADDR_APB_PMU                       (0x46101000)
#define BASE_ADDR_APB_PMU_ALIAS                 (0x56101000)
#define BASE_ADDR_APB_GPIO5_AON                 (0x46103000)
#define BASE_ADDR_APB_GPIO5_AON_ALIAS           (0x56103000)
#define BASE_ADDR_APB_RTC0                      (0x46107000)
#define BASE_ADDR_APB_RTC0_ALIAS                (0x56107000)
#define BASE_ADDR_APB_RTC1                      (0x46108000)
#define BASE_ADDR_APB_RTC1_ALIAS                (0x56108000)
#define BASE_ADDR_APB_RTC2                      (0x46109000)
#define BASE_ADDR_APB_RTC2_ALIAS                (0x56109000)
#define BASE_ADDR_APB_SWREG_AON                 (0x4610F000)
#define BASE_ADDR_APB_SWREG_AON_ALIAS           (0x5610F000)
#define BASE_ADDR_PPB                           (0xE0000000)
#endif


#define BASE_ADDR_HW_SENSORCTRL         		(0x45005000)
#define BASE_ADDR_HW_SENSORCTRL_ALIAS   		(0x55005000)
#define BASE_ADDR_HW_INP                		(0x45004100)
#define BASE_ADDR_HW_INP_ALIAS          		(0x55004100)
#define BASE_ADDR_HW_CDM                		(0x45004200)
#define BASE_ADDR_HW_CDM_ALIAS          		(0x55004200)
#define BASE_ADDR_HW_DP                 		(0x45004300)
#define BASE_ADDR_HW_DP_ALIAS           		(0x55004300)
#define BASE_ADDR_HW_2X2                		(0x45004400)
#define BASE_ADDR_HW_2X2_ALIAS          		(0x55004400)
#define BASE_ADDR_HW_5X5                		(0x45004500)
#define BASE_ADDR_HW_5X5_ALIAS          		(0x55004500)
#define BASE_ADDR_HW_TPG                		(0x45004600)
#define BASE_ADDR_HW_TPG_ALIAS          		(0x55004600)
#define BASE_ADDR_HW_XDMA               		(0x45004700)
#define BASE_ADDR_HW_XDMA_ALIAS         		(0x55004700)
#define BASE_ADDR_HW_JPEG               		(0x45004800)
#define BASE_ADDR_HW_JPEG_ALIAS         		(0x55004800)
#define BASE_ADDR_HW_HXCSC                  	(0x45004900)
#define BASE_ADDR_HW_HXCSC_ALIAS            	(0x55004900)
#define BASE_ADDR_HW_HXRGB2YUV              	(0x45004A00)
#define BASE_ADDR_HW_HXRGB2YUV_ALIAS        	(0x55004A00)
#define HW_SRAMTEST_BASEADDR            		(0xB0001900)
#define HW_SSIREGBK_BASEADDR            		(0xB0001A00)
#define HW_ARCREGBK_BASEADDR            		(0xB0001B00)
#define BASE_ADDR_HW_EDM                		(0x45004C00)
#define BASE_ADDR_HW_EDM_ALIAS          		(0x55004C00)
#define BASE_ADDR_HW_INP1BITPARSER      		(0x45005D00)
#define BASE_ADDR_HW_INP1BITPARSER_ALIAS    	(0x55005D00)
#define BASE_ADDR_HW_INPOVPARSER      			(0x45005B00)
#define BASE_ADDR_HW_INPOVPARSER_ALIAS    		(0x55005B00)
#define BASE_ADDR_HW_SENSORCTRL_TOP         	(0x45005F00)
#define BASE_ADDR_HW_SENSORCTRL_TOP_ALIAS   	(0x55005F00)
#define BASE_ADDR_HW_HXAUTOI2C                  (0x46008000)
#define BASE_ADDR_HW_HXAUTOI2C_ALIAS            (0x56008000)

#define BASE_ADDR_IMAGE_PATH_STS                (0x43100000) //HSC
#define BASE_ADDR_IMAGE_PATH_STS_ALIAS          (0x53100000) //HSC
#define BASE_ADDR_IMAGE_SENCTRL_STS             (0x4200E000) //LSC
#define BASE_ADDR_IMAGE_SENCTRL_STS_ALIAS       (0x5200E000) //LSC


#define SRAM0_SIZE                  			(0x00100000)
#define SRAM1_SIZE                  			(0x00100000)
#define SRAM2_SIZE                  			(0x00060000)
#define FLASH1_SIZE                 			(0x01000000)
#define FLASH2_SIZE                 			(0x01000000)
#define DTCM_SIZE                   			(0x00040000)
#define ITCM_SIZE                   			(0x00040000)

#define BOOTROM_RW_BASE    						0x34001000
#define BOOTROM_SRAM0_SIZE     					0x00004000
#define BOOTLOADER_BASE    						(BOOTROM_RW_BASE+BOOTROM_SRAM0_SIZE)//0x34005000
#define BOOTLOADER_SIZE     					0x0001A000

#define BOOT2NDLOADER_BASE    					(BOOTLOADER_BASE+BOOTLOADER_SIZE)//0x3401F000
#define BOOT2NDLOADER_SIZE     					0x00018000//0x34037000

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_NS_2x2
#define HW_2X2_BASEADDR			BASE_ADDR_HW_2X2
#else
#define HW_2X2_BASEADDR			BASE_ADDR_HW_2X2_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define HW_2X2_BASEADDR         BASE_ADDR_HW_2X2_ALIAS
#else
#define HW_2X2_BASEADDR			BASE_ADDR_HW_2X2
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_NS_5x5
#define HW_5X5_BASEADDR			BASE_ADDR_HW_5X5
#else
#define HW_5X5_BASEADDR			BASE_ADDR_HW_5X5_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define HW_5X5_BASEADDR         BASE_ADDR_HW_5X5_ALIAS
#else
#define HW_5X5_BASEADDR			BASE_ADDR_HW_5X5
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_NS_ADCC
#define HW_ADCC_BASEADDR                BASE_ADDR_APB_ADC_CTRL_LV
#define HW_ADC_BASEADDR                 BASE_ADDR_APB_ADC_AIP_LV
#define HW_ADC_SB_BASEADDR              BASE_ADDR_APB_ADC_AIP_HV
#else
#define HW_ADCC_BASEADDR                BASE_ADDR_APB_ADC_CTRL_LV_ALIAS
#define HW_ADC_BASEADDR                 BASE_ADDR_APB_ADC_AIP_LV_ALIAS
#define HW_ADC_SB_BASEADDR              BASE_ADDR_APB_ADC_AIP_HV_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define HW_ADCC_BASEADDR                BASE_ADDR_APB_ADC_CTRL_LV_ALIAS
#define HW_ADC_BASEADDR                 BASE_ADDR_APB_ADC_AIP_LV_ALIAS
#define HW_ADC_SB_BASEADDR              BASE_ADDR_APB_ADC_AIP_HV_ALIAS
#else
#define HW_ADCC_BASEADDR                BASE_ADDR_APB_ADC_CTRL_LV
#define HW_ADC_BASEADDR                 BASE_ADDR_APB_ADC_AIP_LV
#define HW_ADC_SB_BASEADDR              BASE_ADDR_APB_ADC_AIP_HV
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_NS_ADCC_HV
#define HW_ADCC_HV_BASEADDR             BASE_ADDR_APB_ADCCTRL_HV
#define HW_ADC_HV_BASEADDR              BASE_ADDR_APB_ADC_AIP_HV
#define HW_ADC_LV_BASEADDR              BASE_ADDR_APB_ADC_AIP_LV
#else
#define HW_ADCC_HV_BASEADDR             BASE_ADDR_APB_ADCCTRL_HV_ALIAS
#define HW_ADC_HV_BASEADDR              BASE_ADDR_APB_ADC_AIP_HV_ALIAS
#define HW_ADC_LV_BASEADDR              BASE_ADDR_APB_ADC_AIP_LV_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define HW_ADCC_HV_BASEADDR             BASE_ADDR_APB_ADCCTRL_HV_ALIAS
#define HW_ADC_HV_BASEADDR              BASE_ADDR_APB_ADC_AIP_HV_ALIAS
#define HW_ADC_LV_BASEADDR              BASE_ADDR_APB_ADC_AIP_LV_ALIAS
#else
#define HW_ADCC_HV_BASEADDR             BASE_ADDR_APB_ADCCTRL_HV
#define HW_ADC_HV_BASEADDR              BASE_ADDR_APB_ADC_AIP_HV
#define HW_ADC_LV_BASEADDR              BASE_ADDR_APB_ADC_AIP_LV
#endif
#endif


#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_cdm) || defined(IP_INST_NS_CDM)
#define HW_CDM_BASEADDR			BASE_ADDR_HW_CDM
#else
#define HW_CDM_BASEADDR			BASE_ADDR_HW_CDM_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define HW_CDM_BASEADDR         BASE_ADDR_HW_CDM_ALIAS
#else
#define HW_CDM_BASEADDR			BASE_ADDR_HW_CDM
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_csirx) || defined(IP_INST_NS_CSIRX)
#define	CSIRX_REGS_BASE 				BASE_ADDR_MIPI_RX_CTRL
#define CSIRX_DPHY_REG					(BASE_ADDR_APB_MIPI_RX_PHY+0x50)
#define CSIRX_DPHY_TUNCATE_REG			(BASE_ADDR_APB_MIPI_RX_PHY+0x48)
#define CSIRX_DPHY_REG_BASE				BASE_ADDR_APB_MIPI_RX_PHY
#else
#define CSIRX_REGS_BASE             	BASE_ADDR_MIPI_RX_CTRL_ALIAS
#define CSIRX_DPHY_REG					(BASE_ADDR_APB_MIPI_RX_PHY_ALIAS+0x50)
#define CSIRX_DPHY_TUNCATE_REG			(BASE_ADDR_APB_MIPI_RX_PHY_ALIAS+0x48)
#define CSIRX_DPHY_REG_BASE				BASE_ADDR_APB_MIPI_RX_PHY_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define CSIRX_REGS_BASE             	BASE_ADDR_MIPI_RX_CTRL_ALIAS
#define CSIRX_DPHY_REG					(BASE_ADDR_APB_MIPI_RX_PHY_ALIAS+0x50)
#define CSIRX_DPHY_TUNCATE_REG			(BASE_ADDR_APB_MIPI_RX_PHY_ALIAS+0x48)
#define CSIRX_DPHY_REG_BASE				BASE_ADDR_APB_MIPI_RX_PHY_ALIAS
#else
#define CSIRX_REGS_BASE             	BASE_ADDR_MIPI_RX_CTRL
#define CSIRX_DPHY_REG					(BASE_ADDR_APB_MIPI_RX_PHY+0x50)
#define CSIRX_DPHY_TUNCATE_REG			(BASE_ADDR_APB_MIPI_RX_PHY+0x48)
#define CSIRX_DPHY_REG_BASE				BASE_ADDR_APB_MIPI_RX_PHY
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_csitx) || defined(IP_INST_NS_CSITX)
#define	CSITX_REGS_BASE 				BASE_ADDR_APB_MIPITX_CTRL
#define CSITX_DPHY_REG					BASE_ADDR_APB_MIPITX_AIP
#else
#define CSITX_REGS_BASE             	BASE_ADDR_APB_MIPITX_CTRL_ALIAS
#define CSITX_DPHY_REG					BASE_ADDR_APB_MIPITX_AIP_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define CSITX_REGS_BASE             	BASE_ADDR_APB_MIPITX_CTRL_ALIAS
#define CSITX_DPHY_REG					BASE_ADDR_APB_MIPITX_AIP_ALIAS
#else
#define CSITX_REGS_BASE             	BASE_ADDR_APB_MIPITX_CTRL
#define CSITX_DPHY_REG					BASE_ADDR_APB_MIPITX_AIP
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_DMA0
#ifdef IP_INST_NS_DMA0
#define DMAC0_BASE BASE_ADDR_DMA0_PL081
#else
#define DMAC0_BASE BASE_ADDR_DMA0_PL081_ALIAS
#endif
#else
#define DMAC0_BASE BASE_ADDR_DMA0_PL081
#endif
#ifdef IP_INST_DMA1
#ifdef IP_INST_NS_DMA1
#define DMAC1_BASE BASE_ADDR_DMA1_PL081
#else
#define DMAC1_BASE BASE_ADDR_DMA1_PL081_ALIAS
#endif
#else
#define DMAC1_BASE BASE_ADDR_DMA1_PL081
#endif
#ifdef IP_INST_DMA2
#ifdef IP_INST_NS_DMA2
#define DMAC2_BASE BASE_ADDR_DMA2_PL080
#else
#define DMAC2_BASE BASE_ADDR_DMA2_PL080_ALIAS
#endif
#else 
#define DMAC2_BASE BASE_ADDR_DMA2_PL080
#endif
#ifdef IP_INST_DMA3
#ifdef IP_INST_NS_DMA3
#define DMAC3_BASE BASE_ADDR_DMA3_PL080
#else
#define DMAC3_BASE BASE_ADDR_DMA3_PL080_ALIAS
#endif
#else
#define DMAC3_BASE BASE_ADDR_DMA3_PL080
#endif
#else
#ifndef TRUSTZONE
#define DMAC0_BASE BASE_ADDR_DMA0_PL081_ALIAS
#define DMAC1_BASE BASE_ADDR_DMA1_PL081_ALIAS
#define DMAC2_BASE BASE_ADDR_DMA2_PL080_ALIAS
#define DMAC3_BASE BASE_ADDR_DMA3_PL080_ALIAS
#else
#ifdef IP_INST_DMA0
#define DMAC0_BASE BASE_ADDR_DMA0_PL081
#else
#define DMAC0_BASE BASE_ADDR_DMA0_PL081_ALIAS
#endif
#ifdef IP_INST_DMA1
#define DMAC1_BASE BASE_ADDR_DMA1_PL081
#else
#define DMAC1_BASE BASE_ADDR_DMA1_PL081_ALIAS
#endif
#ifdef IP_INST_DMA2
#define DMAC2_BASE BASE_ADDR_DMA2_PL080
#else
#define DMAC2_BASE BASE_ADDR_DMA2_PL080_ALIAS
#endif
#ifdef IP_INST_DMA3
#define DMAC3_BASE BASE_ADDR_DMA3_PL080
#else
#define DMAC3_BASE BASE_ADDR_DMA3_PL080_ALIAS
#endif
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_dp) || defined(IP_INST_NS_DP)
#define HW_DP_BASEADDR			                BASE_ADDR_HW_DP
#define HW_HXRGB2YUV_BASEADDR                   BASE_ADDR_HW_HXRGB2YUV
#define HW_HXCSC_BASEADDR                   	BASE_ADDR_HW_HXCSC
#define HW_DP_HSC_BASEADDR                      (BASE_ADDR_IMAGE_PATH_STS+0x300)
#define HW_IMAGEPATH_RXPCLK_BASEADDR            (BASE_ADDR_APB_SCU_AON+0x28)
#else
#define HW_DP_BASEADDR			                BASE_ADDR_HW_DP_ALIAS
#define HW_HXRGB2YUV_BASEADDR                   BASE_ADDR_HW_HXRGB2YUV_ALIAS
#define HW_HXCSC_BASEADDR                   	BASE_ADDR_HW_HXCSC_ALIAS
#define HW_DP_HSC_BASEADDR                      (BASE_ADDR_IMAGE_PATH_STS_ALIAS+0x300)
#define HW_IMAGEPATH_RXPCLK_BASEADDR            (BASE_ADDR_APB_SCU_AON_ALIAS+0x28)
#endif
#else
#ifndef TRUSTZONE
#define HW_DP_BASEADDR                          BASE_ADDR_HW_DP_ALIAS
#define HW_HXRGB2YUV_BASEADDR                   BASE_ADDR_HW_HXRGB2YUV_ALIAS
#define HW_HXCSC_BASEADDR                   	BASE_ADDR_HW_HXCSC_ALIAS
#define HW_DP_HSC_BASEADDR                      (BASE_ADDR_IMAGE_PATH_STS_ALIAS+0x300)
#define HW_IMAGEPATH_RXPCLK_BASEADDR            (BASE_ADDR_APB_SCU_AON_ALIAS+0x28)
#else
#define HW_DP_BASEADDR			                BASE_ADDR_HW_DP
#define HW_HXRGB2YUV_BASEADDR                   BASE_ADDR_HW_HXRGB2YUV
#define HW_HXCSC_BASEADDR                   	BASE_ADDR_HW_HXCSC
#define HW_DP_HSC_BASEADDR                      (BASE_ADDR_IMAGE_PATH_STS+0x300)
#define HW_IMAGEPATH_RXPCLK_BASEADDR            (BASE_ADDR_APB_SCU_AON+0x28)
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_edm) || defined(IP_INST_NS_EDM)
#define HW_EDM_BASEADDR							BASE_ADDR_HW_EDM
#define HW_EDM_HSC_BASEADDR                     (BASE_ADDR_IMAGE_PATH_STS+0xC00)
#define HW_IMAGEPATH_RXPCLK_BASEADDR			(BASE_ADDR_APB_SCU_AON+0x28)
#else
#define HW_EDM_BASEADDR							BASE_ADDR_HW_EDM_ALIAS
#define HW_EDM_HSC_BASEADDR                     (BASE_ADDR_IMAGE_PATH_STS_ALIAS+0xC00)
#define HW_IMAGEPATH_RXPCLK_BASEADDR			(BASE_ADDR_APB_SCU_AON_ALIAS+0x28)
#endif
#else
#ifndef TRUSTZONE
#define HW_EDM_BASEADDR                         BASE_ADDR_HW_EDM_ALIAS
#define HW_EDM_HSC_BASEADDR                     (BASE_ADDR_IMAGE_PATH_STS_ALIAS+0xC00)
#define HW_IMAGEPATH_RXPCLK_BASEADDR            (BASE_ADDR_APB_SCU_AON_ALIAS+0x28)
#else
#define HW_EDM_BASEADDR							BASE_ADDR_HW_EDM
#define HW_EDM_HSC_BASEADDR                     (BASE_ADDR_IMAGE_PATH_STS+0xC00)
#define HW_IMAGEPATH_RXPCLK_BASEADDR			(BASE_ADDR_APB_SCU_AON+0x28)
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_GPIO_G0
#ifdef IP_INST_NS_GPIO_G0
#define HX_GPIO_GROUP_0_BASE      BASE_ADDR_APB_GPIO0
#else
#define HX_GPIO_GROUP_0_BASE      BASE_ADDR_APB_GPIO0_ALIAS
#endif
#endif
#ifdef IP_INST_GPIO_G1
#ifdef IP_INST_NS_GPIO_G1
#define HX_GPIO_GROUP_1_BASE      BASE_ADDR_APB_GPIO1
#else
#define HX_GPIO_GROUP_1_BASE      BASE_ADDR_APB_GPIO1_ALIAS
#endif
#endif
#ifdef IP_INST_GPIO_G2
#ifdef IP_INST_NS_GPIO_G2
#define HX_GPIO_GROUP_2_BASE      BASE_ADDR_APB_GPIO2
#else
#define HX_GPIO_GROUP_2_BASE      BASE_ADDR_APB_GPIO2_ALIAS
#endif
#endif
#ifdef IP_INST_GPIO_G3
#ifdef IP_INST_NS_GPIO_G3
#define HX_GPIO_GROUP_3_BASE      BASE_ADDR_APB_GPIO3
#else
#define HX_GPIO_GROUP_3_BASE      BASE_ADDR_APB_GPIO3_ALIAS
#endif
#endif
#ifdef IP_INST_SB_GPIO
#ifdef IP_INST_NS_SB_GPIO
#define HX_GPIO_GROUP_4_BASE      BASE_ADDR_APB_GPIO4_SB
#else
#define HX_GPIO_GROUP_4_BASE      BASE_ADDR_APB_GPIO4_SB_ALIAS
#endif
#endif
#ifdef IP_INST_AON_GPIO
#ifdef IP_INST_NS_AON_GPIO
#define HX_GPIO_GROUP_5_BASE      BASE_ADDR_APB_GPIO5_AON
#else
#define HX_GPIO_GROUP_5_BASE      BASE_ADDR_APB_GPIO5_AON_ALIAS
#endif
#endif
#else
#ifndef TRUSTZONE
#ifdef IP_INST_GPIO_G0
#define HX_GPIO_GROUP_0_BASE      BASE_ADDR_APB_GPIO0_ALIAS
#endif
#ifdef IP_INST_GPIO_G1
#define HX_GPIO_GROUP_1_BASE      BASE_ADDR_APB_GPIO1_ALIAS
#endif
#ifdef IP_INST_GPIO_G2
#define HX_GPIO_GROUP_2_BASE      BASE_ADDR_APB_GPIO2_ALIAS
#endif
#ifdef IP_INST_GPIO_G3
#define HX_GPIO_GROUP_3_BASE      BASE_ADDR_APB_GPIO3_ALIAS
#endif
#ifdef IP_INST_SB_GPIO
#define HX_GPIO_GROUP_4_BASE      BASE_ADDR_APB_GPIO4_SB_ALIAS
#endif
#ifdef IP_INST_AON_GPIO
#define HX_GPIO_GROUP_5_BASE      BASE_ADDR_APB_GPIO5_AON_ALIAS
#endif
#else
#ifdef IP_INST_GPIO_G0
#define HX_GPIO_GROUP_0_BASE      BASE_ADDR_APB_GPIO0
#endif
#ifdef IP_INST_GPIO_G1
#define HX_GPIO_GROUP_1_BASE      BASE_ADDR_APB_GPIO1
#endif
#ifdef IP_INST_GPIO_G2
#define HX_GPIO_GROUP_2_BASE      BASE_ADDR_APB_GPIO2
#endif
#ifdef IP_INST_GPIO_G3
#define HX_GPIO_GROUP_3_BASE      BASE_ADDR_APB_GPIO3
#endif
#ifdef IP_INST_SB_GPIO
#define HX_GPIO_GROUP_4_BASE      BASE_ADDR_APB_GPIO4_SB
#endif
#ifdef IP_INST_AON_GPIO
#define HX_GPIO_GROUP_5_BASE      BASE_ADDR_APB_GPIO5_AON
#endif
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_hxautoi2c_mst) || defined(IP_INST_NS_HXAUTOI2C_MST)
#define HW_HXAUTOI2C_BASE_ADDR            BASE_ADDR_HW_HXAUTOI2C
#else
#define HW_HXAUTOI2C_BASE_ADDR            BASE_ADDR_HW_HXAUTOI2C_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define HW_HXAUTOI2C_BASE_ADDR            BASE_ADDR_HW_HXAUTOI2C_ALIAS
#else
#define HW_HXAUTOI2C_BASE_ADDR            BASE_ADDR_HW_HXAUTOI2C
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_i3c_mst) || defined(IP_INST_NS_I3C_MST)
#define HW_I3C_HC_BASE_ADDR             BASE_ADDR_I3C_HC
#else
#define HW_I3C_HC_BASE_ADDR             BASE_ADDR_I3C_HC_ALIAS
#endif
#else
#define HW_I3C_HC_BASE_ADDR             BASE_ADDR_I3C_HC
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_IIIC_SLAVE0
#ifdef IP_INST_NS_IIIC_SLAVE0
#define HX_I3C_SLV0_BASE              BASE_ADDR_APB_I3C_SLAVE0
#else
#define HX_I3C_SLV0_BASE              BASE_ADDR_APB_I3C_SLAVE0_ALIAS
#endif
#endif
#ifdef IP_INST_IIIC_SLAVE1
#ifdef IP_INST_NS_IIIC_SLAVE1
#define HX_I3C_SLV1_BASE              BASE_ADDR_APB_I3C_SLAVE1
#else
#define HX_I3C_SLV1_BASE              BASE_ADDR_APB_I3C_SLAVE1_ALIAS
#endif
#endif
#else
#ifndef TRUSTZONE
#ifdef IP_INST_IIIC_SLAVE0
#define HX_I3C_SLV0_BASE              BASE_ADDR_APB_I3C_SLAVE0_ALIAS
#endif
#ifdef IP_INST_IIIC_SLAVE1
#define HX_I3C_SLV1_BASE              BASE_ADDR_APB_I3C_SLAVE1_ALIAS
#endif
#else
#ifdef IP_INST_IIIC_SLAVE0
#define HX_I3C_SLV0_BASE              BASE_ADDR_APB_I3C_SLAVE0
#endif
#ifdef IP_INST_IIIC_SLAVE1
#define HX_I3C_SLV1_BASE              BASE_ADDR_APB_I3C_SLAVE1
#endif
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_IIC_HOST
#ifdef IP_INST_NS_IIC_HOST
#define HX_I2C_HOST_MST_0_BASE      BASE_ADDR_APB_I2C_MST_0
#else
#define HX_I2C_HOST_MST_0_BASE      BASE_ADDR_APB_I2C_MST_0_ALIAS
#endif
#endif
#ifdef IP_INST_IIC_HOST_SENSOR
#ifdef IP_INST_NS_IIC_HOST_SENSOR
#define HX_I2C_HOST_MST_1_BASE      BASE_ADDR_APB_I2C_MST_1
#else
#define HX_I2C_HOST_MST_1_BASE      BASE_ADDR_APB_I2C_MST_1_ALIAS
#endif
#endif
#ifdef IP_INST_IIC_SLAVE0
#ifdef IP_INST_NS_IIC_SLAVE0
#define HX_I2C_HOST_SLV_0_BASE      BASE_ADDR_APB_I2C_SLAVE
#else
#define HX_I2C_HOST_SLV_0_BASE      BASE_ADDR_APB_I2C_SLAVE_ALIAS
#endif
#endif
#ifdef IP_INST_IIC_SLAVE1
#ifdef IP_INST_NS_IIC_SLAVE1
#define HX_I2C_HOST_SLV_1_BASE      BASE_ADDR_APB_I2C_SLAVE1
#else
#define HX_I2C_HOST_SLV_1_BASE      BASE_ADDR_APB_I2C_SLAVE1_ALIAS
#endif
#endif
#else
#ifndef TRUSTZONE
#ifdef IP_INST_IIC_HOST
#define HX_I2C_HOST_MST_0_BASE      BASE_ADDR_APB_I2C_MST_0_ALIAS
#endif
#ifdef IP_INST_IIC_HOST_SENSOR
#define HX_I2C_HOST_MST_1_BASE      BASE_ADDR_APB_I2C_MST_1_ALIAS
#endif
#ifdef IP_INST_IIC_SLAVE0
#define HX_I2C_HOST_SLV_0_BASE      BASE_ADDR_APB_I2C_SLAVE_ALIAS
#endif
#ifdef IP_INST_IIC_SLAVE1
#define HX_I2C_HOST_SLV_1_BASE      BASE_ADDR_APB_I2C_SLAVE1_ALIAS
#endif
#else
#ifdef IP_INST_IIC_HOST
#define HX_I2C_HOST_MST_0_BASE      BASE_ADDR_APB_I2C_MST_0
#endif
#ifdef IP_INST_IIC_HOST_SENSOR
#define HX_I2C_HOST_MST_1_BASE      BASE_ADDR_APB_I2C_MST_1
#endif
#ifdef IP_INST_IIC_SLAVE0
#define HX_I2C_HOST_SLV_0_BASE      BASE_ADDR_APB_I2C_SLAVE
#endif
#ifdef IP_INST_IIC_SLAVE1
#define HX_I2C_HOST_SLV_1_BASE      BASE_ADDR_APB_I2C_SLAVE1
#endif
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_inp) || defined(IP_INST_NS_INP)
#define HW_INP_BASEADDR					BASE_ADDR_HW_INP
#define HW_INP_HSC_BASEADDR             (BASE_ADDR_IMAGE_PATH_STS+0x100)
#define HW_SEN_CTRL_SWRST_BASEADDR		BASE_ADDR_HW_SENSORCTRL
#else
#define HW_INP_BASEADDR					BASE_ADDR_HW_INP_ALIAS
#define HW_INP_HSC_BASEADDR             (BASE_ADDR_IMAGE_PATH_STS_ALIAS+0x100)
#define HW_SEN_CTRL_SWRST_BASEADDR		BASE_ADDR_HW_SENSORCTRL_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define HW_INP_BASEADDR                 BASE_ADDR_HW_INP_ALIAS
#define HW_INP_HSC_BASEADDR             (BASE_ADDR_IMAGE_PATH_STS_ALIAS+0x100)
#define HW_SEN_CTRL_SWRST_BASEADDR      BASE_ADDR_HW_SENSORCTRL_ALIAS
#else
#define HW_INP_BASEADDR					BASE_ADDR_HW_INP
#define HW_INP_HSC_BASEADDR             (BASE_ADDR_IMAGE_PATH_STS+0x100)
#define HW_SEN_CTRL_SWRST_BASEADDR		BASE_ADDR_HW_SENSORCTRL
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_inp1bitparser) || defined(IP_INST_NS_INP1BITPARSER)
#define HW_INP1BITPARSER_BASEADDR			BASE_ADDR_HW_INP1BITPARSER
#define HW_INP1BITPARSER_LSC_BASEADDR       (BASE_ADDR_IMAGE_SENCTRL_STS+0xD00)
#else
#define HW_INP1BITPARSER_BASEADDR			BASE_ADDR_HW_INP1BITPARSER_ALIAS
#define HW_INP1BITPARSER_LSC_BASEADDR       (BASE_ADDR_IMAGE_SENCTRL_STS_ALIAS+0xD00)
#endif
#else
#ifndef TRUSTZONE
#define HW_INP1BITPARSER_BASEADDR           BASE_ADDR_HW_INP1BITPARSER_ALIAS
#define HW_INP1BITPARSER_LSC_BASEADDR       (BASE_ADDR_IMAGE_SENCTRL_STS_ALIAS+0xD00)
#else
#define HW_INP1BITPARSER_BASEADDR			BASE_ADDR_HW_INP1BITPARSER
#define HW_INP1BITPARSER_LSC_BASEADDR       (BASE_ADDR_IMAGE_SENCTRL_STS+0xD00)
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_inpovparser) || defined(IP_INST_NS_INPOVPARSER)
#define HW_INPOVPARSER_BASEADDR			BASE_ADDR_HW_INPOVPARSER
#define HW_INPOVPARSER_LSC_BASEADDR       	(BASE_ADDR_IMAGE_SENCTRL_STS+0xB00)
#else
#define HW_INPOVPARSER_BASEADDR			BASE_ADDR_HW_INPOVPARSER_ALIAS
#define HW_INPOVPARSER_LSC_BASEADDR       	(BASE_ADDR_IMAGE_SENCTRL_STS_ALIAS+0xB00)
#endif
#else
#ifndef TRUSTZONE
#define HW_INPOVPARSER_BASEADDR           	BASE_ADDR_HW_INPOVPARSER_ALIAS
#define HW_INPOVPARSER_LSC_BASEADDR       	(BASE_ADDR_IMAGE_SENCTRL_STS_ALIAS+0xB00)
#else
#define HW_INPOVPARSER_BASEADDR				BASE_ADDR_HW_INPOVPARSER
#define HW_INPOVPARSER_LSC_BASEADDR       	(BASE_ADDR_IMAGE_SENCTRL_STS+0xB00)
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_jpeg) || defined(IP_INST_NS_JPEG)
#define HW_JPEG_BASEADDR			BASE_ADDR_HW_JPEG
#define HW_JPEG_HSC_BASEADDR        (BASE_ADDR_IMAGE_PATH_STS+0x800)
#else
#define HW_JPEG_BASEADDR			BASE_ADDR_HW_JPEG_ALIAS
#define HW_JPEG_HSC_BASEADDR        (BASE_ADDR_IMAGE_PATH_STS_ALIAS+0x800)
#endif
#else
#ifndef TRUSTZONE
#define HW_JPEG_BASEADDR            BASE_ADDR_HW_JPEG_ALIAS
#define HW_JPEG_HSC_BASEADDR        (BASE_ADDR_IMAGE_PATH_STS_ALIAS+0x800)
#else
#define HW_JPEG_BASEADDR			BASE_ADDR_HW_JPEG
#define HW_JPEG_HSC_BASEADDR        (BASE_ADDR_IMAGE_PATH_STS+0x800)
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_mb) || defined(IP_INST_NS_MB)
#define HX_MAILBOX_INT_BASE      BASE_ADDR_APB_MAILBOX
#define HX_MAILBOX_INFO_BASE      (BASE_ADDR_APB_MAILBOX + 0x100)
#define HX_MAILBOX_MUTEX_BASE      (BASE_ADDR_APB_MAILBOX + 0x200)
#else
#define HX_MAILBOX_INT_BASE      BASE_ADDR_APB_MAILBOX_ALIAS
#define HX_MAILBOX_INFO_BASE      (BASE_ADDR_APB_MAILBOX_ALIAS + 0x100)
#define HX_MAILBOX_MUTEX_BASE      (BASE_ADDR_APB_MAILBOX_ALIAS + 0x200)
#endif
#else
#ifndef TRUSTZONE
#define HX_MAILBOX_INT_BASE      BASE_ADDR_APB_MAILBOX_ALIAS
#define HX_MAILBOX_INFO_BASE      (BASE_ADDR_APB_MAILBOX_ALIAS + 0x100)
#define HX_MAILBOX_MUTEX_BASE      (BASE_ADDR_APB_MAILBOX_ALIAS + 0x200)
#else
#define HX_MAILBOX_INT_BASE      BASE_ADDR_APB_MAILBOX
#define HX_MAILBOX_INFO_BASE      (BASE_ADDR_APB_MAILBOX + 0x100)
#define HX_MAILBOX_MUTEX_BASE      (BASE_ADDR_APB_MAILBOX + 0x200)
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_mpc) || defined(IP_INST_NS_MPC)
#define HX_MPC_SRAM0_BASE      BASE_ADDR_APB_MPC_SRAM0
#define HX_MPC_SRAM1_BASE      BASE_ADDR_APB_MPC_SRAM1
#define HX_MPC_SRAM2_BASE      BASE_ADDR_APB_MPC_SRAM2
#define HX_MPC_QSPI_BASE      BASE_ADDR_APB_MPC_qspicache
#define HX_MPC_OSPI_BASE      BASE_ADDR_APB_MPC_OSPI
#else
#define HX_MPC_SRAM0_BASE      BASE_ADDR_APB_MPC_SRAM0_ALIAS
#define HX_MPC_SRAM1_BASE      BASE_ADDR_APB_MPC_SRAM1_ALIAS
#define HX_MPC_SRAM2_BASE      BASE_ADDR_APB_MPC_SRAM2_ALIAS
#define HX_MPC_QSPI_BASE      BASE_ADDR_APB_MPC_qspicache_ALIAS
#define HX_MPC_OSPI_BASE      BASE_ADDR_APB_MPC_OSPI_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define HX_MPC_SRAM0_BASE      BASE_ADDR_APB_MPC_SRAM0_ALIAS
#define HX_MPC_SRAM1_BASE      BASE_ADDR_APB_MPC_SRAM1_ALIAS
#define HX_MPC_SRAM2_BASE      BASE_ADDR_APB_MPC_SRAM2_ALIAS
#define HX_MPC_QSPI_BASE      BASE_ADDR_APB_MPC_qspicache_ALIAS
#define HX_MPC_OSPI_BASE      BASE_ADDR_APB_MPC_OSPI_ALIAS
#else
#define HX_MPC_SRAM0_BASE      BASE_ADDR_APB_MPC_SRAM0
#define HX_MPC_SRAM1_BASE      BASE_ADDR_APB_MPC_SRAM1
#define HX_MPC_SRAM2_BASE      BASE_ADDR_APB_MPC_SRAM2
#define HX_MPC_QSPI_BASE      BASE_ADDR_APB_MPC_qspicache
#define HX_MPC_OSPI_BASE      BASE_ADDR_APB_MPC_OSPI
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_NS_PDM
#define HIMAX_PDM_BASE_ADDR         BASE_ADDR_APB_HIMAXPDM
#else
#define HIMAX_PDM_BASE_ADDR         BASE_ADDR_APB_HIMAXPDM_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define HIMAX_PDM_BASE_ADDR         BASE_ADDR_APB_HIMAXPDM_ALIAS
#else
#define HIMAX_PDM_BASE_ADDR         BASE_ADDR_APB_HIMAXPDM
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_pmu) || defined(IP_INST_NS_PMU)
#define HX_PMU_BASE      BASE_ADDR_APB_PMU
#else
#define HX_PMU_BASE      BASE_ADDR_APB_PMU_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define HX_PMU_BASE      BASE_ADDR_APB_PMU_ALIAS
#else
#define HX_PMU_BASE      BASE_ADDR_APB_PMU
#endif
#endif

/*
PWM, one PPC configuration for three PWM instances
*/
#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_pwm) || defined(IP_INST_NS_PWM)
#define HW_PWM0_BASEADDR      BASE_ADDR_PWM
#define HW_PWM1_BASEADDR      BASE_ADDR_PWM_1
#define HW_PWM2_BASEADDR      BASE_ADDR_PWM_2
#else
#define HW_PWM0_BASEADDR      BASE_ADDR_PWM_ALIAS
#define HW_PWM1_BASEADDR      BASE_ADDR_PWM_1_ALIAS
#define HW_PWM2_BASEADDR      BASE_ADDR_PWM_2_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define HW_PWM0_BASEADDR      BASE_ADDR_PWM_ALIAS
#define HW_PWM1_BASEADDR      BASE_ADDR_PWM_1_ALIAS
#define HW_PWM2_BASEADDR      BASE_ADDR_PWM_2_ALIAS
#else
#define HW_PWM0_BASEADDR      BASE_ADDR_PWM
#define HW_PWM1_BASEADDR      BASE_ADDR_PWM_1
#define HW_PWM2_BASEADDR      BASE_ADDR_PWM_2
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_RTC0
#ifdef IP_INST_NS_RTC0
#define HX_RTC0_BASE 	BASE_ADDR_APB_RTC0
#else
#define HX_RTC0_BASE 	BASE_ADDR_APB_RTC0_ALIAS
#endif//#ifdef IP_INST_NS_RTC0
#else
#define HX_RTC0_BASE 	BASE_ADDR_APB_RTC0
#endif//#ifdef IP_INST_RTC0
#ifdef IP_INST_RTC1
#ifdef IP_INST_NS_RTC1
#define HX_RTC1_BASE 	BASE_ADDR_APB_RTC1
#else
#define HX_RTC1_BASE 	BASE_ADDR_APB_RTC1_ALIAS
#endif
#else
#define HX_RTC1_BASE 	BASE_ADDR_APB_RTC1
#endif//#ifdef IP_INST_RTC1
#ifdef IP_INST_RTC2
#ifdef IP_INST_NS_RTC2
#define HX_RTC2_BASE 	BASE_ADDR_APB_RTC2
#else
#define HX_RTC2_BASE 	BASE_ADDR_APB_RTC2_ALIAS
#endif
#else
#define HX_RTC2_BASE 	BASE_ADDR_APB_RTC2
#endif
#else
#ifndef TRUSTZONE
#define HX_RTC0_BASE 	BASE_ADDR_APB_RTC0_ALIAS
#define HX_RTC1_BASE 	BASE_ADDR_APB_RTC1_ALIAS
#define HX_RTC2_BASE 	BASE_ADDR_APB_RTC2_ALIAS
#else
#define HX_RTC0_BASE 	BASE_ADDR_APB_RTC0
#define HX_RTC1_BASE 	BASE_ADDR_APB_RTC1
#define HX_RTC2_BASE 	BASE_ADDR_APB_RTC2
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_scu) || defined(IP_INST_NS_SCU)
#define SCU_LSC_ADDR	BASE_ADDR_SCU_LSC
#define SCU_HSC_ADDR	BASE_ADDR_APB_SCU_HSC
#define SCU_AON_ADDR	BASE_ADDR_APB_SCU_AON
#define SCU_SB_ADDR		BASE_ADDR_APB_SCU_SB
#else
#define SCU_LSC_ADDR	BASE_ADDR_SCU_LSC_ALIAS
#define SCU_HSC_ADDR	BASE_ADDR_APB_SCU_HSC_ALIAS
#define SCU_AON_ADDR	BASE_ADDR_APB_SCU_AON_ALIAS
#define SCU_SB_ADDR		BASE_ADDR_APB_SCU_SB_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define SCU_LSC_ADDR	BASE_ADDR_SCU_LSC_ALIAS
#define SCU_HSC_ADDR	BASE_ADDR_APB_SCU_HSC_ALIAS
#define SCU_AON_ADDR	BASE_ADDR_APB_SCU_AON_ALIAS
#define SCU_SB_ADDR		BASE_ADDR_APB_SCU_SB_ALIAS
#else
#define SCU_LSC_ADDR	BASE_ADDR_SCU_LSC
#define SCU_HSC_ADDR	BASE_ADDR_APB_SCU_HSC
#define SCU_AON_ADDR	BASE_ADDR_APB_SCU_AON
#define SCU_SB_ADDR		BASE_ADDR_APB_SCU_SB
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_sdio) || defined(IP_INST_NS_SDIO)
#define SD_REG_BASE              BASE_ADDR_APB_SDIO
#else
#define SD_REG_BASE              BASE_ADDR_APB_SDIO_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define SD_REG_BASE              BASE_ADDR_APB_SDIO_ALIAS
#else
#define SD_REG_BASE              BASE_ADDR_APB_SDIO
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_sensorctrl) || defined(IP_INST_NS_SENSORCTRL)
#define HW_SENSORCTRL_BASEADDR			BASE_ADDR_HW_SENSORCTRL
#define HW_SENSORCTRL_LSC_BASEADDR      (BASE_ADDR_IMAGE_SENCTRL_STS+0x000)
#else
#define HW_SENSORCTRL_BASEADDR			BASE_ADDR_HW_SENSORCTRL_ALIAS
#define HW_SENSORCTRL_LSC_BASEADDR      (BASE_ADDR_IMAGE_SENCTRL_STS_ALIAS+0x000)
#endif
#else
#ifndef TRUSTZONE
#define HW_SENSORCTRL_BASEADDR          BASE_ADDR_HW_SENSORCTRL_ALIAS
#define HW_SENSORCTRL_LSC_BASEADDR      (BASE_ADDR_IMAGE_SENCTRL_STS_ALIAS+0x000)
#else
#define HW_SENSORCTRL_BASEADDR			BASE_ADDR_HW_SENSORCTRL
#define HW_SENSORCTRL_LSC_BASEADDR      (BASE_ADDR_IMAGE_SENCTRL_STS+0x000)
#endif
#endif

#ifdef TRUSTZONE_SEC

#define HX_SNDWIRE_BASE 	BASE_ADDR_APB_SND_WIRE_ALIAS
#define HX_SNDWIRE_PDI0_BASE 	BASE_ADDR_APB_SND_WIRE_PDI0_ALIAS
#define HX_SNDWIRE_PDI1_BASE 	BASE_ADDR_APB_SND_WIRE_PDI1_ALIAS
#define HX_SNDWIRE_PDI2_BASE 	BASE_ADDR_APB_SND_WIRE_PDI2_ALIAS

#else
#ifndef TRUSTZONE

#define HX_SNDWIRE_BASE 	BASE_ADDR_APB_SND_WIRE_ALIAS
#define HX_SNDWIRE_PDI0_BASE 	BASE_ADDR_APB_SND_WIRE_PDI0_ALIAS
#define HX_SNDWIRE_PDI1_BASE 	BASE_ADDR_APB_SND_WIRE_PDI1_ALIAS
#define HX_SNDWIRE_PDI2_BASE 	BASE_ADDR_APB_SND_WIRE_PDI2_ALIAS

#else

#define HX_SNDWIRE_BASE 	BASE_ADDR_APB_SND_WIRE
#define HX_SNDWIRE_PDI0_BASE 	BASE_ADDR_APB_SND_WIRE_PDI0
#define HX_SNDWIRE_PDI1_BASE 	BASE_ADDR_APB_SND_WIRE_PDI1
#define HX_SNDWIRE_PDI2_BASE 	BASE_ADDR_APB_SND_WIRE_PDI2

#endif
#endif


#ifdef TRUSTZONE_SEC
#define DW_SPI_Q_RELBASE (BASE_ADDR_FLASH1_R_ALIAS) /*!< designware qspi relative baseaddr */
#ifdef IP_INST_SSPI_HOST
#ifdef IP_INST_NS_SSPI_HOST
#define DW_SPI_S_RELBASE (BASE_ADDR_AHB_SSPI) /*!< designware sspi relative baseaddr */
#else
#define DW_SPI_S_RELBASE (BASE_ADDR_AHB_SSPI_ALIAS) /*!< designware sspi relative baseaddr */
#endif
#else
#define DW_SPI_S_RELBASE (BASE_ADDR_AHB_SSPI) /*!< designware sspi relative baseaddr */
#endif
#define DW_SPI_O_RELBASE (BASE_ADDR_FLASH2_ALIAS) /*!< designware ospi relative baseaddr */
#else
#ifndef TRUSTZONE
#define DW_SPI_Q_RELBASE (BASE_ADDR_FLASH1_R_ALIAS) /*!< designware qspi relative baseaddr */
#define DW_SPI_S_RELBASE (BASE_ADDR_AHB_SSPI_ALIAS) /*!< designware sspi relative baseaddr */
#define DW_SPI_O_RELBASE (BASE_ADDR_FLASH2_ALIAS)   /*!< designware sspi relative baseaddr */
#else
#define DW_SPI_Q_RELBASE (BASE_ADDR_FLASH1_R) /*!< designware qspi relative baseaddr */
#define DW_SPI_S_RELBASE (BASE_ADDR_AHB_SSPI) /*!< designware sspi relative baseaddr */
#define DW_SPI_O_RELBASE (BASE_ADDR_FLASH2)   /*!< designware ospi relative baseaddr */
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_SSPI_SLAVE
#ifdef IP_INST_NS_SSPI_SLAVE
#define DW_SSPI_S_RELBASE (BASE_ADDR_AHB_SPI_S) /*!< designware spi slave relative baseaddr */
#else
#define DW_SSPI_S_RELBASE (BASE_ADDR_AHB_SPI_S_ALIAS) /*!< designware spi slave relative baseaddr */
#endif
#else
#define DW_SSPI_S_RELBASE (BASE_ADDR_AHB_SPI_S) /*!< designware spi slave relative baseaddr */
#endif
#else
#ifndef TRUSTZONE
#define DW_SSPI_S_RELBASE (BASE_ADDR_AHB_SPI_S_ALIAS) /*!< designware spi slave relative baseaddr */
#else
#define DW_SSPI_S_RELBASE (BASE_ADDR_AHB_SPI_S) /*!< designware spi slave relative baseaddr */
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_swreg_aon) || defined(IP_INST_NS_SWREG_AON)
#define SWREG_AON_ADDR	BASE_ADDR_APB_SWREG_AON
#else
#define SWREG_AON_ADDR	BASE_ADDR_APB_SWREG_AON_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define SWREG_AON_ADDR	BASE_ADDR_APB_SWREG_AON_ALIAS
#else
#define SWREG_AON_ADDR	BASE_ADDR_APB_SWREG_AON
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_swreg_lsc) || defined(IP_INST_NS_SWREG_LSC)
#define SWREG_LSC_ADDR	BASE_ADDR_APB_SWREG_LSC
#else
#define SWREG_LSC_ADDR	BASE_ADDR_APB_SWREG_LSC_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define SWREG_LSC_ADDR	BASE_ADDR_APB_SWREG_LSC_ALIAS
#else
#define SWREG_LSC_ADDR	BASE_ADDR_APB_SWREG_LSC
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_TIMER0
#ifdef IP_INST_NS_TIMER0
#define HX_TIMER0_BASE 	BASE_ADDR_APB_TIMER0
#else
#define HX_TIMER0_BASE 	BASE_ADDR_APB_TIMER0_ALIAS
#endif
#else
#define HX_TIMER0_BASE 	BASE_ADDR_APB_TIMER0
#endif
#ifdef IP_INST_TIMER1
#ifdef IP_INST_NS_TIMER1
#define HX_TIMER1_BASE 	BASE_ADDR_APB_TIMER1
#else
#define HX_TIMER1_BASE 	BASE_ADDR_APB_TIMER1_ALIAS
#endif
#else
#define HX_TIMER1_BASE 	BASE_ADDR_APB_TIMER1
#endif
#ifdef IP_INST_TIMER2
#ifdef IP_INST_NS_TIMER2
#define HX_TIMER2_BASE 	BASE_ADDR_APB_TIMER2
#else
#define HX_TIMER2_BASE 	BASE_ADDR_APB_TIMER2_ALIAS
#endif
#else
#define HX_TIMER2_BASE 	BASE_ADDR_APB_TIMER2
#endif
#ifdef IP_INST_TIMER3
#ifdef IP_INST_NS_TIMER3
#define HX_TIMER3_BASE 	BASE_ADDR_APB_TIMER3
#else
#define HX_TIMER3_BASE 	BASE_ADDR_APB_TIMER3_ALIAS
#endif
#else
#define HX_TIMER3_BASE 	BASE_ADDR_APB_TIMER3
#endif
#ifdef IP_INST_TIMER4
#ifdef IP_INST_NS_TIMER4
#define HX_TIMER4_BASE 	BASE_ADDR_APB_TIMER4
#else
#define HX_TIMER4_BASE 	BASE_ADDR_APB_TIMER4_ALIAS
#endif
#else
#define HX_TIMER4_BASE 	BASE_ADDR_APB_TIMER4
#endif
#ifdef IP_INST_TIMER5
#ifdef IP_INST_NS_TIMER5
#define HX_TIMER5_BASE 	BASE_ADDR_APB_TIMER5
#else
#define HX_TIMER5_BASE 	BASE_ADDR_APB_TIMER5_ALIAS
#endif
#else
#define HX_TIMER5_BASE 	BASE_ADDR_APB_TIMER5
#endif
#ifdef IP_INST_TIMER6
#ifdef IP_INST_NS_TIMER6
#define HX_TIMER6_BASE 	BASE_ADDR_APB_TIMER6
#else
#define HX_TIMER6_BASE 	BASE_ADDR_APB_TIMER6_ALIAS
#endif
#else
#define HX_TIMER6_BASE 	BASE_ADDR_APB_TIMER6
#endif
#ifdef IP_INST_TIMER7
#ifdef IP_INST_NS_TIMER7
#define HX_TIMER7_BASE 	BASE_ADDR_APB_TIMER7
#else
#define HX_TIMER7_BASE 	BASE_ADDR_APB_TIMER7_ALIAS
#endif
#else
#define HX_TIMER7_BASE 	BASE_ADDR_APB_TIMER7
#endif
#ifdef IP_INST_TIMER8
#ifdef IP_INST_NS_TIMER8
#define HX_TIMER8_BASE 	BASE_ADDR_APB_TIMER8
#else
#define HX_TIMER8_BASE 	BASE_ADDR_APB_TIMER8_ALIAS
#endif
#else
#define HX_TIMER8_BASE 	BASE_ADDR_APB_TIMER8
#endif


#else
#ifndef TRUSTZONE
#define HX_TIMER0_BASE 	BASE_ADDR_APB_TIMER0_ALIAS
#define HX_TIMER1_BASE 	BASE_ADDR_APB_TIMER1_ALIAS
#define HX_TIMER2_BASE 	BASE_ADDR_APB_TIMER2_ALIAS
#define HX_TIMER3_BASE 	BASE_ADDR_APB_TIMER3_ALIAS
#define HX_TIMER4_BASE 	BASE_ADDR_APB_TIMER4_ALIAS
#define HX_TIMER5_BASE 	BASE_ADDR_APB_TIMER5_ALIAS
#define HX_TIMER6_BASE 	BASE_ADDR_APB_TIMER6_ALIAS
#define HX_TIMER7_BASE 	BASE_ADDR_APB_TIMER7_ALIAS
#define HX_TIMER8_BASE 	BASE_ADDR_APB_TIMER8_ALIAS
#else
#define HX_TIMER0_BASE 	BASE_ADDR_APB_TIMER0
#define HX_TIMER1_BASE 	BASE_ADDR_APB_TIMER1
#define HX_TIMER2_BASE 	BASE_ADDR_APB_TIMER2
#define HX_TIMER3_BASE 	BASE_ADDR_APB_TIMER3
#define HX_TIMER4_BASE 	BASE_ADDR_APB_TIMER4
#define HX_TIMER5_BASE 	BASE_ADDR_APB_TIMER5
#define HX_TIMER6_BASE 	BASE_ADDR_APB_TIMER6
#define HX_TIMER7_BASE 	BASE_ADDR_APB_TIMER7
#define HX_TIMER8_BASE 	BASE_ADDR_APB_TIMER8
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_NS_TPG
#define HW_TPG_BASEADDR			BASE_ADDR_HW_TPG
#else
#define HW_TPG_BASEADDR			BASE_ADDR_HW_TPG_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define HW_TPG_BASEADDR			BASE_ADDR_HW_TPG_ALIAS
#else
#define HW_TPG_BASEADDR			BASE_ADDR_HW_TPG
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_ts) || defined(IP_INST_NS_TS)
#define TS_AIP_BASEADDR             (BASE_ADDR_APB_TSCTRL)
#define TS_CTRL_BASEADDR            (BASE_ADDR_APB_TSCTRL + 0x40)
#else
#define TS_AIP_BASEADDR             (BASE_ADDR_APB_TSCTRL_ALIAS)
#define TS_CTRL_BASEADDR            (BASE_ADDR_APB_TSCTRL_ALIAS + 0x40)
#endif
#else
#ifndef TRUSTZONE
#define TS_AIP_BASEADDR             (BASE_ADDR_APB_TSCTRL_ALIAS)
#define TS_CTRL_BASEADDR            (BASE_ADDR_APB_TSCTRL_ALIAS + 0x40)
#else
#define TS_AIP_BASEADDR             (BASE_ADDR_APB_TSCTRL)
#define TS_CTRL_BASEADDR            (BASE_ADDR_APB_TSCTRL + 0x40)
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_NS_U55
#define U55_BASE_REG (BASE_ADDR_APB_U55_CTRL)
#else
#define U55_BASE_REG (BASE_ADDR_APB_U55_CTRL_ALIAS)
#endif
#else
#ifndef TRUSTZONE
#define U55_BASE_REG (BASE_ADDR_APB_U55_CTRL_ALIAS)
#else
#define U55_BASE_REG (BASE_ADDR_APB_U55_CTRL)
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_UART0
#ifdef IP_INST_NS_UART0
#define HX_UART0_BASE (BASE_ADDR_APB_UART0)
#else
#define HX_UART0_BASE (BASE_ADDR_APB_UART0_ALIAS)
#endif
#else
#define HX_UART0_BASE (BASE_ADDR_APB_UART0)
#endif
#ifdef IP_INST_UART1
#ifdef IP_INST_NS_UART1
#define HX_UART1_BASE (BASE_ADDR_APB_UART1)
#else
#define HX_UART1_BASE (BASE_ADDR_APB_UART1_ALIAS)
#endif
#else
#define HX_UART1_BASE (BASE_ADDR_APB_UART1)
#endif
#ifdef IP_INST_UART2
#ifdef IP_INST_NS_UART2
#define HX_UART2_BASE (BASE_ADDR_APB_UART2)
#else
#define HX_UART2_BASE (BASE_ADDR_APB_UART2_ALIAS)
#endif
#else
#define HX_UART2_BASE (BASE_ADDR_APB_UART2)
#endif
#else
#ifndef TRUSTZONE
#define HX_UART0_BASE (BASE_ADDR_APB_UART0_ALIAS)
#define HX_UART1_BASE (BASE_ADDR_APB_UART1_ALIAS)
#define HX_UART2_BASE (BASE_ADDR_APB_UART2_ALIAS)
#else
#define HX_UART0_BASE (BASE_ADDR_APB_UART0)
#define HX_UART1_BASE (BASE_ADDR_APB_UART1)
#define HX_UART2_BASE (BASE_ADDR_APB_UART2)
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_NS_VAD
#define HW_VAD_BASEADDR					BASE_ADDR_APB_VAD_D_TOP
#else
#define HW_VAD_BASEADDR					BASE_ADDR_APB_VAD_D_TOP_ALIAS
#endif
#else
#ifndef TRUSTZONE
#define HW_VAD_BASEADDR					BASE_ADDR_APB_VAD_D_TOP_ALIAS
#else
#define HW_VAD_BASEADDR					BASE_ADDR_APB_VAD_D_TOP
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_WDT0
#ifdef IP_INST_NS_WDT0
#define HX_WDG0_BASE 	BASE_ADDR_APB_WDT0
#else
#define HX_WDG0_BASE 	BASE_ADDR_APB_WDT0_ALIAS
#endif
#else
#define HX_WDG0_BASE 	BASE_ADDR_APB_WDT0
#endif
#ifdef IP_INST_WDT1
#ifdef IP_INST_NS_WDT1
#define HX_WDG1_BASE 	BASE_ADDR_APB_WDT1
#else
#define HX_WDG1_BASE 	BASE_ADDR_APB_WDT1_ALIAS
#endif
#else
#define HX_WDG1_BASE 	BASE_ADDR_APB_WDT1
#endif
#else
#ifndef TRUSTZONE
#define HX_WDG0_BASE 	BASE_ADDR_APB_WDT0_ALIAS
#define HX_WDG1_BASE 	BASE_ADDR_APB_WDT1_ALIAS
#else
#define HX_WDG0_BASE 	BASE_ADDR_APB_WDT0
#define HX_WDG1_BASE 	BASE_ADDR_APB_WDT1
#endif
#endif

#ifdef TRUSTZONE_SEC
#if defined(IP_INST_NS_xdma) || defined(IP_INST_NS_XDMA)
#define HW_XDMA_BASEADDR			BASE_ADDR_HW_XDMA
#define HW_XDMA_HSC_BASEADDR        (BASE_ADDR_IMAGE_PATH_STS+0x700)
#else
#define HW_XDMA_BASEADDR			BASE_ADDR_HW_XDMA_ALIAS
#define HW_XDMA_HSC_BASEADDR        (BASE_ADDR_IMAGE_PATH_STS_ALIAS+0x700)
#endif
#else
#ifndef TRUSTZONE
#define HW_XDMA_BASEADDR            BASE_ADDR_HW_XDMA_ALIAS
#define HW_XDMA_HSC_BASEADDR        (BASE_ADDR_IMAGE_PATH_STS_ALIAS+0x700)
#else
#define HW_XDMA_BASEADDR			BASE_ADDR_HW_XDMA
#define HW_XDMA_HSC_BASEADDR        (BASE_ADDR_IMAGE_PATH_STS+0x700)
#endif
#endif

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_NS_I2S_HOST
#define BASE_ADDR_I2S_HOST			(BASE_ADDR_APB_I2S_HOST)
#else
#define BASE_ADDR_I2S_HOST			(BASE_ADDR_APB_I2S_HOST_ALIAS)
#endif
#ifdef IP_INST_NS_I2S_SLAVE
#define BASE_ADDR_I2S_SLAVE			(BASE_ADDR_APB_I2S_SLAVE)
#else
#define BASE_ADDR_I2S_SLAVE			(BASE_ADDR_APB_I2S_SLAVE_ALIAS)
#endif
#else
#ifndef TRUSTZONE
#define BASE_ADDR_I2S_HOST			(BASE_ADDR_APB_I2S_HOST_ALIAS)
#define BASE_ADDR_I2S_SLAVE			(BASE_ADDR_APB_I2S_SLAVE_ALIAS)
#else
#define BASE_ADDR_I2S_HOST			(BASE_ADDR_APB_I2S_HOST)
#define BASE_ADDR_I2S_SLAVE			(BASE_ADDR_APB_I2S_SLAVE)
#endif
#endif

#define WEII_SECURE_INFO_BASE			(BASE_ADDR_SRAM0_ALIAS)//0x34000000

#define OTP_USER_START_ADDR			0x36
#define OTP_USER_END_ADDR			0xFF

#endif /* DEVICE_INC_EPII_DEVICE_ADDR_H_ */
