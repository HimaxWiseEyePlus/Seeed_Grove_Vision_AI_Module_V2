/*
 * exif_gps.c
 *
 *  Created on: 11 Mar 2025
 *      Author: charl
 */

/********************************** Includes ******************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "exif_gps.h"

/*************************************** Local routine declarations  *************************************/

static void setNumDenFromString(uint32_t * num, uint32_t * dem, char * decimalNumber);

/********************************** Local Variables **************************************/

// Save the GPS location of the device in this file
// Other files can reference these as extern
GPS_Coordinate exif_gps_deviceLat;
GPS_Coordinate exif_gps_deviceLon;
GPS_Altitude exif_gps_deviceAlt;

/*************************************** Local function definitions  *************************************/

/**
 * A helper function that parses a decimal number string and returns numerator and denominator
 *
 * @param num - pointer to numerator
 * @param den - pointer to denominator
 * @param pointer to string
 */
static void setNumDenFromString(uint32_t * num, uint32_t * dem, char * decimalNumber) {
    char * dot;	// points to the palce in the string where the dot is
    uint8_t decimalPlaces = 0;
    uint32_t numerator;
    uint32_t denominator = 1;

    // A char type pointer to the position in the string where the character was found
    // or NULL if the character was not found.
    dot = strchr(decimalNumber, '.');

    printf("DEBUG: '%s' = ", decimalNumber);	// print this before we mess with the dot character

    if (dot) {
         // Split at the decimal point (replace '.' with ' ')
    	// Typically, the string will end with the '"' character (seconds)
    	*dot = '\0';
        decimalPlaces = strspn((dot + 1), "0123456789"); // number of integer characters before the '"'
        numerator = atoi(decimalNumber);
        for (uint8_t i=0; i < decimalPlaces; i++) {
        	numerator = numerator * 10;
        	denominator = denominator * 10;
        }
        numerator += atoi(dot + 1);
    }
    else {
        numerator = atoi(decimalNumber);
        denominator = 1;
    }

    // print the result
    printf("%u/%u (decimal places = %d decimal part '%s')\n", (int) numerator, (int) denominator, decimalPlaces, (dot + 1));
    *num = numerator;
    *dem = denominator;
}

/*************************************** Global function definitions  *************************************/

/**
 * Initialises a latitude or longitude structure from component parts
 *
 * @param coord - pointer to a structure of type GPS_Coordinate
 * @param deg_num - degrees (numerator)
 * @param deg_den - degrees (denominator)
 * @param min_num - minutes (numerator)
 * @param min_den - minutes (denominator)
 * @param sec_num - seconds (numerator)
 * @param sec_den - seconds (denominator)
 * @param ref - one of 'N', 'S', 'E', 'W'
 */
void exif_gps_set_coordinate(GPS_Coordinate *coord, uint32_t deg_num, uint32_t deg_den,
		uint32_t min_num, uint32_t min_den, uint32_t sec_num, uint32_t sec_denom, char ref) {

    if (!coord) {
    	return;
    }

    coord->degrees_num = deg_num;
    coord->degrees_den = deg_den;
    coord->minutes_num = min_num;
    coord->minutes_den = min_den;
    coord->seconds_num = sec_num;
    coord->seconds_den = sec_denom;
    coord->ref = ref;
}

/**
 * Initialises an altitude structure from component parts
 *
 * @param coord - pointer to a structure of type GPS_Coordinate
 * @param alt_num - metres (numerator)
 * @param alt_den - metres (denominator)
 * @param ref - 0 = above, 1 = below
 */
void exif_gps_set_altitude(GPS_Altitude *alt, uint32_t alt_num, uint32_t alt_den, uint8_t ref) {
    if (!alt)  {
    	return;
    }

    alt->altitude_num = alt_num;
    alt->altitude_den = alt_den;
    alt->ref = ref;
}

/**
 * Function to set GPS coordinate from an ASCII string
 *
 * (parsing decimal seconds as an integer fraction)
 *
 * @param coord - pointer to a structure of type GPS_Coordinate
 * @param str - string containing the coordinate
 * @param ref - one of 'N', 'S', 'E', 'W'
 */
