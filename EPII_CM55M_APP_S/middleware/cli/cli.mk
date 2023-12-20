#################################################################
# This module is meant to be built from other modules.
# When doing this, the CLI_MENU_PATH variable must be specified, 
# or the linking fails.
# In addition, OUTPUT_DIR should be set to the desired output
# path, relative to this dir. 
# CLI_MENU_PATH should be specified as relative to OUTPUT_DIR
# or absolute.
#################################################################

######################################################
# Override the following variables from command line:
######################################################
# directory declaration
MID_CLI_DIR = $(MIDDLEWARES_ROOT)/cli

MID_CLI_ASMSRCDIR	= $(MID_CLI_DIR)
MID_CLI_CSRCDIR	= $(MID_CLI_DIR)
MID_CLI_INCDIR	= $(MID_CLI_DIR) $(MID_CLI_DIR)/inc $(MID_CLI_DIR)/inc $(EPII_ROOT)/app/integrate/

# find all the source files in the target directories
MID_CLI_CSRCS = $(call get_csrcs, $(MID_CLI_CSRCDIR))
MID_CLI_ASMSRCS = $(call get_asmsrcs, $(MID_CLI_ASMSRCDIR))

# get object files
MID_CLI_COBJS = $(call get_relobjs, $(MID_CLI_CSRCS))
MID_CLI_ASMOBJS = $(call get_relobjs, $(MID_CLI_ASMSRCS))
MID_CLI_OBJS = $(MID_CLI_COBJS) $(MID_CLI_ASMOBJS)

# get dependency files
MID_CLI_DEPS = $(call get_deps, $(MID_CLI_OBJS))

# extra macros to be defined
# -D###_CONSOLE : console type
# -DPROJECT_### : project name
MID_CLI_DEFINES = -DMID_CLI -DUART_CONSOLE -DPROJECT_EPII

# genearte library
ifeq ($(MWCLI_LIB_FORCE_PREBUILT), y)
override MID_CLI_OBJS:=
endif
MIDCLI_LIB_NAME = libmidcli.a
MID_LIB_CLI := $(subst /,$(PS), $(strip $(OUT_DIR)/$(MIDCLI_LIB_NAME)))

# library generation rule
$(MID_LIB_CLI): $(MID_CLI_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(MID_CLI_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(MIDCLI_LIB_NAME) $(MID_LIB_CLI)
else
	$(Q)$(AR) $(AR_OPT) $@ $(MID_CLI_OBJS)
	$(CP) $(MID_LIB_CLI) $(PREBUILT_LIB)$(MIDCLI_LIB_NAME)
endif
# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_CLI_INCDIR)
MID_CSRCDIR += $(MID_CLI_CSRCDIR)
MID_ASMSRCDIR += $(MID_CLI_ASMSRCDIR)

MID_CSRCS += $(MID_CLI_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_CLI_ASMSRCS)
MID_ALLSRCS += $(MID_CLI_CSRCS) $(MID_CLI_ASMSRCS)

MID_COBJS += $(MID_CLI_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_CLI_ASMOBJS)
MID_ALLOBJS += $(MID_CLI_OBJS)

MID_DEFINES += $(MID_CLI_DEFINES)
MID_DEPS += $(MID_CLI_DEPS)
MID_LIBS += $(MID_LIB_CLI)

