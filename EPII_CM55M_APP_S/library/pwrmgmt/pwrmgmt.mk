# directory declaration
LIB_PWRMGMT_DIR = $(LIBRARIES_ROOT)/pwrmgmt

LIB_PWRMGMT_ASMSRCDIR	= $(LIB_PWRMGMT_DIR)
LIB_PWRMGMT_CSRCDIR	= $(LIB_PWRMGMT_DIR)
LIB_PWRMGMT_CXXSRCSDIR    = $(LIB_PWRMGMT_DIR)
LIB_PWRMGMT_INCDIR	= $(LIB_PWRMGMT_DIR) $(LIB_PWRMGMT_DIR)/seconly_inc 

# find all the source files in the target directories
LIB_PWRMGMT_CSRCS = $(call get_csrcs, $(LIB_PWRMGMT_CSRCDIR))
LIB_PWRMGMT_CXXSRCS = $(call get_cxxsrcs, $(LIB_PWRMGMT_CXXSRCSDIR))
LIB_PWRMGMT_ASMSRCS = $(call get_asmsrcs, $(LIB_PWRMGMT_ASMSRCDIR))

# get object files
LIB_PWRMGMT_COBJS = $(call get_relobjs, $(LIB_PWRMGMT_CSRCS))
LIB_PWRMGMT_CXXOBJS = $(call get_relobjs, $(LIB_PWRMGMT_CXXSRCS))
LIB_PWRMGMT_ASMOBJS = $(call get_relobjs, $(LIB_PWRMGMT_ASMSRCS))
LIB_PWRMGMT_OBJS = $(LIB_PWRMGMT_COBJS) $(LIB_PWRMGMT_ASMOBJS) $(LIB_PWRMGMT_CXXOBJS)

# get dependency files
LIB_PWRMGMT_DEPS = $(call get_deps, $(LIB_PWRMGMT_OBJS))

# extra macros to be defined
LIB_PWRMGMT_DEFINES = -DLIB_PWRMGMT

# genearte library
ifeq ($(PWRMGMT_LIB_FORCE_PREBUILT), y)
override LIB_PWRMGMT_OBJS:=
endif
PWRMGMT_LIB_NAME = libpwrmgmt.a
LIB_LIB_PWRMGMT := $(subst /,$(PS), $(strip $(OUT_DIR)/$(PWRMGMT_LIB_NAME)))

# library generation rule
$(LIB_LIB_PWRMGMT): $(LIB_PWRMGMT_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_PWRMGMT_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(PWRMGMT_LIB_NAME) $(LIB_LIB_PWRMGMT)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_PWRMGMT_OBJS)
	$(CP) $(LIB_LIB_PWRMGMT) $(PREBUILT_LIB)$(PWRMGMT_LIB_NAME)
endif


# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_PWRMGMT_INCDIR)
LIB_CSRCDIR += $(LIB_PWRMGMT_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_PWRMGMT_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_PWRMGMT_ASMSRCDIR)

LIB_CSRCS += $(LIB_PWRMGMT_CSRCS)
LIB_CXXSRCS += $(LIB_PWRMGMT_CXXSRCS)
LIB_ASMSRCS += $(LIB_PWRMGMT_ASMSRCS)
LIB_ALLSRCS += $(LIB_PWRMGMT_CSRCS) $(LIB_PWRMGMT_ASMSRCS)

LIB_COBJS += $(LIB_PWRMGMT_COBJS)
LIB_CXXOBJS += $(LIB_PWRMGMT_CXXOBJS)
LIB_ASMOBJS += $(LIB_PWRMGMT_ASMOBJS)
LIB_ALLOBJS += $(LIB_PWRMGMT_OBJS)

LIB_DEFINES += $(LIB_PWRMGMT_DEFINES)
LIB_DEPS += $(LIB_PWRMGMT_DEPS)
LIB_LIBS += $(LIB_LIB_PWRMGMT)