# directory declaration
LIB_INFERENCE_ENGINE_DIR = $(LIBRARIES_ROOT)/inference/tflmtag2209_u55tag2205
LIB_INFERENCE_ENGINE_DEFINES = -DTFLM2209_U55TAG2205
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
LIB_INFERENCE_ENGINE_INCDIR	= $(LIB_INFERENCE_ENGINE_DIR) $(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro \
				$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/schema \
				$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/c \
				$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/kernels/internal/reference \
				$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/kernels/internal \
				$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/kernels \
				$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite $(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/core/api \
				$(LIB_INFERENCE_ENGINE_DIR) $(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels
				
LIB_INFERENCE_ENGINE_INCDIR  += $(LIB_INFERENCE_ENGINE_DIR)/third_party/flatbuffers/include \
				$(LIB_INFERENCE_ENGINE_DIR)/third_party \
				$(LIB_INFERENCE_ENGINE_DIR)/third_party/ethos_u_core_driver/include \
				$(LIB_INFERENCE_ENGINE_DIR)/third_party/gemmlowp \
				$(LIB_INFERENCE_ENGINE_DIR)/third_party/ruy

# find all the source files in the target directories
#LIB_INFERENCE_ENGINE_CSRCS = $(call get_csrcs, $(LIB_INFERENCE_ENGINE_CSRCDIR))
# LIB_INFERENCE_ENGINE_CSRCS = $(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/c/common.c
LIB_INFERENCE_ENGINE_CSRCS = 

LIB_INFERENCE_ENGINE_CSRCS += \
$(LIB_INFERENCE_ENGINE_DIR)/third_party/ethos_u_core_driver/src/ethosu_pmu.c \
$(LIB_INFERENCE_ENGINE_DIR)/third_party/ethos_u_core_driver/src/ethosu_driver.c \
$(LIB_INFERENCE_ENGINE_DIR)/third_party/ethos_u_core_driver/src/ethosu_device_u55_u65.c
# $(LIB_INFERENCE_ENGINE_DIR)/third_party/ethos_u_core_driver/src/ethosu_driver.c

#LIB_INFERENCE_ENGINE_CXXSRCS = $(call get_cxxsrcs, $(LIB_INFERENCE_ENGINE_CXXSRCSDIR))
# $(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/cortex_m_corstone_300/system_setup.cc
# $(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/system_setup.cc 

LIB_INFERENCE_ENGINE_CXXSRCS = \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/c/common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/schema/schema_utils.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/kernels/internal/reference/portable_tensor_utils.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/kernels/internal/quantization_util.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/kernels/kernel_util.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/core/api/op_resolver.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/core/api/error_reporter.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/core/api/tensor_utils.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/core/api/flatbuffer_conversions.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/all_ops_resolver.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/debug_log.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/fake_micro_context.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/flatbuffer_utils.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/memory_helpers.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/micro_allocation_info.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/micro_allocator.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/micro_context.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/micro_error_reporter.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/micro_graph.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/micro_interpreter.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/micro_log.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/micro_profiler.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/micro_resource_variable.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/micro_string.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/micro_time.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/micro_utils.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/mock_micro_graph.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/recording_micro_allocator.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/test_helpers.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/test_helper_custom_ops.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/arena_allocator/non_persistent_arena_buffer_allocator.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/arena_allocator/persistent_arena_buffer_allocator.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/arena_allocator/recording_single_arena_buffer_allocator.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/arena_allocator/single_arena_buffer_allocator.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/cortex_m_corstone_300/micro_time.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/memory_planner/greedy_memory_planner.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/memory_planner/linear_memory_planner.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/memory_planner/non_persistent_buffer_planner_shim.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/activations.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/activations_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/add_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/add_n.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/arg_min_max.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/assign_variable.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/batch_to_space_nd.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/broadcast_args.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/broadcast_to.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/call_once.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/cast.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/ceil.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/circular_buffer.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/circular_buffer_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/comparisons.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/concatenation.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/conv_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/cumsum.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/depth_to_space.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/depthwise_conv_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/dequantize.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/dequantize_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/detection_postprocess.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/div.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/elementwise.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/elu.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/exp.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/expand_dims.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/fill.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/floor.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/floor_div.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/floor_mod.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/fully_connected_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/gather.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/gather_nd.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/hard_swish.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/hard_swish_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/if.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/kernel_runner.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/kernel_util.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/l2_pool_2d.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/l2norm.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/leaky_relu.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/leaky_relu_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/log_softmax.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/logical.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/logical_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/logistic.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/logistic_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/lstm_eval.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/maximum_minimum.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/micro_tensor_utils.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/mirror_pad.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/mul_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/neg.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/pack.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/pad.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/pooling_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/prelu.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/prelu_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/quantize.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/quantize_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/read_variable.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/reduce.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/reduce_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/reshape.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/resize_bilinear.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/resize_nearest_neighbor.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/round.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/select.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/shape.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/slice.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/softmax_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/space_to_batch_nd.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/space_to_depth.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/split.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/split_v.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/squared_difference.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/squeeze.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/strided_slice.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/sub.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/sub_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/svdf_common.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/tanh.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/transpose.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/transpose_conv.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/unidirectional_sequence_lstm.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/unpack.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/var_handle.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/while.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/zeros_like.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/ethos_u/ethosu.cc 

ifeq ($(LIB_CMSIS_NN_ENALBE), 1)
LIB_INFERENCE_ENGINE_CXXSRCS += \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/cmsis_nn/add.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/cmsis_nn/conv.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/cmsis_nn/depthwise_conv.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/cmsis_nn/fully_connected.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/cmsis_nn/mul.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/cmsis_nn/pooling.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/cmsis_nn/softmax.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/cmsis_nn/svdf.cc 

LIB_INFERENCE_ENGINE_DEFINES += -DCMSIS_NN

else
#if not use cmsis nn, unmask the function below and mask cmsis nn related information 
LIB_INFERENCE_ENGINE_CXXSRCS +=  \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/add.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/conv.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/depthwise_conv.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/fully_connected.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/mul.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/pooling.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/softmax.cc \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/kernels/svdf.cc 
endif
#$(LIB_INFERENCE_ENGINE_DIR)//home/bigcat-himax/code_we2/ntflm_1101/tensorflow/lite/micro/tools/make/downloads/ethos_u_core_platform/targets/corstone-300/retarget.c \
$(LIB_INFERENCE_ENGINE_DIR)//home/bigcat-himax/code_we2/ntflm_1101/tensorflow/lite/micro/tools/make/downloads/ethos_u_core_platform/targets/corstone-300/uart.c \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/tools/make/downloads/cmsis/Device/ARM/ARMCM55/Source/system_ARMCM55.c \
$(LIB_INFERENCE_ENGINE_DIR)/tensorflow/lite/micro/tools/make/downloads/cmsis/Device/ARM/ARMCM55/Source/startup_ARMCM55.c

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
LIB_INFERENCE_ENGINE_DEFINES +=  -DTF_LITE_STATIC_MEMORY -DTF_LITE_MCU_DEBUG_LOG  
LIB_INFERENCE_ENGINE_DEFINES += -DETHOSU_ARCH=u55 -DETHOSU55 -DETHOSU_LOG_SEVERITY=ETHOSU_LOG_WARN -DETHOS_U

# genearte library
ifeq ($(LIB_CMSIS_NN_ENALBE), 1)
INFERENCE_ENGINE_LIB_NAME = libtflmtag2209_u55tag2205_cmsisnn_$(TOOLCHAIN).a
lib_sel += cmsis_nn
else
INFERENCE_ENGINE_LIB_NAME = libtflmtag2209_u55tag2205_$(TOOLCHAIN).a
endif

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

