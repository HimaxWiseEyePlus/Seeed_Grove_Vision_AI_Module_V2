override LINKER_SCRIPT_GCC_ROOT = $(EPII_ROOT)/linker_script/gcc


ifeq ($(OS_SEL), freertos)
ifeq ($(TRUSTZONE), y)
ifeq ($(TRUSTZONE_TYPE), security)
ifeq ($(TRUSTZONE_FW_TYPE), 0)
override LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_GCC_ROOT)/TrustZone_S.ld
else
override LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_GCC_ROOT)/TrustZone_S_ONLY.ld
endif
else
override LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_GCC_ROOT)/TrustZone_NS.ld
endif
else#TRUSTZONE

override LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_GCC_ROOT)/NoTrustZone.ld

endif
else#baremetal
ifeq ($(TRUSTZONE), y)
ifeq ($(TRUSTZONE_TYPE), security)
ifeq ($(TRUSTZONE_FW_TYPE), 0)
override LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_GCC_ROOT)/TrustZone_S.ld
else
override LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_GCC_ROOT)/TrustZone_S_ONLY.ld
endif
else
override LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_GCC_ROOT)/TrustZone_NS.ld
endif
else#TRUSTZONE
override LINKER_SCRIPT_FILE ?= $(LINKER_SCRIPT_GCC_ROOT)/NoTrustZone.ld
endif#TRUSTZONE
endif#OS_SEL


