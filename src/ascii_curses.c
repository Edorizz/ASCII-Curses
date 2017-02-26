#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/ascii_curses.h"

void usage(void)
{
	printf("usage: ascii_curses [-d] <file_path.png>\n");
}

void initcolor(void)
{
	int i;

	if (has_colors() == FALSE) {
		endwin();
		fprintf(stderr, "Your terminal doesn't support color >:(\n");
		exit(1);
	}
		
	use_default_colors();
	start_color();

	for (i = 1; i != ASCII_COLOR_MAX; ++i)
		init_pair(i, COLOR_BLACK + i, -1);
}

void initcurses(void)
{
	initscr();

	initcolor();
	timeout(-1);
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
		if (strcmp("-d", argv[i]) == 0) {
			img.flags |= ASCII_BACKGROUND_DARK;
		} else if (image_index == 0) {
			image_index = i;
		} else {
			fprintf(stderr, "ascii_curses: invalid argument '%s'\n", argv[i]);
			usage();
			return 1;
		}
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
						img.pos_x = ((float)img.pos_x / img.block_size) * MAX((img.block_size - 1), 1);
						img.pos_y = ((float)img.pos_y / img.block_size) * MAX((img.block_size - 1), 1);

						++img.block_size;
						img.flags |= ASCII_DRAW | ASCII_CONVERT;
					}

					break;
			case 'e':
					if (img.block_size > 2) {
						img.pos_x = ((float)img.pos_x / img.block_size) * (img.block_size + 1);
						img.pos_y = ((float)img.pos_y / img.block_size) * (img.block_size + 1);

						--img.block_size;
						img.flags |= ASCII_DRAW | ASCII_CONVERT;
					}

					break;
		}

		/* Only convert if necessary */
		if (img.flags & ASCII_CONVERT) {
			to_ascii(&img);
			mv = MAX(1.0f / ((float)LINES / (float)img.height), 1.0f);

			img.flags ^= ASCII_CONVERT;
		}

		/* Only draw if necessary */
		if (img.flags & ASCII_DRAW) {
			clear();

			/* Print image */
			print_ascii(&img);

			/* Status bar */
			mvprintw(LINES - 1, 0, "%s (%d x %d):(%d x %d) at:(%d, %d) block_size:%d(%d%%)\n", argv[image_index],
					img.bmp->width, img.bmp->height,
					img.width, img.height,
					-img.pos_x, -img.pos_y,
					img.block_size, img.width * 100 / img.bmp->width);
			mvchgat(LINES -1, 0, -1, A_REVERSE, 0, NULL);

			refresh();

			img.flags ^= ASCII_DRAW;
		}

	}

	endwin();
	return 0;
}
