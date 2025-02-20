Android DFU Library
===================
#### CGP 9/2/25

**Nordic implement a Device Firmware Update (DFU) mechanism that is used to securely download new firmware images
to BLE devices. I am researching the workings of the DFU mechanism to see whether it could be extended to transfer other 
data to/from our Wildlife Watcher devices. Extensions could include:**

* Writing of files to the SD card (could include new Neural Network models).
* Reading of files from the SD card (could include uploading of image files). 
* DFU for the HX65386
* Bringing the DFU of the MKL62BA into the app.

In the short term there is no need to embed the DFU function within our own app, since a reliable standard
mechanism exists for DFU to the MKL62BA (the BLE and LoRaWAN module on the WW500).

But perhaps we can find and extend the BLE transport code from the DFU application in order to provide the 
enhanced functionality. Then, at a later stage, perhaps we can add back in the DFU functionality for the MKL62BA
so the app can perform all DFU update operations.


#### DFU app and Library Source Code

* Here is the [DFU app for Android](https://play.google.com/store/apps/details?id=no.nordicsemi.android.dfu&hl=en_NZ)
* It is also a component of the [nRFToolbox app](https://play.google.com/store/apps/details?id=no.nordicsemi.android.nrftoolbox&hl=en_NZ).
* The library is available here [Android-DFU-Library](https://github.com/NordicSemiconductor/Android-DFU-Library).
* There appears to be an [iOS equivalent](https://github.com/NordicSemiconductor/IOS-DFU-Library).

In fact, although this is called a "library", in fact it appears to be the source code of a complete app, 
which performs all stages of downloading a new firmware image to a BLE device. 
Embedded within this code is a class [Android-DFU-Library/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java)
which appears to contain the main components involved with transfer of messages between the 
Controller (phone) and device (e.g. WW500).

#### Background on the DFU process and other resources:

* [Tutorial part 1](https://novelbits.io/ota-device-firmware-update-part-1)
* [Tutorial part 2](https://novelbits.io/ota-device-firmware-update-part-2)
* [Tutorial part 3](https://novelbits.io/ota-device-firmware-update-part-3)
* [Tutorial part 4](https://novelbits.io/nrf52-ota-dfu-ble-part-4/)
* [Buttonless Secure DFU Service](https://docs.nordicsemi.com/bundle/sdk_nrf5_v17.1.0/page/service_dfu.html)
* [Secure Device Firmware Update (DFU) Service](https://docs.nordicsemi.com/bundle/sdk_nrf5_v17.1.0/page/lib_dfu_transport_ble.html)
Definition and sequence diagram.
* [DFU Service API](https://docs.nordicsemi.com/bundle/sdk_nrf5_v17.1.0/page/group_nrf_dfu_ble.html)
* The [DFU package generation](https://docs.nordicsemi.com/bundle/nrfutil/page/guides-nrf5sdk/dfu_generating_packages.html)
step creates a .zip file that contains the new firmware image plus extract information.
* [Buttonless DFU Service source](https://github.com/cmdwtf/nRF5_SDK/tree/master/components/ble/ble_services/ble_dfu) (a 
"mirror" of the official Nordic SDK which apparently is not on github). This contains the `ble_dfu.h` and `ble_dfu.c` files which implement this service.
* [ble_app_buttonless_dfu example app](https://github.com/cmdwtf/nRF5_SDK/tree/master/examples/ble_peripheral/ble_app_buttonless_dfu) This
is an example of building the DFU Service into an app (in contrast to the bootloader implementing the DFU Service).


## DFU vs BLE Transport

There is a __BLE Transport__ mechanism which moves the necessary data to the device. This is just one
component of the full __DFU operation per se__ which checks and uses this data.

The __BLE Transport__ mechanism:

* Uses a `DFU Control Point` characteristic to send commands to the device and to receive responses.
* Uses a `DFU Packet` characteristic to send an "init packet" to the device (without acknowledgement).
* Uses the same `DFU Packet` characteristic to send the new firmware image data to the device (without acknowledgement).

The __complete DFU operation__ uses the BLE Transport mechanism and adds this:

* There is a [DFU package generation](https://docs.nordicsemi.com/bundle/nrfutil/page/guides-nrf5sdk/dfu_generating_packages.html)
step performed after compiling the new firmware image that adds a PKI signature 
and zips up the image and an "init packet". The user selects this file in the DFU app.
* Extracts data components from the .zip file: the init packet and the firmware image. 
* Extensive checking that the firmware should in fact be downloaded and used (looks at hardware and firmware versions
and PKI signatures).
* The device is responsible of placing the new firmware image at the right place in its flash memory
and reseting the device so that the new version executes.

In the current WW500 a bootloader exists in the MKL62BA flash memory, and this bootloader contains all of the
DFU code, part of which is the BLE Transport code. The bootloader can be invoked by button presses at reset time 
or under software direction from within the main app.

For the purposes of this exercise we don't need to understand all steps of the DFU operation. It will suffice to
understand the BLE Transport mechanism to see if and how it can be extended to perform our extract operations.

## Examining the DFU Library to understand the DFU download operations.

The next section of this file is the result of examining the BLE Library code. It will be very helpful to
consider the description and sequence diagrams shown [here](https://docs.nordicsemi.com/bundle/sdk_nrf5_v17.1.0/page/lib_dfu_transport_ble.html).
(Not certain these are complete...)

It looks like the DFU app performs several operations such as finding and verifying the file to download,
and connecting to the device. It then calls `performDfu()` to perform the download.

Before `performDfu()` is called the library must prepare the two key parts for transfer. 
The BLE connection process also involves negotiating an MTU 
(data packet size) that is as large as possible and has a size to contain an integral number of 32-bit words.

#### performDfu()


There are several instances of `performDfu()` - for now let's look at the copy in `SecureDfuImpl.java`
[performDfu()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L210)

It seems to involve two steps (which are echoed in the [BLE Service documentation](https://docs.nordicsemi.com/bundle/sdk_nrf5_v17.1.0/page/lib_dfu_transport_ble.html)):

1. Transfer of an init packet: [sendInitPacket()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L349)
2. Transfer of a firmware image: [sendFirmware()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L501)

There is considerable complexity here, as the library must partition the two key parts into multiple BLE transfers,
and recover from a variety of error conditions.

#### Send Init Packet

[sendInitPacket()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L349)
does these things:
1. Sends a [selectObject()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/b0d73c655ea336b3cce0265614aac4d7d082e66f/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L357)
command to the DFU Control Point Characteristic to initialise this step.
2. Sends a [writeCreateRequest()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L424)
command to advise the number of bytes to be sent (`mInitPacketSizeInBytes`).
3. Writes to the DFU Packet Characteristic to transfer the data.
4. Issues a Write Execute command sends a OP_CODE_EXECUTE to the DFU Control Point Characteristic

Specifically, it calls: 
1. Command [setPacketReceiptNotifications(0)](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L417)
to explicitly disable PRNs (Packet Receipt Notifications).
2. [writeCreateRequest(OBJECT_COMMAND, mInitPacketSizeInBytes)](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L847)
to the Control Point Characteristic with [writeOpCode()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/BaseDfuImpl.java#L599) which finally calls [mGatt.writeCharacteristic()]()
3. followed by [writeInitData()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/BaseCustomDfuImpl.java#L293)
 which calls [writeInitPacket()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/BaseCustomDfuImpl.java#L322)
 which finally transfers data to the DFU PACKET characteristic: [`mGatt.writeCharacteristic(characteristic)`](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/BaseCustomDfuImpl.java#L344)
4. the write execute command [writeExecute()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L948)
verifies the download is OK. It expects a return notification containing status code.

#### Send Firmware
[sendFirmware()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L501)
does three things (some in a loop!!!):
1. Writes to the DFU Control Point Characteristic to initialise this step.
2. Writes to the DFU Packet Characteristic 
3. Issues a Write Execute command

Specifically, it calls: 
1. [writeCreateRequest(OBJECT_DATA, availableObjectSizeInBytes)]() to the Control Point Characteristic with writeOpCode() which finally calls mGatt.writeCharacteristic()
This tells the device how many bytes to expect.
2. Uses [uploadFirmwareImage()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/BaseCustomDfuImpl.java#L373)
to send the data. This uses [writePacket()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/BaseCustomDfuImpl.java#L420)
This uses `gatt.writeCharacteristic()` to write to the DFU Packet Characteristic
3. The write execute command [writeExecute()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L948)
expects a return notification containing status code.

It looks like the first `writePacket()` call is made from [uploadFirmwareImage()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/BaseCustomDfuImpl.java#L390)
and then is called more times as a results of the [onCharacteristicWrite()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/BaseCustomDfuImpl.java#L153) 
and/or [handlePacketReceiptNotification()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/BaseCustomDfuImpl.java#L225) 
events to transfer subsequent packets.

It looks like these `onCharacteristicWrite()` and `handlePacketReceiptNotification()` events keep track of the progress through the full firmware transfer.
Perhaps `uploadFirmwareImage()` blocks till the whole transfer is complete.

## Controller Commands and Device Responses

Several commands and responses are shown in the [sequence diagrams](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/BaseDfuImpl.java#L786)

The commands and responses are listed here (all except the last entry originate from the app):

| Command | Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Parameter(s)| Purpose |
| -------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- |
| OP_CODE_CREATE_COMMAND           | 01 | 01 | nn | nn | nn | nn | Init packet size (32-bit, LE) | Define size of init packet |
| OP_CODE_CREATE_DATA              | 01 | 02 | nn | nn | nn | nn | Available object size (32-bit, LE) | Define size of next firmware tx? |
| OP_CODE_PACKET_RECEIPT_NOTIF_REQ | 02 | nn | nn | -- | -- | -- | PRN (16-bit, LE) | Sends the number of packets before notifications |
| OP_CODE_CALCULATE_CHECKSUM       | 03 | -- | -- | -- | -- | -- | -- | App asks device for offset and checksum from recent data transfers |
| OP_CODE_EXECUTE                  | 04 | -- | -- | -- | -- | -- | -- | Validates init packet? What about data? |
| OP_CODE_SELECT_OBJECT            | 06 | nn | -- | -- | -- | -- | type (1=command, 2=data)| Tells device the nature of the data that follows. |
| OP_CODE_RESPONSE_CODE_KEY        | 60 | rq | st | xx | -- | -- | rq = request, st = status (1=OK), xx = possible extra bytes | Device returns status to the app. |

Commands use [writeOpCode(mControlPointCharacteristic, <byte array>))](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L831)
to write the byte array to the DFU Control Point Characteristic.

These commands are used by various functions, which are listed below in the same sequence as in the table above.
Text in _italics_ describes the corresponding behaviour on the device.

#### writeCreateRequest()

The [writeCreateRequest()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L847)
function calls [setObjectSize()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L777)
function uses the `OP_CODE_CREATE_DATA` or `OP_CODE_CREATE_COMMAND` commands (above).

_The device processes this command in [on_ctrl_pt_write()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L482)
by saving the `type` parameter (`NRF_DFU_OBJ_TYPE_COMMAND` or `NRF_DFU_OBJ_TYPE_DATA`).
 It sends a response from [ble_dfu_req_handler_callback()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L435)
which returns only the status._

#### setPacketReceiptNotifications()

The [setPacketReceiptNotifications()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L799) 
function calls [setNumberOfPackets()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/b0d73c655ea336b3cce0265614aac4d7d082e66f/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L766)
to set the number of data packets (Packet Receipt Notification = PRN) that will be send before the notification will be received.
It uses the `OP_CODE_PACKET_RECEIPT_NOTIF_REQ` command (in table above).

It is used in [sendFirmware()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L501)
before [selectObject(OBJECT_DATA)]()
and [writeExecute()]()

_The device processes this command in [on_ctrl_pt_write()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L482)
by saving the `number of packets` parameter. this is decremeneted on every incoming packet.
 It sends a response from [ble_dfu_req_handler_callback()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L435)
which returns (what? check???)._

#### readChecksum()

The [readChecksum()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/b0d73c655ea336b3cce0265614aac4d7d082e66f/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L910)
function uses the `OP_CODE_CALCULATE_CHECKSUM` command to read a checksum.
It uses [readNotificationResponse()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/BaseDfuImpl.java#L786)
to read the incoming Notification, and extracts this:
1. Status (1 byte)
2. Offset (32-bits)
3. Checksum (32-bits)

It is used by `sendInitPacket()` and `sendFirmware()`

_The device processes this command in [on_ctrl_pt_write()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L482)
There are no incoming parameters.
It sends a response from [ble_dfu_req_handler_callback()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L435)
which returns the `offset` and `CRC`._

#### writeExecute()

The [writeExecute()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/b0d73c655ea336b3cce0265614aac4d7d082e66f/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L948)
uses the `OP_CODE_EXECUTE` command. 
It sends the Execute operation code and awaits for a return notification containing status code.

It is used in several places:
* At the end of `sendInitPacket()` to ask the device to execute the Init packet.
* In `sendFirmware()` to tell the device to use the transferred data.  (???)

For data transfers it is called before `uploadFirmwareImage()` and for init packet transfers it is called before `writeInitData()`.

After sending the command the functions waits for a response then calls
[getStatusCode(response, OP_CODE_CREATE_KEY)](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L744)
to parse the returned status.

_The device processes this command in [on_cmd_obj_execute_request()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L323)
then [nrf_dfu_validation_init_cmd_execute()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_validation.c#L574)
then [nrf_dfu_settings_write_and_backup()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_settings.c#L406)
This presumably does some kind of validation and writing to flash (not investigated)_

#### selectObject()

The [selectObject()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L876)
function uses the `OP_CODE_SELECT_OBJECT` command. It selects the current object and reads its metadata. The object info contains:
* the max object size
* the offset
* the CRC32 of the whole object until now.

The byte 1 can perhaps be thought of a the opcode (could perhaps be extended for file write, file read etc?)

It is used twice:

* `selectObject(OBJECT_COMMAND)` to start the Init Packet transfer - "Setting object to Command (Op Code = 6, Type = 1)" - used by `sendInitPacket()`
* `selectObject(OBJECT_DATA)` to start the data transfer - "Setting object to Data (Op Code = 6, Type = 2)" - used by `sendFirmware()`


`selectObject()` does this:
* Calls [writeOpCode()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/b0d73c655ea336b3cce0265614aac4d7d082e66f/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L831)
to send the command to the DFU Control Point Characteristic, then:
* Reads the result with a notification: [readNotificationResponse()](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/BaseDfuImpl.java#L786)
This returns information as follows:
1. status
2. maxSize
3. offset
4. CRC

_The device processes this command in [on_ctrl_pt_write()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L482)
by saving the `type` parameter. 
This is used subsequently to determine what to do, in particular how to handle incoming data.
Then [cmd_response_offset_and_crc_set()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L263)
This returns the `offset` and `CRC`_

#### getStatusCode()

The function [getStatusCode(response, OP_CODE_CREATE_KEY)](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L744)
is called to parse the incoming response (Notification). 
The notification implements the `OP_CODE_RESPONSE_CODE_KEY` command.

Bytes received are:
1. 0x60
2. request (the previous command is echoed back).
3. status (1 = OK, else [these error codes]([getStatusCode(response, OP_CODE_CREATE_KEY)](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/main/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L744)
to parse the returned status)
4. Possibly further parameters, depending on what the request was


TODO - check all instances of writeOpCode()

## Device-Side Software

I am trying to work out which files are needed to execute the DFU stuff on the device side.

The device-side software is [in here.](https://github.com/rickeycv/nRF52840_BLE_bootloader/tree/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader)
**NOTE: Links to files are to an unofficial repository as it looks like Nordic's is missing from Github**


Possibly only these files are necessary to extend the DFU mechanism?:

* Generic DFU code [components/libraries/bootloader/dfu/](https://github.com/rickeycv/nRF52840_BLE_bootloader/tree/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu)
Contains several, including:
	* [nrf_dfu_req_handler.h](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.h)
	This defines the constants such as `NRF_DFU_OP_OBJECT_CREATE`, data types, and function prtotypes.
	* [nrf_dfu_req_handler.c](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c)
This implements functions that process incoming commands (once these are identified and parsed in `nrf_dfu_ble.c`.
* BLE transport code [components/libraries/bootloader/dfu_ble/](https://github.com/rickeycv/nRF52840_BLE_bootloader/tree/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu)
Contains only these:
	* [nrf_dfu_ble.h](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.h)
This defines the service and characteristic UUIDs and functions such as `ble_dfu_transport_init()`
	* [nrf_dfu_ble.c](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c)
Implements functions such as `ble_dfu_transport_init()` as well as `ble_dfu_init()` 
which adds the DFU Service and Characteristics. It has code to handle the incoming BLE commands, and to send BLE responses.
Interfaces to functions in `nrf_dfu_req_handler.c` which do the main work. 
such as `NRF_DFU_OP_OBJECT_CREATE` and `NRF_DFU_OP_OBJECT_EXECUTE` so probably implements the core logic.
	* [nrf_dfu_ble_svci_bond_sharing.h](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble_svci_bond_sharing.h)
Something to do with BLE bonds...
* Other folders have drivers for ANT and UART transports.


#### Writing to the Device

The DFU Service (uses `BLE_DFU_SERVICE_UUID`) exposes two characteristsics and it looks like these get different events from the SoftDevice
when the central writes to them:

* The `DFU Control Point` characteristic (uses `BLE_DFU_CTRL_PT_UUID`) write comes from BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST: 
"This event is triggered when a peer device requests authorization to read or write a characteristic that requires 
authorization. It's essentially asking for permission before proceeding with the read or write operation."
* The `DFU Packet` characteristic (uses `BLE_DFU_PKT_CHAR_UUID`) write comes from BLE_GATTS_EVT_WRITE: 
"This event occurs when a peer device successfully writes data to a characteristic. It indicates that the 
write operation has been completed and the new value is now stored in the characteristic."

So: 
* The `DFU Control Point` characteristic write ends in the [on_ctrl_pt_write()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L470)
event. 
* The `DFU Packet` characteristic write ends in the [on_write()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L583)
event. 

Both types of write go through sequences of functions as follows:

#### `DFU Control Point` characteristic write:

_characteristic properties: write, notify_

1. [on_ctrl_pt_write()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L470)
which extracts parameters, before calling:
2. [nrf_dfu_req_handler_on_req()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L855)
which asks the scheduler to call:
3. [nrf_dfu_req_handler_req()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L848)
4. [nrf_dfu_req_handler_req_process()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L764)
which starts preparing the response, then uses a switch() statement to call different functions depending on the command. These include (to follow)
However most/all of the BLE commands end up here:
5. [nrf_dfu_obj_op()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L720)
(Note when control returns here, the response is returned).
6. [nrf_dfu_command_req()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L367)
This uses a switch to call one of these, depending on the command:
	* [on_cmd_obj_create_request()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L283)
	* [on_cmd_obj_crc_request()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L349)
	* [on_cmd_obj_execute_request()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L323)
	* [on_cmd_obj_select_request()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L273)
Returns max TX size (512 bytes) and current offset and CRC.

#### `DFU Packet` characteristic write:

_characteristic properties: write_wo_resp_

1. [on_write()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L583)
which copies the data to a buffer before calling the following:
This is the same as step 2 of the Control Point processing: the request type is set to `NRF_DFU_OP_OBJECT_WRITE`
2. [nrf_dfu_req_handler_on_req()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L855)
which asks the scheduler to call:
3. [nrf_dfu_req_handler_req()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L848)
4. [nrf_dfu_req_handler_req_process()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L764)
which starts preparing the response, then uses a switch() statement to call different functions depending on the command. These include (to follow)
However most/all of the BLE commands end up here:
5. [nrf_dfu_obj_op()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L720)
(Note when control returns here, the response is returned). Then:
	* For a command (init packet?) write: [nrf_dfu_command_req()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L367)
	* For a data write: [ nrf_dfu_data_req()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L669)

The command (init packet?) write then proceeds:

6. [on_data_obj_write_request()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L492)
7. [nrf_dfu_validation_init_cmd_append()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_validation.c#L277)
then what?

Whereas the data write then proceeds:

6. [on_data_obj_write_request()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L297)
6. [nrf_dfu_command_req()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L367)
which does various processing, then:
8. [nrf_dfu_flash_store()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_flash.c#L117)
which is clearly a write to flash memory. 
The originating code receives a [on_flash_write()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L583)
event when the write completes, to free the buffer. As the writing proceds pointers are updated and a CRC is calculated._

#### `DFU Control Point` characteristic notification:

Although the [SDK documentation](https://docs.nordicsemi.com/bundle/sdk_nrf5_v17.1.0/page/lib_dfu_transport_ble.html)
says that the DFU Packet characteristic sends notifications, it does not. All responses sent to the central device
come from DFU Control Point characteristic notifications. 

> Notifications are unacknowledged messages sent from the server (peripheral) to the client (central). They are faster because they don't require an acknowledgment from the client.
The notification is sent by [response_send()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L266)
which uses `sd_ble_gatts_hvx()` ('hvx' stands for "Handle Value Exchange").

Responses are prepared by [response_prepare()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L325) 
and by other command-specific functions, then by [ble_dfu_req_handler_callback()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c#L392)
This callback function is defined in `on_ctrl_pt_write()` and `on_write()`.
The callback is invoke by this code: `p_req->callback.response(&response, p_req->p_context);` at various places,
for example [here](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L824)

## Thoughts on Extending the DFU Mechanism

`nrf_dfu_ble.c` contains all the necessary BLE interface code, but also a lot of code only required in the case
of a stand-alone bootloader, such as code to initialise the softDevice stack, set up advertising etc.
So the necessary functions should be copied from here and added to a custom service (e.g. an extended `WWUS`).

`nrf_dfu_req_handler.c` probably contains necessary code. Some might be removed (temporarily) if the nRF52832
DFU update is __NOT__ in the main application.
So the necessary functions should be copied from here and added to a file to support the custom service.

New top-level functions could probably be added to these app-side 
[list of definitions](https://github.com/NordicSemiconductor/Android-DFU-Library/blob/b0d73c655ea336b3cce0265614aac4d7d082e66f/lib/dfu/src/main/java/no/nordicsemi/android/dfu/SecureDfuImpl.java#L66):
Write and read file examples are added here.
```
	// Object types
	private static final int OBJECT_COMMAND = 0x01;  // Probably means: data transfer is init packet 
	private static final int OBJECT_DATA = 0x02;	 // Probably means: data transfer is firmware image
	private static final int OBJECT_FILE_WR = 0x03;	 // Potential addition, for file write
	private static final int OBJECT_FILE_RD = 0x04;	 // Potential addition, for file read
```

and to this device-side [enum](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/0884150055de7274c809d900180c4774c1d9dc31/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.h#L71):
```
typedef enum
{
    NRF_DFU_OBJ_TYPE_INVALID,                   //!< Invalid object type.
    NRF_DFU_OBJ_TYPE_COMMAND,                   //!< Command object.
    NRF_DFU_OBJ_TYPE_DATA,                      //!< Data object.
    NRF_DFU_OBJ_TYPE_FILE_WR,                   //!< Potential addition, for file write
    NRF_DFU_OBJ_TYPE_FILE_RD,                   //!< Potential addition, for file read
} nrf_dfu_obj_type_t;
```

Then some new code like this in [nrf_dfu_obj_op()](https://github.com/rickeycv/nRF52840_BLE_bootloader/blob/main/nRF5_SDK_17.1.0_ddde560_bootloader/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c#L720):
```
case NRF_DFU_OBJ_TYPE_FILE_WR:
	response_ready = nrf_dfu_file_write_req(p_req, p_res);
	break;
case NRF_DFU_OBJ_TYPE_FILE_RD:
	response_ready = nrf_dfu_file_read_req(p_req, p_res);
	break;
```

Then a new `nrf_dfu_file_write_req()` function would be required to accept the data and to transfer it to the 
HX6538 for moving to the SD card.

The `nrf_dfu_file_read_req()` code is different since the direction of data transfer is different.
The NUS (and WWUS) service send data to the app using notifications, so I guess this could be done for file reads as well. 
Perhaps the notification could be added to the `DFU Packet` characteristic to make it easier for the app to recognise the
source of the notification (i.e. that it contained file data). 

Then perhaps the device would send a notification from the `DFU Control Point` characteristic to indicate when the transfer 
was complete. 

#### Device-side Development

I am fairly confident that I can do the device-side work. One appraoch might be as follows:

1. Create a new WW500 project/app that contained a new Service that implemented the extensions. Perhaps this could __initially__
advertise as the DFU Service. In that way debugging could __initially__ start by using the unmodified server-side DFU app.
2. Probably I should try fully implementing the existing DFU functionality.
3. I could then change the device-side code so that the downloaded data was treated as a file and routed to the SD card as a file.
4. Perhaps some other elements of functionality could be tested (on the device-side) by further tricking the existing DFU app. This
could include DFU of code for the HX6538 (though that is not the highest priority).
5. Further testing will require modifications to app-side code (see below).

#### App-side Development

It looks like I can make some progress on implementing and testing the app-side code by myself, by using the
[Bleak python library](https://github.com/hbldh/bleak)
which can run on a laptop and turn it into the BLE central device. Docs for Bleak are [here.](https://bleak.readthedocs.io/en/latest/)
At the time of writing I have already written [a script](./Scan_nus.py) (actually, Copilot did most of the work) that connects 
to a WW500 and prints messages it sends (normally to the WW app or to the NUS in nRFToolbox.

It is possible that a python version of nrfTools can perform DFU operations from a laptop, but I have not explored this yet.
See [here](https://github.com/NordicSemiconductor/pc-nrfutil/blob/master/nordicsemi/dfu/dfu_transport_ble.py)
See separate [nrftools_python.md](./nrftools_python.md) file for my early work on this...

Hopefully I can make progress on extending the functionality of both the device-side and app-side code by myself,
using the app-side tools mentioned above. It will be more efficient it I do both parts by myself.

After that there seems to be two options for adding new app-side functionality:

1. Modifying the Wildlife Watcher app. This will obviously be needed at some point, but more work?
2. Modifying the DFU app, as an interim step. It looks like full source code is available (for both Android and iOS).

Either or both of these options should be done by Miha (or other app-side developer) as I don't have the skills to do a decent job here.
If we want an app that works for both Android and iOS then there may be two lots of work to do.

#### Markdown Syntax
(Note to self: [Markdown Guide](https://confluence.atlassian.com/bitbucketserver083/markdown-syntax-guide-1155483368.html))
