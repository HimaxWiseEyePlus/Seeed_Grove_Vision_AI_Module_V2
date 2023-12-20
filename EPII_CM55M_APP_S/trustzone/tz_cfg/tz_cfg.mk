################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
TRUSTZONE_CFG_CSRC_DIR = $(EPII_ROOT)/trustzone/tz_cfg
TRUSTZONE_CFG_ASMSRC_DIR = $(EPII_ROOT)/trustzone/tz_cfg
TRUSTZONE_CFG_INC_DIR = $(EPII_ROOT)/trustzone/tz_cfg 
TRUSTZONE_CFG_CXXSRC_DIR = $(EPII_ROOT)/trustzone/tz_cfg
TRUSTZONE_CFG_CCSRC_DIR = $(EPII_ROOT)/trustzone/tz_cfg

TRUSTZONE_CFG_CSRCS =
TRUSTZONE_CFG_ASMSRCS =
TRUSTZONE_CFG_CXXSRCS =
TRUSTZONE_CFG_CCSRCS =

TRUSTZONE_CFG_COBJS =
TRUSTZONE_CFG_ASMOBJS =
TRUSTZONE_CFG_CXXOBJS =
TRUSTZONE_CFG_CCOBJS =
TRUSTZONE_CFG_OBJS = 

TRUSTZONE_CFG_CSRCS += $(call get_csrcs, $(TRUSTZONE_CFG_CSRC_DIR))
TRUSTZONE_CFG_ASMSRCS += $(call get_asmsrcs, $(TRUSTZONE_CFG_ASMSRC_DIR))
TRUSTZONE_CFG_CXXSRCS += $(call get_cxxsrcs, $(TRUSTZONE_CFG_CXXSRC_DIR))
TRUSTZONE_CFG_CCSRCS += $(call get_ccsrcs, $(TRUSTZONE_CFG_CCSRC_DIR))

TRUSTZONE_CFG_COBJS += $(call get_relobjs, $(TRUSTZONE_CFG_CSRCS))
TRUSTZONE_CFG_ASMOBJS += $(call get_relobjs, $(TRUSTZONE_CFG_ASMSRCS))
TRUSTZONE_CFG_CXXOBJS += $(call get_relobjs, $(TRUSTZONE_CFG_CXXSRCS))
TRUSTZONE_CFG_CCOBJS += $(call get_relobjs, $(TRUSTZONE_CFG_CCSRCS))
TRUSTZONE_CFG_OBJS = $(TRUSTZONE_CFG_COBJS) $(TRUSTZONE_CFG_ASMOBJS) $(TRUSTZONE_CFG_CXXOBJS) $(TRUSTZONE_CFG_CCOBJS)

# get dependency files
TRUSTZONE_CFG_DEPS = $(call get_deps, $(TRUSTZONE_CFG_OBJS))

# extra macros to be defined
TRUSTZONE_CFG_DEFINES = -DTRUSTZONE_CFG

# genearte library
ifeq ($(TZ_CFG_FORCE_PREBUILT), y)
override TRUSTZONE_CFG_OBJS:=
endif
TRUSTZONE_CFG_LIB_NAME = libtrustzone_cfg.a
TRUSTZONE_CFG_LIB_ARM := $(subst /,$(PS), $(strip $(OUT_DIR)/$(TRUSTZONE_CFG_LIB_NAME)))

# library generation rule
$(TRUSTZONE_CFG_LIB_ARM): $(TRUSTZONE_CFG_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(TRUSTZONE_CFG_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(TRUSTZONE_CFG_LIB_NAME) $(TRUSTZONE_CFG_LIB_ARM)
else
	$(Q)$(AR) $(AR_OPT) $@ $(TRUSTZONE_CFG_OBJS)
	$(CP) $(TRUSTZONE_CFG_LIB_ARM) $(PREBUILT_LIB)$(TRUSTZONE_CFG_LIB_NAME)
endif

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(TRUSTZONE_CFG_DEPS)
endif
