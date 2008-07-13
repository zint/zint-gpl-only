/* postal.c - Handles PostNet, PLANET, FIM. RM4SCC and Flattermarken */

/*  Zint - A barcode generating program using libpng
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
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

#define BESET	"ABCD"
#define KRSET "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"

/* PostNet number encoding table - In this table L is long as S is short */
static char *PNTable[10] = {"LLSSS", "SSSLL", "SSLSL", "SSLLS", "SLSSL", "SLSLS", "SLLSS", "LSSSL",
	"LSSLS", "LSLSS"};
static char *PLTable[10] = {"SSLLL", "LLLSS", "LLSLS", "LLSSL", "LSLLS", "LSLSL", "LSSLL", "SLLLS",
	"SLLSL", "SLSLL"};

static char *FIMTable[4] = {"12121112121", "111112111211111", "121111111111121", "13111111131"};

static char *RoyalValues[36] = {"11", "12", "13", "14", "15", "10", "21", "22", "23", "24", "25",
	"20", "31", "32", "33", "34", "35", "30", "41", "42", "43", "44", "45", "40", "51", "52",
	"53", "54", "55", "50", "01", "02", "03", "04", "05", "00"};

static char *RoyalTable[36] = {"3300", "3210", "3201", "2310", "2301", "2211", "3120", "3030", "3021",
	"2130", "2121", "2031", "3102", "3012", "3003", "2112", "2103", "2013", "1320", "1230",
	"1221", "0330", "0321", "0231", "1302", "1212", "1203", "0312", "0303", "0213", "1122",
	"1032", "1023", "0132", "0123", "0033"};

static char *FlatTable[10] = {"0504", "18", "0117", "0216", "0315", "0414", "0513", "0612", "0711",
	"0810"};

int postnet(struct zint_symbol *symbol, unsigned char source[], char dest[])
{
	/* Handles the PostNet system used for Zip codes in the US */
	unsigned int i, sum, check_digit;
	int errno, h;
	
	errno = 0;

	if(strlen(source) > 90) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	errno = is_sane(NESET, source);
	if(errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return errno;
	}
	sum = 0;

	/* start character */
	concat (dest, "L");

	for (i=0; i < strlen(source); i++)
	{
		lookup(NESET, PNTable, source[i], dest);
		sum += ctoi(source[i]);
	}

	check_digit = 10 - (sum%10);
	concat(dest, PNTable[check_digit]);

	/* stop character */
	concat (dest, "L");
	
	h = strlen(source);
	source[h] = itoc(check_digit);
	source[h + 1] = '\0';
	strcpy(symbol->text, "");
	
	return errno;
}
 
int post_plot(struct zint_symbol *symbol, unsigned char source[])
{
	/* Puts PostNet barcodes into the pattern matrix */
	char height_pattern[200];
	unsigned int loopey;
	int writer;
	strcpy(height_pattern, "");
	int errno;
	
	errno = 0;

	errno = postnet(symbol, source, height_pattern);
	if(errno != 0) {
		return errno;
	}

	writer = 0;
	for(loopey = 0; loopey < strlen(height_pattern); loopey++)
	{
		if(height_pattern[loopey] == 'L')
		{
			symbol->encoded_data[0][writer] = '1';
		}
		symbol->encoded_data[1][writer] = '1';
		writer += 2;
	}
	symbol->row_height[0] = 6;
	symbol->row_height[1] = 6;
	symbol->rows = 2;
	symbol->width = writer - 1;
	
	return errno;
}

int planet(struct zint_symbol *symbol, unsigned char source[], char dest[])
{
	/* Handles the PLANET  system used for item tracking in the US */
	unsigned int i, sum, check_digit;
	int errno, h;
	
	errno = 0;
	
	if(strlen(source) > 90) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	errno = is_sane(NESET, source);
	if(errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return errno;
	}
	sum = 0;

	/* start character */
	concat (dest, "L");

	for (i=0; i < strlen(source); i++)
	{
		lookup(NESET, PLTable, source[i], dest);
		sum += ctoi(source[i]);
	}

	check_digit = 10 - (sum%10);
	concat(dest, PLTable[check_digit]);

	/* stop character */
	concat (dest, "L");
	
	h = strlen(source);
	source[h] = itoc(check_digit);
	source[h + 1] = '\0';
	strcpy(symbol->text, "");

	return errno;
}

