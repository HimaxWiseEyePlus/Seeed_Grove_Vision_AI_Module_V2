##
# platform (onchip ip) support feature
# Add all of supported ip list here
# The source code should be located in ~\drivers\{ip_name}\
##

DRIVERS_IP_LIST		?= uart \
					swreg_aon \
					swreg_lsc \
					mb \
					scu \
					timer \
					watchdog \
					rtc	\
					mpc \
					pmu \
					ppc

				
DRIVERS_IP_INSTANCE  ?= RTC0 \
						RTC1 \
						RTC2 \
						TIMER0 \
						TIMER1 \
						TIMER2 \
						TIMER3 \
						WDT0 \
						WDT1 \
						UART0 
						
DRIVERS_IP_NS_INSTANCE ?=
					