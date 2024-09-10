Architecture of Inter-Processor Communications
==============================================
#### CGP 10 September 2024

This document introduces the elements, or sub-systems, in the Wildlife Watcher device, as they exist today. 
It then explains how the elements communicate, and how software developers can use and extend the 
communications features. The sub-systems are described next, and the communications mechanism is described later.

There are three elements to consider here:

* HX6538 processor (initially the Seeed Grove Vision AI V2)
* Mokotech MKL62BA module which does BLE and LoRaWAN comms (initially the WW130)
* Smart phone running an app.

The elements and their methods of interacting are described next.

### HX6538 processor subsystem

This implements (or will implement):
* One or more trigger devices
* One or more image sensors
* Image illumination
* Neural network (NN) hardware accelerator
* SD card storage 
* Communications with the Mokotech MKBL62BA module (I2C and bi-directional interrupt)
* The processor is accompanied by a 16M byte serial eeprom for storage of firmware and neural network models.

The [HX6538](https://www.himax.com.tw/products/wiseeye-ai-sensing/wiseeye2-ai-processor/) processor is an ARM processor 
with neural network accelerators and camera interface.

The development setup uses the [Grove Vision AI V2](https://wiki.seeedstudio.com/grove_vision_ai_v2/).
This includes no hardware trigger devices or illumination feature. It has a single Raspberry Pi image sensor 
(Omnivision [OV5647](https://cdn.sparkfun.com/datasheets/Dev/RaspberryPi/ov5647_full.pdf) with a 2592 x 1944 maximum resolution). 
Many camera modules are available that use this sensor - [for example here](https://docs.arducam.com/Raspberry-Pi-Camera/Native-camera/5MP-OV5647/).
The Raspberry Pi camera interface is described [here](https://www.raspberrypi.com/documentation/accessories/camera.html)

Our own single-board solution is likely to add an [HM0360](https://www.himax.com.tw/products/cmos-image-sensor/always-on-vision-sensors/hm0360/) 
image sensor which has a low-power ("always on") motion sensing capability. We will retain the Raspberry Pi camera connector
which should allow a wide selection of image sensors to be added. Some form of visible or infra-red illumination circuitry will
be provided.

In operation something will wake the HX6538 and causes it to take a picture. The processor can then decide
to process the image through its neural network accelerator. It can decide to save the image to the SD card
(typically using its JPEG accelerator), and it can decide to communicate with the MKBL62BA.

For example, an HM0360 motion sensor running at one or two frames per second could detect movement. It could wake the processor
which could turn on illumination and take an image. The processor could process the image with its NN model and depending on the
result of the analysis, decide to save the image as a JPEG image (with embedded EXIF metadata) and to interrupt the MKBL62BA
module to ask it to send some metadata to the cloud. 

### MKL62BA subsystem

The [MKL62BA module](https://www.mokolora.com/lorawan-module-mkl62ba/) has these features:
* Nordic Semiconductor nRF52832 processor. This is an ARM processor with an integral BLE engine.
* Bluetooth Low Enery (BLE) function.
* LoRa radio using the Semtech Sx1262 radio chip. This requires an external antenna.

The development setup uses my WW130 module. This has the following additional features (not all of which 
are necessarily in place):
* Interface to the WWIF100 developemnt board which carries 3 LEDs, 3 switches, 5V to 3.3V regulator and a serial port connector
for the [TTL-232R-3V3](https://ftdichip.com/products/ttl-232r-3v3/) USB cable.
* Clips for two AA batteries.
* Low-power magnetic sensors(s).
* GPS receiver.
* Accelerometer
* [Qwiik](https://www.sparkfun.com/qwiic) connector for external power and I2C peripherals

The purpose of the MKL62BA subsystem is to provide local area and wide area communications.

By providing a BLE comms option, we can develop a smart phone app that can configure the system
in the field and in the office.  

The [LoRaWAN communications protocol](https://docs.aws.amazon.com/iot-wireless/latest/developerguide/what-is-lorawan.html) 
provides low power comms that can work at several kilometers in an open environment - much less in hilly areas
or vegetation. It has a star topology, requiring an always-on gateway, with a wide-area (internet)
connection at the centre of the star. 
In due course it might be possible to add mesh networking capabilities, such as with the [Neocortec software](https://neocortec.com/).
Neocortec probably have a port for the nRF52832/Sx1262 combination.

### Smart Phone App

Some development has begun on an app, though there has been no recent progress (at the time of writing). 

I have elected to implement the [Nordic UART Service](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/libraries/bluetooth_services/services/nus.html)
 (NUS) as the primary communication protocol between the BLE stack and the app. This is a simple protocol that allows transfer of 
 blocks of data - either text or binary data.
 
Using the NUS service allows simple communications during development. I have also created a copy with a different UUID 
(service ID) that our app can work with as an alternative. For initial development, you can use the [nRFToolbox app](https://www.nordicsemi.com/Products/Development-tools/nRF-Toolbox)
that allows exchange of text messages with the device.

Communications Protocol
--------------------------

### Hardware
The HX6538 acts as an I2C slave, at address 0x62. So the nRF52832 has to initiate communications - either writing to the HX6538 or reading from it.
There is (currently) a single interrupt signal that connects GPIO pins on the two processors. Either processor can interrupt 
the other by taking that pin low.

The nRF52832 uses I2C instance 1 on pins P07 and P08. It runs at 400kHz clock speed. The nRF52832 has a limit of 255 bytes in each
data exchange. Also, the BLE configuration has been set for an MTU of 247 bytes. The NUS protocol has a 3 byte header, giving a usable 
packet size of 244 bytes. At the time of writing the I2C maximum packet size has also been set at 244 bytes, so that packets of this size
can travel from the app to the HX6538 without adjustment. (It might prove useful to add some header bytes in which case the usable 
payload size might be a few bytes smaller).

Both processors can assert the common interrupt pin, and both processors can be interrupted on either edge.

The interrupt in is P05 on the nRF52832. This is connected to PA0 on the Grove Vision AI V2. It also connects to
SW2 on the WWIF100 board. Interval pull-up resistors are enabled (on both boards I think).

P05 on the nRF52832 was previously allocated to one of the "buttons" and as such it had debouncing logic which meant it had to 
be asserted for tens of ms before the nRF52832 saw an event. I have now removed it from the button set and configured it to 
generate an interrupt on both edges. It responds fast to pulses from the HX6538.

Because I was using the SW2 pin to put the nRF52832 into DFU mode, for firmware updates, I have retained this feature, as follows.
I start a timer on the falling edge of P05 and measure the time that has elapsed at the rising edge. 
Intervals less that 1s are routed to interprocessor communications code. Intervals between 1s and 2s generate a message
(and could be used to execute some other code). Intervals >2s put the board in DFU mode.

On the HX6538 the PA0 pin is associated with AON_GPIO0. Code (in aon_gpio0_interrupt_init() initialises this as an interrupt
that responds to the falling edge. When the HX6538 wants to interrupt the nRF52832
it calls aon_gpio0_drive_low(). This disables interrupts, sets the pin as an output, low.
Soon afterwards aon_gpio0_drive_high() reverses this.

### 



  


(Note to self: [Markdown Guide](https://confluence.atlassian.com/bitbucketserver083/markdown-syntax-guide-1155483368.html))


