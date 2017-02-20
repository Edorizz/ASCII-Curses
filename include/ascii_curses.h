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

/* Read PNG image and store it in a bitmap_t structure */
bitmap_t *read_png_image(const char *file_name);

/* Return pointer to pixel at location (y, x) */
BYTE *pixel_at(bitmap_t *bmp, int y, int x);

/* Calculate grayscale value out a pixel taking into account its alpha, if present */
BYTE grayscale_value(BYTE *pixel, int color_type);
/* Average the grayscale value of all pixels in a row of size count starting from (y, x) */
BYTE avg_grayscale(bitmap_t *bmp, int y, int x, int count);
/* Average the grayscale value of all pixels in a block of size * size starting from (y, x) */
BYTE avg_grayscale_block(bitmap_t *bmp, int y, int x, int size);

/* Print ascii equivalent of the specified image */
void print_ascii(bitmap_t *bmp, int block_size);

#endif /* ASCII_CURSES_H */
