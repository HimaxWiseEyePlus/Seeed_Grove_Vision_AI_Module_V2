# library root declaration
LIBRARIES_ROOT = $(EPII_ROOT)/library

ifeq ($(APP_TYPE), EPII_SIMULATOR)
LIB_REQUIRED = 
else
ifeq ($(SEMIHOST), y)
LIB_REQUIRED = common
else
LIB_REQUIRED = common clib
endif
endif

ifeq ($(LIB_CMSIS_NN_ENALBE), 1)
LIB_REQUIRED +=  cmsis_nn
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
	LIB_INCLUDES = $(foreach LIB_MK, $(LIB_MKS), $(wildcard $(addprefix $(LIBRARIES_ROOT)/, $(LIB_MK))))
	LIB_INCLUDES += $(foreach LIB_CV_MK, $(LIB_CV_MKS), $(wildcard $(addprefix $(LIBRARIES_ROOT)/, $(LIB_CV_MK))))
	LIB_INCLUDES += $(foreach LIB_AUDIO_ALGO_MK, $(LIB_AUDIO_ALGO_MKS), $(wildcard $(addprefix $(LIBRARIES_ROOT)/, $(LIB_AUDIO_ALGO_MK))))
	LIB_INCLUDES += $(foreach LIB_INFERENCE_ENGINE_MK, $(LIB_INFERENCE_ENGINE_MKS), $(wildcard $(addprefix $(LIBRARIES_ROOT)/, $(LIB_INFERENCE_ENGINE_MK))))
	include $(LIB_INCLUDES)


	# include dependency files
	ifneq ($(MAKECMDGOALS),clean)
	-include $(LIB_DEPS)
	-include $(LIB_CV_DEPS)
	-include $(LIB_AUDIOALGO_DEPS)
	-include $(LIB_INFERENCE_ENGINE_DEPS)	
	endif
endif