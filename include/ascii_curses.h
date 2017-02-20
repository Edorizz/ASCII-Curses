#ifndef ASCII_CURSES_H
#define ASCII_CURSES_H

#include <png.h>

typedef unsigned char BYTE;

/* Single pixel */
typedef struct {
	BYTE r, g, b, a;
} pixelRGBA_t;

typedef struct {
	BYTE r, g, b;
} pixelRGB_t;

/* Image data */
typedef struct {
	BYTE *pixel_data;
	int width, height;
	int color_type;
	int pixel_size;
} bitmap_t;

bitmap_t *read_png_image(const char *file_name);

#endif /* ASCII_CURSES_H */
