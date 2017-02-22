#include "../include/ascii_curses.h"

#include <string.h>

BYTE grayscale_value(BYTE *pixel, int color_type)
{
	pixelRGBA_t *pRGBA;
	int grayscale;

	pRGBA = (pixelRGBA_t*)pixel;
	grayscale = 0.21f * pRGBA->r + 0.72f * pRGBA->g + 0.07 * pRGBA->b;

	if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		grayscale += (255 - grayscale) * ((255 - pRGBA->a) / 255);

	return grayscale;
}

BYTE *pixel_at(bitmap_t *bmp, int y, int x)
{
	return &bmp->pixel_data[bmp->pixel_size * (bmp->width * y + x)];
}

BYTE avg_grayscale(bitmap_t *bmp, int y, int x, int count)
{
	int avg_grayscale, i;

	avg_grayscale = 0;
	for (i = 0; x + i < bmp->width && i < count; ++i)
		avg_grayscale += grayscale_value(pixel_at(bmp, y, x + i), bmp->color_type);

	return avg_grayscale / i;
}

BYTE avg_grayscale_block(bitmap_t *bmp, int y, int x, int block_size)
{
	int i, grayscale;

	for (i = grayscale = 0; i < bmp->height - y && i < block_size; ++i)
		grayscale += avg_grayscale(bmp, y + i, x, block_size);

	return grayscale / i;
}

void print_ascii(bitmap_t *bmp, int y, int x, int block_size)
{
	const char *grayscale_chars = "@%#*+=-:. ";
	int i, j, ch;
	float scale;

	scale = 256.0f / (float)strlen(grayscale_chars);
	for (i = 0; i < bmp->height; i += block_size) {
		for (j = 0; j < bmp->width; j += block_size) {
			ch = grayscale_chars[(int)(avg_grayscale_block(bmp, i, j, block_size) / scale)];

			mvprintw(i / block_size + y, (j / block_size + x) * 2, "%c%c", ch, ch);
		}
		putchar('\n');
	}

}

