SCENARIO_APP_SUPPORT_LIST := $(APP_TYPE)

APPL_DEFINES += -DEI_STANDALONE_INFERENCING
APPL_DEFINES += -DEI_PORTING_HIMAX_WE2
APPL_DEFINES += -DEIDSP_LOAD_CMSIS_DSP_SOURCES
APPL_DEFINES += -DEI_ETHOS
APPL_DEFINES += -DETHOSU_ARCH=u55
APPL_DEFINES += -DETHOSU55
APPL_DEFINES += -DETHOS_U

##
# library support feature
# Add new library here
# The source code should be loacted in ~\library\{lib_name}\
##
LIB_SEL =

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

ifeq ($(strip $(TOOLCHAIN)), arm)
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/ei_standalone_inferencing.sct
else
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/ei_standalone_inferencing.ld
endif

SCENARIO_APP_SUPPORT_LIST +=	ei_standalone_inferencing/ei-model/tflite-model \
								ei_standalone_inferencing/ei-model \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/classifier \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/BasicMathFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/BayesFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/CommonTables \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/ComplexMathFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/ControllerFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/DistanceFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/FastMathFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/FilteringFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/InterpolationFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/MatrixFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/QuaternionMathFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/SVMFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/StatisticsFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/SupportFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/TransformFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/NN/Source/ActivationFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/NN/Source/BasicMathFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/NN/Source/ConcatenationFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/NN/Source/ConvolutionFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/NN/Source/FullyConnectedFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/NN/Source/NNSupportFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/NN/Source/PoolingFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/NN/Source/ReshapeFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/NN/Source/SVDFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/CMSIS/NN/Source/SoftmaxFunctions \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/porting \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/porting/himax-we2 \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/porting/ethos-core-driver/include \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/porting/ethos-core-driver/src \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/dsp/kissfft \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/dsp/dct \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/dsp/image \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/tensorflow/lite/ \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/tensorflow/lite/c \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/tensorflow/lite/core/api \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/tensorflow/lite/kernels \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/tensorflow/lite/kernels/internal \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/tensorflow/lite/micro \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/tensorflow/lite/micro/kernels \
								ei_standalone_inferencing/ei-model/edge-impulse-sdk/tensorflow/lite/micro/memory_planner \

