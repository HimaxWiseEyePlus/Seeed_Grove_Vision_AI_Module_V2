override SCENARIO_APP_SUPPORT_LIST := $(APP_TYPE)

APPL_DEFINES += -DWW130_CLI

#APPL_DEFINES += -DIP_xdma
#APPL_DEFINES += -DEVT_DATAPATH

#APPL_DEFINES += -DEVT_CM55MTIMER -DEVT_CM55MMB
APPL_DEFINES += -DDBG_MORE

#EVENTHANDLER_SUPPORT = event_handler
#EVENTHANDLER_SUPPORT_LIST += evt_datapath

##
# library support feature
# Add new library here
# The source code should be loacted in ~\library\{lib_name}\
##
LIB_SEL = pwrmgmt sensordp tflmtag2209_u55tag2205 spi_ptl spi_eeprom i2c_comm #hxevent
#LIB_SEL = pwrmgmt sensordp tflmtag2209_u55tag2205 spi_ptl spi_eeprom hxevent

##
# middleware support feature
# Add new middleware here
# The source code should be loacted in ~\middleware\{mid_name}\
##
MID_SEL = fatfs
#MID_SEL =
FATFS_PORT_LIST = mmc_spi
CMSIS_DRIVERS_LIST = SPI

#override OS_SEL := freertos
override OS_SEL := freertos_10_5_1
override OS_HAL := n
override MPU := n
override TRUSTZONE := y
override TRUSTZONE_TYPE := security
override TRUSTZONE_FW_TYPE := 1
override CIS_SEL := HM_COMMON
override EPII_USECASE_SEL := drv_onecore_cm55m_s

CIS_SUPPORT_INAPP = cis_sensor
#CIS_SUPPORT_INAPP_MODEL = cis_hm0360
# IMX219 for RP v1 camera
#CIS_SUPPORT_INAPP_MODEL = cis_ov5647
# IMX219 for RP v2 camera
#CIS_SUPPORT_INAPP_MODEL = cis_imx219
#CIS_SUPPORT_INAPP_MODEL = cis_imx477
# IMX708 for RP v3 camera
CIS_SUPPORT_INAPP_MODEL = cis_imx708

ifeq ($(CIS_SUPPORT_INAPP_MODEL), cis_imx219)
APPL_DEFINES += -DCIS_IMX
else ifeq ($(CIS_SUPPORT_INAPP_MODEL), cis_imx477)
APPL_DEFINES += -DCIS_IMX
else ifeq ($(CIS_SUPPORT_INAPP_MODEL), cis_imx708)
APPL_DEFINES += -DCIS_IMX
endif

$(info In ww130.mk TOOLCHAIN='${TOOLCHAIN}', SCENARIO_APP_ROOT='${SCENARIO_APP_ROOT}',  APP_TYPE='${APP_TYPE}') 

ifeq ($(strip $(TOOLCHAIN)), arm)
# CGP change: to have the name of the linker script the same as the APP_TYPE
#override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/allon_sensor_tflm.sct
# override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/$(APP_TYPE).sct
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/ww130_cli.sct
else#TOOLChain
#override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/allon_sensor_tflm.ld
# override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/$(APP_TYPE).ld
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/ww130_cli.ld
endif

$(info In ww130.mk LINKER_SCRIPT_FILE='${LINKER_SCRIPT_FILE}')
##
# Add new external device here
# The source code should be located in ~\external\{device_name}\
##
#EXT_DEV_LIST += 

$(info In ww130.mk CIS_SUPPORT_INAPP_MODEL='${CIS_SUPPORT_INAPP_MODEL}' SCENARIO_APP_INCDIR='${SCENARIO_APP_INCDIR}')


