## How to build yolov8n gender classification scenario_app and run on WE2?
- This is the scenario_app which will run face detection first and then choose the biggest face to do the gender classification.
  ![alt text](../../../../images/gen_cls_0.png)
### Linux Environment
- Change the `APP_TYPE` to `tflm_yolov8_gender_cls` at [makefile](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/makefile)
    ```
    APP_TYPE = tflm_yolov8_gender_cls
    ```
- Build the firmware reference the part of [Build the firmware at Linux environment](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2?tab=readme-ov-file#build-the-firmware-at-linux-environment)
- How to flash firmware image and model at [model_zoo](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/tree/main/model_zoo)?
  - Prerequisites for xmodem
    - Please install the package at [xmodem/requirements.txt](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/tree/main/xmodem/requirements.txt) 
        ```
        pip install -r xmodem/requirements.txt
        ```
  - Disconnect `Minicom`
  - Make sure your `Seeed Grove Vision AI Module V2` is connect to PC.
  - Open the permissions to acceess the deivce
    ```
    sudo setfacl -m u:[USERNAME]:rw /dev/ttyUSB0
    # in my case
    # sudo setfacl -m u:kris:rw /dev/ttyACM0
    ```
    ![alt text](../../../../images/flash_image_model_6.png)
  - Open `Terminal` and key-in following command
    - port: the COM number of your `Seeed Grove Vision AI Module V2`, for example,`/dev/ttyACM0`
    - baudrate: 921600
    - file: your firmware image [maximum size is 1MB]
    - model: you can burn multiple models "[model tflite] [position of model on flash] [offset]"
      - Position of model on flash is defined at [~/tflm_yolov8_gender_cls/common_config.h](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/app/scenario_app/tflm_yolov8_gender_cls/common_config.h#L23)
        ```
        python3 xmodem/xmodem_send.py --port=[your COM number] --baudrate=921600 --protocol=xmodem --file=we2_image_gen_local/output_case1_sec_wlcsp/output.img --model="model_zoo/tflm_yolov8_gender_cls/0_fd_0x200000.tflite 0x200000 0x00000" --model="model_zoo/tflm_yolov8_gender_cls/1_gender_cls_0x280000.tflite 0x280000 0x00000"

        # example:
        # python3 xmodem/xmodem_send.py --port=/dev/ttyACM0 --baudrate=921600 --protocol=xmodem --file=we2_image_gen_local/output_case1_sec_wlcsp/output.img --model="model_zoo/tflm_yolov8_gender_cls/0_fd_0x200000.tflite 0x200000 0x00000" --model="model_zoo/tflm_yolov8_gender_cls/1_gender_cls_0x280000.tflite 0x280000 0x00000"
        ```
    - It will start to burn firmware image and model automatically.
  -  Please press `reset` buttun on `Seeed Grove Vision AI Module V2`.
    ![alt text](../../../../images/grove_vision_ai_v2_all.jpg) 
  - It will success to run the algorithm.

[Back to Outline](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2?tab=readme-ov-file#outline)

### Windows Environment
- Change the `APP_TYPE` to `tflm_yolov8_gender_cls` at [makefile](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/makefile)
    ```
    APP_TYPE = tflm_yolov8_gender_cls
    ```
- Build the firmware reference the part of [Build the firmware at Windows environment](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2?tab=readme-ov-file#build-the-firmware-at-windows-environment)
- How to flash firmware image and model at [model_zoo](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/tree/main/model_zoo)?
  - Prerequisites for xmodem
    - Please install the package at [xmodem/requirements.txt](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/tree/main/xmodem/requirements.txt) 
        ```
        pip install -r xmodem/requirements.txt
        ```
  - Disconnect `Tera Term`
  - Make sure your `Seeed Grove Vision AI Module V2` is connect to PC.
  - Open `CMD` and key-in following command
    - port: the COM number of your `Seeed Grove Vision AI Module V2` 
    - baudrate: 921600
    - file: your firmware image [maximum size is 1MB]
    - model: you can burn multiple models "[model tflite] [position of model on flash] [offset]"
      - Position of model on flash is defined at [~/tflm_yolov8_gender_cls/common_config.h](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/app/scenario_app/tflm_yolov8_gender_cls/common_config.h#L23)
        ```
        python xmodem\xmodem_send.py --port=[your COM number] --baudrate=921600 --protocol=xmodem --file=we2_image_gen_local\output_case1_sec_wlcsp\output.img --model="model_zoo\tflm_yolov8_gender_cls\0_fd_0x200000.tflite 0x200000 0x00000" --model="model_zoo\tflm_yolov8_gender_cls\1_gender_cls_0x280000.tflite 0x280000 0x00000"

        # example:
        # python xmodem\xmodem_send.py --port=COM131 --baudrate=921600 --protocol=xmodem --file=we2_image_gen_local\output_case1_sec_wlcsp\output.img --model="model_zoo\tflm_yolov8_gender_cls\0_fd_0x200000.tflite 0x200000 0x00000" --model="model_zoo\tflm_yolov8_gender_cls\1_gender_cls_0x280000.tflite 0x280000 0x00000"
        ```
    - It will start to burn firmware image and model automatically.
  -  Please press `reset` buttun on `Seeed Grove Vision AI Module V2`.
    ![alt text](../../../../images/grove_vision_ai_v2_all.jpg)  
  - It will success to run the algorithm.


[Back to Outline](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2?tab=readme-ov-file#outline)

### Send image and meta data by UART
- Disconnect the uart at your `Tera Term` or `Minicom` first.
- You can use the [Himax AI web toolkit](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/releases/download/v1.1/Himax_AI_web_toolkit.zip) which we provide, download it and unzip it to local PC, and double click `index.html`.
- Please check you select `Grove Vision AI(V2)` and press `connect` button
    ![alt text](../../../../images/uart_show_1.png)
- Select your own COM.
    ![alt text](../../../../images/uart_show_2.png)
- You will see the preview result on website.
- Tip
    - Windows:
        - Please use "Microsoft Edge" browser
    - Linux:
        - Open the permissions to acceess the deivce
            ```
            sudo setfacl -m u:[USERNAME]:rw /dev/ttyUSB0
            # in my case
            # sudo setfacl -m u:kris:rw /dev/ttyACM0
            ```
        - Please use "Google Chrome" browser

[Back to Outline](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2?tab=readme-ov-file#outline)

### Model source link
- [Face detection](https://github.com/dog-qiuqiu/Yolo-Fastest)
- [Yolov8n gender classification](https://hub.degirum.com/degirum/public/yolov8n_relu6_gender--160x160_quant_tflite_multidevice_1)

[Back to Outline](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2?tab=readme-ov-file#outline)