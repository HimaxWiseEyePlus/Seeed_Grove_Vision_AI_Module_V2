# Experiments with Sensecraft
#### CGP 29 August 2025

## Top-Level problem Statement

It would be very helpful if we could get a live stream of video from the WW500 onto a laptop, so that we could review:
* Image quality in general
* In particular to correct auto-exposure for the RP camera
* Effect of LED flash in low-light conditions (esp. is the flash on at the right time).
* Maybe we could extend the output to show the HM0360 motion detection in operation (as a 16x16 array of cells,
each of which may have detected motion). This could help with MD sensitivity.

The standard Seeed Grove Vision AI Module V2 supports live video feeds, in the context of:

* Standard pre-programmed firmware image.
* A web-browser environment that is coupled with model downloads and other features.

Can we get our own board to stream video? Can we display that in the Seeed browser or do we need a different web page?

## Experiments with Sensecraft and Grove Vision AI Module V2

The Seeed Studio pages on the [Grove Vision AI Module V2
](https://wiki.seeedstudio.com/grove_vision_ai_v2_software_support/#-no-code-getting-started-with-sensecraft-ai-)
suggests a way of getting started quickly. I followed the steps it suggests I have documented this below:

0. I think the Grove Vision AI Module V2 needs to be programmed with the 
[default image file](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/6bd00a3a633ab57ec4f03850ccb7a7f1ccc2edc8/Seeed_SenseCraft_AI_2023_12_19T183401_402.img)

1. Connect a RP v1 camera to the the Grove Vision AI Module V2 and plug it into a laptop via the USB cable.

2. Open a Chrome browser and go to [sensecraft.seeed.cc](https://sensecraft.seeed.cc/ai/#/home)
(I had already set up an account, which is probably mandatory).

3. "Step 1: Discover Pretrained Models" - select `Grove Vision AI V2` (not `XIAO ESP32S3 Sense`).
I then selected the Gesture Detection (rock/paper/scissors) example.

4. "Step 2: Deploy and Preview Vision" - click the Deploy button and a window should open showing a serial port to select. Do this.
I then see a "Flashing" progress indication that must be downloading something (model only? Or main software also?). This took a while.
Then live video data appears in the browser pane on the RHS. Superimposed on this are bounding boxes that may include "Rock", "Paper" and Scissors" boxes.
Note I do NOT see the console output that is shown in the web screen shots.

5. I scrolled down to "Workspace" and clicked "Explore Now". That took me to
[https://sensecraft.seeed.cc/ai/device/local/36](https://sensecraft.seeed.cc/ai/device/local/36).
I clicked on Select, then selected the serial port. Then I got not only the live image but also the "Device Logger"
pane with outputs JSON like this:

```
perf:{"preprocess":6,"inference":50,"postprocess":0}
boxes:[]

perf:{"preprocess":6,"inference":50,"postprocess":0}
boxes:[[50,220,105,38,29,2]]

perf:{"preprocess":6,"inference":50,"postprocess":0}
boxes:[[61,220,127,38,29,2]]

perf:{"preprocess":6,"inference":50,"postprocess":0}
boxes:[]
```

## Other Models

Other models are available and maybe these should be tried. I tried the Apple detection model which seemed to work.

## How is this working?

I imagine there is code running on the Grove Vision AI Module V2 that includes the gesture recognition model.

I imagine this is streaming data down the serial port and that this data includes a sequence of JPEG images 
plus metadata relating to the NN model output.

I imagine there is code running in the Chrome browser that parses the serial port stream and paints the screen appropriately.

I [asked ChatGPT](https://chatgpt.com/c/687d985f-31ac-8005-86af-9961de2a8dc6) some months ago and got answers that seemed
a bit vaugue. However today I asked supplementary questions and got further information, including the advice to clone 
a `SenseCraft-Web-Toolkit` repository:

```
git clone https://github.com/Seeed-Studio/SenseCraft-Web-Toolkit.git
cd SenseCraft-Web-Toolkit
```
This code is [here](https://github.com/Seeed-Studio/SenseCraft-Web-Toolkit)

However a quick look at the ChatGPT advice did not seem to match the code I had downloaded - but I did not spend much time on this.

It might be worthwhile downloading that repository, then asking Claude to analyse it and derive documentation for 
the serial protocol. Objectives:

1.	Understand the protocol sod we can be certain that we have found teh right C code to generate it (See below).
2.	Understand how to display the live data in a browser.
3.	Understand how we might extend the protocol, for example to send settings back to the WW500 that might 
adjust autoexposure registers, or flash LED timing settings.

Amongst other things the repository includes:

* [SenseCraft Model Assistant Overview](https://sensecraftma.seeed.cc/introduction/quick_start#quick-start)
* [SenseCraft Model Assistant](https://github.com/Seeed-Studio/ModelAssistant)

which might be useful tools in their own right. (Would these replace Edge Impulse?)

## C source code

I suspect that the [Himax repository](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2)
does not include the source of the default Grove Vision AI Module V2 app.

It does include many [Scenario app examples](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/tree/main/EPII_CM55M_APP_S/app/scenario_app)
 that can be selected to run on the Grove Vision AI Module V2. Do some of these send output that can be
 displayed on the browser?
 
 A search for `send_bytes` shows several apps have a `send_results.cpp` file that sends data in JSON form over the UART.
 In these there seem to be calls to `event_reply()` which sends JPEG data.
 
 Is this the code we should be adding to send jpeg data by UART?
 
 Again, maybe Claude could be given access to this repository and asked if the C code produces data streams that 
 are accepted by the Sensecraft code.
 
 #### Clues
 
 Although the text in the JSON example above does not appear in a text search of the scenarios_app examples, there are some clues that the 
 C source code generates a serial steam that the Chrome browser will expect.
 
 If you connect the board to a terminal emulator (e.g. Teraterm) and reset the board this outut appears:
 
 ```
 {"type": 0, "name": "INIT@STAT?", "code": 0, "data": {"boot_count": 0, "is_ready": 1}}
 ```
 
 Although `INIT@STAT` and `boot_count` is not in the source code, there are functions that will produce
 JSON with the same pattern, such as this:
 
 ```
     const auto& ss_model_info{concat_strings("\r{\"type\": 0, \"name\": \"",
                                  "MODEL?",
                                  "\", \"code\": ",
                                  std::to_string(EL_OK),
                                  ", \"data\": ",
                                  model_info_2_json_str(model_info),
                                  "}\n")};
    send_bytes(ss_model_info.c_str(), ss_model_info.size())
    ```
    
Also, the "SSCMA" name appears here:

```
char info[CONFIG_SSCMA_CMD_MAX_LENGTH]{};
```    
 
 



