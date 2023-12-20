# devices root declaration
EXTERNAL_ROOT = $(EPII_ROOT)/external
EXTERNAL_INCDIR += $(EXTERNAL_ROOT)/inc $(EXTERNAL_ROOT)
# extra macros to be defined
EXTERNAL_DEFINES = 
EXTERNAL_OBJS =
##
# Add new external device here
# The source code should be located in ~\external\{device_name}\
##

ifeq ($(CIS_SEL), HM_COMMON)
EXT_DEV_LIST += cis/hm_common
endif
ifeq ($(CIS_SEL), OV02C10)
EXT_DEV_LIST += cis/hm_common
APPL_DEFINES += -DCIS_OV02C10
endif
ifeq ($(CIS_SEL), SOIK351P)
EXT_DEV_LIST += cis/hm_common
APPL_DEFINES += -DCIS_SOIK351P
endif
ifeq ($(CIS_SEL), HM5532)
EXT_DEV_LIST += cis/hm_common
APPL_DEFINES += -DCIS_HM5532
endif
ifeq ($(CIS_SEL), HM1245)
EXT_DEV_LIST += cis/hm_common
EXT_DEV_LIST += cis/hm1245
endif
ifeq ($(CIS_SEL), HM01B0_MONO) 
EXT_DEV_LIST += cis/hm_common
EXT_DEV_LIST += cis/hm01b0
endif
ifeq ($(CIS_SEL), HM01B0_BAYER)
EXT_DEV_LIST += cis/hm_common 
EXT_DEV_LIST += cis/hm01b0
endif
ifeq ($(CIS_SEL), HM2140)
EXT_DEV_LIST += cis/hm_common
EXT_DEV_LIST += cis/hm2140
endif
ifeq ($(CIS_SEL), HM2130)
EXT_DEV_LIST += cis/hm_common
APPL_DEFINES += -DCIS_HM2130
endif
ifeq ($(CIS_SEL), HM2170)
EXT_DEV_LIST += cis/hm_common
APPL_DEFINES += -DCIS_HM2170
endif
ifeq ($(CIS_SEL), HM1246)
EXT_DEV_LIST += cis/hm_common
APPL_DEFINES += -DCIS_HM1246
endif
ifeq ($(CIS_SEL), HM0435)
EXT_DEV_LIST += cis/hm_common
APPL_DEFINES += -DCIS_HM0435
endif
ifeq ($(CIS_SEL), HM2056)
EXT_DEV_LIST += cis/hm_common
APPL_DEFINES += -DCIS_HM2056
endif
#HM0360_MONO
ifeq ($(CIS_SEL), HM0360_MONO)
EXT_DEV_LIST += cis/hm_common
EXT_DEV_LIST += cis/hm0360
endif
#HM0360_MONO_OSC
ifeq ($(CIS_SEL), HM0360_MONO_OSC)
EXT_DEV_LIST += cis/hm_common
EXT_DEV_LIST += cis/hm0360
endif
#HM0360_BAYER
ifeq ($(CIS_SEL), HM0360_BAYER)
EXT_DEV_LIST += cis/hm_common
EXT_DEV_LIST += cis/hm0360
endif
#HM0360_BAYER_OSC
ifeq ($(CIS_SEL), HM0360_BAYER_OSC)
EXT_DEV_LIST += cis/hm_common
EXT_DEV_LIST += cis/hm0360
endif
#HM11B1
ifeq ($(CIS_SEL), HM11B1)
EXT_DEV_LIST += cis/hm_common
EXT_DEV_LIST += cis/hm11b1
endif
#HM11B1_REVC
ifeq ($(CIS_SEL), HM11B1_REVC)
EXT_DEV_LIST += cis/hm_common
EXT_DEV_LIST += cis/hm11b1_revc
endif
#HM0360_MONO_REVB
ifeq ($(CIS_SEL), HM0360_MONO_REVB)
EXT_DEV_LIST += cis/hm_common
EXT_DEV_LIST += cis/hm0360_revb
endif
#HM0360_MONO_OSC
ifeq ($(CIS_SEL), HM0360_MONO_OSC_REVB)
EXT_DEV_LIST += cis/hm_common
EXT_DEV_LIST += cis/hm0360_revb
endif
#HM0360_BAYER
ifeq ($(CIS_SEL), HM0360_BAYER_REVB)
EXT_DEV_LIST += cis/hm_common
EXT_DEV_LIST += cis/hm0360_revb
endif
#HM0360_BAYER_OSC
ifeq ($(CIS_SEL), HM0360_BAYER_OSC_REVB)
EXT_DEV_LIST += cis/hm_common
EXT_DEV_LIST += cis/hm0360_revb
endif

