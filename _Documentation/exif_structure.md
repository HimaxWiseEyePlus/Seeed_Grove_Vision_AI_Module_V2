# EXIF content

This aims to assist understanding for the embedded EXIF data that gets inserted manually into images.
They are inserted via an APP1 block and are added as custom tags. As these are custom tags, generic exif tools don't have meaning to the tags initially so the .config files enable human readable results to map the key:value pairs within the image.

## EXIF config folder
----------------
| File Name | Purpose | Use Case |
|-----------|---------|----------|
| custom_exif_tags.config | Defines custom tag structures and hierarchies | Reading and writing custom tags |
| exiftool_validation_hook.config | Prevents validation warnings for custom tags | Validating EXIF data with custom tags |
| exiftool_complete.config | Combined configuration with tag definitions and validation fix | Complete solution for custom tags |

## ExifTool Custom Tags and Validation

This guide explains how to define custom EXIF tags with complete hierarchies and properly handle validation with ExifTool.

### exiftool_complete.config

This comprehensive configuration combines:

1. **Complete tag definitions** with formatting and conversion functions
2. **Validation fix** to handle custom tags properly
3. **Global options** for sorting and duplicate handling

The file is organized into two main sections:
- Tag definitions for different IFDs
- Validation override with a comprehensive list of tags to ignore

## Overview

We've created a comprehensive configuration file (`exiftool_complete.config`) that:

1. Defines custom EXIF IFD pointers and sub-IFDs with proper tag definitions
2. Makes all custom tags writable
3. Overrides ExifTool's validation to ignore warnings for our custom tags
4. Adds helpful global options for tag sorting and duplicate handling

## Custom EXIF Tag Structure

Our configuration defines three main custom IFD pointers:

| Tag ID | Name | Description | Sub-IFD Contains |
|--------|------|-------------|-----------------|
| 0xF100 | MediaInfoIFD | Media Information IFD Pointer | Media metadata tags |
| 0xF200 | DeploymentInfoIFD | Deployment Information IFD Pointer | Deployment location tags |
| 0xF300 | ObservationInfoIFD | Observation Information IFD Pointer | Observation data tags |

### Sub-IFD Structure

Each main IFD contains specific child tags:

#### Media Information (0xF100-0xF1FF)
- MediaID (0xF101): Unique identifier for the media
- DeploymentString (0xF102): Reference code for deployment
- Timestamp (0xF103): Media timestamp
- FilePath (0xF104): Original file path
- FilePublic (0xF105): Public access flag
- FileMediaType (0xF106): Media type (image/video/audio)

#### Deployment Information (0xF200-0xF2FF)
- DeploymentStringDup (0xF201): Deployment reference code
- LatitudeDecimal (0xF202): Latitude in decimal degrees
- LongitudeDecimal (0xF203): Longitude in decimal degrees
- DeploymentStartTime (0xF204): Deployment start time
- DeploymentEndTime (0xF205): Deployment end time

#### Observation Information (0xF300-0xF3FF)
- ObservationID (0xF301): Unique identifier for observation
- DeploymentStringDup2 (0xF302): Deployment reference code
- EventStart (0xF303): Event start time
- EventEnd (0xF304): Event end time
- ObservationNotes (0xF305): Notes about the observation
- ObservationType (0xF306): Type of observation (animal, human, etc.)

### IFD0 (Main Image Directory)

IFD0 contains basic information about the image and pointers to other IFDs:

| Tag ID | Name | Type | Value | Description |
|--------|------|------|-------|-------------|
| 0x0112 | Orientation | SHORT | 1 | Normal orientation |
| 0x011A | XResolution | RATIONAL | 72/1 | Horizontal resolution (72 DPI) |
| 0x011B | YResolution | RATIONAL | 72/1 | Vertical resolution (72 DPI) |
| 0x0213 | YCbCrPositioning | SHORT | 1 | Centered |
| 0x8769 | ExifOffset | LONG | Offset | Pointer to the Exif SubIFD |
| 0x8825 | GPSInfo | LONG | Offset | Pointer to the GPS SubIFD |

### Exif SubIFD

Contains detailed metadata about the image and our custom tags:

| Tag ID | Name | Type | Value | Description |
|--------|------|------|-------|-------------|
| 0x9000 | ExifVersion | UNDEFINED | "0231" | EXIF version 2.31 |
| 0x9101 | ComponentsConfiguration | UNDEFINED | [1,2,3,0] | Y, Cb, Cr, - |
| 0xA001 | ColorSpace | SHORT | 1 | sRGB |
| 0xA002 | ExifImageWidth | SHORT | 640 | Image width in pixels |
| 0xA003 | ExifImageHeight | SHORT | 480 | Image height in pixels |
| 0xF200 | DeploymentID | ASCII | "DEPLOY-2023-001" | Custom: Project deployment ID |
| 0xF201 | DeploymentProject | ASCII | "Coastal Monitoring Project" | Custom: Project name |
| 0xF300 | ObservationID | ASCII | "OBS-20230615-001" | Custom: Observation ID |
| 0xF301 | ObservationType | ASCII | "Wildlife Survey" | Custom: Type of observation |

