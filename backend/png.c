/* png.c - Handles output to PNG file */

/*
    libzint - the open source barcode library
    Copyright (C) 2008 Robin Stuart <zint@hotmail.co.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/* Any resemblance to the code used by Greg Reolofs' wpng code examples is not
   coincidental. Rather odd use of mainprog_info in this file is due to gradual adaption
   from that code. Read his excellent book "PNG: The Definitive Guide" online at
   http://www.libpng.org/pub/png/book/ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "png.h"        /* libpng header; includes zlib.h and setjmp.h */
#include "common.h"
#include "maxipng.h"	/* Maxicode shapes */

#define SSET	"0123456789ABCDEF"

struct mainprog_info_type {
    long width;
    long height;
    FILE *outfile;
    jmp_buf jmpbuf;
};

static void writepng_error_handler(png_structp png_ptr, png_const_charp msg)
{
    struct mainprog_info_type  *graphic;

    fprintf(stderr, "writepng libpng error: %s\n", msg);
    fflush(stderr);

    graphic = png_get_error_ptr(png_ptr);
    if (graphic == NULL) {         /* we are completely hosed now */
        fprintf(stderr,
          "writepng severe error:  jmpbuf not recoverable; terminating.\n");
        fflush(stderr);
        exit(99);
    }
    longjmp(graphic->jmpbuf, 1);
}

