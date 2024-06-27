# https://github.com/ARM-software/CMSIS-CV
# 2024/06/24     CMSIS-CV

# directory declaration
LIB_CMSIS_CV_DIR = $(LIBRARIES_ROOT)/cmsis_cv/CMSIS-CV

LIB_CMSIS_CV_ASMSRCDIR		= $(LIB_CMSIS_CV_DIR) $(LIB_CMSIS_CV_ASMSRC_SUBDIR)
LIB_CMSIS_CV_CSRCDIR		= $(LIB_CMSIS_CV_DIR) $(LIB_CMSIS_CV_CSRC_SUBDIR)
LIB_CMSIS_CV_CXXSRCSDIR	= $(LIB_CMSIS_CV_DIR) $(LIB_CMSIS_CV_CXXSRC_SUBDIR)
LIB_CMSIS_CV_INCDIR		= $(LIB_CMSIS_CV_DIR) $(LIB_CMSIS_CV_INC_SUBDIR)

LIB_CMSIS_CV_CSRC_SUBDIR += $(LIB_CMSIS_CV_DIR)/Source/ \
							$(LIB_CMSIS_CV_DIR)/Source/FeatureDetection/ \
							$(LIB_CMSIS_CV_DIR)/Source/LinearFilters/ \
							$(LIB_CMSIS_CV_DIR)/Source/ColorTransforms/ \
							$(LIB_CMSIS_CV_DIR)/Source/ImageTransforms/ \



LIB_CMSIS_CV_CSRCS += \
$(LIB_CMSIS_CV_CSRCDIR)/arm_cv_common.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_cannysobel.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_gaussian.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_yuv420_to_gray8.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_bgr_8U3C_to_gray8.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_gray8_to_rgb24.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_bgr_8U3C_to_rgb24.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_yuv420_to_rgb24.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_rgb24_to_gray8.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_crop_gray8.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_crop_rgb24.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_image_resize_bgr_8U3C_f32.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_image_resize_gray8_f32.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_image_resize_common_f32.c \
$(LIB_CMSIS_CV_CSRCDIR)/arm_image_resize_bgr_8U3C_to_rgb24_f32.c \

LIB_CMSIS_CV_ASMSRC_SUBDIR += 
LIB_CMSIS_CV_CXXSRC_SUBDIR +=

LIB_CMSIS_CV_INC_SUBDIR += $(LIB_CMSIS_CV_DIR)/Include \
						$(LIB_CMSIS_CV_DIR)/Include/cv \
						$(LIB_CMSIS_CV_DIR)/PrivateInclude

# find all the source files in the target directories
LIB_CMSIS_CV_CSRCS   =	$(call get_csrcs, $(LIB_CMSIS_CV_CSRCDIR))
LIB_CMSIS_CV_CXXSRCS = $(call get_cxxsrcs, $(LIB_CMSIS_CV_CXXSRCSDIR))
LIB_CMSIS_CV_ASMSRCS = $(call get_asmsrcs, $(LIB_CMSIS_CV_ASMSRCDIR))

# get object files
LIB_CMSIS_CV_COBJS   = $(call get_relobjs, $(LIB_CMSIS_CV_CSRCS))
LIB_CMSIS_CV_CXXOBJS = $(call get_relobjs, $(LIB_CMSIS_CV_CXXSRCS))
LIB_CMSIS_CV_ASMOBJS = $(call get_relobjs, $(LIB_CMSIS_CV_ASMSRCS))
LIB_CMSIS_CV_OBJS    = $(LIB_CMSIS_CV_COBJS) $(LIB_CMSIS_CV_ASMOBJS) $(LIB_CMSIS_CV_CXXOBJS)

# get dependency files
LIB_CMSIS_CV_DEPS = $(call get_deps, $(LIB_CMSIS_CV_OBJS))

# extra macros to be defined
LIB_CMSIS_CV_DEFINES = -DLIB_CMSIS_DSP -DARM_MATH_MVEI -DARM_MATH_DSP -DARM_MATH_LOOPUNROLL
# refer to https://arm-software.github.io/CMSIS-DSP/latest/
# Preprocessor Macros
# -DARM_DSP_CONFIG_TABLES
# -DARM_MATH_MVEF
# -DARM_MATH_HELIUM

# genearte library
# ifeq ($(CMSIS_CV_LIB_FORCE_PREBUILT), y)
# override LIB_CMSIS_CV_OBJS:=
# endif
CMSIS_CV_LIB_NAME = lib_cmsis_cv.a
LIB_CMSIS_CV := $(subst /,$(PS), $(strip $(OUT_DIR)/$(CMSIS_CV_LIB_NAME)))

# library generation rule
$(LIB_CMSIS_CV): $(LIB_CMSIS_CV_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_CMSIS_CV_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(CMSIS_CV_LIB_NAME) $(LIB_CMSIS_CV)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_CMSIS_CV_OBJS)
	$(CP) $(LIB_CMSIS_CV) $(PREBUILT_LIB)$(CMSIS_CV_LIB_NAME)
endif

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_CMSIS_CV_INCDIR)
LIB_CSRCDIR += $(LIB_CMSIS_CV_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_CMSIS_CV_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_CMSIS_CV_ASMSRCDIR)

LIB_CSRCS += $(LIB_CMSIS_CV_CSRCS)
LIB_CXXSRCS += $(LIB_CMSIS_CV_CXXSRCS)
LIB_ASMSRCS += $(LIB_CMSIS_CV_ASMSRCS)
LIB_ALLSRCS += $(LIB_CMSIS_CV_CSRCS) $(LIB_CMSIS_CV_ASMSRCS)

LIB_COBJS += $(LIB_CMSIS_CV_COBJS)
LIB_CXXOBJS += $(LIB_CMSIS_CV_CXXOBJS)
LIB_ASMOBJS += $(LIB_CMSIS_CV_ASMOBJS)
LIB_ALLOBJS += $(LIB_CMSIS_CV_OBJS)

LIB_DEFINES += $(LIB_CMSIS_CV_DEFINES)
LIB_DEPS += $(LIB_CMSIS_CV_DEPS)
LIB_LIBS += $(LIB_CMSIS_CV)