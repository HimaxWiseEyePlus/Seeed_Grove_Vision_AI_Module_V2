# directory declaration
LIB_INFERENCE_ENGINE_DIR = $(LIBRARIES_ROOT)/inference/torchtag1_0_0_u55tag2502
LIB_INFERENCE_ENGINE_DEFINES = -DTORCHTAG1_0_0_U55TAG2502
# LIB_INFERENCE_ENGINE_ASMSRCDIR
# use for information only rules.mk - ALL_ASMSRC_DIRS
LIB_INFERENCE_ENGINE_ASMSRCDIR	= $(LIB_INFERENCE_ENGINE_DIR)
LIB_INFERENCE_ENGINE_CSRCDIR	= $(LIB_INFERENCE_ENGINE_DIR) 
LIB_INFERENCE_ENGINE_CXXSRCSDIR    = $(LIB_INFERENCE_ENGINE_DIR)

# LIB_INFERENCE_ENGINE_INCDIR (needed) used in files.mk - INCS_DIR 
# add into ALL_INCLUDES = $(foreach dir,$(ALL_INC_DIRS),-I$(dir)) in rules.mk
# INCS_DIR		= $(OS_INCDIR) $(BOARD_INCDIR) $(LIB_INCDIR) $(BOARD_INC_DIR) $(DEVICE_INC_DIR) $(DRIVERS_INCDIR) $(EXTERNAL_INCDIR) $(LIB_BSS_DATA_INCDIR) \
				$(TRUSTZONE_SEC_INC_DIR) $(TRUSTZONE_CFG_INC_DIR) $(NSC_INC_DIR) $(MID_INCDIR) $(LIB_INFERENCE_ENGINE_INCDIR) $(LIB_AUDIOALGO_INCDIR) $(HWACCAUTOTEST_INCDIR)\
				$(SCENARIO_APP_INCDIR) $(LINKER_INC_DIR) 
