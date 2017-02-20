#include <stdio.h>
#include <string.h>

#include "../include/ascii_curses.h"

#define BLOCK_SIZE	8

void usage(void)
{
	printf("usage: ascii_curses <file_path.png>\n");
}

int pixel_size(bitmap_t *bmp)
{
	switch (bmp->color_type) {
		case PNG_COLOR_TYPE_RGB:	return 3;
		case PNG_COLOR_TYPE_RGB_ALPHA:	return 4;
	}

	return 0;
}

BYTE grayscale_value(BYTE *pixel, int color_type)
{
	pixelRGB_t *pRGB;
	pixelRGBA_t *pRGBA;
	int grayscale;

	switch (color_type) {
		case PNG_COLOR_TYPE_RGB:
			pRGB = (pixelRGB_t*) pixel;
			grayscale = (pRGB->r + pRGB->g + pRGB->b) / 3;

			return grayscale;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			pRGBA = (pixelRGBA_t*) pixel;
			grayscale = (pRGBA->r + pRGBA->g + pRGBA->b) / 3;

			return grayscale + (255 - grayscale) * ((255 - pRGBA->a) / 255);
	}

	fprintf(stderr, "Colortype %d not supported\n", color_type);
	return 255;
}

BYTE *pixel_at(bitmap_t *bmp, int y, int x)
{
	return &bmp->pixel_data[pixel_size(bmp) * (bmp->width * y + x)];
}

BYTE avg_grayscale(bitmap_t *bmp, int y, int x, int count)
{
	int avg_grayscale, i;

	avg_grayscale = 0;
	for (i = 0; i < count && x + i < bmp->width; ++i)
		avg_grayscale += grayscale_value(pixel_at(bmp, y, x + i), bmp->color_type);

	return avg_grayscale / i;
}

BYTE avg_grayscale_block(bitmap_t *bmp, int y, int x)
{
	int i, grayscale;

	for (i = grayscale = 0; i < BLOCK_SIZE; ++i)
		grayscale += avg_grayscale(bmp, y + i, x, BLOCK_SIZE);

	return grayscale / BLOCK_SIZE;
}

void print_ascii(bitmap_t *bmp)
{
	const char *grayscale_chars = "@%#*+=-:. ";
	int i, j, ch;
	float scale;

	scale = 256.0f / (float)strlen(grayscale_chars);
	for (i = 0; i < bmp->height; i += BLOCK_SIZE) {
		for (j = 0; j < bmp->width; j += BLOCK_SIZE) {
			ch = grayscale_chars[(int)(avg_grayscale_block(bmp, i, j) / scale)];

			printf("%c%c", ch, ch);
		}
		putchar('\n');
	}

}

int main(int argc, char **argv)
{
	bitmap_t *bmp;

	if (argc != 2) {
		usage();
		return 1;
	}

	if ((bmp = read_png_image(argv[1])) == NULL) {
		fprintf(stderr, "%s: Failed to read %s\n", argv[0], argv[1]);
		return 1;
	}

	print_ascii(bmp);

	return 0;
}
