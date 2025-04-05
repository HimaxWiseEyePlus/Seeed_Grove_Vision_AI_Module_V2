# Details of Inter-processor Communications.
#### CGP 10/3/25

As of this date all inter-processor communications have been initiated by the MKL62A,
for instance when the phone app sends a command beginning "AI ". 

However we need to add code for communications initiated by the HX6538, 
for instance when the HX6538 wants to fetch the UTC from the MKL62BA.

It is useful to document these cases.

## MKL62BA initiates communication.

In this case the MKL62BA sends a command to the HX6538 and expects a response.
A typical sequence is this:

1. App sends "AI status" command via BLE.
2. MKL62BA receives an event with this text and parses it.
3. I2Cwrite phase:
	1. MKL62BA sends "status" message (with header, CRC etc) via I2C to the HX6538.
	2. HX6538 receives an interrupt  (I2CCOMM_RX in the IF Task)  when the message arrives.
	3. HX6538 IF Task partly parses the message and sends "status" to the CLI Task.
	4. CLI task parses the message and produces the response, which it returns to the IF Task.
4. I2C Read Phase	
	1. IF Task prepares the I2C payload e.g. "enabled" (and adds header, CRC etc) and presents it to the I2C system.
	2. HX6538 interrupts the MKL62A (pulses inter-processor communications interrupt (IPCI) pin.
	3. MKL62BA receives the interrupt and initiates an I2C read.
	4. I2C read completes and MKL62BA receives an interrupt.
	5. MKL62BA parses the message and extracts the response.
	6. MKL62BA sends the response to the app as a BLE message.
	7. When the I2C read completes the HX6538 also receives an interrupt (I2CCOMM_TX in the IF Task). This markes the end of the exchange.


## HX6538 initiates communication.

In this case the HX6538 sends a command to the MKL62BA and expects a response.
A typical sequence is this:

1. Image task requires UTC for JPEG timestamp - asks IF task
2. I2C Read Phase	
	1. IF Task prepares the I2C payload e.g. "utcreq" (and adds header, CRC etc) and presents it to the I2C system.
	2. HX6538 interrupts the MKL62A (pulses inter-processor communications interrupt (IPCI) pin.
	3. MKL62BA receives the interrupt and initiates an I2C read.
	4. I2C read completes and MKL62BA receives an interrupt.
	5. MKL62BA parses the message and extracts the request.
	6. When the I2C read completes the HX6538 also receives an interrupt. This markes the end of the exchange.
3. I2Cwrite phase:
	1. MKL62BA sends "utc <time>" message (with header, CRC etc) via I2C to the HX6538.
	2. HX6538 receives an interrupt (in the IF Task) when the message arrives.
	3. HX6538 IF Task partly parses the message and sends "utc <time>" to the CLI Task.
	4. CLI task parses the message and extracts the timestamp, which it returns to the Image Task.


#### HX6538 Events and States

| Event      | Initial State | New State | Activity                    |  
|------------|---------------|-----------|-----------------------------|
| I2CCOMM_RX | IDLE          | I2C_RX    | Parse msg, prepare response | 
|            | I2C_RX        | I2C_TX    | Wait for tx completion |
| 

## HX6538 initiates communication.

In this case the HX6538 sends a command to the MKL62BA and expects a response.
A typical sequence is this:


## What happens when the HX6538 is in deep power down (DPD)?

The issue here is that the HX6538 won't respond to I2C communications when it is DPD, and needs to be woken.

On the WW500 there are two signals related to waking and interrupting the HX6538:

| Signal     | MKL62BA pin | HX6538 pin | Direction      |  Function                                  |
|------------|-------------|-----------|-----------------|--------------------------------------------|
| /BLE_WAKE  | P05         | PA0       | Input to HX6538 | Pulsed by MKL62BA to wake HX6538           |
| /IP_INT    | P18         | PB11      | Bidirectional   | Pulsed by HX6538 to ask MKL62A to read I2C |
|            |             |           |                 | Pulsed by MKL62BA? RFU                     |


#### Markdown Syntax
(Note to self: [Markdown Guide](https://confluence.atlassian.com/bitbucketserver083/markdown-syntax-guide-1155483368.html))
