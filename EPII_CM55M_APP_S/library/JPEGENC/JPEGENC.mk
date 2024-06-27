# directory declaration
LIB_JPEGENC_DIR = $(LIBRARIES_ROOT)/JPEGENC

LIB_JPEGENC_ASMSRCDIR	= $(LIB_JPEGENC_DIR)
LIB_JPEGENC_CSRCDIR	= $(LIB_JPEGENC_DIR)
LIB_JPEGENC_CXXSRCSDIR    = $(LIB_JPEGENC_DIR)
LIB_JPEGENC_INCDIR	= $(LIB_JPEGENC_DIR)

# find all the source files in the target directories
LIB_JPEGENC_CSRCS = $(call get_csrcs, $(LIB_JPEGENC_CSRCDIR))
LIB_JPEGENC_CXXSRCS = $(call get_cxxsrcs, $(LIB_JPEGENC_CXXSRCSDIR))
LIB_JPEGENC_ASMSRCS = $(call get_asmsrcs, $(LIB_JPEGENC_ASMSRCDIR))

# get object files
LIB_JPEGENC_COBJS = $(call get_relobjs, $(LIB_JPEGENC_CSRCS))
LIB_JPEGENC_CXXOBJS = $(call get_relobjs, $(LIB_JPEGENC_CXXSRCS))
LIB_JPEGENC_ASMOBJS = $(call get_relobjs, $(LIB_JPEGENC_ASMSRCS))
LIB_JPEGENC_OBJS = $(LIB_JPEGENC_COBJS) $(LIB_JPEGENC_ASMOBJS) $(LIB_JPEGENC_CXXOBJS)

# get dependency files
LIB_JPEGENC_DEPS = $(call get_deps, $(LIB_JPEGENC_OBJS))

# extra macros to be defined
LIB_JPEGENC_DEFINES = -DLIB_JPEGENC

# genearte library
# ifeq ($(IMGPROC_LIB_FORCE_PREBUILT), y)
# override LIB_JPEGENC_OBJS:=
# endif
JPEGENC_LIB_NAME = lib_JPEGENC.a
LIB_LIB_JPEGENC := $(subst /,$(PS), $(strip $(OUT_DIR)/$(JPEGENC_LIB_NAME)))

# library generation rule
$(LIB_LIB_JPEGENC): $(LIB_JPEGENC_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_JPEGENC_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(JPEGENC_LIB_NAME) $(LIB_LIB_JPEGENC)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_JPEGENC_OBJS)
	$(CP) $(LIB_LIB_JPEGENC) $(PREBUILT_LIB)$(JPEGENC_LIB_NAME)
endif

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_JPEGENC_INCDIR)
LIB_CSRCDIR += $(LIB_JPEGENC_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_JPEGENC_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_JPEGENC_ASMSRCDIR)

LIB_CSRCS += $(LIB_JPEGENC_CSRCS)
LIB_CXXSRCS += $(LIB_JPEGENC_CXXSRCS)
LIB_ASMSRCS += $(LIB_JPEGENC_ASMSRCS)
LIB_ALLSRCS += $(LIB_JPEGENC_CSRCS) $(LIB_JPEGENC_ASMSRCS)

LIB_COBJS += $(LIB_JPEGENC_COBJS)
LIB_CXXOBJS += $(LIB_JPEGENC_CXXOBJS)
LIB_ASMOBJS += $(LIB_JPEGENC_ASMOBJS)
LIB_ALLOBJS += $(LIB_JPEGENC_OBJS)

LIB_DEFINES += $(LIB_JPEGENC_DEFINES)
LIB_DEPS += $(LIB_JPEGENC_DEPS)
LIB_LIBS += $(LIB_LIB_JPEGENC)