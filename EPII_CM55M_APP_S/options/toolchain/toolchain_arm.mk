ifeq ($(DEBUG), 1)
	CDEBUG_OPTION := -g
	TOOLCHAIN_DEFINES += -DDEBUG
else
	TOOLCHAIN_DEFINES += -DNDEBUG
endif

ifeq ($(MAP), 1)
	LMAP_OPTION = --map --load_addr_map_info --xref --symbols --info=libraries --info=unused --info=veneers --info=sizes --list=$(APPL_FULL_NAME).map
endif


TOOLCHAIN_DEFINES += -D__ARM__
#TOOLCHAIN_DEFINES += -D__ARM__ -D__MICROLIB

SUPPORTED_OLEVELS = O O0 O1 O2 O3 Os Os1 Oz Ofast Og
##
# Optimization level settings
# refer to Using the ARM Compiler Collection
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
ARCH_FLAGS=--target=arm-arm-none-eabi -mthumb -mcpu=cortex-m$(CORTEX_M)
ARCH_FLAGS+=-DARMCM$(CORTEX_M)
ifeq ("$(FLOAT_ABI)","none")#no

else ifeq ("$(FLOAT_ABI)","hard")#hard
ARCH_FLAGS += -mfloat-abi=hard
else ifeq ("$(FLOAT_ABI)","soft")#soft 
ARCH_FLAGS += -mfloat-abi=soft
endif

CORE=CM$(CORTEX_M)


LCORE_OPT_ARM := --userlibpath "$(EPII_ROOT)/libs"
WARN_OPT := -Wall
DEVELOP_OPT :=
ARM_OPT := -mlittle-endian -fvectorize
#COMPILE_OPT +=-fmerge-constants
C_DIALECT_OPT := #-fno-builtin
CCORE_OPT_ARM := $(ARCH_FLAGS) -c $(ARM_OPT) $(WARN_OPT) $(C_DIALECT_OPT) $(DEVELOP_OPT)
CXXCORE_OPT_ARM := $(ARCH_FLAGS) -c $(ARM_OPT) $(WARN_OPT) $(C_DIALECT_OPT)  $(DEVELOP_OPT)
ACORE_OPT_ARM := $(ARCH_FLAGS) -c $(ARM_OPT) $(WARN_OPT) $(C_DIALECT_OPT)  $(DEVELOP_OPT) 
ifeq ($(TRUSTZONE), y)
ifeq ($(TRUSTZONE_TYPE), security)
ifeq ($(TRUSTZONE_FW_TYPE), 0)
LCORE_OPT_ARM += --import-cmse-lib-out=$(TRUSTZONE_SEC_LIB)
endif
CCORE_OPT_ARM += -mcmse
CXXCORE_OPT_ARM += -mcmse
ACORE_OPT_ARM += -mcmse
NSC_OBJ :=
else
NSC_OBJ := $(EPII_ROOT)/prebuilt_libs/$(strip $(TOOLCHAIN))/CMSE_lib_$(strip $(TOOLCHAIN)).o
endif
endif

ifeq ($(SEMIHOST), y)
	ALL_DEFINES += -DSEMIHOST
endif
#  
##
# define tools
##
ARM_TOOLCHAIN_PREFIX := $(strip $(ARM_TOOLPATH))

## ARM TOOLCHAIN TOOLS NAME DEFINITIONS ##
	CC	= armclang
	CXX	= armclang
	AS	= armclang
	DMP	= fromelf
	LD	= armlink
	AR	= armar
	NM	= arm-none-eabi-nm
	OBJCOPY	= fromelf
	ELF2HEX	= arm-none-eabi-objcopy
	SIZE	= fromelf --info=sizes,totals 

ARM_TOOLCHAIN_PREFIX_TEST := $(wildcard $(ARM_TOOLCHAIN_PREFIX)/$(CC)*)

ifneq ($(ARM_TOOLCHAIN_PREFIX_TEST), )
	CC	:= $(ARM_TOOLCHAIN_PREFIX)/$(CC)
	CXX	:= $(ARM_TOOLCHAIN_PREFIX)/$(CXX)
	AS	:= $(ARM_TOOLCHAIN_PREFIX)/$(AS)
	DMP	:= $(ARM_TOOLCHAIN_PREFIX)/$(DMP)
	LD	:= $(ARM_TOOLCHAIN_PREFIX)/$(LD)
	AR	:= $(ARM_TOOLCHAIN_PREFIX)/$(AR)
	NM	:= $(ARM_TOOLCHAIN_PREFIX)/$(NM)
	OBJCOPY	:= $(ARM_TOOLCHAIN_PREFIX)/$(OBJCOPY)
	ELF2HEX	:= $(ARM_TOOLCHAIN_PREFIX)/$(OBJCOPY)
	SIZE	:= $(ARM_TOOLCHAIN_PREFIX)/$(SIZE)
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

	## C/CPP/ASM/LINK Options
	COMPILE_OPT	+= $(CCORE_OPT_ARM)   $(ADT_COPT)   $(COMMON_COMPILE_OPT) -xc -std=c11 -ffp-mode=fast
	ifdef OS_SEL
	CXX_COMPILE_OPT	+= $(CXXCORE_OPT_ARM) $(ADT_CXXOPT) $(COMMON_COMPILE_OPT) -std=c++23
	else
	CXX_COMPILE_OPT	+= $(CXXCORE_OPT_ARM) $(ADT_CXXOPT) $(COMMON_COMPILE_OPT) -std=c++23 -fno-threadsafe-statics	
	endif
	ASM_OPT		+= $(ACORE_OPT_ARM)   $(ADT_AOPT)   $(COMMON_COMPILE_OPT) -x assembler-with-cpp
	PRE_LINKER_SCRIPT_FILE = $(OUT_DIR)/$(APPL_NAME).sct
	LINK_OPT	+= $(LCORE_OPT_ARM) $(ADT_LOPT) \
				$(LMAP_OPTION) --datacompressor=off --entry=Reset_Handler --scatter=$(PRE_LINKER_SCRIPT_FILE) $(NSC_OBJ)

ifeq ($(strip $(HX_TFM)),ON)
# Suppress link warnings that are consistant (and therefore hopefully
# harmless)
# https://developer.arm.com/documentation/100074/0608/linker-errors-and-warnings/list-of-the-armlink-error-and-warning-messages
# Empty region description
	LINK_OPT	+= --diag_suppress=6312
# Ns section matches pattern
	LINK_OPT	+= --diag_suppress=6314
# Duplicate input files
	LINK_OPT	+= --diag_suppress=6304
# Pattern only matches removed unused sections.
	LINK_OPT	+= --diag_suppress=6329
endif

	## Other Options
	AR_OPT		+= -r
	DMP_OPT		+= 
	DASM_OPT	+= --cpu=cortex-m$(CORTEX_M) --text -c -d -s -t -v -z
	SIZE_OPT	+=

	#LD_START_GROUPLIB	= -Wl,--start-group
	#LD_SYSTEMLIBS		= -lm -lc -lgcc
	#LD_END_GROUPLIB    = -Wl,--end-group
##
#   additional options
##
	ELF2HEX_INOPT		= -O ihex
	ELF2HEX_OUTOPT		=
	ELF2BIN_INOPT		= --bin
	ELF2BIN_OUTOPT		= -o
##
# Application Link file definition
##

	