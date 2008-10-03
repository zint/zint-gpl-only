/* code128.c - Handles Code 128 and derivatives */

/*
    libzint - the open source barcode library
    Copyright (C) 2008 Robin Stuart <zint@hotmail.co.uk>
    Bugfixes thanks to Christian Sakowski and BogDan Vatra

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

#include <stdio.h>
#include <string.h>
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

int list[2][170];

/* Code 128 tables checked against ISO/IEC 15417:2007 */

static char *C128Table[107] = {"212222", "222122", "222221", "121223", "121322", "131222", "122213",
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
	"2331112"};
/* Code 128 character encodation - Table 1 */

int parunmodd(unsigned char llyth)
{
	int modd;
	modd = 0;
	
	if(llyth <= 31) { modd = SHIFTA; }
	if((llyth >= 32) && (llyth <= 95)) { modd = AORB; }
	if((llyth >= 48) && (llyth <= 57)) { modd = ABORC; }
	if((llyth >= 96) && (llyth <= 127)) { modd = SHIFTB; }
	if((llyth >= 128) && (llyth <= 159)) { modd = SHIFTA; }
	if((llyth >= 160) && (llyth <= 223)) { modd = AORB; }
	if(llyth >= 224) { modd = SHIFTB; }
	
	return modd;
}

void grwp(int *indexliste)
{
	int i, j;
	
	/* bring together same type blocks */
	if(*(indexliste) > 1) {
		i = 1;
		while(i < *(indexliste)) {
			if(list[1][i - 1] == list[1][i]) {
				/* bring together */
				list[0][i - 1] = list[0][i - 1] + list[0][i];
				j = i + 1;
				
				/* decreace the list */
				while(j < *(indexliste)) {
					list[0][j - 1] = list[0][j];
					list[1][j - 1] = list[1][j];
					j++;
				}
				*(indexliste) = *(indexliste) - 1;
				i--;
			}
			i++;
		}
	}
}

void dxsmooth(int *indexliste)
{ /* Implements rules from ISO 15417 Annex E */
	int i, current, last, next, length;
	
	for(i = 0; i < *(indexliste); i++) {
		current = list[1][i];
		length = list[0][i];
		if(i != 0) { last = list[1][i - 1]; } else { last = FALSE; }
		if(i != *(indexliste) - 1) { next = list[1][i + 1]; } else { next = FALSE; }
		
		if(i == 0) { /* first block */
			if((*(indexliste) == 1) && ((length == 2) && (current == ABORC))) { /* Rule 1a */ list[1][i] = LATCHC; }
			if(current == ABORC) { 
				if(length >= 4) {/* Rule 1b */ list[1][i] = LATCHC; } else { list[1][i] = AORB; current = AORB; }
			}
			if(current == SHIFTA) { /* Rule 1c */ list[1][i] = LATCHA; }
			if((current == AORB) && (next == SHIFTA)) { /* Rule 1c */ list[1][i] = LATCHA; current = LATCHA; }
			if(current == AORB) { /* Rule 1d */ list[1][i] = LATCHB; }
		} else {
			if((current == ABORC) && (length >= 4)) { /* Rule 3 */ list[1][i] = LATCHC; current = LATCHC; }
			if(current == ABORC) { list[1][i] = AORB; current = AORB; }
			if((current == AORB) && (last == LATCHA)) { list[1][i] = LATCHA; current = LATCHA; }
			if((current == AORB) && (last == LATCHB)) { list[1][i] = LATCHB; current = LATCHB; }
			if((current == AORB) && (next == SHIFTA)) { list[1][i] = LATCHA; current = LATCHA; }
			if((current == AORB) && (next == SHIFTB)) { list[1][i] = LATCHB; current = LATCHB; }
			if(current == AORB) { list[1][i] = LATCHB; current = LATCHB; }
			if((current == SHIFTA) && (length > 1)) { /* Rule 4 */ list[1][i] = LATCHA; current = LATCHA; }
			if((current == SHIFTB) && (length > 1)) { /* Rule 5 */ list[1][i] = LATCHB; current = LATCHB; }
			if((current == SHIFTA) && (last == LATCHA)) { list[1][i] = LATCHA; current = LATCHA; }
			if((current == SHIFTB) && (last == LATCHB)) { list[1][i] = LATCHB; current = LATCHB; }
		} /* Rule 2 is implimented elsewhere, Rule 6 is implied */
	}
	grwp(indexliste);

}

