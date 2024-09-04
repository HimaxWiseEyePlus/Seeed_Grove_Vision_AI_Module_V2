override SCENARIO_APP_SUPPORT_LIST := $(APP_TYPE)

APPL_DEFINES += -DWW_TEMPLATE

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
MID_SEL =

#override OS_SEL := freertos
override OS_SEL := freertos_10_5_1

override OS_HAL := n
override MPU := n
override TRUSTZONE := y
override TRUSTZONE_TYPE := security
override TRUSTZONE_FW_TYPE := 1
override EPII_USECASE_SEL := drv_user_defined

CIS_SUPPORT_INAPP = cis_sensor
CIS_SUPPORT_INAPP_MODEL = cis_ov5647


$(info In ww130.mk TOOLCHAIN='${TOOLCHAIN}', SCENARIO_APP_ROOT='${SCENARIO_APP_ROOT}',  APP_TYPE='${APP_TYPE}') 

ifeq ($(strip $(TOOLCHAIN)), arm)
# CGP change: to have the name of the linker script the same as the APP_TYPE
#override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/allon_sensor_tflm.sct
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/$(APP_TYPE).sct
else#TOOLChain
#override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/allon_sensor_tflm.ld
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/$(APP_TYPE).ld
endif

$(info In ww130.mk LINKER_SCRIPT_FILE='${LINKER_SCRIPT_FILE}')
##
# Add new external device here
# The source code should be located in ~\external\{device_name}\
##
#EXT_DEV_LIST += 

$(info In ww130.mk CIS_SUPPORT_INAPP_MODEL='${CIS_SUPPORT_INAPP_MODEL}' SCENARIO_APP_INCDIR='${SCENARIO_APP_INCDIR}')

