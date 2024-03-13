ifeq ($(DEBUG), 1)
	CDEBUG_OPTION := -g
	TOOLCHAIN_DEFINES += -DDEBUG
else
	TOOLCHAIN_DEFINES += -DNDEBUG	
endif

ifeq ($(MAP), 1)
	LMAP_OPTION = -Wl,-M,-Map=$(APPL_FULL_NAME).map
endif


TOOLCHAIN_DEFINES += -D__GNU__ -D__NEWLIB__

SUPPORTED_OLEVELS = O O0 O1 O2 O3 Os Os1 Oz Ofast Og
##
# Optimization level settings
# refer to Using the GNU Compiler Collection (GCC)
##
## -O<number>	Set optimization level to <number>
## -Ofast	Optimize for speed disregarding exact standards compliance
## -Og		Optimize for debugging experience rather than speed or size
## -Os		Optimize for space rather than speed
ifeq ($(OLEVEL), O)
	#Optimize. Optimizing compilation takes
	#somewhat more time, and a lot more memory
	#for a large function.
	#equivalent to -O1
	OPT_OLEVEL = -O
else
ifeq ($(OLEVEL), O0)
	#Reduce compilation time and
	#make debugging produce the expected results
	#This is the default.
	OPT_OLEVEL = -O0
else
ifeq ($(OLEVEL), O1)
	#equivalent to -O
	OPT_OLEVEL = -O1
else
ifeq ($(OLEVEL), O2)
	OPT_OLEVEL = -O2
else
ifeq ($(OLEVEL), O3)
	OPT_OLEVEL = -O3
else
ifeq ($(OLEVEL), Os)
	OPT_OLEVEL = -Os
else
ifeq ($(OLEVEL), Os1)
	#here equivalent to -Os
	OPT_OLEVEL = -Os
else
ifeq ($(OLEVEL), Oz)
	#here equivalent to -Os
	OPT_OLEVEL = -Os
else
ifeq ($(OLEVEL), Ofast)
	OPT_OLEVEL = -Ofast
else
ifeq ($(OLEVEL), Og)
	OPT_OLEVEL = -Og
else
	#no optimization option defined
	OPT_OLEVEL =
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif

EXTRA_LIBS :=
ARCH_FLAGS=-mthumb -mcpu=cortex-m$(CORTEX_M) 
ARCH_FLAGS+=-DARMCM$(CORTEX_M)
ifeq ("$(FLOAT_ABI)","none")#no

else ifeq ("$(FLOAT_ABI)","hard")#hard
ARCH_FLAGS += -mfloat-abi=hard
else ifeq ("$(FLOAT_ABI)","soft")#soft 
ARCH_FLAGS += -mfloat-abi=soft
endif


CORE=CM$(CORTEX_M)
ifdef OS_SEL
ifeq ($(SEMIHOST), y)
	USE_SPECS=-specs=rdimon.specs
	LD_START_GROUPLIB	= -Wl,--start-group
	LD_SYSTEMLIBS		= -lm -lrdimon -lgcc -lstdc++
	LD_END_GROUPLIB		= -Wl,--end-group
	ALL_DEFINES += -DSEMIHOST
else
	USE_SPECS=
	LD_START_GROUPLIB	= -Wl,--start-group
	LD_SYSTEMLIBS		= -lm -lc -lgcc -lstdc++
	LD_END_GROUPLIB		= -Wl,--end-group
endif
else
ifeq ($(SEMIHOST), y)
	USE_SPECS=-specs=rdimon.specs
	LD_START_GROUPLIB	= -Wl,--start-group
	LD_SYSTEMLIBS		= -lm -lrdimon -lgcc -lstdc++_nano
	LD_END_GROUPLIB		= -Wl,--end-group
	ALL_DEFINES += -DSEMIHOST
else
	USE_SPECS=-specs=nano.specs
	LD_START_GROUPLIB	= -Wl,--start-group
	LD_SYSTEMLIBS		= -lm -lc_nano -lgcc -lstdc++_nano
	LD_END_GROUPLIB		= -Wl,--end-group
endif
endif

