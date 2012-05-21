/* code16k.c - Handles Code 16k stacked symbology */

/*
    libzint - the open source barcode library
    Copyright (C) 2008 Robin Stuart <robin@zint.org.uk>

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

/* Updated to comply with BS EN 12323:2005 */

/* up to 77 characters or 154 numbers */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

#define TRUE 1
#define FALSE 0
#define SHIFTA 90
#define LATCHA 91
#define SHIFTB 92
#define LATCHB 93
#define SHIFTC 94
#define LATCHC 95
#define AORB 96
#define ABORC 97
#define CANDB 98
#define CANDBB 99

int list[2][170];

/* EN 12323 Table 1 - "Code 16K" character encodations */
const char *C16KTable[107] = {"212222", "222122", "222221", "121223", "121322", "131222", "122213",
	"122312", "132212", "221213", "221312", "231212", "112232", "122132", "122231", "113222",
	"123122", "123221", "223211", "221132", "221231", "213212", "223112", "312131", "311222",
	"321122", "321221", "312212", "322112", "322211", "212123", "212321", "232121", "111323",
	"131123", "131321", "112313", "132113", "132311", "211313", "231113", "231311", "112133",
	"112331", "132131", "113123", "113321", "133121", "313121", "211331", "231131", "213113",
	"213311", "213131", "311123", "311321", "331121", "312113", "312311", "332111", "314111",
	"221411", "431111", "111224", "111422", "121124", "121421", "141122", "141221", "112214",
	"112412", "122114", "122411", "142112", "142211", "241211", "221114", "413111", "241112",
	"134111", "111242", "121142", "121241", "114212", "124112", "124211", "411212", "421112",
	"421211", "212141", "214121", "412121", "111143", "111341", "131141", "114113", "114311",
	"411113", "411311", "113141", "114131", "311141", "411131", "211412", "211214", "211232",
	"211133"};

/* EN 12323 Table 3 and Table 4 - Start patterns and stop patterns */
const char *C16KStartStop[8] = {"3211", "2221", "2122", "1411", "1132", "1231", "1114", "3112"};

/* EN 12323 Table 5 - Start and stop values defining row numbers */
const int C16KStartValues[16] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7};
const int C16KStopValues[16] = {0, 1, 2, 3, 4, 5, 6, 7, 4, 5, 6, 7, 0, 1, 2, 3};

void grwp16(int *indexliste)
{
	/* bring together same type blocks */
	if(*(indexliste) > 1) {
		for (int i = 1; i < *indexliste; i++) {
			if(list[1][i - 1] == list[1][i]) {
				/* bring together */
				list[0][i - 1] = list[0][i - 1] + list[0][i];

				/* decreace the list */
				for (int j = i + 1 ;j < *indexliste; j++) {
					list[0][j - 1] = list[0][j];
					list[1][j - 1] = list[1][j];
				}
				(*indexliste)--;
				i--;
			}
		}
	}
}

