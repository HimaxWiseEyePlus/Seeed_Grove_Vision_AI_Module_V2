##
# platform (onchip ip) support feature
# Add all of supported ip list here
# The source code should be located in ~\drivers\{ip_name}\
##
DRIVERS_IP_LIST ?= scu \
                   mpc \
                   swreg_aon \
                   swreg_lsc \
                   ppc \
                   pmu \
                   uart \
                   timer \
                   spi \
                   gpio \
                   dma

DRIVERS_IP_INSTANCE ?= UART0 \
                       TIMER0 \
                       TIMER2 \
                       TIMER3 \
                       QSPI_HOST \
                       OSPI_HOST \
                       SSPI_HOST \
                       GPIO_G0 \
                       GPIO_G1 \
                       GPIO_G2 \
                       GPIO_G3 \
                       SB_GPIO \
                       AON_GPIO \
                       DMA1 \
                       DMA3 \
                       DMA2

#DRIVERS_IP_NS_INSTANCE ?= UART0 TIMER0 DMA3
