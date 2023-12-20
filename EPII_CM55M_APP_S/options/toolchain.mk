##
# TOOLCHAIN
# Select which TOOLCHAIN should be used
# Supported toolchains:
#
#    arm  -ARM CLANG
#    gnu -ARM GNU Tool chain
##
SUPPORTED_TOOLCHAINS = gnu arm
## Another define in options.mk
TOOLCHAIN ?= arm
# none, hard, soft
FLOAT_ABI = hard
# semihost (y or n)
SEMIHOST ?= n
override FLOAT_ABI := $(strip $(FLOAT_ABI))
override TOOLCHAIN := $(strip $(TOOLCHAIN))

## Set Valid Toolchain
VALID_TOOLCHAIN = $(call check_item_exist, $(TOOLCHAIN), $(SUPPORTED_TOOLCHAINS))

ifeq ($(VALID_TOOLCHAIN), arm)
override ELF_FILENAME := axf
else
override ELF_FILENAME := elf
endif

## Try include toolchain makefile
ifneq ($(VALID_TOOLCHAIN), )
include $(EPII_ROOT)/options/toolchain/toolchain_$(VALID_TOOLCHAIN).mk
else
$(info TOOLCHAIN - $(SUPPORTED_TOOLCHAINS) are supported)
$(error Toolchain $(TOOLCHAIN) is not supported, please check it!)
endif

