################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
TRUSTZONE_SEC_CSRC_DIR = $(EPII_ROOT)/trustzone
TRUSTZONE_SEC_ASMSRC_DIR = $(EPII_ROOT)/trustzone
TRUSTZONE_SEC_INC_DIR = $(EPII_ROOT)/trustzone 
TRUSTZONE_SEC_CXXSRC_DIR = $(EPII_ROOT)/trustzone
TRUSTZONE_SEC_CCSRC_DIR = $(EPII_ROOT)/trustzone

TRUSTZONE_SEC_CSRCS =
TRUSTZONE_SEC_ASMSRCS =
TRUSTZONE_SEC_CXXSRCS =
TRUSTZONE_SEC_CCSRCS =

TRUSTZONE_SEC_COBJS =
TRUSTZONE_SEC_ASMOBJS =
TRUSTZONE_SEC_CXXOBJS =
TRUSTZONE_SEC_CCOBJS =
TRUSTZONE_SEC_OBJS = 

TRUSTZONE_SEC_CSRCS += $(call get_csrcs, $(TRUSTZONE_SEC_CSRC_DIR))
TRUSTZONE_SEC_ASMSRCS += $(call get_asmsrcs, $(TRUSTZONE_SEC_ASMSRC_DIR))
TRUSTZONE_SEC_CXXSRCS += $(call get_cxxsrcs, $(TRUSTZONE_SEC_CXXSRC_DIR))
TRUSTZONE_SEC_CCSRCS += $(call get_ccsrcs, $(TRUSTZONE_SEC_CCSRC_DIR))

TRUSTZONE_SEC_COBJS += $(call get_relobjs, $(TRUSTZONE_SEC_CSRCS))
TRUSTZONE_SEC_ASMOBJS += $(call get_relobjs, $(TRUSTZONE_SEC_ASMSRCS))
TRUSTZONE_SEC_CXXOBJS += $(call get_relobjs, $(TRUSTZONE_SEC_CXXSRCS))
TRUSTZONE_SEC_CCOBJS += $(call get_relobjs, $(TRUSTZONE_SEC_CCSRCS))
TRUSTZONE_SEC_OBJS = $(TRUSTZONE_SEC_COBJS) $(TRUSTZONE_SEC_ASMOBJS) $(TRUSTZONE_SEC_CXXOBJS) $(TRUSTZONE_SEC_CCOBJS)

# get dependency files
TRUSTZONE_SEC_DEPS = $(call get_deps, $(TRUSTZONE_SEC_OBJS))

# extra macros to be defined
TRUSTZONE_SEC_DEFINES = -DTRUSTZONE_SEC

# generate library
TRUSTZONE_SEC_LIB_ARM = $(OUT_DIR)/libtrustzone_sec.a

# library generation rule
$(TRUSTZONE_SEC_LIB_ARM): $(TRUSTZONE_SEC_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(TRUSTZONE_SEC_OBJS)

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(TRUSTZONE_SEC_DEPS)
endif
