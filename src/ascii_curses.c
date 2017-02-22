#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../include/ascii_curses.h"

void usage(void)
{
	printf("usage: ascii_curses <file_path.png> <blocksize>\n");
}

void initcurses(void)
{
	initscr();
	timeout(0);
	curs_set(0);
	cbreak();
	noecho();
}

int main(int argc, char **argv)
{
	bitmap_t *bmp;
	int block_size, x, y, ch, draw;

	if (argc != 3 || sscanf(argv[2], "%d", &block_size) != 1) {
		usage();
		return 1;
	}

	if ((bmp = read_png_image(argv[1])) == NULL) {
		fprintf(stderr, "%s: Failed to read %s\n", argv[0], argv[1]);
		return 1;
	}

	initcurses();

	draw = 1;
	x = y = 0;
	while (tolower(ch = getch()) != 'q') {
		switch (ch) {
			case 'a': case 'A':	++x;	draw = 1;	break;
			case 'd': case 'D':	--x;	draw = 1;	break;
			case 'w': case 'W':	++y;	draw = 1;	break;
			case 's': case 'S':	--y;	draw = 1;	break;	
		}

		if (draw) {
			clear();
			print_ascii(bmp, y, x, block_size);
			refresh();

			draw = 0;
		}

	}

	endwin();
	return 0;
}
