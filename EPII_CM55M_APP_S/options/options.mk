##
# Special Default Make Goal to all
# so if no make goal specified,
# all will be the make goal,
# not the first target in the makefile
##
.DEFAULT_GOAL = all


## Force convert windows path to linux format
override EPII_ROOT := $(strip $(subst \,/,$(EPII_ROOT)))


## Default options settings ##

##
# Output OBJS Root Directory
##
OUT_DIR_ROOT ?=

##
# Compile Toolchain
# Refer to toolchain.mk
##
TOOLCHAIN ?= gnu

# Optimization Level
# Please Refer to toolchain_xxx.mk for this option
OLEVEL ?= O2

##
# additional options
# compile link assembler
##
ADT_COPT ?= 
ADT_CXXOPT ?=
ADT_LOPT ?=
ADT_AOPT ?=

##
# DEBUG
# 1 for enable
# other for disable
##
DEBUG ?= 1

##
# generate map
# 1 for enable
# other for disable
##
MAP ?= 1

##
# Control Compiler Message Show
# 1: show compile total options
# 0: just show compile file info
##
V ?= 1

##
# Suppress All Messages
##
SILENT ?= 0

##
# Overwrite some configurations
# Never modify this if you don't know about it
##
override APPL := $(strip $(APPL))
override OUT_DIR_ROOT := $(strip $(OUT_DIR_ROOT))

override ADT_COPT := $(strip $(ADT_COPT))
override ADT_CXXOPT := $(strip $(ADT_CXXOPT))
override ADT_LOPT := $(strip $(ADT_LOPT))
override ADT_AOPT := $(strip $(ADT_AOPT))
override CUSTOMER := $(strip $(CUSTOMER))

## Include Scripts and Functions ##
include $(EPII_ROOT)/options/scripts.mk

LINKER_SCRIPT_ROOT = $(EPII_ROOT)/linker_script

-include $(EPII_ROOT)/options/prebuilt_force.mk
# application makefile
include $(EPII_ROOT)/app/app.mk

