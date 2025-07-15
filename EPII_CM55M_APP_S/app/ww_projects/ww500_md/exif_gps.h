/*
 * exif_gps.h
 *
 * Supports EXIF storage of GPS coordinate data.
 *
 *  There is a need to accept an ASCII string from the app and produce a byte array for saving in the
 *  EXIF section of the JPEG file.
 *
 *  My research so far suggests this:
 *
 *  https://exiftool.org/TagNames/GPS.html
 *
- each of Lat and Long is stored as degrees, minutes seconds.
- for each of these there is a pair of 32-bit values: numerator and denominator
- they are stored in BE format (MS byte first)
- The north/south/east/west values are stored as ASCII values with a '0' byte delimiter

Chat GPT gave this example:
Let's assume we want to store the following GPS latitude:

37° 48' 30.50" (North)

    Convert to Rational Representation:
        Degrees: 37/1
        Minutes: 48/1
        Seconds: 3050/100 (since 30.50 = 3050/100)

Byte Array Representation (Big-Endian)

00 00 00 25   # 37 (Degrees) - numerator
00 00 00 01   # 1 - denominator
00 00 00 30   # 48 (Minutes) - numerator
00 00 00 01   # 1 - denominator
00 00 0B EA   # 3050 (Seconds) - numerator
00 00 00 64   # 100 - denominator

 In the EXIF specification, the directional indicators for latitude and longitude aren’t part of the rational array values
 but are stored separately in dedicated ASCII tags. Here’s how it works:

EXIF GPS Direction Tags

    GPSLatitudeRef (Tag 0x0001):
    This tag indicates whether the latitude is in the northern or southern hemisphere. Its value is stored as an ASCII string,
    typically consisting of a single character followed by a null terminator. For example:
        "N" for north
        "S" for south

    GPSLongitudeRef (Tag 0x0003):
    Similarly, this tag tells you whether the longitude is in the eastern or western hemisphere.
    Its value is also stored as an ASCII string:
        "E" for east
        "W" for west
 *
 *  Created on: 11 Mar 2025
 *      Author: charl
 */

#ifndef EXIF_GPS_H_
#define EXIF_GPS_H_



/********************************** Typedefs ******************************************/

// Structure for storing GPS coordinates in EXIF format
typedef struct {
    uint32_t degrees_num;
    uint32_t degrees_den;
    uint32_t minutes_num;
    uint32_t minutes_den;
    uint32_t seconds_num;
    uint32_t seconds_den;
    char ref; // 'N' or 'S' for latitude, 'E' or 'W' for longitude
} GPS_Coordinate;

// Structure for storing GPS altitude in EXIF format
typedef struct {
    uint32_t altitude_num;
    uint32_t altitude_den;
    uint8_t ref; // 0 = Above Sea Level, 1 = Below Sea Level
} GPS_Altitude;

/********************************** Public Function Definitions **************************************/

// Function to set a GPS coordinate
void exif_gps_set_coordinate(GPS_Coordinate *coord, uint32_t deg_num, uint32_t deg_den, uint32_t min_num, uint32_t min_den, uint32_t sec_num, uint32_t sec_den, char ref);

// Function to set GPS altitude
void exif_gps_set_altitude(GPS_Altitude *alt, uint32_t alt_num, uint32_t alt_den, uint8_t ref);

// Function to set GPS coordinates from an ASCII string
void exif_gps_set_coordinate_from_string(GPS_Coordinate *coord, const char *str, char ref);

// Function to get GPS coordinates as an ASCII string
void exif_gps_get_coordinate_as_string(GPS_Coordinate *coord, char *str, uint16_t bufLen);

// Function to set GPS altitude from an ASCII string
void exif_gps_set_altitude_from_string(GPS_Altitude *alt, const char *str);

void exif_gps_get_altitude_as_string(GPS_Altitude *alt, char *str, uint16_t bufLen);

// Function to parse a full GPS string containing latitude, longitude, and altitude
void exif_gps_parse_full_string(GPS_Coordinate *lat, GPS_Coordinate *lon, GPS_Altitude *alt, const char *str);

// Function to print the GPS coordinate in a readable format
void exif_gps_print_coordinate(const GPS_Coordinate *coord);

// Function to print the GPS altitude in a readable format
void exif_gps_print_altitude(const GPS_Altitude *alt);

// Function to generate the EXIF byte array for GPS data
void exif_gps_generate_byte_array(const GPS_Coordinate *coord, uint8_t *buffer);

// Function to generate the EXIF byte array for GPS altitude
void exif_gps_generate_altitude_byte_array(const GPS_Altitude *alt, uint8_t *buffer);

// Test functions
void exif_gps_test_example_1(char * latString, char latRef, char * longString, char longRef, char * altString);

void exif_gps_test_example_2(char * gps_string);

//  Test function to re-convert coordinates to a string
void exif_gps_test_reconstruct(GPS_Coordinate *lat, GPS_Coordinate *lon, GPS_Altitude *alt);

// Runs all tests with a single command
void exif_gps_test_all(void);

#endif /* EXIF_GPS_H_ */
