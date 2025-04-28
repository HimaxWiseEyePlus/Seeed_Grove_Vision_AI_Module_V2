# EXIF content

This aims to assist understanding for the embedded EXIF data that gets inserted manually into images.
They are inserted via an APP1 block and are added as custom tags. As these are custom tags, generic exif tools don't have meaning to the tags initially so the .config files enable human readable results to map the key:value pairs within the image.

EXIF command line verification
----------------
Various command line tools:
"exiftool -g1 -a -s -warning -validate image_0001_2025-01-02.jpg"
"exiftool image_0001_2025-01-02.jpg -v3"
"jpeginfo -c image_0001_2025-01-02.jpg"
exiftool -config ../Seeed_Grove_Vision_AI_Module_V2/EPII_CM55M_APP_S/app/ww_projects/ww130_cli/exif_config/exiftool_validation_hook.config -G1 -a -validate image_0001_2025-01-02.jpg
exiftool -config ../Seeed_Grove_Vision_AI_Module_V2/EPII_CM55M_APP_S/app/ww_projects/ww130_cli/exif_config/exiftool_validation_hook.config -g1 -a -s -warning -validate *.jpg
exiftool -config ../Seeed_Grove_Vision_AI_Module_V2/EPII_CM55M_APP_S/app/ww_projects/ww130_cli/exif_config/exiftool_complete.config *.jpg

## EXIF config folder
----------------
| File Name | Purpose | Use Case |
|-----------|---------|----------|
| custom_exif_tags.config | Defines custom tag structures and hierarchies | Reading and writing custom tags |
| exiftool_validation_hook.config | Prevents validation warnings for custom tags | Validating EXIF data with custom tags |
| exiftool_complete.config | Combined configuration with tag definitions and validation fix | Complete solution for custom tags |

# ExifTool Custom Tags and Validation

This guide explains how to define custom EXIF tags with complete hierarchies and properly handle validation with ExifTool.

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

## Validation Fix

The configuration includes a validation hook that suppresses warnings for our custom tags:

```perl
# Create a reference to the original CheckTagValue function
*OLD_CheckTagValue = *Image::ExifTool::Validate::CheckTagValue;

# Override the CheckTagValue function
sub CheckTagValue {
    my ($tagID, $format, $count, $value, $tagName, $dirName) = @_;
    
    # Skip validation for our custom tags
    if (defined $tagID) {
        foreach my $skipTag (@skipTags) {
            return '' if ($tagID == $skipTag);
        }
    }
    
    # Otherwise, call the original function
    return OLD_CheckTagValue($tagID, $format, $count, $value, $tagName, $dirName);
}
```

## Usage

### Basic Usage

```bash
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
```

### Writing Sub-IFD Tags

```bash
# Write Media Information tags
exiftool -config exiftool_complete.config -m \
  -MediaID="M20240418-001" \
  -DeploymentString="DEP-2024-04" \
  -FilePublic=1 \
  -FileMediaType="image" \
  image.jpg

# Write Deployment Information tags  
exiftool -config exiftool_complete.config -m \
  -LatitudeDecimal="51.5074" \
  -LongitudeDecimal="-0.1278" \
  -DeploymentStartTime="2024:04:18 09:00:00" \
  image.jpg
```

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

```bash
$ exiftool -config exiftool_complete.config -G1 -a -v2 image.jpg | grep -A1 "0xf"
  | 5)  MediaInfoIFD = 800
  |     - Tag 0xf100 (4 bytes, int32u[1])
  | 6)  DeploymentInfoIFD = 1200
  |     - Tag 0xf200 (4 bytes, int32u[1])
  | 7)  ObservationInfoIFD = 1600
  |     - Tag 0xf300 (4 bytes, int32u[1])
```

## Benefits of the Consolidated Approach

1. **Complete Tag Hierarchy**: Full definition of all custom tags in one place
2. **Proper Validation**: No warnings for custom tags, only for actual issues
3. **Writing Support**: All tags are defined as writable
4. **Clean Organization**: Tags grouped by their purpose (Media, Deployment, Observation)

## Notes

1. When writing custom tags, you may still need to use the `-m` option to suppress errors
2. The configuration prevents validation warnings about our custom tags but doesn't address other standard EXIF warnings
3. You might still see validation warnings about standard EXIF tags that are missing, such as:
   ```
   Warning = Missing required JPEG ExifIFD tag 0xa002 ExifImageWidth
   Warning = Missing required JPEG ExifIFD tag 0xa003 ExifImageHeight
   Warning = Missing required JPEG IFD1 tag 0x0103 Compression
   ```
   These are not related to our custom tags and are normal for JPEGs without complete EXIF data
4. Consider updating the tag list in `@skipTags` if you add new custom tags
5. Writing to sub-IFD tags may be challenging without properly setting up the IFD structure first 