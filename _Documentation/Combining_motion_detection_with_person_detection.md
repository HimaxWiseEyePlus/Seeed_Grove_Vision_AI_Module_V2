# Combining Motion Detection with Neural Network and Notifications
#### CGP - 4 April 2025

This document proposes a path to demonstrate end-to-end operation with elements that are already working
or close to working. This will demonstrate a level of achievement to others
and it will also be a valuable tool for us to learn about practical aspects of our system.

Subject to feedback, I intend to work towards setting up this system.

__The parts are:__

1. WW500.A01 with adapter(s).
2. Mobile app connecting to WW500 and performing some setup operations
3. HM0360 performing motion detection.
4. Images analysis by existing person detection neural network.
5. Images saved to SD card (preferrably containing EXIF metadata).
6. Messages sent to the cloud via LoRaWAN
7. Messages routed to Trap.nz which sends notifications via email.
8. Entering a deep-power-down state.
9. The system in an enclosure operating from batteries.

At present I am working with two apps, that will soon need to be merged:

1.	`ww130_cli` - this combines the functions of the MKL62BA (BLE & LoRaWAN) module and the HX6538 (AI) module.
At the time of writing it does not do motion detection or deep power down (DPD).
2. `allon_sensotflm_freertos` This code provided by Himax supports HM0360 motion detection and also DPD. 
This knows nothing of the MKL62BA (BLE & LoRaWAN) module.

In addition Tobyn probably has a branch of `ww130_cli` which has enhancements related to EXIF metadata.

Quite soon we will need to combine these apps into one branch.  

## Each Part In Turn

The parts in the list above can be put together as described below.

#### 1. WW500

The existing rev A01 boards are OK for this. I need to assemble more of the `WW500ADPT` adapter boards 
which are needed to make the correct connections to the HM0360, and fit them to the WW500.

I have two WW500s here that I can get working. After that it would be good if the other boards could 
make their way back to me for updating.

_I have made another adapter PCB - the `WW500SD` - which moves the SD card socket to the end of a 8-way ribbon cable.
This was necessary to check that the SD card can be powered off when the WW500 enters deep power down (DPD). (It can).
I have also added some circuitry on that board for infrared or white light illumination. 
It also provides an easy way for the magnetic sensor to be located remotely. This is not yet tested, but
I just want to flag that there is a path for testing this._

#### 2. Mobile App

The WW500 can be asked to advertise (make itself visible to smart phones) using a magnet through the case.
This already works. If necessary the magnetic sensor can be relocated for mechanical convenience.

The existing app needs to be enhanced somewhat. I have provided Miha with information he needs to do several things
and I am waiting for a modified app so these can be checked:

* App to send UTC time to the WW500 (for inclusion in EXIF metadata and for file names).
* App to send GPS location to the WW500 (for inclusion in EXIF metadata).
* App to request latest image from the WW500 and display it on the phone.

Other enhancements would be useful but not essential - mainly to improve the user experience and to populate the
database with configuration information. For example, displaying or setting location, user notes etc.

Optionally, some information on operational mode could be sent to the device. In the medium term it might be best 
for this to be passed as a file to the SD card. That way an operational mode file could be placed on the SD card 
without needing the app to set it up, while still allowing the app to download a (new) operational mode file.

#### 3. Motion Detection

The HM0360 is doing this now, though at the time of writing I have it only one one board. It needs more testing. I hope to do this today.

The motion detection has several parameters as outlined by a 
[Himax Motion Detection document](images\HM0360_Motion_Detection_Setting_20250306.pdf).
The parameters relate to sensitivity.

I intend to provide a way to allow different parameter settings to be read from a file on the SD card.
That will make it easier for testers to experiment with different parameter settings and thus
to see what settings might be optimal.

At the time of writing the HM0360 has to take the photo - not the Raspberry Pi camera. In due course
it should be possible to use the HM0360 only for motion detection, and the RP camera to take the picture. 
That code does not exist yet and is not essential. The HM0360 image is monochrome.

__Not essential, but useful:__

