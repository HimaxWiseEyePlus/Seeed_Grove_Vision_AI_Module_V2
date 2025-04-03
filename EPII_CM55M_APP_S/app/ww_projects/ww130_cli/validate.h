
#ifndef VALIDATE_H
#define VALIDATE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Constants for JPEG/EXIF validation
#define EXIF_HEADER "Exif\0\0"
#define EXIF_HEADER_SIZE 6
#define TIFF_HEADER_SIZE 8
#define IFD_ENTRY_SIZE 12
#define MAX_APP1_SIZE 65535                                     // Maximum size of APP1 segment (2^16 - 1)
#define MIN_APP1_SIZE (2 + EXIF_HEADER_SIZE + TIFF_HEADER_SIZE) // Minimum valid size

// Error codes for detailed reporting
typedef enum
{
    APP1_VALID = 0,
    ERROR_NULL_DATA,
    ERROR_INSUFFICIENT_SIZE,
    ERROR_INVALID_MARKER,
    ERROR_INVALID_LENGTH,
    ERROR_INVALID_EXIF_HEADER,
    ERROR_INVALID_TIFF_HEADER,
    ERROR_INVALID_BYTE_ORDER,
    ERROR_INVALID_TIFF_MAGIC,
    ERROR_INVALID_IFD_OFFSET,
    ERROR_INVALID_IFD_COUNT,
    ERROR_TRUNCATED_IFD,
    ERROR_INVALID_TAG_TYPE,
    ERROR_INVALID_VALUE_OFFSET,
    ERROR_DATA_PAST_SEGMENT
} APP1ValidationError;

// Structure to hold validation results
typedef struct
{
    APP1ValidationError error_code;
    size_t error_offset;
    const char *error_message;
    uint16_t reported_length;
    uint16_t actual_length;
    uint16_t ifd_count;
    bool is_little_endian;
} APP1ValidationResult;

// Function declarations
APP1ValidationResult validate_app1_block(const uint8_t *data, size_t data_size);
void print_validation_results(const APP1ValidationResult *result);

#endif // VALIDATE_
