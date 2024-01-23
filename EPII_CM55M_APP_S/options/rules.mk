## All defines ##
ALL_DEFINES += $(BOARD_DEFINES) $(DRIVERS_DEFINES) $(CMSIS_DRIVERS_DEFINES) \
		$(DEVICE_DEFINES) $(EXTERNAL_DEFINES) $(MID_DEFINES) $(LIB_DEFINES) $(OS_DEFINES) $(OS_HAL_DEFINES)\
		$(TOOLCHAIN_DEFINES) $(APPL_DEFINES) $(TRUSTZONE_SEC_DEFINES) $(TRUSTZONE_CFG_DEFINES) $(NSC_DEFINES) $(INTERFACE_DEFINES) $(CUSTOMER_DEFINES)

## all includes conversion  ##
ALL_INCLUDES = $(foreach dir,$(ALL_INC_DIRS),-I$(dir))
ALL_INCLUDES += -I$(EPII_ROOT)/CMSIS

##
# All object sub-directories need to be created
##
ALL_OBJDIRS = $(sort $(dir $(ALL_ALLOBJS)))

##
# Generated directories required to build this applications
##
ALL_GENERATED_DIRS += $(ALL_OBJDIRS)/
ALL_GENERATED_DIRS_TMPFILES = $(sort $(call get_mkdir_tmps, $(ALL_GENERATED_DIRS)))

##
# Collect all libraries
##


##
# Collect Dependency Files
##
APPL_DEPS = $(call get_deps, $(APPL_OBJS))
ALL_DEPS = $(call get_deps, $(ALL_ALLOBJS))

# include dependency files of application
ifneq ($(MAKECMDGOALS),clean)
 -include $(APPL_DEPS)
endif


##
# Clean Related
##
OUT_DIR_PREFIX_WILDCARD = $(subst \,/,$(OUT_DIR_PREFIX))
OUT_DIRS_WILDCARD = $(sort $(wildcard $(OUT_DIR_PREFIX_WILDCARD)*))
OUT_DIRS = $(subst /,$(PS), $(strip $(OUT_DIRS_WILDCARD)))

ifeq ($(strip $(OUT_DIRS)), )
DIST_DIR_CLEAN =
else
DIST_DIR_CLEAN = $(RMD) $(OUT_DIRS)
endif

TEMP_FILES2CLEAN = $(sort $(wildcard *.o *.out *.bin *.dis *.axf *.elf *.a *.hex *.map *.bak *.dump *.d *.img *.dasm *.log))
ifeq ($(strip $(TEMP_FILES2CLEAN)), )
TEMPFILES_CLEAN =
else
TEMPFILES_CLEAN = $(RM) $(TEMP_FILES2CLEAN)
endif

##
# Overwrite some variables
##
override ALL_DEFINES := $(sort $(ALL_DEFINES))
override ALL_INCLUDES := $(sort $(ALL_INCLUDES))
#
#  the definition of source file directory
#
vpath %.C $(ALL_CSRC_DIRS)
vpath %.c $(ALL_CSRC_DIRS)
vpath %.cpp $(ALL_CXXSRC_DIRS)
vpath %.cc $(ALL_CCSRC_DIRS)
vpath %.CC $(ALL_CCSRC_DIRS)
vpath %.S $(ALL_ASMSRC_DIRS)
vpath %.s $(ALL_ASMSRC_DIRS)

ifeq ($(TOOLCHAIN), arm)
override PATH :=$(ARM_TOOLPATH):$(PATH)
else
override PATH :=$(GNU_TOOLPATH):$(PATH)
endif

.PHONY : all build dump dasm bin hex size clean boardclean distclean run gui cfg opt info spopt infodirs infosrcs infoobjs help scripts env_check

all : $(APPL_FULL_NAME).$(ELF_FILENAME)

build : clean all

dump : $(APPL_FULL_NAME).dump

dasm : $(APPL_FULL_NAME).dasm

bin : $(APPL_FULL_NAME).bin

hex : $(APPL_FULL_NAME).hex

env_check: cfg scripts opt info

