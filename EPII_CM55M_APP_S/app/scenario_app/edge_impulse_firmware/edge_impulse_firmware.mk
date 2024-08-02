SCENARIO_APP_SUPPORT_LIST := $(APP_TYPE)

APPL_DEFINES += -DEDGE_IMPULSE_FIRMWARE
APPL_DEFINES += -DEI_PORTING_HIMAX_WE2
APPL_DEFINES += -DEIDSP_LOAD_CMSIS_DSP_SOURCES
APPL_DEFINES += -DIP_xdma
APPL_DEFINES += -DEVT_DATAPATH
APPL_DEFINES += -DEI_ETHOS
APPL_DEFINES += -DETHOSU_ARCH=u55
APPL_DEFINES += -DETHOSU55
APPL_DEFINES += -DETHOS_U

EVENTHANDLER_SUPPORT = event_handler
EVENTHANDLER_SUPPORT_LIST += evt_datapath

##
# library support feature
# Add new library here
# The source code should be loacted in ~\library\{lib_name}\
##
LIB_SEL = sensordp hxevent pwrmgmt img_proc spi_eeprom

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
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/edge_impulse_firmware.sct
else
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/edge_impulse_firmware.ld
endif

SCENARIO_APP_SUPPORT_LIST +=	edge_impulse_firmware/src \
								edge_impulse_firmware/inference \
								edge_impulse_firmware/firmware-sdk \
								edge_impulse_firmware/firmware-sdk/jpeg \
								edge_impulse_firmware/firmware-sdk/at-server \
								edge_impulse_firmware/firmware-sdk/sensor-aq \
								edge_impulse_firmware/firmware-sdk/QCBOR/inc \
								edge_impulse_firmware/firmware-sdk/QCBOR/src \
								edge_impulse_firmware/ei-model \
								edge_impulse_firmware/ei-model/tflite-model \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/classifier \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/BasicMathFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/BayesFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/CommonTables \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/ComplexMathFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/ControllerFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/DistanceFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/FastMathFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/FilteringFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/InterpolationFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/MatrixFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/QuaternionMathFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/SVMFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/StatisticsFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/SupportFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/TransformFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/ActivationFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/BasicMathFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/ConcatenationFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/ConvolutionFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/FullyConnectedFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/NNSupportFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/PoolingFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/ReshapeFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/SVDFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/SoftmaxFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/dsp/kissfft \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/dsp/dct \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/dsp/image \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/porting \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/porting/himax-we2 \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/porting/ethos-core-driver/include \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/porting/ethos-core-driver/src \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/ \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/c \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/core/api \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/kernels \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/kernels/internal \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/micro \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/micro/kernels \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/micro/memory_planner

