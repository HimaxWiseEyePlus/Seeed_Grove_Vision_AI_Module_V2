# platform root declaration
DRIVERS_ROOT = $(EPII_ROOT)/drivers
DRIVERS_OBJS =
DRIVERS_CSRCS =
DRIVERS_ASMSRCS =
DRIVERS_CXXSRCS =
DRIVERS_CCSRCS =

DRIVERS_COBJS =
DRIVERS_ASMOBJS =
DRIVERS_CXXOBJS =
DRIVERS_CCOBJS =
DRIVERS_OBJS =

DRIVERS_INCDIR =

DRIVERS_INCDIR += $(DRIVERS_ROOT)/inc $(DRIVERS_ROOT) $(DRIVERS_ROOT)/seconly_inc

# extra macros to be defined
DRIVERS_DEFINES = 

override EPII_USECASE_SEL := $(strip $(EPII_USECASE_SEL))
-include $(EPII_ROOT)/drivers/mk_cfg/$(EPII_USECASE_SEL).mk
-include $(SCENARIO_APP_ROOT)/$(APP_TYPE)/$(EPII_USECASE_SEL).mk
# onchip ip rules
ifdef DRIVERS_IP_LIST
	override DRIVERS_IP_LIST  := $(sort $(strip $(DRIVERS_IP_LIST)))
	DRIVERS_CSRCDIR += $(foreach DRIVERS_IP, $(DRIVERS_IP_LIST), $(addprefix $(DRIVERS_ROOT)/, $(DRIVERS_IP)))
	DRIVERS_CCSRC_DIR += $(foreach DRIVERS_IP, $(DRIVERS_IP_LIST), $(addprefix $(DRIVERS_ROOT)/, $(DRIVERS_IP)))
	DRIVERS_INCDIR += $(foreach DRIVERS_IP, $(DRIVERS_IP_LIST), $(addprefix $(DRIVERS_ROOT)/, $(DRIVERS_IP)))
	DRIVERS_DEFINES += $(foreach DRIVERS_IP, $(DRIVERS_IP_LIST), $(addprefix -DIP_, $(DRIVERS_IP)))
endif

ifdef DRIVERS_IP_INSTANCE
	DRIVERS_DEFINES += $(foreach DRIVERS_IP, $(DRIVERS_IP_INSTANCE), $(addprefix -DIP_INST_, $(DRIVERS_IP)))
endif

ifdef DRIVERS_IP_NS_INSTANCE
	DRIVERS_DEFINES += $(foreach DRIVERS_IP, $(DRIVERS_IP_NS_INSTANCE), $(addprefix -DIP_INST_NS_, $(DRIVERS_IP)))
	DRIVERS_DEFINES += $(foreach DRIVERS_IP, $(DRIVERS_IP_NS_INSTANCE), $(addprefix -DIP_INST_, $(DRIVERS_IP)))
endif


# find all the source files in the target directories
DRIVERS_CSRCS += $(call get_csrcs, $(DRIVERS_CSRCDIR))
DRIVERS_ASMSRCS += $(call get_asmsrcs, $(DRIVERS_ASMSRCDIR))
DRIVERS_CXXSRCS += $(call get_cxxsrcs, $(DRIVERS_CXXSRC_DIR))
DRIVERS_CCSRCS += $(call get_ccsrcs, $(DRIVERS_CCSRC_DIR))

DRIVERS_COBJS += $(call get_relobjs, $(DRIVERS_CSRCS))
DRIVERS_ASMOBJS += $(call get_relobjs, $(DRIVERS_ASMSRCS))
DRIVERS_CXXOBJS += $(call get_relobjs, $(DRIVERS_CXXSRCS))
DRIVERS_CCOBJS += $(call get_relobjs, $(DRIVERS_CCSRCS))

DRIVERS_OBJS += $(DRIVERS_COBJS) $(DRIVERS_ASMOBJS) $(DRIVERS_CXXOBJS) $(DRIVERS_CCOBJS)

# get dependency files
DRIVERS_DEPS = $(call get_deps, $(DRIVERS_OBJS))


# library generation rule
# genearte library
ifeq ($(DRIVER_LIB_FORCE_PREBUILT), y)
override DRIVERS_OBJS:=
endif
DRIVERS_LIB_NAME = libdriver.a
DRIVERS_LIB := $(subst /,$(PS), $(strip $(OUT_DIR)/$(DRIVERS_LIB_NAME)))

# library generation rule
$(DRIVERS_LIB): $(DRIVERS_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(DRIVERS_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(DRIVERS_LIB_NAME) $(DRIVERS_LIB)
else
	$(Q)$(AR) $(AR_OPT) $@ $(DRIVERS_OBJS)
	$(CP) $(DRIVERS_LIB) $(PREBUILT_LIB)$(DRIVERS_LIB_NAME)
endif

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(DRIVERS_DEPS)
endif
