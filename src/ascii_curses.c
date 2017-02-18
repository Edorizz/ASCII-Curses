#include <stdio.h>

#include "../include/ascii_curses.h"

void usage(void)
{
	printf("usage: ascii_curses <file_path.png>\n");
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		usage();
		return 1;
	}

	read_png_image(argv[1]);

	return 0;
}
