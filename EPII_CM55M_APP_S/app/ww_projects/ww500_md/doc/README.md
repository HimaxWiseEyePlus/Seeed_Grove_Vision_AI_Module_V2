# ww500_md
#### CGP 14/9/24

This project is an evolving work in progress towards the WW500 app.

This is a copy of ww130_cli downloaded from github on 14/4/25, plus changes.

Notes before 13 April 2025 are [here](../ww130_cli/README_9Sept24.md].

## Changes from 14 April 2025

I began by fully implementing the UTC & GPS enhancements and the preliminary file-upload code, which were missing from github.

The main point of this set of work is to add in MD0360 motion detection work and deep power down code.
Those functions were tested in the `ww500_md_test_1` project and related projects. Code from there is being added here..

Further changes include:

1. Reworking the image_task state machine.
2. Add `inactivity.c' and `inactivity.h` which monitor tasks to determine when nothing is going on, and the chip can enter DPD.
3. Add code to enter DPD.
4. Add code to distinguish wake fro cold boot and wake from warm boot.
5. Change camera from Raspberry Pi (cis_imx219) to HM0360 (cis_hm0360)  
6. Refactored image task
7. Refactored file naming

## TODO

Here are some things that are not working yet:

1. Change timing between boards now that HX6538 is in DPD
2. HX6538 to altert MKL62BA on NN detect events etc
3. MKL62BA to send messages to LoRaWAN
4. On wake from sleep, the HX6538 needs to get the time from the MKL62BA (or does it????)