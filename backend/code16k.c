/* code16k.c - Handles Code 16k stacked symbology */

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
static char *C16KTable[107] = {"212222", "222122", "222221", "121223", "121322", "131222", "122213",
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
static char *C16KStartStop[8] = {"3211", "2221", "2122", "1411", "1132", "1231", "1114", "3112"};

/* EN 12323 Table 5 - Start and stop values defining row numbers */
static int C16KStartValues[16] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7};
static int C16KStopValues[16] = {0, 1, 2, 3, 4, 5, 6, 7, 4, 5, 6, 7, 0, 1, 2, 3};

int parunmodd(unsigned char llyth);
void grwp(int *indexliste);
void dxsmooth(int *indexliste);

void c16k_set_a(unsigned char source, int values[], int *bar_chars)
{
	if(source > 127) {
		if(source < 160) {
			values[(*bar_chars)] = source + 64 - 128;
		} else {
			values[(*bar_chars)] = source - 32 - 128;
		}
	} else {
		if(source < 32) {
			values[(*bar_chars)] = source + 64;
		} else {
			values[(*bar_chars)] = source - 32;
		}
	}
	(*bar_chars)++;
}

void c16k_set_b(unsigned char source, int values[], int *bar_chars)
{
	if(source > 127) {
		values[(*bar_chars)] = source - 32 - 128;
	} else {
		values[(*bar_chars)] = source - 32;
	}
	(*bar_chars)++;
}

void c16k_set_c(unsigned char source_a, unsigned char source_b, int values[], int *bar_chars)
{
	int weight;

	weight = (10 * ctoi(source_a)) + ctoi(source_b);
	values[(*bar_chars)] = weight;
	(*bar_chars)++;
}

