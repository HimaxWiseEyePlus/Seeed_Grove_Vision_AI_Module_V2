allon_sensor_tflm - CGP 28/7/09
===================================

This app appears to run a person detect model.

You can see console output below, and match the text printed with the xprintf() lines in the source.


https://seeed-studio.github.io/SenseCraft-Web-Toolkit/#/setup/process

C:\Users\charl\OneDrive\Desktop\Himax_AI_web_toolkit\index.html


Output on the console:
--------------------------

1st BL Modem Build DATE=Nov 30 2023, 0x0002000b
Please input any key to enter X-Modem mode in 100 ms
waiting input key...
slot flash_offset 0x00100000
New MemDesp himax_sec_SB_image_process PASS
set_memory_s_ns
bl_status = 0x800000, HX_DSP_FLAG 1
bl_status = 0x800000
jump_addr=0x10000000
Compiler Version: ARM GNU, 10.3.1 20210824 (release)
wakeup_event=0x0,WakeupEvt1=0x0
Ethos-U55 device initialised
model's schema version 3
initial done
cis_OV5647_init
mclk DIV3, xshutdown_pin=82
Set PA1(AON_GPIO1) to High
hx_drv_cis_set_slaveID(0x36)
OV5647 Init Stream by app
WD1[340e05e0], WD2_J[340e05e0], WD3_RAW[3406fde0], JPAuto[3406fd60]
MIPI TX CLK: 96M
MIPI CSI Init Enable
MIPI TX CLK: 96M
MIPI BITRATE 1LANE: 440M
MIPI DATA LANE: 2
MIPI PIXEL DEPTH: 10
MIPI LINE LENGTH: 640
MIPI FRAME LENGTH: 480
MIPI CONTINUOUSOUT: 1
MIPI DESKEW: 0
t_input: 7387ns
t_output: 6666ns
t_preload: 385ns
MIPI RX FIFO FILL: 10
MIPI TX FIFO FILL: 0
RESET MIPI CSI RX/TX
VMUTE: 0x00000000
0x53061000: 0x0606070D
0x53061004: 0x2B07201C
0x53061008: 0x00000505
0x5306100C: 0x00005500
0x53061010: 0x00000000
sensor_type: 15
Event loop init done
OV5647 on by app done
g_cursensorstream: 0
Event loop start
EVT event = 10
SENSORDPLIB_STATUS_XDMA_FRAME_READY 1
write frame result 0, data size=5596,addr=0x340e05e0
invoke pass
person_score:-4
EVT event = 10
SENSORDPLIB_STATUS_XDMA_FRAME_READY 2
write frame result 0, data size=5584,addr=0x340e05e0
invoke pass
person_score:-2
EVT event = 10
SENSORDPLIB_STATUS_XDMA_FRAME_READY 3
write frame result 0, data size=5588,addr=0x340e05e0
invoke pass
person_score:-8
EVT event = 10
SENSORDPLIB_STATUS_XDMA_FRAME_READY 4
write frame result 0, data size=6232,addr=0x340e05e0
invoke pass
person_score:-34
EVT event = 10
SENSORDPLIB_STATUS_XDMA_FRAME_READY 5
write frame result 0, data size=6208,addr=0x340e05e0
invoke pass
person_score:-23

<keeps going forever>