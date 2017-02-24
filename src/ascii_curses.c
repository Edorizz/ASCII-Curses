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
	keypad(stdscr, TRUE);
	noecho();
}

int main(int argc, char **argv)
{
	ascii_image_t img = { 0 };
	int x, y, ch, draw, convert;

	if (argc != 3 || sscanf(argv[2], "%d", &img.block_size) != 1) {
		usage();
		return 1;
	}

	if ((img.bmp = read_png_image(argv[1])) == NULL) {
		fprintf(stderr, "%s: Failed to read %s\n", argv[0], argv[1]);
		return 1;
	}

	initcurses();

	x = y = 0;
	draw = convert = 1;
	while (tolower(ch = getch()) != KEY_F(1)) {
		switch (tolower(ch)) {
			case 'a':	++x;	draw = 1;	break;
			case 'd':	--x;	draw = 1;	break;
			case 'w':	++y;	draw = 1;	break;
			case 's':	--y;	draw = 1;	break;
			case 'q':
					if (img.block_size < img.bmp->width)
						++img.block_size;
					convert = draw = 1;
					break;
			case 'e':
					if (img.block_size > 1)
						--img.block_size;
					convert = draw = 1;
					break;
		}

		if (convert)
			to_ascii(&img);

		if (draw) {
			clear();
			print_ascii(&img, y, x);
			refresh();

			draw = 0;
		}

	}

	endwin();
	return 0;
}
