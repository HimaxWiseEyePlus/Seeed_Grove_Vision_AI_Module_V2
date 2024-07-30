override SCENARIO_APP_SUPPORT_LIST := $(APP_TYPE)

APPL_DEFINES += -DKWS_PDM_RECORD#-DTFLM_NOTZ
APPL_DEFINES += -DTF_LITE_STATIC_MEMORY
APPL_DEFINES += -DETHOSU_ARCH=u55
APPL_DEFINES += -DETHOSU55
APPL_DEFINES += -DETHOSU_LOG_SEVERITY=ETHOSU_LOG_WARN
APPL_DEFINES += -DMODEL_ETHOSU55 # use ethosu op
APPL_DEFINES += -DIP_xdma
APPL_DEFINES += -DDBG_MORE


APPL_DEFINES += -DKWS_MODEL_VELA=1 # if only use ethous u55

APPL_DEFINES += -DRECORD=0
APPL_DEFINES += -DTOTAL_TICK=1
APPL_DEFINES += -DTOTAL_TICK_Without_data_loader=0
APPL_DEFINES += -DU55_PMU_ENABLE=0

##
# library support feature
# Add new library here
# The source code should be loacted in ~\library\{lib_name}\
##
LIB_SEL = pwrmgmt sensordp tflmtag2209_u55tag2205 spi_ptl spi_eeprom hxevent cmsis_dsp
# middleware support feature
# Add new middleware here
# The source code should be loacted in ~\middleware\{mid_name}\
##
FATFS_PORT_LIST = mmc_spi
CMSIS_DRIVERS_LIST = SPI

MID_SEL = 

override OS_SEL:=
override TRUSTZONE := y
override TRUSTZONE_TYPE := security
override TRUSTZONE_FW_TYPE := 1
override CIS_SEL := HM_COMMON
override EPII_USECASE_SEL := drv_user_defined

ifeq ($(strip $(TOOLCHAIN)), arm)
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/kws_pdm_record.sct
else#TOOLChain
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/kws_pdm_record.ld
endif

# ifeq ("$(ALGO_TYPE)","TEST_CV_ALGO")
# LIB_SEL += test_cv_algo
# else #default algo
# endif
# LIB_SEL += tflmtag2209_u55tag2205 #spi_eeprom spi_ptl
##
# Add new external device here
# The source code should be located in ~\external\{device_name}\
##
#EXT_DEV_LIST += 