int planet_plot(struct zint_symbol *symbol, unsigned char source[])
{
	/* Puts PLANET barcodes into the pattern matrix */
	char height_pattern[200];
	unsigned int loopey;
	int writer;
	int errno;
	strcpy(height_pattern, "");
	
	errno = 0;

	errno = planet(symbol, source, height_pattern);
	if(errno != 0) {
		return errno;
	}

	writer = 0;
	for(loopey = 0; loopey < strlen(height_pattern); loopey++)
	{
		if(height_pattern[loopey] == 'L')
		{
			symbol->encoded_data[0][writer] = '1';
		}
		symbol->encoded_data[1][writer] = '1';
		writer += 2;
	}
	symbol->row_height[0] = 6;
	symbol->row_height[1] = 6;
	symbol->rows = 2;
	symbol->width = writer - 1;
	return errno;
}

int fim(struct zint_symbol *symbol, unsigned char source[])
{
	/* The simplest barcode symbology ever! Supported by MS Word, so here it is! */
	/* glyphs from http://en.wikipedia.org/wiki/Facing_Identification_Mark */
	
	int errno;
	char dest[17];
	
	errno = 0;
	strcpy(dest, "");
	
	to_upper(source);
	if(strlen(source) > 1) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	errno = is_sane(BESET, source);
	if(errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return errno;
	}
	lookup(BESET, FIMTable, source[0], dest);
	
	expand(symbol, dest);
	strcpy(symbol->text, "");
	return errno;
}

char rm4scc(char source[], unsigned char dest[])
{
	/* Handles the 4 State barcodes used in the UK by Royal Mail */
	unsigned int i;
	int top, bottom, row, column, check_digit;
	char values[3], set_copy[38];
	strcpy(set_copy, KRSET);

	top = 0;
	bottom = 0;

	/* start character */
	concat (dest, "1");

	for (i=0; i < strlen(source); i++) {
		lookup(KRSET, RoyalTable, source[i], dest);
		strcpy(values, RoyalValues[posn(KRSET, source[i])]);
		top += ctoi(values[0]);
		bottom += ctoi(values[1]);
	}

	/* Calculate the check digit */
	row = (top % 6) - 1;
	column = (bottom % 6) - 1;
	if(row == -1) { row = 5; }
	if(column == -1) { column = 5; }
	check_digit = (6 * row) + column;
	concat(dest, RoyalTable[check_digit]);

	/* stop character */
	concat (dest, "0");
	
	return set_copy[check_digit];
}

int royal_plot(struct zint_symbol *symbol, unsigned char source[])
{
	/* Puts RM4SCC into the data matrix */
	char height_pattern[200], check;
	unsigned int loopey;
	int writer;
	int errno, h;
	strcpy(height_pattern, "");

	errno = 0;
	
	to_upper(source);
	if(strlen(source) > 120) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	errno = is_sane(KRSET, source);
	if(errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return errno;
	}
	check = rm4scc(source, height_pattern);
	
	writer = 0;
	for(loopey = 0; loopey < strlen(height_pattern); loopey++)
	{
		if((height_pattern[loopey] == '1') || (height_pattern[loopey] == '0'))
		{
			symbol->encoded_data[0][writer] = '1';
		}
		symbol->encoded_data[1][writer] = '1';
		if((height_pattern[loopey] == '2') || (height_pattern[loopey] == '0'))
		{
			symbol->encoded_data[2][writer] = '1';
		}
		writer += 2;
	}

	symbol->row_height[0] = 4;
	symbol->row_height[1] = 2;
	symbol->row_height[2] = 4;
	symbol->rows = 3;
	symbol->width = writer - 1;
	
	h = strlen(source);
	source[h] = check;
	source[h + 1] = '\0';
	strcpy(symbol->text, "");
	
	return errno;
}

int flattermarken(struct zint_symbol *symbol, unsigned char source[])
{ /* Flattermarken - Not really a barcode symbology and (in my opinion) probably not much use
	but it's supported by TBarCode so it's supported by Zint! */
	int loop, errno;
	char dest[1000];
	
	errno = 0;
	strcpy(dest, "");
	
	if(strlen(source) > 90) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	errno = is_sane(NESET, source);
	if(errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return errno;
	}
	
	for(loop = 0; loop < strlen(source); loop++) {
		lookup(NESET, FlatTable, source[loop], dest);
	}
	
	expand(symbol, dest);
	strcpy(symbol->text, "");	
	return errno;
}
