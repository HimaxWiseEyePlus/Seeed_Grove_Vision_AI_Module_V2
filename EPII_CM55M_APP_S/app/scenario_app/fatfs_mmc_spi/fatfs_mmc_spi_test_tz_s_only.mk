override SCENARIO_APP_SUPPORT_LIST := $(APP_TYPE)

APPL_DEFINES += -DFATFS_TEST

##
# library support feature
# Add new library here
# The source code should be loacted in ~\library\{lib_name}\
##
LIB_SEL =
##
# middleware support feature
# Add new middleware here
# The source code should be loacted in ~\middleware\{mid_name}\
##
MID_SEL = fatfs

#FATFS_PORT_LIST = mmc_spi flash ram
FATFS_PORT_LIST = mmc_spi
#FATFS_PORT_LIST = flash
#FATFS_PORT_LIST = ram

CMSIS_DRIVERS_LIST = SPI

override OS_SEL :=
override MPU := n
override TRUSTZONE := y
override TRUSTZONE_TYPE := security
override TRUSTZONE_FW_TYPE := 1
override EPII_USECASE_SEL := drv_onecore_cm55m_s

##
# Add new external device here
# The source code should be located in ~\external\{device_name}\
##
#EXT_DEV_LIST += 