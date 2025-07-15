# Adding EXIF data to JPEG files (Himax work)
#### CGP 5/7/25

On 2/7/25 Himax provided a new version of `allon_sensor_tflm_freertos` which included
code to add EXIF data to JPEG files. The changelog reads:
```
20250703
1. Supported the FatFs timestamp feature.
2. Added EXIF information "Make, Camera Model Name, and Date/Time" to the JPEG file.
```

This document describes that code, and provides some extra commentary and links to tools. 

Note that I have not tested this code. But, inspired by the code, I have extended the concepts
by adding EXIF into the JPEG file in the ww500_md project (at the time of writing, only in the 'rp3' branch).

## Code Changes

The approach is as follows:

1. Add `SUPPORT_EXIF` in `common_config.h` which enables the feature when set to 1.
2. The jpeg creation proceeds as before, with the jpeg data being assembled in a buffer
whose address and size can be obtained with `cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);`
(this buffer is `jpegbuf[]` declared in the HM0360 `cisdp_sensor.c`).
3. A new buffer `jpeg_exif_buf[]` is created in `cisdp_sensor.c` - this is 512 bytes larger than the original jpeg buffer.
4. A new function `insert_exif_in_memory()` has two purposes:
	* It creates a 512 byte buffer and proceeds to fill this with data formatted according to the EXIF specification.
	In this example the fields added are:
	1. Camera make ("Himax").
	2. Camera model ("HM0360").
	3. Timestamp - "original".
	4. Timestamp - "create".
	* It copies the original jpeg data from `jpegbuf[]` to the new buffer `jpeg_exif_buf[]`, 
and places the exif data into that buffer as well. Note that the original jpeg buffer remains unmodified.
5. The file with the EXIF data is written to the SD card - by passing the address and size of the new `jpeg_exif_buf[]` buffer.

The simplified code is as follows:
```
	uint32_t jpeg_sz, jpeg_addr, jpeg_exif_addr, jpeg_exif_buff_size, jpeg_exif_size;
    // Get pointers and sizes
	cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);
	jpeg_exif_addr = app_get_jpeg_exif_addr();
	jpeg_exif_buff_size = app_get_jpeg_exif_size();
	
	// Create EXIF and move it plus the JPEG data to the new buffer
	insert_exif_in_memory(jpeg_addr, jpeg_sz, jpeg_exif_addr, jpeg_exif_buff_size, &jpeg_exif_size);
	
	// Write the file
	xsprintf(filename, "image%04d_exif.jpg", g_save_jpg_cnt++);
	fastfs_write_image(jpeg_exif_addr, jpeg_exif_size, filename);
``` 

## Commentary

The approach taken is simple and fine for proof of concept. However it has a few draw-backs, including:

1. The use of two buffers is inefficient. An alternative approach might be to make `jpegbuf[]` large enough for a JPEG file
that includes EXIF (actually, it probably already is quite large enough) and move the JPEG data around within that buffer
to make space for the EXIF segment.

__NOTE: I tried to do this when I added EXIF into the `ww500_md` project, but it did not work.__
It looked like I was inserting the EXIF data OK, but in the file on the disk there was no sign of the 
EXIF APP1 section. I returned to the 2-buffer approach. I speculate it could be related to the JPEG buffer
being in the cache....

2. Generalise the process of generating EXIF data so there is flexibility on which fields might be added. Other fields might include:
	* GPS location.
	* Image resolution.
	* Custom fields contaning the output of the neural network processing.
	* Image thumbnail. 

## Tools

These may be of use when developing with EXIF.

