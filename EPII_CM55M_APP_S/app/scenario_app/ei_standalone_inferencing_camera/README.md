# Edge Impulse Example: standalone inferencing using Grove Vision AI Module V2 (Himax WiseEye2)

This example runs the model exported from the [Edge Impulse Studio](https://studio.edgeimpulse.com/) on the [Grove Vision AI Module V2](https://wiki.seeedstudio.com/grove_vision_ai_v2/) ([Himax WiseEye2](https://www.himax.com.tw/products/wiseeye-ai-sensing/wiseeye2-ai-processor/)). It is bare minimal source code required to run the inference using the camera as an input sensor.
The default model included in this example is face detection using the [Edge Impulse FOMO model](https://docs.edgeimpulse.com/docs/edge-impulse-studio/learning-blocks/object-detection/fomo-object-detection-for-constrained-devices).

To test your model using this example, you need to perform four steps: train, deploy, build firmware, and flash.

## How to train your model using Edge Impulse Studio?

To train your model, check the [end-to-end tutorials in the Edge Impulse Documentation](https://docs.edgeimpulse.com/docs/tutorials/end-to-end-tutorials). As this example uses a camera as a sensor, the best recommended examples are:
* [Image classification](https://docs.edgeimpulse.com/docs/tutorials/end-to-end-tutorials/image-classification)
* [Object detection using FOMO](https://docs.edgeimpulse.com/docs/tutorials/end-to-end-tutorials/object-detection/detect-objects-using-fomo)

## How to deploy your model from the Edge Impulse Studio?

To deploy your model converted to Ethos-U55-64 format, in your project, go to the `Deployment` section (`step 1`) and then in the search field type win `wiseeye2` (`step 2`). You should see the result in the image below:

![Ethos-U55-64 deployment in the Edge Impulse Studio](../../../../images/ei-ethos-deployment-1.png)

After selecting the deployment target, make sure the `Quantized (int8)` optimization is selected (`step 3`) - Ethos NPU doesn't support unoptimized (float32) models. Then click `Build` (`step 4`)

![Ethos-U55-64 deployment in the Edge Impulse Studio](../../../../images/ei-ethos-deployment-2.png)

After downloading the zip archive, extract it. Then move the content of the archive (three directories) into `EPII_CM55M_APP_S/app/scenario_app/ei_standalone_inferencing_camera/ei-model` and replace the existing one.

## Getting raw features (for non-image projects)

If you are using a model that is using audio or other input data, you need to provide static data from your model. The image data is covered in this example by enabling camera support and image processing path.
To get the static raw features for your model (for non-image project) follow the steps below:

1. Go to the DSP block in your impulse (`Spectral features` in the example below), then select the interesting sample (`step 2`) and copy the features into the clipboard (`step 3`).

   ![Getting raw features from the Edge Impulse Studio](../../../../images/ei-getting-raw-features.png)

2. Open the [ei_standalone_inferencing.cpp](ei_standalone_inferencing.cpp#L15) and paste (replacing) the copied features into the `static const float features[]` array.

## How to build your firmware?

1. Open the [EPII_CM55M_APP_S/makefile](../../../makefile#L149), find the line that is setting `APP_TYPE` and change it to:
   ```
   APP_TYPE = ei_standalone_inferencing_camera
   ```
2. Follow the [instructions in the main README](../../../../README.md#how-to-build-the-firmware) to build the firmware

## How to flash your firmware?

The easiest approach is to use the [himax-flash-tool](https://docs.edgeimpulse.com/docs/tools/edge-impulse-cli/himax-flash-tool) which is a part of the [Edge Impulse CLI](https://docs.edgeimpulse.com/docs/tools/edge-impulse-cli) tools.
1. Install the flashtool following the instructions in the [Edge Impulse docs](https://docs.edgeimpulse.com/docs/tools/edge-impulse-cli/cli-installation)
2. Open your terminal window in the root directory of this repository
3. Run the following command
   ```bash
   himax-flash-tool -d WiseEye2 -f output_case1_sec_wlcsp/output.img
   ```

If you don't want to install the Edge Impulse CLI tools, follow the [official guide on flashing the board](../../../../README.md#how-to-flash-the-firmware)


(c) 2024 Edge Impulse Inc. All rights reserved