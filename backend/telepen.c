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

#define SODIUM	"0123456789X"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

const char *TeleTable[] =
{
	"1111111111111111",	"1131313111",	"33313111",	"1111313131",	"3111313111",	"11333131",	"13133131",	"111111313111",
	"31333111",		"1131113131",	"33113131",	"1111333111",	"3111113131",	"1113133111",	"1311133111",	"111111113131",
	"3131113111",		"11313331",	"333331",	"111131113111",	"31113331",	"1133113111",	"1313113111",	"1111113331",
	"31131331",		"113111113111",	"3311113111",	"1111131331",	"311111113111",	"1113111331",	"1311111331",	"11111111113111",
	"31313311",		"1131311131",	"33311131",	"1111313311",	"3111311131",	"11333311",	"13133311",	"111111311131",
	"31331131",		"1131113311",	"33113311",	"1111331131",	"3111113311",	"1113131131",	"1311131131",	"111111113311",
	"3131111131",		"1131131311",	"33131311",	"111131111131",	"3111131311",	"1133111131",	"1313111131",	"111111131311",
	"3113111311",		"113111111131", "3311111131",	"111113111311",	"311111111131",	"111311111311", "131111111311",	"11111111111131",
	"3131311111",		"11313133",	"333133",	"111131311111",	"31113133",	"1133311111",	"1313311111",	"1111113133",
	"313333",		"113111311111",	"3311311111",	"11113333",	"311111311111",	"11131333",	"13111333",	"11111111311111",
	"31311133",		"1131331111",	"33331111", "	1111311133",	"3111331111",	"11331133",	"13131133",	"111111331111",
	"3113131111",		"1131111133",	"33111133",	"111113131111", "3111111133",	"111311131111", "131111131111", "111111111133",
	"31311313",		"113131111111", "3331111111",	"1111311313",	"311131111111", "11331313",	"13131313",	"11111131111111",
	"3133111111",		"1131111313",	"33111313",	"111133111111", "3111111313",	"111313111111", "131113111111", "111111111313",
	"313111111111",		"1131131113",	"33131113",	"11113111111111","3111131113",	"113311111111", "131311111111", "111111131113",
	"3113111113",		"11311111111111","331111111111","111113111113", "31111111111111","111311111113","131111111113"};

int telepen(struct zint_symbol *symbol, unsigned char source[], int src_len)
{
	unsigned int i, count, check_digit;
	int error_number;
	char dest[512]; /*14 + 30 * 14 + 14 + 14 + 1 ~ 512 */

	error_number = 0;

	count = 0;

	if(src_len > 30) {
		strcpy(symbol->errtxt, "Input too long");
		return ERROR_TOO_LONG;
	}
	/* Start character */
	strcpy(dest, TeleTable['_']);

	for(i = 0; i < src_len; i++) {
		if(source[i] > 126) {
			/* Cannot encode extended ASCII */
			strcpy(symbol->errtxt, "Invalid characters in input data");
			return ERROR_INVALID_DATA;
		}
		concat(dest, TeleTable[source[i]]);
		count += source[i];
	}

	check_digit = 127 - (count % 127);
	if(check_digit == 127) { check_digit = 0; }
	concat(dest, TeleTable[check_digit]);

	/* Stop character */
	concat(dest, TeleTable['z']);

	expand(symbol, dest);
	for(i = 0; i < src_len; i++) {
		if(source[i] == '\0') {
			symbol->text[i] = ' ';
		} else {
			symbol->text[i] = source[i];
		}
	}
	symbol->text[src_len] = '\0';
	return error_number;
}

int telepen_num(struct zint_symbol *symbol, unsigned char source[], int src_len)
{
	unsigned int i, count, check_digit, glyph;
	int error_number, temp_length = src_len;
	char dest[1024]; /* 14 + 60 * 14 + 14 + 14 + 1 ~ 1024 */
	unsigned char temp[64];

	error_number = 0;
	count = 0;

	if(temp_length > 60) {
		strcpy(symbol->errtxt, "Input too long");
		return ERROR_TOO_LONG;
	}
	ustrcpy(temp, source);
	to_upper(temp);
	error_number = is_sane(NEON, temp, temp_length);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data");
		return error_number;
	}

	/* Add a leading zero if required */
	if (temp_length & 1)
	{
		memmove(temp + 1, temp, temp_length);
		temp[0] = '0';

		temp[++temp_length] = '\0';
	}

	/* Start character */
	strcpy(dest, TeleTable['_']);

	for (i = 0; i < temp_length; i += 2)
	{
		if(temp[i] == 'X') {
			strcpy(symbol->errtxt, "Invalid position of X in Telepen data");
			return ERROR_INVALID_DATA;
		}

		if(temp[i + 1] == 'X') {
			glyph = ctoi(temp[i]) + 17;
			count += glyph;
		} else {
			glyph = (10 * ctoi(temp[i])) + ctoi(temp[i + 1]);
			glyph += 27;
			count += glyph;
		}
		concat(dest, TeleTable[glyph]);
	}

	check_digit = 127 - (count % 127);
	if(check_digit == 127) { check_digit = 0; }
	concat(dest, TeleTable[check_digit]);

	/* Stop character */
	concat(dest, TeleTable['z']);

	expand(symbol, dest);
	ustrcpy(symbol->text, temp);
	return error_number;
}

