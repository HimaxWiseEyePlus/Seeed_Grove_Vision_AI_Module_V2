/**
 * metadata.h
 * created on: 24.12.2024
 * author: 	TBP
 *
 * @brief Header file for metadata structures and functions
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#define MAX_METADATA_SIZE 512

#ifndef METADATA_H
#define METADATA_H

typedef struct
{
    char mediaID[20];
    char deploymentID[20];
    char captureMethod[50];
    double latitude;
    double longitude;
    char timestamp[20];
    bool favourite;
    // Add more fields as needed
} ImageMetadata;

int createAPP1Block(ImageMetadata *metadata, unsigned char *buffer, int bufferSize);
void printEXIF(size_t app1Size, char *app1Block);

#endif // METADATA_H