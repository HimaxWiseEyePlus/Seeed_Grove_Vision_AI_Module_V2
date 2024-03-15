# SSCMA Deployment on Himax WE-II

<div align="center">
  <img width="100%" src="https://cdn.jsdelivr.net/gh/Seeed-Studio/SSCMA-Micro@dev/docs/images/sscma.png">
</div>

## Introduction

The project provides examples of how to deploy models from SSCMA to Himax WE-II chipsets.


## Getting Started

### Clone and Setup the Repository

Clone our repository.

```sh
git clone https://github.com/Seeed-Studio/sscma-example-we2
```

Go to `sscma-example-we2` folder.

```sh
cd sscma-example-we2
```

Fetch the submodules.

```sh
git submodule update --init
```

## Develop Environment

Tested build toolchains:

- `arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi`
- `arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi`

*Note: You can download the toolchain from [Arm GNU Toolchain Downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).*

Supported cameras:

- `hm0360`
- `ov5647`


## Build the Firmware

### 0x00

```sh
git clone https://github.com/Seeed-Studio/sscma-example-we2
cd sscma-example-we2
```

### 0x01

```sh
make clean && make -j

# the output elf is located at obj_epii_evb_icv30_bdv10/gnu_epii_evb_WLCSP65/EPII_CM55M_gnu_epii_evb_WLCSP65_s.elf
```

### 0x02

Please use `we2_local_image_gen` to generate the firmware binary (currently we do not publicize this tool).

## Flash and Run

### 0x00

Flash the firmware using `EPII_ISP_TOOL` or Xmodem.

### 0x01

Flash the model to address `0x00400000` using `EPII_ISP_TOOL`, the model files are located at:

```
8078326f275ce87a371bbb273b010f9dce93f1c0  model_zoo/ee.tflite
07ea3502bda2351184b3670b7a342ad1896765e3  model_zoo/meter_seg7_vela.tflite
```

*Note: currently only support single model for inference.*

### 0x02

Connect to the computer using a Type-C cable, use `minicom` or alternative to monitor the serial outputs and control the device. These commands are for testing:

- `AT+SAMPLE=-1`
- `AT+INVOKE=-1,0,1`
- `AT+INVOKE=-1,0,0,`
- `AT+BREAK`

All the commands that available for test can be found in this documents [AT Protocol Specification](https://github.com/Seeed-Studio/SSCMA-Micro/blob/dev/docs/protocol/at_protocol.md).

## Supported Models

Please refer to [SSCMA Model Zoo](https://github.com/Seeed-Studio/sscma-model-zoo) for details.


## Contributing

- If you find any issue in using these examples, or wish to submit an enhancement request, please use the raise a [Issue](https://github.com/Seeed-Studio/sscma-example-esp32/issues) or submit a [Pull Request](https://github.com/Seeed-Studio/sscma-example-esp32/pulls).


## License

```
These examples are covered under MIT License.

These examples use the ESP-IDF which is covered under Apache License 2.0.

TensorFlow, FlashDB, JPEGENC and other third-party libraries are distribute under their own License.
```
