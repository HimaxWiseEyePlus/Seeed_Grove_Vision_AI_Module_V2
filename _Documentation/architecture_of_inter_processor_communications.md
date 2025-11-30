Architecture of Inter-Processor Communications
==============================================
#### CGP 4 August 2025

This document, designed for software developers, introduces the elements, or sub-systems, in the Wildlife Watcher 
devices using the Himax AI processor. It then explains how the elements communicate, and how software developers can 
use and extend the communications features. The sub-systems are described next, and the communications mechanism 
is described later. The document describes:
- The WW130.B00 plus Seeed Grove Vision AI V2 (two-board solution for early development).
- The WW500 (single-board solution).

There are three elements to consider here:

* HX6538 processor (initially the Seeed Grove Vision AI V2, and later a component on the WW500). 
The HX6538 is referred to below as the "AI processor".
* Mokotech MKL62BA module which does BLE and LoRaWAN comms (initially the WW130.B00, and later a component on the WW500).
The MKL62BA module is based on a Nordic Semiconductor nRF52836 processor. 
The MKL62BA is referred to below as the "BLE processor".
* Smart phone running an app.

The elements and their methods of interacting are described next.

## HX6538 (AI processor) "Wise Eye" Processor Subsystem

The circuitry surrounding the HX6538 processor implements (or will implement):
* One or more trigger devices
* One or more image sensors
* Image illumination
* Neural network (NN) hardware accelerator
* SD card storage 
* Communications with the Mokotech MKBL62BA module (I2C and bi-directional interrupt)
* The processor is accompanied by a 16M byte serial eeprom for storage of firmware and neural network models.

