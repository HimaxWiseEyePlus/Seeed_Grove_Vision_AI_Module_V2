SCENARIO_APP_SUPPORT_LIST := $(APP_TYPE)

APPL_DEFINES += -DEI_STANDALONE_INFERENCING_CAMERA
APPL_DEFINES += -DEI_PORTING_HIMAX_WE2
APPL_DEFINES += -DEIDSP_LOAD_CMSIS_DSP_SOURCES
APPL_DEFINES += -DEI_ETHOS
APPL_DEFINES += -DETHOSU_ARCH=u55
APPL_DEFINES += -DETHOSU55
APPL_DEFINES += -DETHOS_U
APPL_DEFINES += -DIP_xdma
APPL_DEFINES += -DEVT_DATAPATH

EVENTHANDLER_SUPPORT = event_handler
EVENTHANDLER_SUPPORT_LIST += evt_datapath

##
# library support feature
# Add new library here
# The source code should be loacted in ~\library\{lib_name}\
##
LIB_SEL = pwrmgmt sensordp hxevent img_proc

##
# middleware support feature
# Add new middleware here
# The source code should be loacted in ~\middleware\{mid_name}\
##
MID_SEL =

override undefine OS_SEL
override TRUSTZONE := y
override TRUSTZONE_TYPE := security
override TRUSTZONE_FW_TYPE := 1
override EPII_USECASE_SEL := drv_onecore_cm55m_s
override CIS_SEL := HM_COMMON

CIS_SUPPORT_INAPP = cis_sensor
CIS_SUPPORT_INAPP_MODEL = cis_ov5647

ifeq ($(CIS_SUPPORT_INAPP_MODEL), cis_imx219)
APPL_DEFINES += -DCIS_IMX
else ifeq ($(CIS_SUPPORT_INAPP_MODEL), cis_imx477)
APPL_DEFINES += -DCIS_IMX
else ifeq ($(CIS_SUPPORT_INAPP_MODEL), cis_imx708)
APPL_DEFINES += -DCIS_IMX
endif

ifeq ($(strip $(TOOLCHAIN)), arm)
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/ei_standalone_inferencing_camera.sct
else
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/ei_standalone_inferencing_camera.ld
endif

SCENARIO_APP_SUPPORT_LIST +=	ei_standalone_inferencing_camera/ei-model/tflite-model \
								ei_standalone_inferencing_camera/ei-model \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/classifier \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/BasicMathFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/BayesFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/CommonTables \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/ComplexMathFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/ControllerFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/DistanceFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/FastMathFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/FilteringFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/InterpolationFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/MatrixFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/QuaternionMathFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/SVMFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/StatisticsFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/SupportFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/TransformFunctions \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/porting \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/porting/himax-we2 \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/dsp/kissfft \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/dsp/dct \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/dsp/image \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/tensorflow/lite/ \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/tensorflow/lite/c \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/tensorflow/lite/core/api \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/tensorflow/lite/kernels \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/tensorflow/lite/kernels/internal \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/tensorflow/lite/micro \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/tensorflow/lite/micro/kernels \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/tensorflow/lite/micro/memory_planner \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/porting/ethos-core-driver/include \
								ei_standalone_inferencing_camera/ei-model/edge-impulse-sdk/porting/ethos-core-driver/src
