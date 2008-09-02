/* medical.c - Handles 1 track and 2 track pharmacode and Codabar */

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

/* Codabar table checked against EN 798:1995 */

#define CASET	"0123456789-$:/.+ABCD"

static char *CodaTable[20] = {"11111221", "11112211", "11121121", "22111111", "11211211", "21111211",
	"12111121", "12112111", "12211111", "21121111", "11122111", "11221111", "21112121", "21211121",
	"21212111", "11212121", "11221211", "12121121", "11121221", "11122211"};
	
int pharma_one(struct zint_symbol *symbol, unsigned char source[])
{
	/* "Pharmacode can represent only a single integer from 3 to 131070. Unlike other
	   commonly used one-dimensional barcode schemes, pharmacode does not store the data in a
	   form corresponding to the human-readable digits; the number is encoded in binary, rather
	   than decimal. Pharmacode is read from right to left: with n as the bar position starting
	   at 0 on the right, each narrow bar adds 2n to the value and each wide bar adds 2(2^n).
	   The minimum barcode is 2 bars and the maximum 16, so the smallest number that could
	   be encoded is 3 (2 narrow bars) and the biggest is 131070 (16 wide bars)."
	   - http://en.wikipedia.org/wiki/Pharmacode */

	/* This code uses the One Track Pharamacode calculating algorithm as recommended by
	   the specification at http://www.laetus.com/laetus.php?request=file&id=69 */

	unsigned long int tester;
	int counter;
	char inter[17];
	int error_number;
	char dest[1000];
	
	error_number = 0;
	strcpy(dest, "");

	if(strlen(source) > 6) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return error_number;
	}

	strcpy(inter, "");
	tester = atoi(source);

	if((tester < 3) || (tester > 131070)) {
		strcpy(symbol->errtxt, "error: data out of range");
		return ERROR_INVALID_DATA;
	}

	do
	{
		if(tester%2 == 0) {
			concat(inter, "W");
			tester = (tester - 2) / 2;
		} else {
			concat(inter, "N");
			tester = (tester - 1) / 2;
		}
	}
	while(tester != 0);

	for(counter = (strlen(inter) - 1); counter >= 0; counter--) {
		if(inter[counter] == 'W') {
			concat(dest, "32");
		} else {
			concat(dest, "12");
		}
	}
	
	expand(symbol, dest);
	strcpy(symbol->text, "");
	return error_number;
}

int pharma_two_calc(struct zint_symbol *symbol, unsigned char source[], char dest[])
{
	/* This code uses the Two Track Pharamacode defined in the document at
	   http://www.laetus.com/laetus.php?request=file&id=69 and using a modified
	   algorithm from the One Track system. This standard accepts integet values
	   from 4 to 64570080. */

	unsigned long int tester;
	int counter;
	char inter[17];
	int error_number;
	
	error_number = 0;
	strcpy(dest, "");
	
	strcpy(inter, "");
	tester = atoi(source);

	if((tester < 4) || (tester > 64570080))
	{
		strcpy(symbol->errtxt, "error: data out of range");
		return ERROR_INVALID_DATA;
	}
	do
	{
		switch(tester%3) {
			case 0:
			concat(inter, "3");
			tester = (tester - 3) / 3;
			break;
			case 1:
			concat(inter, "1");
			tester = (tester - 1) / 3;
			break;
			case 2:
			concat(inter, "2");
			tester = (tester - 2) / 3;
			break;
		}
	}
	while(tester != 0);

	for(counter = (strlen(inter) - 1); counter >= 0; counter--)
	{
		dest[(strlen(inter) - 1) - counter] = inter[counter];
	}
	dest[strlen(inter)] = '\0';
	
	return error_number;
}

