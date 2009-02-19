/* 2of5.c - Handles Code 2 of 5 barcodes */

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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"


static char *C25MatrixTable[10] = {"113311", "311131", "131131", "331111", "113131", "313111",
	"133111", "111331", "311311", "131311"};

static char *C25IndustTable[10] = {"1111313111", "3111111131", "1131111131", "3131111111", "1111311131",
	"3111311111", "1131311111", "1111113131", "3111113111", "1131113111"};

static char *C25InterTable[10] = {"11331", "31113", "13113", "33111", "11313", "31311", "13311", "11133",
	"31131", "13131"};

	
int matrix_two_of_five(struct zint_symbol *symbol, unsigned char source[])
{ /* Code 2 of 5 Standard (Code 2 of 5 Matrix) */
	
	int i, error_number;
	char dest[1000];

	error_number = 0;
	strcpy(dest, "");
	
	if(ustrlen(source) > 80) {
		strcpy(symbol->errtxt, "Input too long [021]");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data [022]");
		return error_number;
	}
	
	/* start character */
	concat (dest, "411111");

	for(i = 0; i <= ustrlen(source); i++) {
		lookup(NESET, C25MatrixTable, source[i], dest);
	}

	/* Stop character */
	concat (dest, "41111");
	
	expand(symbol, dest);
	ustrcpy(symbol->text, source);
	return error_number;
}

int industrial_two_of_five(struct zint_symbol *symbol, unsigned char source[])
{ /* Code 2 of 5 Industrial */

	int i, error_number;
	char dest[1000];

	error_number = 0;
	strcpy(dest, "");
	
	if(ustrlen(source) > 45) {
		strcpy(symbol->errtxt, "Input too long [071]");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid character in data [072]");
		return error_number;
	}
	
	/* start character */
	concat (dest, "313111");

	for(i = 0; i <= ustrlen(source); i++) {
		lookup(NESET, C25IndustTable, source[i], dest);
	}

	/* Stop character */
	concat (dest, "31113");
	
	expand(symbol, dest);
	ustrcpy(symbol->text, source);
	return error_number;
}

int iata_two_of_five(struct zint_symbol *symbol, unsigned char source[])
{ /* Code 2 of 5 IATA */
	int i, error_number;
	char dest[1000];
	
	error_number = 0;
	strcpy(dest, "");
	
	if(ustrlen(source) > 45) {
		strcpy(symbol->errtxt, "Input too long [041]");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data [042]");
		return error_number;
	}
	
	/* start */
	concat (dest, "1111");
	
	for(i = 0; i < ustrlen(source); i++) {
		lookup(NESET, C25IndustTable, source[i], dest);
	}
	
	/* stop */
	concat (dest, "311");
	
	expand(symbol, dest);
	ustrcpy(symbol->text, source);
	return error_number;
}

int logic_two_of_five(struct zint_symbol *symbol, unsigned char source[])
{ /* Code 2 of 5 Data Logic */
	
	int i, error_number;
	char dest[1000];
	
	error_number = 0;
	strcpy(dest, "");

	if(ustrlen(source) > 80) {
		strcpy(symbol->errtxt, "Input too long [061]");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data [062]");
		return error_number;
	}

	/* start character */
	concat (dest, "1111");

	for(i = 0; i <= ustrlen(source); i++) {
		lookup(NESET, C25MatrixTable, source[i], dest);
	}

	/* Stop character */
	concat (dest, "311");
	
	expand(symbol, dest);
	ustrcpy(symbol->text, source);
	return error_number;
}

