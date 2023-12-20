##
# platform (onchip ip) support feature
# Add all of supported ip list here
# The source code should be located in ~\drivers\{ip_name}\
##
DRIVERS_IP_LIST		?= scu \
					dma \
					mpc \
					swreg_aon \
					swreg_lsc \
					timer \
					pmu \
					ppc
					
DRIVERS_IP_INSTANCE  ?= RTC0 \
						RTC1 \
						RTC2 \
						TIMER3 \
						WDT0 \
						WDT1 \
						DMA0 \
						DMA1 \
						DMA3 \
						IIC_HOST_SENSOR \
						IIC_HOST \
						IIC_HOST_MIPI \
						IIC_SLAVE0 \
						IIC_SLAVE1 \
						SSPI_HOST \
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

						
DRIVERS_IP_NS_INSTANCE ?= TIMER0
				
