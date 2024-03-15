# source code FatFS R0.15 Nov 6, 2022
# http://elm-chan.org/fsw/ff/arc/ff15.zip
# http://elm-chan.org/fsw/ff/ffsample.zip

MID_FATFS_DIR = $(MIDDLEWARES_ROOT)/fatfs

MID_FATFS_OBJS =

MID_FATFS_COBJS =
MID_FATFS_ASMOBJS =
MID_FATFS_CXXOBJS =
MID_FATFS_CCOBJS =

MID_FATFS_CSRCS =
MID_FATFS_ASMSRCS =
MID_FATFS_CXXSRCS =

MID_FATFS_ASMSRCDIR	 =
MID_FATFS_CSRCDIR    = $(MID_FATFS_DIR)/source
MID_FATFS_CXXSRCSDIR = 
MID_FATFS_INCDIR     = $(MID_FATFS_DIR)/source

MID_FATFS_CSRCS += $(MID_FATFS_DIR)/source/ff.c
MID_FATFS_CSRCS += $(MID_FATFS_DIR)/source/ffsystem.c
MID_FATFS_CSRCS += $(MID_FATFS_DIR)/source/ffunicode.c
MID_FATFS_CSRCS += $(MID_FATFS_DIR)/source/diskio.c
#MID_FATFS_CSRCS += $(MID_FATFS_DIR)/port/mmc_spi/mmc_we2_spi.c
#MID_FATFS_CSRCS += $(MID_FATFS_DIR)/port/flash/flash_we2.c
#MID_FATFS_CSRCS += $(MID_FATFS_DIR)/port/ram/ram_we2.c

ifdef FATFS_PORT_LIST
override FATFS_PORT_LIST := $(sort $(strip $(FATFS_PORT_LIST)))
else
FATFS_PORT_LIST = mmc_spi
endif

FOREACH_FATFS_PORT_LIST = $(foreach FATFS_PORT_, $(FATFS_PORT_LIST), $(addprefix $(MID_FATFS_DIR)/port/, $(FATFS_PORT_)))

MID_FATFS_CSRCDIR += $(FOREACH_FATFS_PORT_LIST)
MID_FATFS_INCDIR  += $(FOREACH_FATFS_PORT_LIST)
MID_FATFS_CSRCS   += $(call get_csrcs, $(FOREACH_FATFS_PORT_LIST))
	
# get object files
MID_FATFS_COBJS = $(call get_relobjs, $(MID_FATFS_CSRCS))
MID_FATFS_CXXOBJS = $(call get_relobjs, $(MID_FATFS_CXXSRCS))
MID_FATFS_ASMOBJS = $(call get_relobjs, $(MID_FATFS_ASMSRCS))
MID_FATFS_OBJS = $(MID_FATFS_COBJS) $(MID_FATFS_ASMOBJS) $(MID_FATFS_CXXOBJS)

# get dependency files
MID_FATFS_DEPS = $(call get_deps, $(MID_FATFS_OBJS))

# extra macros to be defined
MID_FATFS_DEFINES = -DMID_FATFS
MID_FATFS_DEFINES += $(foreach FATFS_PORT_, $(FATFS_PORT_LIST), $(addprefix -DFATFS_PORT_, $(FATFS_PORT_)))

# genearte library
ifeq ($(C_MID_FORCE_PREBUILT), y)
override MID_FATFS_OBJS:=
endif
FATFS_MID_NAME = libfatfs.a
MID_FATFS := $(subst /,$(PS), $(strip $(OUT_DIR)/$(FATFS_MID_NAME)))

# library generation rule
$(MID_FATFS): $(MID_FATFS_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(MID_FATFS_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(FATFS_MID_NAME) $(MID_FATFS)
else
	$(Q)$(AR) $(AR_OPT) $@ $(MID_FATFS_OBJS)
	$(CP) $(MID_FATFS) $(PREBUILT_LIB)$(FATFS_MID_NAME)
endif

# specific compile rules
# user can add rules to compile this library
# if not rules specified to this library, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_FATFS_INCDIR)
MID_CSRCDIR += $(MID_FATFS_CSRCDIR)
MID_CXXSRCDIR += $(MID_FATFS_CXXSRCDIR)
MID_ASMSRCDIR += $(MID_FATFS_ASMSRCDIR)

MID_CSRCS += $(MID_FATFS_CSRCS)
MID_CXXSRCS += $(MID_FATFS_CXXSRCS)
MID_ASMSRCS += $(MID_FATFS_ASMSRCS)
MID_ALLSRCS += $(MID_FATFS_CSRCS) $(MID_FATFS_ASMSRCS)

MID_COBJS += $(MID_FATFS_COBJS)
MID_CXXOBJS += $(MID_FATFS_CXXOBJS)
MID_ASMOBJS += $(MID_FATFS_ASMOBJS)
MID_ALLOBJS += $(MID_FATFS_OBJS)

MID_DEFINES += $(MID_FATFS_DEFINES)
MID_DEPS += $(MID_FATFS_DEPS)
MID_LIBS += $(MID_FATFS)
