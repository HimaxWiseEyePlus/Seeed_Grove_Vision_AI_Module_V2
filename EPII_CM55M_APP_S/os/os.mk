##
# \defgroup	MK_OS	OS Makefile Configurations
# \brief	makefile related to operation system configurations
##
##
# OS Root
##
OSES_ROOT_DIR = $(EPII_ROOT)/os


##
# OS
# select supported os
##
SUPPORTED_OSES = freertos TFM_NS_RTX TFM_SPM freertos_10_5_1 rtos2_rtx rtos2_freertos

override OS_SEL := $(strip $(OS_SEL))



## Set Valid OS
VALID_OS_SEL = $(call check_item_exist, $(OS_SEL), $(SUPPORTED_OSES))

## Try Check OS is valid
ifeq ($(VALID_OS_SEL), )
$(info OS - $(SUPPORTED_OSES) are supported)
$(error OS $(OS_SEL) is not supported, please check it!)
endif

OS_CSRCDIR	=
OS_ASMSRCDIR	=
OS_INCDIR	=

# source files and object files
OS_CSRCS =
OS_CXXSRCS =
OS_ASMSRCS =
OS_ALLSRCS =

OS_COBJS =
OS_CXXOBJS =
OS_ASMOBJS =
OS_ALLOBJS =

OS_HAL_INCDIR =
OS_HAL_CSRCDIR =

OS_HAL_CSRCS =
OS_HAL_ALLSRCS =

OS_HAL_COBJS =
OS_HAL_ALLOBJS =

OS_HAL_DEFINES =
OS_HAL_DEPS =
OS_HAL_LIBS =

## OS Selection ##
ifeq ($(OS_SEL), freertos)
#os definition
OS_ID = OS_FREERTOS
## check MPU
override MPU := $(strip $(MPU))
override OS_HAL := $(strip $(OS_HAL))
ifeq ($(MPU), y)
OS_DEFINES += -DconfigENABLE_MPU=1
else
OS_DEFINES += -DconfigENABLE_MPU=0
endif
override OSES_ROOT_DIR := $(OSES_ROOT_DIR)/freertos
#os usage settings
ifeq ($(TRUSTZONE), y)
override TRUSTZONE_TYPE := $(strip $(TRUSTZONE_TYPE))
ifeq ($(TRUSTZONE_TYPE), security)
ifeq ($(TRUSTZONE_FW_TYPE), 0)
include $(OSES_ROOT_DIR)/TZ_Sec/TZ_Sec.mk
else
include $(OSES_ROOT_DIR)/NTZ/NTZ.mk
endif
else
include $(OSES_ROOT_DIR)/TZ_NonSec/TZ_NonSec.mk
endif
else
include $(OSES_ROOT_DIR)/NTZ/NTZ.mk
endif
else ifeq ($(OS_SEL),TFM_NS_RTX)
OS_ID = OS_TFM_NS_RTX
else ifeq ($(OS_SEL),TFM_SPM)
OS_ID = OS_TFM_SPM
else ifeq ($(OS_SEL),rtos2_rtx)
OS_ID = OS_RTOS2_RTX
override OSES_ROOT_DIR := $(OSES_ROOT_DIR)/rtos2_rtx
include $(OSES_ROOT_DIR)/rtos2_rtx.mk
else ifeq ($(OS_SEL),rtos2_freertos)
OS_ID = OS_RTOS2_FREERTOS
override OSES_ROOT_DIR := $(OSES_ROOT_DIR)/rtos2_freertos
include $(OSES_ROOT_DIR)/rtos2_freertos.mk
endif #end of freertos

ifeq ($(OS_SEL), freertos_10_5_1)
#os definition
OS_ID = OS_FREERTOS_10_5_1
## check MPU
override MPU := $(strip $(MPU))
ifeq ($(MPU), y)
OS_DEFINES += -DconfigENABLE_MPU=1
else
OS_DEFINES += -DconfigENABLE_MPU=0
endif
override OSES_ROOT_DIR := $(OSES_ROOT_DIR)/freertos_10_5_1
#os usage settings
ifeq ($(TRUSTZONE), y)
override TRUSTZONE_TYPE := $(strip $(TRUSTZONE_TYPE))
ifeq ($(TRUSTZONE_TYPE), security)
ifeq ($(TRUSTZONE_FW_TYPE), 0)
include $(OSES_ROOT_DIR)/TZ_Sec/TZ_Sec.mk
else
include $(OSES_ROOT_DIR)/NTZ/NTZ.mk
ifeq ($(OS_HAL), y)
include $(OSES_ROOT_DIR)/OS_HAL/OS_HAL.mk
endif#OS_HAL
endif
else
include $(OSES_ROOT_DIR)/TZ_NonSec/TZ_NonSec.mk
endif
else
include $(OSES_ROOT_DIR)/NTZ/NTZ.mk
ifeq ($(OS_HAL), y)
include $(OSES_ROOT_DIR)/OS_HAL/OS_HAL.mk
endif#OS_HAL
endif
endif #end of freertos

## OS HAL LAYER ##
#include $(OSES_ROOT_DIR)/hal/os_hal.mk
##
# \brief	add defines for os
##
ifdef OS_ID
OS_DEFINES += -D$(OS_ID) -DENABLE_OS
else
OS_DEFINES +=
endif


# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(OS_DEPS)
endif