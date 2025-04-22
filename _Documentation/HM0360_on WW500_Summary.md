# HM0360 on WW500 Summary
#### CGP 22/4/25

The HM0360 is a an image sensor from Himax. Its web page is [here](https://www.himax.com.tw/products/cmos-image-sensor/always-on-vision-sensors/hm0360/).

There is a demo of the HM0360 being used for motion detection in this [Youtube video](https://www.youtube.com/watch?v=7tuq-vz4aVk)
at about 28:40. A graphic showing motion detection in a grid of 16 x 15 is [here](https://media.licdn.com/dms/image/v2/D5622AQEhOXRfFxke5g/feedshare-shrink_800/feedshare-shrink_800/0/1724290518546?e=1748476800&v=beta&t=yCAhWt4YXZHPhc_3XgUHit6GFtoqd1jhdqUT3vbohow) 

The HM0360 has these attributes that are of significance for the WW500:

* It has a motion detection capability which can wake the processor from sleep when there is a difference between two images that exceeds a threshold - i.e. motion detection.
8 It can connect directly to the HX6538 AI processor.
* It can takes monochrome images up to 640 x 480 pixels (VGA resolution).

The module we are using on the WW50 is the [HM0360-MWA-00FP963](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/4287/HM0360-MWA-00FP963.pdf)
which has the HM0360 sensor on the end of a short flat flexible cable, with a 30-way connector to the PCB.

The key questions about this device, relevant to the WW500 project, were these:

* Could we connect the HM0360 to the AI processor,communicate with it, and collect images?
* Could we put the processor into a deep power down (DPD) mode, and use the HM0360 motion detection features to 
wake the processor when the scene changes?
* Would the power consumption of this design be low?

After some work, and help from Himax, the answer to all of these questions is yes. Power consumption for the whole WW500 is deep power down is c. 330uA.
I would hope to be able to reduce this, but 330uA is satisfactory - it represents over 300 days operation from a set of AA batteries PROVIDED NO OTHER
PROCESSING TAKES PLACE. Of course, the expectation is that other processing will take place - that the WW500 will
respond to motion detection and wake to process the scene.

At present the HM0360 detects motion and then captures a monochrome image at 640 x 480 resolution.

As the HX6538 has two camera interfaces,it is possible that the HM0360 can be used only for motion detection, and 
a different (Raspberry PI) camera can capture the image itself - perhaps in colour or at higher resolution. 

There are a number of registers which determine the behaviour of the HM0360 in motion detection mode. 
These allow the sensitivity to be set, in particular. There are also opportunities to investigate lower power and
faster responsiveness. These are subjects for further investigation in the future.

 