void dxsmooth16(int *indexliste)
{ /* Implements rules from ISO 15417 Annex E */
	int current, last, next, length;

	for(int i = 0; i < *indexliste; i++) {
		current = list[1][i];
		length = list[0][i];

		if (i != 0)
			last = list[1][i - 1];
		else
			last = FALSE;

		if (i != *indexliste - 1)
			next = list[1][i + 1];
		else
			next = FALSE;

		if(i == 0) { /* first block */
			if ((*(indexliste) == 1) && ((length == 2) && (current == ABORC)))
				/* Rule 1a */
				list[1][i] = LATCHC;
			if (current == ABORC) {
				if(length >= 4)
					/* Rule 1b */
					list[1][i] = LATCHC;
				else
					list[1][i] = current = AORB;
			}
			if (current == SHIFTA)
				/* Rule 1c */
				list[1][i] = LATCHA;
			if ((current == AORB) && (next == SHIFTA))
				/* Rule 1c */
				list[1][i] = current = LATCHA;
			if (current == AORB)
				/* Rule 1d */
				list[1][i] = LATCHB;
		} else {
			if ((current == ABORC) && (length >= 4))
				/* Rule 3 */
				list[1][i] = current = LATCHC;
			if (current == ABORC)
				list[1][i] = current = AORB;
			if ((current == AORB) && (last == LATCHA))
				list[1][i] = current = LATCHA;
			if ((current == AORB) && (last == LATCHB))
				list[1][i] = current = LATCHB;
			if ((current == AORB) && (next == SHIFTA))
				list[1][i] = current = LATCHA;
			if((current == AORB) && (next == SHIFTB))
				list[1][i] = current = LATCHB;
			if (current == AORB)
				list[1][i] = current = LATCHB;
			if ((current == SHIFTA) && (length > 1))
				/* Rule 4 */
				list[1][i] = current = LATCHA;
			if ((current == SHIFTB) && (length > 1))
				/* Rule 5 */
				list[1][i] = current = LATCHB;
			if ((current == SHIFTA) && (last == LATCHA))
				list[1][i] = current = LATCHA;
			if ((current == SHIFTB) && (last == LATCHB))
				list[1][i] = current = LATCHB;
			if ((current == SHIFTA) && (last == LATCHC))
				list[1][i] = current = LATCHA;
			if ((current == SHIFTB) && (last == LATCHC))
				list[1][i] = current = LATCHB;
		} /* Rule 2 is implimented elsewhere, Rule 6 is implied */
	}
	grwp16(indexliste);
}

void c16k_set_a(unsigned char source, unsigned int values[], unsigned int *bar_chars)
{
	source &= 127; /* limit the range to 0-127 */
	if(source < 32)
		source += 64;
	else
		source -= 32;
	values[(*bar_chars)++] = source;
}

void c16k_set_b(unsigned char source, unsigned int values[], unsigned int *bar_chars)
{
	source &= 127; /* limit the range to 0-127 */
	values[(*bar_chars)++] = source - 32;
}

void c16k_set_c(unsigned char source_a, unsigned char source_b, unsigned int values[], unsigned int *bar_chars)
{
	int weight;

	weight = (10 * ctoi(source_a)) + ctoi(source_b);
	values[(*bar_chars)++] = weight;
}

