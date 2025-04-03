#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

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
// Helper function to read 16-bit value considering endianness
uint16_t read_uint16(const uint8_t *data, bool little_endian)
{
    if (little_endian)
    {
        return (uint16_t)(data[0] | (data[1] << 8));
    }
    return (uint16_t)((data[0] << 8) | data[1]);
}
// Helper function to read 32-bit value considering endianness
uint32_t read_uint32(const uint8_t *data, bool little_endian)
{
    if (little_endian)
    {
        return (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
    }
    return (uint32_t)((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
}
// Function to validate TIFF data type and size
bool validate_tiff_type(uint16_t type, uint32_t count, uint16_t total_size)
{
    static const uint8_t type_sizes[] = {
        0, // Type 0 (invalid)
        1, // BYTE
        1, // ASCII
        2, // SHORT
        4, // LONG
        8, // RATIONAL
        1, // SBYTE
        1, // UNDEFINED
        2, // SSHORT
        4, // SLONG
        8, // SRATIONAL
        4, // FLOAT
        8  // DOUBLE
    };
    if (type == 0 || type > 12)
    {
        return false;
    }
    uint32_t required_size = count * type_sizes[type];
    return required_size <= total_size;
}
// Main validation function
APP1ValidationResult validate_app1_block(const uint8_t *data, size_t data_size)
{
    APP1ValidationResult result = {
        .error_code = APP1_VALID,
        .error_offset = 0,
        .error_message = "Valid APP1 block",
        .reported_length = 0,
        .actual_length = 0,
        .ifd_count = 0,
        .is_little_endian = true};
    // Basic size checks
    if (!data)
    {
        result.error_code = ERROR_NULL_DATA;
        result.error_message = "NULL data pointer";
        return result;
    }
    if (data_size < MIN_APP1_SIZE)
    {
        result.error_code = ERROR_INSUFFICIENT_SIZE;
        result.error_message = "Data size too small for APP1 block";
        return result;
    }
    // Validate APP1 marker
    if (data[0] != 0xFF || data[1] != 0xE1)
    {
        result.error_code = ERROR_INVALID_MARKER;
        result.error_message = "Invalid APP1 marker";
        result.error_offset = 0;
        return result;
    }
    // Validate length
    result.reported_length = (data[2] << 8) | data[3];
    if (result.reported_length < MIN_APP1_SIZE || result.reported_length > MAX_APP1_SIZE)
    {
        result.error_code = ERROR_INVALID_LENGTH;
        result.error_message = "Invalid APP1 length";
        result.error_offset = 2;
        return result;
    }
    if (result.reported_length > data_size)
    {
        result.error_code = ERROR_INSUFFICIENT_SIZE;
        result.error_message = "APP1 block truncated";
        result.error_offset = data_size;
        return result;
    }
    // Validate EXIF header
    if (memcmp(data + 4, EXIF_HEADER, EXIF_HEADER_SIZE) != 0)
    {
        result.error_code = ERROR_INVALID_EXIF_HEADER;
        result.error_message = "Invalid EXIF header";
        result.error_offset = 4;
        return result;
    }
    // TIFF header validation
    const uint8_t *tiff_data = data + 4 + EXIF_HEADER_SIZE;
    // Check byte order
    if (memcmp(tiff_data, "II", 2) == 0)
    {
        result.is_little_endian = true;
    }
    else if (memcmp(tiff_data, "MM", 2) == 0)
    {
        result.is_little_endian = false;
    }
    else
    {
        result.error_code = ERROR_INVALID_BYTE_ORDER;
        result.error_message = "Invalid byte order marker";
        result.error_offset = 4 + EXIF_HEADER_SIZE;
        return result;
    }
    // Validate TIFF version (42)
    uint16_t tiff_version = read_uint16(tiff_data + 2, result.is_little_endian);
    if (tiff_version != 42)
    {
        result.error_code = ERROR_INVALID_TIFF_MAGIC;
        result.error_message = "Invalid TIFF version number";
        result.error_offset = 4 + EXIF_HEADER_SIZE + 2;
        return result;
    }
    // Validate IFD offset
    uint32_t ifd_offset = read_uint32(tiff_data + 4, result.is_little_endian);
    if (ifd_offset < 8 || ifd_offset >= (result.reported_length - 4 - EXIF_HEADER_SIZE))
    {
        result.error_code = ERROR_INVALID_IFD_OFFSET;
        result.error_message = "Invalid IFD offset";
        result.error_offset = 4 + EXIF_HEADER_SIZE + 4;
        return result;
    }
    // Validate IFD entries
    const uint8_t *ifd_data = tiff_data + ifd_offset;
    result.ifd_count = read_uint16(ifd_data, result.is_little_endian);
    if (result.ifd_count == 0)
    {
        result.error_code = ERROR_INVALID_IFD_COUNT;
        result.error_message = "No IFD entries found";
        result.error_offset = 4 + EXIF_HEADER_SIZE + ifd_offset;
        return result;
    }
    // Check if we have enough space for IFD entries
    size_t required_ifd_size = 2 + (result.ifd_count * IFD_ENTRY_SIZE) + 4;
    if (ifd_offset + required_ifd_size > (result.reported_length - 4 - EXIF_HEADER_SIZE))
    {
        result.error_code = ERROR_TRUNCATED_IFD;
        result.error_message = "IFD entries truncated";
        result.error_offset = 4 + EXIF_HEADER_SIZE + ifd_offset;
        return result;
    }
    // Validate each IFD entry
    const uint8_t *entry = ifd_data + 2;
    for (uint16_t i = 0; i < result.ifd_count; i++)
    {
        uint16_t tag = read_uint16(entry, result.is_little_endian);
        uint16_t type = read_uint16(entry + 2, result.is_little_endian);
        uint32_t count = read_uint32(entry + 4, result.is_little_endian);
        uint32_t value_offset = read_uint32(entry + 8, result.is_little_endian);
        // Validate type
        if (!validate_tiff_type(type, count, result.reported_length))
        {
            result.error_code = ERROR_INVALID_TAG_TYPE;
            result.error_message = "Invalid tag type or count";
            result.error_offset = entry - data;
            return result;
        }
        // Validate value offset if data doesn't fit in 4 bytes
        if (type * count > 4 &&
            value_offset > (result.reported_length - 4 - EXIF_HEADER_SIZE))
        {
            result.error_code = ERROR_INVALID_VALUE_OFFSET;
            result.error_message = "Invalid value offset";
            result.error_offset = entry - data + 8;
            return result;
        }
        entry += IFD_ENTRY_SIZE;
    }
    return result;
}
// Helper function to print validation results
void print_validation_results(const APP1ValidationResult *result)
{
    printf("APP1 Validation Results :\n");
    printf("Status : %s\n", result->error_message);
    printf("Error Code : %d\n", result->error_code);
    if (result->error_code != APP1_VALID)
    {
        printf("Error Offset : %zu\n", result->error_offset);
    }
    printf("Reported Length : %u\n", result->reported_length);
    printf("IFD Entry Count : %u\n", result->ifd_count);
    printf("Byte Order : %s\n", result->is_little_endian ? "Little Endian" : "Big Endian");
}
// // Example usage
// int main()
// {
//     // Example APP1 block data
//     uint8_t test_data[] = {
//         0xFF, 0xE1,                     // APP1 marker
//         0x00, 0x42,                     // Length
//         'E', 'x', 'i', 'f', 0x00, 0x00, // EXIF header
//         'I', 'I',                       // TIFF byte order (little endian)
//         0x2A, 0x00,                     // TIFF magic number
//         0x08, 0x00, 0x00, 0x00,         // IFD offset
//         // ... rest of your TIFF/EXIF data ...
//     };
//     APP1ValidationResult result = validate_app1_block(test_data, sizeof(test_data));
//     print_validation_results(&result);
//     return 0;
// }