LIB_INFERENCE_ENGINE_INCDIR	= $(LIB_INFERENCE_ENGINE_DIR) $(LIB_INFERENCE_ENGINE_DIR)/executorch \

				
LIB_INFERENCE_ENGINE_INCDIR  += $(LIB_INFERENCE_ENGINE_DIR)/executorch/backends/arm/third-party/ethos-u-core-driver/include \
				$(LIB_INFERENCE_ENGINE_DIR)/executorch/backends/arm/runtime \
				$(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/backend \
				$(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/core/portable_type/c10 \
				$(LIB_INFERENCE_ENGINE_DIR)/executorch/kernels \
				$(LIB_INFERENCE_ENGINE_DIR)/executorch/schema \
				$(LIB_INFERENCE_ENGINE_DIR)/third_party/schema/include \
				$(LIB_INFERENCE_ENGINE_DIR)/third_party/flatbuffers/include \
				$(LIB_INFERENCE_ENGINE_DIR)/executorch/backends/cortex_m/ops \
				$(LIB_INFERENCE_ENGINE_DIR)/third_party/cmsis-nn/Include/Internal \
				$(LIB_INFERENCE_ENGINE_DIR)/third_party/cmsis-nn/Include \
				$(LIB_INFERENCE_ENGINE_DIR)/third_party/cortex_m_ops_lib \

# find all the source files in the target directories
# LIB_INFERENCE_ENGINE_CSRCS = $(call get_csrcs, $(LIB_INFERENCE_ENGINE_CSRCDIR))
LIB_INFERENCE_ENGINE_CSRCS += \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/backends/arm/third-party/ethos-u-core-driver/src/ethosu_pmu.c \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/backends/arm/third-party/ethos-u-core-driver/src/ethosu_driver.c \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/backends/arm/third-party/ethos-u-core-driver/src/ethosu_device_u55_u65.c \

LIB_INFERENCE_ENGINE_CSRCS += \
$(wildcard $(LIB_INFERENCE_ENGINE_DIR)/third_party/cmsis-nn/Source/BasicMathFunctions/*c) \


# LIB_INFERENCE_ENGINE_CXXSRCS = $(call get_cxxsrcs, $(LIB_INFERENCE_ENGINE_CXXSRCSDIR))
LIB_INFERENCE_ENGINE_CXXSRCS += \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/executor/method_meta.cpp \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/executor/method.cpp \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/executor/program.cpp \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/executor/pte_data_map.cpp \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/executor/tensor_parser_portable.cpp \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/core/exec_aten/util/tensor_util_portable.cpp \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/core/exec_aten/util/tensor_shape_to_c_string.cpp \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/executor/tensor_parser_exec_aten.cpp \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/backends/arm/runtime/EthosUBackend.cpp \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/backends/arm/runtime/VelaBinStream.cpp \
$(LIB_INFERENCE_ENGINE_DIR)/third_party/cortex_m_ops_lib/RegisterCodegenUnboxedKernelsEverything.cpp \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/kernels/portable/cpu/util/broadcast_util.cpp \
$(LIB_INFERENCE_ENGINE_DIR)/executorch/kernels/portable/cpu/util/repeat_util.cpp \

LIB_INFERENCE_ENGINE_CXXSRCS += \
$(wildcard $(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/backend/*.cpp) \
$(wildcard $(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/core/portable_type/*.cpp) \
$(wildcard $(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/core/*.cpp) \
$(wildcard $(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/kernel/*.cpp) \
$(wildcard $(LIB_INFERENCE_ENGINE_DIR)/executorch/runtime/platform/*.cpp) \
$(wildcard $(LIB_INFERENCE_ENGINE_DIR)/executorch/schema/*.cpp) \
$(wildcard $(LIB_INFERENCE_ENGINE_DIR)/executorch/kernels/prim_ops/*.cpp) \
$(wildcard $(LIB_INFERENCE_ENGINE_DIR)/executorch/backends/cortex_m/ops/*.cpp) \
# $(wildcard $(LIB_INFERENCE_ENGINE_DIR)/executorch/kernels/portable/cpu/util/*.cpp) \
# $(wildcard $(LIB_INFERENCE_ENGINE_DIR)/executorch/kernels/quantized/cpu/*.cpp) \
# $(wildcard $(LIB_INFERENCE_ENGINE_DIR)/executorch/kernels/portable/cpu/*.cpp) \


#LIB_INFERENCE_ENGINE_ASMSRCS = $(call get_asmsrcs, $(LIB_INFERENCE_ENGINE_ASMSRCDIR))
LIB_INFERENCE_ENGINE_ASMSRCS = 

#needed by elf build
# get object files, needed by files.mk
LIB_INFERENCE_ENGINE_COBJS = $(call get_relobjs, $(LIB_INFERENCE_ENGINE_CSRCS))
LIB_INFERENCE_ENGINE_CXXOBJS = $(call get_relobjs, $(LIB_INFERENCE_ENGINE_CXXSRCS))
LIB_INFERENCE_ENGINE_ASMOBJS = $(call get_relobjs, $(LIB_INFERENCE_ENGINE_ASMSRCS))


LIB_INFERENCE_ENGINE_OBJS = $(LIB_INFERENCE_ENGINE_COBJS) $(LIB_INFERENCE_ENGINE_ASMOBJS) $(LIB_INFERENCE_ENGINE_CXXOBJS)

# get dependency files
LIB_INFERENCE_ENGINE_DEPS = $(call get_deps, $(LIB_INFERENCE_ENGINE_OBJS))

# extra macros to be defined
LIB_INFERENCE_ENGINE_DEFINES += -DC10_USING_CUSTOM_GENERATED_MACROS -D__XTENSA__ 
LIB_INFERENCE_ENGINE_DEFINES += -DETHOSU_ARCH=u55 -DETHOSU55 -DETHOSU_LOG_SEVERITY=ETHOSU_LOG_WARN -DETHOS_U 
LIB_INFERENCE_ENGINE_DEFINES += -DFalse=false
LIB_INFERENCE_ENGINE_DEFINES += -DMAX_KERNEL_NUM=4#if you have more kerel should modify the number
# LIB_INFERENCE_ENGINE_DEFINES += -D__ARM_FEATURE_MVE #open for MVE CMSIS-NN


# genearte library
INFERENCE_ENGINE_LIB_NAME = libtorchtag1_0_0_u55tag2502_$(TOOLCHAIN).a

# Middleware Definitions
INFERENCE_ENGINE_LIB_CSRCDIR += $(LIB_INFERENCE_ENGINE_CSRCDIR)
INFERENCE_ENGINE_LIB_CXXSRCDIR += $(LIB_INFERENCE_ENGINE_CXXSRCDIR)
INFERENCE_ENGINE_LIB_ASMSRCDIR += $(LIB_INFERENCE_ENGINE_ASMSRCDIR)

# definition only exist in inference_engine 
#INFERENCE_ENGINE_LIB_CSRCS += $(LIB_INFERENCE_ENGINE_CSRCS)
#INFERENCE_ENGINE_LIB_CXXSRCS += $(LIB_INFERENCE_ENGINE_CXXSRCS)
#INFERENCE_ENGINE_LIB_ASMSRCS += $(LIB_INFERENCE_ENGINE_ASMSRCS)
#INFERENCE_ENGINE_LIB_ALLSRCS += $(LIB_INFERENCE_ENGINE_CSRCS) $(LIB_INFERENCE_ENGINE_ASMSRCS)


INFERENCE_ENGINE_LIB_COBJS += $(LIB_INFERENCE_ENGINE_COBJS)
INFERENCE_ENGINE_LIB_CXXOBJS += $(LIB_INFERENCE_ENGINE_CXXOBJS)
INFERENCE_ENGINE_LIB_ASMOBJS += $(LIB_INFERENCE_ENGINE_ASMOBJS)
INFERENCE_ENGINE_LIB_ALLOBJS += $(LIB_INFERENCE_ENGINE_OBJS)

LIB_DEFINES += $(LIB_INFERENCE_ENGINE_DEFINES)
LIB_DEPS += $(LIB_INFERENCE_ENGINE_DEPS)
# INFERENCE_ENGINE_LIB_LIBS += $(LIB_LIB_INFERENCE_ENGINE)

# library generation rule
ifeq ($(INFERENCE_FORCE_PREBUILT), y)
override LIB_INFERENCE_ENGINE_OBJS:=
endif

INFERENCE_ENGINE_LIB := $(subst /,$(PS), $(strip $(OUT_DIR)/$(INFERENCE_ENGINE_LIB_NAME)))

# library generation rule
$(INFERENCE_ENGINE_LIB): $(LIB_INFERENCE_ENGINE_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_INFERENCE_ENGINE_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(INFERENCE_ENGINE_LIB_NAME) $(INFERENCE_ENGINE_LIB)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_INFERENCE_ENGINE_OBJS)
	$(CP) $(INFERENCE_ENGINE_LIB) $(PREBUILT_LIB)$(INFERENCE_ENGINE_LIB_NAME)
endif

