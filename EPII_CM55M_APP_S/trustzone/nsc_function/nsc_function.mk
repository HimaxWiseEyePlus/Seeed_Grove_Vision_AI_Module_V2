# Add inputs and outputs from these tool invocations to the build variables 
NSC_ROOT_DIR = $(EPII_ROOT)/trustzone/nsc_function
NSC_ROOT_SRC_DIR = $(NSC_ROOT_DIR)/nsc_src
NSC_CSRC_DIR = $(NSC_ROOT_DIR) $(NSC_ROOT_SRC_DIR)
NSC_ASMSRC_DIR = $(NSC_ROOT_DIR) $(NSC_ROOT_SRC_DIR)
NSC_INC_DIR = $(NSC_ROOT_DIR)  $(NSC_ROOT_DIR)/nsc_inc
NSC_CXXSRC_DIR = $(NSC_ROOT_DIR) $(NSC_ROOT_SRC_DIR)
NSC_CCSRC_DIR = $(NSC_ROOT_DIR) $(NSC_ROOT_SRC_DIR)

# extra macros to be defined
ifeq ($(TRUSTZONE), y)
ifeq ($(TRUSTZONE_TYPE), security)
NSC_DEFINES =
else
NSC_DEFINES = -DNSC
endif
endif

override EPII_NSC_SEL := $(strip $(EPII_NSC_SEL))
-include $(NSC_ROOT_DIR)/nsc_cfg/$(EPII_NSC_SEL).mk


ifdef NSC_IP_LIST
	override NSC_IP_LIST  := $(sort $(strip $(NSC_IP_LIST)))
	NSC_CSRC_DIR += $(foreach NSC_IP, $(NSC_IP_LIST), $(addprefix $(NSC_ROOT_SRC_DIR)/, $(NSC_IP)))
	NSC_ASMSRC_DIR += $(foreach NSC_IP, $(NSC_IP_LIST), $(addprefix $(NSC_ROOT_SRC_DIR)/, $(NSC_IP)))
	NSC_INC_DIR += $(foreach NSC_IP, $(NSC_IP_LIST), $(addprefix $(NSC_ROOT_SRC_DIR)/, $(NSC_IP)))
	NSC_CXXSRC_DIR += $(foreach NSC_IP, $(NSC_IP_LIST), $(addprefix $(NSC_ROOT_SRC_DIR)/, $(NSC_IP)))
	NSC_CCSRC_DIR += $(foreach NSC_IP, $(NSC_IP_LIST), $(addprefix $(NSC_ROOT_SRC_DIR)/, $(NSC_IP)))

	NSC_DEFINES += $(foreach NSC_IP, $(NSC_IP_LIST), $(addprefix -DNSC_, $(NSC_IP)))
endif

NSC_CSRCS =
NSC_ASMSRCS =
NSC_CXXSRCS =
NSC_CCSRCS =

NSC_COBJS =
NSC_ASMOBJS =
NSC_CXXOBJS =
NSC_CCOBJS =
NSC_OBJS = 

-include $(NSC_ROOT_DIR)/nsc_customer/nsc_customer.mk

NSC_INC_DIR += $(NSC_CUSTOMER_INC_DIR)

NSC_CSRC_DIR += $(NSC_CUSTOMER_CSRC_DIR)
NSC_ASMSRC_DIR += $(NSC_CUSTOMER_ASMSRC_DIR)
NSC_INC_DIR += $(NSC_CUSTOMER_CUSTOMER_INC_DIR)
NSC_CXXSRC_DIR += $(NSC_CUSTOMER_CXXSRC_DIR)
NSC_CCSRC_DIR += $(NSC_CUSTOMER_CCSRC_DIR)

NSC_CSRCS += $(call get_csrcs, $(NSC_CSRC_DIR))
NSC_ASMSRCS += $(call get_asmsrcs, $(NSC_ASMSRC_DIR))
NSC_CXXSRCS += $(call get_cxxsrcs, $(NSC_CXXSRC_DIR))
NSC_CCSRCS += $(call get_ccsrcs, $(NSC_CCSRC_DIR))

NSC_COBJS += $(call get_relobjs, $(NSC_CSRCS))
NSC_ASMOBJS += $(call get_relobjs, $(NSC_ASMSRCS))
NSC_CXXOBJS += $(call get_relobjs, $(NSC_CXXSRCS))
NSC_CCOBJS += $(call get_relobjs, $(NSC_CCSRCS))
NSC_OBJS = $(NSC_COBJS) $(NSC_ASMOBJS) $(NSC_CXXOBJS) $(NSC_CCOBJS)




# get dependency files
NSC_DEPS = $(call get_deps, $(NSC_OBJS))

# genearte library
ifdef NSC_IP_LIST
ifeq ($(NSC_CFG_FORCE_PREBUILT), y)
override NSC_OBJS:=
endif
NSC_LIB_NAME = libnsc.a
NSC_LIB := $(subst /,$(PS), $(strip $(OUT_DIR)/$(NSC_LIB_NAME)))

# library generation rule
$(NSC_LIB): $(NSC_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(TRUSTZONE_CFG_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(NSC_LIB_NAME) $(NSC_LIB)
else
	$(Q)$(AR) $(AR_OPT) $@ $(NSC_OBJS)
	$(CP) $(NSC_LIB) $(PREBUILT_LIB)$(NSC_LIB_NAME)
endif
else
NSC_LIB :=
endif

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(NSC_DEPS)
endif