int code16k(struct zint_symbol *symbol, unsigned char source[])
{
	char width_pattern[100];
	int current_row, rows_needed, flip_flop, looper, first_check, second_check;
	int indexliste, indexchaine, pads_needed;
	char set[160], fset[160], mode, last_set, last_fset;
	unsigned int i, j, k, m, e_count, read, mx_reader, writer;
	unsigned int values[160];
	unsigned int bar_characters;
	strcpy(width_pattern, "");
	float glyph_count;
	int errornum, first_sum, second_sum;
	int input_length;

	errornum = 0;
	input_length = ustrlen(source);
	
	if(input_length > 157) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}

	e_count = 0;
	bar_characters = 0;

	for(i = 0; i < 160; i++) {
		values[i] = 0;
		set[i] = ' ';
		fset[i] = ' ';
	}
	
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
			if((j >= 5) || ((j >= 3) && ((i + j) == input_length))) {
				for(k = 0; k <= j; k++) {
					fset[i + k] = 'F';
				}
			}
		}
	}
	
	/* Detect mode A, B and C characters */
	indexliste = 0;
	indexchaine = 0;
	
	mode = parunmodd(source[indexchaine]);
	
	for(i = 0; i < 160; i++) {
		list[0][i] = 0;
	}
	
	do {
		list[1][indexliste] = mode;
		while ((list[1][indexliste] == mode) && (indexchaine < input_length)) {
			list[0][indexliste]++;
			indexchaine++;
			mode = parunmodd(source[indexchaine]);
		}
		indexliste++;
	} while (indexchaine < input_length);
	
	dxsmooth(&indexliste);

	/* Resolve odd length LATCHC blocks */
	if((list[1][0] == LATCHC) && ((list[0][0] % 2) == 1)) {
		for(i = 1; i <= indexliste; i++) {
			list[0][i] = list[0][i - 1];
			list[1][i] = list[1][i - 1];
		}
		list[0][1]--;
		list[0][0] = 1;
		list[1][0] = LATCHB;
		indexliste++;
	}
	if(indexliste > 1) {
		for(i = 1; i < indexliste; i++) {
			if((list[1][i] == LATCHC) && ((list[0][i] % 2) == 1)) {
				list[0][i - 1]++;
				list[0][i]--;
			}
		}
	}
	
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

	/* Make sure the data will fit in the symbol */
	last_set = ' ';
	last_fset = ' ';
	glyph_count = 0.0;
	for(i = 0; i < input_length; i++) {
		if((set[i] == 'a') || (set[i] == 'b')) {
			glyph_count = glyph_count + 1.0;
		}
		if(fset[i] == 'f') {
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
				last_fset = 'F';
				glyph_count = glyph_count + 2.0;
			}
		} else {
			if((fset[i] == 'F') && (fset[i - 1] != 'F')) {
				last_fset = 'F';
				glyph_count = glyph_count + 2.0;
			}
			if((fset[i] != 'F') && (fset[i - 1] == 'F')) {
				last_fset = ' ';
				glyph_count = glyph_count + 2.0;
			}
		}
		
		if(set[i] == 'C') {
			glyph_count = glyph_count + 0.5;
		} else {
			glyph_count = glyph_count + 1.0;
		}
	}
	if(glyph_count > 77.0) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	
	/* Calculate how tall the symbol will be */
	glyph_count = glyph_count + 2.0;
	i = glyph_count;
	rows_needed = (i/5);
	if(i%5 > 0) { rows_needed++; }
	
	/* start with the mode character - Table 2 */
	m = 0;
	switch(set[0]) {
		case 'A': m = 0; break;
		case 'B': m = 1; break;
		case 'C': m = 2; break;
	}
	if((set[0] == 'B') && (set[1] == 'C')) { m = 5; }
	if(((set[0] == 'B') && (set[1] == 'B')) && (set[2] == 'C')) { m = 6; }
	values[bar_characters] = (7 * (rows_needed - 2)) + m; /* see 4.3.4.2 */
	bar_characters++;

	if(fset[0] == 'F') {
		switch(set[0]) {
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
	}
	
	/* Encode the data */
	read = 0;
	do {

		if((read != 0) && (set[read] != set[read - 1]))
		{ /* Latch different code set */
			switch(set[read])
			{
				case 'A':
					values[bar_characters] = 101;
					bar_characters++;
					break;
				case 'B':
					values[bar_characters] = 100;
					bar_characters++;
					break;
				case 'C':
					if(!((read == 1) && (set[0] == 'B'))) { /* Not Mode C/Shift B */
						if(!((read == 2) && ((set[0] == 'B') && (set[1] == 'B')))) {
							/* Not Mode C/Double Shift B */
							values[bar_characters] = 99;
							bar_characters++;
						}
					}
					break;
			}
		}

		if((read != 0) && (fset[read] != fset[read - 1])) {
			if(fset[read] == 'F') {
				/* Latch beginning of extended mode */
				switch(set[0]) {
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
			}
			if(fset[read - 1] == 'F') {
				/* Latch end of extended mode */
				switch(set[0]) {
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
			}
		}
		
		if(fset[i] == 'f') {
			/* Shift extended mode */
			switch(set[i]) {
				case 'A':
					values[bar_characters] = 101;
					break;
				case 'B':
					values[bar_characters] = 100;
					break;
			}
			bar_characters++;
		}
		
		if((set[i] == 'a') || (set[i] == 'b')) {
			/* Insert shift character */
			values[bar_characters] = 98;
			bar_characters++;
		}

		switch(set[read])
		{ /* Encode data characters */
			case 'A': c16k_set_a(source[read], values, &bar_characters);
				read++;
				break;
			case 'B': c16k_set_b(source[read], values, &bar_characters);
				read++;
				break;
			case 'C': c16k_set_c(source[read], source[read + 1], values, &bar_characters);
				read += 2;
				break;
		}
	} while (read < ustrlen(source));

	
	pads_needed = 5 - ((bar_characters + 2) % 5);
	if(pads_needed == 5) {
		pads_needed = 0;
	}
	for(i = 0; i < pads_needed; i++) {
		values[bar_characters] = 106;
		bar_characters++;
	}

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
		}
		concat(width_pattern, C16KStartStop[C16KStopValues[current_row]]);

		/* Write the information into the symbol */
		writer = 0;
		flip_flop = 1;
		for (mx_reader = 0; mx_reader < strlen(width_pattern); mx_reader++) {
			for(looper = 0; looper < ctoi(width_pattern[mx_reader]); looper++) {
				if(flip_flop == 1) {
					symbol->encoded_data[current_row][writer] = '1';
					writer++; }
				else {
					symbol->encoded_data[current_row][writer] = '0';
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


