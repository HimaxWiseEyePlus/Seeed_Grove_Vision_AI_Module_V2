# directory declaration
LIB_SPI_PTL_DIR = $(LIBRARIES_ROOT)/spi_ptl

LIB_SPI_PTL_ASMSRCDIR	= $(LIB_SPI_PTL_DIR)
LIB_SPI_PTL_CSRCDIR	= $(LIB_SPI_PTL_DIR)
LIB_SPI_PTL_CXXSRCSDIR    = $(LIB_SPI_PTL_DIR)
LIB_SPI_PTL_INCDIR	= $(LIB_SPI_PTL_DIR)

# find all the source files in the target directories
LIB_SPI_PTL_CSRCS = $(call get_csrcs, $(LIB_SPI_PTL_CSRCDIR))
LIB_SPI_PTL_CXXSRCS = $(call get_cxxsrcs, $(LIB_SPI_PTL_CXXSRCSDIR))
LIB_SPI_PTL_ASMSRCS = $(call get_asmsrcs, $(LIB_SPI_PTL_ASMSRCDIR))

# get object files
LIB_SPI_PTL_COBJS = $(call get_relobjs, $(LIB_SPI_PTL_CSRCS))
LIB_SPI_PTL_CXXOBJS = $(call get_relobjs, $(LIB_SPI_PTL_CXXSRCS))
LIB_SPI_PTL_ASMOBJS = $(call get_relobjs, $(LIB_SPI_PTL_ASMSRCS))
LIB_SPI_PTL_OBJS = $(LIB_SPI_PTL_COBJS) $(LIB_SPI_PTL_ASMOBJS) $(LIB_SPI_PTL_CXXOBJS)

# get dependency files
LIB_SPI_PTL_DEPS = $(call get_deps, $(LIB_SPI_PTL_OBJS))

# extra macros to be defined
LIB_SPI_PTL_DEFINES = -DLIB_SPI_PTL

# genearte library
ifeq ($(SPIPTL_LIB_FORCE_PREBUILT), y)
override LIB_SPI_PTL_OBJS:=
endif
SPIPTL_LIB_NAME = lib_spi_ptl.a
LIB_LIB_SPI_PTL := $(subst /,$(PS), $(strip $(OUT_DIR)/$(SPIPTL_LIB_NAME)))

# library generation rule
$(LIB_LIB_SPI_PTL): $(LIB_SPI_PTL_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_SPI_PTL_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(SPIPTL_LIB_NAME) $(LIB_LIB_SPI_PTL)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_SPI_PTL_OBJS)
	$(CP) $(LIB_LIB_SPI_PTL) $(PREBUILT_LIB)$(SPIPTL_LIB_NAME)
endif

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_SPI_PTL_INCDIR)
LIB_CSRCDIR += $(LIB_SPI_PTL_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_SPI_PTL_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_SPI_PTL_ASMSRCDIR)

LIB_CSRCS += $(LIB_SPI_PTL_CSRCS)
LIB_CXXSRCS += $(LIB_SPI_PTL_CXXSRCS)
LIB_ASMSRCS += $(LIB_SPI_PTL_ASMSRCS)
LIB_ALLSRCS += $(LIB_SPI_PTL_CSRCS) $(LIB_SPI_PTL_ASMSRCS)

LIB_COBJS += $(LIB_SPI_PTL_COBJS)
LIB_CXXOBJS += $(LIB_SPI_PTL_CXXOBJS)
LIB_ASMOBJS += $(LIB_SPI_PTL_ASMOBJS)
LIB_ALLOBJS += $(LIB_SPI_PTL_OBJS)

LIB_DEFINES += $(LIB_SPI_PTL_DEFINES)
LIB_DEPS += $(LIB_SPI_PTL_DEPS)
LIB_LIBS += $(LIB_LIB_SPI_PTL)