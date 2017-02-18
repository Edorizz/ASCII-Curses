#ifndef ASCII_CURSES_H
#define ASCII_CURSES_H

typedef unsigned char BYTE;

/* Single pixel */
typedef struct {
	BYTE r, g, b;
} pixel_t;

/* Image data */
typedef struct {
	pixel_t *pixel_data;
	int width, height;
} bitmap_t;

bitmap_t *read_png_image(const char *file_name);

#endif /* ASCII_CURSES_H */
