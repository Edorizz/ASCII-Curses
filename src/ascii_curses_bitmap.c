#include "../include/ascii_curses.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

BYTE *pixel_at(bitmap_t *bmp, int y, int x)
{
	return &bmp->pixel_data[bmp->pixel_size * (bmp->width * y + x)];
}

BYTE color_index(pixelRGB_t *color)
{
	int dist_rg, dist_gb, dist_rb;

	dist_rg = ABS(color->r - color->g);
	dist_gb = ABS(color->g - color->b);
	dist_rb = ABS(color->r - color->b);

	if (dist_rg + dist_gb + dist_rb < 100)
		return 0;
	else
		return rand() % 6 + 1;
}

BYTE grayscale_value(BYTE *pixel, int color_type, int background)
{
	pixelRGBA_t *pRGBA;
	int grayscale;

	pRGBA = (pixelRGBA_t*)pixel;
	grayscale = 0.21f * pRGBA->r + 0.72f * pRGBA->g + 0.07 * pRGBA->b;

	if (background & ASCII_BACKGROUND_DARK)
		grayscale = 255 - grayscale;

	if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		grayscale += (255 - grayscale) * ((255 - pRGBA->a) / 255);

	return grayscale;
}

BYTE avg_grayscale(ascii_image_t *img, int y, int x, int count)
{
	int avg_grayscale, i;

	avg_grayscale = 0;
	for (i = 0; x + i < img->bmp->width && i < count; ++i)
		avg_grayscale += grayscale_value(pixel_at(img->bmp, y, x + i), img->bmp->color_type, img->flags);

	return avg_grayscale / i;
}

BYTE avg_grayscale_block(ascii_image_t *img, int y, int x)
{
	int grayscale, i;

	grayscale = 0;
	for (i = 0; y + i < img->bmp->height && i < img->block_size; ++i)
		grayscale += avg_grayscale(img, y + i, x, img->block_size / 2);

	return grayscale / i;
}

void avg_color(ascii_image_t *img, int y, int x, int count, pixelRGB_t *color)
{
	int r, g, b, i;
	pixelRGB_t *pixel;

	r = g = b = 0;
	for (i = 0; x + i < img->bmp->width && i < count; ++i) {
		pixel = (pixelRGB_t*)pixel_at(img->bmp, y, x + i);

		r += pixel->r;
		g += pixel->g;
		b += pixel->b;
	}

	color->r = r / i;
	color->g = g / i;
	color->b = b / i;
}

void avg_color_block(ascii_image_t *img, int y, int x, pixelRGB_t *color)
{
	int r, g, b, i;

	r = g = b = 0;
	for (i = 0; y + i < img->bmp->height && i < img->block_size; ++i) {
		avg_color(img, y + i, x, img->block_size / 2, color);

		r += color->r;
		g += color->g;
		b += color->b;
	}

	color->r = r / i;
	color->g = g / i;
	color->b = b / i;
}

void to_ascii(ascii_image_t *img)
{
	const char *grayscale_chars = "@%#*+=-:. ";
	const float scale = 256.0f / (float)strlen(grayscale_chars);

	int i, j, index, ch;
	pixelRGB_t color = { 69, 69, 69 };

	if (img->ascii_data != NULL)
		free(img->ascii_data);

	img->width = img->bmp->width * 2 / img->block_size;
	img->height = img->bmp->height / img->block_size;

	if (img->width * img->block_size < img->bmp->width * 2)
		++img->width;
	if (img->height * img->block_size < img->bmp->height)
		++img->height;

	img->ascii_data = malloc(img->width * img->height * sizeof(*img->ascii_data));

	/* Image data */
	for (i = 0; i < img->bmp->height; i += img->block_size) {
		for (j = 0; j < img->bmp->width; j += img->block_size / 2) {
			/* Get necessary data to form an ASCII pixel */
			ch = grayscale_chars[(int)(avg_grayscale_block(img, i, j) / scale)];
			avg_color_block(img, i, j, &color);

			/* Calculate index */
			index = (i / img->block_size) * img->width + (j * 2 / img->block_size);

			/* Create ASCII pixel */
			img->ascii_data[index].ch = ch;

			img->ascii_data[index].color.r = color.r;
			img->ascii_data[index].color.g = color.g;
			img->ascii_data[index].color.b = color.b;
		}
	}
}

void print_ascii(ascii_image_t *img)
{
	int i, j, color;
	pixelASCII_t *pixel;

	/* Why do we stop at 'LINES - 1' in y but just at 'COLS' in x you may ask?
	 * well, we have to take into account the status bar at the bottom for y
	 * but not for x.
	 */
	for (i = 0; i + img->pos_y < LINES - 1 && i < img->height; ++i) {
		for (j = 0; j + img->pos_x < COLS && j < img->width; ++j) {
			pixel = &img->ascii_data[i * img->width + j];
			color = color_index(&pixel->color);

			attron(COLOR_PAIR(color));
			mvprintw(i + img->pos_y, j + img->pos_x, "%c", pixel->ch);
			attroff(COLOR_PAIR(color));
		}
	}
}

