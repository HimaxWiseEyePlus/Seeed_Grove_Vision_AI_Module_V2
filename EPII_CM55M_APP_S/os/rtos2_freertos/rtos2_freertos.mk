
#rtos2_freertos/10.5.1 copy from ${LOCALAPPDATA}/Arm/Packs/ARM/CMSIS-FreeRTOS (pack release version="10.5.1")
#rtos2_freertos/10.5.1/CMSIS/RTOS2/Include copy from ${LOCALAPPDATA}/Arm/Packs/ARM/CMSIS/5.9.0/CMSIS/RTOS2/Include

OS_RTOS2_FREERTOS_CSRCDIR =
OS_RTOS2_FREERTOS_ASMSRCDIR =
OS_RTOS2_FREERTOS_INCDIR =
OS_RTOS2_FREERTOS_CSRCS =
OS_RTOS2_FREERTOS_ASMSRCS =

# TRUSTZONE support y or n
# TRUSTZONE_TYPE security or non-security
# TRUSTZONE_FW_TYPE 0(S and NS) or 1(S only)

ifeq ($(TRUSTZONE),y)
  ifeq ($(TRUSTZONE_FW_TYPE),0)
    ifeq ($(TRUSTZONE_TYPE),security)
tmp_TZ_S_NonOS = 1
    else
tmp_TZ_NS_OS = 1
tmp_RTOS2 = 1
    endif
  else
tmp_S_ONLY_OS = 1
tmp_RTOS2 = 1
  endif
else
tmp_S_ONLY_OS = 1
tmp_RTOS2 = 1
endif

ifdef tmp_RTOS2
OS_RTOS2_FREERTOS_CSRCDIR += $(OSES_ROOT_DIR)/10.5.1/Source
OS_RTOS2_FREERTOS_CSRCDIR += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC
endif

ifdef tmp_TZ_NS_OS
OS_RTOS2_FREERTOS_CSRCDIR += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55/non_secure
endif

ifdef tmp_TZ_S_NonOS
OS_RTOS2_FREERTOS_CSRCDIR += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55/secure
endif

ifdef tmp_RTOS2
ifeq ($(MPU),y)
OS_RTOS2_FREERTOS_CSRCDIR += $(OSES_ROOT_DIR)/10.5.1/Source/portable/Common
endif
endif

ifdef tmp_S_ONLY_OS
OS_RTOS2_FREERTOS_CSRCDIR += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55_NTZ/non_secure
endif

ifdef tmp_RTOS2
OS_RTOS2_FREERTOS_CSRCDIR += $(OSES_ROOT_DIR)/10.5.1/CMSIS/RTOS2/FreeRTOS/Source
OS_RTOS2_FREERTOS_CSRCDIR += $(OSES_ROOT_DIR)/10.5.1/CMSIS/RTOS2/FreeRTOS/Source/ARM
endif

ifdef tmp_RTOS2
OS_RTOS2_FREERTOS_INCDIR += $(OSES_ROOT_DIR)/10.5.1/CMSIS/RTOS2/Include
OS_RTOS2_FREERTOS_INCDIR += $(OSES_ROOT_DIR)/10.5.1/Source/include
OS_RTOS2_FREERTOS_INCDIR += $(OSES_ROOT_DIR)/10.5.1/CMSIS/RTOS2/FreeRTOS/Include
endif

ifdef tmp_TZ_NS_OS
OS_RTOS2_FREERTOS_INCDIR += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55/secure
OS_RTOS2_FREERTOS_INCDIR += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55/non_secure
endif
ifdef tmp_TZ_S_NonOS
OS_RTOS2_FREERTOS_INCDIR += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55/secure
endif
ifdef tmp_S_ONLY_OS
OS_RTOS2_FREERTOS_INCDIR += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55_NTZ/non_secure
endif

ifdef tmp_RTOS2
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/croutine.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/event_groups.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/list.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/queue.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/stream_buffer.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/tasks.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/timers.c
endif

