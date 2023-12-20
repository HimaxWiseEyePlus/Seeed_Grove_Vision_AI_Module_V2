# directory declaration
LIB_AUDIO_DIR = $(LIBRARIES_ROOT)/audio

LIB_AUDIO_ASMSRCDIR	= $(LIB_AUDIO_DIR)
LIB_AUDIO_CSRCDIR	= $(LIB_AUDIO_DIR)
LIB_AUDIO_CXXSRCSDIR    = $(LIB_AUDIO_DIR)
LIB_AUDIO_INCDIR	= $(LIB_AUDIO_DIR) 

# find all the source files in the target directories
LIB_AUDIO_CSRCS = $(call get_csrcs, $(LIB_AUDIO_CSRCDIR))
LIB_AUDIO_CXXSRCS = $(call get_cxxsrcs, $(LIB_AUDIO_CXXSRCSDIR))
LIB_AUDIO_ASMSRCS = $(call get_asmsrcs, $(LIB_AUDIO_ASMSRCDIR))

# get object files
LIB_AUDIO_COBJS = $(call get_relobjs, $(LIB_AUDIO_CSRCS))
LIB_AUDIO_CXXOBJS = $(call get_relobjs, $(LIB_AUDIO_CXXSRCS))
LIB_AUDIO_ASMOBJS = $(call get_relobjs, $(LIB_AUDIO_ASMSRCS))
LIB_AUDIO_OBJS = $(LIB_AUDIO_COBJS) $(LIB_AUDIO_ASMOBJS) $(LIB_AUDIO_CXXOBJS)

# get dependency files
LIB_AUDIO_DEPS = $(call get_deps, $(LIB_AUDIO_OBJS))

# extra macros to be defined
LIB_AUDIO_DEFINES = -DLIB_AUDIO

# genearte library
ifeq ($(AUD_LIB_FORCE_PREBUILT), y)
override LIB_AUDIO_OBJS:=
endif
AUDIO_LIB_NAME = libaudio.a
LIB_LIB_AUDIO := $(subst /,$(PS), $(strip $(OUT_DIR)/$(AUDIO_LIB_NAME)))

# library generation rule
$(LIB_LIB_AUDIO): $(LIB_AUDIO_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_AUDIO_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(AUDIO_LIB_NAME) $(LIB_LIB_AUDIO)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_AUDIO_OBJS)
	$(CP) $(LIB_LIB_AUDIO) $(PREBUILT_LIB)$(AUDIO_LIB_NAME)
endif

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_AUDIO_INCDIR)
LIB_CSRCDIR += $(LIB_AUDIO_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_AUDIO_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_AUDIO_ASMSRCDIR)

LIB_CSRCS += $(LIB_AUDIO_CSRCS)
LIB_CXXSRCS += $(LIB_AUDIO_CXXSRCS)
LIB_ASMSRCS += $(LIB_AUDIO_ASMSRCS)
LIB_ALLSRCS += $(LIB_AUDIO_CSRCS) $(LIB_AUDIO_ASMSRCS)

LIB_COBJS += $(LIB_AUDIO_COBJS)
LIB_CXXOBJS += $(LIB_AUDIO_CXXOBJS)
LIB_ASMOBJS += $(LIB_AUDIO_ASMOBJS)
LIB_ALLOBJS += $(LIB_AUDIO_OBJS)

LIB_DEFINES += $(LIB_AUDIO_DEFINES)
LIB_DEPS += $(LIB_AUDIO_DEPS)
LIB_LIBS += $(LIB_LIB_AUDIO)