################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CUSTOMER_CSRC_DIR = $(EPII_ROOT)/customer/sec_src/$(CUSTOMER)
CUSTOMER_ASMSRC_DIR = $(EPII_ROOT)/customer/sec_src/$(CUSTOMER)
CUSTOMER_INC_DIR = $(EPII_ROOT)/customer/sec_inc/$(CUSTOMER)
CUSTOMER_CXXSRC_DIR = $(EPII_ROOT)/customer/sec_src/$(CUSTOMER)
CUSTOMER_CCSRC_DIR = $(EPII_ROOT)/customer/sec_src/$(CUSTOMER)

CUSTOMER_CSRCS =
CUSTOMER_ASMSRCS =
CUSTOMER_CXXSRCS =
CUSTOMER_CCSRCS =

CUSTOMER_COBJS =
CUSTOMER_ASMOBJS =
CUSTOMER_CXXOBJS =
CUSTOMER_CCOBJS =
CUSTOMER_OBJS = 

CUSTOMER_CSRCS += $(call get_csrcs, $(CUSTOMER_CSRC_DIR))
CUSTOMER_ASMSRCS += $(call get_asmsrcs, $(CUSTOMER_ASMSRC_DIR))
CUSTOMER_CXXSRCS += $(call get_cxxsrcs, $(CUSTOMER_CXXSRC_DIR))
CUSTOMER_CCSRCS += $(call get_ccsrcs, $(CUSTOMER_CCSRC_DIR))

CUSTOMER_COBJS += $(call get_relobjs, $(CUSTOMER_CSRCS))
CUSTOMER_ASMOBJS += $(call get_relobjs, $(CUSTOMER_ASMSRCS))
CUSTOMER_CXXOBJS += $(call get_relobjs, $(CUSTOMER_CXXSRCS))
CUSTOMER_CCOBJS += $(call get_relobjs, $(CUSTOMER_CCSRCS))
CUSTOMER_OBJS = $(CUSTOMER_COBJS) $(CUSTOMER_ASMOBJS) $(CUSTOMER_CXXOBJS) $(CUSTOMER_CCOBJS)

# get dependency files
CUSTOMER_DEPS = $(call get_deps, $(CUSTOMER_OBJS))

# extra macros to be defined
ifeq "$(strip $(CUSTOMER))" ""
CUSTOMER_DEFINES = 
else
CUSTOMER_DEFINES = -D$(CUSTOMER)
endif

# genearte library
ifeq ($(CUSTOMER_FORCE_PREBUILT), y)
override CUSTOMER_OBJS:=
endif
CUSTOMER_LIB_NAME = libcustomer.a
CUSTOMER_LIB := $(subst /,$(PS), $(strip $(OUT_DIR)/$(CUSTOMER_LIB_NAME)))


ifeq "$(strip $(CUSTOMER_OBJS))" ""
ifeq "$(strip $(CUSTOMER))" ""
override CUSTOMER_LIB_NAME:=
override CUSTOMER_LIB:=
else
override PATH_PREBUILT_LIB := $(strip $(subst \,/,$(PREBUILT_LIB)))
PREBUILT_LIB_LIST = $(call get_prelibs, $(PATH_PREBUILT_LIB))
VALID_CUSTOMER_LIB_NAME = $(call check_item_exist, $(PATH_PREBUILT_LIB)$(CUSTOMER_LIB_NAME), $(PREBUILT_LIB_LIST))
ifeq "$(strip $(VALID_CUSTOMER_LIB_NAME))" ""
override CUSTOMER_LIB_NAME:=
override CUSTOMER_LIB:=
else
# library generation rule
$(CUSTOMER_LIB): $(CUSTOMER_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(CUSTOMER_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(CUSTOMER_LIB_NAME) $(CUSTOMER_LIB)
else
	$(Q)$(AR) $(AR_OPT) $@ $(CUSTOMER_OBJS)
	$(CP) $(CUSTOMER_LIB) $(PREBUILT_LIB)$(CUSTOMER_LIB_NAME)
endif
endif
endif#ifeq "$(strip $(CUSTOMER))" ""
else
# library generation rule
$(CUSTOMER_LIB): $(CUSTOMER_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(CUSTOMER_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(CUSTOMER_LIB_NAME) $(CUSTOMER_LIB)
else
	$(Q)$(AR) $(AR_OPT) $@ $(CUSTOMER_OBJS)
	$(CP) $(CUSTOMER_LIB) $(PREBUILT_LIB)$(CUSTOMER_LIB_NAME)
endif
endif

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(CUSTOMER_DEPS)
endif
