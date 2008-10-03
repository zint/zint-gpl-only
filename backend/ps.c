/* ps.c - Post Script output */

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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

#define SSET	"0123456789ABCDEF"

/* This file has expanded quite a bit since version 1.5 in order to accomodate
   the formatting rules for EAN and UPC symbols as set out in EN 797:1995 - the
   down side of this support is that the code is now vertually unreadable! */

int ps_plot(struct zint_symbol *symbol)
{
	int i, block_width, latch, r, this_row;
	float textpos, large_bar_height, preset_height, row_height, row_posn;
	FILE *feps;
	int fgred, fggrn, fgblu, bgred, bggrn, bgblu;
	float red_ink, green_ink, blue_ink, red_paper, green_paper, blue_paper;
	int error_number = 0;
	int textoffset, xoffset, yoffset, textdone, main_width;
	char textpart[10], addon[6];
	int large_bar_count, comp_offset;
	float addon_text_posn;
	
	row_height=0;
	textdone = 0;
	main_width = symbol->width;
	strcpy(addon, "");
	comp_offset = 0;
	addon_text_posn = 0.0;
	
	feps = fopen(symbol->outfile, "w");
	if(feps == NULL) {
		strcpy(symbol->errtxt, "error: could not open output file");
		return ERROR_FILE_ACCESS;
	}
	
	/* sort out colour options */
	to_upper((unsigned char*)symbol->fgcolour);
	to_upper((unsigned char*)symbol->bgcolour);
	
	if(strlen(symbol->fgcolour) != 6) {
		strcpy(symbol->errtxt, "error: malformed foreground colour target");
		return ERROR_INVALID_OPTION;
	}
	if(strlen(symbol->bgcolour) != 6) {
		strcpy(symbol->errtxt, "error: malformed background colour target");
		return ERROR_INVALID_OPTION;
	}
	error_number = is_sane(SSET, (unsigned char*)symbol->fgcolour);
	if (error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: malformed foreground colour target");
		return ERROR_INVALID_OPTION;
	}
	error_number = is_sane(SSET, (unsigned char*)symbol->bgcolour);
	if (error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: malformed background colour target");
		return ERROR_INVALID_OPTION;
	}
	
	fgred = (16 * ctoi(symbol->fgcolour[0])) + ctoi(symbol->fgcolour[1]);
	fggrn = (16 * ctoi(symbol->fgcolour[2])) + ctoi(symbol->fgcolour[3]);
	fgblu = (16 * ctoi(symbol->fgcolour[4])) + ctoi(symbol->fgcolour[5]);
	bgred = (16 * ctoi(symbol->bgcolour[0])) + ctoi(symbol->bgcolour[1]);
	bggrn = (16 * ctoi(symbol->bgcolour[2])) + ctoi(symbol->bgcolour[3]);
	bgblu = (16 * ctoi(symbol->bgcolour[4])) + ctoi(symbol->bgcolour[5]);
	red_ink = fgred / 256.0;
	green_ink = fggrn / 256.0;
	blue_ink = fgblu / 256.0;
	red_paper = bgred / 256.0;
	green_paper = bggrn / 256.0;
	blue_paper = bgblu / 256.0;
	
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
	
	while(symbol->encoded_data[symbol->rows - 1][comp_offset] != '1') {
		comp_offset++;
	}

	/* Certain symbols need whitespace otherwise characters get chopped off the sides */
	if (((symbol->symbology == BARCODE_EANX) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_EANX_CC)) {
		switch(strlen(symbol->text)) {
			case 13: /* EAN 13 */
			case 16:
			case 19:
				if(symbol->whitespace_width == 0) {
					symbol->whitespace_width = 10;
				}
				main_width = 96 + comp_offset;
				break;
			default:
				main_width = 68 + comp_offset;
		}
	}
	
	if (((symbol->symbology == BARCODE_UPCA) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCA_CC)) {
		if(symbol->whitespace_width == 0) {
			symbol->whitespace_width = 10;
			main_width = 96 + comp_offset;
		}
	}
	
	if (((symbol->symbology == BARCODE_UPCE) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCE_CC)) {
		if(symbol->whitespace_width == 0) {
			symbol->whitespace_width = 10;
			main_width = 51 + comp_offset;
		}
	}
	
	latch = 0;
	r = 0;
	/* Isolate add-on text */
	for(i = 0; i < strlen(symbol->text); i++) {
		if (latch == 1) {
			addon[r] = symbol->text[i];
			r++;
		}
		if (symbol->text[i] == '+') {
			latch = 1;
		}
	}
	addon[r] = '\0';
	
	if(strcmp(symbol->text, "")) {
		textoffset = 9;
	} else {
		textoffset = 0;
	}
	xoffset = symbol->border_width + symbol->whitespace_width;
	yoffset = symbol->border_width;
	
	/* Start writing the header */
	fprintf(feps, "%%!PS-Adobe-3.0 EPSF-3.0\n");
	fprintf(feps, "%%%%Creator: Zint %s\n", ZINT_VERSION);
	if(strlen(symbol->text) != 0) {
		fprintf(feps, "%%%%Title: %s\n",symbol->text);
	} else {
		fprintf(feps, "%%%%Title: Zint Generated Symbol\n");
	}
	fprintf(feps, "%%%%Pages: 0\n");
	if(symbol->symbology != BARCODE_MAXICODE) {
		fprintf(feps, "%%%%BoundingBox: 0 0 %d %d\n", (symbol->width + xoffset + xoffset), (symbol->height + textoffset + yoffset + yoffset));
	} else {
		fprintf(feps, "%%%%BoundingBox: 0 0 74 72\n");
	}
	fprintf(feps, "%%%%EndComments\n");
	
	/* Definitions */
	fprintf(feps, "/TL { setlinewidth moveto lineto stroke } bind def\n");
	fprintf(feps, "/TC { moveto 0 360 arc 360 0 arcn fill } bind def\n");
	fprintf(feps, "/TH { 0 setlinewidth moveto lineto lineto lineto lineto lineto closepath fill } bind def\n");
	fprintf(feps, "/TB { 2 copy } bind def\n");
	fprintf(feps, "/TR { newpath 4 1 roll exch moveto 1 index 0 rlineto 0 exch rlineto neg 0 rlineto closepath fill } bind def\n");
	fprintf(feps, "/TE { pop pop } bind def\n");
	
	fprintf(feps, "newpath\n");
	
	/* Now the actual representation */
	fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
	fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_paper, green_paper, blue_paper);
	fprintf(feps, "%d.00 0.00 TB 0.00 %d.00 TR\n", (symbol->height + textoffset + yoffset + yoffset), symbol->width + xoffset + xoffset);
	

	if(symbol->symbology == BARCODE_MAXICODE) {
		/* Maxicode uses hexagons */
		float ax, ay, bx, by, cx, cy, dx, dy, ex, ey, fx, fy, mx, my;
		
		fprintf(feps, "TE\n");
		fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
		fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
		fprintf(feps, "35.76 35.60 10.85 35.76 35.60 8.97 44.73 35.60 TC\n");
		fprintf(feps, "35.76 35.60 7.10 35.76 35.60 5.22 40.98 35.60 TC\n");
		fprintf(feps, "35.76 35.60 3.31 35.76 35.60 1.43 37.19 35.60 TC\n");
		for(r = 0; r < symbol->rows; r++) {
			for(i = 0; i < symbol->width; i++) {
				if(symbol->encoded_data[r][i] == '1') {
					/* Dump a hexagon */
					my = ((symbol->rows - r - 1)) * 2.135 + 1.43;
					ay = my + 1.0;
					by = my + 0.5;
					cy = my - 0.5;
					dy = my - 1.0;
					ey = my - 0.5;
					fy = my + 0.5;
					if(r % 2 == 1) {
						mx = (2.46 * i) + 1.23 + 1.23;
					} else {
						mx = (2.46 * i) + 1.23;
					}
					ax = mx;
					bx = mx + 0.86;
					cx = mx + 0.86;
					dx = mx;
					ex = mx - 0.86;
					fx = mx - 0.86;
					fprintf(feps, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f TH\n", ax, ay, bx, by, cx, cy, dx, dy, ex, ey, fx, fy);
				}
			}
		}
	}	
	
	if(symbol->symbology != BARCODE_MAXICODE) {
		/* everything else uses rectangles (or squares) */
		/* Works from the bottom of the symbol up */
		int addon_latch = 0;
		
		for(r = 0; r < symbol->rows; r++) {
			this_row = symbol->rows - r - 1; /* invert r otherwise plots upside down */
			if(symbol->row_height[this_row] == 0) {
				row_height = large_bar_height;
			} else {
				row_height = symbol->row_height[this_row];
			}
			row_posn = 0;
			for(i = 0; i < r; i++) {
				if(symbol->row_height[symbol->rows - i - 1] == 0) {
					row_posn += large_bar_height;
				} else {
					row_posn += symbol->row_height[symbol->rows - i - 1];
				}
			}
			row_posn += (textoffset + yoffset);
			
			fprintf(feps, "TE\n");
			fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
			fprintf(feps, "%.2f %.2f ", row_height, row_posn);
			i = 0;
			if(symbol->encoded_data[this_row][0] == '1') {
				latch = 1;
			} else {
				latch = 0;
			}
			
			do {
				block_width = 0;
				do {
					block_width++;
				} while (symbol->encoded_data[this_row][i + block_width] == symbol->encoded_data[this_row][i]);
				if((addon_latch == 0) && (r == 0) && (i > main_width)) {
					fprintf(feps, "TE\n");
					fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
					fprintf(feps, "%.2f %.2f ", (row_height - 5.0), (row_posn - 5.0));
					addon_text_posn = row_posn + row_height - 8.0;
					addon_latch = 1;
				} 
				if(latch == 1) { 
					/* a bar */
					fprintf(feps, "TB %d.00 %d.00 TR\n", i + xoffset, block_width);
					latch = 0;
				} else {
					/* a space */
					latch = 1;
				}
				i += block_width;
				
			} while (i < symbol->width);
		}
	}
	/* That's done the actual data area, everything else is human-friendly */

	xoffset += comp_offset;

	if (((symbol->symbology == BARCODE_EANX) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_EANX_CC)) {
		/* guard bar extensions and text formatting for EAN8 and EAN13 */
		switch(strlen(symbol->text)) {
			case 8: /* EAN-8 */
			case 11:
			case 14:
				fprintf(feps, "TE\n");
				fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
				fprintf(feps, "%.2f %.2f ", 5.0, 4.0 + yoffset);
				fprintf(feps, "TB %d.00 %d.00 TR\n", 0 + xoffset, 1);
				fprintf(feps, "TB %d.00 %d.00 TR\n", 2 + xoffset, 1);
				fprintf(feps, "TB %d.00 %d.00 TR\n", 32 + xoffset, 1);
				fprintf(feps, "TB %d.00 %d.00 TR\n", 34 + xoffset, 1);
				fprintf(feps, "TB %d.00 %d.00 TR\n", 64 + xoffset, 1);
				fprintf(feps, "TB %d.00 %d.00 TR\n", 66 + xoffset, 1);
				for(i = 0; i < 4; i++) {
					textpart[i] = symbol->text[i];
				}
				textpart[4] = '\0';
				fprintf(feps, "TE\n");
				fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
				fprintf(feps, "matrix currentmatrix\n");
				fprintf(feps, "/Helvetica findfont\n");
				fprintf(feps, "11.00 scalefont setfont\n");
				textpos = 17;
				fprintf(feps, " 0 0 moveto %.2f 0.50 translate 0.00 rotate 0 0 moveto\n", textpos + xoffset);
				fprintf(feps, " (%s) stringwidth\n", textpart);
				fprintf(feps, "pop\n");
				fprintf(feps, "-2 div 0 rmoveto\n");
				fprintf(feps, " (%s) show\n", textpart);
				fprintf(feps, "setmatrix\n");
				for(i = 0; i < 4; i++) {
					textpart[i] = symbol->text[i + 4];
				}
				textpart[4] = '\0';
				fprintf(feps, "matrix currentmatrix\n");
				fprintf(feps, "/Helvetica findfont\n");
				fprintf(feps, "11.00 scalefont setfont\n");
				textpos = 50;
				fprintf(feps, " 0 0 moveto %.2f 0.50 translate 0.00 rotate 0 0 moveto\n", textpos + xoffset);
				fprintf(feps, " (%s) stringwidth\n", textpart);
				fprintf(feps, "pop\n");
				fprintf(feps, "-2 div 0 rmoveto\n");
				fprintf(feps, " (%s) show\n", textpart);
				fprintf(feps, "setmatrix\n");
				textdone = 1;
				switch(strlen(addon)) {
					case 2:	
						fprintf(feps, "matrix currentmatrix\n");
						fprintf(feps, "/Helvetica findfont\n");
						fprintf(feps, "11.00 scalefont setfont\n");
						textpos = symbol->width + xoffset - 10;
						fprintf(feps, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos, addon_text_posn);
						fprintf(feps, " (%s) stringwidth\n", addon);
						fprintf(feps, "pop\n");
						fprintf(feps, "-2 div 0 rmoveto\n");
						fprintf(feps, " (%s) show\n", addon);
						fprintf(feps, "setmatrix\n");
						break;
					case 5:
						fprintf(feps, "matrix currentmatrix\n");
						fprintf(feps, "/Helvetica findfont\n");
						fprintf(feps, "11.00 scalefont setfont\n");
						textpos = symbol->width + xoffset - 23;
						fprintf(feps, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos, addon_text_posn);
						fprintf(feps, " (%s) stringwidth\n", addon);
						fprintf(feps, "pop\n");
						fprintf(feps, "-2 div 0 rmoveto\n");
						fprintf(feps, " (%s) show\n", addon);
						fprintf(feps, "setmatrix\n");
						break;
				}

				break;
			case 13: /* EAN 13 */
			case 16:
			case 19:
				fprintf(feps, "TE\n");
				fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
				fprintf(feps, "%.2f %.2f ", 5.0, 4.0 + yoffset);
				fprintf(feps, "TB %d.00 %d.00 TR\n", 0 + xoffset, 1);
				fprintf(feps, "TB %d.00 %d.00 TR\n", 2 + xoffset, 1);
				fprintf(feps, "TB %d.00 %d.00 TR\n", 46 + xoffset, 1);
				fprintf(feps, "TB %d.00 %d.00 TR\n", 48 + xoffset, 1);
				fprintf(feps, "TB %d.00 %d.00 TR\n", 92 + xoffset, 1);
				fprintf(feps, "TB %d.00 %d.00 TR\n", 94 + xoffset, 1);
				textpart[0] = symbol->text[0];
				textpart[1] = '\0';
				fprintf(feps, "TE\n");
				fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
				fprintf(feps, "matrix currentmatrix\n");
				fprintf(feps, "/Helvetica findfont\n");
				fprintf(feps, "11.00 scalefont setfont\n");
				textpos = -7;
				fprintf(feps, " 0 0 moveto %.2f 0.50 translate 0.00 rotate 0 0 moveto\n", textpos + xoffset);
				fprintf(feps, " (%s) stringwidth\n", textpart);
				fprintf(feps, "pop\n");
				fprintf(feps, "-2 div 0 rmoveto\n");
				fprintf(feps, " (%s) show\n", textpart);
				fprintf(feps, "setmatrix\n");
				for(i = 0; i < 6; i++) {
					textpart[i] = symbol->text[i + 1];
				}
				textpart[6] = '\0';
				fprintf(feps, "matrix currentmatrix\n");
				fprintf(feps, "/Helvetica findfont\n");
				fprintf(feps, "11.00 scalefont setfont\n");
				textpos = 24;
				fprintf(feps, " 0 0 moveto %.2f 0.50 translate 0.00 rotate 0 0 moveto\n", textpos + xoffset);
				fprintf(feps, " (%s) stringwidth\n", textpart);
				fprintf(feps, "pop\n");
				fprintf(feps, "-2 div 0 rmoveto\n");
				fprintf(feps, " (%s) show\n", textpart);
				fprintf(feps, "setmatrix\n");
				for(i = 0; i < 6; i++) {
					textpart[i] = symbol->text[i + 7];
				}
				textpart[6] = '\0';
				fprintf(feps, "matrix currentmatrix\n");
				fprintf(feps, "/Helvetica findfont\n");
				fprintf(feps, "11.00 scalefont setfont\n");
				textpos = 71;
				fprintf(feps, " 0 0 moveto %.2f 0.50 translate 0.00 rotate 0 0 moveto\n", textpos + xoffset);
				fprintf(feps, " (%s) stringwidth\n", textpart);
				fprintf(feps, "pop\n");
				fprintf(feps, "-2 div 0 rmoveto\n");
				fprintf(feps, " (%s) show\n", textpart);
				fprintf(feps, "setmatrix\n");
				textdone = 1;
				switch(strlen(addon)) {
					case 2:	
						fprintf(feps, "matrix currentmatrix\n");
						fprintf(feps, "/Helvetica findfont\n");
						fprintf(feps, "11.00 scalefont setfont\n");
						textpos = symbol->width + xoffset - 10;
						fprintf(feps, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos, addon_text_posn);
						fprintf(feps, " (%s) stringwidth\n", addon);
						fprintf(feps, "pop\n");
						fprintf(feps, "-2 div 0 rmoveto\n");
						fprintf(feps, " (%s) show\n", addon);
						fprintf(feps, "setmatrix\n");
						break;
					case 5:
						fprintf(feps, "matrix currentmatrix\n");
						fprintf(feps, "/Helvetica findfont\n");
						fprintf(feps, "11.00 scalefont setfont\n");
						textpos = symbol->width + xoffset - 23;
						fprintf(feps, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos, addon_text_posn);
						fprintf(feps, " (%s) stringwidth\n", addon);
						fprintf(feps, "pop\n");
						fprintf(feps, "-2 div 0 rmoveto\n");
						fprintf(feps, " (%s) show\n", addon);
						fprintf(feps, "setmatrix\n");
						break;
				}
				break;

		}
	}	

	if (((symbol->symbology == BARCODE_UPCA) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCA_CC)) {
		/* guard bar extensions and text formatting for UPCA */
		fprintf(feps, "TE\n");
		fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
		fprintf(feps, "%.2f %.2f ", 5.0, 4.0 + yoffset);
		latch = 1;
		
		i = 0 + comp_offset;
		do {
			block_width = 0;
			do {
				block_width++;
			} while (symbol->encoded_data[symbol->rows - 1][i + block_width] == symbol->encoded_data[symbol->rows - 1][i]);
			if(latch == 1) {
				/* a bar */
				fprintf(feps, "TB %d.00 %d.00 TR\n", i + xoffset - comp_offset, block_width);
				latch = 0;
			} else {
				/* a space */
				latch = 1;
			}
			i += block_width;
		} while (i < 11 + comp_offset);
		fprintf(feps, "TB %d.00 %d.00 TR\n", 46 + xoffset, 1);
		fprintf(feps, "TB %d.00 %d.00 TR\n", 48 + xoffset, 1);
		latch = 1;
		i = 85 + comp_offset;
		do {
			block_width = 0;
			do {
				block_width++;
			} while (symbol->encoded_data[symbol->rows - 1][i + block_width] == symbol->encoded_data[symbol->rows - 1][i]);
			if(latch == 1) {
				/* a bar */
				fprintf(feps, "TB %d.00 %d.00 TR\n", i + xoffset - comp_offset, block_width);
				latch = 0;
			} else {
				/* a space */
				latch = 1;
			}
			i += block_width;
		} while (i < 96 + comp_offset);
		textpart[0] = symbol->text[0];
		textpart[1] = '\0';
		fprintf(feps, "TE\n");
		fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
		fprintf(feps, "matrix currentmatrix\n");
		fprintf(feps, "/Helvetica findfont\n");
		fprintf(feps, "8.00 scalefont setfont\n");
		textpos = -5;
		fprintf(feps, " 0 0 moveto %.2f 0.50 translate 0.00 rotate 0 0 moveto\n", textpos + xoffset);
		fprintf(feps, " (%s) stringwidth\n", textpart);
		fprintf(feps, "pop\n");
		fprintf(feps, "-2 div 0 rmoveto\n");
		fprintf(feps, " (%s) show\n", textpart);
		fprintf(feps, "setmatrix\n");
		for(i = 0; i < 5; i++) {
			textpart[i] = symbol->text[i + 1];
		}
		textpart[5] = '\0';
		fprintf(feps, "matrix currentmatrix\n");
		fprintf(feps, "/Helvetica findfont\n");
		fprintf(feps, "11.00 scalefont setfont\n");
		textpos = 27;
		fprintf(feps, " 0 0 moveto %.2f 0.50 translate 0.00 rotate 0 0 moveto\n", textpos + xoffset);
		fprintf(feps, " (%s) stringwidth\n", textpart);
		fprintf(feps, "pop\n");
		fprintf(feps, "-2 div 0 rmoveto\n");
		fprintf(feps, " (%s) show\n", textpart);
		fprintf(feps, "setmatrix\n");
		for(i = 0; i < 5; i++) {
			textpart[i] = symbol->text[i + 6];
		}
		textpart[6] = '\0';
		fprintf(feps, "matrix currentmatrix\n");
		fprintf(feps, "/Helvetica findfont\n");
		fprintf(feps, "11.00 scalefont setfont\n");
		textpos = 68;
		fprintf(feps, " 0 0 moveto %.2f 0.50 translate 0.00 rotate 0 0 moveto\n", textpos + xoffset);
		fprintf(feps, " (%s) stringwidth\n", textpart);
		fprintf(feps, "pop\n");
		fprintf(feps, "-2 div 0 rmoveto\n");
		fprintf(feps, " (%s) show\n", textpart);
		fprintf(feps, "setmatrix\n");
		textpart[0] = symbol->text[11];
		textpart[1] = '\0';
		fprintf(feps, "matrix currentmatrix\n");
		fprintf(feps, "/Helvetica findfont\n");
		fprintf(feps, "8.00 scalefont setfont\n");
		textpos = 100;
		fprintf(feps, " 0 0 moveto %.2f 0.50 translate 0.00 rotate 0 0 moveto\n", textpos + xoffset);
		fprintf(feps, " (%s) stringwidth\n", textpart);
		fprintf(feps, "pop\n");
		fprintf(feps, "-2 div 0 rmoveto\n");
		fprintf(feps, " (%s) show\n", textpart);
		fprintf(feps, "setmatrix\n");
		textdone = 1;
		switch(strlen(addon)) {
			case 2:	
				fprintf(feps, "matrix currentmatrix\n");
				fprintf(feps, "/Helvetica findfont\n");
				fprintf(feps, "11.00 scalefont setfont\n");
				textpos = symbol->width + xoffset - 10;
				fprintf(feps, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos, addon_text_posn);
				fprintf(feps, " (%s) stringwidth\n", addon);
				fprintf(feps, "pop\n");
				fprintf(feps, "-2 div 0 rmoveto\n");
				fprintf(feps, " (%s) show\n", addon);
				fprintf(feps, "setmatrix\n");
				break;
			case 5:
				fprintf(feps, "matrix currentmatrix\n");
				fprintf(feps, "/Helvetica findfont\n");
				fprintf(feps, "11.00 scalefont setfont\n");
				textpos = symbol->width + xoffset - 23;
				fprintf(feps, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos, addon_text_posn);
				fprintf(feps, " (%s) stringwidth\n", addon);
				fprintf(feps, "pop\n");
				fprintf(feps, "-2 div 0 rmoveto\n");
				fprintf(feps, " (%s) show\n", addon);
				fprintf(feps, "setmatrix\n");
				break;
		}

	}	

	if (((symbol->symbology == BARCODE_UPCE) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCE_CC)) {
		/* guard bar extensions and text formatting for UPCE */
		fprintf(feps, "TE\n");
		fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
		fprintf(feps, "%.2f %.2f ", 5.0, 4.0 + yoffset);
		fprintf(feps, "TB %d.00 %d.00 TR\n", 0 + xoffset, 1);
		fprintf(feps, "TB %d.00 %d.00 TR\n", 2 + xoffset, 1);
		fprintf(feps, "TB %d.00 %d.00 TR\n", 46 + xoffset, 1);
		fprintf(feps, "TB %d.00 %d.00 TR\n", 48 + xoffset, 1);
		fprintf(feps, "TB %d.00 %d.00 TR\n", 50 + xoffset, 1);
		textpart[0] = symbol->text[0];
		textpart[1] = '\0';
		fprintf(feps, "TE\n");
		fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
		fprintf(feps, "matrix currentmatrix\n");
		fprintf(feps, "/Helvetica findfont\n");
		fprintf(feps, "8.00 scalefont setfont\n");
		textpos = -5;
		fprintf(feps, " 0 0 moveto %.2f 0.50 translate 0.00 rotate 0 0 moveto\n", textpos + xoffset);
		fprintf(feps, " (%s) stringwidth\n", textpart);
		fprintf(feps, "pop\n");
		fprintf(feps, "-2 div 0 rmoveto\n");
		fprintf(feps, " (%s) show\n", textpart);
		fprintf(feps, "setmatrix\n");
		for(i = 0; i < 6; i++) {
			textpart[i] = symbol->text[i + 1];
		}
		textpart[6] = '\0';
		fprintf(feps, "matrix currentmatrix\n");
		fprintf(feps, "/Helvetica findfont\n");
		fprintf(feps, "11.00 scalefont setfont\n");
		textpos = 24;
		fprintf(feps, " 0 0 moveto %.2f 0.50 translate 0.00 rotate 0 0 moveto\n", textpos + xoffset);
		fprintf(feps, " (%s) stringwidth\n", textpart);
		fprintf(feps, "pop\n");
		fprintf(feps, "-2 div 0 rmoveto\n");
		fprintf(feps, " (%s) show\n", textpart);
		fprintf(feps, "setmatrix\n");
		textpart[0] = symbol->text[7];
		textpart[1] = '\0';
		fprintf(feps, "matrix currentmatrix\n");
		fprintf(feps, "/Helvetica findfont\n");
		fprintf(feps, "8.00 scalefont setfont\n");
		textpos = 55;
		fprintf(feps, " 0 0 moveto %.2f 0.50 translate 0.00 rotate 0 0 moveto\n", textpos + xoffset);
		fprintf(feps, " (%s) stringwidth\n", textpart);
		fprintf(feps, "pop\n");
		fprintf(feps, "-2 div 0 rmoveto\n");
		fprintf(feps, " (%s) show\n", textpart);
		fprintf(feps, "setmatrix\n");
		textdone = 1;
		switch(strlen(addon)) {
			case 2:	
				fprintf(feps, "matrix currentmatrix\n");
				fprintf(feps, "/Helvetica findfont\n");
				fprintf(feps, "11.00 scalefont setfont\n");
				textpos = symbol->width + xoffset - 10;
				fprintf(feps, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos, addon_text_posn);
				fprintf(feps, " (%s) stringwidth\n", addon);
				fprintf(feps, "pop\n");
				fprintf(feps, "-2 div 0 rmoveto\n");
				fprintf(feps, " (%s) show\n", addon);
				fprintf(feps, "setmatrix\n");
				break;
			case 5:
				fprintf(feps, "matrix currentmatrix\n");
				fprintf(feps, "/Helvetica findfont\n");
				fprintf(feps, "11.00 scalefont setfont\n");
				textpos = symbol->width + xoffset - 23;
				fprintf(feps, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos, addon_text_posn);
				fprintf(feps, " (%s) stringwidth\n", addon);
				fprintf(feps, "pop\n");
				fprintf(feps, "-2 div 0 rmoveto\n");
				fprintf(feps, " (%s) show\n", addon);
				fprintf(feps, "setmatrix\n");
				break;
		}

	}

	xoffset -= comp_offset;


	/* Put boundary bars or box around symbol */
	if ((symbol->output_options == BARCODE_BOX) || (symbol->output_options == BARCODE_BIND)) {
		if(symbol->symbology != BARCODE_CODABLOCKF) {
			/* boundary bars */
			fprintf(feps, "TE\n");
			fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
			fprintf(feps, "%d.00 %d.00 TB %d.00 %d.00 TR\n", symbol->border_width, textoffset, 0, (symbol->width + xoffset + xoffset));
			fprintf(feps, "TE\n");
			fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
			fprintf(feps, "%d.00 %d.00 TB %d.00 %d.00 TR\n", symbol->border_width, textoffset + symbol->height + symbol->border_width, 0, (symbol->width + xoffset + xoffset));
			if(symbol->rows > 1) {
				/* row binding */
				for(r = 1; r < symbol->rows; r++) {
					fprintf(feps, "TE\n");
					fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
					fprintf(feps, "%d.00 %.2f TB %d.00 %d.00 TR\n", 2, ((r * row_height) + textoffset + yoffset - 1), xoffset, symbol->width);
				}
			}
		} else {
			/* boundary bars */
			fprintf(feps, "TE\n");
			fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
			fprintf(feps, "%d.00 %d.00 TB %d.00 %d.00 TR\n", symbol->border_width, textoffset, xoffset, symbol->width);
			fprintf(feps, "TE\n");
			fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
			fprintf(feps, "%d.00 %d.00 TB %d.00 %d.00 TR\n", symbol->border_width, textoffset + symbol->height + symbol->border_width, xoffset, symbol->width);
			if(symbol->rows > 1) {
				/* row binding */
				for(r = 1; r < symbol->rows; r++) {
					fprintf(feps, "TE\n");
					fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
					fprintf(feps, "%d.00 %.2f TB %d.00 %d.00 TR\n", 2, ((r * row_height) + textoffset + yoffset - 1), (xoffset + 11), (symbol->width - 24));
				}
			}
		}
	}
	
	if (symbol->output_options == BARCODE_BOX) {
		/* side bars */
		fprintf(feps, "TE\n");
		fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
		fprintf(feps, "%d.00 %d.00 TB %d.00 %d.00 TR\n", symbol->height + (2 * symbol->border_width), textoffset, 0, symbol->border_width); 
		fprintf(feps, "TE\n");
		fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
		fprintf(feps, "%d.00 %d.00 TB %d.00 %d.00 TR\n", symbol->height + (2 * symbol->border_width), textoffset, (symbol->width + xoffset + xoffset - symbol->border_width), symbol->border_width);
	}
	
	/* Put the human readable text at the bottom */
	if((textdone == 0) && (strlen(symbol->text) != 0)) {
		fprintf(feps, "TE\n");
		fprintf(feps, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
		fprintf(feps, "matrix currentmatrix\n");
		fprintf(feps, "/Helvetica findfont\n");
		fprintf(feps, "8.00 scalefont setfont\n");
		textpos = symbol->width / 2.0;
		fprintf(feps, " 0 0 moveto %.2f 1.67 translate 0.00 rotate 0 0 moveto\n", textpos + xoffset);
		fprintf(feps, " (%s) stringwidth\n", symbol->text);
		fprintf(feps, "pop\n");
		fprintf(feps, "-2 div 0 rmoveto\n");
		fprintf(feps, " (%s) show\n", symbol->text);
		fprintf(feps, "setmatrix\n");
	}
	fprintf(feps, "\nshowpage\n");
	
	fclose(feps);
	
	return error_number;
}