help :
	@$(ECHO) 'Build Targets for selected configuration:'
	@$(ECHO) '  all         - Build example'
	@$(ECHO) '  bin         - Build and Generate binary for example'
	@$(ECHO) '  hex         - Build and Generate Intel Hex File for example'
	@$(ECHO) '  build       - Clean first then compile example'
	@$(ECHO) '  dump        - Generate dump information for example'
	@$(ECHO) '  dasm        - Disassemble object file'
	@$(ECHO) '  size        - Display size information of object file'
	@$(ECHO) 'Clean Targets:'
	@$(ECHO) '  clean       - Remove object files of selected configuration'
	@$(ECHO) '  boardclean  - Remove object files of selected board'
	@$(ECHO) '  distclean   - Remove object files of all boards'
	@$(ECHO) 'Debug & Run Targets for selected configuration:'
	@$(ECHO) 'Other Targets:'
	@$(ECHO) '  cfg         - Display build target configuration'
	@$(ECHO) '  opt         - Display Current MAKE options'
	@$(ECHO) '  spopt       - Display Supported MAKE options'
	@$(ECHO) 'Available Configurations:'
	@$(ECHO) '  BOARD=fpga|evb                         - Build for which board(EMSK, NSIM, AXS)'
	@$(ECHO) '  BD_VER=10|20                 - Board Version of development board'
	@$(ECHO) '  OLEVEL=Os|O0|O1|O2|O3                       - Optimization Level of examples to be built'
	@$(ECHO) '                                              - Current core configuration'
	@$(ECHO) '  TOOLCHAIN=mw|gnu                            - Current selected compiling toolchain'
	@$(ECHO) '  OUT_DIR_ROOT=.|xxx                          - Specify where to generate object files default current example makefile folder'
	@$(ECHO) '  SILENT=0|1                                  - Disable or enable message output'
	@$(ECHO) '  V=0|1                                       - Disable or enable verbose compiling information'
	@$(ECHO) 'Example Usage:'
	@$(ECHO) '  make all                                                                - build example in current directory using default configuration'

scripts:
	@$(ECHO) '=======Scripts======='
	@$(ECHO) 'PS            : $(PS)'
	@$(ECHO) 'DOS_CMD              : $(DOS_CMD)'
	@$(ECHO) 'SHELL   : $(SHELL)'
	@$(ECHO) 'RM       : $(RM)'
	@$(ECHO) 'RMD: $(RMD)'
	@$(ECHO) 'ECHO    : $(ECHO)'
	@$(ECHO) 'CP : $(CP)'
	@$(ECHO) 'MKD         : $(MKD)'
	@$(ECHO) 'PATH	: $(PATH)'
	
cfg :
	@$(ECHO) '=======Current Configuration======='
	@$(ECHO) 'Host OS            : $(HOST_OS)'
	@$(ECHO) 'Board              : $(BOARD)'
	@$(ECHO) 'Hardware Version   : $(BD_VER)'
	@$(ECHO) 'CPU Clock HZ       : $(CPU_FREQ)'
	@$(ECHO) 'Peripheral Clock HZ: $(DEV_FREQ)'
	@$(ECHO) 'Build Toolchain    : $(TOOLCHAIN)'
	@$(ECHO) 'Optimization Level : $(OLEVEL)'
	@$(ECHO) 'Debug Jtag         : $(JTAG)'
	@$(ECHO) '======Supported Configurations of $(BOARD)-$(BD_VER)======'
	@$(ECHO) 'Boards (BOARD)                 : $(SUPPORTED_BOARDS)'
	@$(ECHO) 'Build Toolchains (TOOLCHAIN)   : $(SUPPORTED_TOOLCHAINS)'
	@$(ECHO) 'Debug Jtags (JTAG)             : $(SUPPORTED_JTAGS)'


opt :
	@$(ECHO) ======CURRENT CONFIGURATION=====
	@$(ECHO) BOARD : $(BOARD)
	@$(ECHO) BD_VER : $(BD_VER)
	@$(ECHO) TOOLCHAIN : $(TOOLCHAIN)
	@$(ECHO) OLEVEL : $(OLEVEL)
	@$(ECHO) JTAG : $(JTAG)
	@$(ECHO) EPII_ROOT : $(EPII_ROOT)
	@$(ECHO) OUT_DIR_ROOT : $(OUT_DIR_ROOT)
	@$(ECHO) COMPILE_OPT : $(subst $(MKDEP_OPT), , $(COMPILE_OPT))
	@$(ECHO) CXX_COMPILE_OPT: $(subst $(MKDEP_OPT), , $(CXX_COMPILE_OPT))
	@$(ECHO) ASM_OPT : $(subst $(MKDEP_OPT), , $(ASM_OPT))
	@$(ECHO) AR_OPT : $(AR_OPT)
	@$(ECHO) LINK_OPT : $(LINK_OPT)
	@$(ECHO) LINKER_SCRIPT_FILE : $(LINKER_SCRIPT_FILE)
	@$(ECHO) EPII_USECASE_SEL : $(EPII_USECASE_SEL)

