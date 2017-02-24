#include "../include/ascii_curses.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

	for (i = grayscale = 0; y + i < bmp->height && i < block_size; ++i)
		grayscale += avg_grayscale(bmp, y + i, x, block_size);

	return grayscale / i;
}

void to_ascii(ascii_image_t *img)
{
	const char *grayscale_chars = "@%#*+=-:. ";
	/*static const char *grayscale_chars = " .:-=+*#$@";*/
	const float scale = 256.0f / (float)strlen(grayscale_chars);

	int i, j, ch;

	if (img->ascii_data != NULL)
		free(img->ascii_data);

	img->width = img->bmp->width / img->block_size;
	img->height = img->bmp->height / img->block_size;

	if (img->width * img->block_size < img->bmp->width)
		++img->width;
	if (img->height * img->block_size < img->bmp->height)
		++img->height;

	img->ascii_data = malloc(img->width * img->height);
	/*mvprintw(1, 0, "%d, %d", img->width, img->height);*/

	/* Image data */
	for (i = 0; i < img->bmp->height; i += img->block_size) {
		/*mvprintw(0, 0, "%d", tmp++);
		refresh();
		sleep(1);*/
		for (j = 0; j < img->bmp->width; j += img->block_size) {
			ch = grayscale_chars[(int)(avg_grayscale_block(img->bmp, i, j, img->block_size) / scale)];

			img->ascii_data[(i / img->block_size) * img->width + (j / img->block_size)] = ch;
		}
	}
}

void print_ascii(ascii_image_t *img, int y, int x)
{
	int i, j, ch;

	for (i = 0; i < img->height; ++i) {
		for (j = 0; j < img->width; ++j) {
			ch = img->ascii_data[i * img->width + j];

			mvprintw(i + y, j * 2 + x, "%c%c", ch, ch);
		}
	}
}