int code16k(struct zint_symbol *symbol, unsigned char source[], int length)
{
	char width_pattern[100];
	int current_row, rows_needed, flip_flop, looper, first_check, second_check;
	int indexliste, indexchaine, pads_needed, f_state;
	char set[160] = { ' ' }, fset[160] = { ' ' }, mode, last_set, current_set;
	unsigned int i, j, k, m, read, mx_reader, writer;
	unsigned int values[160] = { 0 };
	unsigned int bar_characters;
	float glyph_count;
	int errornum, first_sum, second_sum;
	int input_length;
	int gs1, c_count;

	errornum = 0;
        strcpy(width_pattern, "");
        input_length = length;

	if(symbol->input_mode == GS1_MODE) { gs1 = 1; } else { gs1 = 0; }

	if(input_length > 157) {
		strcpy(symbol->errtxt, "Input too long");
		return ERROR_TOO_LONG;
	}

	bar_characters = 0;

	/* Detect extended ASCII characters */
	for(i = 0; i <  input_length; i++) {
		if(source[i] >=128) {
			fset[i] = 'f';
		}
	}
	fset[i] = '\0';

	/* Decide when to latch to extended mode */
	for(i = 0; i < input_length; i++) {
		j = 0;
		if(fset[i] == 'f') {
			do {
				j++;
			} while(fset[i + j] == 'f');
			if((j >= 5) || ((j >= 3) && ((i + j) == (input_length - 1)))) {
				for(k = 0; k <= j; k++) {
					fset[i + k] = 'F';
				}
			}
		}
	}

	/* Decide if it is worth reverting to 646 encodation for a few characters */
	if(input_length > 1) {
		for(i = 1; i < input_length; i++) {
			if((fset[i - 1] == 'F') && (fset[i] == ' ')) {
				/* Detected a change from 8859-1 to 646 - count how long for */
				for(j = 0; (fset[i + j] == ' ') && ((i + j) < input_length); j++);
				if((j < 5) || ((j < 3) && ((i + j) == (input_length - 1)))) {
					/* Change to shifting back rather than latching back */
					for(k = 0; k < j; k++) {
						fset[i + k] = 'n';
					}
				}
			}
		}
	}
	/* Detect mode A, B and C characters */
	indexliste = 0;
	indexchaine = 0;

	mode = parunmodd(source[indexchaine]);
	if((gs1) && (source[indexchaine] == '[')) { mode = ABORC; } /* FNC1 */

	for(i = 0; i < 160; i++) {
		list[0][i] = 0;
	}

	do {
		list[1][indexliste] = mode;
		while ((list[1][indexliste] == mode) && (indexchaine < input_length)) {
			list[0][indexliste]++;
			indexchaine++;
			mode = parunmodd(source[indexchaine]);
			if((gs1) && (source[indexchaine] == '[')) { mode = ABORC; } /* FNC1 */
		}
		indexliste++;
	} while (indexchaine < input_length);

	dxsmooth16(&indexliste);

	/* Put set data into set[] */
	read = 0;
	for(i = 0; i < indexliste; i++) {
		for(j = 0; j < list[0][i]; j++) {
			switch(list[1][i]) {
				case SHIFTA: set[read] = 'a'; break;
				case LATCHA: set[read] = 'A'; break;
				case SHIFTB: set[read] = 'b'; break;
				case LATCHB: set[read] = 'B'; break;
				case LATCHC: set[read] = 'C'; break;
			}
			read++;
		}
	}

	/* Adjust for strings which start with shift characters - make them latch instead */
	if(set[0] == 'a') {
		i = 0;
		do {
			set[i] = 'A';
			i++;
		} while (set[i] == 'a');
	}

	if(set[0] == 'b') {
		i = 0;
		do {
			set[i] = 'B';
			i++;
		} while (set[i] == 'b');
	}

	/* Watch out for odd-length Mode C blocks */
	c_count = 0;
	for(i = 0; i < read; i++) {
		if(set[i] == 'C') {
			if(source[i] == '[') {
				if(c_count & 1) {
					if((i - c_count) != 0) {
						set[i - c_count] = 'B';
					} else {
						set[i - 1] = 'B';
					}
				}
				c_count = 0;
			} else {
				c_count++;
			}
		} else {
			if(c_count & 1) {
				if((i - c_count) != 0) {
					set[i - c_count] = 'B';
				} else {
					set[i - 1] = 'B';
				}
			}
			c_count = 0;
		}
	}
	if(c_count & 1) {
		if((i - c_count) != 0) {
			set[i - c_count] = 'B';
		} else {
			set[i - 1] = 'B';
		}
	}
	for(i = 1; i < read - 1; i++) {
		if((set[i] == 'C') && ((set[i - 1] == 'B') && (set[i + 1] == 'B'))) {
			set[i] = 'B';
		}
	}

	/* Make sure the data will fit in the symbol */
	last_set = ' ';
	glyph_count = 0.0;
	for(i = 0; i < input_length; i++) {
		if((set[i] == 'a') || (set[i] == 'b')) {
			glyph_count = glyph_count + 1.0;
		}
		if((fset[i] == 'f') || (fset[i] == 'n')) {
			glyph_count = glyph_count + 1.0;
		}
		if(((set[i] == 'A') || (set[i] == 'B')) || (set[i] == 'C')) {
			if(set[i] != last_set) {
				last_set = set[i];
				glyph_count = glyph_count + 1.0;
			}
		}
		if(i == 0) {
			if((set[i] == 'B') && (set[1] == 'C')) {
				glyph_count = glyph_count - 1.0;
			}
			if((set[i] == 'B') && (set[1] == 'B')) {
				if(set[2] == 'C') {
					glyph_count = glyph_count - 1.0;
				}
			}
			if(fset[i] == 'F') {
				glyph_count = glyph_count + 2.0;
			}
		} else {
			if((fset[i] == 'F') && (fset[i - 1] != 'F')) {
				glyph_count = glyph_count + 2.0;
			}
			if((fset[i] != 'F') && (fset[i - 1] == 'F')) {
				glyph_count = glyph_count + 2.0;
			}
		}

		if((set[i] == 'C') && (!((gs1) && (source[i] == '[')))) {
			glyph_count = glyph_count + 0.5;
		} else {
			glyph_count = glyph_count + 1.0;
		}
	}

	if((gs1) && (set[0] != 'A')) {
		/* FNC1 can be integrated with mode character */
		glyph_count--;
	}

	if(glyph_count > 77.0) {
		strcpy(symbol->errtxt, "Input too long");
		return ERROR_TOO_LONG;
	}

	/* Calculate how tall the symbol will be */
	glyph_count = glyph_count + 2.0;
	i = glyph_count;
	rows_needed = (i/5);
	if(i%5 > 0) { rows_needed++; }

	if(rows_needed == 1) {
		rows_needed = 2;
	}

	/* start with the mode character - Table 2 */
	m = 0;
	switch(set[0]) {
		case 'A': m = 0; break;
		case 'B': m = 1; break;
		case 'C': m = 2; break;
	}

	if(symbol->output_options & READER_INIT) {
		if(m == 2) { m = 5; }
		if(gs1) {
			strcpy(symbol->errtxt, "Cannot use both GS1 mode and Reader Initialisation");
			return ERROR_INVALID_OPTION;
		} else {
			if((set[0] == 'B') && (set[1] == 'C')) { m = 6; }
		}
		values[bar_characters] = (7 * (rows_needed - 2)) + m; /* see 4.3.4.2 */
		values[bar_characters + 1] = 96; /* FNC3 */
		bar_characters += 2;
	} else {
		if(gs1) {
			/* Integrate FNC1 */
			switch(set[0]) {
				case 'B': m = 3; break;
				case 'C': m = 4; break;
			}
		} else {
			if((set[0] == 'B') && (set[1] == 'C')) { m = 5; }
			if(((set[0] == 'B') && (set[1] == 'B')) && (set[2] == 'C')) { m = 6; }
		}
		values[bar_characters] = (7 * (rows_needed - 2)) + m; /* see 4.3.4.2 */
		bar_characters++;
	}

	current_set = set[0];
	f_state = 0; /* f_state remembers if we are in Extended ASCII mode (value 1) or
	in ISO/IEC 646 mode (value 0) */
	if(fset[0] == 'F') {
		switch(current_set) {
			case 'A':
				values[bar_characters] = 101;
				values[bar_characters + 1] = 101;
				break;
			case 'B':
				values[bar_characters] = 100;
				values[bar_characters + 1] = 100;
				break;
		}
		bar_characters += 2;
		f_state = 1;
	}

	read = 0;

	/* Encode the data */
	do {

		if((read != 0) && (set[read] != set[read - 1]))
		{ /* Latch different code set */
			switch(set[read])
			{
				case 'A':
					values[bar_characters] = 101;
					bar_characters++;
					current_set = 'A';
					break;
				case 'B':
					values[bar_characters] = 100;
					bar_characters++;
					current_set = 'B';
					break;
				case 'C':
					if(!((read == 1) && (set[0] == 'B'))) { /* Not Mode C/Shift B */
						if(!((read == 2) && ((set[0] == 'B') && (set[1] == 'B')))) {
							/* Not Mode C/Double Shift B */
							values[bar_characters] = 99;
							bar_characters++;
						}
					}
					current_set = 'C';
					break;
			}
		}
		/* printf("tp8\n"); */
		if(read != 0) {
			if((fset[read] == 'F') && (f_state == 0)) {
				/* Latch beginning of extended mode */
				switch(current_set) {
					case 'A':
						values[bar_characters] = 101;
						values[bar_characters + 1] = 101;
						break;
					case 'B':
						values[bar_characters] = 100;
						values[bar_characters + 1] = 100;
						break;
				}
				bar_characters += 2;
				f_state = 1;
			}
			if((fset[read] == ' ') && (f_state == 1)) {
				/* Latch end of extended mode */
				switch(current_set) {
					case 'A':
						values[bar_characters] = 101;
						values[bar_characters + 1] = 101;
						break;
					case 'B':
						values[bar_characters] = 100;
						values[bar_characters + 1] = 100;
						break;
				}
				bar_characters += 2;
				f_state = 0;
			}
		}

		if((fset[i] == 'f') || (fset[i] == 'n')) {
			/* Shift extended mode */
			switch(current_set) {
				case 'A':
					values[bar_characters] = 101; /* FNC 4 */
					break;
				case 'B':
					values[bar_characters] = 100; /* FNC 4 */
					break;
			}
			bar_characters++;
		}

		if((set[i] == 'a') || (set[i] == 'b')) {
			/* Insert shift character */
			values[bar_characters] = 98;
			bar_characters++;
		}

		if(!((gs1) && (source[read] == '['))) {
			switch(set[read])
			{ /* Encode data characters */
				case 'A':
				case 'a':
					c16k_set_a(source[read], values, &bar_characters);
					read++;
					break;
				case 'B':
				case 'b':
					c16k_set_b(source[read], values, &bar_characters);
					read++;
					break;
				case 'C': c16k_set_c(source[read], source[read + 1], values, &bar_characters);
					read += 2;
					break;
			}
		} else {
			values[bar_characters] = 102;
			bar_characters++;
			read++;
		}
		/* printf("tp9 read=%d surrent set=%c\n", read, set[read]); */
	} while (read < ustrlen(source));

	pads_needed = 5 - ((bar_characters + 2) % 5);
	if(pads_needed == 5) {
		pads_needed = 0;
	}
	if((bar_characters + pads_needed) < 8) {
		pads_needed += 8 - (bar_characters + pads_needed);
	}
	for(i = 0; i < pads_needed; i++) {
		values[bar_characters] = 106;
		bar_characters++;
	}

	/* Calculate check digits */
	first_sum = 0;
	second_sum = 0;
	for(i = 0; i < bar_characters; i++)
	{
		first_sum += (i+2) * values[i];
		second_sum += (i+1) * values[i];
	}
	first_check = first_sum % 107;
	second_sum += first_check * (bar_characters + 1);
	second_check = second_sum % 107;
	values[bar_characters] = first_check;
	values[bar_characters + 1] =  second_check;
	bar_characters += 2;

	for(current_row = 0; current_row < rows_needed; current_row++) {

		strcpy(width_pattern, "");
		concat(width_pattern, C16KStartStop[C16KStartValues[current_row]]);
		concat(width_pattern, "1");
		for(i = 0; i < 5; i++) {
			concat(width_pattern, C16KTable[values[(current_row * 5) + i]]);
			/* printf("[%d] ", values[(current_row * 5) + i]); */

		}
		concat(width_pattern, C16KStartStop[C16KStopValues[current_row]]);
		/* printf("\n"); */

		/* Write the information into the symbol */
		writer = 0;
		flip_flop = 1;
		for (mx_reader = 0; mx_reader < strlen(width_pattern); mx_reader++) {
			for(looper = 0; looper < ctoi(width_pattern[mx_reader]); looper++) {
				if(flip_flop == 1) {
					set_module(symbol, current_row, writer);
					writer++; }
				else {
					writer++; }
			}
			if(flip_flop == 0) { flip_flop = 1; } else { flip_flop = 0; }
		}
		symbol->row_height[current_row] = 10;
	}

	symbol->rows = rows_needed;
	symbol->width = 70;
	return errornum;
}


