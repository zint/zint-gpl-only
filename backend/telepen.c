/* telepen.c - Handles Telepen and Telepen numeric */

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

#define NASET	"0123456789X"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

static char *TeleTable[128] = {	"1111111111111111", "1131313111", "33313111", "1111313131",
	"3111313111", "11333131", "13133131", "111111313111", "31333111", "1131113131", "33113131",
	"1111333111", "3111113131", "1113133111", "1311133111", "111111113131", "3131113111",
	"11313331", "333331", "111131113111", "31113331", "1133113111", "1313113111", "1111113331",
	"31131331", "113111113111", "3311113111", "1111131331", "311111113111", "1113111331",
	"1311111331", "11111111113111", "31313311", "1131311131", "33311131", "1111313311",
	"3111311131", "11333311", "13133311", "111111311131", "31331131", "1131113311", "33113311",
	"1111331131", "3111113311", "1113131131", "1311131131", "111111113311", "3131111131",
	"1131131311", "33131311", "111131111131", "3111131311", "1133111131", "1313111131",
	"111111131311", "3113111311", "113111111131", "3311111131", "111113111311", "311111111131",
	"111311111311", "131111111311", "11111111111131", "3131311111", "11313133", "333133",
	"111131311111", "31113133", "1133311111", "1313311111", "1111113133", "313333",
	"113111311111", "3311311111", "11113333", "311111311111", "11131333", "13111333",
	"11111111311111", "31311133", "1131331111", "33331111", "1111311133", "3111331111",
	"11331133", "13131133", "111111331111", "3113131111", "1131111133", "33111133",
	"111113131111", "3111111133", "111311131111", "131111131111", "111111111133", "31311313",
	"113131111111", "3331111111", "1111311313", "311131111111", "11331313", "13131313",
	"11111131111111", "3133111111", "1131111313", "33111313", "111133111111", "3111111313",
	"111313111111", "131113111111", "111111111313", "313111111111", "1131131113", "33131113",
	"11113111111111", "3111131113", "113311111111", "131311111111", "111111131113", "3113111113",
	"11311111111111", "331111111111", "111113111113", "31111111111111", "111311111113",
	"131111111113"};
	
int telepen(struct zint_symbol *symbol, unsigned char source[])
{
	unsigned int i, count, check_digit;
	int ascii_value;
	int error_number;
	char dest[1000];
	
	error_number = 0;
	strcpy(dest, "");

	count = 0;

	if(ustrlen(source) > 30) {
		strcpy(symbol->errtxt, "Input too long");
		return ERROR_TOO_LONG;
	}

	for(i = 0; i < ustrlen(source); i++) {
		if(source[i] > 127) {
			/* Cannot encode extended ASCII */
			strcpy(symbol->errtxt, "Invalid characters in input data");
			return ERROR_INVALID_DATA;
		}
	}
	
	/* Start character */
	concat(dest, TeleTable['_']);

	for (i=0; i < ustrlen(source); i++)
	{
		ascii_value = source[i];
		if(ascii_value == symbol->nullchar) {
			concat(dest, TeleTable[0]);
		} else {
			concat(dest, TeleTable[ascii_value]);
		}
		count += source[i];
	}

	check_digit = 127 - (count % 127);
	concat(dest, TeleTable[check_digit]);

	/* Stop character */
	concat(dest, TeleTable['z']);
	
	expand(symbol, dest);
	ustrcpy(symbol->text, source);
	return error_number;
}

int telepen_num(struct zint_symbol *symbol, unsigned char source[])
{
	unsigned int i, count, check_digit, glyph;
	int error_number, input_length;
	unsigned char dest[1000];
	unsigned char local_source[100];
	
	error_number = 0;
	memset(dest, 0, 1000);
	memset(local_source, 0, 100);
	strcpy((char*)local_source, (char*)source);
	input_length = ustrlen(source);

	count = 0;

	if(input_length > 60) {
		strcpy(symbol->errtxt, "Input too long");
		return ERROR_TOO_LONG;
	}
	to_upper(local_source);
	error_number = is_sane(NASET, local_source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data");
		return error_number;
	}
	
	/* Add a leading zero if required */
	if ((input_length % 2) != 0)
	{
		char temp[200];

		strcpy(temp, (char*)local_source);
		local_source[0] = '0';

		for(i = 0; i <= input_length; i++)
		{
			local_source[i + 1] = temp[i];
		}
		input_length++;
	}

	/* Start character */
	concat((char*)dest, TeleTable['_']);

	for (i=0; i < input_length; i+=2)
	{
		if(local_source[i] == 'X') {
			strcpy(symbol->errtxt, "Invalid position of X in Telepen data");
			return ERROR_INVALID_DATA;
		}
		
		if(local_source[i + 1] == 'X') {
			glyph = ctoi(local_source[i]) + 17;
			count += glyph;
		} else {
			glyph = (10 * ctoi(local_source[i])) + ctoi(local_source[i + 1]);
			glyph += 27;
			count += glyph;
		}
		concat((char*)dest, TeleTable[glyph]);
	}

	check_digit = 127 - (count % 127);
	concat((char*)dest, TeleTable[check_digit]);

	/* Stop character */
	concat((char*)dest, TeleTable['z']);
	
	expand(symbol, (char*)dest);
	ustrcpy(symbol->text, local_source);
	return error_number;
}

