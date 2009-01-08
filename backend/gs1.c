/* gs1.c - Verifies GS1 data */

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

#include <string.h>
#include <stdlib.h>
#include "common.h"

int gs1_verify(struct zint_symbol *symbol, unsigned char source[], char reduced[])
{
	int i, j, last_ai, ai_latch;
	char ai_string[4];
	
	/* Detect extended ASCII characters */
	for(i = 0; i <  sourcelen; i++) {
		if(source[i] >=128) {
			strcpy(symbol->errtxt, "Extended ASCII characters are not supported by GS1");
			return ERROR_INVALID_DATA;
		}
	}
	
	/* Start with basic tests */
	if(source[0] != '[') {
		strcpy(symbol->errtxt, "Data does not start with an AI");
		return ERROR_INVALID_DATA;
	}
	
	for(i = 0; i < ustrlen(source) - 1; i++) {
		if((source[i] == '[') && (source[i + 1] == '[')) {
			/* Can't have nested brackets - Quit */
			strcpy(symbol->errtxt, "Nested AI detected (two or more open brackets)");
			return ERROR_INVALID_DATA;
		}
	}
	
	for(i = 0; i < ustrlen(source) - 1; i++) {
		if((source[i] == ']') && (source[i + 1] == ']')) {
			/* Can't have nested brackets - Quit */
			strcpy(symbol->errtxt, "Nested AI detected (two or more close brackets)");
			return ERROR_INVALID_DATA;
		}
	}
	
	/* Resolve AI data - put resulting string in 'reduced' */
	j = 0;
	last_ai = 0;
	ai_latch = 1;
	for(i = 0; i < ustrlen(source); i++) {
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
	return 0;
}