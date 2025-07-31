# Motion Detection Latency
#### CGP 28/7/25

How long does it take from movement to taking a picture of an animal?

Measurements made today (28/7/25) measure 320ms from the time that the HM0360 interrupt pin is asserted to wake the processor until
the first image capture is complete. However and number of factors need to be considered:

1.	At the time of writing the HM0360 is taking a new frame every 1s. If the animal moves just before an HM0360 frame capture
then the above figure represents the latency. But if the animal moves into frame just after the previous frame capture
then a whole second elapses before the next frame is captured and motion is detected. So 320ms becomes 1.3s.

The average (animal in view to image captured) would be 500ms plus 320ms = 820ms.

2.	More frames per second can be taken while in MD mode which would reduce the latency at the cost of increased power consumption.
I will make this a variable, soon.

3.	A number of operations happen between the HM0360 interrupt and starting the frame grabber. I will look to see whether some
of these could be deferred until after the first image is captured. For example I have measured the various SD card 
activities which take c. 170ms. These could be deferred (with some software effort) so the measured 320ms figure would
become 150ms.

4.	If capturing images with Raspberry Pi cameras, the cameras need to be powered up when the processor wakes.
A delay needs to be added before the cameras are ready to take a photo. I have not tried to optimise this yet.
A value between 10ms and 100ms is likey, which would need to be added to the latency.

### A Resonable Average Figure

Assuming:

1.	The HM0360 takes 2 frames/sec in MD mode.
2.	I defer the SD card operations.

Average latency = 400ms

If power is not an issue then:

1.	The HM0360 takes 10 frames/sec in MD mode.
2.	I defer the SD card operations.

Average latency = 200ms