ifeq ($(strip ${HX_TFM}),ON)
USE_SPECS=-specs=nano.specs
LD_SYSTEMLIBS=-lm -lc_nano -lgcc -lstdc++_nano
endif

# Use seimhosting or not
USE_NANO=-specs=nano.specs
USE_SEMIHOST=-specs=rdimon.specs
USE_NOHOST=-specs=nosys.specs

LCORE_OPT_GNU := -L"$(EPII_ROOT)/linker_script/" -L"$(EPII_ROOT)/linker_script/gcc" -L"$(EPII_ROOT)/libs" -Xlinker --gc-sections -Xlinker -print-memory-usage \
				-Xlinker --sort-section=alignment -Xlinker --cref $(ARCH_FLAGS)

LCORE_OPT_GNU += -L"$(EPII_ROOT)/linker_script/gcc/"
LCORE_OPT_GNU += -L"$(OUT_DIR)"

WARN_OPT := -Wall 
DEVELOP_OPT := -fstack-usage -flax-vector-conversions
#DEVELOP_OPT += -save-temps=obj
GNU_OPT := -ffunction-sections -fdata-sections
#COMPILE_OPT +=-fmerge-constants 
C_DIALECT_OPT := #-fno-builtin
CCORE_OPT_GNU := $(ARCH_FLAGS) -c $(GNU_OPT) $(WARN_OPT) $(C_DIALECT_OPT) $(DEVELOP_OPT) $(USE_SPECS)
CXXCORE_OPT_GNU := $(ARCH_FLAGS) -c $(GNU_OPT) $(WARN_OPT) $(C_DIALECT_OPT)  $(DEVELOP_OPT) $(USE_SPECS) 
ACORE_OPT_GNU := $(ARCH_FLAGS) -c $(GNU_OPT) $(WARN_OPT) $(C_DIALECT_OPT)  $(DEVELOP_OPT) $(USE_SPECS) 

ifeq ($(TRUSTZONE), y)
ifeq ($(TRUSTZONE_TYPE), security)
LCORE_OPT_GNU += -Xlinker --cmse-implib
ifeq ($(TRUSTZONE_FW_TYPE), 0)
LCORE_OPT_GNU += -Xlinker --out-implib=$(TRUSTZONE_SEC_LIB)
endif
CCORE_OPT_GNU += -mcmse
CXXCORE_OPT_GNU += -mcmse
ACORE_OPT_GNU += -mcmse
NSC_OBJ :=
else
NSC_OBJ := $(EPII_ROOT)/prebuilt_libs/$(strip $(TOOLCHAIN))/CMSE_lib_$(strip $(TOOLCHAIN)).o
endif
endif
#  
##
# define tools
##
GNU_TOOLCHAIN_PREFIX := $(strip $(GNU_TOOLPATH))

ifneq ($(GNU_TOOLCHAIN_PREFIX), )
GNU_TOOLCHAIN_PREFIX := $(wildcard $(GNU_TOOLCHAIN_PREFIX))
endif

## GNU TOOLCHAIN TOOLS NAME DEFINITIONS ##
	CC	= arm-none-eabi-gcc
	CXX	= arm-none-eabi-g++
	AS	= arm-none-eabi-as
	DMP	= arm-none-eabi-objdump
	LD	= arm-none-eabi-gcc
	AR	= arm-none-eabi-ar
	NM	= arm-none-eabi-nm
	OBJCOPY	= arm-none-eabi-objcopy
	ELF2HEX	= arm-none-eabi-objcopy
	SIZE	= arm-none-eabi-size

	MAKE	= make
## GNU TOOLCHAIN EXIST TESTING ##
GNU_TOOLCHAIN_PREFIX_TEST := $(wildcard $(GNU_TOOLCHAIN_PREFIX)/$(DMP)*)
ifeq "$(HOST_OS)" "Windows"
GCC_VERSION := $(SHELL $(CC) -dumpversion)
else
GCC_VERSION := $(shell $(CC) -dumpversion)
endif