void exif_gps_set_coordinate_from_string(GPS_Coordinate *coord, const char *str, char ref) {
    uint32_t deg;
    uint32_t min;
    uint32_t sec_num = 0;
    uint32_t sec_den = 1;
    char sec_str[16] = {0};
    uint8_t parsed;


    if (!coord || !str) {
    	return;
    }

//    sec_num = 0;
//    sec_den = 1;

    parsed = sscanf(str, "%u°%u'%15s\"", (unsigned int *)&deg, (unsigned int *)&min, sec_str);

    if (parsed < 3) {
    	return;
    }

    setNumDenFromString(&sec_num, &sec_den, sec_str);
//    char *dot = strchr(sec_str, '.');
//
//    if (dot) {
//        *dot = '\0'; // Split at the decimal point
//        sec_num = atoi(sec_str) * 100 + atoi(dot + 1);
//        sec_den = 100;
//    }
//    else {
//        sec_num = atoi(sec_str);
//        sec_den = 1;
//    }

    exif_gps_set_coordinate(coord, deg, 1, min, 1, sec_num, sec_den, ref);
}

/**
 * Function to get GPS coordinates as an ASCII string
 *
 * We get a string like this:
 * 		36°49'55\" S
 * 		174°47'51.8\" E
 *
 * @param coord = pointer to a structure of type GPS_Coordinate
 * @param str - pointer to a buffer to take teh string
 * @param bufLen - length of buffer
 */
void exif_gps_get_coordinate_as_string(GPS_Coordinate *coord, char *str, uint16_t bufLen) {
	uint16_t secondsWhole;
	uint16_t secondsFraction;

	secondsWhole = coord->seconds_num / coord->seconds_den;
	secondsFraction = coord->seconds_num % coord->seconds_den;

	if (coord->seconds_den == 1) {
		snprintf(str, bufLen, "%d°%d'%d\" %c", (int) coord->degrees_num, (int) coord->minutes_num, (int) coord->seconds_num, coord->ref);
	}
	else {
		snprintf(str, bufLen, "%d°%d'%d.%d\" %c", (int) coord->degrees_num, (int) coord->minutes_num,
				(int) secondsWhole, (int) secondsFraction, coord->ref);
	}
}

/**
 * Function to get GPS altitude as an ASCII string
 *
 * We get a string like this:
 * 		31.234 Above
 *
 * @param coord = pointer to a structure of type GPS_Coordinate
 * @param str - pointer to a buffer to take the string
 * @param bufLen - length of buffer
 */
void exif_gps_get_altitude_as_string(GPS_Altitude *alt, char *str, uint16_t bufLen) {
	uint16_t metresWhole;
	uint16_t metresFraction;

	metresWhole = alt->altitude_num / alt->altitude_den;
	metresFraction = alt->altitude_num % alt->altitude_den;

	if (alt->altitude_den == 1) {
		snprintf(str, bufLen, "%dm %s sea level", (int) alt->altitude_num, (alt->ref == 0) ? "above" : "below");
	}
	else {
		snprintf(str, bufLen, "%d.%dm %s sea level", (int) metresWhole, (int) metresFraction, (alt->ref == 0) ? "above" : "below");
	}
}


/**
 * Function to set GPS altitude from an ASCII string
 *
 * (parsing decimal seconds as an integer fraction)
 *
 * @param coord - pointer to a structure of type GPS_Altitude
 * @param str - string containing the altitude string
 */
void exif_gps_set_altitude_from_string(GPS_Altitude *alt, const char *str) {
    char alt_str[16] = {0};
    char ref[10] = {0};
    uint8_t parsed;
    uint8_t ref_value;
    uint32_t alt_num;
    uint32_t alt_den;

    if (!alt || !str) {
    	return;
    }

    parsed = sscanf(str, "%15s %9s", alt_str, ref);
    if (parsed < 2) {
    	return;
    }

    ref_value = (strcmp(ref, "Above") == 0) ? 0 : 1;
//    alt_num = 0;
//    alt_den = 1;

    printf("DEBUG: extract from '%s' the altitude string '%s', (%d)\n", str, alt_str, ref_value);
    setNumDenFromString(&alt_num, &alt_den, alt_str);

    exif_gps_set_altitude(alt, alt_num, alt_den, ref_value);
}

