##
# \defgroup	MK_OS_FREERTOS	FreeRTOS Related Makefile Configurations
# \ingroup	MK_OS
# \brief	makefile related to freertos os configurations
##

##
# \brief	current os directory definition
##
OS_HAL_FREERTOS_DIR += $(OSES_ROOT_DIR)/OS_HAL/
OS_HAL_FREERTOS_CFG_DIR += $(OSES_ROOT_DIR)/OS_HAL/config


				
##
# \brief 		freertos os related source and header
##
OS_HAL_FREERTOS_CSRCDIR	= $(OS_HAL_FREERTOS_DIR)/Source $(OS_HAL_FREERTOS_DIR)/Source/ARM
OS_HAL_FREERTOS_INCDIR	= $(OS_HAL_FREERTOS_DIR)/Include $(OS_HAL_FREERTOS_DIR)/Include1 $(OS_HAL_FREERTOS_CFG_DIR)

# find all the source files in the target directories
OS_HAL_FREERTOS_CSRCS = $(call get_csrcs, $(OS_HAL_FREERTOS_CSRCDIR))

# get object files
OS_HAL_FREERTOS_COBJS = $(call get_relobjs, $(OS_HAL_FREERTOS_CSRCS))
OS_HAL_FREERTOS_OBJS = $(OS_HAL_FREERTOS_COBJS)

# get dependency files
OS_HAL_FREERTOS_DEPS = $(call get_deps, $(OS_HAL_FREERTOS_OBJS))

# extra macros to be defined
OS_HAL_FREERTOS_DEFINES = -DFREERTOS_OSHAL


# genearte library
OS_HAL_LIB_FREERTOS = $(OUT_DIR)/liboshal.a

# specific compile rules
# user can add rules to compile this OS
# if not rules specified to this OS, it will use default compiling rules

# OS Definitions
OS_HAL_INCDIR += $(OS_HAL_FREERTOS_INCDIR)
OS_HAL_CSRCDIR += $(OS_HAL_FREERTOS_CSRCDIR)


OS_HAL_CSRCS += $(OS_HAL_FREERTOS_CSRCS)
OS_HAL_ALLSRCS += $(OS_HAL_FREERTOS_CSRCS)

OS_HAL_COBJS += $(OS_HAL_FREERTOS_COBJS)
OS_HAL_ALLOBJS += $(OS_HAL_FREERTOS_OBJS)

OS_HAL_DEFINES += $(OS_HAL_FREERTOS_DEFINES)
OS_HAL_DEPS += $(OS_HAL_FREERTOS_DEPS)
OS_HAL_LIBS += $(OS_HAL_LIB_FREERTOS)
