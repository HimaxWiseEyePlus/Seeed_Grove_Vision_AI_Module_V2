# directory declaration
LIB_TPGDP_DIR = $(LIBRARIES_ROOT)/tpgdp

LIB_TPGDP_ASMSRCDIR	= $(LIB_TPGDP_DIR)
LIB_TPGDP_CSRCDIR	= $(LIB_TPGDP_DIR)
LIB_TPGDP_CXXSRCSDIR    = $(LIB_TPGDP_DIR)
LIB_TPGDP_INCDIR	= $(LIB_TPGDP_DIR)

# find all the source files in the target directories
LIB_TPGDP_CSRCS = $(call get_csrcs, $(LIB_TPGDP_CSRCDIR))
LIB_TPGDP_CXXSRCS = $(call get_cxxsrcs, $(LIB_TPGDP_CXXSRCSDIR))
LIB_TPGDP_ASMSRCS = $(call get_asmsrcs, $(LIB_TPGDP_ASMSRCDIR))

# get object files
LIB_TPGDP_COBJS = $(call get_relobjs, $(LIB_TPGDP_CSRCS))
LIB_TPGDP_CXXOBJS = $(call get_relobjs, $(LIB_TPGDP_CXXSRCS))
LIB_TPGDP_ASMOBJS = $(call get_relobjs, $(LIB_TPGDP_ASMSRCS))
LIB_TPGDP_OBJS = $(LIB_TPGDP_COBJS) $(LIB_TPGDP_ASMOBJS) $(LIB_TPGDP_CXXOBJS)

# get dependency files
LIB_TPGDP_DEPS = $(call get_deps, $(LIB_TPGDP_OBJS))

# extra macros to be defined
LIB_TPGDP_DEFINES = -DLIB_TPGDP

# genearte library
ifeq ($(TPGDP_LIB_FORCE_PREBUILT), y)
override LIB_TPGDP_OBJS:=
endif
TPGDP_LIB_NAME = libtpgdp.a
LIB_LIB_TPGDP := $(subst /,$(PS), $(strip $(OUT_DIR)/$(TPGDP_LIB_NAME)))

# library generation rule
$(LIB_LIB_TPGDP): $(LIB_TPGDP_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_TPGDP_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(TPGDP_LIB_NAME) $(LIB_LIB_TPGDP)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_TPGDP_OBJS)
	$(CP) $(LIB_LIB_TPGDP) $(PREBUILT_LIB)$(TPGDP_LIB_NAME)
endif
# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_TPGDP_INCDIR)
LIB_CSRCDIR += $(LIB_TPGDP_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_TPGDP_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_TPGDP_ASMSRCDIR)

LIB_CSRCS += $(LIB_TPGDP_CSRCS)
LIB_CXXSRCS += $(LIB_TPGDP_CXXSRCS)
LIB_ASMSRCS += $(LIB_TPGDP_ASMSRCS)
LIB_ALLSRCS += $(LIB_TPGDP_CSRCS) $(LIB_TPGDP_ASMSRCS)

LIB_COBJS += $(LIB_TPGDP_COBJS)
LIB_CXXOBJS += $(LIB_TPGDP_CXXOBJS)
LIB_ASMOBJS += $(LIB_TPGDP_ASMOBJS)
LIB_ALLOBJS += $(LIB_TPGDP_OBJS)

LIB_DEFINES += $(LIB_TPGDP_DEFINES)
LIB_DEPS += $(LIB_TPGDP_DEPS)
LIB_LIBS += $(LIB_LIB_TPGDP)