// Function to parse a full GPS string containing latitude, longitude, and altitude
void exif_gps_parse_full_string(GPS_Coordinate *lat, GPS_Coordinate *lon, GPS_Altitude *alt, const char *str) {
    char lat_str[32];
    char lon_str[32];
    char alt_str[32];
    char alt_ref[6];	// "Above" or "Below"
    char altitude[40];
    char lat_ref;
    char lon_ref;

    if (!lat || !lon || !alt || !str) {
    	return;
    }

    // %31s → Reads up to 31 non-whitespace characters and stores them in the buffer, adding a '\0' (null terminator) at the end.
    int parsed = sscanf(str, "%31s %c %31s %c %31s %5s", lat_str, &lat_ref, lon_str, &lon_ref, alt_str, alt_ref);
    if (parsed < 6) {
    	return;
    }

    // concatenate the two parts of the altitude
    snprintf(altitude, sizeof(altitude), "%s %s", alt_str, alt_ref);

    exif_gps_set_coordinate_from_string(lat, lat_str, lat_ref);
    exif_gps_set_coordinate_from_string(lon, lon_str, lon_ref);
    exif_gps_set_altitude_from_string(alt, altitude);
}

// Function to format the GPS coordinate as a string
void exif_gps_format_coordinate(const GPS_Coordinate *coord, char *buffer, size_t buffer_size) {
    if (!coord || !buffer) {
    	return;
    }
    snprintf(buffer, buffer_size, "%u/%u degrees, %u/%u minutes, %u/%u seconds %c",
             (uint) coord->degrees_num, (uint) coord->degrees_den,
			 (uint) coord->minutes_num, (uint) coord->minutes_den,
			 (uint) coord->seconds_num, (uint) coord->seconds_den,
			 (uint) coord->ref);
}

// Function to format the GPS altitude as a string
void exif_gps_format_altitude(const GPS_Altitude *alt, char *buffer, size_t buffer_size) {
    if (!alt || !buffer) {
    	return;
    }
    snprintf(buffer, buffer_size, "%u/%u meters (%s sea level)",
    		(uint) alt->altitude_num, (uint) alt->altitude_den,
             alt->ref == 0 ? "Above" : "Below");
}

// Function to generate the EXIF byte array for GPS data
void exif_gps_generate_byte_array(const GPS_Coordinate *coord, uint8_t *buffer) {
    if (!coord || !buffer) {
    	return;
    }

    // GPSLatitudeRef (ASCII 'N' or 'S')
    buffer[0] = coord->ref;
    buffer[1] = 0x00; // Null terminator

    // GPSLatitude (Big Endian format)
    uint32_t *buf32 = (uint32_t *)(buffer + 2);
    buf32[0] = __builtin_bswap32(coord->degrees_num);
    buf32[1] = __builtin_bswap32(coord->degrees_den);
    buf32[2] = __builtin_bswap32(coord->minutes_num);
    buf32[3] = __builtin_bswap32(coord->minutes_den);
    buf32[4] = __builtin_bswap32(coord->seconds_num);
    buf32[5] = __builtin_bswap32(coord->seconds_den);
}

// Function to generate the EXIF byte array for GPS altitude
void exif_gps_generate_altitude_byte_array(const GPS_Altitude *alt, uint8_t *buffer) {
    if (!alt || !buffer) {
    	return;
    }

    buffer[0] = alt->ref; // Altitude reference (0 = Above, 1 = Below)
    uint32_t *buf32 = (uint32_t *)(buffer + 1);
    buf32[0] = __builtin_bswap32(alt->altitude_num);
    buf32[1] = __builtin_bswap32(alt->altitude_den);
}

/**
 * Test function
 *
 * Takes string inputs and outputs results, including byte buffer for EXIF content.
 * If inputs are NULL then different functions are tested with default values.
 *
 * When a coordinate is set it updates the exif_gps_deviceLat, exif_gps_deviceLon, exif_gps_deviceAlt values
 *
 * Examples of use:
 *
 * exif_gps_test_example_1(NULL, 'N', NULL, 'W', NULL);
 * exif_gps_test_example_1("37°48'30.50\"", 'N', "122°25'10.00\"", 'W', "20 Below");
 *
 * @param latString - ASCII string like 37°48'30.50"
 * @param latRef - character 'N' or 'S'
 * @param longString - ASCII string like 122°25'10.00"
 * @param longRef - character 'E' or 'W'
 * @param altString - ASCII string like "500 Above"
 */
