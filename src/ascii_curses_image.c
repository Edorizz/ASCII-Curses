#include "../include/ascii_curses.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <png.h>

bitmap_t *read_png_image(const char *file_path)
{
	char header[8];
	int width, height, color_type, bit_depth, number_of_passes, i;
	FILE *fp;

	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *row_pointers;

	/* Open file */
	if ((fp = fopen(file_path, "rb")) == NULL) {
		fprintf(stderr, "Failed to open %s: %s\n",
				file_path, strerror(errno));
		return NULL;
	}

	/* Check for PNG header signature */
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8)) {
		fprintf(stderr, "%s is not a PNG file\n",
				file_path);
		return NULL;
	}

	/* Create necessary libpng structs */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
	if (!png_ptr) {
		fprintf(stderr, "png_create_read_struct failed");
		return NULL;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		fprintf(stderr, "png_create_info_struct failed");
		return NULL;
	}

	/* Error handling for init_io */
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during init_io\n");
		return NULL;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	/* Get image information */
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	/* Debugging */
	printf("%s (%dx%d) color_type:%d bit_depth:%d\n",
			file_path, width, height, color_type, bit_depth);

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	/* Error handling for reading image */
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error while reading file\n");
		return NULL;
	}

	/* Read image data */
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (i = 0; i < height; ++i)
		row_pointers[i] = (png_byte*) malloc(png_get_rowbytes(png_ptr, info_ptr));

	png_read_image(png_ptr, row_pointers);

	fclose(fp);

	return NULL;
}
