# Python script to facilitate loading HM0360 Settings from a file
#### CGP 15/6/25
  
This document describes the `scan_cis_settings.py` script and how it is used.
  
The HM0360 image sensor has many registers which determine its operation. Most of these registers are initialised
at cold boot and remain in place so long at the power is applied.

It is useful to change some register settings after cold boot for these reasons:
1. To change operation dynamically during normal operation.
2. To try different settings during development.

This can be done programmatically from hard-coded settings. But to make this easier I decided to create a 
system that could change settings based on the contents of a file.

### How the Loading from a File Works:
1. **Start with a text file**: This is a human-readable file, using the same syntax as the file that contains the initial register values.
2. **Create a binary file**: This text file is transformed to a binary file by the `scan_cis_settings.py` script.
3. **Place the binary file on the SD card**: That can be done by the developer. Possibly different versions could be loaded dynamically, e.g. by the smart phone app.
4. **Load the settings from the file**: At cold boot, after loading the main list of register settings, the code reads the file, parses it, extracts register/value pairs and updates the HM0360 registers.

The `scan_cis_settings.py` script is a python script to parse a text file (such as `hm0360_strobe_1.txt`) containing entries of type 
`HX_CIS_SensorSetting_t`, like this (these registers relate to control of the STROBE pin):
```
// Strobe Control Registers - 0x3080-0x3089 
		{HX_CIS_I2C_Action_W, 0x3080, 0x0B},	// STROBE_CFG enable 
		{HX_CIS_I2C_Action_W, 0x3081, 0x00},	// STROBE_SEL	
		{HX_CIS_I2C_Action_W, 0x3082, 0x00},	// STROBE_FRONT_H
		{HX_CIS_I2C_Action_W, 0x3083, 0x20},	// STROBE_FRONT_L
		{HX_CIS_I2C_Action_W, 0x3084, 0x00},	// STROBE_END_H
		{HX_CIS_I2C_Action_W, 0x3085, 0x40},	// STROBE_END_L
		{HX_CIS_I2C_Action_W, 0x3086, 0x00},	// STROBE_LINE_H
		{HX_CIS_I2C_Action_W, 0x3087, 0x20},	// STROBE_LINE_L
		{HX_CIS_I2C_Action_W, 0x3088, 0x00},	// STROBE_FRAME_H
		{HX_CIS_I2C_Action_W, 0x3089, 0x04},	// STROBE_FRAME_L

```
The output is a file containing binary data, such as `hm0360_extra.bin`.

This script reads an input text file containing I2C action commands, parses each line, 
and writes the corresponding binary data to an output file.

### How the Python Script Works:
1. **Parsing the input file**: Reads each line and extracts three elements using a regex. Ignores comments which can and should be added.
2. **Mapping the action type**: Converts the action string into its corresponding byte value using a dictionary.
3. **Converting to binary format**: The 16-bit register is stored in little-endian format, and all values are packed into a binary structure.
4. **Writing to a binary file**: The parsed data is written to an output `.bin` file.

### Usage:
Run the script from the command line with specified input and output file names:
  ```sh
  python scan_cis_settings.py hm0360_strobe_1.txt hm0360_extra.bin
  ```

Code running on the HX6538 reads the binary file and processes it. Key lines from the source code are as follows:
```
// Name of file containing extra HM0360 register settings
#define HM0360_EXTRA_FILE "hm0360_extra.bin"

// Initialise extra registers from file
cis_file_process(HM0360_EXTRA_FILE);
```

The function `cis_file_process()` is (typically) used during the cold boot sequence, after the large list of 
fixed register values are loaded. The source of the `cis_file_process()` function follows.

```
/**
 * Read CIS register settings from a file and process them
 *
 * @param filename - name of file containing binary data
 * @return error code
 */
HX_CIS_ERROR_E cis_file_process(const char *filename) {
    FIL file;
    FRESULT res;
    UINT bytes_read;
    HX_CIS_ERROR_E result;
    DWORD file_size;
    uint16_t num_entries ;

    if (!fatfs_mounted()) {
        xprintf("SD card not mounted.\n");
    	return FR_NO_FILESYSTEM;
    }

    // Open file
    res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) {
        xprintf("Error opening '%s': %d\n", filename, res);
        return HX_CIS_ERROR_INVALID_PARAMETERS;
    }

    // Get file size
    file_size = f_size(&file);

    if (file_size % sizeof(HX_CIS_SensorSetting_t) != 0) {
        xprintf("Error: Invalid file size\n");
        f_close(&file);
        return HX_CIS_ERROR_INVALID_PARAMETERS;
    }

    num_entries = file_size / sizeof(HX_CIS_SensorSetting_t);

    // Allocate memory
    HX_CIS_SensorSetting_t *sensor_settings = malloc(file_size);

    if (!sensor_settings) {
        xprintf("Memory allocation failed\n");
        f_close(&file);
        return HX_CIS_UNKNOWN_ERROR;
    }

    // Read the binary data
    res = f_read(&file, sensor_settings, file_size, &bytes_read);
    f_close(&file);

    if (res != FR_OK || bytes_read != file_size) {
        xprintf("Error reading file: %d\n", res);
        free(sensor_settings);
        return HX_CIS_UNKNOWN_ERROR;
    }

    // Apply the settings
    result = hx_drv_cis_setRegTable(sensor_settings, num_entries);
    if (result == HX_CIS_NO_ERROR) {
        xprintf("Processed %d settings from '%s'\n", num_entries, filename);
    }
    else {
        xprintf("Error: hx_drv_cis_setRegTable failed with code %d\n", result);
    }

    free(sensor_settings);

    return result;
}
```



Thanks to ChatGPT!  