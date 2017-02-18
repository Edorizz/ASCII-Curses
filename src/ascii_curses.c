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

void print_ascii(bitmap_t *bmp)
{
	const char *greyscale_chars = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
	pixel_t *pixel;
	int i, j, c;
	float scale;

	scale = 255.0f / (float)strlen(greyscale_chars);
	for (i = 0; i < bmp->height; i += BLOCK_SIZE) {
		for (j = 0; j < bmp->width; j += BLOCK_SIZE) {
			pixel = &bmp->pixel_data[i * bmp->width + j];
			c = greyscale_chars[(int)(grayscale_value(pixel) / scale)];

			if (pixel->a == 0)
				printf("  ");
			else
				printf("%c%c", c, c);
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
		fprintf("%s: Failed to read %d\n", argv[0], argv[1]);
		return 1;
	}

	print_ascii(bmp);

	return 0;
}
