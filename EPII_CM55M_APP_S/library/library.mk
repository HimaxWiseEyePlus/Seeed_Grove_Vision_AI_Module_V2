# library root declaration
LIBRARIES_ROOT = $(EPII_ROOT)/library

ifeq ($(APP_TYPE), EPII_SIMULATOR)
LIB_REQUIRED = 
else
LIB_REQUIRED = common
endif

ifeq ($(LIB_CMSIS_NN_ENALBE), 1)
override LIB_CMSIS_NN_VERSION := $(strip $(LIB_CMSIS_NN_VERSION))
ifeq ($(LIB_CMSIS_NN_VERSION), 0)
LIB_REQUIRED +=  cmsis_nn
else
LIB_REQUIRED +=  cmsis_nn_$(LIB_CMSIS_NN_VERSION)
endif
endif

LIB_INCDIR =
LIB_CSRCDIR =
LIB_CXXSRCDIR =
LIB_ASMSRCDIR =

LIB_CSRCS =
LIB_CXXSRCS =
LIB_ASMSRCS =
LIB_ALLSRCS =

LIB_COBJS =
LIB_CXXOBJS =
LIB_ASMOBJS =
LIB_ALLOBJS =

LIB_DEFINES =
LIB_DEPS =
LIB_LIBS =

override LIB_SEL := $(strip $(LIB_SEL)) $(LIB_REQUIRED)
ifdef LIB_SEL
	LIB_SEL_SORTED = $(sort $(LIB_SEL))
	LIB_MKS = $(foreach LIB, $(LIB_SEL_SORTED), $(join $(LIB)/, $(LIB).mk))
	LIB_CV_MKS = $(foreach LIB, $(LIB_SEL_SORTED), $(join cv/$(LIB)/, $(LIB).mk))
	LIB_INFERENCE_ENGINE_MKS = $(foreach LIB, $(LIB_SEL_SORTED), $(join inference/$(LIB)/, $(LIB).mk))
	LIB_AUDIO_ALGO_MKS = $(foreach LIB, $(LIB_SEL_SORTED), $(join audio_algo/$(LIB)/, $(LIB).mk))
	LIB_CMSIS_NN_MKS = $(foreach LIB, $(LIB_SEL_SORTED), $(join cmsis_nn/$(LIB)/, $(LIB).mk))
	LIB_INCLUDES = $(foreach LIB_MK, $(LIB_MKS), $(wildcard $(addprefix $(LIBRARIES_ROOT)/, $(LIB_MK))))
	LIB_INCLUDES += $(foreach LIB_CV_MK, $(LIB_CV_MKS), $(wildcard $(addprefix $(LIBRARIES_ROOT)/, $(LIB_CV_MK))))
	LIB_INCLUDES += $(foreach LIB_AUDIO_ALGO_MK, $(LIB_AUDIO_ALGO_MKS), $(wildcard $(addprefix $(LIBRARIES_ROOT)/, $(LIB_AUDIO_ALGO_MK))))
	LIB_INCLUDES += $(foreach LIB_INFERENCE_ENGINE_MK, $(LIB_INFERENCE_ENGINE_MKS), $(wildcard $(addprefix $(LIBRARIES_ROOT)/, $(LIB_INFERENCE_ENGINE_MK))))
	LIB_INCLUDES += $(foreach LIB_CMSIS_NN_MK, $(LIB_CMSIS_NN_MKS), $(wildcard $(addprefix $(LIBRARIES_ROOT)/, $(LIB_CMSIS_NN_MK))))
	include $(LIB_INCLUDES)


	# include dependency files
	ifneq ($(MAKECMDGOALS),clean)
	-include $(LIB_DEPS)
	-include $(LIB_CV_DEPS)
	-include $(LIB_AUDIOALGO_DEPS)
	-include $(LIB_INFERENCE_ENGINE_DEPS)	
	-include $(LIB_CMSIS_NN_DEPS)		
	endif
endif


# --------- SSCMA-Micro ---------

ifeq ($(findstring sscma_micro, $(LIB_SEL)), sscma_micro)

# directory declaration
LIB_SSCMA_MICRO_DIR = $(LIBRARIES_ROOT)/sscma_micro
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/core
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/core/data
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/core/utils
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/core/engine
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/core/algorithm
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/core/synchronize
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/porting
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/porting/himax/we2
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/porting/himax/we2/boards
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/porting/himax/we2/configs
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/porting/himax/we2/drivers
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/sscma
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/sscma/callback
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/sscma/interpreter
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/sscma/repl
LIB_SSCMA_MICRO_DIR += $(LIBRARIES_ROOT)/sscma_micro/third_party/FlashDB