info:
	@$(ECHO) ======CURRENT BUILD INFORMATION=====
	@$(ECHO) EPII_ROOT : $(realpath $(EPII_ROOT))
	@$(ECHO) OUT_DIR_ROOT : $(realpath $(OUT_DIR_ROOT))
	@$(ECHO) APPLICATION_NAME : $(APPL_NAME)
	@$(ECHO) APPLICATION_LINKSCRIPT : $(APPL_LINK_FILE)
	@$(ECHO) APPLICATION_ELF : $(APPL_FULL_NAME).$(ELF_FILENAME)
	@$(ECHO) APPLICATION_BIN : $(APPL_FULL_NAME).bin
	@$(ECHO) APPLICATION_HEX : $(APPL_FULL_NAME).hex
	@$(ECHO) APPLICATION_MAP : $(APPL_FULL_NAME).map
	@$(ECHO) APPLICATION_DUMP : $(APPL_FULL_NAME).dump
	@$(ECHO) APPLICATION_DASM : $(APPL_FULL_NAME).dasm
	@$(ECHO) LIB_SEL : $(sort $(LIB_SEL))
	@$(ECHO) OS_SEL : $(sort $(OS_SEL))
	@$(ECHO) MIDDLEWARE : $(sort $(MID_SEL))
	@$(ECHO) EXT_DEV_LIST : $(sort $(EXT_DEV_LIST))
	@$(ECHO) DRIVERS_IP_LIST : $(sort $(DRIVERS_IP_LIST))

spopt :
	@$(ECHO) ======SUPPORTED CONFIGURATIONS=====
	@$(ECHO) SUPPORTED_BOARDS : $(SUPPORTED_BOARDS)
	@$(ECHO) SUPPORTED_BD_VERS : $(SUPPORTED_BD_VERS)
	@$(ECHO) SUPPORTED_OLEVELS : $(SUPPORTED_OLEVELS)
	@$(ECHO) SUPPORTED_TOOLCHAINS : $(SUPPORTED_TOOLCHAINS)
	@$(ECHO) SUPPORTED_JTAGS : $(SUPPORTED_JTAGS)

infodirs :
	@$(ECHO) ======ALL Directories Information=====
	@$(ECHO) ALL_INC_DIRS : $(ALL_INC_DIRS)
	@$(ECHO) ALL_CSRC_DIRS : $(ALL_CSRC_DIRS)
	@$(ECHO) ALL_ASMSRC_DIRS : $(ALL_ASMSRC_DIRS)
	@$(ECHO) ALL_CXXSRC_DIRS : $(ALL_CXXSRC_DIRS)

infosrcs :
	@$(ECHO) ======ALL Sources Information=====
	@$(ECHO) ALL_CSRCS : $(ALL_CSRCS)
	@$(ECHO) ALL_ASMSRCS : $(ALL_ASMSRCS)
	@$(ECHO) ALL_CXXSRCS : $(ALL_CXXSRCS)
	@$(ECHO) ALL_CCSRCS : $(ALL_CCSRCS)

infoobjs :
	@$(ECHO) ======ALL Sources Information=====
	@$(ECHO) ALL_COBJS : $(ALL_COBJS)
	@$(ECHO) ALL_ASMOBJS : $(ALL_ASMOBJS)
	@$(ECHO) ALL_CXXOBJS : $(ALL_CXXOBJS)


size : $(APPL_FULL_NAME).$(ELF_FILENAME)
	@$(ECHO) "Print Application Program Size"
	$(Q)$(SIZE) $(SIZE_OPT) $<

clean :
	@$(ECHO) "Clean Workspace For Selected Configuration : $(SELECTED_CONFIG)"
	-$(IFEXISTDIR) $(subst /,$(PS),$(OUT_DIR)) $(ENDIFEXISTDIR) $(RMD) $(subst /,$(PS),$(OUT_DIR))
	-$(IFEXISTDIR) .sc.project $(ENDIFEXISTDIR) $(RMD) .sc.project
	-$(TEMPFILES_CLEAN)


#####RULES FOR GENERATING ELF FILE#####
.SECONDEXPANSION:
$(APPL_FULL_NAME).$(ELF_FILENAME) : $(ALL_GENERATED_DIRS_TMPFILES) $(DEVICE_OBJS) $(BOARD_OBJS) $(INTEGRATE_OBJS) $(HWACCAUTOTEST_OBJS) $(SCENARIO_APP_OBJS) $(APPL_OBJS) $(TRUSTZONE_SEC_OBJS) $(NSC_OBJS) $(OS_ALLOBJS) $(OS_HAL_ALLOBJS) $(TFM_OBJS) $(INTERFACE_OBJS) $(HMX_BSP_LIBS) $(INFERENCE_ENGINE_LIB) $(TFM_LIBS) $(CV_LIB) $(AUDIOALGO_LIB) $(PRE_LINKER_SCRIPT_FILE) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_LINK)
	$(Q)$(LD) $(LINK_OPT) $(DEVICE_OBJS) $(INTEGRATE_OBJS) $(HWACCAUTOTEST_OBJS) $(BOARD_OBJS) $(SCENARIO_APP_OBJS) $(APPL_OBJS) $(TRUSTZONE_SEC_OBJS) $(NSC_OBJS) $(OS_ALLOBJS) $(OS_HAL_ALLOBJS) $(TFM_OBJS) $(INTERFACE_OBJS) $(LD_START_GROUPLIB) $(HMX_BSP_LIBS) $(INFERENCE_ENGINE_LIB) $(TFM_LIBS) $(CV_LIB) $(AUDIOALGO_LIB) $(EXTRA_LIBS) $(LD_SYSTEMLIBS) $(LD_END_GROUPLIB) -o $@
	$(Q)$(SIZE) $(APPL_FULL_NAME).$(ELF_FILENAME)
