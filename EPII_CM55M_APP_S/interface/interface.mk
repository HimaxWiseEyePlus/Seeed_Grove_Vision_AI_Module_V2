################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INTERFACE_CSRC_DIR = $(EPII_ROOT)/interface
INTERFACE_ASMSRC_DIR = $(EPII_ROOT)/interface
INTERFACE_INC_DIR = $(EPII_ROOT)/interface
INTERFACE_CXXSRC_DIR = $(EPII_ROOT)/interface
INTERFACE_CCSRC_DIR = $(EPII_ROOT)/interface

INTERFACE_CSRCS =
INTERFACE_ASMSRCS =
INTERFACE_CXXSRCS =
INTERFACE_CCSRCS =

INTERFACE_COBJS =
INTERFACE_ASMOBJS =
INTERFACE_CXXOBJS =
INTERFACE_CCOBJS =
INTERFACE_OBJS = 

INTERFACE_CSRCS += $(call get_csrcs, $(INTERFACE_CSRC_DIR))
INTERFACE_ASMSRCS += $(call get_asmsrcs, $(INTERFACE_ASMSRC_DIR))
INTERFACE_CXXSRCS += $(call get_cxxsrcs, $(INTERFACE_CXXSRC_DIR))
INTERFACE_CCSRCS += $(call get_ccsrcs, $(INTERFACE_CCSRC_DIR))

INTERFACE_COBJS += $(call get_relobjs, $(INTERFACE_CSRCS))
INTERFACE_ASMOBJS += $(call get_relobjs, $(INTERFACE_ASMSRCS))
INTERFACE_CXXOBJS += $(call get_relobjs, $(INTERFACE_CXXSRCS))
INTERFACE_CCOBJS += $(call get_relobjs, $(INTERFACE_CCSRCS))
INTERFACE_OBJS = $(INTERFACE_COBJS) $(INTERFACE_ASMOBJS) $(INTERFACE_CXXOBJS) $(INTERFACE_CCOBJS)

# get dependency files
INTERFACE_DEPS = $(call get_deps, $(INTERFACE_OBJS))

# extra macros to be defined
INTERFACE_DEFINES = 

# generate library
INTERFACE_LIB_ARM = $(OUT_DIR)/libinterface.a

# library generation rule
$(INTERFACE_LIB_ARM): $(INTERFACE_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(INTERFACE_OBJS)

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(INTERFACE_DEPS)
endif
