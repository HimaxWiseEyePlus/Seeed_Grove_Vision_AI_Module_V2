################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
DEVICE_CSRC_DIR = $(EPII_ROOT)/device
DEVICE_ASMSRC_DIR = $(EPII_ROOT)/device
DEVICE_INC_DIR = $(EPII_ROOT)/device  $(EPII_ROOT)/device/inc
DEVICE_CXXSRC_DIR = $(EPII_ROOT)/device
DEVICE_CCSRC_DIR = $(EPII_ROOT)/device

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

# extra macros to be defined
DEVICE_DEFINES = 

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
