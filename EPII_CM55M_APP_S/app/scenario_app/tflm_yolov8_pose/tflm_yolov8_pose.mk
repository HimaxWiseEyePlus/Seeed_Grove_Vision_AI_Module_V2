override SCENARIO_APP_SUPPORT_LIST := $(APP_TYPE)

APPL_DEFINES += -DTFLM_YOLOV8_POSE
APPL_DEFINES += -DTF_LITE_STATIC_MEMORY
APPL_DEFINES += -Daudvidpre_ret_pll400_timer1 -DIP_xdma
APPL_DEFINES += -DEVT_DATAPATH

APPL_DEFINES += -DUART_SEND_ALOGO_RESEULT
#APPL_DEFINES += -DEVT_CM55MTIMER -DEVT_CM55MMB
APPL_DEFINES += -DDBG_MORE

EVENTHANDLER_SUPPORT = event_handler
EVENTHANDLER_SUPPORT_LIST += evt_datapath

##
# library support feature
# Add new library here
# The source code should be loacted in ~\library\{lib_name}\
##
LIB_SEL = pwrmgmt sensordp tflmtag2209_u55tag2205 spi_ptl spi_eeprom hxevent img_proc


override OS_SEL:=
override TRUSTZONE := y
override TRUSTZONE_TYPE := security
override TRUSTZONE_FW_TYPE := 1
override CIS_SEL := HM_COMMON
override EPII_USECASE_SEL := drv_user_defined

CIS_SUPPORT_INAPP = cis_sensor
# CIS_SUPPORT_INAPP_MODEL = cis_hm0360
CIS_SUPPORT_INAPP_MODEL = cis_ov5647

ifeq ($(strip $(TOOLCHAIN)), arm)
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/TFLM_yolov8_pose_S_only.sct
else#TOOLChain
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/TFLM_yolov8_pose_S_only.ld
endif
##
# Add new external device here
# The source code should be located in ~\external\{device_name}\
##
#EXT_DEV_LIST += 