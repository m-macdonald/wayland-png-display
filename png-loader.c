#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <png.h>
#include "png-loader.h"

img load_png(char* file_name) {
    printf("loading");
    printf("%s", file_name);
    FILE* fp = fopen(file_name, "rb");
    if (fp == NULL) {
        printf("failed to open file");
        perror("reason: ");

        exit(1);
    }
    printf("file opened");
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) abort();
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) abort();
    png_init_io(png_ptr, fp);
    // png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_read_info(png_ptr, info_ptr);

    printf("info read");
    img img = {
        .height = png_get_image_height(png_ptr, info_ptr),
        .width = png_get_image_width(png_ptr, info_ptr),
    };

    printf("height: %i\n", img.height);
    printf("width: %i\n", img.width);
    
    printf("image");
    png_set_palette_to_rgb(png_ptr);
    png_read_update_info(png_ptr, info_ptr);
    img.row_pointers = malloc(sizeof(png_bytep) * img.height);
    unsigned int i;
    for (i = 0; i < img.height; i++) {
        png_read_row(png_ptr, &img.row_pointers[i], NULL);
    }

    printf("pointers loaded");
    // png_read_image(png_ptr, img.row_pointers);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    printf("returning");
    unsigned int j;
    for(i = 0; i < img.height; i++) {
        for(j = 0; j < img.width; j++) {
            printf("%i", img.row_pointers[(i * img.width) + j]);
        }
    }

    return img;
}

//
// img load_png(char* file_name) {
//     printf("loading");
//
//     png_image image;
//
//     memset(&image, 0, sizeof image);
//     image.version = PNG_IMAGE_VERSION;
//
//     if (png_image_begin_read_from_file(&image, file_name)) {
//         png_bytep buffer;
//
//         image.format = PNG_FORMAT_RGBA;
//
//         buffer = malloc(PNG_IMAGE_SIZE(image));
//
//         if (buffer != NULL) {
//             if (png_image_finish_read(&image, NULL, buffer, 0, NULL)) {
//                 img img = {
//                     .width = image.width,
//                     .height = image.height,
//                     .row_pointers = buffer
//                 };
//                 unsigned int i;
//                 unsigned int j;
//                 for(i = 0; i < img.height; i++) {
//                     for (j = 0; j < img.width; j++)
//                         printf("%i", img.row_pointers[i * image.width]);
//                 }
//
//                 return img;
//             }
//         } else {
//             fprintf(stderr, "%s", image.message);
//
//             png_image_free(&image);
//         }
//     }
//
//     exit(1);
// }

// img load_png(char* file_name) {
//     printf("loading");
//     printf("%s", file_name);
//     FILE* fp = fopen(file_name, "rb");
//     if (fp == NULL) {
//         printf("failed to open file");
//         perror("reason: ");
//
//         exit(1);
//     }
//     printf("file opened");
//     png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//     if (!png_ptr) abort();
//     png_infop info_ptr = png_create_info_struct(png_ptr);
//     if (!info_ptr) abort();
//     png_init_io(png_ptr, fp);
//
//     png_read_info(png_ptr, info_ptr);
//
//     png_bytep row = png_malloc(png_ptr, png_get_rowbytes(png_ptr, info_ptr));
//
//     png_uint_32 width, height;
//     int bit_depth, color_type, interlace_method, compression_method, filter_method;
//     png_bytep row_tmp;
//
//     row_tmp = row;
//     if (png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, &compression_method, &filter_method)) {
//         int passes, pass;
//
//
//         switch (interlace_method) {
//             case PNG_INTERLACE_NONE:
//                 passes = 1;
//                 break;
//             case PNG_INTERLACE_ADAM7:
//                 passes = PNG_INTERLACE_ADAM7_PASSES;
//                 break;
//         }
//
//         png_start_read_image(png_ptr);
//
//         img img = {
//             .height = png_get_image_height(png_ptr, info_ptr),
//             .width = png_get_image_width(png_ptr, info_ptr),
//         };
//
//         img.row_pointers = malloc(sizeof(png_bytep) * img.height);
//
//         for (pass = 0; pass < passes; ++pass) {
//             png_uint_32 ystart, xstart, ystep, xstep;
//             png_uint_32 py;
//
//             if (interlace_method == PNG_INTERLACE_ADAM7) {
//                 if (PNG_PASS_COLS(width, pass) == 0)
//                     continue;
//
//                 xstart = PNG_PASS_START_COL(pass);
//                 ystart = PNG_PASS_START_ROW(pass);
//                 xstep = PNG_PASS_COL_OFFSET(pass);
//                 ystep = PNG_PASS_ROW_OFFSET(pass);
//         } else {
//             ystart = xstart = 0;
//             ystep = xstep = 0;
//         }
//
//         for (py = ystart; py < height; py += ystep) {
//             png_uint_32 px, ppx;
//
//             png_read_row(png_ptr, row_tmp, NULL);
//             img.row_pointers =  
//         }
//     }
//
//
//
//
//
//
//
//
//     // png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
//     png_read_info(png_ptr, info_ptr);
//
//     printf("info read");
//     img img = {
//         .height = png_get_image_height(png_ptr, info_ptr),
//         .width = png_get_image_width(png_ptr, info_ptr),
//     };
//
//     printf("height: %i\n", img.height);
//     printf("width: %i\n", img.width);
//     
//     printf("image");
//     png_set_palette_to_rgb(png_ptr);
//     png_read_update_info(png_ptr, info_ptr);
//     img.row_pointers = malloc(sizeof(unsigned char*) * img.height);
//     unsigned int i;
//     for (i = 0; i < img.height; i++) {
//         img.row_pointers[i] = malloc(png_get_rowbytes(png_ptr, info_ptr));
//     }
//
//     printf("pointers loaded");
//     png_read_image(png_ptr, &img.row_pointers);
//     png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
//     fclose(fp);
//     printf("returning");
//
//     unsigned int j;
//     for(i = 0; i < img.height; i++) {
//         for(j = 0; j < img.width; j++) {
//             printf("%i", img.row_pointers[(i * img.width) + j]);
//         }
//     }
//
//     return img;
// }
