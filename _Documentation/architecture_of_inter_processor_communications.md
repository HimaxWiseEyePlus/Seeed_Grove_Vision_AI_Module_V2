Architecture of Inter-Processor Communications
==============================================
#### CGP 22 December 2024

This document, designed for software developers, introduces the elements, or sub-systems, in the Wildlife Watcher 
devices using the Himax HX6538. It then explains how the elements communicate, and how software developers can 
use and extend the communications features. The sub-systems are described next, and the communications mechanism 
is described later. The document describes:
- The WW130.B00 plus Seeed Grove Vision AI V2 (two-board solution for early development).
- The WW500 (single-board solution).

There are three elements to consider here:

* HX6538 processor (initially the Seeed Grove Vision AI V2, and later a component on the WW500).
* Mokotech MKL62BA module which does BLE and LoRaWAN comms (initially the WW130.B00, and later a component on the WW500).
* Smart phone running an app.

The elements and their methods of interacting are described next.

## HX6538 processor subsystem

This implements (or will implement):
* One or more trigger devices
* One or more image sensors
* Image illumination
* Neural network (NN) hardware accelerator
* SD card storage 
* Communications with the Mokotech MKBL62BA module (I2C and bi-directional interrupt)
* The processor is accompanied by a 16M byte serial eeprom for storage of firmware and neural network models.

