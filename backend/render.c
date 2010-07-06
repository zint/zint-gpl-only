/* 
 * render.c - Generic Rendered Format 
 *
 * Initiall written by Sam Lown for use in gLabels. Converts encoded
 * data into a generic internal structure of lines and characters
 * usable in external applications.
 */

/*
    libzint - the open source barcode library
    Copyright (C) 2009 Robin Stuart <robin@zint.org.uk>

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

#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

struct zint_render_line *render_plot_create_line(float x, float y, float width, float length);
int render_plot_add_line(struct zint_symbol *symbol, struct zint_render_line *line, struct zint_render_line **last_line);

int render_plot_add_string(struct zint_symbol *symbol, unsigned char *text, float x, float y, float fsize, float width, struct zint_render_string **last_string);

int render_plot(struct zint_symbol *symbol, float width, float height)
{
	struct zint_render        *render;
	struct zint_render_line   *line, *last_line = NULL;
	struct zint_render_string *last_string = NULL;

	int i, r, block_width, latch, this_row;
	float textpos, textwidth, large_bar_height, preset_height, row_height, row_posn = 0.0;
	// int error_number = 0;
	int textoffset, textheight, xoffset, yoffset, textdone, main_width;
	char addon[6], textpart[10];
	int large_bar_count, comp_offset;
	float addon_text_posn;
	float default_text_posn;
	float scaler;
	const char *locale = NULL;
	int hide_text = 0;
	float required_aspect;
	float symbol_aspect;
	float x_spacer, y_spacer;

	// Allocate memory for the rendered version
	render = symbol->rendered = malloc(sizeof(struct zint_render));
	render->lines = NULL;
	render->strings = NULL;

	locale = setlocale(LC_ALL, "C");

	row_height = 0;
	textdone = 0;
	textpos = 0.0;
	main_width = symbol->width;
	strcpy(addon, "");
	comp_offset = 0;
	addon_text_posn = 0.0;

  /*
   * Determine if there will be any addon texts and text height 
   */
	latch = 0;
	r = 0;
	/* Isolate add-on text */
	if(is_extendable(symbol->symbology)) {
		for(i = 0; i < ustrlen(symbol->text); i++) {
			if (latch == 1) {
				addon[r] = symbol->text[i];
				r++;
			}
			if (symbol->text[i] == '+') {
				latch = 1;
			}
		}
	}
	addon[r] = '\0';
	
	if((!symbol->show_hrt) || (ustrlen(symbol->text) == 0)) {
		hide_text = 1;
		textheight = textoffset = 0.0;
	} else {
		textheight = 9.0;
		textoffset = 2.0;
	}


  /*
   * Calculate the width of the barcode, especially if there are any extra
   * borders or white space to add.
   */
	
	while(!(module_is_set(symbol, symbol->rows - 1, comp_offset))) {
		comp_offset++;
	}

	/* Certain symbols need whitespace otherwise characters get chopped off the sides */
	if ((((symbol->symbology == BARCODE_EANX) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_EANX_CC))
		|| (symbol->symbology == BARCODE_ISBNX)) {
		switch(ustrlen(symbol->text)) {
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

	xoffset = symbol->border_width + symbol->whitespace_width;
	yoffset = symbol->border_width;

	// Determine if height should be overridden
	large_bar_count = 0;
	preset_height = 0.0;
	for(i = 0; i < symbol->rows; i++) {
		preset_height += symbol->row_height[i];
		if(symbol->row_height[i] == 0) {
			large_bar_count++;
		}
	}

	if (large_bar_count == 0) {
		required_aspect = width / height;
		symbol_aspect = (main_width + (2 * xoffset)) / (preset_height + (2 * yoffset) + textoffset + textheight);
		symbol->height = preset_height;
		if (required_aspect > symbol_aspect) {
			/* horizontal padding is required */
			scaler = height / (preset_height + (2 * yoffset) + textoffset + textheight);
			x_spacer = ((width / scaler) - (main_width + (2 * xoffset))) / 2;
			y_spacer = 0.0;
		} else {
			/* vertical padding is required */
			scaler = width / (main_width + (2 * xoffset));
			y_spacer = ((height / scaler) - (preset_height + (2 * yoffset) + textoffset + textheight)) / 2;
			x_spacer = 0.0;
		}
	} else {
		scaler = width / (main_width + (2 * xoffset));
		symbol->height = (height / scaler) - ((2 * yoffset) + textoffset + textheight);
		
		x_spacer = 0.0;
		y_spacer = 0.0;
	}
	large_bar_height = (symbol->height - preset_height) / large_bar_count;
	
	// Set initial render dimensions
	render->width = width;
	render->height = height;

	if(((symbol->output_options & BARCODE_BOX) != 0) || ((symbol->output_options & BARCODE_BIND) != 0)) {
		default_text_posn = (symbol->height + textoffset + symbol->border_width + symbol->border_width) * scaler;
	} else {
		default_text_posn = (symbol->height + textoffset + symbol->border_width) * scaler;
	}
	// SAMS ORIGINAL // default_text_posn = (symbol->height + textoffset + symbol->border_width) * scaler;

	if(symbol->symbology == BARCODE_MAXICODE) {
		/*
		 * TODO INSERT BARCODE_MAXICODE HERE!
		 */
	
	} else {
		/* everything else uses rectangles (or squares) */
		/* Works from the bottom of the symbol up */
		int addon_latch = 0;
		
		for(r = 0; r < symbol->rows; r++) {
			this_row = r;
			if(symbol->row_height[this_row] == 0) {
				row_height = large_bar_height;
			} else {
				row_height = symbol->row_height[this_row];
			}
			row_posn = 0;
			for(i = 0; i < r; i++) {
				if(symbol->row_height[i] == 0) {
					row_posn += large_bar_height;
				} else {
					row_posn += symbol->row_height[i];
				}
			}
			row_posn += yoffset;
			
			i = 0;
			if(module_is_set(symbol, this_row, 0)) {
				latch = 1;
			} else {
				latch = 0;
			}

			do {
				block_width = 0;
				do {
					block_width++;
				} while (module_is_set(symbol, this_row, i + block_width) == module_is_set(symbol, this_row, i));
				if((addon_latch == 0) && (r == (symbol->rows - 1)) && (i > main_width)) {
					addon_text_posn = (row_posn + 8.0) * scaler;
					addon_latch = 1;
				} 
				if(latch == 1) {
					/* a bar */
					line = malloc(sizeof(struct zint_render_line));
					line->next = NULL;
					
					line->width = block_width * scaler;
					line->x = (i + xoffset + x_spacer) * scaler;
				
					if(addon_latch == 0) {
						line->y = (row_posn + y_spacer) * scaler;
						line->length = row_height * scaler;
					} else {
						line->y = (row_posn + 10.0 + y_spacer) * scaler;
						line->length = (row_height - 5.0) * scaler;
					}
					latch = 0;
					
					render_plot_add_line(symbol, line, &last_line);
				} else {
					/* a space */
					latch = 1;
				}
				i += block_width;
				
			} while (i < symbol->width);
		}
	}
	/* That's done the actual data area, everything else is human-friendly */


	/* Add the text */
	xoffset -= comp_offset;
	row_posn = (row_posn + large_bar_height) * scaler;

	if (!hide_text) {
		if ((((symbol->symbology == BARCODE_EANX) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_EANX_CC)) ||
			(symbol->symbology == BARCODE_ISBNX)) {
			/* guard bar extensions and text formatting for EAN8 and EAN13 */
			switch(ustrlen(symbol->text)) {
				case 8: /* EAN-8 */
				case 11:
				case 14:
					line = render_plot_create_line((0 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
					render_plot_add_line(symbol, line, &last_line);
					line = render_plot_create_line((2 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
					render_plot_add_line(symbol, line, &last_line);
					line = render_plot_create_line((32 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
					render_plot_add_line(symbol, line, &last_line);
					line = render_plot_create_line((34 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
					render_plot_add_line(symbol, line, &last_line);
					line = render_plot_create_line((64 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
					render_plot_add_line(symbol, line, &last_line);
					line = render_plot_create_line((66 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
					render_plot_add_line(symbol, line, &last_line);

					for(i = 0; i < 4; i++) {
						textpart[i] = symbol->text[i];
					}
					textpart[4] = '\0';
					textpos = 17;
					textwidth = 28.0; 
					render_plot_add_string(symbol, (unsigned char *) textpart, (textpos + xoffset) * scaler, default_text_posn, 9.0 * scaler, textwidth * scaler, &last_string);
					for(i = 0; i < 4; i++) {
						textpart[i] = symbol->text[i + 4];
					}
					textpart[4] = '\0';
					textpos = 50;
					render_plot_add_string(symbol, (unsigned char *) textpart, (textpos + xoffset) * scaler, default_text_posn, 9.0 * scaler, textwidth * scaler, &last_string);
					textdone = 1;
					switch(strlen(addon)) {
						case 2:	
							textpos = xoffset + 86;
							textwidth = 2.0 * 7.0;
					    render_plot_add_string(symbol, (unsigned char *) addon, textpos * scaler, addon_text_posn * scaler, 9.0 * scaler, textwidth * scaler, &last_string);
							break;
						case 5:
							textpos = xoffset + 100;
							textwidth = 5.0 * 7.0;
					    render_plot_add_string(symbol, (unsigned char *) addon, textpos * scaler, addon_text_posn * scaler, 9.0 * scaler, textwidth * scaler, &last_string);
							break;
					}

					break;
				case 13: /* EAN 13 */
				case 16:
				case 19:
					line = render_plot_create_line((0 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
					render_plot_add_line(symbol, line, &last_line);
					line = render_plot_create_line((2 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
					render_plot_add_line(symbol, line, &last_line);
					line = render_plot_create_line((46 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
					render_plot_add_line(symbol, line, &last_line);
					line = render_plot_create_line((48 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
					render_plot_add_line(symbol, line, &last_line);
					line = render_plot_create_line((92 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
					render_plot_add_line(symbol, line, &last_line);
					line = render_plot_create_line((94 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler); 
					render_plot_add_line(symbol, line, &last_line);

					textpart[0] = symbol->text[0];
					textpart[1] = '\0';
					textpos = -5; // 7
					textwidth = 7.0;
					render_plot_add_string(symbol, (unsigned char *) textpart, (textpos + xoffset) * scaler, default_text_posn, 9.0 * scaler, textwidth * scaler, &last_string);

					for(i = 0; i < 6; i++) {
						textpart[i] = symbol->text[i + 1];
					}
					textpart[6] = '\0';
					textpos = 25;
					textwidth = 6.0 * 7.0;
					render_plot_add_string(symbol, (unsigned char *) textpart, (textpos + xoffset) * scaler, default_text_posn, 9.0 * scaler, textwidth * scaler, &last_string);
					for(i = 0; i < 6; i++) {
						textpart[i] = symbol->text[i + 7];
					}
					textpart[6] = '\0';
					textpos = 72;
					render_plot_add_string(symbol, (unsigned char *) textpart, (textpos + xoffset) * scaler, default_text_posn, 9.0 * scaler, textwidth * scaler, &last_string);
					textdone = 1;
					switch(strlen(addon)) {
						case 2:	
							textpos = xoffset + 114;
							textwidth = 2.0 * 7.0;
							render_plot_add_string(symbol, (unsigned char *) addon, textpos * scaler, addon_text_posn * scaler, 9.0 * scaler, textwidth * scaler, &last_string);
							break;
						case 5:
							textpos = xoffset + 128;
							textwidth = 5.0 * 7.0;
							render_plot_add_string(symbol, (unsigned char *) addon, textpos * scaler, addon_text_posn * scaler, 9.0 * scaler, textwidth * scaler, &last_string);
							break;
					}
					break;
			}
		}
		
		if (((symbol->symbology == BARCODE_UPCA) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCA_CC)) {
			/* guard bar extensions and text formatting for UPCA */
			latch = 1;
			
			i = 0 + comp_offset;
			do {
				block_width = 0;
				do {
					block_width++;
				} while (module_is_set(symbol, symbol->rows - 1, i + block_width) == module_is_set(symbol, symbol->rows - 1, i));
				if(latch == 1) {
					/* a bar */
					line = render_plot_create_line((i + xoffset - comp_offset) * scaler, row_posn, block_width * scaler, 5.0 * scaler);
					render_plot_add_line(symbol, line, &last_line);
					latch = 0;
				} else {
					/* a space */
					latch = 1;
				}
				i += block_width;
			} while (i < 11 + comp_offset);
			line = render_plot_create_line((46 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
			render_plot_add_line(symbol, line, &last_line);
			line = render_plot_create_line((48 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
			render_plot_add_line(symbol, line, &last_line);
			latch = 1;
			i = 85 + comp_offset;
			do {
				block_width = 0;
				do {
					block_width++;
				} while (module_is_set(symbol, symbol->rows - 1, i + block_width) == module_is_set(symbol, symbol->rows - 1, i));
				if(latch == 1) {
					/* a bar */
					line = render_plot_create_line((i + xoffset - comp_offset) * scaler, row_posn, block_width * scaler, 5.0 * scaler);
					render_plot_add_line(symbol, line, &last_line);
					latch = 0;
				} else {
					/* a space */
					latch = 1;
				}
				i += block_width;
			} while (i < 96 + comp_offset);
			textpart[0] = symbol->text[0];
			textpart[1] = '\0';
			textpos = -5;
			textwidth = 7.0;
			render_plot_add_string(symbol, (unsigned char *) textpart, (textpos + xoffset) * scaler, default_text_posn, 9.0 * scaler, textwidth * scaler, &last_string);
			for(i = 0; i < 5; i++) {
				textpart[i] = symbol->text[i + 1];
			}
			textpart[5] = '\0';
			textpos = 27;
			textwidth = 4.0 * 7.0;
			render_plot_add_string(symbol, (unsigned char *) textpart, (textpos + xoffset) * scaler, default_text_posn, 9.0 * scaler, textwidth * scaler, &last_string);
			for(i = 0; i < 5; i++) {
				textpart[i] = symbol->text[i + 6];
			}
			textpos = 68;
			render_plot_add_string(symbol, (unsigned char *) textpart, (textpos + xoffset) * scaler, default_text_posn, 9.0 * scaler, textwidth * scaler, &last_string);
			textpart[0] = symbol->text[11];
			textpart[1] = '\0';
			textpos = 100;
			textwidth = 7.0;
			render_plot_add_string(symbol, (unsigned char *) textpart, (textpos + xoffset) * scaler, default_text_posn, 9.0 * scaler, textwidth * scaler, &last_string);
			textdone = 1;
			switch(strlen(addon)) {
				case 2:	
					textpos = xoffset + 116;
					textwidth = 2.0 * 7.0;
					render_plot_add_string(symbol, (unsigned char *) textpart, (textpos + xoffset) * scaler, default_text_posn, 9.0 * scaler, textwidth * scaler, &last_string);
					break;
				case 5:
					textpos = xoffset + 130;
					textwidth = 5.0 * 7.0;
					render_plot_add_string(symbol, (unsigned char *) textpart, (textpos + xoffset) * scaler, default_text_posn, 9.0 * scaler, textwidth * scaler, &last_string);
					break;
			}
		}

		/* Put normal human readable text at the bottom (and centered) */
		if (textdone == 0) {
			// caculate start xoffset to center text
			render_plot_add_string(symbol, symbol->text, (symbol->width / 2.0) * scaler, default_text_posn, 9.0 * scaler, 0.0, &last_string); 
		}
	}

	if (locale)
		setlocale(LC_ALL, locale);

	return 1;
}


/*
 * Create a new line with its memory allocated ready for adding to the 
 * rendered structure.
 *
 * This is much quicker than writing out each line manually (in some cases!)
 */
struct zint_render_line *render_plot_create_line(float x, float y, float width, float length)
{
	struct zint_render_line *line;
	
	line = malloc(sizeof(struct zint_render_line));
	line->next = NULL;
	line->x = x;
	line->y = y;
	line->width = width;
	line->length = length;	

	return line;
}

/*
 * Add the line to the current rendering and update the last line's 
 * next value.
 */
int render_plot_add_line(struct zint_symbol *symbol, struct zint_render_line *line, struct zint_render_line **last_line)
{
	if (*last_line)
		(*last_line)->next = line;
	else
		symbol->rendered->lines = line; // first line

	*last_line = line;
	return 1;
}

/*
 * Add a string structure to the symbol.
 * Coordinates assumed to be from top-center.
 */
int render_plot_add_string(struct zint_symbol *symbol,
		unsigned char *text, float x, float y, float fsize, float width,
		struct zint_render_string **last_string)
{
	struct zint_render_string *string;

	string = malloc(sizeof(struct zint_render_string));
	string->next = NULL;
	string->x = x;
	string->y = y;
	string->width = width; 
	string->fsize = fsize;
	string->length = ustrlen(text);
	string->text = malloc(sizeof(unsigned char) * (ustrlen(text) + 1));
	ustrcpy(string->text, text);

	if (*last_string)
		(*last_string)->next = string;
	else
		symbol->rendered->strings = string; // First character
	*last_string = string;

	return 1;
}
