# directory declaration
LIB_CLIB_DIR = $(LIBRARIES_ROOT)/clib

LIB_CLIB_ASMSRCDIR	= $(LIB_CLIB_DIR)
LIB_CLIB_CSRCDIR	= $(LIB_CLIB_DIR)
LIB_CLIB_CXXSRCSDIR    = $(LIB_CLIB_DIR)
LIB_CLIB_INCDIR	= $(LIB_CLIB_DIR)

# find all the source files in the target directories
LIB_CLIB_CSRCS = $(call get_csrcs, $(LIB_CLIB_CSRCDIR))
LIB_CLIB_CXXSRCS = $(call get_cxxsrcs, $(LIB_CLIB_CXXSRCSDIR))
LIB_CLIB_ASMSRCS = $(call get_asmsrcs, $(LIB_CLIB_ASMSRCDIR))

# get object files
LIB_CLIB_COBJS = $(call get_relobjs, $(LIB_CLIB_CSRCS))
LIB_CLIB_CXXOBJS = $(call get_relobjs, $(LIB_CLIB_CXXSRCS))
LIB_CLIB_ASMOBJS = $(call get_relobjs, $(LIB_CLIB_ASMSRCS))
LIB_CLIB_OBJS = $(LIB_CLIB_COBJS) $(LIB_CLIB_ASMOBJS) $(LIB_CLIB_CXXOBJS)

# get dependency files
LIB_CLIB_DEPS = $(call get_deps, $(LIB_CLIB_OBJS))

# extra macros to be defined
LIB_CLIB_DEFINES = -DLIB_CLIB

# genearte library
ifeq ($(C_LIB_FORCE_PREBUILT), y)
override LIB_CLIB_OBJS:=
endif
CLIB_LIB_NAME = libclib.a
LIB_CLIB := $(subst /,$(PS), $(strip $(OUT_DIR)/$(CLIB_LIB_NAME)))

# library generation rule
$(LIB_CLIB): $(LIB_CLIB_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_CLIB_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(CLIB_LIB_NAME) $(LIB_CLIB)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_CLIB_OBJS)
	$(CP) $(LIB_CLIB) $(PREBUILT_LIB)$(CLIB_LIB_NAME)
endif

# specific compile rules
# user can add rules to compile this library
# if not rules specified to this library, it will use default compiling rules

# library Definitions
LIB_INCDIR += $(LIB_CLIB_INCDIR)
LIB_CSRCDIR += $(LIB_CLIB_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_CLIB_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_CLIB_ASMSRCDIR)

LIB_CSRCS += $(LIB_CLIB_CSRCS)
LIB_CXXSRCS += $(LIB_CLIB_CXXSRCS)
LIB_ASMSRCS += $(LIB_CLIB_ASMSRCS)
LIB_ALLSRCS += $(LIB_CLIB_CSRCS) $(LIB_CLIB_ASMSRCS)

LIB_COBJS += $(LIB_CLIB_COBJS)
LIB_CXXOBJS += $(LIB_CLIB_CXXOBJS)
LIB_ASMOBJS += $(LIB_CLIB_ASMOBJS)
LIB_ALLOBJS += $(LIB_CLIB_OBJS)

LIB_DEFINES += $(LIB_CLIB_DEFINES)
LIB_DEPS += $(LIB_CLIB_DEPS)
LIB_LIBS += $(LIB_CLIB)