void exif_gps_test_example_1(char * latString, char latRef, char * longString, char longRef, char * altString) {
    uint8_t lat_buffer[26];	// 2 bytes for ref string then 6 x 32-bit words
    uint8_t lon_buffer[26];
    uint8_t alt_buffer[9];	// 1 byte for ref then 2 x 32-bit words
    char coord_str[64];
    char alt_str[40];

    // Used these fixed coordinates if no strings are provided:
    // Example: 37°48'30.50" N, 122°25'10.00" W, Altitude: "500 Above"

    if (latString == NULL) {
        exif_gps_set_coordinate(&exif_gps_deviceLat, 37, 1, 48, 1, 3050, 100, 'N');
    }
    else {
        //exif_gps_set_coordinate_from_string(&lat, "37°48'30.50\"", 'N');
        exif_gps_set_coordinate_from_string(&exif_gps_deviceLat, latString, latRef);
    }

    if (longString == NULL) {
        exif_gps_set_coordinate(&exif_gps_deviceLon, 122, 1, 25, 1, 1000, 100, 'W');
    }
    else {
        //exif_gps_set_coordinate_from_string(&lon, "122°25'10.22\"", 'W');
        exif_gps_set_coordinate_from_string(&exif_gps_deviceLon, longString, longRef);
    }

    if (altString == NULL) {
        exif_gps_set_altitude(&exif_gps_deviceAlt, 500, 1, 0);
    }
    else {
        //exif_gps_set_altitude_from_string(&alt, "500 Above");
        exif_gps_set_altitude_from_string(&exif_gps_deviceAlt, altString);
    }

    exif_gps_format_coordinate(&exif_gps_deviceLat, coord_str, sizeof(coord_str));
    printf("Latitude: %s\n", coord_str);

    exif_gps_format_coordinate(&exif_gps_deviceLon, coord_str, sizeof(coord_str));
    printf("Longitude: %s\n", coord_str);

    exif_gps_format_altitude(&exif_gps_deviceAlt, alt_str, sizeof(alt_str));
    printf("Altitude: %s\n", alt_str);

    exif_gps_generate_byte_array(&exif_gps_deviceLat, lat_buffer);
    exif_gps_generate_byte_array(&exif_gps_deviceLon, lon_buffer);
    exif_gps_generate_altitude_byte_array(&exif_gps_deviceAlt, alt_buffer);

    printf("Latitude Buffer:  ");
    for (size_t i = 0; i < sizeof(lat_buffer); i++) {
        printf("%02X ", lat_buffer[i]);
    }
    printf("\n");

    printf("Longitude Buffer: ");
    for (size_t i = 0; i < sizeof(lon_buffer); i++) {
        printf("%02X ", lon_buffer[i]);
    }
    printf("\n");

    printf("Altitude Buffer:  ");
    for (size_t i = 0; i < sizeof(alt_buffer); i++) {
        printf("%02X ", alt_buffer[i]);
    }
    printf("\n");

}

/**
 * Test function that accepts a full ASCII string
 *
 * If the string is NULL then a default is used.
 *
 * When a coordinate is set it updates the exif_gps_deviceLat, exif_gps_deviceLon, exif_gps_deviceAlt values
 *
 *  * Examples of use:
 *
 * exif_gps_test_example_2(NULL);
 * exif_gps_test_example_2("36°49'55.68\" S 174°47'51.68\" E 31.2 Above");
 *
 * @param GPS string such as:
 */
void exif_gps_test_example_2(char * gps_string) {
    char coord_str[64];
    char alt_str[40];

    if (gps_string == NULL) {
    	const char *gps_input = "37°48'30.50\" N 122°25'10.22\" W 500.75 Above";
    	exif_gps_parse_full_string(&exif_gps_deviceLat, &exif_gps_deviceLon, &exif_gps_deviceAlt, gps_input);
    }
    else {
    	exif_gps_parse_full_string(&exif_gps_deviceLat, &exif_gps_deviceLon, &exif_gps_deviceAlt, gps_string);
    }

    // Now check this is right

    exif_gps_format_coordinate(&exif_gps_deviceLat, coord_str, sizeof(coord_str));
    printf("Latitude: %s\n", coord_str);

    exif_gps_format_coordinate(&exif_gps_deviceLon, coord_str, sizeof(coord_str));
    printf("Longitude: %s\n", coord_str);

    exif_gps_format_altitude(&exif_gps_deviceAlt, alt_str, sizeof(alt_str));
    printf("Altitude: %s\n", alt_str);

}

/**
 * Test function to re-convert coordinates to a string
 */
void exif_gps_test_reconstruct(GPS_Coordinate *lat, GPS_Coordinate *lon, GPS_Altitude *alt) {
	char str[30];

	exif_gps_get_coordinate_as_string(lat, str, sizeof(str));
	printf("*** Device Location: %s ", str);

	exif_gps_get_coordinate_as_string(lon, str, sizeof(str));
	printf("%s ", str);

	exif_gps_get_altitude_as_string(alt, str, sizeof(str));
	printf("%s\n", str);
}
