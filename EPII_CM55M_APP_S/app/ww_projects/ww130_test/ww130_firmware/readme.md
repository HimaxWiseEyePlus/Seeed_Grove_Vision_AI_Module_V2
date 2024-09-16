# WW130 firmware for this application

The 'ww130_test' project works with the project 'Rewild_1' on the WW130.

This folder includes essential/important files from the WW130 project:

- Rewild_1_ww130_b00_000402.zip. This file can be installed on the WW130 using the DFU mechanism.
- Rewild_1_ww130_b00_000503.zip. (see note below)

### NOTE: 11 Sept 24:

As of this date the WW130 software is at v503 and the inter-processor communication algorithm has changed.
To make this ww130_test app work with WW130 V502 etc I haave had to make a small change in ``com_task.c``
which is invoked when ``#define WW130_500`` is uncommented. In this case the PA0 pin is toggled by the Seeed board
to attract the attention of the WW130.