ifdef tmp_TZ_NS_OS
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55/non_secure/port.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55/non_secure/portasm.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/portable/MemMang/heap_4.c
endif

ifdef tmp_TZ_S_NonOS
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55/secure/secure_context.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55/secure/secure_context_port.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55/secure/secure_heap.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55/secure/secure_init.c
endif

ifdef tmp_S_ONLY_OS
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55_NTZ/non_secure/port.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/portable/GCC/ARM_CM55_NTZ/non_secure/portasm.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/portable/MemMang/heap_4.c
endif

ifdef tmp_RTOS2
ifeq ($(MPU),y)
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/Source/portable/Common/mpu_wrappers.c
endif
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/CMSIS/RTOS2/FreeRTOS/Source/cmsis_os2.c
#OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/CMSIS/RTOS2/FreeRTOS/Source/freertos_evr.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/CMSIS/RTOS2/FreeRTOS/Source/os_systick.c
OS_RTOS2_FREERTOS_CSRCS += $(OSES_ROOT_DIR)/10.5.1/CMSIS/RTOS2/FreeRTOS/Source/ARM/clib_arm.c
endif


# get object files
OS_RTOS2_FREERTOS_COBJS = $(call get_relobjs, $(OS_RTOS2_FREERTOS_CSRCS))
OS_RTOS2_FREERTOS_ASMOBJS = $(call get_relobjs, $(OS_RTOS2_FREERTOS_ASMSRCS))
OS_RTOS2_FREERTOS_OBJS = $(OS_RTOS2_FREERTOS_COBJS) $(OS_RTOS2_FREERTOS_ASMOBJS)

# get dependency files
OS_RTOS2_FREERTOS_DEPS = $(call get_deps, $(OS_RTOS2_FREERTOS_OBJS))

# extra macros to be defined
OS_RTOS2_FREERTOS_DEFINES = -DRTOS2_FREERTOS
ifeq ($(TRUSTZONE),y)
  ifeq ($(TRUSTZONE_FW_TYPE),0) #TRUSTZONE_FW_TYPE 0(S and NS) or 1(S only)
    ifeq ($(TRUSTZONE_TYPE),security)
    else
OS_RTOS2_FREERTOS_DEFINES += -DRTOS2_FREERTOS_NS
    endif
  else
OS_RTOS2_FREERTOS_DEFINES += -DRTOS2_FREERTOS_SECONLY
  endif
else
OS_RTOS2_FREERTOS_DEFINES += -DRTOS2_FREERTOS_SECONLY
endif

# genearte library
OS_LIB_RTOS2_FREERTOS = $(OUT_DIR)/librtos2_freertos.a

# specific compile rules
# user can add rules to compile this OS
# if not rules specified to this OS, it will use default compiling rules

# OS Definitions
OS_INCDIR += $(OS_RTOS2_FREERTOS_INCDIR)
OS_CSRCDIR += $(OS_RTOS2_FREERTOS_CSRCDIR)
OS_ASMSRCDIR += $(OS_RTOS2_FREERTOS_ASMSRCDIR)

OS_CSRCS += $(OS_RTOS2_FREERTOS_CSRCS)
OS_CXXSRCS +=
OS_ASMSRCS += $(OS_RTOS2_FREERTOS_ASMSRCS)
OS_ALLSRCS += $(OS_RTOS2_FREERTOS_CSRCS) $(OS_RTOS2_FREERTOS_ASMSRCS)

OS_COBJS += $(OS_RTOS2_FREERTOS_COBJS)
OS_CXXOBJS +=
OS_ASMOBJS += $(OS_RTOS2_FREERTOS_ASMOBJS)
OS_ALLOBJS += $(OS_RTOS2_FREERTOS_OBJS)

OS_DEFINES += $(OS_RTOS2_FREERTOS_DEFINES)
OS_DEPS += $(OS_RTOS2_FREERTOS_DEPS)
OS_LIBS += $(OS_LIB_RTOS2_FREERTOS)