int png_plot(struct zint_symbol *symbol)
{
	struct mainprog_info_type wpng_info;
	struct mainprog_info_type *graphic;
	unsigned char outdata[6000];
	png_structp  png_ptr;
	png_infop  info_ptr;
	graphic = &wpng_info;
        long j;
        unsigned long rowbytes;
	unsigned char *image_data;
	int i, k, offset, row, row_binding, errno;
	int fgred, fggrn, fgblu, bgred, bggrn, bgblu;
	float large_bar_height, preset_height;
	int large_bar_count;
	
	/* sort out colour options */
	to_upper(symbol->fgcolour);
	to_upper(symbol->bgcolour);
	
	if(strlen(symbol->fgcolour) != 6) {
		strcpy(symbol->errtxt, "error: malformed foreground colour target");
		return ERROR_INVALID_OPTION;
	}
	if(strlen(symbol->bgcolour) != 6) {
		strcpy(symbol->errtxt, "error: malformed background colour target");
		return ERROR_INVALID_OPTION;
	}
	errno = is_sane(SSET, symbol->fgcolour);
	if (errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: malformed foreground colour target");
		return ERROR_INVALID_OPTION;
	}
	errno = is_sane(SSET, symbol->bgcolour);
	if (errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: malformed background colour target");
		return ERROR_INVALID_OPTION;
	}
	
	fgred = (16 * ctoi(symbol->fgcolour[0])) + ctoi(symbol->fgcolour[1]);
	fggrn = (16 * ctoi(symbol->fgcolour[2])) + ctoi(symbol->fgcolour[3]);
	fgblu = (16 * ctoi(symbol->fgcolour[4])) + ctoi(symbol->fgcolour[5]);
	bgred = (16 * ctoi(symbol->bgcolour[0])) + ctoi(symbol->bgcolour[1]);
	bggrn = (16 * ctoi(symbol->bgcolour[2])) + ctoi(symbol->bgcolour[3]);
	bgblu = (16 * ctoi(symbol->bgcolour[4])) + ctoi(symbol->bgcolour[5]);
	
	/* x-dimension is equivalent to 2 pixels so multiply everything by 2 */
	
	/* calculate graphic width and height */
	graphic->width = 2 * (symbol->width + (symbol->border_width * 2) + (symbol->whitespace_width * 2));
	row_binding = 0;
	
	if (symbol->height == 0) {
		symbol->height = 50;
	}
	
	large_bar_count = 0;
	preset_height = 0.0;
	for(i = 0; i < symbol->rows; i++) {
		preset_height += symbol->row_height[i];
		if(symbol->row_height[i] == 0) {
			large_bar_count++;
		}
	}
	large_bar_height = (symbol->height - preset_height) / large_bar_count;

	if (large_bar_count == 0) {
		symbol->height = preset_height;
	}

	graphic->height = (2 * symbol->height) + (symbol->border_width * 4);

	if((symbol->rows > 1) && ((symbol->output_options == BARCODE_BIND) || (symbol->output_options == BARCODE_BOX))) {
		row_binding = 1;
	}

	if(symbol->symbology == BARCODE_CODE16K) {
		row_binding = 1;
	}
	
	/* Open output file in binary mode */
	if (!(graphic->outfile = fopen(symbol->outfile, "wb"))) {
		strcpy(symbol->errtxt, "error: can't open output file");
		return ERROR_FILE_ACCESS;
	}
	
	/* Set up error handling routine as proc() above */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, graphic,
		writepng_error_handler, NULL);
	if (!png_ptr) {
		strcpy(symbol->errtxt, "error: out of memory");
		return ERROR_MEMORY;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, NULL);
		strcpy(symbol->errtxt, "error: out of memory");
		return ERROR_MEMORY;
	}

	/* catch jumping here */
	if (setjmp(graphic->jmpbuf)) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		strcpy(symbol->errtxt, "error: libpng error occurred");
		return ERROR_MEMORY;
	}

	/* open output file with libpng */
	png_init_io(png_ptr, graphic->outfile);

	/* set compression */
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

	/* set Header block */
	png_set_IHDR(png_ptr, info_ptr, graphic->width, graphic->height,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	/* write all chunks up to (but not including) first IDAT */
	png_write_info(png_ptr, info_ptr);

	/* set up the transformations:  for now, just pack low-bit-depth pixels
	   into bytes (one, two or four pixels per byte) */
	png_set_packing(png_ptr);

	/* set rowbytes - depends on picture depth */
	rowbytes = wpng_info.width * 3;

	/* Pixel Plotting */
	offset = (2 * symbol->border_width) + (2 * symbol->whitespace_width);
	for (j = 0;  j < graphic->height;  j++) {


		
		/* top border */
		if (j < (2 * symbol->border_width)) {
			if ((symbol->output_options == BARCODE_BOX) || (symbol->output_options == BARCODE_BIND)) {
				for(i = 0; i < (graphic->width * 3); i+=3) {
					outdata[i] = fgred;
					outdata[i + 1] = fggrn;
					outdata[i + 2] = fgblu;
				}
			} else {
				for(i = 0; i < (graphic->width * 3); i+=3) {
					outdata[i] = bgred;
					outdata[i + 1] = bggrn;
					outdata[i + 2] = bgblu;
				}
			}
		}

		/* middle section */
		if ((j >= (2 * symbol->border_width)) && (j < (graphic->height - (2 * symbol->border_width)))) {
			for(i = 0; i < (graphic->width * 3); i+= 3) {
				k = (i / 3);

				/* left hand border */
				if(k < (2 * symbol->border_width)) {
					if(symbol->output_options == BARCODE_BOX) {
						outdata[i] = fgred;
						outdata[i + 1] = fggrn;
						outdata[i + 2] = fgblu;
					} else {
						outdata[i] = bgred;
						outdata[i + 1] = bggrn;
						outdata[i + 2] = bgblu;
					}
				}

				/* left whitespace */
				if((k >= (2 * symbol->border_width)) && (k < ((2 * symbol->border_width) + (2 * symbol->whitespace_width)))) {
					outdata[i] = bgred;
					outdata[i + 1] = bggrn;
					outdata[i + 2] = bgblu;
				}
				
				/* the symbol area */
				if((k >= ((2 * symbol->border_width) + (2 * symbol->whitespace_width))) && (k < (graphic->width - ((2 * symbol->border_width) + (2 * symbol->whitespace_width))))) {
					float y_position, cumul;
		
					y_position = ((j / 2) - symbol->border_width);
					y_position += 1.0;
					row = 0;
		
					if(symbol->row_height[row] == 0) {
						cumul = large_bar_height;
					} else {
						cumul = symbol->row_height[row];
					}
		
					while(cumul < y_position) {
						row++;
						if(symbol->row_height[row] == 0) {
							cumul += large_bar_height;
						} else {
							cumul += symbol->row_height[row];
						}
					}
					
					switch(symbol->encoded_data[row][(k - offset) / 2])
					{
						case '1':
							outdata[i] = fgred;
							outdata[i + 1] = fggrn;
							outdata[i + 2] = fgblu;
							break;
							/* NOTE: Other preset colours will go here */
						case 'R':
							outdata[i] = 0xff;
							outdata[i + 1] = 0x00;
							outdata[i + 2] = 0x00;
							break;
						case 'G':
							outdata[i] = 0x00;
							outdata[i + 1] = 0xff;
							outdata[i + 2] = 0x00;
							break;
						case 'B':
							outdata[i] = 0x00;
							outdata[i + 1] = 0x00;
							outdata[i + 2] = 0xff;
							break;
						case 'C':
							outdata[i] = 0x00;
							outdata[i + 1] = 0xff;
							outdata[i + 2] = 0xff;
							break;
						case 'M':
							outdata[i] = 0xff;
							outdata[i + 1] = 0x00;
							outdata[i + 2] = 0xff;
							break;
						case 'Y':
							outdata[i] = 0xff;
							outdata[i + 1] = 0xff;
							outdata[i + 2] = 0x00;
							break;
						default:
							outdata[i] = bgred;
							outdata[i + 1] = bggrn;
							outdata[i + 2] = bgblu;
							break;

					}
					
					/* row binding */
					if(row_binding) {
						/* Only valid if all rows are the same height */
						float total_height, equal_height;
						
						total_height = (graphic->height / 2) - symbol->border_width;
						equal_height = total_height / symbol->rows;
						
						if((y_position > ((equal_height * row) - 1.0)) && (y_position < ((equal_height * row) + 1.0))) {
							outdata[i] = fgred;
							outdata[i + 1] = fggrn;
							outdata[i + 2] = fgblu;
						}
						
						if((y_position > ((equal_height * (row + 1)) - 1.0)) && (y_position < ((equal_height * (row + 1)) + 1.0))) {
							outdata[i] = fgred;
							outdata[i + 1] = fggrn;
							outdata[i + 2] = fgblu;
						}
					}

				}

				/* right whitespace */
				if((k >= (graphic->width - ((2 *symbol->border_width) + (2 * symbol->whitespace_width)))) && (k < (graphic->width - (2 * symbol->border_width)))) {
					outdata[i] = bgred;
					outdata[i + 1] = bggrn;
					outdata[i + 2] = bgblu;
				}

				/* right hand border */
				if(k >= (graphic->width - (2 * symbol->border_width))) {
					if(symbol->output_options == BARCODE_BOX) {
						outdata[i] = fgred;
						outdata[i + 1] = fggrn;
						outdata[i + 2] = fgblu;
					} else {
						outdata[i] = bgred;
						outdata[i + 1] = bggrn;
						outdata[i + 2] = bgblu;
					}
				}
			}
		}

		/* bottom border */
		if (j >= (graphic->height - (2 * symbol->border_width))) {
			if ((symbol->output_options == BARCODE_BOX) || (symbol->output_options == BARCODE_BIND)) {
				for(i = 0; i < (graphic->width * 3); i+=3) {
					outdata[i] = fgred;
					outdata[i + 1] = fggrn;
					outdata[i + 2] = fgblu;
				}
			} else {
				for(i = 0; i < (graphic->width * 3); i+=3) {
					outdata[i] = bgred;
					outdata[i + 1] = bggrn;
					outdata[i + 2] = bgblu;
				}
			}
		}

		/* write row contents to file */
		image_data = outdata;
		png_write_row(png_ptr, image_data);
	}

	/* End the file */
	png_write_end(png_ptr, NULL);

	/* make sure we have disengaged */
	if (png_ptr && info_ptr) png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(wpng_info.outfile);

	return 0;
}

