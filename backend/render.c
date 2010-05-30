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

int render_plot(struct zint_symbol *symbol, unsigned int hide_text)
{
	struct zint_render      *render;
	struct zint_render_line *line, *last_line = NULL;
	struct zint_render_char *bchar, *last_bchar = NULL;

	int i, r, block_width, latch, this_row;
	float textpos, large_bar_height, preset_height, row_height, row_posn = 0.0;
	// int error_number = 0;
	int textoffset, textheight, xoffset, yoffset, textdone, main_width;
	char addon[6]; //, textpart[10];
	int large_bar_count, comp_offset;
	float addon_text_posn;
	float default_text_posn;
	float scaler = symbol->scale;
	unsigned char *p;

	// Allocate memory for the rendered version
	render = symbol->rendered = malloc(sizeof(struct zint_render));
	render->lines = NULL;
	render->chars = NULL;

	row_height = 0;
	textdone = 0;
	textpos = 0.0;
	main_width = symbol->width;
	strcpy(addon, "");
	comp_offset = 0;
	addon_text_posn = 0.0;

	if (symbol->height < 15) {
		symbol->height = 15;
	}
	// symbol->height = 50;

	if(!hide_text && strlen((char *) symbol->text)) {
		textheight = 9.0;
		textoffset = 2.0;
	} else {
		textheight = textoffset = 0.0;
	}
	// Update height for texts
	symbol->height -= textheight + textoffset;

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
	
	while(!(module_is_set(symbol, symbol->rows - 1, comp_offset))) {
		comp_offset++;
	}

	xoffset = symbol->border_width + symbol->whitespace_width;
	yoffset = symbol->border_width;

	render->width = (symbol->width + xoffset + xoffset) * scaler;
	render->height = (symbol->height + textheight + textoffset + yoffset + yoffset) * scaler;

	default_text_posn = (symbol->height + textoffset + symbol->border_width) * scaler;

	if(symbol->symbology != BARCODE_MAXICODE) {
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
					line->x = (i + xoffset) * scaler;
				
					if(addon_latch == 0) {
						line->y = row_posn * scaler;
						line->length = row_height * scaler;
					} else {
						line->y = (row_posn + 10.0) * scaler;
						line->length = (row_height - 5.0) * scaler;
					}
					latch = 0;
					
					if (last_line)
						last_line->next = line;
					else
						render->lines = line; // first line
				  last_line = line;
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

	if (!hide_text) {
		// caculate start xoffset to center text
		xoffset = symbol->width / 2.0;
		xoffset -= (strlen((char *) symbol->text) / 2) * 5.0;

		for (p = symbol->text; *p != 0; p++) {
			if (p != symbol->text && *p == '(') xoffset += 3.0;

			bchar = malloc(sizeof(struct zint_render_char));
			bchar->next = NULL;
			bchar->x = (textpos + xoffset) * scaler;
			bchar->y = default_text_posn;
			bchar->fsize = 8.0 * scaler;
			bchar->c = (char) *p;

			if (last_bchar)
				last_bchar->next = bchar;
			else
				render->chars = bchar; // First character
			last_bchar = bchar;

			// Poor mans kerning for next character
			if (*p == '(') {
				xoffset += 3.0;
			} else if (*p == ')') {
				xoffset += 3.0;
			} else {
				xoffset += 5.0;
			}
		}
	}

	return 1;
}
