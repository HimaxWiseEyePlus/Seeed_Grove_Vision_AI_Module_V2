# WW500 power measurements
#### CGP 2/4/25

I have made adjustments to the code to enable me to make power consumption measurements

Unless otherwise noted, the power is measured as follows:

* WW500 connects to WWIF100 on J2.
* FTDI USB cable powers the board via the WWIF100 (3.3V)
* Multimeter connected to WWIF100 P1 & P3 and link at J3 pins 5 & 6 is removed, so current flows through
multimeter.
* MKL62BA runs its app (v0.8.0?)
* WW500ADPT installed - initially without HM0360
* WW500SD installed - initially without SD card

# Test 1 - Baseline

The app runs, detects no SD card or HM0360, and enters DPD.

Baseline current = 24uA

Interestingly, the current pulses high again 5 times at 5s intervals before stabilising.

Is any of this related to FTDI TxD and RxD pins? When disconnected from WW500:

* FTDI connector pin 4 RxD (Rx from HX6538) measures 3.33v
* FTDI connector pin 5 TxD (Tx to HX6538) measures 3.28V

I pulled out RxD pin 4: no change
I pulled out TxD pin 5: no change

Both pins on HX6538  are 3.3V while awake and 0V in DPD.

# Test 2 - Add SD card

The WW500SD allows me to power the SD card from 3V3_WE which turns off in DPD mode.

As expected, the voltage on all of the SD card pins is 0V during DPD. 
The SD card appears to work OK when the device wakes from sleep.

# Test 3 - Add HM0360

At 1Hz and 2Hz frame rates:
* Total DPD current is c.330uA. 
* 130uA through R10 (1V8 to HM0360) - but pulses higher every 1s? to c. 1mA
* 90uA through R9 (3V3 input to the 2V8 regulator)  - but pulses higher every 1s? to c. 300uA
* Difference between 330uA and R9+R10 current and 24uA baseline current is c. 90uA - where is that going?

 