int interleaved_two_of_five(struct zint_symbol *symbol, unsigned char source[])
{ /* Code 2 of 5 Interleaved */

	int i, j, k, error_number;
	char bars[7], spaces[7], mixed[14], dest[1000];

	error_number = 0;
	strcpy(dest, "");
	
	if(ustrlen(source) > 90) {
		strcpy(symbol->errtxt, "Input too long [031]");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if (error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data [032]");
		return error_number;
	}
	
	/* Input must be an even number of characters for Interlaced 2 of 5 to work:
	   if an odd number of characters has been entered then add a leading zero */
	if ((ustrlen(source)%2) != 0)
	{
		/* there are an odd number of input characters */
		unsigned int length;
		char temp[100];

		length = ustrlen(source);

		strcpy(temp, (char*)source);
		source[0] = '0';

		for(i = 0; i <= length; i++)
		{
			source[i + 1] = temp[i];
		}
	}

	/* start character */
	concat(dest, "1111");

	for(i = 0; i < ustrlen(source); i+=2 )
	{
		/* look up the bars and the spaces and put them in two strings */
		strcpy(bars, "");
		lookup(NESET, C25InterTable, source[i], bars);
		strcpy(spaces, "");
		lookup(NESET, C25InterTable, source[i + 1], spaces);

		/* then merge (interlace) the strings together */
		k = 0;
		for(j = 0; j <= 4; j++)
		{
			mixed[k] = bars[j]; k++;
			mixed[k] = spaces[j]; k++;
		}
		mixed[k] = '\0';
		concat (dest, mixed);
	}

	/* Stop character */
	concat (dest, "311");
	
	expand(symbol, dest);
	ustrcpy(symbol->text, source);
	return error_number;

}

int itf14(struct zint_symbol *symbol, unsigned char source[])
{
	int i, error_number, h, zeroes;
	unsigned int count, check_digit;
	char localstr[15];
	char checkstr[3];
	
	error_number = 0;

	count = 0;
	h = ustrlen(source);
	
	if(h > 13) {
		strcpy(symbol->errtxt, "Input too long [891]");
		return ERROR_TOO_LONG;
	}
	
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid character in data [892]");
		return error_number;
	}

	/* Add leading zeros as required */
	strcpy(localstr, "");
	zeroes = 13 - ustrlen(source);
	for(i = 0; i < zeroes; i++) {
		concat(localstr, "0");
	}
	concat(localstr, (char *)source);
	
	/* Calculate the check digit - the same method used for EAN-13 */

	for (i = h - 1; i >= 0; i--)
	{
		count += ctoi(localstr[i]);

		if (!((i%2) == 0))
		{
			count += 2 * ctoi(localstr[i]);
		}
	}
	check_digit = 10 - (count%10);
	if (check_digit == 10) { check_digit = 0; }
	checkstr[0] = itoc(check_digit);
	checkstr[1] = '\0';
	concat(localstr, checkstr);
	error_number = interleaved_two_of_five(symbol, (unsigned char *)localstr);
	ustrcpy(symbol->text, (unsigned char*)localstr);
	return error_number;
}

int dpleit(struct zint_symbol *symbol, unsigned char source[])
{ /* Deutshe Post Leitcode */
	int i, error_number;
	unsigned int h, count, check_digit;
	char localstr[15], checkstr[3];
	int zeroes;

	error_number = 0;
	count = 0;
	h = ustrlen(source);
	if(h > 13) {
		strcpy(symbol->errtxt, "Input wrong length [211]");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data [212]");
		return error_number;
	}

	strcpy(localstr, "");
	zeroes = 13 - h;
	for(i = 0; i < zeroes; i++)
		concat(localstr, "0");
	concat(localstr, (char *)source);
	
	for (i = 12; i >= 0; i--)
	{
		count += 4 * ctoi(localstr[i]);

		if (!((i%2) == 0))
		{
			count += 5 * ctoi(localstr[i]);
		}
	}
	check_digit = 10 - (count%10);
	if (check_digit == 10) { check_digit = 0; }
	checkstr[0] = itoc(check_digit);
	checkstr[1] = '\0';
	concat(localstr, checkstr);
	error_number = interleaved_two_of_five(symbol, (unsigned char *)localstr);
	ustrcpy(symbol->text, (unsigned char*)localstr);
	return error_number;
}

int dpident(struct zint_symbol *symbol, unsigned char source[])
{ /* Deutsche Post Identcode */
	int i, error_number, zeroes;
	unsigned int h, count, check_digit;
	char localstr[13], checkstr[3];

	count = 0;
	h = ustrlen(source);
	if(h > 11) {
		strcpy(symbol->errtxt, "Input wrong length [221]");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data [222]");
		return error_number;
	}

	strcpy(localstr, "");
	zeroes = 11 - h;
	for(i = 0; i < zeroes; i++)
		concat(localstr, "0");
	concat(localstr, (char *)source);
	
	for (i = 10; i >= 0; i--)
	{
		count += 4 * ctoi(localstr[i]);

		if (!((i%2) == 0))
		{
			count += 5 * ctoi(localstr[i]);
		}
	}
	check_digit = 10 - (count%10);
	if (check_digit == 10) { check_digit = 0; }
	checkstr[0] = itoc(check_digit);
	checkstr[1] = '\0';
	concat(localstr, checkstr);
	error_number = interleaved_two_of_five(symbol, (unsigned char *)localstr);
	ustrcpy(symbol->text, (unsigned char*)localstr);
	return error_number;
}
