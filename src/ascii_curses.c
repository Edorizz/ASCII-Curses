#include <stdio.h>
#include <string.h>

#include "../include/ascii_curses.h"

#define BLOCK_SIZE	8

void usage(void)
{
	printf("usage: ascii_curses <file_path.png>\n");
}

BYTE grayscale_value(pixel_t *pixel)
{
	return (pixel->r + pixel->g + pixel->b) / 3;
}

BYTE avg_grayscale(bitmap_t *bmp, int y, int x, int count)
{
	int avg_grayscale, i;

	avg_grayscale = 0;
	for (i = 0; i < count && x + i < bmp->width; ++i)
		avg_grayscale += grayscale_value(&bmp->pixel_data[y * bmp->width + x + i]);

	return avg_grayscale / i;
}

BYTE avg_grayscale_block(bitmap_t *bmp, int y, int x)
{
	int i, grayscale, val;

/*	printf("%3d,%3d|", x, y);*/
	for (i = grayscale = 0; i < BLOCK_SIZE; ++i)
		grayscale += avg_grayscale(bmp, y + i, x, BLOCK_SIZE);

	return grayscale / BLOCK_SIZE;
}

void print_ascii(bitmap_t *bmp)
{
	const char *grayscale_chars = "@%#*+=-:. ";
	pixel_t *pixel;
	int i, j, ch;
	float scale;

	scale = 255.0f / (float)strlen(grayscale_chars);
	for (i = 0; i < bmp->height; i += BLOCK_SIZE) {
		for (j = 0; j < bmp->width; j += BLOCK_SIZE) {
			pixel = &bmp->pixel_data[i * bmp->width + j];
			ch = grayscale_chars[(int)(avg_grayscale_block(bmp, i, j) / scale)];
			
			if (pixel->a == 0)
				printf("  ");
			else
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
