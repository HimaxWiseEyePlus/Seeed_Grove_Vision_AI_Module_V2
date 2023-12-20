override LINKER_SCRIPT_ROOT = $(EPII_ROOT)/linker_script


ifeq ($(VALID_TOOLCHAIN), arm)
include $(LINKER_SCRIPT_ROOT)/arm/linker_script_arm.mk
else#TOOLChain
include $(LINKER_SCRIPT_ROOT)/gcc/linker_script_gcc.mk
endif

ifneq ($(MAKECMDGOALS),clean)
-include $(basename $(PRE_LINKER_SCRIPT_FILE)).d
endif
