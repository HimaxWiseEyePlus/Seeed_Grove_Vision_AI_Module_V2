override LINKER_SCRIPT_ARM_ROOT = $(EPII_ROOT)/linker_script/arm

override LINKER_INC_DIR := $(LINKER_SCRIPT_ARM_ROOT)

ifeq ($(APP_TYPE), CLI_MEM_TCM_TEST) 
LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_ARM_ROOT)/TrustZone_S_ONLY_TCMTest.sct
else

ifneq ("$(findstring freertos,$(strip $(OS_SEL)))", "")
ifeq ($(TRUSTZONE), y)
ifeq ($(TRUSTZONE_TYPE), security)
ifeq ($(TRUSTZONE_FW_TYPE), 0)
LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_ARM_ROOT)/TrustZone_S.sct
else#ifeq ($(TRUSTZONE_FW_TYPE), 0)
ifeq ($(MPU), y)
LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_ARM_ROOT)/FreeRTOS_MPU_S_Only.sct
else#ifeq ($(MPU), y) n
LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_ARM_ROOT)/FreeRTOS_S_Only.sct
endif#ifeq ($(MPU), y)
endif#ifeq ($(TRUSTZONE_FW_TYPE), 0)
else#ifeq ($(TRUSTZONE_TYPE), security)
ifeq ($(MPU), y)
LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_ARM_ROOT)/FreeRTOS_MPU_NS.sct
else#ifeq ($(MPU), y)
LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_ARM_ROOT)/FreeRTOS_NS.sct
endif
endif#ifeq ($(TRUSTZONE_TYPE), security)

else#ifeq ($(TRUSTZONE), y)
ifeq ($(MPU), y)
LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_ARM_ROOT)/FreeRTOS_MPU_NOTZ.sct
else
LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_ARM_ROOT)/NoTrustZone.sct
endif
endif#ifeq ($(TRUSTZONE), y)

else#ifeq ($(OS_SEL), freertos) baremetal
ifeq ($(TRUSTZONE), y)
ifeq ($(TRUSTZONE_TYPE), security)
ifeq ($(TRUSTZONE_FW_TYPE), 0)
LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_ARM_ROOT)/TrustZone_S.sct
else#ifeq ($(TRUSTZONE_FW_TYPE), 0)
LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_ARM_ROOT)/TrustZone_S_ONLY.sct
endif#ifeq ($(TRUSTZONE_FW_TYPE), 0)
else#ifeq ($(TRUSTZONE_TYPE), security)
LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_ARM_ROOT)/TrustZone_NS.sct
endif#ifeq ($(TRUSTZONE_TYPE), security)
else#ifeq ($(TRUSTZONE), y)
LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_ARM_ROOT)/NoTrustZone.sct
endif#ifeq ($(TRUSTZONE), y)
endif#ifeq ($(OS_SEL), freertos)
endif