int maxi_png_plot(struct zint_symbol *symbol)
{
	struct mainprog_info_type wpng_info;
	struct mainprog_info_type *graphic;
	unsigned char outdata[6000];
	png_structp  png_ptr;
	png_infop  info_ptr;
	graphic = &wpng_info;
	long j;
	unsigned long rowbytes;
	unsigned char *image_data;
	int i, k, row, row_binding, errno;
	int fgred, fggrn, fgblu, bgred, bggrn, bgblu;
	int x_posn, y_posn, w_posn, h_posn;
	
	/* sort out colour options */
	to_upper(symbol->fgcolour);
	to_upper(symbol->bgcolour);
	
	if(strlen(symbol->fgcolour) != 6) {
		strcpy(symbol->errtxt, "error: malformed foreground colour target");
		return ERROR_INVALID_OPTION;
	}
	if(strlen(symbol->bgcolour) != 6) {
		strcpy(symbol->errtxt, "error: malformed background colour target");
		return ERROR_INVALID_OPTION;
	}
	errno = is_sane(SSET, symbol->fgcolour);
	if (errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: malformed foreground colour target");
		return ERROR_INVALID_OPTION;
	}
	errno = is_sane(SSET, symbol->bgcolour);
	if (errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: malformed background colour target");
		return ERROR_INVALID_OPTION;
	}
	
	fgred = (16 * ctoi(symbol->fgcolour[0])) + ctoi(symbol->fgcolour[1]);
	fggrn = (16 * ctoi(symbol->fgcolour[2])) + ctoi(symbol->fgcolour[3]);
	fgblu = (16 * ctoi(symbol->fgcolour[4])) + ctoi(symbol->fgcolour[5]);
	bgred = (16 * ctoi(symbol->bgcolour[0])) + ctoi(symbol->bgcolour[1]);
	bggrn = (16 * ctoi(symbol->bgcolour[2])) + ctoi(symbol->bgcolour[3]);
	bgblu = (16 * ctoi(symbol->bgcolour[4])) + ctoi(symbol->bgcolour[5]);
	
	graphic->width = 300;
	graphic->height = 300;
	
	/* Open output file in binary mode */
	if (!(graphic->outfile = fopen(symbol->outfile, "wb"))) {
		strcpy(symbol->errtxt, "error: can't open output file");
		return ERROR_FILE_ACCESS;
	}
	
	/* Set up error handling routine as proc() above */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, graphic,
					  writepng_error_handler, NULL);
	if (!png_ptr) {
		strcpy(symbol->errtxt, "error: out of memory");
		return ERROR_MEMORY;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, NULL);
		strcpy(symbol->errtxt, "error: out of memory");
		return ERROR_MEMORY;
	}

	/* catch jumping here */
	if (setjmp(graphic->jmpbuf)) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		strcpy(symbol->errtxt, "error: libpng error occurred");
		return ERROR_MEMORY;
	}

	/* open output file with libpng */
	png_init_io(png_ptr, graphic->outfile);

	/* set compression */
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

	/* set Header block */
	png_set_IHDR(png_ptr, info_ptr, graphic->width, graphic->height,
		     8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
       PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	/* write all chunks up to (but not including) first IDAT */
	png_write_info(png_ptr, info_ptr);

	/* set up the transformations:  for now, just pack low-bit-depth pixels
	into bytes (one, two or four pixels per byte) */
	png_set_packing(png_ptr);

	/* set rowbytes - depends on picture depth */
	rowbytes = wpng_info.width * 3;

	/* Pixel Plotting */
	for (j = 0;  j < graphic->height;  j++) {
		for(i = 0; i < (graphic->width * 3); i += 3) {
			outdata[i] = bgred;
			outdata[i + 1] = bggrn;
			outdata[i + 2] = bgblu;
		}
		
		if((j % 18) < 12) {
			/* We are in a 'full' row of hexagons */
			for(i = 0; i < graphic->width; i++) {
				x_posn = i / 10;
				y_posn = j / 18;
				y_posn *= 2;
				
				if(symbol->encoded_data[y_posn][x_posn] == '1') {
					/* A hexagon needs plotting here */
					w_posn = i % 10;
					h_posn = j % 18;
					if(hexagon[(h_posn * 10) + w_posn] == 1) {
						outdata[i * 3] = fgred;
						outdata[(i * 3) + 1] = fggrn;
						outdata[(i * 3) + 2] = fgblu;
					}
				}
			}
		}

		if(((j + 9) % 18) < 12) {
			int ia, ja;
			ja = j - 9;
			/* We are in a 'reduced' row of hexagons */
			for(i = 5; i < (graphic->width - 5); i++) {
				ia = i - 5;
				x_posn = ia / 10;
				y_posn = ja / 18;
				y_posn *= 2;
				y_posn++;
				
				if(symbol->encoded_data[y_posn][x_posn] == '1') {
					/* A hexagon needs plotting here */
					w_posn = ia % 10;
					h_posn = ja % 18;
					if(hexagon[(h_posn * 10) + w_posn] == 1) {
						outdata[i * 3] = fgred;
						outdata[(i * 3) + 1] = fggrn;
						outdata[(i * 3) + 2] = fgblu;
					}
				}
			}
		}
		
		if((j >= 103) && (j < 196)) {
			/* Central bullseye */
			for(i = 0; i < 93; i++) {
				if(bullseye[(((j - 103) * 93) + i) * 4] == '!') {
					outdata[i * 3 + 297] = fgred;
					outdata[(i * 3) + 1 + 297] = fggrn;
					outdata[(i * 3) + 2 + 297] = fgblu;
				}
			}
		}
		
		/* write row contents to file */
		image_data = outdata;
		png_write_row(png_ptr, image_data);
	}

	/* End the file */
	png_write_end(png_ptr, NULL);

	/* make sure we have disengaged */
	if (png_ptr && info_ptr) png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(wpng_info.outfile);

	return 0;
}
