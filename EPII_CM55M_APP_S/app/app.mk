##
# application source dirs
##
override ALGO_TYPE := $(strip $(ALGO_TYPE))
#override LINKER_SCRIPT_FILE := $(strip $(LINKER_SCRIPT_FILE))

APPL_CSRC_DIR = $(EPII_ROOT)/app
APPL_ASMSRC_DIR = $(EPII_ROOT)/app
APPL_CXXSRC_DIR = $(EPII_ROOT)/app
APPL_CCSRC_DIR = $(EPII_ROOT)/app
APPL_INC_DIR = $(EPII_ROOT)/app 

### Himax Internal Use #####
ifneq ("$(findstring CLI,$(strip $(APP_TYPE)))", "")
include $(EPII_ROOT)/app/integrate/integrate.mk
endif
ifneq ("$(findstring HWACCBYTPG,$(strip $(APP_TYPE)))", "")
include $(EPII_ROOT)/app/hwaccautotest/hwaccautotest.mk
endif

### For Product application ####

# CGP choose where to look for the apps!
ifeq (${PROJECT_FOLDER}, scenario_app)
include $(EPII_ROOT)/app/scenario_app/scenario_app.mk
else
include $(EPII_ROOT)/app/ww_projects/ww.mk
endif