LIB_SSCMA_MICRO_ASMSRCDIR	= $(LIB_SSCMA_MICRO_DIR)
LIB_SSCMA_MICRO_CSRCDIR	= $(LIB_SSCMA_MICRO_DIR)
LIB_SSCMA_MICRO_CXXSRCSDIR = $(LIB_SSCMA_MICRO_DIR)
LIB_SSCMA_MICRO_INCDIR	= $(LIB_SSCMA_MICRO_DIR)

# find all the source files in the target directories
LIB_SSCMA_MICRO_CSRCS = $(call get_csrcs, $(LIB_SSCMA_MICRO_CSRCDIR))
LIB_SSCMA_MICRO_CXXSRCS = $(call get_cxxsrcs, $(LIB_SSCMA_MICRO_CXXSRCSDIR))
LIB_SSCMA_MICRO_ASMSRCS = $(call get_asmsrcs, $(LIB_SSCMA_MICRO_ASMSRCDIR))

# get object files
LIB_SSCMA_MICRO_COBJS = $(call get_relobjs, $(LIB_SSCMA_MICRO_CSRCS))
LIB_SSCMA_MICRO_CXXOBJS = $(call get_relobjs, $(LIB_SSCMA_MICRO_CXXSRCS))
LIB_SSCMA_MICRO_ASMOBJS = $(call get_relobjs, $(LIB_SSCMA_MICRO_ASMSRCS))
LIB_SSCMA_MICRO_OBJS = $(LIB_SSCMA_MICRO_COBJS) $(LIB_SSCMA_MICRO_ASMOBJS) $(LIB_SSCMA_MICRO_CXXOBJS)

$(info LIB_SSCMA_MICRO_CXXSRCS - $(LIB_SSCMA_MICRO_CXXSRCS))

# get dependency files
LIB_SSCMA_MICRO_DEPS = $(call get_deps, $(LIB_SSCMA_MICRO_OBJS))

# extra macros to be defined
TARGET ?= GROVE_VISION_AI_V2
LIB_SSCMA_MICRO_DEFINES = -DLIB_SSCMA_MICRO -DCONFIG_EL_TARGET_HIMAX 
ifeq ($(TARGET), SENSECAP_WATCHER)
	LIB_SSCMA_MICRO_DEFINES += -DCONFIG_EL_BOARD_SENSECAP_WATCHER
else
	LIB_SSCMA_MICRO_DEFINES += -DCONFIG_EL_BOARD_GROVE_VISION_AI_V2
endif


# genearte library
ifeq ($(C_LIB_FORCE_PREBUILT), y)
override LIB_SSCMA_MICRO_OBJS:=
endif
SSCMA_MICRO_LIB_NAME = libsscma_micro.a
LIB_SSCMA_MICRO := $(subst /,$(PS), $(strip $(OUT_DIR)/$(SSCMA_MICRO_LIB_NAME)))

# library generation rule
$(LIB_SSCMA_MICRO): $(LIB_SSCMA_MICRO_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_SSCMA_MICRO_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(SSCMA_MICRO_LIB_NAME) $(LIB_SSCMA_MICRO)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_SSCMA_MICRO_OBJS)
	$(CP) $(LIB_SSCMA_MICRO) $(PREBUILT_LIB)$(SSCMA_MICRO_LIB_NAME)
endif

# specific compile rules
# user can add rules to compile this library
# if not rules specified to this library, it will use default compiling rules

# library Definitions
LIB_INCDIR += $(LIB_SSCMA_MICRO_INCDIR)
LIB_CSRCDIR += $(LIB_SSCMA_MICRO_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_SSCMA_MICRO_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_SSCMA_MICRO_ASMSRCDIR)

LIB_CSRCS += $(LIB_SSCMA_MICRO_CSRCS)
LIB_CXXSRCS += $(LIB_SSCMA_MICRO_CXXSRCS)
LIB_ASMSRCS += $(LIB_SSCMA_MICRO_ASMSRCS)
LIB_ALLSRCS += $(LIB_SSCMA_MICRO_CSRCS) $(LIB_SSCMA_MICRO_ASMSRCS)

LIB_COBJS += $(LIB_SSCMA_MICRO_COBJS)
LIB_CXXOBJS += $(LIB_SSCMA_MICRO_CXXOBJS)
LIB_ASMOBJS += $(LIB_SSCMA_MICRO_ASMOBJS)
LIB_ALLOBJS += $(LIB_SSCMA_MICRO_OBJS)

LIB_DEFINES += $(LIB_SSCMA_MICRO_DEFINES)
LIB_DEPS += $(LIB_SSCMA_MICRO_DEPS)
LIB_LIBS += $(LIB_SSCMA_MICRO)

endif
