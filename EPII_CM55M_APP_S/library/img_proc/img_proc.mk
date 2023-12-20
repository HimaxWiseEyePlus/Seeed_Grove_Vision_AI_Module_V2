# directory declaration
LIB_IMG_PROC_DIR = $(LIBRARIES_ROOT)/img_proc

LIB_IMG_PROC_ASMSRCDIR	= $(LIB_IMG_PROC_DIR)
LIB_IMG_PROC_CSRCDIR	= $(LIB_IMG_PROC_DIR)
LIB_IMG_PROC_CXXSRCSDIR    = $(LIB_IMG_PROC_DIR)
LIB_IMG_PROC_INCDIR	= $(LIB_IMG_PROC_DIR)

# find all the source files in the target directories
LIB_IMG_PROC_CSRCS = $(call get_csrcs, $(LIB_IMG_PROC_CSRCDIR))
LIB_IMG_PROC_CXXSRCS = $(call get_cxxsrcs, $(LIB_IMG_PROC_CXXSRCSDIR))
LIB_IMG_PROC_ASMSRCS = $(call get_asmsrcs, $(LIB_IMG_PROC_ASMSRCDIR))

# get object files
LIB_IMG_PROC_COBJS = $(call get_relobjs, $(LIB_IMG_PROC_CSRCS))
LIB_IMG_PROC_CXXOBJS = $(call get_relobjs, $(LIB_IMG_PROC_CXXSRCS))
LIB_IMG_PROC_ASMOBJS = $(call get_relobjs, $(LIB_IMG_PROC_ASMSRCS))
LIB_IMG_PROC_OBJS = $(LIB_IMG_PROC_COBJS) $(LIB_IMG_PROC_ASMOBJS) $(LIB_IMG_PROC_CXXOBJS)

# get dependency files
LIB_IMG_PROC_DEPS = $(call get_deps, $(LIB_IMG_PROC_OBJS))

# extra macros to be defined
LIB_IMG_PROC_DEFINES = -DLIB_IMG_PROC

# genearte library
ifeq ($(IMGPROC_LIB_FORCE_PREBUILT), y)
override LIB_IMG_PROC_OBJS:=
endif
IMG_PROC_LIB_NAME = lib_img_proc.a
LIB_LIB_IMG_PROC := $(subst /,$(PS), $(strip $(OUT_DIR)/$(IMG_PROC_LIB_NAME)))

# library generation rule
$(LIB_LIB_IMG_PROC): $(LIB_IMG_PROC_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_IMG_PROC_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(IMG_PROC_LIB_NAME) $(LIB_LIB_IMG_PROC)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_IMG_PROC_OBJS)
	$(CP) $(LIB_LIB_IMG_PROC) $(PREBUILT_LIB)$(IMG_PROC_LIB_NAME)
endif

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_IMG_PROC_INCDIR)
LIB_CSRCDIR += $(LIB_IMG_PROC_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_IMG_PROC_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_IMG_PROC_ASMSRCDIR)

LIB_CSRCS += $(LIB_IMG_PROC_CSRCS)
LIB_CXXSRCS += $(LIB_IMG_PROC_CXXSRCS)
LIB_ASMSRCS += $(LIB_IMG_PROC_ASMSRCS)
LIB_ALLSRCS += $(LIB_IMG_PROC_CSRCS) $(LIB_IMG_PROC_ASMSRCS)

LIB_COBJS += $(LIB_IMG_PROC_COBJS)
LIB_CXXOBJS += $(LIB_IMG_PROC_CXXOBJS)
LIB_ASMOBJS += $(LIB_IMG_PROC_ASMOBJS)
LIB_ALLOBJS += $(LIB_IMG_PROC_OBJS)

LIB_DEFINES += $(LIB_IMG_PROC_DEFINES)
LIB_DEPS += $(LIB_IMG_PROC_DEPS)
LIB_LIBS += $(LIB_LIB_IMG_PROC)