ifneq ($(GNU_TOOLCHAIN_PREFIX_TEST), )
	CC	:= $(GNU_TOOLCHAIN_PREFIX)/$(CC)
	CXX	:= $(GNU_TOOLCHAIN_PREFIX)/$(CXX)
	AS	:= $(GNU_TOOLCHAIN_PREFIX)/$(AS)
	DMP	:= $(GNU_TOOLCHAIN_PREFIX)/$(DMP)
	LD	:= $(GNU_TOOLCHAIN_PREFIX)/$(LD)
	AR	:= $(GNU_TOOLCHAIN_PREFIX)/$(AR)
	NM	:= $(GNU_TOOLCHAIN_PREFIX)/$(NM)
	OBJCOPY	:= $(GNU_TOOLCHAIN_PREFIX)/$(OBJCOPY)
	ELF2HEX	:= $(GNU_TOOLCHAIN_PREFIX)/$(OBJCOPY)
	SIZE	:= $(GNU_TOOLCHAIN_PREFIX)/$(SIZE)
endif

#   toolchain flags
##
    #TOOLCHAIN_DEFINES += -D_HAVE_LIBGLOSS_

##
#   build options
##
	## Common Options
	MKDEP_OPT	= -fmacro-prefix-map="../$(@D)/"=. -MMD -MP -MF"$(@:$(OUT_DIR)/%.o=$(OUT_DIR)/%.d)" -MT"$(@:$(OUT_DIR)/%.o=$(OUT_DIR)/%.o)" -MT"$(@:$(OUT_DIR)/%.o=$(OUT_DIR)/%.d)"
	COMMON_COMPILE_OPT = $(OPT_OLEVEL) $(CDEBUG_OPTION) $(ALL_DEFINES) $(ALL_INCLUDES) $(MKDEP_OPT)

#	## C/CPP/ASM/LINK Options
#	COMPILE_OPT	+= $(CCORE_OPT_GNU)   $(ADT_COPT)   $(COMMON_COMPILE_OPT) -std=gnu99
#	ifdef OS_SEL
#	CXX_COMPILE_OPT	+= $(CXXCORE_OPT_GNU) $(ADT_CXXOPT) $(COMMON_COMPILE_OPT) -std=c++11 -fno-rtti -fno-exceptions
#	else
#	CXX_COMPILE_OPT	+= $(CXXCORE_OPT_GNU) $(ADT_CXXOPT) $(COMMON_COMPILE_OPT) -std=c++11 -fno-rtti -fno-exceptions -fno-threadsafe-statics
#	endif
	COMPILE_OPT	+= $(CCORE_OPT_GNU)   $(ADT_COPT)   $(COMMON_COMPILE_OPT) -std=gnu11
	CXX_COMPILE_OPT	+= $(CXXCORE_OPT_GNU) $(ADT_CXXOPT) $(COMMON_COMPILE_OPT) -std=c++17 -fno-rtti -fno-exceptions -fno-threadsafe-statics -nostdlib

	

	ASM_OPT		+= $(ACORE_OPT_GNU)   $(ADT_AOPT)   $(COMMON_COMPILE_OPT) -x assembler-with-cpp
	PRE_LINKER_SCRIPT_FILE = $(OUT_DIR)/$(APPL_NAME).ld
	ifeq ($(firstword $(sort $(GCC_VERSION) 12.0.0)),12.0.0)
	# if GCC >= 12.0.0, use -Wl,--no-warn-rwx-segments
	LINK_OPT	+= -Wl,--no-warn-rwx-segments
	endif
	LINK_OPT	+= $(ALL_DEFINES) $(LCORE_OPT_GNU) $(ADT_LOPT) \
				$(LMAP_OPTION) $(USE_SPECS) -T $(PRE_LINKER_SCRIPT_FILE) $(NSC_OBJ)

	## Other Options
	AR_OPT		+= -r
	DMP_OPT		+= -x
	DASM_OPT	+= -D
	SIZE_OPT	+=

##
#   additional options
##
	ELF2HEX_INOPT		= -O ihex
	ELF2HEX_OUTOPT		=
	ELF2BIN_INOPT		= -v -O binary
	ELF2BIN_OUTOPT		=	
##
# Application Link file definition
##

	
