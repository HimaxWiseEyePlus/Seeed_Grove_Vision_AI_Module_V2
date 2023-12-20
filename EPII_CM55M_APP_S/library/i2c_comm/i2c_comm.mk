# directory declaration
LIB_I2C_COMM_DIR = $(LIBRARIES_ROOT)/i2c_comm

LIB_I2C_COMM_ASMSRCDIR	= $(LIB_I2C_COMM_DIR)
LIB_I2C_COMM_CSRCDIR	= $(LIB_I2C_COMM_DIR)
LIB_I2C_COMM_CXXSRCSDIR    = $(LIB_I2C_COMM_DIR)
LIB_I2C_COMM_INCDIR	= $(LIB_I2C_COMM_DIR)

# find all the source files in the target directories
LIB_I2C_COMM_CSRCS = $(call get_csrcs, $(LIB_I2C_COMM_CSRCDIR))
LIB_I2C_COMM_CXXSRCS = $(call get_cxxsrcs, $(LIB_I2C_COMM_CXXSRCSDIR))
LIB_I2C_COMM_ASMSRCS = $(call get_asmsrcs, $(LIB_I2C_COMM_ASMSRCDIR))

# get object files
LIB_I2C_COMM_COBJS = $(call get_relobjs, $(LIB_I2C_COMM_CSRCS))
LIB_I2C_COMM_CXXOBJS = $(call get_relobjs, $(LIB_I2C_COMM_CXXSRCS))
LIB_I2C_COMM_ASMOBJS = $(call get_relobjs, $(LIB_I2C_COMM_ASMSRCS))
LIB_I2C_COMM_OBJS = $(LIB_I2C_COMM_COBJS) $(LIB_I2C_COMM_ASMOBJS) $(LIB_I2C_COMM_CXXOBJS)

# get dependency files
LIB_I2C_COMM_DEPS = $(call get_deps, $(LIB_I2C_COMM_OBJS))

# extra macros to be defined
LIB_I2C_COMM_DEFINES = -DLIB_I2C_COMM

# genearte library
ifeq ($(I2CCOMM_LIB_FORCE_PREBUILT), y)
override LIB_I2C_COMM_OBJS:=
endif
I2C_COMM_LIB_NAME = lib_i2c_comm.a
LIB_LIB_I2C_COMM := $(subst /,$(PS), $(strip $(OUT_DIR)/$(I2C_COMM_LIB_NAME)))

# library generation rule
$(LIB_LIB_I2C_COMM): $(LIB_I2C_COMM_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_I2C_COMM_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(I2C_COMM_LIB_NAME) $(LIB_LIB_I2C_COMM)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_I2C_COMM_OBJS)
	$(CP) $(LIB_LIB_I2C_COMM) $(PREBUILT_LIB)$(I2C_COMM_LIB_NAME)
endif

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_I2C_COMM_INCDIR)
LIB_CSRCDIR += $(LIB_I2C_COMM_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_I2C_COMM_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_I2C_COMM_ASMSRCDIR)

LIB_CSRCS += $(LIB_I2C_COMM_CSRCS)
LIB_CXXSRCS += $(LIB_I2C_COMM_CXXSRCS)
LIB_ASMSRCS += $(LIB_I2C_COMM_ASMSRCS)
LIB_ALLSRCS += $(LIB_I2C_COMM_CSRCS) $(LIB_I2C_COMM_ASMSRCS)

LIB_COBJS += $(LIB_I2C_COMM_COBJS)
LIB_CXXOBJS += $(LIB_I2C_COMM_CXXOBJS)
LIB_ASMOBJS += $(LIB_I2C_COMM_ASMOBJS)
LIB_ALLOBJS += $(LIB_I2C_COMM_OBJS)

LIB_DEFINES += $(LIB_I2C_COMM_DEFINES)
LIB_DEPS += $(LIB_I2C_COMM_DEPS)
LIB_LIBS += $(LIB_LIB_I2C_COMM)