### GPS SubIFD

Contains GPS coordinates and related metadata:

| Tag ID | Name | Type | Value | Description |
|--------|------|------|-------|-------------|
| 0x0000 | GPSVersionID | BYTE | [2,2,0,0] | GPS data format version |
| 0x0001 | GPSLatitudeRef | ASCII | "N" or "S" | North or South latitude |
| 0x0002 | GPSLatitude | RATIONAL | [deg, min, sec] | Latitude in DMS format |
| 0x0003 | GPSLongitudeRef | ASCII | "E" or "W" | East or West longitude |
| 0x0004 | GPSLongitude | RATIONAL | [deg, min, sec] | Longitude in DMS format |

### Custom Tag Range Definition

Our implementation uses specific tag ranges within the EXIF IFD:

| Tag Range | Purpose | Description |
|-----------|---------|-------------|
| 0xF200-0xF2FF | Deployment Information | Tags related to field deployment details |
| 0xF300-0xF3FF | Observation Information | Tags related to observation metadata |

### Tag Format Details

#### GPS Coordinates
GPS coordinates are stored in degrees, minutes, seconds format:
- Latitude: 3 RATIONAL values representing degrees, minutes, seconds
- Longitude: 3 RATIONAL values representing degrees, minutes, seconds

Example: `51°30'26.64"N, 0°7'40.08"W` is stored as:
- GPSLatitude: [51/1, 30/1, 26639/1000]
- GPSLatitudeRef: "N"
- GPSLongitude: [0/1, 7/1, 40080/1000]
- GPSLongitudeRef: "W"


## Usage
**How to use with ExifTool:**

# Read tags with proper names
exiftool -config exiftool_complete.config image.jpg

# Validate image with no warnings for custom tags
exiftool -config exiftool_complete.config -validate image.jpg

# Write custom tags
exiftool -config exiftool_complete.config -m \
  -MediaInfoIFD=800 \
  -DeploymentInfoIFD=1200 \
  -ObservationInfoIFD=1600 \
  -MediaID="TEST-001" \
  -DeploymentString="DEPLOY-XYZ" \
  image.jpg

EXIF command line validation
----------------
Various command line tools:
"exiftool -g1 -a -s -warning -validate image_0001_2025-01-02.jpg"
"exiftool image_0001_2025-01-02.jpg -v3"
"jpeginfo -c image_0001_2025-01-02.jpg"
exiftool -config ./Seeed_Grove_Vision_AI_Module_V2/EPII_CM55M_APP_S/app/ww_projects/ww130_cli/exif_config/exiftool_validation_hook.config -G1 -a -validate image.jpg
exiftool -config ./Seeed_Grove_Vision_AI_Module_V2/EPII_CM55M_APP_S/app/ww_projects/ww130_cli/exif_config/exiftool_validation_hook.config -g1 -a -s -warning -validate *.jpg
exiftool -config ./Seeed_Grove_Vision_AI_Module_V2/EPII_CM55M_APP_S/app/ww_projects/ww130_cli/exif_config/exiftool_complete.config *.jpg

## Integration Options

1. **Command Line**: Use the `-config` option with each command
   ```bash
   exiftool -config exiftool_complete.config [options] image.jpg
   ```

2. **System-wide**: Rename the file to `.ExifTool_config` and place it in your home directory
   ```bash
   cp exiftool_complete.config ~/.ExifTool_config
   ```

3. **Application Integration**: Include the config file with your application and specify it in your code

## Example Output

With our configuration, custom tag names are properly displayed and validation shows no warnings for our custom tags:

exiftool -config exiftool_complete.config -G1 -a -v2 image.jpg | grep -A1 "0xf"
  | MediaInfoIFD = 800
  |  - Tag 0xf100 (4 bytes, int32u[1])
  | DeploymentInfoIFD = 1200
  |  - Tag 0xf200 (4 bytes, int32u[1])
  | ObservationInfoIFD = 1600
  |  - Tag 0xf300 (4 bytes, int32u[1])

## Benefits of the Consolidated Approach

1. **Complete Tag Hierarchy**: Full definition of all custom tags in one place
2. **Proper Validation**: No warnings for custom tags, only for actual issues
3. **Writing Support**: All tags are defined as writable
4. **Clean Organization**: Tags grouped by their purpose (Media, Deployment, Observation)

## Notes

1. When writing custom tags, you may still need to use the `-m` option to suppress errors
2. The configuration prevents validation warnings about our custom tags but doesn't address other standard EXIF warnings
3. You might still see validation warnings about standard EXIF tags that are missing, such as:

   Warning = Missing required JPEG ExifIFD tag 0xa002 ExifImageWidth
   Warning = Missing required JPEG ExifIFD tag 0xa003 ExifImageHeight
   Warning = Missing required JPEG IFD1 tag 0x0103 Compression

   These are not related to our custom tags and are normal for JPEGs without complete EXIF data
4. Consider updating the tag list in `@skipTags` if you add new custom tags
5. Writing to sub-IFD tags may be challenging without properly setting up the IFD structure first 