#### exiftool
This is a Windows .exe discussed [here](https://exiftool.org/)
It is available [here](). 

It is useful as it shows the jpeg file tags (e.g. 0xffd8 = SOI, 0xffe1 = APP1 where the EXIF data lives).

An example of running it can be seen below.

```
exiftool HM0360_with_EXIF_fixed.JPG
ExifTool Version Number         : 12.89
File Name                       : HM0360_with_EXIF_fixed.JPG
Directory                       : .
File Size                       : 9.3 kB
Zone Identifier                 : Exists
File Modification Date/Time     : 2025:07:03 17:39:33+12:00
File Access Date/Time           : 2025:07:05 10:17:13+12:00
File Creation Date/Time         : 2025:07:03 17:39:33+12:00
File Permissions                : -rw-rw-rw-
File Type                       : JPEG
File Type Extension             : jpg
MIME Type                       : image/jpeg
Warning                         : [minor] Skipped unknown 2 bytes after JPEG APP1 segment
Exif Byte Order                 : Little-endian (Intel, II)
Resolution Unit                 : inches
JFIF Version                    : 1.01
X Resolution                    : 72
Y Resolution                    : 72
Image Width                     : 640
Image Height                    : 480
Encoding Process                : Baseline DCT, Huffman coding
Bits Per Sample                 : 8
Color Components                : 3
Y Cb Cr Sub Sampling            : YCbCr4:2:0 (2 2)
Image Size                      : 640x480
Megapixels                      : 0.307
```

#### jpegdump
This is a Python script discussed [here](https://blog.didierstevens.com/2018/01/29/new-tool-jpegdump-py/)
It is available [here](https://didierstevens.com/files/software/jpegdump_V0_0_10.zip). 

It is useful as it shows the jpeg file tags (e.g. 0xffd8 = SOI, 0xffe1 = APP1 where the EXIF data lives).

An example of running it can be seen below.

```
python jpegdump.py HM0360_with_EXIF_fixed.JPG
File: HM0360_with_EXIF_fixed.JPG
  1 p=0x00000000    : m=ffd8 SOI
  2 p=0x00000002 d=0: m=ffe1 APP1  l=  103 e=3.287865 a=19.570000
  3 p=0x0000006d d=2: m=ffe0 APP0  l=   16 e=2.352746 a=28.461538
  4 p=0x0000007f d=0: m=ffdb DQT   l=   67 e=5.452836 a=15.750000 remark: 65/65 = 1.000000
  5 p=0x000000c4 d=0: m=ffdb DQT   l=   67 e=1.417849 a=6.015625 remark: 65/65 = 1.000000
  6 p=0x00000109 d=0: m=ffc0 SOF0  l=   17 e=2.973557 a=59.785714 remark: p=8 h=480 w=640 c=3
  7 p=0x0000011c d=0: m=ffc4 DHT   l=   31 e=2.815937 a=0.750000
  8 p=0x0000013d d=0: m=ffc4 DHT   l=  181 e=7.270047 a=13.168539
  9 p=0x000001f4 d=0: m=ffc4 DHT   l=   31 e=2.799180 a=0.642857
 10 p=0x00000215 d=0: m=ffc4 DHT   l=  181 e=7.243483 a=18.140449
 11 p=0x000002cc d=0: m=ffda SOS   l=   12 e=2.446439 a=21.222222 remark: c=3
                                  entropy-coded data: l=8525 e=7.046698 a=78.007156 #ff00=3
 12 p=0x00002427 d=0: m=ffd9 EOI
 13 p=0x00002429    : *trailing*  l=   34 e=0.000000
```

#### EXIF tags

These are listed [here](https://exiftool.org/TagNames/EXIF.html)

#### Graphic showing structure of a JPEG file

[here](https://www.disktuna.com/wp-content/uploads/2016/11/jpeg.jpg)

[and here](https://github.com/corkami/pics/blob/master/binary/JPG.png)

Here are [sample files](https://github.com/ianare/exif-samples/tree/master/jpg/exif-org) containing EXIF

#### Tutorials

* [Understanding and Decoding a JPEG Image using Python](https://yasoob.me/posts/understanding-and-writing-jpeg-decoder-in-python/)
* [JPEG and EXIF Data Manipulation in Javascript](https://getaround.tech/exif-data-manipulation-javascript/)

