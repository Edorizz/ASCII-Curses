#include <stdio.h>
#include "../include/ascii_curses.h"

#define BLOCK_SIZE	8

void usage(void)
{
	printf("usage: ascii_curses <file_path.png>\n");
}

void print_ascii(bitmap_t *bmp)
{
	pixel_t *pixel;
	int i, j;

	for (i = 0; i < bmp->height; i += BLOCK_SIZE) {
		for (j = 0; j < bmp->width; j += BLOCK_SIZE) {
			pixel = &bmp->pixel_data[i * bmp->width + j];
			if (pixel->a == 0)
				printf("  ");
			else if (pixel->r == 0 && pixel->g == 0 && pixel->b == 0)
				printf("++");
			else
				printf("##");
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

	bmp = read_png_image(argv[1]);
	print_ascii(bmp);

	return 0;
}
