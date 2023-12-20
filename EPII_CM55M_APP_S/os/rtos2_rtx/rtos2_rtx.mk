
#rtos2_rtx copy from ${LOCALAPPDATA}/Arm/Packs/ARM/CMSIS/5.9.0/CMSIS/RTOS2

OS_RTX_CSRCDIR =
OS_RTX_ASMSRCDIR =
OS_RTX_INCDIR =
OS_RTX_CSRCS =
OS_RTX_ASMSRCS =

OS_RTX_CSRCDIR += $(OSES_ROOT_DIR)/Source 
OS_RTX_CSRCDIR += $(OSES_ROOT_DIR)/RTX/Source
OS_RTX_ASMSRCDIR += $(OSES_ROOT_DIR)/RTX/Source/GCC

OS_RTX_INCDIR += $(OSES_ROOT_DIR)/Include 
OS_RTX_INCDIR += $(OSES_ROOT_DIR)/RTX/Include 
#OS_RTX_INCDIR += $(OSES_ROOT_DIR)/RTX/Config

OS_RTX_CSRCS += $(OSES_ROOT_DIR)/Source/os_systick.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Source/rtx_delay.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Source/rtx_evflags.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Source/rtx_evr.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Source/rtx_kernel.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Source/rtx_lib.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Source/rtx_memory.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Source/rtx_mempool.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Source/rtx_msgqueue.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Source/rtx_mutex.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Source/rtx_semaphore.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Source/rtx_system.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Source/rtx_thread.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Source/rtx_timer.c
OS_RTX_CSRCS += $(OSES_ROOT_DIR)/RTX/Config/RTX_Config.c

#GNUASM ARMv8-MML
OS_RTX_ASMSRCS += $(OS_RTX_ASMSRCDIR)/irq_armv8mml.S

# get object files
OS_RTX_COBJS = $(call get_relobjs, $(OS_RTX_CSRCS))
OS_RTX_ASMOBJS = $(call get_relobjs, $(OS_RTX_ASMSRCS))
OS_RTX_OBJS = $(OS_RTX_COBJS) $(OS_RTX_ASMOBJS)

# get dependency files
OS_RTX_DEPS = $(call get_deps, $(OS_RTX_OBJS))

# extra macros to be defined
OS_RTX_DEFINES = -DRTOS2_RTX
ifeq ($(TRUSTZONE),y)
  ifeq ($(TRUSTZONE_FW_TYPE),0) #TRUSTZONE_FW_TYPE 0(S and NS) or 1(S only)
    ifeq ($(TRUSTZONE_TYPE),security)
    else
OS_RTX_DEFINES += -DRTOS2_RTX_NS
    endif
  else
OS_RTX_DEFINES += -DRTOS2_RTX_SECONLY
  endif
else
OS_RTX_DEFINES += -DRTOS2_RTX_SECONLY
endif

# genearte library
OS_LIB_RTX = $(OUT_DIR)/librtx.a

# specific compile rules
# user can add rules to compile this OS
# if not rules specified to this OS, it will use default compiling rules

# OS Definitions
OS_INCDIR += $(OS_RTX_INCDIR)
OS_CSRCDIR += $(OS_RTX_CSRCDIR)
OS_ASMSRCDIR += $(OS_RTX_ASMSRCDIR)

OS_CSRCS += $(OS_RTX_CSRCS)
OS_CXXSRCS +=
OS_ASMSRCS += $(OS_RTX_ASMSRCS)
OS_ALLSRCS += $(OS_RTX_CSRCS) $(OS_RTX_ASMSRCS)

OS_COBJS += $(OS_RTX_COBJS)
OS_CXXOBJS +=
OS_ASMOBJS += $(OS_RTX_ASMOBJS)
OS_ALLOBJS += $(OS_RTX_OBJS)

OS_DEFINES += $(OS_RTX_DEFINES)
OS_DEPS += $(OS_RTX_DEPS)
OS_LIBS += $(OS_LIB_RTX)