Figure 1.2 in the 
[Himax Motion Detection document](images\HM0360_Motion_Detection_Setting_20250306.pdf) shows
how the motion is detected separately in an array of 16 x 16 "regions of interest" spaced across the field of view. 
This can be seen in operation in [this video](https://www.youtube.com/watch?v=7tuq-vz4aVk) (HM0360 demo at 28:40)
It is likely to be useful for testers to be able to get this view.

I think that the standard app installed on the Grove Vision AI V2 has a mechanism for streaming camera data (and other stuff?)
through its serial port so that it can be displayed on a laptop. 
This [SenseCraft AI web page](https://wiki.seeedstudio.com/sensecraft_ai_pretrained_models_for_grove_visionai_v2/)
suggests this is possible, but how? I think the [default app](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/Seeed_SenseCraft_AI_2023_12_19T183401_402.img)
binary image is available, but is the source code for that provided? 

This [Using a model for Grove Vision AI V2 page](https://wiki.seeedstudio.com/sensecraft_ai_pretrained_models_for_grove_visionai_v2/)
explains a process and in step 5 it says:

> After the model upload is successful, the preview feature will automatically start. The preview window will display a real-time video feed from your Grove Vision AI V2's camera, along with the model's detection results overlaid on the video.

#### 4. Person Detection Model

The code we are using at this stage of the development uses a person detection neural network. This is present both in the
`ww130_cli` app and the `allon_sensotflm_freertos` app which Himax provided us with for testing the HM0360.

So there is no new work required for this. We take a picture, call the neural network functions, and receive
a value which represents the output of the model. This is an integer that reflects the probability of a person in the image.

For now we can use this as a proxy for a "rat detection" model. When we are done with detecting people we can replace 
the person detection model, perhaps in two phases:

1. Swap out the `person_detect_model_data_vela.h` and `person_detect_model_data_vela.c` files 
for corresponding "rat detection" files (though we have to understand how to make these in the right format
for the HX6538).
2. Understand how to dymanically load a different model, probably from SD card.

However, it would be invaluable to understand all of the same apps provided by Himax [here](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/tree/main/EPII_CM55M_APP_S/app/scenario_app)
as these will provide insight into options for placing different models on the hardware.

__Don't forget if we are going to place a rat detector model on the WW500 we will need live rats to test it. Someone needs to plan for this.__

#### 5. Images to SD card

We are doing this now, at an adequate level. Possible enhancements are:

1. Agree how we are going to name files and where (which directory) on the SD card we are going to save them. 
1. Maybe deciding to only save images where the neural network operation produces a "match" output.
2. Add EXIF data. UTC and GPS data are standard fields. The output of the neural network model should be saved also.
There is likely to be an existing standard we should adopt. 

I asked ChatGPT this:

>  Is there a standard way of representing the output of (small) neural network models, listing categories and probabilities of the image containing one of the target objects. Perhpas also bounding boxes. 

I got some useful pointers that could be explored. We should not invent the wheel if there is an established representation. 
Preferrably, the same representation of the model output should be saved as EXIF and sent as the payload via LoRaWAN.

#### 6. Notifications by LoRaWAN

The MKL62BA already connects to a LoRaWAN server, though I am not exchanging useful data at present. The LoRaWAN server
is able to provide the WW500 with the UTC time (without the mobile app as the other source of time).

The LoRaWAN system includes these elements (amongst others);

* The devices needs to be in radio range of a LoRaWAN "gateway". IoT Taranaki run a collection of these in the Taranaki area,
and Spark runs a [commercial service](https://www.spark.co.nz/online/large-business-govt/products/monitor/iot-networks).
Or you can deploy your own gateways such as [this one](https://www.seeedstudio.com/SenseCAP-Multi-Platform-LoRaWAN-Indoor-Gateway-SX1302-AS923-p-5473.html)
from Seeed (Victor has one of these).
 * Each device needs to be registered with an owner, and provisioned with a security key so it can be permitted to exchange
message with its rightful owner. (I do this).
* Each device needs to be registered with a LoRaWAN service provider. My preferred one is [The Things Network](https://www.thethingsnetwork.org/) but IoT Taranaki use
[Chirpstack](https://www.chirpstack.io/). (I can do this.) 
* In each case the LoRaWAN service provider takes the responsibility for routing network traffic to and from the devices,
but then expects to forward data to a third-party service that does useful stuff with the data exchanged. 
I prefer to use "webhooks" to exchange information with such third-party services. I have such a routing script on a server of mine.
* The device needs to send a "payload" - that is a sequence of bytes that can be understood by the receiving service. At the time of writing the
WW500 is not sending payload data, but I can add some code that does that, for example when the neural network dtects a person.

I have already an infrastructure that runs in the cloud. Amongst other things it directs messages from predator traps to the trap.nz website,
using the kind of APIs and security that we would want for real Wildlife Watcher messages. But it could also be configured to send messages elsewhere.


#### 7. Trap.nz to send notification via email.

At the moment I have a setup that allows my (predator trap) LoRaWAN devices to send messages of a few kinds to Trap.nz. 

Trap.nz is designed to collect messages into a database, present data on a website, including on maps, and to send alert messages via email.
That is probably sufficient to demonstrate proof on concept. We could temporarily set up the WW500 to send "trap triggered" 
messages to Trap.nz when a person (or rat) is detected. That in turn can trigger an email to be sent. 

Someone (me? others?) should set up a demo trap.nz account and/or a new project on an existing account, and invite some of us to be trappers
(i.e. to be assigned a "role"). _(The trap.nz account, project and role mechanisms seem to me to be exactly what we need for WW.
IMHO people should familiarise themselves with this, and see if it is a good model for wildlife Watchers.)_ 

We would use the Trap.nz system to add "traps" (actually WW devices) at locations in the field, 
in such a way as they are associated with LoRaWAN messages from our devices. Then when our WW500 devices send the right payload, this is interpretted as trap events and 
displayed on the trap.nz website.

As well as obvious messages (e.g. trap set, trap sprung, heartbeat) it is possible to send some user-defined data. 
We might be able to set that up to include some messages that are relevant to the WW.

As and when the backend team have bandwidth, we could/should discuss with them how real WW messages could be sent to the WW servers
 and how that then might trigger notifications. 


#### 8. Entering deep-power-down (DPD)

This is happening now with the `allon_sensotflm_freertos` app. The device enters deep power down (DPD) and stays there until the motion detection signal 
from the HM03060.

DPD current for the core electronics plus SD card is approx 25uA
which is excellent. The HM0360 is drawing about 1mA but I am sure this will go much lower as I tune HM0360 parameters.

I need to merge the DPD code into the `ww130_cli` app.

#### 9. Enclosure.

We need a setup that allows the WW500 in its case to operate outside from batteries, for demos and testing.

Dev has put the WW500 into a box with a transparent lid. This is probably OK for this stage. Maybe the enclosure needs to be 
mountable above ground level. I wondered whether [plastic roadside markers](https://safetygenius.co.nz/product/road-side-edge-markers/)
could be useful for this: they are cheap and can be reliably placed into the ground.

Obviously, as bandwidth and skills allow we should specify a new case. I can make the next version of the PCB
any size and shape, to match the case requirements.


#### Markdown Syntax
(Note to self: [Markdown Guide](https://confluence.atlassian.com/bitbucketserver083/markdown-syntax-guide-1155483368.html))


