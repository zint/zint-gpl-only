/* common.c - Contains functions needed for a number of barcodes */

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
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

#define SSET	"0123456789ABCDEF"

void concat(char dest[], char source[])
{ /* Concatinates dest[] with the contents of source[], copying /0 as well */
	unsigned int i, j;

	j = strlen(dest);
	for(i = 0; i <= strlen(source); i++) {
		dest[i + j] = source[i]; }
}

int ctoi(char source)
{ /* Converts a character 0-9 to its equivalent integer value */
	if((source >= '0') && (source <= '9'))
		return (source - '0');
	return(source - 'A' + 10);
}

char itoc(int source)
{ /* Converts an integer value to its hexadecimal character */
	if ((source >= 0) && (source <= 9)) {
		return ('0' + source); }
	else {
		return ('A' + (source - 10)); }
}

void to_upper(unsigned char source[])
{ /* Converts lower case characters to upper case in a string source[] */
	unsigned int i;

	for (i = 0; i < strlen(source); i++) {
		if ((source[i] >= 'a') && (source[i] <= 'z')) {
			source [i] = (source[i] - 'a') + 'A'; }
	}
}

int is_sane(char test_string[], unsigned char source[])
{ /* Verifies that a string only uses valid characters */
	unsigned int i, j, latch;

	for(i = 0; i < strlen(source); i++) {
		latch = FALSE;
		for(j = 0; j < strlen(test_string); j++) {
			if (source[i] == test_string[j]) { latch = TRUE; } }
		if (!(latch)) { 
			return ERROR_INVALID_DATA; }
	}
	
	return 0;
}

int posn(char set_string[], char data)
{ /* Returns the position of data in set_string */
	unsigned int i;

	for(i = 0; i < strlen(set_string); i++) {
		if (data == set_string[i]) { return i; } }
	return 0;
}

void lookup(char set_string[], char *table[], char data, char dest[])
{ /* Replaces huge switch statements for looking up in tables */
	unsigned int i;

	for(i = 0; i < strlen(set_string); i++) {
		if (data == set_string[i]) { concat(dest, table[i]); } }
}

void expand(struct zint_symbol *symbol, char data[])
{ /* Expands from a width pattern to a bit pattern */
	
	int reader, writer, i;
	char latch;
	
	writer = 0;
	latch = '1';
	
	for(reader = 0; reader < strlen(data); reader++) {
		for(i = 0; i < ctoi(data[reader]); i++) {
			symbol->encoded_data[symbol->rows][writer] = latch;
			writer++;
		}
		if(latch == '1') {
			latch = '0';
		} else {
			latch = '1';
		}
	}
	
	if(symbol->symbology != BARCODE_PHARMA) {
		if(writer > symbol->width) {
			symbol->width = writer;
		}
	} else {
		/* Pharmacode One ends with a space - adjust for this */
		if(writer > symbol->width + 2) {
			symbol->width = writer - 2;
		}
	}
	symbol->rows = symbol->rows + 1;
}
