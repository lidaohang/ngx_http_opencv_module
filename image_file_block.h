#pragma once

#ifndef IMAGE_FILE_BLOCK_H
#define IMAGE_FILE_BLOCK_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>




typedef unsigned char u_char;


typedef struct {
        size_t width;
		size_t height;
		size_t quality;
		size_t code;
		size_t watermark;
		char* path;
		u_char* data;
		size_t len;
} image_file_t;

typedef struct {
} empty_struct;

#endif /* IMAGE_FILE_BLOCK_H */