int pharma_two(struct zint_symbol *symbol, unsigned char source[])
{
	/* Draws the patterns for two track pharmacode */
	char height_pattern[200];
	unsigned int loopey;
	int writer;
	int error_number = 0;
	strcpy(height_pattern, "");

	if(strlen(source) > 8) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return error_number;
	}
	error_number = pharma_two_calc(symbol, source, height_pattern);
	if(error_number != 0) {
		return error_number;
	}

	writer = 0;
	for(loopey = 0; loopey < strlen(height_pattern); loopey++)
	{
		if((height_pattern[loopey] == '2') || (height_pattern[loopey] == '3'))
		{
			symbol->encoded_data[0][writer] = '1';
		}
		if((height_pattern[loopey] == '1') || (height_pattern[loopey] == '3'))
		{
			symbol->encoded_data[1][writer] = '1';
		}
		writer += 2;
	}
	symbol->rows = 2;
	symbol->width = writer - 1;
	
	strcpy(symbol->text, "");
	return error_number;
}

int codabar(struct zint_symbol *symbol, unsigned char source[])
{ /* The Codabar system consisting of simple substitution */

	int i, error_number;
	char dest[1000];
	
	error_number = 0;
	strcpy(dest, "");

	if(strlen(source) > 60) { /* No stack smashing please */
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	to_upper(source);
	error_number = is_sane(CASET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return error_number;
	}

	/* Codabar must begin and end with the characters A, B, C or D */
	if(((source[0] != 'A') && (source[0] != 'B')) &&
		    ((source[0] != 'C') && (source[0] != 'D')))
	{
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return 6;
	}

	if(((source[strlen(source) - 1] != 'A') && (source[strlen(source) - 1] != 'B')) &&
		    ((source[strlen(source) - 1] != 'C') && (source[strlen(source) - 1] != 'D')))
	{
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return 6;
	}

	for(i = 0; i <= strlen(source); i++)
	{
		lookup(CASET, CodaTable, source[i], dest);
	}
	
	expand(symbol, dest);
	strcpy(symbol->text, source);
	return error_number;
}

int code32(struct zint_symbol *symbol, unsigned char source[])
{ /* Italian Pharmacode */
	int i, zeroes, error_number, checksum, checkpart, checkdigit;
	char localstr[10], tempstr[2], risultante[7];
	long int pharmacode, remainder, devisor;
	int codeword[6];
	char tabella[34];
	
	/* Validate the input */
	if(strlen(source) > 8) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return error_number;
	}
	
	/* Add leading zeros as required */
	strcpy(localstr, "");
	zeroes = 8 - strlen(source);
	for(i = 0; i < zeroes; i++) {
		concat(localstr, "0");
	}
	concat(localstr, source);
	
	/* Calculate the check digit */
	checksum = 0;
	checkpart = 0;
	for(i = 0; i < 4; i++) {
		checkpart = ctoi(localstr[i * 2]);
		checksum += checkpart;
		checkpart = 2 * (ctoi(localstr[(i * 2) + 1]));
		if(checkpart >= 10) {
			checksum += (checkpart - 10) + 1;
		} else {
			checksum += checkpart;
		}
	}
	
	/* Add check digit to data string */
	checkdigit = checksum % 10;
	tempstr[0] = itoc(checkdigit);
	tempstr[1] = '\0';
	concat(localstr, tempstr);
	
	/* Convert string into an integer value */
	pharmacode = 0;
	for(i = 0; i < 9; i++) {
		pharmacode *= 10;
		pharmacode += ctoi(localstr[i]);
	}

	/* Convert from decimal to base-32 */
	devisor = 33554432;
	for(i = 5; i >= 0; i--) {
		codeword[i] = pharmacode / devisor;
		remainder = pharmacode % devisor;
		pharmacode = remainder;
		devisor /= 32;
	}
	
	/* Look up values in 'Tabella di conversione' */
	strcpy(tabella, "0123456789BCDFGHJKLMNPQRSTUVWXYZ");
	strcpy(risultante, "");
	for(i = 5; i >= 0; i--) {
		tempstr[0] = tabella[codeword[i]];
		concat(risultante, tempstr);
	}
	
	/* Plot the barcode using Code 39 */
	error_number = c39(symbol, risultante);
	if(error_number != 0) { return error_number; }
	
	/* Override the normal text output with the Pharmacode number */
	strcpy(symbol->text, "A");
	concat(symbol->text, localstr);
	
	return error_number;
}
