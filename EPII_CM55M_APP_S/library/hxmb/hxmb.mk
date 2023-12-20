# directory declaration
LIB_HXMB_DIR = $(LIBRARIES_ROOT)/hxmb

LIB_HXMB_ASMSRCDIR	= $(LIB_HXMB_DIR)
LIB_HXMB_CSRCDIR	= $(LIB_HXMB_DIR)
LIB_HXMB_CXXSRCSDIR    = $(LIB_HXMB_DIR)
LIB_HXMB_INCDIR	= $(LIB_HXMB_DIR) 

# find all the source files in the target directories
LIB_HXMB_CSRCS = $(call get_csrcs, $(LIB_HXMB_CSRCDIR))
LIB_HXMB_CXXSRCS = $(call get_cxxsrcs, $(LIB_HXMB_CXXSRCSDIR))
LIB_HXMB_ASMSRCS = $(call get_asmsrcs, $(LIB_HXMB_ASMSRCDIR))

# get object files
LIB_HXMB_COBJS = $(call get_relobjs, $(LIB_HXMB_CSRCS))
LIB_HXMB_CXXOBJS = $(call get_relobjs, $(LIB_HXMB_CXXSRCS))
LIB_HXMB_ASMOBJS = $(call get_relobjs, $(LIB_HXMB_ASMSRCS))
LIB_HXMB_OBJS = $(LIB_HXMB_COBJS) $(LIB_HXMB_ASMOBJS) $(LIB_HXMB_CXXOBJS)

# get dependency files
LIB_HXMB_DEPS = $(call get_deps, $(LIB_HXMB_OBJS))

# extra macros to be defined
LIB_HXMB_DEFINES = -DLIB_HXMB

# genearte library
ifeq ($(HXMB_LIB_FORCE_PREBUILT), y)
override LIB_HXMB_OBJS:=
endif
HXMB_LIB_NAME = libhxmb.a
LIB_HXMB := $(subst /,$(PS), $(strip $(OUT_DIR)/$(HXMB_LIB_NAME)))

# library generation rule
$(LIB_HXMB): $(LIB_HXMB_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_HXMB_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(HXMB_LIB_NAME) $(LIB_HXMB)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_HXMB_OBJS)
	$(CP) $(LIB_HXMB) $(PREBUILT_LIB)$(HXMB_LIB_NAME)
endif
# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_HXMB_INCDIR)
LIB_CSRCDIR += $(LIB_HXMB_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_HXMB_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_HXMB_ASMSRCDIR)

LIB_CSRCS += $(LIB_HXMB_CSRCS)
LIB_CXXSRCS += $(LIB_HXMB_CXXSRCS)
LIB_ASMSRCS += $(LIB_HXMB_ASMSRCS)
LIB_ALLSRCS += $(LIB_HXMB_CSRCS) $(LIB_HXMB_ASMSRCS)

LIB_COBJS += $(LIB_HXMB_COBJS)
LIB_CXXOBJS += $(LIB_HXMB_CXXOBJS)
LIB_ASMOBJS += $(LIB_HXMB_ASMOBJS)
LIB_ALLOBJS += $(LIB_HXMB_OBJS)

LIB_DEFINES += $(LIB_HXMB_DEFINES)
LIB_DEPS += $(LIB_HXMB_DEPS)
LIB_LIBS += $(LIB_HXMB)