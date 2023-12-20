##
# platform (onchip ip) support feature
# Add all of supported ip list here
# The source code should be located in ~\drivers\{ip_name}\
##
#DRIVERS_IP_LIST		?= 2x2

ifeq ($(APP_TYPE), CLI_TZS_TZ_AT_S_MSC_DMA) 
DRIVERS_IP_LIST		?= uart spi \
					scu \
					timer \
					dma \
					mpc \
					swreg_aon \
					ppc
else
		DRIVERS_IP_LIST		?= 2x2 \
					5x5 \
					uart spi \
					i3c_mst isp \
					swreg_aon \
					swreg_lsc \
					mb \
					scu \
					timer \
					watchdog \
					rtc	\
					clkgen \
					dma \
					mpc \
					ppc \
					iic \
					gpio \
					i2s \
					pdm \
					pmu \
					sdio
endif					


DRIVERS_IP_INSTANCE  ?= RTC0 \
						RTC1 \
						RTC2 \
						TIMER0 \
						TIMER1 \
						TIMER2 \
						TIMER3 \
						TIMER4 \
						TIMER5 \
						WDT0 \
						WDT1 \
						DMA0 \
						DMA1 \
						DMA2 \
						DMA3 \
						UART0 \
						UART1 \
						UART2 \
						IIC_HOST_SENSOR \
						IIC_HOST \
						IIC_HOST_MIPI \
						IIC_SLAVE0 \
						IIC_SLAVE1 \
						SSPI_HOST \
						QSPI_HOST \
						OSPI_HOST \
						SSPI_SLAVE \
						GPIO_G0 \
						GPIO_G1 \
						GPIO_G2 \
						GPIO_G3 \
						SB_GPIO \
						AON_GPIO \
						I2S_HOST \
						I2S_SLAVE \
						IIIC_SLAVE0 \
						IIIC_SLAVE1
						
DRIVERS_IP_NS_INSTANCE ?=
						