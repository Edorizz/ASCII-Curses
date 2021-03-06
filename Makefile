CC=gcc
CFLAGS=-c -std=c99 -pedantic -Wall
LFLAGS=-lpng -lncurses

ascii_curses: output/ascii_curses.o output/ascii_curses_image.o output/ascii_curses_bitmap.o
	$(CC) output/ascii_curses.o output/ascii_curses_image.o output/ascii_curses_bitmap.o -o ascii_curses $(LFLAGS)

output/ascii_curses.o: src/ascii_curses.c include/ascii_curses.h
	$(CC) $(CFLAGS) src/ascii_curses.c -o output/ascii_curses.o

output/ascii_curses_image.o: src/ascii_curses_image.c include/ascii_curses.h
	$(CC) $(CFLAGS) src/ascii_curses_image.c -o output/ascii_curses_image.o

output/ascii_curses_bitmap.o: src/ascii_curses_bitmap.c include/ascii_curses.h
	$(CC) $(CFLAGS) src/ascii_curses_bitmap.c -o output/ascii_curses_bitmap.o

clean:
	rm output/*.o ascii_curses