ifeq ($(VALID_TOOLCHAIN), arm)
	$(Q)$(OBJCOPY) --elf --output=$(APPL_FULL_NAME).elf $(APPL_FULL_NAME).$(ELF_FILENAME)
endif
ifeq ($(TRUSTZONE), y)
ifeq ($(TRUSTZONE_TYPE), security)
ifeq ($(TRUSTZONE_FW_TYPE), 0)
	$(CP) $(TRUSTZONE_SEC_LIB_WILDCARD) $(NONTRUSTZONE_SRC_SEC_LIB_WILDCARD)
	$(CP) $(TRUSTZONE_SEC_LIB_WILDCARD) $(NSC_PREBUILT_LIB)
endif
endif
endif


#########RULES FOR PRE COMPILE LINK SCRIPT#####
$(PRE_LINKER_SCRIPT_FILE): $(LINKER_SCRIPT_FILE)
	$(TRACE_COMPILE)
ifeq ($(VALID_TOOLCHAIN), gnu)
	$(Q)$(CC) $(ARCH_FLAGS) $(ALL_DEFINES) $(ALL_INCLUDES) -E -P -xc "$<" -MMD -MF"$(basename $@).d" -MT"$@" -o "$@"
else
	$(Q)$(CC) $(ARCH_FLAGS) $(ALL_DEFINES) $(ALL_INCLUDES) -E -xc "$<" -MMD -MF"$(basename $@).d" -MT"$@" -o "$@"
endif

#####APPLICATION C/ASM/CPP SOURCE FILE COMPILING RULES#####

#####DEFAULT C/ASM/CPP SOURCE FILE COMPILING RULES FOR CPU/BOARD/LIBRARY/MIDDLEWARE/OS/EXTRA#####
.SECONDEXPANSION:
$(OUT_DIR)/%.o :$(EPII_ROOT)/%.c $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) $(COMPILE_OPT) -o "$@" "$<"

.SECONDEXPANSION:
$(OUT_DIR)/%.o :$(EPII_ROOT)/%.cpp $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CXX) $(CXX_COMPILE_OPT) -o "$@" "$<"

.SECONDEXPANSION:
$(OUT_DIR)/%.o :$(EPII_ROOT)/%.cc $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CXX) $(CXX_COMPILE_OPT) -o "$@" "$<"

.SECONDEXPANSION:
$(OUT_DIR)/%.o :$(EPII_ROOT)/%.s $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) $(ASM_OPT) -o "$@" "$<"
	
.SECONDEXPANSION:
$(OUT_DIR)/%.o :$(EPII_ROOT)/%.S $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) $(ASM_OPT) -o "$@" "$<"

#####RULES FOR CREATING REQUIRED DIRECTORIES#####
%/.mkdir_done:
	$(TRACE_CREATE_DIR)
	$(TRY_MK_OBJDIR)
	@$(ECHO) $(@D) > $@

#####RULES FOR GENERATING DUMP/DASM/BIN/HEX FILE#####
$(APPL_FULL_NAME).dump : $(APPL_FULL_NAME).$(ELF_FILENAME)
	@$(ECHO) "Dumping $<"
	$(Q)$(DMP) $(DMP_OPT) $<  > $@

$(APPL_FULL_NAME).dasm : $(APPL_FULL_NAME).$(ELF_FILENAME)
	@$(ECHO) "Disassembling $<"
ifeq ($(VALID_TOOLCHAIN), arm)
	$(Q)$(DMP) $(DASM_OPT) --output=$@ $< 
else
	$(Q)$(DMP) $(DASM_OPT) $<  > $@
endif

$(APPL_FULL_NAME).bin : $(APPL_FULL_NAME).$(ELF_FILENAME)
	@$(ECHO) "Generating Binary $@"
	$(Q)$(OBJCOPY) $(ELF2BIN_INOPT) $< $(ELF2BIN_OUTOPT) $@

$(APPL_FULL_NAME).hex : $(APPL_FULL_NAME).$(ELF_FILENAME)
	@$(ECHO) "Generating Intel Hex File $@"
	$(Q)$(ELF2HEX) $(ELF2HEX_INOPT) $< $(ELF2HEX_OUTOPT) $@

#####RULES FOR WORKAROUNDS#####
<built-in> :
	@$(ECHO) "Rules to fix built-in missing caused by Himax"
