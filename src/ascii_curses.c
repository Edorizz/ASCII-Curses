#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../include/ascii_curses.h"

#define MULTIPLIER	2.0f
#define MIN(a, b)	((a) > (b) ? (b) : (a))
#define MAX(a, b)	((a) > (b) ? (a) : (b))

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
	int ch, i, image_index = 0, mv;

	/* Process command-line arguments */
	for (i = 1; i != argc; ++i) {
		if (strcmp("-d", argv[i]) == 0)
			img.flags |= ASCII_BACKGROUND_DARK;
		else if (image_index == 0)
			image_index = i;
	}

	/* Exit if image failed to load or user did not input file path */
	if (image_index == 0 || (img.bmp = read_png_image(argv[image_index])) == NULL) {
		usage();
		return 1;
	}

	initcurses();

	img.block_size = 8;
	img.flags |= ASCII_DRAW | ASCII_CONVERT;

	/* Main message loop */
	while ((ch = getch()) != KEY_F(1)) {
		switch (tolower(ch)) {
			case 'a':	img.pos_x += mv;	img.flags |= ASCII_DRAW;	break;
			case 'd':	img.pos_x -= mv;	img.flags |= ASCII_DRAW;	break;
			case 'w':	img.pos_y += mv;	img.flags |= ASCII_DRAW;	break;
			case 's':	img.pos_y -= mv;	img.flags |= ASCII_DRAW;	break;
			case 'q':
					if (img.block_size < img.bmp->width) {
						img.pos_x = ((float)img.pos_x / img.block_size) * (img.block_size - 1);
						img.pos_y = ((float)img.pos_y / img.block_size) * (img.block_size - 1);

						++img.block_size;
						img.flags |= ASCII_DRAW | ASCII_CONVERT;
					}

					break;
			case 'e':
					if (img.block_size > 1) {
						img.pos_x = ((float)img.pos_x / img.block_size) * (img.block_size + 1);
						img.pos_y = ((float)img.pos_y / img.block_size) * (img.block_size + 1);

						--img.block_size;
						img.flags |= ASCII_DRAW | ASCII_CONVERT;
					}

					break;
		}

		if (img.flags & ASCII_CONVERT) {
			to_ascii(&img);
			mv = MAX(1.0f / ((float)LINES / (float)img.height), 1.0f);

			img.flags ^= ASCII_CONVERT;
		}

		if (img.flags & ASCII_DRAW) {
			clear();
			print_ascii(&img);
			mvprintw(0, 0, "(mv:%d)", mv);
			refresh();

			img.flags ^= ASCII_DRAW;
		}

	}

	endwin();
	return 0;
}