PREBUILT_LIB =$(subst /,$(PS),prebuilt_libs/$(strip $(TOOLCHAIN))/)
##
# Output Directory Set
##
ifeq ($(strip $(OUT_DIR_ROOT)), )
OUT_DIR_PREFIX = obj_
else
### Check Root Out Directory Doesn't contain whitespace ##
ifneq ($(words $(OUT_DIR_ROOT)),1)
$(error PATH '$(OUT_DIR_ROOT)' contains whitespace, not supported!)
else
OUT_DIR_ROOT_FIXPATH = $(subst \,/, $(strip $(OUT_DIR_ROOT)))
OUT_DIR_ROOT_FIXPATH_EXIST = $(realpath $(OUT_DIR_ROOT_FIXPATH))
ifeq ($(OUT_DIR_ROOT_FIXPATH_EXIST),)
$(error PATH '$(OUT_DIR_ROOT)' doesn't exist, please check it!)
else
override OUT_DIR_ROOT := $(OUT_DIR_ROOT_FIXPATH)
OUT_DIR_PREFIX = $(strip $(OUT_DIR_ROOT))/obj_
endif
endif
endif
## Board Infomation
BOARD_INFO = $(strip $(BOARD))_icv$(strip $(IC_VER))_bdv$(strip $(BD_VER))
## Build Infomation
BUILD_INFO = $(strip $(TOOLCHAIN))_$(strip $(BOARD))_$(strip $(IC_PACKAGE_SEL))
## Selected Configuration
SELECTED_CONFIG=$(BOARD_INFO)-$(BUILD_INFO)

## Objects Output Directory
BOARD_OUT_DIR = $(OUT_DIR_PREFIX)$(BOARD_INFO)
OUT_DIR = $(BOARD_OUT_DIR)/$(BUILD_INFO)
override MPU := $(strip $(MPU))

override EPII_CORE := $(strip $(EPII_CORE))
ALL_DEFINES += -DCM55_$(EPII_CORE)
ALL_DEFINES += -DIC_VERSION=$(strip $(IC_VER))
override CoreVolt := $(strip $(CoreVolt))
ALL_DEFINES += -DCOREV_$(CoreVolt)
##
# Application Path and Name Setting
##
APPL_NAME = $(strip $(APPL)_$(BUILD_INFO))
APPL_FULL_NAME = $(strip $(OUT_DIR)/$(APPL_NAME))
APPL_OUT_DIR = $(OUT_DIR)
ifeq ($(TRUSTZONE), y)
ifeq ($(TRUSTZONE_TYPE), security)
APPL_FULL_NAME =$(strip $(OUT_DIR)/$(APPL_NAME))_s
else
APPL_FULL_NAME =$(strip $(OUT_DIR)/$(APPL_NAME))_ns
endif
else
APPL_FULL_NAME =$(strip $(OUT_DIR)/$(APPL_NAME))
endif
TRUSTZONE_SEC_LIB_NAME=CMSE_lib_$(strip $(TOOLCHAIN)).o
TRUSTZONE_SEC_LIB=$(APPL_OUT_DIR)/$(TRUSTZONE_SEC_LIB_NAME)
NSC_PREBUILT_LIB =$(PREBUILT_LIB)$(TRUSTZONE_SEC_LIB_NAME)
ifeq "$(HOST_OS)" "Windows"
TRUSTZONE_SEC_LIB_WILDCARD=$(subst /,\,$(TRUSTZONE_SEC_LIB))
NONTRUSTZONE_SRC_DIR_WILDCARD = $(subst /,\,$(NONTRUSTZONE_SRC_DIR))
NONTRUSTZONE_SRC_SEC_LIB_WILDCARD = "$(NONTRUSTZONE_SRC_DIR_WILDCARD)\prebuilt_libs\$(strip $(TOOLCHAIN))\"
else
TRUSTZONE_SEC_LIB_WILDCARD=$(subst \,/,$(TRUSTZONE_SEC_LIB))
NONTRUSTZONE_SRC_DIR_WILDCARD = $(subst \,/,$(NONTRUSTZONE_SRC_DIR))
NONTRUSTZONE_SRC_SEC_LIB_WILDCARD = "$(NONTRUSTZONE_SRC_DIR_WILDCARD)\prebuilt_libs\$(strip $(TOOLCHAIN))\"
endif


################## Pre processing #########################################
# include toolchain settings
include $(EPII_ROOT)/options/toolchain.mk
################# Components of EPII ###################################
# board specific settings
include $(EPII_ROOT)/options/ic_package.mk
include $(EPII_ROOT)/device/device.mk
include $(EPII_ROOT)/board/board.mk
include $(EPII_ROOT)/linker_script/linker_script.mk
include $(EPII_ROOT)/drivers/drivers.mk
-include $(EPII_ROOT)/external/external.mk
include $(EPII_ROOT)/interface/interface.mk
-include $(EPII_ROOT)/cmsis_drivers/cmsis_drivers.mk
-include $(EPII_ROOT)/customer/customer_sec.mk

# library makefile
-include $(EPII_ROOT)/library/library.mk
ifeq ($(TRUSTZONE), y)
ALL_DEFINES += -DTRUSTZONE
override TRUSTZONE_TYPE := $(strip $(TRUSTZONE_TYPE))
ifeq ($(TRUSTZONE_TYPE), security)
include $(EPII_ROOT)/trustzone/tz_cfg/tz_cfg.mk
ALL_DEFINES += -DTRUSTZONE_SEC
ifeq ($(TRUSTZONE_FW_TYPE), 1)
ALL_DEFINES += -DTRUSTZONE_SEC_ONLY
else
include $(EPII_ROOT)/trustzone/nsc_function/nsc_function.mk
include $(EPII_ROOT)/trustzone/trustzone.mk
endif
else
ALL_DEFINES += -DTRUSTZONE_NS
include $(EPII_ROOT)/trustzone/nsc_function/nsc_function.mk
endif
endif

# os specific settings
ifdef OS_SEL
override OS_SEL := $(strip $(OS_SEL))
include $(EPII_ROOT)/os/os.mk
endif

# middleware makefile
-include $(EPII_ROOT)/middleware/middleware.mk

############## Post processing #############################################
# source directories and include directories settings
include $(EPII_ROOT)/options/files.mk


##
# Include Compiler / Linker / Other Rules
#
include $(EPII_ROOT)/options/rules.mk
