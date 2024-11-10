// /* exif_handler.c
//  *
//  * Author: TP
//  * Date: 06/11/2024
//  *
//  * This file creates the EXIF handlers, set tags, save and destroy an EXIFHandler object.
//  */

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <libexif/exif-data.h>
// #include "exif_handler.h"

// /* Create a new EXIFHandler
//  * Returns: handler - a pointer to the new EXIFHandler
//  */
// EXIFHandler *media_exif_handler_create()
// {
//     EXIFHandler *handler;
//     handler->exif_data = exif_data_new();
//     return handler;
// }

// /* Create a new EXIFHandler
//  * Returns: handler - a pointer to the new EXIFHandler
//  */
// EXIFHandler *observations_exif_handler_create()
// {
//     EXIFHandler *handler;
//     handler->exif_data = exif_data_new();
//     return handler;
// }

// /* Create a new EXIFHandler
//  * Returns: handler - a pointer to the new EXIFHandler
//  */
// EXIFHandler *delpoyment_exif_handler_create()
// {
//     EXIFHandler *handler;
//     handler->exif_data = exif_data_new();
//     return handler;
// }

// /* Set a tag value in EXIF data
//  * Parameters: handler - the EXIFHandler object
//  *             tag - the tag to set
//  *             value - the value to set
//  * Returns: void
//  */
// void exif_handler_set_tag(EXIFHandler *handler, ExifTag tag, const char *value)
// {
//     if (!handler || !handler->exif_data)
//         return;

//     ExifEntry *entry = exif_content_get_entry(handler->exif_data->ifd[EXIF_IFD_0], tag);
//     if (!entry)
//     {
//         entry = exif_entry_new();
//         exif_content_add_entry(handler->exif_data->ifd[EXIF_IFD_0], entry);
//         exif_entry_initialize(entry, tag);
//         exif_entry_unref(entry);
//     }
//     exif_entry_set_value(entry, value);
// }

// // Save EXIF data to a file
// // TODO: Implement in fatfs task instead
// void exif_handler_save(EXIFHandler *handler, const char *file_path)
// {
//     if (!handler || !handler->exif_data)
//         return;

//     unsigned char *data;
//     unsigned int size;
//     exif_data_save_data(handler->exif_data, &data, &size);

//     FILE *file = fopen(file_path, "wb");
//     if (file)
//     {
//         fwrite(data, 1, size, file);
//         fclose(file);
//     }
//     free(data);
// }

// // Destroy the EXIFHandler and free resources
// void exif_handler_destroy(EXIFHandler *handler)
// {
//     if (handler)
//     {
//         if (handler->exif_data)
//             exif_data_unref(handler->exif_data);
//         free(handler);
//     }
// }