# onchip ip rules
ifdef EXT_DEV_LIST
	override EXT_DEV_LIST  := $(sort $(strip $(EXT_DEV_LIST)))
	EXTERNAL_CSRCDIR += $(foreach EXT_DEVS_IP, $(EXT_DEV_LIST), $(addprefix $(EXTERNAL_ROOT)/, $(EXT_DEVS_IP)))
	EXTERNAL_INCDIR += $(foreach EXT_DEVS_IP, $(EXT_DEV_LIST), $(addprefix $(EXTERNAL_ROOT)/, $(EXT_DEVS_IP)))
	EXT_DEVS_MKS = $(foreach EXT_DEVS, $(EXT_DEV_LIST), $(join $(EXTERNAL_ROOT)/$(EXT_DEVS)/, $(notdir $(EXT_DEVS)).mk))
	include $(EXT_DEVS_MKS)
endif

# find all the source files in the target directories
EXTERNAL_CSRCS += $(call get_csrcs, $(EXTERNAL_CSRCDIR))
EXTERNAL_ASMSRCS += $(call get_asmsrcs, $(EXTERNAL_ASMSRCDIR))
EXTERNAL_CXXSRCS += $(call get_cxxsrcs, $(EXTERNAL_CXXSRC_DIR))
EXTERNAL_CCSRCS += $(call get_ccsrcs, $(EXTERNAL_CCSRC_DIR))

EXTERNAL_COBJS += $(call get_relobjs, $(EXTERNAL_CSRCS))
EXTERNAL_ASMOBJS += $(call get_relobjs, $(EXTERNAL_ASMSRCS))
EXTERNAL_CXXOBJS += $(call get_relobjs, $(EXTERNAL_CXXSRCS))
EXTERNAL_CCOBJS += $(call get_relobjs, $(EXTERNAL_CCSRCS))

EXTERNAL_OBJS += $(EXTERNAL_COBJS) $(EXTERNAL_ASMOBJS) $(EXTERNAL_CXXOBJS) $(EXTERNAL_CCOBJS)

# get dependency files
EXTERNAL_DEPS = $(call get_deps, $(EXTERNAL_OBJS))



# generate library
ifdef EXT_DEV_LIST
ifeq ($(EXTERNAL_LIB_FORCE_PREBUILT), y)
override EXTERNAL_OBJS:=
endif
EXTERNAL_LIB_NAME = libextdevice.a 
EXTERNAL_LIB := $(subst /,$(PS), $(strip $(OUT_DIR)/$(EXTERNAL_LIB_NAME)))

# library generation rule
$(EXTERNAL_LIB): $(EXTERNAL_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(EXTERNAL_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(EXTERNAL_LIB_NAME) $(EXTERNAL_LIB)
else
	$(Q)$(AR) $(AR_OPT) $@ $(EXTERNAL_OBJS)
	$(CP) $(EXTERNAL_LIB) $(PREBUILT_LIB)$(EXTERNAL_LIB_NAME)
endif
else
EXTERNAL_LIB :=
endif

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(EXTERNAL_DEPS)
endif
