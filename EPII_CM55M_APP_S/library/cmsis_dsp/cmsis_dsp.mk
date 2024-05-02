# https://github.com/ARM-software/CMSIS-DSP
# 2022/08/03     CMSIS-DSP-1.11.0

# directory declaration
LIB_CMSIS_DSP_DIR = $(LIBRARIES_ROOT)/cmsis_dsp

LIB_CMSIS_DSP_ASMSRCDIR		= $(LIB_CMSIS_DSP_DIR) $(LIB_CMSIS_DSP_ASMSRC_SUBDIR)
LIB_CMSIS_DSP_CSRCDIR		= $(LIB_CMSIS_DSP_DIR) $(LIB_CMSIS_DSP_CSRC_SUBDIR)
LIB_CMSIS_DSP_CXXSRCSDIR	= $(LIB_CMSIS_DSP_DIR) $(LIB_CMSIS_DSP_CXXSRC_SUBDIR)
LIB_CMSIS_DSP_INCDIR		= $(LIB_CMSIS_DSP_DIR) $(LIB_CMSIS_DSP_INC_SUBDIR)

LIB_CMSIS_DSP_CSRC_SUBDIR += $(LIB_CMSIS_DSP_DIR)/Source/BasicMathFunctions_Used \
							 $(LIB_CMSIS_DSP_DIR)/Source/StatisticsFunctions_Used \
							 $(LIB_CMSIS_DSP_DIR)/Source/MatrixFunctions_Used \
							 $(LIB_CMSIS_DSP_DIR)/Source/MatrixFunctions_Used \
							 $(LIB_CMSIS_DSP_DIR)/Source/ComplexMathFunctions_Used \
							 $(LIB_CMSIS_DSP_DIR)/Source/FastMathFunctions_Used \
							 $(LIB_CMSIS_DSP_DIR)/Source/CommonTables_Used \
							 $(LIB_CMSIS_DSP_DIR)/Source/TransformFunctions_Used \
							 #$(LIB_CMSIS_DSP_DIR)/Source/FilteringFunctions_Used \
						 	 #$(LIB_CMSIS_DSP_DIR)/Source/BasicMathFunctions \
							 #$(LIB_CMSIS_DSP_DIR)/Source/MatrixFunctions \
					 	 	 #$(LIB_CMSIS_DSP_DIR)/Source/FilteringFunctions \
							 #$(LIB_CMSIS_DSP_DIR)/Source/StatisticsFunctions \


LIB_CMSIS_DSP_ASMSRC_SUBDIR += 
LIB_CMSIS_DSP_CXXSRC_SUBDIR +=

LIB_CMSIS_DSP_INC_SUBDIR += $(LIB_CMSIS_DSP_DIR)/Include \
						$(LIB_CMSIS_DSP_DIR)/Include/dsp \
						$(LIB_CMSIS_DSP_DIR)/PrivateInclude \
						$(LIB_CMSIS_DSP_DIR)/ComputeLibrary/Include

# find all the source files in the target directories
LIB_CMSIS_DSP_CSRCS   =	$(call get_csrcs, $(LIB_CMSIS_DSP_CSRCDIR))
LIB_CMSIS_DSP_CXXSRCS = $(call get_cxxsrcs, $(LIB_CMSIS_DSP_CXXSRCSDIR))
LIB_CMSIS_DSP_ASMSRCS = $(call get_asmsrcs, $(LIB_CMSIS_DSP_ASMSRCDIR))

# get object files
LIB_CMSIS_DSP_COBJS   = $(call get_relobjs, $(LIB_CMSIS_DSP_CSRCS))
LIB_CMSIS_DSP_CXXOBJS = $(call get_relobjs, $(LIB_CMSIS_DSP_CXXSRCS))
LIB_CMSIS_DSP_ASMOBJS = $(call get_relobjs, $(LIB_CMSIS_DSP_ASMSRCS))
LIB_CMSIS_DSP_OBJS    = $(LIB_CMSIS_DSP_COBJS) $(LIB_CMSIS_DSP_ASMOBJS) $(LIB_CMSIS_DSP_CXXOBJS)

# get dependency files
LIB_CMSIS_DSP_DEPS = $(call get_deps, $(LIB_CMSIS_DSP_OBJS))

# extra macros to be defined
LIB_CMSIS_DSP_DEFINES = -DLIB_CMSIS_DSP -DARM_MATH_MVEI -DARM_MATH_DSP -DARM_MATH_LOOPUNROLL
# refer to https://arm-software.github.io/CMSIS-DSP/latest/
# Preprocessor Macros
# -DARM_DSP_CONFIG_TABLES
# -DARM_MATH_MVEF
# -DARM_MATH_HELIUM

# genearte library
ifeq ($(CMSIS_DSP_LIB_FORCE_PREBUILT), y)
override LIB_CMSIS_DSP_OBJS:=
endif
CMSIS_DSP_LIB_NAME = lib_cmsis_dsp.a
LIB_CMSIS_DSP := $(subst /,$(PS), $(strip $(OUT_DIR)/$(CMSIS_DSP_LIB_NAME)))

# library generation rule
$(LIB_CMSIS_DSP): $(LIB_CMSIS_DSP_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_CMSIS_DSP_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(CMSIS_DSP_LIB_NAME) $(LIB_CMSIS_DSP)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_CMSIS_DSP_OBJS)
	$(CP) $(LIB_CMSIS_DSP) $(PREBUILT_LIB)$(CMSIS_DSP_LIB_NAME)
endif

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_CMSIS_DSP_INCDIR)
LIB_CSRCDIR += $(LIB_CMSIS_DSP_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_CMSIS_DSP_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_CMSIS_DSP_ASMSRCDIR)

LIB_CSRCS += $(LIB_CMSIS_DSP_CSRCS)
LIB_CXXSRCS += $(LIB_CMSIS_DSP_CXXSRCS)
LIB_ASMSRCS += $(LIB_CMSIS_DSP_ASMSRCS)
LIB_ALLSRCS += $(LIB_CMSIS_DSP_CSRCS) $(LIB_CMSIS_DSP_ASMSRCS)

LIB_COBJS += $(LIB_CMSIS_DSP_COBJS)
LIB_CXXOBJS += $(LIB_CMSIS_DSP_CXXOBJS)
LIB_ASMOBJS += $(LIB_CMSIS_DSP_ASMOBJS)
LIB_ALLOBJS += $(LIB_CMSIS_DSP_OBJS)

LIB_DEFINES += $(LIB_CMSIS_DSP_DEFINES)
LIB_DEPS += $(LIB_CMSIS_DSP_DEPS)
LIB_LIBS += $(LIB_CMSIS_DSP)