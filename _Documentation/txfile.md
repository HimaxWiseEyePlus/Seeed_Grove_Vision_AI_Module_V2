# Transferring of Files from the WW500 to the App
#### CGP - 28 March 2025

This documents an interim mechanism for transferring files from the WW500 to the app.

The main purpose of the mechanism is to allow uploading of photos to the app, in particular to
allow testing of the device deployment process, which calls for the user to be presented with a photo of what the 
device is seeing.

__The mechanism documented here is an interim measure only - not well tested, and likely to be replaced by a more 
general and robust mechanism later.__

## The 'txfile' Command

A new command has been added to the HX6538 command: `txfile`. The syntax (when issued by the app) is:

```
AI txfile <filename>
or:
AI txfile .
```

* If a filename is supplied then that file will be transferred to the app. This allows any file on the SD card to be uploaded.
* If the character '.' is used then the most recently taken photo (a .jpg file) will be transferred. (The WW500 keeps the filename
of the most recent file and substitutes this filename for the '.' character.)
* Filenames are relative to the current directory (which can be determined by the `pwd` command, and can be changed by the `cd`command).

The app should be written so that the `AI txfile .` command is sent after a command such as `AI capture 1 1` has taken a photo.

__NOTE__ I have enhanced the `capture` command so that the WW500 now returns a message when the sequence of image captures completes. The message looks like this:
```
Captured 2 images. Last is image_2025-03-29_0004.jpg
```
If necessary the name of the latest image file can be derived from this.


## The Response

The file upload mechanism has the following constraints:

* We are using the Nordic UART Service (NUS) Service.
* Packet sizes are limited (AFAIK) to the size of the BLE packet - which is negotiated between device and phone at 247 bytes 
(3 of which are taken by the NUS itself, so 244 bytes are available.)
* If the phone receives a NUS payload then it can be assured that the data is valid, as BLE transfers are protected by CRC against errors.
* However the protocol does not guarantee every packet is delivered.
* The file length is not known to the app before requesting the file (OK, it could issue an "AI dir" command and parse the results which 
includes file lengths).

The code has been constructed to be simple to implement on the device. The protocol follows:

1. After the `AI txfile <filename>` or `AI txfile .` command is issued the first packet received back gives the name of the file and its size.
For example: 
```
10361 bytes in image_2025-03-29_0004.jpg
```
This lets the app know how many bytes it should expect in total.

2. Then follows a number of packets, each of which have this format:
  	* A packet type code (1 byte) - currently this is fixed as 6 for binary file transfer. 
  	(If the first byte is a printable character, from 0x20 onwards, the the whole packet should be treated as text).
  	* A packet number, starting at 1 (1 byte). This increments for each packet, and will roll over (from 255 to 0) if more than 255 packets are sent. 
  All the app needs to do is to check that this value increments monotonically. This will flag lost packets.
  	* A payload length (1 byte). This is a value between 1 and 241. Normally this will have the value 241 for all packets except the final one.
  	* The payload itself - this is a number of bytes that should match the payload length.
3. The incoming file is constructed by concatenating all of the payload bytes.
4. There is no 'complete' packet - the app itself needs to determine when the file transfer is complete. It can do this by adding all of the 
'payload length' bytes until this value is equal to the file size sent in 1. above. 
5. The app needs to add appropriate error detection, including the possibility that packets get lost. So a timeout should be added 
to handle the possibility that the app does not receive all the data it expects.
6. Other error handling needs to manage errors such as 'file not found'

## Testing

At the time of writing I have implemented something quickly, and testing has been limited. It is possible that bugs exist. 
I feel that these are most likely to be related to error handling (SD card fails to deliver the data) or timing (unexpected duration between
events at the app, MKL62BA, HX6538 and SD card).

At the time of writing I have an occasional error in which one whole BLE packet is lost. I expect I will find and fix this bug. 
However, the app could consider retrying if it detects an error.

## Future Developments

At the moment the file transferred to the app is the same as the file saved to the SD card. So if it is a large file we will have to wait 
for many bytes to be transferred. The sample I have been working with during development is c.10k bytes, which is around 43 BLE packets. 
It might be a good idea to create smaller 'thumbnail' files for the purpose of giving the app a quick image.

 It takes about 8s to transfer an image. I have not had time to think about optimising file transfer time.
There may be an opportunity to do this, but perhaps the time is better spent implementing a long-term solution.

If the jpeg file contains EXIF data then the uploaded file will contain this EXIF metadata. This might be of use.

Alternatively, the EXIF data could be saved to a `.exif` file and uploaded separately.

In due course I would like to establish an optimised process of sending and receiving large files. This could be used to:
* Download new firmware, for both processors (DFU).
* Download neural network models, so the device can be used to look for different objects without having to be re-programmed at the office.
* Upload captured images, so the SD card does not have to be removed.






 