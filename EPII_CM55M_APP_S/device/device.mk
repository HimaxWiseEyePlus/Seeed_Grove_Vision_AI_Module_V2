################################################################################
# Automatically-generated file. Do not edit!
################################################################################
# extra macros to be defined
DEVICE_DEFINES = 

# Add inputs and outputs from these tool invocations to the build variables 
DEVICE_CSRC_DIR = $(EPII_ROOT)/device
DEVICE_ASMSRC_DIR = $(EPII_ROOT)/device
DEVICE_INC_DIR = $(EPII_ROOT)/device  $(EPII_ROOT)/device/inc $(EPII_ROOT)/device/clib
DEVICE_CXXSRC_DIR = $(EPII_ROOT)/device
DEVICE_CCSRC_DIR = $(EPII_ROOT)/device

ifeq ($(SEMIHOST), y)
else
DEVICE_CSRC_DIR += $(EPII_ROOT)/device/clib
ifeq ($(strip $(TOOLCHAIN)),arm)
# from Arm\Packs\Keil\ARM_Compiler\1.7.2\Source\retarget_io.c and add _sys_exit
DEVICE_CSRC_DIR += $(EPII_ROOT)/device/clib/arm
else
DEVICE_CSRC_DIR += $(EPII_ROOT)/device/clib/gnu
endif
endif

ifneq ("$(findstring freertos,$(strip $(OS_SEL)))", "")
ifeq ($(TRUSTZONE), n)
ifeq ($(strip $(TOOLCHAIN)),arm)
DEVICE_CSRC_DIR += $(EPII_ROOT)/device/clib/arm/os/freertos
else
DEVICE_CSRC_DIR += $(EPII_ROOT)/device/clib/gnu/os/freertos
endif
endif

ifeq ($(TRUSTZONE), y)
ifeq ($(TRUSTZONE_FW_TYPE), 1)
ifeq ($(strip $(TOOLCHAIN)),arm)
DEVICE_CSRC_DIR += $(EPII_ROOT)/device/clib/arm/os/freertos
else
DEVICE_CSRC_DIR += $(EPII_ROOT)/device/clib/gnu/os/freertos
endif
endif#ifeq ($(TRUSTZONE_FW_TYPE), 1)
endif#ifeq ($(TRUSTZONE), y)
endif

ifneq ("$(findstring rtx,$(strip $(OS_SEL)))", "")
ifeq ($(TRUSTZONE), n)
ifeq ($(strip $(TOOLCHAIN)),arm)
DEVICE_CSRC_DIR += $(EPII_ROOT)/device/clib/arm/os/rtx
DEVICE_DEFINES += -DRETARGET_PLATFORMPOST
else
DEVICE_CSRC_DIR += $(EPII_ROOT)/device/clib/gnu/os/rtx
endif
endif

ifeq ($(TRUSTZONE), y)
ifeq ($(TRUSTZONE_FW_TYPE), 1)
ifeq ($(strip $(TOOLCHAIN)),arm)
DEVICE_CSRC_DIR += $(EPII_ROOT)/device/clib/arm/os/rtx
DEVICE_DEFINES += -DRETARGET_PLATFORMPOST
else
DEVICE_CSRC_DIR += $(EPII_ROOT)/device/clib/gnu/os/rtx
endif
endif#ifeq ($(TRUSTZONE_FW_TYPE), 1)
endif#ifeq ($(TRUSTZONE), y)
endif

DEVICE_CSRCS =
DEVICE_ASMSRCS =
DEVICE_CXXSRCS =
DEVICE_CCSRCS =

DEVICE_COBJS =
DEVICE_ASMOBJS =
DEVICE_CXXOBJS =
DEVICE_CCOBJS =
DEVICE_OBJS = 

DEVICE_CSRCS += $(call get_csrcs, $(DEVICE_CSRC_DIR))
DEVICE_ASMSRCS += $(call get_asmsrcs, $(DEVICE_ASMSRC_DIR))
DEVICE_CXXSRCS += $(call get_cxxsrcs, $(DEVICE_CXXSRC_DIR))
DEVICE_CCSRCS += $(call get_ccsrcs, $(DEVICE_CCSRC_DIR))

DEVICE_COBJS += $(call get_relobjs, $(DEVICE_CSRCS))
DEVICE_ASMOBJS += $(call get_relobjs, $(DEVICE_ASMSRCS))
DEVICE_CXXOBJS += $(call get_relobjs, $(DEVICE_CXXSRCS))
DEVICE_CCOBJS += $(call get_relobjs, $(DEVICE_CCSRCS))
DEVICE_OBJS = $(DEVICE_COBJS) $(DEVICE_ASMOBJS) $(DEVICE_CXXOBJS) $(DEVICE_CCOBJS)


# get dependency files
DEVICE_DEPS = $(call get_deps, $(DEVICE_OBJS))



# generate library
DEVICE_LIB_ARM = $(OUT_DIR)/libdevice.a

# library generation rule
$(DEVICE_LIB_ARM): $(DEVICE_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(DEVICE_OBJS)

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(DEVICE_DEPS)
endif
