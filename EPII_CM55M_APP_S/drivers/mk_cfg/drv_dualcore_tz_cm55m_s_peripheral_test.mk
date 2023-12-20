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
					ppc \
					rtc
					
DRIVERS_IP_INSTANCE  ?= RTC0 \
						RTC1 \
						RTC2 \
						TIMER0 \
						TIMER3 \
						WDT0 \
						WDT1 \
						DMA0 \
						DMA1
						
DRIVERS_IP_NS_INSTANCE ?=
