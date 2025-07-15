# Adding EXIF data to JPEG files (On WW500)
#### CGP 9/7/25

On 2/7/25 Himax provided a new version of `allon_sensor_tflm_freertos` which included
code to add EXIF data to JPEG files. I documented that work in [EXIF_Addition.md](EXIF_Addition.md).

Inspired by that code I added similar code to the WW500_md project. This document decribes that work.
Further developments will follow. 

## Overview

The approach is as follows:

1. EXIF data is added to the file written to the SD card after the AI processor has created a buffer 
with the JPEG data, after the neural network processing is done, but before the buffer is written to the SD card.
2. The EXIF data is an `APP1` block which can be added almost the start of the JPEG file, after the `SOI` marker
and before the `APP0` block which contains the JPEG data.
3. The EXIF data is created in its own buffer and its size calculated. That allows the insertion of the 
APP1 and APP1 sections at the right place.
4.	The EXIF data has a particular format which is documented in links given below. 
The [JPEG and EXIF Data Manipulation in Javascript tutorial](https://getaround.tech/exif-data-manipulation-javascript/)
is particularly useful and provides diagrams of the key components. Key components are:
	* APP1 marker and length
	* EXIF Header
	* TIFF header
	* A main IFD0 (Image File Directory) containing several 12-byte IFD tags
	* If GPS data is present then this is in its own GPS IFD
	* A data area exists for data items greater than 4 bytes, and the IFD entries include pointers to 
	the right place in this data area.
5.	The resulting buffer is written to the SD card. It should be possible to use the same buffer, shift the
APP0 section to the right and insert the APP1 section. However early experiments with this failed. 
The SD card file lacked the APP1. So at the time of writing both the EXIF data and the JPEG data are copied 
into a new buffer and that is used for the disk file write.

## EXIF tags

At the time of writing I have created sample code that writes 9 EXIF tags, plus a separate IFD 
region for the GPS data, which has 6 tags. In my initial example the tags are:
1. Camera make ("Wildlife.ai").
2. Camera model ("WW500").
3. Image resolution units
4. X resolution
5. Y resolution
6. Timestamp - "original".
7. Timestamp - "create".
8. Neural Network Output (a binary buffer)
9. Pointer to the GPS IFD section.

Then the GPS IFD section contains these tags:
1. GPS latitude Reference (string, "E" or "W")
2. GPS latitude (in degrees, minutes seconds, stored in numerator & denominator)
3. GPS longitude Reference (string, "N" or "S")
4. GPS longitude (in degrees, minutes seconds, stored in numerator & denominator)
5. GPS altitude Reference (number, 0 or 1)
6. GPS altitude (in metres, stored in numerator & denominator)
 
## Neural Network Output
For the moment I have created a way to save this, in a custom tag. Data is saved as a byte array with
the following structure:

* Byte 0: the number of NN categories (example: 2 for person detection)
* Bytes 1-n: the output of the NN processor 

In the person detection example the output is an array of (two) 8-bit signed integers.
byte 0 is "no person" probability and byte 1 is "person" probability.

As NN models get more complex then perhaps this tag will have to change. Or perhaps 
a generic array of binary data is fine.

## Tools

These may be of use when developing with EXIF.

#### exiftool
This is a Windows .exe discussed [here](https://exiftool.org/). That page has a link
for downloading the tool. It has a great number of features.

An example of running it can be seen below. The file `IMG00238.JPG` is used in examples through 
this document. It displays the EXIF tags our code has inserted,
with the exception of the neural network custom tag. 

```
exiftool IMG00238.JPG
ExifTool Version Number         : 12.89
File Name                       : IMG00238.JPG
Directory                       : .
File Size                       : 11 kB
File Modification Date/Time     : 2025:07:09 02:12:14+12:00
File Access Date/Time           : 2025:07:09 16:37:46+12:00
File Creation Date/Time         : 2025:07:09 14:17:10+12:00
File Permissions                : -rw-rw-rw-
File Type                       : JPEG
File Type Extension             : jpg
MIME Type                       : image/jpeg
Exif Byte Order                 : Little-endian (Intel, II)
Make                            : Wildlife.ai
Camera Model Name               : WW500
Resolution Unit                 : inches
X Resolution                    : 640
Y Resolution                    : 480
Date/Time Original              : 2025:07:09 02:12:08
Create Date                     : 2025:07:09 02:12:08
GPS Latitude Ref                : North
GPS Longitude Ref               : East
GPS Altitude Ref                : Above Sea Level
JFIF Version                    : 1.01
Image Width                     : 640
Image Height                    : 480
Encoding Process                : Baseline DCT, Huffman coding
Bits Per Sample                 : 8
Color Components                : 3
Y Cb Cr Sub Sampling            : YCbCr4:2:0 (2 2)
Image Size                      : 640x480
Megapixels                      : 0.307
GPS Altitude                    : 50 m Above Sea Level
GPS Latitude                    : 37 deg 48' 30.00" N
GPS Longitude                   : 122 deg 25' 15.00" E
GPS Position                    : 37 deg 48' 30.00" N, 122 deg 25' 15.00" E
```

#### jpegdump
This is a Python script discussed [here](https://blog.didierstevens.com/2018/01/29/new-tool-jpegdump-py/)
It is available [here](https://didierstevens.com/files/software/jpegdump_V0_0_10.zip). 
I have downloaded it and placed it in the [_Tools folder](../_Tools/jpegdump.py)

It is useful as it shows the jpeg file tags (e.g. 0xffd8 = SOI, 0xffe1 = APP1 where the EXIF data lives).

An example of running it can be seen below. You can see the EXIF data as the `APP1` section containing 338 bytes.

```
python jpegdump.py IMG00238.JPG
File: IMG00238.JPG
  1 p=0x00000000    : m=ffd8 SOI
  2 p=0x00000002 d=0: m=ffe1 APP1  l=  338 e=3.410573 a=29.429851
  3 p=0x00000156 d=0: m=ffe0 APP0  l=   16 e=2.352746 a=28.461538
  4 p=0x00000168 d=0: m=ffdb DQT   l=   67 e=5.452836 a=15.750000 remark: 65/65 = 1.000000
  5 p=0x000001ad d=0: m=ffdb DQT   l=   67 e=1.417849 a=6.015625 remark: 65/65 = 1.000000
  6 p=0x000001f2 d=0: m=ffc0 SOF0  l=   17 e=2.973557 a=59.785714 remark: p=8 h=480 w=640 c=3
  7 p=0x00000205 d=0: m=ffc4 DHT   l=   31 e=2.815937 a=0.750000
  8 p=0x00000226 d=0: m=ffc4 DHT   l=  181 e=7.270047 a=13.168539
  9 p=0x000002dd d=0: m=ffc4 DHT   l=   31 e=2.799180 a=0.642857
 10 p=0x000002fe d=0: m=ffc4 DHT   l=  181 e=7.243483 a=18.140449
 11 p=0x000003b5 d=0: m=ffda SOS   l=   12 e=2.446439 a=21.222222 remark: c=3
                                  entropy-coded data: l=9842 e=7.217378 a=78.740880 #ff00=5
 12 p=0x00002a35 d=0: m=ffd9 EOI
 13 p=0x00002a37    : *trailing*  l=   33 e=0.000000
```

#### EXIF tags

These are listed [here](https://exiftool.org/TagNames/EXIF.html)

#### Graphic showing structure of a JPEG file
Can be seen [here](https://www.disktuna.com/wp-content/uploads/2016/11/jpeg.jpg) and [here](https://github.com/corkami/pics/blob/master/binary/JPG.png)

Here are [sample files](https://github.com/ianare/exif-samples/tree/master/jpg/exif-org) containing EXIF

#### Tutorials

* [Understanding and Decoding a JPEG Image using Python](https://yasoob.me/posts/understanding-and-writing-jpeg-decoder-in-python/)
* [JPEG and EXIF Data Manipulation in Javascript](https://getaround.tech/exif-data-manipulation-javascript/)

## Python script to display EXIF: `jpegdump_exit.py`
This is a script (largely written by Copilot with my help) that displays the EXIF data in a jpeg file.
It is available in the [_Tools folder](../_Tools/jpegdump_exif.py)

Sample output, from a real file created by the software, is shown below. The hex data itself is shown, 
annotated, in the section below.

```
python jpegdump_exif.py IMG00238.JPG

Found APP1 (EXIF) segment at 0x00000002, length: 338 bytes

--- EXIF Segment Diagnostic Info ---
  APP1 Segment Offset      : 0x00000006
  EXIF Header Start        : 0x00000006 (+0 bytes)
  TIFF Header Start        : 0x0000000C (+6 bytes)
  Endian Format            : Little Endian
  TIFF Magic Number        : 0x002A (should be 0x002A)
  First IFD Offset         : 0x00000008
  Absolute First IFD Addr  : 0x00000014
  IFD Entry Count          : 9
--- End of EXIF Diagnostic ---


IFD at 0x00000014 contains 9 entries:
  Tag: Make                   (0x010F)
    Type: 2  Count: 12  Offset: 0x00000016
    Value: Wildlife.ai
  Tag: Model                  (0x0110)
    Type: 2  Count: 6  Offset: 0x00000022
    Value: WW500
  Tag: Resolution Unit        (0x0128)
    Type: 3  Count: 1  Offset: 0x0000002E
    Value: 0x0200...
  Tag: X Resolution           (0x011A)
    Type: 5  Count: 1  Offset: 0x0000003A
    Value: 640/1
  Tag: Y Resolution           (0x011B)
    Type: 5  Count: 1  Offset: 0x00000046
    Value: 480/1
  Tag: Datetime Original      (0x9003)
    Type: 2  Count: 20  Offset: 0x00000052
    Value: 2025:07:09 02:12:08
  Tag: Datetime Create        (0x9004)
    Type: 2  Count: 20  Offset: 0x0000005E
    Value: 2025:07:09 02:12:08
  Tag: Custom Data            (0xC000)
    Type: 1  Count: 3  Offset: 0x0000006A
    Value: 02 37 C9
  Tag: GPSInfoIFDPointer      (0x8825)
    Type: 4  Count: 1  Offset: 0x00000076
    Value: 0x7a000000...
    --> Following GPSInfoIFDPointer to 0x0000007A

IFD at 0x00000086 contains 6 entries:
  Tag: GPS Latitude Reference (0x0001)
    Type: 2  Count: 2  Offset: 0x00000088
    Value: N
  Tag: GPS Latitude           (0x0002)
    Type: 5  Count: 3  Offset: 0x00000094
    Value: 37/1, 48/1, 3000/100
  Tag: GPS Longitude Reference (0x0003)
    Type: 2  Count: 2  Offset: 0x000000A0
    Value: E
  Tag: GPS Longitude          (0x0004)
    Type: 5  Count: 3  Offset: 0x000000AC
    Value: 122/1, 25/1, 1500/100
  Tag: GPS Altitude Reference (0x0005)
    Type: 1  Count: 1  Offset: 0x000000B8
    Value: 00
  Tag: GPS Altitude           (0x0006)
    Type: 5  Count: 1  Offset: 0x000000C4
    Value: 5000/100
```

## Sample EXIF data

The following are the bytes in an EXIF section created by my software, annotated.
```
// APP1 identifier, and length
FF E1 01 52
// EXIF header 
45 78 69 66 00 00 
// TIFF header ("II" indicate little-endian
49 49 2A 00
// Offset to where the tags start 
08 00 00 00
 
// Number of (12-byte) tags following:
09 00 
// The tags that follow consist of: 2-byte Tag ID / 2-byte type / 4-byte count / 4-byte data or pointer to data
// Make
0F 01 / 02 00 / 0C 00 00 00 / C8 00 00 00 
//Model
10 01 / 02 00 / 06 00 00 00 / D4 00 00 00 
// Resolution Units
28 01 / 03 00 / 01 00 00 00 / 02 00 00 00 
// X resolution
1A 01 / 05 00 / 01 00 00 00 / DA 00 00 00 
// Y resolution
1B 01 / 05 00 / 01 00 00 00 / E2 00 00 00
// Datetime - original
03 90 / 02 00 / 14 00 00 00 / EA 00 00 00 
// Datetime - created
04 90 / 02 00 / 14 00 00 00 / FE 00 00 00 
// Neural Network output (custom tag)
00 C0 / 01 00 / 03 00 00 00 / 02 37 C9 00 
// GPS Info IFD pointer
25 88 / 04 00 / 01 00 00 00 / 7A 00 00 00
// 4 x 00 marks the end of the tags
00 00 00 00 

// GPS IFD starts with the number of (12-byte) tags following:
06 00 
// GPS latitude Reference (string, "E" or "W")
01 00 / 02 00 / 02 00 00 00 / 4E 00 00 00 
// GPS latitude 
02 00 / 05 00 / 03 00 00 00 / 12 01 00 00 
// GPS longitude Reference (string, "N" or "S")
03 00 / 02 00 / 02 00 00 00 / 45 00 00 00 
// GPS longitude
04 00 / 05 00 / 03 00 00 00 / 2A 01 00 00 
// GPS altitude Reference (number, 0 or 1)
05 00 / 01 00 / 01 00 00 00 / 00 00 00 00 
// GPS altitude 
06 00 / 05 00 / 01 00 00 00 / 42 01 00 00 

// 4 x 00 marks the end of the tags
00 00 00 00 

// Data area starts here

// Make string: "Wildlife.ai"
57 69 6C 64 6C 69 66 65 2E 61 69 00 
// Model string: "WW500"
57 57 35 30 30 00 
// X resolution = 2 32-bit words (numerator & denominator)
80 02 00 00 01 00 00 00 
// Y resolution = 2 32-bit words (numerator & denominator)
E0 01 00 00 01 00 00 00 
// Datetime - original (string "2025:07:09 02:12:08")
32 30 32 35 3A 30 37 3A 30 39 20 30 32 3A 31 32 3A 30 38 00 
// Datetime - original (string "2025:07:09 02:12:08")
32 30 32 35 3A 30 37 3A 30 39 20 30 32 3A 31 32 3A 30 38 00 
// GPS latitude, degrees: 2 x 32-bit words (numerator & denominator)
25 00 00 00 01 00 00 00 
// GPS latitude, minutes: 2 x 32-bit words (numerator & denominator)
30 00 00 00 01 00 00 00 
// GPS latitude, seconds: 2 x 32-bit words (numerator & denominator)
B8 0B 00 00 64 00 00 00 
// GPS longitude, degrees: 2 x 32-bit words (numerator & denominator)
7A 00 00 00 01 00 00 00 
// GPS longitude, minutes: 2 x 32-bit words (numerator & denominator)
19 00 00 00 01 00 00 00 
// GPS longitude, seconds: 2 x 32-bit words (numerator & denominator)
DC 05 00 00 64 00 00 00 
// GPS altitude: 2 x 32-bit words (numerator & denominator)
88 13 00 00 64 00 00 00
```
