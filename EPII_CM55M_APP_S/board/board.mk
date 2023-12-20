# boards root declaration
BOARDS_ROOT = $(EPII_ROOT)/board
##
# BOARD
# select the target board
# scan the sub-dirs of board to get the supported boards
SUPPORTED_BOARDS = $(basename $(notdir $(wildcard $(BOARDS_ROOT)/*/*.mk)))
override BOARD := $(strip $(BOARD))

## Set Valid Board
VALID_BOARD = $(call check_item_exist, $(BOARD), $(SUPPORTED_BOARDS))

## Try Check BOARD is valid
ifeq ($(VALID_BOARD), )
$(info BOARD - $(SUPPORTED_BOARDS) are supported)
$(error BOARD $(BOARD) is not supported, please check it!)
endif

BOARD_CSRC_DIR = $(EPII_ROOT)/board
BOARD_ASMSRC_DIR = $(EPII_ROOT)/board
BOARD_INC_DIR = $(EPII_ROOT)/board 
BOARD_CXXSRC_DIR = $(EPII_ROOT)/board
BOARD_CCSRC_DIR = $(EPII_ROOT)/board

BOARD_CSRCS =
BOARD_ASMSRCS =
BOARD_CXXSRCS =
BOARD_CCSRCS =

BOARD_COBJS =
BOARD_ASMOBJS =
BOARD_CXXOBJS =
BOARD_CCOBJS =
BOARD_OBJS =

#device usage settings
#must be before include
include $(BOARDS_ROOT)/$(VALID_BOARD)/$(VALID_BOARD).mk

BOARD_CSRC_DIR += $(BOARD_CSRC_SUBDIR)
BOARD_ASMSRC_DIR += $(BOARD_ASMSRC_SUBDIR)
BOARD_INC_DIR += $(BOARD_INC_SUBDIR)
BOARD_CXXSRC_DIR += $(BOARD_CXXSRC_SUBDIR)
BOARD_CCSRC_DIR += $(BOARD_CCSRC_SUBDIR)

BOARD_CSRCS += $(call get_csrcs, $(BOARD_CSRC_DIR))
BOARD_ASMSRCS += $(call get_asmsrcs, $(BOARD_ASMSRC_DIR))
BOARD_CXXSRCS += $(call get_cxxsrcs, $(BOARD_CXXSRC_DIR))
BOARD_CCSRCS += $(call get_ccsrcs, $(BOARD_CCSRC_DIR))

BOARD_COBJS += $(call get_relobjs, $(BOARD_CSRCS))
BOARD_ASMOBJS += $(call get_relobjs, $(BOARD_ASMSRCS))
BOARD_CXXOBJS += $(call get_relobjs, $(BOARD_CXXSRCS))
BOARD_CCOBJS += $(call get_relobjs, $(BOARD_CCSRCS))
BOARD_OBJS = $(BOARD_COBJS) $(BOARD_ASMOBJS) $(BOARD_CXXOBJS) $(BOARD_CCOBJS)

# get dependency files
BOARD_DEPS = $(call get_deps, $(BOARD_OBJS))

# extra macros to be defined
BOARD_DEFINES = 
BOARD_DEFINES += $(BOARD_SUB_DEFINES)

# generate library
BOARD_LIB = $(OUT_DIR)/libboard.a

# library generation rule
$(BOARD_LIB): $(BOARD_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(BOARD_OBJS)

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(BOARD_DEPS)
endif