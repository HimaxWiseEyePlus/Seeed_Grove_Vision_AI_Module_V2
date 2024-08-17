# Project List

This is a very brief summary of Wildlife Watcher projects in the 'ww_projects' directory.
Each of these should have its own readme.md or readme.txt file for full information.

Projects are arranged here roughly in chronological order, with the newest at the bottom, rather 
than in alphabetical order as they are likely to appear in a Project Explorer pane.

If you want to try different projects, edit ww.mk and uncomment one of the 'APP_TYPE = xxx' lines.

i2c_slave_app
---------------
This was based on sample code received from Himax on 28/6/24, in response to a request for an example
of using Seeed board as an I2C slave. This modified version works with the WW130 as the I2C master.

i2c_slave_test
----------------
I started this after i2c_slave_app but did not complete it. Instead I moved onto ww130_test.
This app is here in case I need to return to it later.

ww130_test
------------
This project is to establish essential functionality, based on a previous Himax project.
It adds SD card storage of images, WW130 I2C communications and interrupts between the two processors.
It includes sending messages from a phone to the WW130 to the Seeed board to carry out commands.
(Note this mechanism is being replaced in ww130_cli. But at the time of writing (17/8/24) the ww130_cli
work is not completed, so this project is the best one to demonstrate many of the essential functions).

ww_template
--------------
This project is a fairly minimal project using FreeRTOS and a command line interface (CLI). 
It can be used as the basis for other projects that use both of those. (It does not include WW130 comms).

CLI is a library from FreeRTOS that allows text commands to be sent from the serial port console in a 
flexible and exstensible way. This is great for development work. in ww130_cli I will extend this to
commands sent from a phone using BLE.

My idea was to begin to clean up the preceding projects and move towards a structure that we would be comfortable with
working with in the medium term.

To understand the strategy for the structure, look at the Tasks_and_states.md file in the ww_130_test project.

ww130_cli
-------------
At the time of writing (17/8/24) this is work in progress. It aims to merge the features in ww130_test 
into the structure of ww_template. In particular, the idea is to send strings from a phone through the ww130
to the Seeed board and feed these into the CLI mechanism. This allows a command to be sent to the Seeed board
from either the console or a phone, and for these to be processed the same way.

Enhancements will be added - watch this space!








