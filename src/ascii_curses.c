#include <stdio.h>
#include <string.h>

#include "../include/ascii_curses.h"

void usage(void)
{
	printf("usage: ascii_curses <file_path.png> <blocksize>\n");
}

int main(int argc, char **argv)
{
	bitmap_t *bmp;
	int block_size;

	if (argc != 3 || sscanf(argv[2], "%d", &block_size) != 1) {
		usage();
		return 1;
	}

	if ((bmp = read_png_image(argv[1])) == NULL) {
		fprintf(stderr, "%s: Failed to read %s\n", argv[0], argv[1]);
		return 1;
	}

	print_ascii(bmp, block_size);

	return 0;
}