void c128_set_a(unsigned char source, char dest[], int values[], int *bar_chars)
{ /* Translate Code 128 Set A characters into barcodes */
  /* This set handles all control characters NULL to US */
	
	if(source > 127) {
		if(source < 160) {
			concat(dest, C128Table[(source - 128) + 64]);
			values[(*bar_chars)] = (source - 128) + 64;
		} else {
			concat(dest, C128Table[(source - 128) - 32]);
			values[(*bar_chars)] = (source - 128) - 32;
		}
	} else {
		if(source < 32) {
			concat(dest, C128Table[source + 64]);
			values[(*bar_chars)] = source + 64;
		} else {
			concat(dest, C128Table[source - 32]);
			values[(*bar_chars)] = source - 32;
		}
	}
	(*bar_chars)++;
}

void c128_set_b(unsigned char source, char dest[], int values[], int *bar_chars)
{ /* Translate Code 128 Set B characters into barcodes */
  /* This set handles all characters which are not part of long numbers and not control characters */

	if(source > 127) {
		concat(dest, C128Table[source - 32 - 128]);
		values[(*bar_chars)] = source - 32 - 128;
	} else {
		concat(dest, C128Table[source - 32]);
		values[(*bar_chars)] = source - 32;
	}
	(*bar_chars)++;
}

void c128_set_c(unsigned char source_a, unsigned char source_b, char dest[], int values[], int *bar_chars)
{ /* Translate Code 128 Set C characters into barcodes */
  /* This set handles numbers in a compressed form */
	int weight;

	weight = (10 * ctoi(source_a)) + ctoi(source_b);
	concat(dest, C128Table[weight]);
	values[(*bar_chars)] = weight;
	(*bar_chars)++;
}