The [HX6538](https://www.himax.com.tw/products/wiseeye-ai-sensing/wiseeye2-ai-processor/) processor is an 
ARM processor with neural network accelerators and a camera interface.

## MKL62BA (BLE Processor) subsystem

The [MKL62BA module](https://www.mokolora.com/lorawan-module-mkl62ba/) has these features:
* Nordic Semiconductor nRF52832 processor. This is an ARM processor with an integral BLE engine.
* Bluetooth Low Energy (BLE) function with onboard antenna.
* LoRa radio using the Semtech Sx1262 radio chip. This requires an external antenna.

The purpose of the BLE processor is to provide local area and wide area communications.

By providing a BLE comms option we can develop a smart phone app that can configure the system
in the field and in the office.  

The [LoRaWAN communications protocol](https://docs.aws.amazon.com/iot-wireless/latest/developerguide/what-is-lorawan.html) 
provides low power comms that can work at several kilometers in an open environment - much less in hilly areas
or vegetation. It has a star topology, requiring an always-on gateway, with a wide-area (internet)
connection at the centre of the star. 

In due course it might be possible to add mesh networking capabilities, such as with the [Neocortec software](https://neocortec.com/).
Neocortec probably have a port for the nRF52832/Sx1262 combination.

## Smart Phone App

Some development has begun on an app, though there has been little recent progress (at the time of writing). 

I have elected to implement the [Nordic UART Service](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/libraries/bluetooth_services/services/nus.html)
 (NUS) as the primary communication protocol between the BLE stack and the app. This is a simple protocol that allows transfer of 
 blocks of data - either text or binary data.
 
Using the NUS service allows simple communications during development. I have also cloned the NUS service and given it a different UUID 
(service ID) that our app can work with as an alternative. I term this the Wildlife Watcher Uart Service (WWUS).
For initial development, you can use the [nRFToolbox app](https://www.nordicsemi.com/Products/Development-tools/nRF-Toolbox)
that allows exchange of text messages with the device, using the NUS.

## Two hardware Implementations

#### (1) The Early Development Setup

The WW130 module was designed as an initial development setup. The main PCB includes a MKL62BA and pins are provided to allow
a [Grove Vision AI V2](https://wiki.seeedstudio.com/grove_vision_ai_v2/) to plug on - this module provides the AI processor.

The WW130 implementation has no hardware trigger devices or illumination features. It has a single Raspberry Pi image sensor 
(Omnivision [OV5647](https://cdn.sparkfun.com/datasheets/Dev/RaspberryPi/ov5647_full.pdf) with a 2592 x 1944 maximum resolution). 
Many camera modules are available that use this sensor - [for example here](https://docs.arducam.com/Raspberry-Pi-Camera/Native-camera/5MP-OV5647/)
and other image sensors are available that could (in principle) be plugged into the same connector.
The Raspberry Pi camera interface is described [here.](https://www.raspberrypi.com/documentation/accessories/camera.html)

The Grove Vision AI V2 plugs onto a WW130.B00 board. This board includes the MKL62BA module. This module uses the Nordic Semiconductor nRF52832
ARM processor with internal BLE comms, plus a Semtech LoRa radio, capable of LoRaWAN communications. 

 The WW130 has the following additional features (not all of which 
are necessarily in place):
* Interface to the WWIF100 development board which carries 3 LEDs, 3 switches, 5V to 3.3V regulator and a serial port connector
for the [TTL-232R-3V3](https://ftdichip.com/products/ttl-232r-3v3/) USB cable.
* Clips for two AA batteries.
* Low-power magnetic sensors(s).
* GPS receiver (typically not fitted).
* Accelerometer (typically not fitted).
* [Qwiik](https://www.sparkfun.com/qwiic) connector for external power and I2C peripherals.

#### (2) WW500

This is our own single-board solution which includes the AI processor and the BLE processor.

The WW500 has provision to add an [HM0360](https://www.himax.com.tw/products/cmos-image-sensor/always-on-vision-sensors/hm0360/) 
image sensor which has a low-power ("always on") motion sensing capability. It retains the Raspberry Pi camera connector
which should allow a wide selection of image sensors to be added. Some form of visible or infra-red illumination circuitry can
be provided externally. (Himax already offer the [ISM028-03M0098](https://www.himax.com.tw/products/wiseeye-ai-sensing/wiseeye-solutions/) 
product which combines the AI processor and the HM0360. It is not really useable in our applications, but it shows it can be done.)

## Typical Operation

The hardware described above is typically programmed to behave as follows. Something will wake the AI processor and causes it to take picture(s). The processor can then decide
to process the image through its neural network accelerator. It can decide to save the image to the SD card
(typically using its JPEG accelerator), and it can decide to communicate with the MKBL62BA, which can send messages to the cloud.

As of the time of writing (August 2025) the HM0360 performs motion detection. Either the HM0360 or a Raspberry Pi camera is the 
"main" camera that takes images.

In this typical scenario an HM0360 motion sensor runs at one or two frames per second to detect movement. 
This then wakes the AI processor processor
which can turn on illumination and take an image - either using the HM0360 or a RP camera.
The processor then processes the image with its NN model and saves the image to the SD card, as a JPEG image (with embedded EXIF metadata).
The AI processor sends messages to the BLE processor which are passed to the app via BLE (if connected).
The BLE processor also sends some metadata to the cloud, where it can be recorded and displayed, such as on the trap.nz website.

Communications Protocol
--------------------------
### Examples of the System in Action

Before looking at the technology, it is worth examining how the system works at the time of writing. The description that follows assumes you are 
using the nRFToolbox app and have selected the UART sub-app. At the time of writing the WW130 and WW500 will start 
BLE advertising by pressing SW1, or by moving a magnet near the magnetic sensor (IC2 on the WW130 PCB edge, or IC4 on the WW500). 
At this point the device appears in the phone and you can click it to establish a BLE connection.

Any text typed in the app will be sent to the nRF52832 (in the BLE processor). The nRF52832 software will 
look for certain strings and, if there is a match, pass these to functions which interpret them and send a response.
The response is then displayed on the phone. Examples are:

| Command   | Function                                             | 
| ----------|------------------------------------------------------|
| id        | Print the device's BLE name                          | 
| ver       | Print the hardware and software version              |
| status    | Prints some status                                   |
| enable    | Enables the magnetic sensor to send LoRaWAN messages |
| disable   | Disables the magnetic sensor                         | 
| xxx       | Returns "unrecognised" for unrecognised commands     | 

(More commands are documented in [ble_commands.md](ble_commands.md).

Any command the begins "AI " results in the rest of the line being sent to the AI processor. The BLE processor sends "AI ACK" 
to the phone (or "AI NACK" in case of a transmission error).
The AI processor then looks for commands it understands and sends a response. This response is also sent on the the app.
Examples of commands to the AI processor include:

| Command               | Function                                              | 
| ----------------------|-------------------------------------------------------|
| AI status             | Prints some status                                    | 
| AI info               | Prints some information about the SD card             | 
| AI dir                | Prints the SD card directory listing                  | 
| AI type \<file\>      | Reads a file and sends it to the phone                | 
| AI fileread \<file\>  | Reads a file on the SD card and returns info about it | 
| AI filewrite \<file\> | Reads test data to a file and returns info about it   |  
| AI xxx                | Returns "unrecognised" for unrecognised commands      | 

I have implemented a command line interface (CLI) in the AI processor firmware, and its commands can be seen by typing "help".
I have routed the messages that come from the phone via the BLE processor into the CLI mechanism, so every command can be 
executed by typing at the keyboard or typing at the phone. To understand this you should have a play and look at the results. 
Also look at the source code of the ``CLI-commands.c`` and ``CLI-FATFS-commands.c`` files.

To a large extent, the functionality of the Wildlife Watcher system, including the app, can be established by 
extending these commands. Understanding this, with the help of the sections below, will help understand the communications protocol. 

## Hardware

The functionality described above relies on the hardware described below. The material below here is not needed
to implement app software. But is it provided for those who need to write code on either of the two 
processors.

The AI processor acts as an I2C slave, at address 0x62. So the BLE processor has to initiate communications - either writing to the 
AI processor or reading from it. If the AI processor needs to initiate a transfer of data to the BLE processor it generates an interrupt
using the bidirectional interrupt line (see below) to prompt the BLE processor to initiate the read. 

The BLE processor uses I2C instance 1 on pins P07 and P08. It runs at 400kHz clock speed. The BLE processor has a limit of 255 bytes in each
data exchange. Also, the BLE configuration has been set for an MTU of 247 bytes. The NUS protocol has a 3 byte header, giving a usable 
packet size of 244 bytes. At the time of writing the I2C maximum packet size has also been set at 244 bytes, so that packets of this size
can travel between the app to the AI processor without adjustment. (It might prove useful to add some header bytes to the BLE packets,
in which case the usable payload size might be a few bytes smaller).

There is an interrupt signal that connects GPIO pins on the two processors. Either processor can interrupt 
the other by taking that pin low. Both processors can assert the common interrupt pin, and both processors can be 
interrupted on either edge.

On the WW500m but not the WW130, there is an additional signal that allows the BLE processor to wake the AI processor when it is in deep power-down (DPD).

#### WW130 Implementation 

On the WW130 the inter-processor interrupt is P05 on the BLE processor, which is connected to PA0 on the AI processor. 
It also connects to SW2 on the WWIF100 board. Internal pull-up resistors are enabled (on both boards I think).

P05 on the BLE processor was previously allocated to one of the "buttons" and as such it had debouncing logic which meant it had to 
be asserted for tens of ms before the BLE processor saw an event. I have now removed it from the button set and configured it to 
generate an interrupt on both edges. It responds fast to pulses from the AI processor.

On the AI processor the PA0 pin is associated with AON_GPIO0. Code (in ``interprocessor_interrupt_init()``) initialises this as an interrupt
that responds to the falling edge. When the AI processor wants to interrupt the BLE processor
it calls ``interprocessor_interrupt_assert()``. This disables interrupts, sets the pin as an output, low.
Soon afterwards ``interprocessor_interrupt_negate()`` reverses this. (The code is shown below).
 
#### WW500 Implementation

On the WW500 the inter-processor interrupt is P18 on the BLE processor, which is connected to PB11 on the AI processor. 
It also connects to SW2 on the WWIF100 board. Internal pull-up resistors are enabled (on both boards I think).

The WW500 has an additional inter-processor signal that is absent on the WW130. This is the `/BLE_WAKE` signal which 
is used by the BLE processor to wake the AI processor when it is in deep power down (DPD). The active-low /BLE_WAKE signal
is an output on P05 on the BLE processor. It routes via gates to an active high WAKE signal on PA0 on the AI processor. 
The gates allow other wake signals - the sensor interrupt from the HM0360 and a radar interrupt if the radar module is fitted.
 
### Role of BLE processor

The BLE processor receives messages from the app and either operates on these, returning a response, 
or passes them to the AI processor if the messages start with "AI ". Messages originating from the BLE processor are largely:
* Responses to commands sent by the app.
* Spontaneous messages, such as those reporting LoRaWAN traffic.

Messages from the AI processor are, at the time of writing, all sent to the app, provided there is a BLE connection.
So, at the time of writing, the BLE processor serves as a transparent conduit between the app and the AI processor. 
Messages from the AI processor  are largely:
* Responses to commands sent by the app.
* Spontaneous messages, such as those reporting camera and neural network operation.
* Messages mainly intended for the BLE processor - in particular "Sleep" and "Wake" messages that indicate when the AI processor
enters and leaves DPD mode. 

(If the messages originating from the BLE processor or the AI processor are thought too "noisy" then I could change the 
code so only important messages were sent to the BLE processor. At present these can be helpful for development.)

#### Master-to-slave messages

The BLE processor is the I2C "master" and the AI processor is the I2C "slave. The I2C master must initiate
messages in both directions.

Messages from the phone that begin with "AI " have these 3 characters stripped and the rest of the string is sent
to the AI processor across the I2C interface. This communication is initiated by the BLE processor as I2C master. 
As discussed above, the messages have a maximum payload of 244 bytes.

On the AI processor, inter-processor communications is managed within the IfTask, implemented in ``if_task.c``. This file implements three
interrupt-driven callbacks:
* i2cs_cb_rx() - called when a packet arrives
* i2cs_cb_tx() - called when a packet from the AI processor has been sent
* i2cs_cb_err() - called on error conditions. This includes the situation that the I2C master attempts to read
from the AI processor before the AI processor has prepared a packet for it.

Payload data is preceded, in the I2C packet, by 4 header bytes, and followed by 2 CRC check bytes. 
(These 4 header bytes are an artifact of some pre-existing Himax code.)

The header includes a message type byte. At the time of writing the only incoming message that is processed is a string, 
and it is dispatched, after checking, to the CLI task in ``CLI-commands.c``. In the CLI task the messages are parsed 
as though they were typed at the console.

The BLE processor does not attempt the read a response until the AI processor has a response ready to send. 
Any response is sent asynchronously using the mechanism described in the 'Slave-to-master messages' section below.

#### Slave-to-master messages

As mentioned above, messages from the AI processor  are largely:
* Responses to commands sent by the app.
* Spontaneous messages, such as those reporting camera and neural network operation.
* Messages mainly intended for the BLE processor - in particular "Sleep" and "Wake" messages that indicate when the AI processor
enters and leaves DPD mode.

When the the AI processor has a message to send, it prepares data for the outgoing I2C system, and generates 
an interrupt to the BLE processor. On receipt of the interrupt the BLE processor, as the I2C master,
initiates an I2C read operation to rectrieve the data from the AI processor.

The AI processor code that sends a message is:

```c
static void sendI2CMessage(uint8_t * data, aiProcessor_msg_type_t messageType, uint16_t payloadLength) {
	interprocessor_interrupt_assert();
	i2ccomm_write_enable(data, messageType, payloadLength);
	interprocessor_interrupt_negate();	// MKL62BA responds on the rising edge.
}
``` 

The ``i2ccomm_write_enable()`` function prepares the outgoing I2C data, including the 4 byte header and 2-byte CRC.
The BLE processor receives an event as the interrupt pin goes high, with the ``aon_gpio0_drive_high()`` call. 
It then reads the packet prepared for it. At the time of writing any message is sent to the smart phone app, 
and the messageType byte indicates either that the payload contains a string (terminated by '\0') or binary data. 
In the future decisions can be made depending on the value of the messageType parameter. For example, other values 
of this byte could indicate that the message should be processed by the BLE processor.

At present, a short response is returned to the BLE processor by the AI processor very shortly after the BLE processor 
sends a packet to the AI processor. However, some responses are likely to exceed the 244 byte payload limit. 
In this case the AI processor calls ``sendI2CMessage()`` multiple times. Each call generates an interrupt and 
transfers (up to) 244 bytes. This matches the FreeRTOS CLI mechanism, which allows any single command typed at the 
console to return multiple strings. That is why, when the app user types ``AI dir`` (to do an SD card directory listing),
the result is a sequence of BLE packets, each containing a single line of the SD card directory listing. 
That is because the ``prvDirCommand()`` function is called many times by the CLI code, each time returning a single 
directory entry.

As a further example, consider two ways of reading a file that is greater than 244 bytes. One command treats the file as text
and the other as binary. 

In the text file case, use the ``AI type longfile.txt`` command. In this case the file is split into blocks of 243 bytes 
(a C string terminated by '\0').

In the binary file case, use the ``AI read longfile.txt`` command. In this case the file is split into blocks of 244 bytes.
The (binary) data sent to the nRFToolbox app may not be printable text, so expect trouble on the screen. When operating with
the Wildlife Watcher app this issue can be managed. (It is likely that a future version of the protocol might transfer 
the messageType and payloadLength parameters in the BLE messages, to help the app interpret the incoming messages). 

### Extending the Inter-processor Communications Protocol

As implied above, it is likely that the protocol may evolve variously:

* Passing header information in both directions in the BLE packets.
* Defining messages that don't involve the app and the BLE interface, but allow the BLE processor and the AI processor
to communicate privately.

In the short term, given the current state of the protocol, it should be possible for the app developer to make progress
with app functionality with zero or minimal changes to the BLE processor code. In other words:
* The app developer specifies a new command that begins "AI " (such as ``AI getmetadata``)
* The AI processor developer adds a new ``CLI_Command_Definition_t`` in ``CLI-commands.c`` that implements this request.
* The app developer parses the incoming response and acts upon it.

(Note to self: [Markdown Guide](https://confluence.atlassian.com/bitbucketserver083/markdown-syntax-guide-1155483368.html))

 


