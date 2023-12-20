# middlewares root declaration
MIDDLEWARES_ROOT = $(EPII_ROOT)/middleware

MID_INCDIR =
MID_CSRCDIR =
MID_CXXSRCDIR =
MID_ASMSRCDIR =

MID_CSRCS =
MID_CXXSRCS =
MID_ASMSRCS =
MID_ALLSRCS =

MID_COBJS =
MID_CXXOBJS =
MID_ASMOBJS =
MID_ALLOBJS =

MID_DEFINES =
MID_DEPS =
MID_LIBS =

override MID_SEL := $(strip $(MID_SEL)) $(MID_REQUIRED)

ifdef MID_SEL
	override MID_SEL := $(strip $(MID_SEL))
	MID_SEL_SORTED = $(sort $(MID_SEL))
	MID_MKS = $(foreach MID, $(MID_SEL_SORTED), $(join $(MID)/, $(MID).mk))
	MID_INCLUDES = $(foreach MID_MK, $(MID_MKS), $(wildcard $(addprefix $(MIDDLEWARES_ROOT)/, $(MID_MK))))
	include $(MID_INCLUDES)

	# include dependency files
	ifneq ($(MAKECMDGOALS),clean)
	-include $(MID_DEPS)
	endif
endif