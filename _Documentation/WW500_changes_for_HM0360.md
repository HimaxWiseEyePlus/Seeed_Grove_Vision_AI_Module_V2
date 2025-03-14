# Changes to WW500.A01 to use HM0360 Image Sensor for Motion Detection
#### CGP 14/3/25

Himax made some recomendations on changing the circuit so that the HM0360 would operate to
wake the HX6538 from deep power down (DPD). These are documented in [Wildlife.ai WW500 Circuit
Modification](images/WW500_Circuit_Modification_20250227.pdf) of Feb 25, 2025. These changes are summarised below.

Key elements are:
* HM0360 should be always-on (so it can detect motion while the HX6538 is in deep power down (DPD) mode.
* Because the HX6538 does not provide a clock to the HM0360 while in DPD, the HM0360 should be set to 
operate with an internal oscillator.
 

## WW500ADT Adapter Board

On that basis, because I did not want to make delicate changes by hand on 5 boards, and also because I wanted to test
some additional "wake from sleep" circuitry before committing to a Rev B00 board, I decided to make a small adapter PCB.

The adapter PCB is called "WW500ADPT". Its schematic and PCB are provided here.

The [WW500ADPT schematic](images/SCH_235050.A00.pdf) shows it has a male and female connector
and some logic gates.

WW500ADPT PCB artwork is shown here:

![Top side view](images/PCB-235051-TOP.A00.svg) 
![Bottom side view](images/PCB-235051-BOTTOM.A00.svg) 

The WW500ADPT plugs onto the WW500 in place of the HM0360 module, and the HM0360 plugs onto the connector
on the top of the adapter. Most signals connect straight through on the adapter, but not all: 
the necessary corrections are made.

The 4-way connector allows a cable assembly (documented below) to get 3.3V power from the WW500 J4 connector,
and routes a wakeup signal to the right place on the WW500.A01

The assembled WW500ADPT is shown below, by itself and assembled on the WW500 with the HM0360.

WW500ADPT PCB artwork is shown here: [TODO - TAKE PHOTOS AND REPLACE THESE!]

![Top side view](images/PCB-235051-TOP.A00.svg) 
![Bottom side view](images/PCB-235051-BOTTOM.A00.svg) 

## 4-Way Cable Assembly

A cable assembly can take 3V3 and GND from the main WW500 PCB. It can deliver the (active high) WAKE
signal from the WW500ADPT to the WW500 track currently labelled as /WAKE - this connects to the HX6538 PA0 pin 
and can be used to wake that processor from deep power down (DPD).

Remove Q1 and solder wire 4 to the Q1 drain.

| WW500ADPT pin | WW500ADPT signal | WW500 Connection | Notes      |
| --------------|------------------|------------------|-------------|
| 1             | GND              | GND              | Via unmodified cable |
| 2             | 3V3              | 3V3              | Via unmodified cable |
| 3             | N/C              | SDA              | Not used. Leave cable unmodified. |
| 4             | WAKE             | /WAKE            | Cut cable and solder at Q1 site (drain). |

## Himax Instructions of 11/3/25

Himax emailed these instructions:

```
You should make the following circuit changes to your existing PCB.

1. 1V8_WE/3V3_WE switch capacitor : C43, C44 -> 1nF
2. Add a level-shifting circuit from SEN_INT to PA0 (without inverter)
3. Add p.u to SEN_SCL & SEN_SDA
4. HM0360 2V8 always on
5. HM0360 XSLEEP/XSHDN should be pull-high (always on)
6. WE2 SEN_XSLEEP/SEN_GPIO/SEN_MCLK should be NC (You can lift the SEN_XSLEEP, SEN_GPIO, and SEN_MCLK pins on the WE2)
7. HM0360 CLK_SEL/MCLK connect to DGND (OSC mode)
```

Instruction 1 needs to be made by changing components on the WW500. 
Instructions 2, 3, 4, 5, 6, 7 are handled by the WW500ADPT PCB.

## Software

Himax have provided software to test this.

[It is a available on Github](https://github.com/stevehuang82/for_wildlife_ai/tree/main/allon_sensor_tflm_freertos)

This location also provides a document on how the HM0360 is used in this scenario:
[HM0360 Motion Detection Setting](https://github.com/stevehuang82/for_wildlife_ai/blob/main/doc/HM0360_Motion_Detection_Setting_20250306.pdf)

The WW500 must use a different bootloader - to support DPD. This is available [here](https://github.com/stevehuang82/for_wildlife_ai/tree/main/we2_image_gen_local_dpd)