The [HX6538](https://www.himax.com.tw/products/wiseeye-ai-sensing/wiseeye2-ai-processor/) processor is an 
ARM processor with neural network accelerators and a camera interface.

## MKL62BA subsystem

The [MKL62BA module](https://www.mokolora.com/lorawan-module-mkl62ba/) has these features:
* Nordic Semiconductor nRF52832 processor. This is an ARM processor with an integral BLE engine.
* Bluetooth Low Energy (BLE) function with onboard antenna.
* LoRa radio using the Semtech Sx1262 radio chip. This requires an external antenna.

The development setup uses my WW130 module. This has the following additional features (not all of which 
are necessarily in place):
* Interface to the WWIF100 development board which carries 3 LEDs, 3 switches, 5V to 3.3V regulator and a serial port connector
for the [TTL-232R-3V3](https://ftdichip.com/products/ttl-232r-3v3/) USB cable.
* Clips for two AA batteries.
* Low-power magnetic sensors(s).
* GPS receiver.
* Accelerometer
* [Qwiik](https://www.sparkfun.com/qwiic) connector for external power and I2C peripherals.

The purpose of the MKL62BA subsystem is to provide local area and wide area communications.

By providing a BLE comms option, we can develop a smart phone app that can configure the system
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
(service ID) that our app can work with as an alternative. I term this the Wildlife Watcher Unart Service (WWUS).
For initial development, you can use the [nRFToolbox app](https://www.nordicsemi.com/Products/Development-tools/nRF-Toolbox)
that allows exchange of text messages with the device, using the NUS.

## Two hardware Implementations

#### (1) The development setup

This uses the [Grove Vision AI V2](https://wiki.seeedstudio.com/grove_vision_ai_v2/).
This includes no hardware trigger devices or illumination features. It has a single Raspberry Pi image sensor 
(Omnivision [OV5647](https://cdn.sparkfun.com/datasheets/Dev/RaspberryPi/ov5647_full.pdf) with a 2592 x 1944 maximum resolution). 
Many camera modules are available that use this sensor - [for example here](https://docs.arducam.com/Raspberry-Pi-Camera/Native-camera/5MP-OV5647/)
and other image sensors are available that could (in principle) be plugged into the same connector.
The Raspberry Pi camera interface is described [here.](https://www.raspberrypi.com/documentation/accessories/camera.html)

The Grove Vision AI V2 plugs onto a WW130.B00 board. This board includes the MKL62BA module. This module uses the Nordic Semiconductor nRF52832
ARM processor with internal BLE comms, plus a Semtech LoRa radio, capable of LoRaWAN communications. 

#### (2) WW500

This is our own single-board solution which includes the HX6538 and the MKL62BA.

The WW500 has provision to add an [HM0360](https://www.himax.com.tw/products/cmos-image-sensor/always-on-vision-sensors/hm0360/) 
image sensor which has a low-power ("always on") motion sensing capability. It retains the Raspberry Pi camera connector
which should allow a wide selection of image sensors to be added. Some form of visible or infra-red illumination circuitry can
be provided externally. (Himax already offer the [ISM028-03M0098](https://www.himax.com.tw/products/wiseeye-ai-sensing/wiseeye-solutions/) 
product which combines the HX6538 and the HM0360. It is not really useable in our applications, but it shows it can be done.

## Expected operation

In operation something will wake the HX6538 and causes it to take a picture. The processor can then decide
to process the image through its neural network accelerator. It can decide to save the image to the SD card
(typically using its JPEG accelerator), and it can decide to communicate with the MKBL62BA.

For example, an HM0360 motion sensor running at one or two frames per second could detect movement. It could wake the processor
which could turn on illumination and take an image. The processor could process the image with its NN model and depending on the
result of the analysis, decide to save the image as a JPEG image (with embedded EXIF metadata) and to interrupt the MKBL62BA
module to ask it to send some metadata to the cloud.

Communications Protocol
--------------------------
### Examples of the System in Action

Before looking at the technology, it is worth examining how the system works at the time of writing. The description that follows assumes you are 
using the nRFToolbox app and have selected the UART sub-app. At the time of writing the WW130 will start BLE advertising by pressing SW1,
or by moving a magnet near the magnetic sensor (IC2 on the WW130 PCB edge). 
At this point the device appears in the phone and you can click it to establish a BLE connection.

Any text typed in the app will be sent to the nRF52832 (in the MKL62BA subsystem, on the WW130). The nRF52832 software will 
look for certain strings and, if there is a match, pass these to functions which interpret them and send a response.
The response is then displayed on the phone. Examples are:

| Command   | Function                                             | 
| ----------|------------------------------------------------------|
| id        | Print the device's BLE name                          | 
| ver       | Print the hardware and software version              |
| status    | Prints some status                                   |
| enable    | Enables the magnetic sensor to send LoRaWAN messages |
| disable   | Disables the magnetic sensor                         |

(More commands are documented elsewhere).

Any command the begins "AI " results in the rest of the line being sent to the HX6538. The MKL62BA sends "OK" to the phone
and the HX6538 then looks for commands it understands and sends a response. Examples include:

| Command               | Function                                              | 
| ----------------------|-------------------------------------------------------|
| AI status             | Prints some status                                    | 
| AI info               | Prints some information about the SD card             | 
| AI dir                | Prints the SD card directory listing                  | 
| AI type \<file\>      | Reads a file and sends it to the phone                | 
| AI fileread \<file\>  | Reads a file on the SD card and returns info about it | 
| AI filewrite \<file\> | Reads test data to a file and returns info about it   | 

I have implemented a command line interface (CLI) on the Seeed board, and its commands can be seen by typing "help".
I have routed the messages that come from the phone via the MKL62BA into the CLI mechanism, so every command can be 
executed by typing at the keyboard or typing at the phone. To understand this you should have a play and look at the results. 
Also look at the source code of the ``CLI-commands.c`` and ``CLI-FATFS-commands.c`` files.

Understanding this, with the help of the sections below, will help understand the communications protocol. To a large extent,
the functionality of the Wildlife Watcher system, including the app, can be established by extending these commands.

### Hardware

The HX6538 acts as an I2C slave, at address 0x62. So the MKL62BA has to initiate communications - either writing to the HX6538 or reading from it.
There is (currently) a single interrupt signal that connects GPIO pins on the two processors. Either processor can interrupt 
the other by taking that pin low.

The MKL62BA uses I2C instance 1 on pins P07 and P08. It runs at 400kHz clock speed. The MKL62BA has a limit of 255 bytes in each
data exchange. Also, the BLE configuration has been set for an MTU of 247 bytes. The NUS protocol has a 3 byte header, giving a usable 
packet size of 244 bytes. At the time of writing the I2C maximum packet size has also been set at 244 bytes, so that packets of this size
can travel between the app to the HX6538 without adjustment. (It might prove useful to add some header bytes to the BLE packets,
in which case the usable payload size might be a few bytes smaller).

Both processors can assert the common interrupt pin, and both processors can be interrupted on either edge.

The interrupt pin is P05 on the MKL62BA. This is connected to PA0 on the Grove Vision AI V2. It also connects to
SW2 on the WWIF100 board. Interval pull-up resistors are enabled (on both boards I think).

P05 on the MKL62BA was previously allocated to one of the "buttons" and as such it had debouncing logic which meant it had to 
be asserted for tens of ms before the MKL62BA saw an event. I have now removed it from the button set and configured it to 
generate an interrupt on both edges. It responds fast to pulses from the HX6538.

Because I was using the SW2 pin to put the MKL62BA into DFU mode, for firmware updates, I have retained this useful feature, 
as follows. I start a timer on the falling edge of P05 and measure the time that has elapsed at the rising edge. 
Intervals less that 1s are routed to interprocessor communications code. Intervals between 1s and 2s generate a message
(and could be used to execute some other code). Intervals >2s put the board in DFU mode.

On the HX6538 the PA0 pin is associated with AON_GPIO0. Code (in ``interprocessor_interrupt_init()``) initialises this as an interrupt
that responds to the falling edge. When the HX6538 wants to interrupt the MKL62BA
it calls ``interprocessor_interrupt_assert()``. This disables interrupts, sets the pin as an output, low.
Soon afterwards ``interprocessor_interrupt_negate()`` reverses this. (The code is shown below).

### Role of MKL62BA subsystem

The MKL62BA subsystem receives messages from the app and either operates on these, returning a response, 
or passes them to the HX6538 if the messages start with "AI ".

Message responses from the HX6538 are, at the time of writing, sent to the app, provided there is a BLE connection.
So, at the time of writing, the MKL62BA subsystem serves only as a transparent conduit between the app and the HX6538.

At the time of writing the HX6538 does not generate any messages that are intended soley for the MKL62BA subsystem. 
And the MKL62BA subsystem does not generate any messages to the HX6538 spontaneously. However this is likely to change, 
since it is likely that in the finished system the HX6538 will ask the MKL62BA subsystem to generate LoRaWAN messages.
Similarly, it is possible that some messages that arrive from the LoRaWAN channel will be sent to the HX6538.

At the time of writing, the protocol discussed in the next section is only used for app to HX6538 communications.

### Inter-processor Communications Protocol

* For the most part, inter-processor communications is initiated by the MKL62BA which sends messages to the HX6538. 
* In this case, the HX6538 usually (always?) returns a response to the MKL62BA.
* Less often, inter-processor communications is initiated by the HX6538 (for example when it wants to know the current UTC time).
* In this case, since the HX6538 is not the I2C master, it has to interrupt the MKL62BA as a way of asking it to 
read the message that the HX6538 has to send. 

#### Master-to-slave messages

Messages from the phone that begin with "AI " have these 3 characters stripped and the rest of the string is sent
to the HX6538 across the I2C interface. This communication is initiated by the MKL62BA as I2C master. 
As discussed above, the messages have a maximum payload of 244 bytes.

On the HX6538, inter-processor communications is managed within the IfTask, implemented in ``if_task.c``. This file implements three
interrupt-driven callbacks:
* i2cs_cb_rx() - called when a packet arrives
* i2cs_cb_tx() - called when a packet from the HX6538 has been sent
* i2cs_cb_err() - called on error conditions. This includes the situation that the I2C master attempts to read
from the HX6538 before the HX6538 has prepared a packet for it.

Payload data is preceded, in the I2C packet, by 4 header bytes, and followed by 2 CRC check bytes. The header includes a message
type byte. At _the time of writing_ the only incoming message that is processed is a string, and it is dispatched, after
checking, to the CLI task in ``CLI-commands.c``. In the CLI task the messages are parsed as though they were typed at the console.

The MKL62BA subsystem does not attempt the read a response until the HX6538 has a response ready to send. 
When the the HX6538 has a response to send, it 
prepares data for the outgoing I2C system, and generates an interrupt. The HX6538 code that sends a message is:

```c
static void sendI2CMessage(uint8_t * data, aiProcessor_msg_type_t messageType, uint16_t payloadLength) {
	interprocessor_interrupt_assert();
	i2ccomm_write_enable(data, messageType, payloadLength);
	interprocessor_interrupt_negate();	// MKL62BA responds on the rising edge.
}
``` 

The ``i2ccomm_write_enable()`` function prepares the outgoing I2C data, including the 4 byte header and 2-byte CRC.
The MKL62BA receives an event as the interrupt pin goes high, with the ``aon_gpio0_drive_high()`` call. It then reads the packet prepared
for it. At the time of writing any message is sent to the smart phone app, and the messageType byte indicates either that the payload
contains a string (terminated by '\0') or binary data. In the future decisions can be made depending on the
value of the messageType parameter. For example, other values of this byte could indicate that the message should be 
processed by the MKL62BA subsystem.

At present, a short response is returned to the MKL62BA subsystem by the HX6538 very shortly after the MKL62BA subsystem sends a 
packet to the HX6538. However, some responses are likely to exceed the 244 byte payload limit. In this case the HX6538
calls ``sendI2CMessage()`` multiple times. Each call generates an interrupt and transfers (up to) 244 bytes. 
This matches the FreeRTOS CLI mechanism, which allows any single command typed at the 
console to return multiple strings. That is why, when the app user types ``AI dir``, the result is a sequence of BLE packets, each 
containing a single line of the SD card directory listing. That is because the ``prvDirCommand()`` function is called many times
by the CLI code, each time returning a single directory entry.

As a further example, consider two ways of reading a file that is greater than 244 bytes. One command treats the file as text
and the other as binary. 

In the text file case, use the ``AI type longfile.txt`` command. In this case the file is split into blocks of 243 bytes 
(a C string terminated by '\0').

In the binary file case, use the ``AI read longfile.txt`` command. In this case the file is split into blocks of 244 bytes.
The (binary) data sent to the nRFToolbox app may not be printable text, so expect trouble on the screen. When operating with
the Wildlife Watcher app this issue can be managed. (It is likely that a future version of the protocol might transfer 
the messageType and payloadLength parameters in the BLE messages, to help the app interpret the incoming messages).  


#### Slave-to-master messages

To follow.


### Extending the Inter-processor Communications Protocol

As implied above, it is likely that the protocol may evolve variously:

* Passing header information in both directions in the BLE packets.
* Defining messages that don't involve the app and the BLE interface, but allow the MKL62BA subsystem and the HX6538
to communicate privately.

In the short term, given the current state of the protocol, it should be possible for the app developer to make progress
with app functionality with zero or minimal changes to the MKL62BA subsystem code. In other words:
* The app developer specifies a new command that begins "AI " (such as ``AI getmetadata``)
* The HX6538 developer adds a new ``CLI_Command_Definition_t`` in ``CLI-commands.c`` that implements this request.
* The app developer parses the incoming response and acts upon it.

### A Suggestion for the First Extension
Here is a (useful) exercise to get software developers up to speed with the inter-processor communications described in this document.

Two new commands are added:
* ``AI snap`` - asks the HX6538 to take a picture. The response message includes \<filename_of_picture\>.
* ``AI getExif <filename_of_picture>`` - asks the HX6538 to return metadata (which could include the thumbnail).

The app developer could use this to ask for a picture that could be displayed to the user of the app. (We have already 
discussed the need for this feature, so the user can be assured that the Wildlife Watcher camera is directed properly).
Probably, the existing ``AI read <filename_of_picture>`` command is sufficient to get the whole JPEG image to the phone. 
It should be tested!

(Note to self: [Markdown Guide](https://confluence.atlassian.com/bitbucketserver083/markdown-syntax-guide-1155483368.html))

 