int code_128(struct zint_symbol *symbol, unsigned char source[])
{ /* Handle Code 128 and NVE-18 */
	int i, j, k, e_count, values[170], bar_characters, read, total_sum, nve_check;
	int errornum, indexchaine, indexliste, sourcelen;
	char set[170], fset[170], mode, last_set, last_fset;
	float glyph_count;
	char dest[1000];
	
	errornum = 0;
	strcpy(dest, "");
	
	sourcelen = ustrlen(source);
	
	j = 0;
	e_count = 0;
	bar_characters = 0;
	nve_check = 0;

	for(i = 0; i < 170; i++) {
		values[i] = 0;
		set[i] = ' ';
		fset[i] = ' ';
	}
	
	if(sourcelen > 160) {
		/* This only blocks rediculously long input - the actual length of the
		   resulting barcode depends on the type of data, so this is trapped later */
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}

	/* Add check digit if encoding an NVE18 symbol */
	if(symbol->symbology == BARCODE_NVE18) {
		errornum = is_sane(NESET, source);
		if(errornum == ERROR_INVALID_DATA) {
			strcpy(symbol->errtxt, "error: invalid characters in data");
			return errornum;
		}
		if(sourcelen != 17) {
			strcpy(symbol->errtxt, "error: input wrong length");
			return ERROR_TOO_LONG;
		}
		for(i = sourcelen + 2; i > 1; i--) {
			source[i] = source[i - 2];
		}
		source[0] = '0';
		source[1] = '0';
		total_sum = 0;
		for(i = 0; i < 19; i++)
		{
			if((i % 2) == 0) {
				total_sum +=  3 * ctoi(source[i]);
			} else {
				total_sum += ctoi(source[i]);
			}

		}
		nve_check = 10 - total_sum%10;
		source[sourcelen + 1] = '\0';
		source[sourcelen] = itoc(nve_check);
	}
	
	/* Detect extended ASCII characters */
	for(i = 0; i < sourcelen; i++) {
		if(source[i] >= 128) {
			fset[i] = 'f';
		} else {
			fset[i] = ' ';
		}
	}
	fset[i] = '\0';
	
	/* Decide when to latch to extended mode - Annex E note 3 */
	j = 0;
	for(i = 0; i < sourcelen; i++) {
		if(fset[i] == 'f') {
			j++;
		} else {
			j = 0;
		}
		
		if(j >= 5) {
			for(k = i; k > (i - 5); k--) {
				fset[k] = 'F';
			}
		}
		
		if((j >= 3) && (i == (sourcelen - 1))) {
			for(k = i; k > (i - 3); k--) {
				fset[k] = 'F';
			}
		}
	}

	/* Decide on mode using same system as PDF417 and rules of ISO 15417 Annex E */
	indexliste = 0;
	indexchaine = 0;
	
	mode = parunmodd(source[indexchaine]);
	if((symbol->symbology == BARCODE_CODE128B) && (mode == ABORC)) {
		mode = AORB;
	}
	
	for(i = 0; i < 170; i++) {
		list[0][i] = 0;
	}

	do {
		list[1][indexliste] = mode;
		while ((list[1][indexliste] == mode) && (indexchaine < sourcelen)) {
			list[0][indexliste]++;
			indexchaine++;
			mode = parunmodd(source[indexchaine]);
			if((symbol->symbology == BARCODE_CODE128B) && (mode == ABORC)) {
				mode = AORB;
			}
		}
		indexliste++;
	} while (indexchaine < sourcelen);
	
	dxsmooth(&indexliste);

	/* Resolve odd length LATCHC blocks */
	if((list[1][0] == LATCHC) && ((list[0][0] % 2) == 1)) {
		/* Rule 2 */
		list[0][1]++;
		list[0][0]--;
		if(indexliste == 1) {
			list[0][1] = 1;
			list[1][1] = LATCHB;
			indexliste = 2;
		}
	}	
	if(indexliste > 1) {
		for(i = 1; i < indexliste; i++) {
			if((list[1][i] == LATCHC) && ((list[0][i] % 2) == 1)) {
				/* Rule 3b */
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
	
	/* Now we can calculate how long the barcode is going to be - and stop it from
	   being too long */
	last_set = ' ';
	last_fset = ' ';
	glyph_count = 0.0;
	for(i = 0; i < sourcelen; i++) {
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
	if(glyph_count > 80.0) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	
	
	/* So now we know what start character to use - we can get on with it! */
	switch(set[0])
	{
		case 'A': /* Start A */
			concat(dest, C128Table[103]);
			values[0] = 103;
			break;
		case 'B': /* Start B */
			concat(dest, C128Table[104]);
			values[0] = 104;
			break;
		case 'C': /* Start C */
			concat(dest, C128Table[105]);
			values[0] = 105;
			break;
	}
	bar_characters++;
	
	if(symbol->symbology == BARCODE_NVE18) {
		concat(dest, C128Table[102]);
		values[1] = 102;
		bar_characters++;
	}

	if(fset[0] == 'F') {
		switch(set[0]) {
			case 'A':
				concat(dest, C128Table[101]);
				concat(dest, C128Table[101]);
				values[bar_characters] = 101;
				values[bar_characters + 1] = 101;
				break;
			case 'B':
				concat(dest, C128Table[100]);
				concat(dest, C128Table[100]);
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
				case 'A': concat(dest, C128Table[101]);
					values[bar_characters] = 101;
					bar_characters++;
					break;
				case 'B': concat(dest, C128Table[100]);
					values[bar_characters] = 100;
					bar_characters++;
					break;
				case 'C': concat(dest, C128Table[99]);
					values[bar_characters] = 99;
					bar_characters++;
					break;
			}
		}

		if((read != 0) && (fset[read] != fset[read - 1])) {
			if(fset[read] == 'F') {
				/* Latch beginning of extended mode */
				switch(set[0]) {
					case 'A':
						concat(dest, C128Table[101]);
						concat(dest, C128Table[101]);
						values[bar_characters] = 101;
						values[bar_characters + 1] = 101;
						break;
					case 'B':
						concat(dest, C128Table[100]);
						concat(dest, C128Table[100]);
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
						concat(dest, C128Table[101]);
						concat(dest, C128Table[101]);
						values[bar_characters] = 101;
						values[bar_characters + 1] = 101;
						break;
					case 'B':
						concat(dest, C128Table[100]);
						concat(dest, C128Table[100]);
						values[bar_characters] = 100;
						values[bar_characters + 1] = 100;
						break;
				}
				bar_characters += 2;
			}
		}

		if(fset[read] == 'f') {
			/* Shift extended mode */
			switch(set[read]) {
				case 'a':
				case 'A':
					concat(dest, C128Table[101]);
					values[bar_characters] = 101;
					break;
				case 'b':
				case 'B':
					concat(dest, C128Table[100]);
					values[bar_characters] = 100;
					break;
			}
			bar_characters++;
		}

		if((set[read] == 'a') || (set[read] == 'b')) {
			/* Insert shift character */
			concat(dest, C128Table[98]);
			values[bar_characters] = 98;
			bar_characters++;
		}

		switch(set[read])
		{ /* Encode data characters */
			case 'a':
			case 'A': c128_set_a(source[read], dest, values, &bar_characters);
				read++;
				break;
			case 'b':
			case 'B': c128_set_b(source[read], dest, values, &bar_characters);
				read++;
				break;
			case 'C': c128_set_c(source[read], source[read + 1], dest, values, &bar_characters);
				read += 2;
				break;
		}
		
	} while (read < sourcelen);

	/* check digit calculation */
	total_sum = 0;
	for(i = 0; i < bar_characters; i++)
	{
		if(i > 0)
		{
			values[i] *= i;

		}
		total_sum += values[i];
	}
	concat(dest, C128Table[total_sum%103]);
	
	/* Stop character */
	concat(dest, C128Table[106]);
	expand(symbol, dest);
	strcpy(symbol->text, (char*)source);
	return errornum;
}

int ean_128(struct zint_symbol *symbol, unsigned char source[])
{ /* Handle EAN-128 (Now known as GS1-128) */
	int i, j, e_count, values[170], bar_characters, read, total_sum;
	int errornum, indexchaine, indexliste, ai_latch, sourcelen;
	char set[170], mode, last_set, reduced[170], ai_string[4];
	float glyph_count;
	char dest[1000];
	int last_ai, separator_row, linkage_flag;

	errornum = 0;
	strcpy(dest, "");
	linkage_flag = 0;
	sourcelen = ustrlen(source);

	j = 0;
	e_count = 0;
	bar_characters = 0;
	separator_row = 0;

	for(i = 0; i < 170; i++) {
		values[i] = 0;
		set[i] = ' ';
	}
	
	if(sourcelen > 160) {
		/* This only blocks rediculously long input - the actual length of the
		resulting barcode depends on the type of data, so this is trapped later */
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}

	/* Detect extended ASCII characters */
	for(i = 0; i <  sourcelen; i++) {
		if(source[i] >=128) {
			strcpy(symbol->errtxt, "error: extended ASCII characters not supported by GS1-128");
			return ERROR_INVALID_DATA;
		}
	}
	
	if(source[0] != '[') {
		strcpy(symbol->errtxt, "error: input string doesn't start with AI");
		return ERROR_INVALID_DATA;
	}
	
	/* if part of a composite symbol make room for the separator pattern */
	if(symbol->symbology == BARCODE_EAN128_CC) {
		separator_row = symbol->rows;
		symbol->row_height[symbol->rows] = 1;
		symbol->rows += 1;
	}

	/* Resolve AI data - put resulting string in 'reduced' */
	j = 0;
	last_ai = 0;
	ai_latch = 1;
	for(i = 0; i < sourcelen; i++) {
		if((source[i] != '[') && (source[i] != ']')) {
			reduced[j] = source[i];
			j++;
		}
		if(source[i] == '[') {
			/* Start of an AI string */
			if(ai_latch == 0) {
				reduced[j] = '[';
				j++;
			}
			ai_string[0] = source[i + 1];
			ai_string[1] = source[i + 2];
			ai_string[2] = '\0';
			last_ai = atoi(ai_string);
			ai_latch = 0;
			/* The following values from GS1 specification figure 5.3.8.2.1 - 1
			   "Element Strings with Pre-Defined Length Using Application Identifiers" */
			if((last_ai >= 0) && (last_ai <= 4)) { ai_latch = 1; }
			if((last_ai >= 11) && (last_ai <= 20)) { ai_latch = 1; }
			if(last_ai == 23) { ai_latch = 1; } /* legacy support - see 5.3.8.2.2 */
			if((last_ai >= 31) && (last_ai <= 36)) { ai_latch = 1; }
			if(last_ai == 41) { ai_latch = 1; }
		}
		/* The ']' character is simply dropped from the input */
	}
	reduced[j] = '\0';
	
	/* the character '[' in the reduced string refers to the FNC1 character */
	
	/* Note that no attempt is made to verify that the data to be encoded does
	actually conform to the right data length - that is required of the person or
	program inputting the data */
	
	/* Decide on mode using same system as PDF417 and rules of ISO 15417 Annex E */
	indexliste = 0;
	indexchaine = 0;
	
	mode = parunmodd(reduced[indexchaine]);
	if(reduced[indexchaine] == '[') {
		mode = ABORC;
	}
	
	for(i = 0; i < 170; i++) {
		list[0][i] = 0;
	}
	
	do {
		list[1][indexliste] = mode;
		while ((list[1][indexliste] == mode) && (indexchaine < strlen(reduced))) {
			list[0][indexliste]++;
			indexchaine++;
			mode = parunmodd(reduced[indexchaine]);
			if(reduced[indexchaine] == '[') {
				if(indexchaine % 2 == 0) {
					mode = ABORC;
				} else {
					mode = AORB;
				}
			}
		}
		indexliste++;
	} while (indexchaine < strlen(reduced));
	
	dxsmooth(&indexliste);
	
	/* Resolve odd length LATCHC blocks */
	if((list[1][0] == LATCHC) && ((list[0][0] % 2) == 1)) {
		/* Rule 2 */
		list[0][1]++;
		list[0][0]--;
		if(indexliste == 1) {
			list[0][1] = 1;
			list[1][1] = LATCHB;
			indexliste = 2;
		}
	}	
	if(indexliste > 1) {
		for(i = 1; i < indexliste; i++) {
			if((list[1][i] == LATCHC) && ((list[0][i] % 2) == 1)) {
				/* Rule 3b */
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
	
	/* Now we can calculate how long the barcode is going to be - and stop it from
	being too long */
	last_set = ' ';
	glyph_count = 0.0;
	for(i = 0; i < strlen(reduced); i++) {
		if((set[i] == 'a') || (set[i] == 'b')) {
			glyph_count = glyph_count + 1.0;
		}
		if(((set[i] == 'A') || (set[i] == 'B')) || (set[i] == 'C')) {
			if(set[i] != last_set) {
				last_set = set[i];
				glyph_count = glyph_count + 1.0;
			}
		}
		
		if(set[i] == 'C') {
			glyph_count = glyph_count + 0.5;
		} else {
			glyph_count = glyph_count + 1.0;
		}
	}
	if(glyph_count > 80.0) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	
	/* So now we know what start character to use - we can get on with it! */
	switch(set[0])
	{
		case 'A': /* Start A */
			concat(dest, C128Table[103]);
			values[0] = 103;
			break;
		case 'B': /* Start B */
			concat(dest, C128Table[104]);
			values[0] = 104;
			break;
		case 'C': /* Start C */
			concat(dest, C128Table[105]);
			values[0] = 105;
			break;
	}
	bar_characters++;
	
	concat(dest, C128Table[102]);
	values[1] = 102;
	bar_characters++;
	
	/* Encode the data */
	read = 0;
	do {

		if((read != 0) && (set[read] != set[read - 1]))
		{ /* Latch different code set */
			switch(set[read])
			{
				case 'A': concat(dest, C128Table[101]);
				values[bar_characters] = 101;
				bar_characters++;
				break;
				case 'B': concat(dest, C128Table[100]);
				values[bar_characters] = 100;
				bar_characters++;
				break;
				case 'C': concat(dest, C128Table[99]);
				values[bar_characters] = 99;
				bar_characters++;
				break;
			}
		}
		
		if((set[i] == 'a') || (set[i] == 'b')) {
			/* Insert shift character */
			concat(dest, C128Table[98]);
			values[bar_characters] = 98;
			bar_characters++;
		}

		if(reduced[read] != '[') {
			switch(set[read])
			{ /* Encode data characters */
				case 'A': c128_set_a(reduced[read], dest, values, &bar_characters);
				read++;
				break;
				case 'B': c128_set_b(reduced[read], dest, values, &bar_characters);
				read++;
				break;
				case 'C': c128_set_c(reduced[read], reduced[read + 1], dest, values, &bar_characters);
				read += 2;
				break;
			}
		} else {
			concat(dest, C128Table[102]);
			values[bar_characters] = 102;
			bar_characters++;
			read++;
		}
	} while (read < strlen(reduced));
	
	/* "...note that the linkage flag is an extra code set character between
	the last data character and the Symbol Check Character" (GS1 Specification) */
	
	/* Linkage flags in GS1-128 are determined by ISO/IEC 24723 section 7.4 */

	switch(symbol->option_1) {
		case 1:
		case 2:
			/* CC-A or CC-B 2D component */
			switch(set[strlen(reduced) - 1]) {
				case 'A': linkage_flag = 100; break;
				case 'B': linkage_flag = 99; break;
				case 'C': linkage_flag = 101; break;
			}
			break;
		case 3:
			/* CC-C 2D component */
			switch(set[strlen(reduced) - 1]) {
				case 'A': linkage_flag = 99; break;
				case 'B': linkage_flag = 101; break;
				case 'C': linkage_flag = 100; break;
			}
			break;
	}
	
	if(linkage_flag != 0) {
		concat(dest, C128Table[linkage_flag]);
		values[bar_characters] = linkage_flag;
		bar_characters++;
	}
	
	/* check digit calculation */
	total_sum = 0;
	for(i = 0; i < bar_characters; i++)
	{
		if(i > 0)
		{
			values[i] *= i;

		}
		total_sum += values[i];
	}
	concat(dest, C128Table[total_sum%103]);
	
	/* Stop character */
	concat(dest, C128Table[106]);
	expand(symbol, dest);
	
	/* Add the separator pattern for composite symbols */
	if(symbol->symbology == BARCODE_EAN128_CC) {
		for(i = 0; i < symbol->width; i++) {
			if(symbol->encoded_data[separator_row + 1][i] != '1') {
				symbol->encoded_data[separator_row][i] = '1';
			}
		}
	}

	for(i = 0; i <= sourcelen; i++) {
		if((source[i] != '[') && (source[i] != ']')) {
			symbol->text[i] = source[i];
		}
		if(source[i] == '[') {
			symbol->text[i] = '(';
		}
		if(source[i] == ']') {
			symbol->text[i] = ')';
		}
	}
	
	return errornum;
}

int ean_14(struct zint_symbol *symbol, unsigned char source[])
{
	/* EAN-14 - A version of EAN-128 */
	int input_length, i, count, check_digit;
	int error_number;
	unsigned char ean128_equiv[20];
	
	memset(ean128_equiv, 0, 20);
	input_length = ustrlen(source);
	
	if(input_length != 13) {
		strcpy(symbol->errtxt, "error: input wrong length");
		return ERROR_TOO_LONG;
	}
	
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid character in data");
		return error_number;
	}
	concat((char*)ean128_equiv, "[01]");
	concat((char*)ean128_equiv, (char*)source);
	
	count = 0;
	for (i = input_length - 1; i >= 0; i--)
	{
		count += ctoi(source[i]);

		if (!((i%2) == 1))
		{
			count += 2 * ctoi(source[i]);
		}
	}
	check_digit = 10 - (count%10);
	if (check_digit == 10) { check_digit = 0; }
	ean128_equiv[17] = itoc(check_digit);
	ean128_equiv[18] = '\0';
	
	error_number = ean_128(symbol, ean128_equiv);
	
	return error_number;
}

