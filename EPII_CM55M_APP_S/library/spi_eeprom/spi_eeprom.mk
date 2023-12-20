# directory declaration
LIB_SPI_EEPROM_DIR = $(LIBRARIES_ROOT)/spi_eeprom

LIB_SPI_EEPROM_ASMSRCDIR	= $(LIB_SPI_EEPROM_DIR)
LIB_SPI_EEPROM_CSRCDIR	    = $(LIB_SPI_EEPROM_DIR)
LIB_SPI_EEPROM_CXXSRCSDIR   = $(LIB_SPI_EEPROM_DIR)
LIB_SPI_EEPROM_INCDIR	    = $(LIB_SPI_EEPROM_DIR)

LIB_SPI_EEPROM_INCDIR += $(LIBRARIES_ROOT)/spi_eeprom/eeprom_param

# find all the source files in the target directories
LIB_SPI_EEPROM_CSRCS = $(call get_csrcs, $(LIB_SPI_EEPROM_CSRCDIR))
LIB_SPI_EEPROM_CXXSRCS = $(call get_cxxsrcs, $(LIB_SPI_EEPROM_CXXSRCSDIR))
LIB_SPI_EEPROM_ASMSRCS = $(call get_asmsrcs, $(LIB_SPI_EEPROM_ASMSRCDIR))

# get object files
LIB_SPI_EEPROM_COBJS = $(call get_relobjs, $(LIB_SPI_EEPROM_CSRCS))
LIB_SPI_EEPROM_CXXOBJS = $(call get_relobjs, $(LIB_SPI_EEPROM_CXXSRCS))
LIB_SPI_EEPROM_ASMOBJS = $(call get_relobjs, $(LIB_SPI_EEPROM_ASMSRCS))
LIB_SPI_EEPROM_OBJS = $(LIB_SPI_EEPROM_COBJS) $(LIB_SPI_EEPROM_ASMOBJS) $(LIB_SPI_EEPROM_CXXOBJS)

# get dependency files
LIB_SPI_EEPROM_DEPS = $(call get_deps, $(LIB_SPI_EEPROM_OBJS))

# extra macros to be defined
LIB_SPI_EEPROM_DEFINES = -DLIB_SPI_EEPROM

LIB_SPI_EEPROM_DEFINES += -DLIB_QSPI_EEPROM
# OSPI eeprom function doesn't be verified.
# LIB_SPI_EEPROM_DEFINES += -DLIB_OSPI_EEPROM
# LIB_SPI_EEPROM_DEFINES += -DLIB_SSPI_EEPROM  

# LIB_SPI_EEPROM_USE_SINGLE_FLASH support y or n
# This flag is to determine whether to support single or multiple flash instruction tables
# if this flag is y and FLASH_SEL has multiple flash model, only frist flash model be selected.
LIB_SPI_EEPROM_USE_SINGLE_FLASH ?= y
# If FLASH_SEL has multiple flash model.
# The priority order of the default flash command table: Winbond > MXIC > GIGADEV
# And then Larger flash capacity is a priority.

# If FLASH_SEL ?= USER_DEFINE
# default flash command table will be NULL, user need to use hx_lib_spi_eeprom_set_flash_config to set flash command table.
# The default flash 
FLASH_SEL ?= WB_25Q128JW
# FLASH_SEL ?= WB_25Q128JW WB_25Q64JW WB_25Q32JW WB_25Q16JW \
# 			MX_25U12843 MX_25U6432 MX_25U3232 MX_25U1632 \
# 			GD_25LQ128 GD_25LQ64 GD_25LQ32 GD_25LQ16

ifeq ($(LIB_SPI_EEPROM_USE_SINGLE_FLASH), y)
DEV_FLASH = $(word 1, $(strip $(FLASH_SEL)))
LIB_SPI_EEPROM_DEFINES += $(addprefix -DSPI_EEPROM_USE_, $(addsuffix _INST_, $(DEV_FLASH)))
else
LIB_SPI_EEPROM_DEFINES += $(foreach DEV_FLASH, $(FLASH_SEL), $(addprefix -DSPI_EEPROM_USE_, $(addsuffix _INST_, $(DEV_FLASH))))
endif


# genearte library
ifeq ($(SPIEEPROM_LIB_FORCE_PREBUILT), y)
override LIB_SPI_EEPROM_OBJS:=
endif
SPIEEPROM_LIB_NAME = lib_spi_eeprom.a
LIB_LIB_SPI_EEPROM := $(subst /,$(PS), $(strip $(OUT_DIR)/$(SPIEEPROM_LIB_NAME)))

# library generation rule
$(LIB_LIB_SPI_EEPROM): $(LIB_SPI_EEPROM_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_SPI_EEPROM_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(SPIEEPROM_LIB_NAME) $(LIB_LIB_SPI_EEPROM)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_SPI_EEPROM_OBJS)
	$(CP) $(LIB_LIB_SPI_EEPROM) $(PREBUILT_LIB)$(SPIEEPROM_LIB_NAME)
endif

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_SPI_EEPROM_INCDIR)
LIB_CSRCDIR += $(LIB_SPI_EEPROM_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_SPI_EEPROM_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_SPI_EEPROM_ASMSRCDIR)

LIB_CSRCS += $(LIB_SPI_EEPROM_CSRCS)
LIB_CXXSRCS += $(LIB_SPI_EEPROM_CXXSRCS)
LIB_ASMSRCS += $(LIB_SPI_EEPROM_ASMSRCS)
LIB_ALLSRCS += $(LIB_SPI_EEPROM_CSRCS) $(LIB_SPI_EEPROM_ASMSRCS)

LIB_COBJS += $(LIB_SPI_EEPROM_COBJS)
LIB_CXXOBJS += $(LIB_SPI_EEPROM_CXXOBJS)
LIB_ASMOBJS += $(LIB_SPI_EEPROM_ASMOBJS)
LIB_ALLOBJS += $(LIB_SPI_EEPROM_OBJS)

LIB_DEFINES += $(LIB_SPI_EEPROM_DEFINES)
LIB_DEPS += $(LIB_SPI_EEPROM_DEPS)
LIB_LIBS += $(LIB_LIB_SPI_EEPROM)