##
# Makefile Version Check
# Must >= 3.81
# $(.FEATURES) is defined from 3.81
# and call function is defined from 3.78
# see http://cvs.savannah.gnu.org/viewvc/make/NEWS?revision=2.109&root=make&view=markup
##
ifndef .FEATURES
$(error Makefile version should be at least 3.81)
endif

##
# define OS specific parameters
##
###
# Under Win9x, COMSPEC is defined, but ComSpec isn't,
# and under WinNT, ComSpec is defined, but COMSPEC isn't!
###
ifdef ComSpec
	EPII_CMD:=$(ComSpec)
endif
ifdef COMSPEC
	EPII_CMD:=$(COMSPEC)
endif

ifneq "$(EPII_CMD)" ""
## Check Cygwin##
ifneq "$(findstring /cygdrive/,$(PATH))" ""
	HOST_OS:=Cygwin
else
	HOST_OS:=Windows
endif
else ## Linux Environment ##
	HOST_OS:=$(shell uname -o)
endif

##
# Define one space
##
nullstring=
space=$(nullstring) # one space

## Check OS ##

## Assume POSIX ##
PS=/$(nullstring)
NULL=/dev/null
OPNPRN=\(
CLSPRN=\)

RM=rm -rf
RMD=rm -rf
ECHO=echo
CP=cp
MKD = mkdir -p

IFEXISTDIR=[ ! -d$(space)
ENDIFEXISTDIR=$(space)] ||

IFNOTEXISTDIR=[ -d$(space)
ENDIFNOTEXISTDIR=$(space)] ||

IFEXISTFILE=[ ! -f$(space)
ENDIFEXISTFILE=$(space)] ||

IFNOTEXISTFILE=[ -f$(space)
ENDIFNOTEXISTFILE=$(space)] ||

## Check OS == Windows ##
ifeq "$(HOST_OS)" "Windows"
	PS=\$(nullstring)
	NULL=NUL
	DOS_CMD=$(EPII_CMD) /C
	OPNPRN=(
	CLSPRN=)
	# when OS is windows, force SHELL to be cmd
	# or if in your evironment path there is
	# a mingw shell, the make process will go wrong
	SHELL:=$(EPII_CMD)

	RM=$(DOS_CMD) del /Q /F
	RMD=$(DOS_CMD) rd /S /Q
	ECHO=$(DOS_CMD) echo
	CP=copy /Y
	MKD = $(DOS_CMD) mkdir

	IFEXISTDIR=$(DOS_CMD) if exist$(space)
	ENDIFEXISTDIR=$(space)

	IFNOTEXISTDIR=$(DOS_CMD) if not exist$(space)
	ENDIFNOTEXISTDIR=$(space)

	IFEXISTFILE=$(DOS_CMD) if exist$(space)
	ENDIFEXISTFILE=$(space)

	IFNOTEXISTFILE=$(DOS_CMD) if not exist$(space)
	ENDIFNOTEXISTFILE=$(space)
endif

## Check OS == Cygwin ##
ifeq "$(HOST_OS)" "Cygwin"
	PS=/$(nullstring)
	NULL=/dev/null
	OPNPRN=\(
	CLSPRN=\)

	RM=rm -rf
	RMD=rm -rf
	ECHO=echo
	CP=cp -rf
	MKD = mkdir -p

	IFEXISTDIR=[ ! -d$(space)
	ENDIFEXISTDIR=$(space)] ||

	IFNOTEXISTDIR=[ -d$(space)
	ENDIFNOTEXISTDIR=$(space)] ||

	IFEXISTFILE=[ ! -f$(space)
	ENDIFEXISTFILE=$(space)] ||

	IFNOTEXISTFILE=[ -f$(space)
	ENDIFNOTEXISTFILE=$(space)] ||
endif

## MAKEFILE COMPILE MESSAGE CONTROL ##
ifeq ($(V),1)
	Q=
else
	Q=@
endif

## Suppress All Message ##
ifeq ($(SILENT), 1)
	TRACE_CREATE_DIR	=
	TRACE_COMPILE		=
	TRACE_ASSEMBLE		=
	TRACE_LINK			=
	TRACE_ARCHIVE		=
	TRACE_GEN_LINKFILE	=
	## Overwrite Q Value set by V option ##
	override Q=@
else
	TRACE_CREATE_DIR	= @$(ECHO) "Creating Directory    : " $(@D)
	TRACE_COMPILE		= @$(ECHO) "Compiling             : " $<
	TRACE_ASSEMBLE		= @$(ECHO) "Assembling            : " $<
	TRACE_LINK			= @$(ECHO) "Linking               : " $@
	TRACE_ARCHIVE		= @$(ECHO) "Archiving             : " $@
	TRACE_GEN_LINKFILE	= @$(ECHO) "Generating Linkfile   : " $@
endif

TRY_MK_OBJDIR = @-$(IFNOTEXISTDIR) $(subst /,$(PS),$(dir $@)) $(ENDIFNOTEXISTDIR) $(MKD) $(subst /,$(PS),$(dir $@)) 2> $(NULL)
TRY_MKDIR = @-$(IFNOTEXISTDIR) $(subst /,$(PS),$@) $(ENDIFNOTEXISTDIR) $(MKD) $(subst /,$(PS),$@) 2> $(NULL)

## Small Functions ##
get_csrcs = $(foreach subdir, $(1), $(wildcard $(subdir)/*.c $(subdir)/*.C))
get_asmsrcs = $(foreach subdir, $(1), $(wildcard $(subdir)/*.s $(subdir)/*.S))
get_cxxsrcs = $(foreach subdir, $(1), $(wildcard $(subdir)/*.cpp $(subdir)/*.CPP))
get_ccsrcs = $(foreach subdir, $(1), $(wildcard $(subdir)/*.cc $(subdir)/*.CC))
get_objs = $(addprefix $(2)/, $(notdir $(addsuffix .o, $(basename $(1)))))
get_relobjs = $(addprefix $(OUT_DIR)/, $(patsubst $(EPII_ROOT)/%,%, $(addsuffix .o, $(basename $(1)))))
get_deps = $(patsubst %.o,%.d, $(1))
get_mkdir_tmps = $(addsuffix .mkdir_done, $(sort $(1)))
## must only use in prerequisites and with SECONDEXPANSION enabled
get_c_prerequisite = $(foreach subdir, $(1), $$(wildcard $(subdir)/%.c))
get_cxx_prerequisite = $(foreach subdir, $(1), $$(wildcard $(subdir)/%.cpp))
get_cc_prerequisite = $(foreach subdir, $(1), $$(wildcard $(subdir)/%.cc))
get_asm_prerequisite = $(foreach subdir, $(1), $$(wildcard $(subdir)/%.s))
## add prefix(1) and suffix(2) to words(3)##
add_prefix_suffix = $(addprefix $(strip $(1)), $(addsuffix $(strip $(2)), $(strip $(3))))
## check item(1) exist in itemlist(2)
# if exist return the item, not return empty string
# the item should be only 1 word, if input 2 or more words
# it will return empty
##
check_item_exist = $(strip $(if $(filter 1, $(words $(1))),$(filter $(1), $(sort $(2))),))
get_prelibs = $(foreach subdir, $(1), $(wildcard $(subdir)*.a $(subdir)*.A))
