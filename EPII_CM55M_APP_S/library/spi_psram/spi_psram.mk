# directory declaration
LIB_SPI_PSRAM_DIR = $(LIBRARIES_ROOT)/spi_psram

LIB_SPI_PSRAM_ASMSRCDIR	= $(LIB_SPI_PSRAM_DIR)
LIB_SPI_PSRAM_CSRCDIR	= $(LIB_SPI_PSRAM_DIR)
LIB_SPI_PSRAM_CXXSRCSDIR    = $(LIB_SPI_PSRAM_DIR)
LIB_SPI_PSRAM_INCDIR	= $(LIB_SPI_PSRAM_DIR)

# find all the source files in the target directories
LIB_SPI_PSRAM_CSRCS = $(call get_csrcs, $(LIB_SPI_PSRAM_CSRCDIR))
LIB_SPI_PSRAM_CXXSRCS = $(call get_cxxsrcs, $(LIB_SPI_PSRAM_CXXSRCSDIR))
LIB_SPI_PSRAM_ASMSRCS = $(call get_asmsrcs, $(LIB_SPI_PSRAM_ASMSRCDIR))

# get object files
LIB_SPI_PSRAM_COBJS = $(call get_relobjs, $(LIB_SPI_PSRAM_CSRCS))
LIB_SPI_PSRAM_CXXOBJS = $(call get_relobjs, $(LIB_SPI_PSRAM_CXXSRCS))
LIB_SPI_PSRAM_ASMOBJS = $(call get_relobjs, $(LIB_SPI_PSRAM_ASMSRCS))
LIB_SPI_PSRAM_OBJS = $(LIB_SPI_PSRAM_COBJS) $(LIB_SPI_PSRAM_ASMOBJS) $(LIB_SPI_PSRAM_CXXOBJS)

# get dependency files
LIB_SPI_PSRAM_DEPS = $(call get_deps, $(LIB_SPI_PSRAM_OBJS))

# extra macros to be defined
LIB_SPI_PSRAM_DEFINES = -DLIB_SPI_PSRAM

# genearte library
ifeq ($(SPIPSRAM_LIB_FORCE_PREBUILT), y)
override LIB_SPI_PSRAM_OBJS:=
endif
SPIPSRAM_LIB_NAME = lib_spi_psram.a
LIB_LIB_SPI_PSRAM := $(subst /,$(PS), $(strip $(OUT_DIR)/$(SPIPSRAM_LIB_NAME)))

# library generation rule
$(LIB_LIB_SPI_PSRAM): $(LIB_SPI_PSRAM_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_SPI_PSRAM_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(SPIPSRAM_LIB_NAME) $(LIB_LIB_SPI_PSRAM)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_SPI_PSRAM_OBJS)
	$(CP) $(LIB_LIB_SPI_PSRAM) $(PREBUILT_LIB)$(SPIPSRAM_LIB_NAME)
endif

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_SPI_PSRAM_INCDIR)
LIB_CSRCDIR += $(LIB_SPI_PSRAM_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_SPI_PSRAM_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_SPI_PSRAM_ASMSRCDIR)

LIB_CSRCS += $(LIB_SPI_PSRAM_CSRCS)
LIB_CXXSRCS += $(LIB_SPI_PSRAM_CXXSRCS)
LIB_ASMSRCS += $(LIB_SPI_PSRAM_ASMSRCS)
LIB_ALLSRCS += $(LIB_SPI_PSRAM_CSRCS) $(LIB_SPI_PSRAM_ASMSRCS)

LIB_COBJS += $(LIB_SPI_PSRAM_COBJS)
LIB_CXXOBJS += $(LIB_SPI_PSRAM_CXXOBJS)
LIB_ASMOBJS += $(LIB_SPI_PSRAM_ASMOBJS)
LIB_ALLOBJS += $(LIB_SPI_PSRAM_OBJS)

LIB_DEFINES += $(LIB_SPI_PSRAM_DEFINES)
LIB_DEPS += $(LIB_SPI_PSRAM_DEPS)
LIB_LIBS += $(LIB_LIB_SPI_PSRAM)