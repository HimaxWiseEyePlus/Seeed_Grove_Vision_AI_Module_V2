# directory declaration
LIB_EVENT_DIR = $(LIBRARIES_ROOT)/hxevent

LIB_EVENT_ASMSRCDIR	= $(LIB_EVENT_DIR)
LIB_EVENT_CSRCDIR	= $(LIB_EVENT_DIR)
LIB_EVENT_CXXSRCSDIR    = $(LIB_EVENT_DIR)
LIB_EVENT_INCDIR	= $(LIB_EVENT_DIR) 

# find all the source files in the target directories
LIB_EVENT_CSRCS = $(call get_csrcs, $(LIB_EVENT_CSRCDIR))
LIB_EVENT_CXXSRCS = $(call get_cxxsrcs, $(LIB_EVENT_CXXSRCSDIR))
LIB_EVENT_ASMSRCS = $(call get_asmsrcs, $(LIB_EVENT_ASMSRCDIR))

# get object files
LIB_EVENT_COBJS = $(call get_relobjs, $(LIB_EVENT_CSRCS))
LIB_EVENT_CXXOBJS = $(call get_relobjs, $(LIB_EVENT_CXXSRCS))
LIB_EVENT_ASMOBJS = $(call get_relobjs, $(LIB_EVENT_ASMSRCS))
LIB_EVENT_OBJS = $(LIB_EVENT_COBJS) $(LIB_EVENT_ASMOBJS) $(LIB_EVENT_CXXOBJS)

# get dependency files
LIB_EVENT_DEPS = $(call get_deps, $(LIB_EVENT_OBJS))

# extra macros to be defined
LIB_EVENT_DEFINES = -DLIB_EVENT

# genearte library
ifeq ($(HXEVENT_LIB_FORCE_PREBUILT), y)
override LIB_EVENT_OBJS:=
endif
HXEVENT_LIB_NAME = libhxevent.a
LIB_HXEVENT := $(subst /,$(PS), $(strip $(OUT_DIR)/$(HXEVENT_LIB_NAME)))

# library generation rule
$(LIB_HXEVENT): $(LIB_EVENT_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_EVENT_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(HXEVENT_LIB_NAME) $(LIB_HXEVENT)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_EVENT_OBJS)
	$(CP) $(LIB_HXEVENT) $(PREBUILT_LIB)$(HXEVENT_LIB_NAME)
endif
# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_EVENT_INCDIR)
LIB_CSRCDIR += $(LIB_EVENT_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_EVENT_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_EVENT_ASMSRCDIR)

LIB_CSRCS += $(LIB_EVENT_CSRCS)
LIB_CXXSRCS += $(LIB_EVENT_CXXSRCS)
LIB_ASMSRCS += $(LIB_EVENT_ASMSRCS)
LIB_ALLSRCS += $(LIB_EVENT_CSRCS) $(LIB_EVENT_ASMSRCS)

LIB_COBJS += $(LIB_EVENT_COBJS)
LIB_CXXOBJS += $(LIB_EVENT_CXXOBJS)
LIB_ASMOBJS += $(LIB_EVENT_ASMOBJS)
LIB_ALLOBJS += $(LIB_EVENT_OBJS)

LIB_DEFINES += $(LIB_EVENT_DEFINES)
LIB_DEPS += $(LIB_EVENT_DEPS)
LIB_LIBS += $(LIB_HXEVENT)