/*  upcean.c - Handles UPC, EAN and ISBN

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

#define NASET	"0123456789+"
#define EAN2	102
#define EAN5	105

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

/* UPC and EAN tables checked against EN 797:1996 */

static char *UPCParity0[10] = {"BBBAAA", "BBABAA", "BBAABA", "BBAAAB", "BABBAA", "BAABBA", "BAAABB",
	"BABABA", "BABAAB", "BAABAB"}; /* Number set for UPC-E symbol (EN Table 4) */
static char *UPCParity1[10] = {"AAABBB", "AABABB", "AABBAB", "AABBBA", "ABAABB", "ABBAAB", "ABBBAA",
	"ABABAB", "ABABBA", "ABBABA"}; /* Not covered by BS EN 797:1995 */
static char *EAN2Parity[4] = {"AA", "AB", "BA", "BB"}; /* Number sets for 2-digit add-on (EN Table 6) */
static char *EAN5Parity[10] = {"BBAAA", "BABAA", "BAABA", "BAAAB", "ABBAA", "AABBA", "AAABB", "ABABA",
	"ABAAB", "AABAB"}; /* Number set for 5-digit add-on (EN Table 7) */
static char *EAN13Parity[10] = {"AAAAA", "ABABB", "ABBAB", "ABBBA", "BAABB", "BBAAB", "BBBAA", "BABAB",
	"BABBA", "BBABA"}; /* Left hand of the EAN-13 symbol (EN Table 3) */
static char *EANsetA[10] = {"3211", "2221", "2122", "1411", "1132", "1231", "1114", "1312", "1213",
	"3112"}; /* Representation set A and C (EN Table 1) */
static char *EANsetB[10] = {"1123", "1222", "2212", "1141", "2311", "1321", "4111", "2131", "3121",
	"2113"}; /* Representation set B (EN Table 1) */

char upc_check(unsigned char source[])
{ /* Calculate the correct check digit for a UPC barcode */
	unsigned int i, count, check_digit;

	count = 0;

	for (i = 0; i < strlen(source); i++)
	{
		count += ctoi(source[i]);

		if ((i%2) == 0)
		{
			count += 2 * (ctoi(source[i]));
		}
	}

	check_digit = 10 - (count%10);
	if (check_digit == 10) { check_digit = 0; }
	return itoc(check_digit);
}

void upca_draw(unsigned char source[], char dest[])
{ /* UPC A is usually used for 12 digit numbers, but this function takes a source of any length */
	unsigned int i, half_way;

	half_way = strlen(source) / 2;

	/* start character */
	concat (dest, "111");

	for(i = 0; i <= strlen(source); i++)
	{
		if (i == half_way)
		{
			/* middle character - separates manufacturer no. from product no. */
			/* also inverts right hand characters */
			concat(dest, "11111");
		}

		lookup(NESET, EANsetA, source[i], dest);
	}

	/* stop character */
	concat (dest, "111");
}

void upca(struct zint_symbol *symbol, unsigned char source[], char dest[])
{ /* Make a UPC A barcode when we haven't been given the check digit */
	int length;

	length = strlen(source);
	source[length] = upc_check(source);
	source[length + 1] = '\0';
	upca_draw(source, dest);
	strcpy(symbol->text, source);
}

void upce(struct zint_symbol *symbol, unsigned char source[], char dest[])
{ /* UPC E is a zero-compressed version of UPC A */
	unsigned int i, num_system;
	char emode, equivalent[12], check_digit, parity[8], temp[8];
	char hrt[8];

	/* Two number systems can be used - system 0 and system 1 */
	if(strlen(source) == 7) {
		switch(source[0]) {
			case '0': num_system = 0; break;
			case '1': num_system = 1; break;
			default: num_system = 0; source[0] = '0'; break;
		}
		strcpy(temp, source);
		strcpy(hrt, source);
		for(i = 1; i <= 7; i++) {
			source[i - 1] = temp[i];
		}
	}
	else {
		num_system = 0;
		hrt[0] = '0';
		hrt[1] = '\0';
		concat(hrt, source);
	}

	/* Expand the zero-compressed UPCE code to make a UPCA equivalent (EN Table 5) */
	emode = source[5];
	for(i = 0; i < 11; i++) {
		equivalent[i] = '0';
	}
	equivalent[1] = source[0];
	equivalent[2] = source[1];
	equivalent[11] = '\0';

	switch(emode)
	{
		case '0':
		case '1':
		case '2':
			equivalent[3] = emode;
			equivalent[8] = source[2];
			equivalent[9] = source[3];
			equivalent[10] = source[4];
			break;
		case '3':
			equivalent[3] = source[2];
			equivalent[9] = source[3];
			equivalent[10] = source[4];
			if(((source[2] == '0') || (source[2] == '1')) || (source[2] == '2')) {
				/* Note 1 - "X3 shall not be equal to 0, 1 or 2" */
				strcpy(symbol->errtxt, "warning: invalid UPC-E data");
			}
			break;
		case '4':
			equivalent[3] = source[2];
			equivalent[4] = source[3];
			equivalent[10] = source[4];
			if(source[3] == '0') {
				/* Note 2 - "X4 shall not be equal to 0" */
				strcpy(symbol->errtxt, "warning: invalid UPC-E data");
			}
			break;
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			equivalent[3] = source[2];
			equivalent[4] = source[3];
			equivalent[5] = source[4];
			equivalent[10] = emode;
			if(source[4] == '0') {
				/* Note 3 - "X5 shall not be equal to 0" */
				strcpy(symbol->errtxt, "warning: invalid UPC-E data");
			}
			break;
	}

	/* Get the check digit from the expanded UPCA code */

	check_digit = upc_check(equivalent);

	/* Use the number system and check digit information to choose a parity scheme */
	if(num_system == 1) {
		strcpy(parity, UPCParity1[ctoi(check_digit)]);
	} else {
		strcpy(parity, UPCParity0[ctoi(check_digit)]);
	}

	/* Take all this information and make the barcode pattern */

	/* start character */
	concat (dest, "111");

	for(i = 0; i <= strlen(source); i++) {
		switch(parity[i]) {
			case 'A': lookup(NESET, EANsetA, source[i], dest); break;
			case 'B': lookup(NESET, EANsetB, source[i], dest); break;
		}
	}

	/* stop character */
	concat (dest, "111111");
	
	hrt[7] = check_digit;
	hrt[8] = '\0';
	strcpy(symbol->text, hrt);
}


void add_on(unsigned char source[], char dest[], int mode)
{ /* EAN-2 and EAN-5 add-on codes */
	char parity[6];
	unsigned int i, code_type;

	/* If an add-on then append with space */
	if (mode != 0)
	{
		concat(dest, "9");
	}

	/* Start character */
	concat (dest, "112");

	/* Determine EAN2 or EAN5 add-on */
	if(strlen(source) == 2)
	{
		code_type = EAN2;
	}
	else
	{
		code_type = EAN5;
	}

	/* Calculate parity for EAN2 */
	if(code_type == EAN2)
	{
		int code_value, parity_bit;

		code_value = (10 * ctoi(source[0])) + ctoi(source[1]);
		parity_bit = code_value%4;
		strcpy(parity, EAN2Parity[parity_bit]);
	}

	if(code_type == EAN5)
	{
		int values[6], parity_sum, parity_bit;

		for(i = 0; i < 6; i++)
		{
			values[i] = ctoi(source[i]);
		}

		parity_sum = (3 * (values[0] + values[2] + values[4]));
		parity_sum += (9 * (values[1] + values[3]));

		parity_bit = parity_sum%10;
		strcpy(parity, EAN5Parity[parity_bit]);
	}

	for(i = 0; i < strlen(source); i++)
	{
		switch(parity[i]) {
			case 'A': lookup(NESET, EANsetA, source[i], dest); break;
			case 'B': lookup(NESET, EANsetB, source[i], dest); break;
		}

		/* Glyph separator */
		if(i != (strlen(source) - 1))
		{
			concat (dest, "11");
		}
	}
}


/* ************************ EAN-13 ****************** */

char ean_check(unsigned char source[])
{ /* Calculate the correct check digit for a EAN-13 barcode */
	int i;
	unsigned int h, count, check_digit;

	count = 0;

	h = strlen(source);
	for (i = h - 1; i >= 0; i--)
	{
		count += ctoi(source[i]);

		if (!((i%2) == 0))
		{
			count += 2 * ctoi(source[i]);
		}
	}
	check_digit = 10 - (count%10);
	if (check_digit == 10) { check_digit = 0; }
	return itoc(check_digit);
}

void ean13(struct zint_symbol *symbol, unsigned char source[], char dest[])
{
	unsigned int length, i, half_way;
	char parity[6];

	strcpy(parity, "");
	
	/* Add the appropriate check digit */
	length = strlen(source);
	source[length] = ean_check(source);
	source[length + 1] = '\0';

	/* Get parity for first half of the symbol */
	lookup(NASET, EAN13Parity, source[0], parity);

	/* Now get on with the cipher */
	half_way = 7;

	/* start character */
	concat (dest, "111");

	for(i = 1; i <= strlen(source); i++)
	{
		if (i == half_way)
		{
			/* middle character - separates manufacturer no. from product no. */
			/* also inverses right hand characters */
			concat (dest, "11111");
		}

		if(((i > 1) && (i < 7)) && (parity[i - 2] == 'B'))
		{
			lookup(NESET, EANsetB, source[i], dest);
		}
		else
		{
			lookup(NESET, EANsetA, source[i], dest);
		}
	}

	/* stop character */
	concat (dest, "111");
	
	strcpy(symbol->text, source);
}

void ean8(struct zint_symbol *symbol, unsigned char source[], char dest[])
{ /* Make an EAN-8 barcode when we haven't been given the check digit */
  /* EAN-8 is basically the same as UPC-A but with fewer digits */
	int length;

	length = strlen(source);
	source[length] = upc_check(source);
	source[length + 1] = '\0';
	upca_draw(source, dest);
	strcpy(symbol->text, source);
}

char isbn13_check(unsigned char source[]) /* For ISBN(13) only */
{
	unsigned int i, weight, sum, check;

	sum = 0;
	weight = 1;

	for(i = 0; i < (strlen(source) - 1); i++)
	{
		sum += ctoi(source[i]) * weight;
		if(weight == 1) weight = 3; else weight = 1;
	}

	check = sum % 10;
	check = 10 - check;
	return itoc(check);
}

char isbn_check(unsigned char source[]) /* For ISBN(10) and SBN only */
{
	unsigned int i, weight, sum, check;

	sum = 0;
	weight = 1;
	for(i = 0; i < (strlen(source) - 1); i++)
	{
		sum += ctoi(source[i]) * weight;
		weight++;
	}

	check = sum % 11;
	return itoc(check);
}

int isbn(struct zint_symbol *symbol, unsigned char source[], char dest[]) /* Make an EAN-13 barcode from an SBN or ISBN */
{
	int i, errno;
	char check_digit;

	errno = 0;
	
	to_upper(source);
	errno = is_sane("0123456789X", source);
	if(errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in input");
		return errno;
	}

	/* Input must be 9, 10 or 13 characters */
	if(((strlen(source) < 9) || (strlen(source) > 13)) || ((strlen(source) > 10) && (strlen(source) < 13)))
	{
		strcpy(symbol->errtxt, "error: input wrong length");
		return ERROR_TOO_LONG;
	}

	if(strlen(source) == 13) /* Using 13 character ISBN */
	{
		if(!(((source[0] == '9') && (source[1] == '7')) &&
				     ((source[2] == '8') || (source[2] == '9'))))
		{
			strcpy(symbol->errtxt, "error: invalid ISBN");
			return ERROR_INVALID_DATA;
		}

		check_digit = isbn13_check(source);
		if (source[strlen(source) - 1] != check_digit)
		{
			strcpy(symbol->errtxt, "error: incorrect ISBN check");
			return ERROR_INVALID_CHECK;
		}
		source[12] = '\0';

		ean13(symbol, source, dest);
	}

	if(strlen(source) == 10) /* Using 10 digit ISBN */
	{
		check_digit = isbn_check(source);
		if(check_digit != source[strlen(source) - 1])
		{
			strcpy(symbol->errtxt, "error: incorrect ISBN check");
			return ERROR_INVALID_CHECK;
		}
		for(i = 13; i > 0; i--)
		{
			source[i] = source[i - 3];
		}
		source[0] = '9';
		source[1] = '7';
		source[2] = '8';
		source[12] = '\0';

		ean13(symbol, source, dest);
	}

	if(strlen(source) == 9) /* Using 9 digit SBN */
	{
		/* Add leading zero */
		for(i = 10; i > 0; i--)
		{
			source[i] = source[i - 1];
		}
		source[0] = '0';

		/* Verify check digit */
		check_digit = isbn_check(source);
		if(check_digit != source[strlen(source) - 1])
		{
			strcpy(symbol->errtxt, "error: incorrect SBN check");
			return ERROR_INVALID_CHECK;
		}

		/* Convert to EAN-13 number */
		for(i = 13; i > 0; i--)
		{
			source[i] = source[i - 3];
		}
		source[0] = '9';
		source[1] = '7';
		source[2] = '8';
		source[12] = '\0';

		ean13(symbol, source, dest);
	}
	
	return errno;
}

int eanx(struct zint_symbol *symbol, unsigned char source[])
{
	/* splits string to parts before and after '+' parts */
	char first_part[20], second_part[20], dest[1000];
	unsigned int latch, reader, writer, with_addon;
	strcpy(first_part, "");
	strcpy(second_part, "");
	int errno, i;
	
	errno = 0;
	strcpy(dest, "");

	with_addon = FALSE;
	latch = FALSE;
	writer = 0;

	if(strlen(source) > 19) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	if(symbol->symbology != BARCODE_ISBNX) {
		/* ISBN has it's own checking routine */
		errno = is_sane(NASET, source);
		if(errno == ERROR_INVALID_DATA) {
			strcpy(symbol->errtxt, "error: invalid characters in data");
			return errno;
		}
	}
	
	for(reader = 0; reader <= strlen(source); reader++)
	{
		if(source[reader] == '+') { with_addon = TRUE; }
	}

	reader = 0;
	if(with_addon) {
		do {
			if(source[reader] == '+') {
				first_part[writer] = '\0';
				latch = TRUE;
				reader++;
				writer = 0;
			}

			if(latch) {
				second_part[writer] = source[reader];
				reader++;
				writer++;
			} else {
				first_part[writer] = source[reader];
				reader++;
				writer++;
			}
		} while (reader <= strlen(source));
	} else {
		strcpy(first_part, source);
	}


	switch(symbol->symbology)
	{
		case BARCODE_EANX:
			switch(strlen(first_part))
			{
				case 2: add_on(first_part, dest, 0); strcpy(symbol->text, first_part); break;
				case 5: add_on(first_part, dest, 0); strcpy(symbol->text, first_part); break;
				case 7: ean8(symbol, first_part, dest); break;
				case 12: ean13(symbol, first_part, dest); break;
				default: strcpy(symbol->errtxt, "error: invalid length input"); return ERROR_TOO_LONG; break;
			}
			break;
		case BARCODE_EANX_CC:
			switch(strlen(first_part))
			{ /* Adds vertical separator bars according to ISO/IEC 24723 section 11.4 */
				case 7: symbol->encoded_data[symbol->rows][1] = '1';
					symbol->encoded_data[symbol->rows][67] = '1';
					symbol->encoded_data[symbol->rows + 1][0] = '1';
					symbol->encoded_data[symbol->rows + 1][68] = '1';
					symbol->encoded_data[symbol->rows + 2][1] = '1';
					symbol->encoded_data[symbol->rows + 2][67] = '1';
					symbol->row_height[symbol->rows] = 2;
					symbol->row_height[symbol->rows + 1] = 2;
					symbol->row_height[symbol->rows + 2] = 2;
					symbol->rows += 3;
					ean8(symbol, first_part, dest); break;
				case 12:symbol->encoded_data[symbol->rows][1] = '1';
					symbol->encoded_data[symbol->rows][95] = '1';
					symbol->encoded_data[symbol->rows + 1][0] = '1';
					symbol->encoded_data[symbol->rows + 1][96] = '1';
					symbol->encoded_data[symbol->rows + 2][1] = '1';
					symbol->encoded_data[symbol->rows + 2][95] = '1';
					symbol->row_height[symbol->rows] = 2;
					symbol->row_height[symbol->rows + 1] = 2;
					symbol->row_height[symbol->rows + 2] = 2;
					symbol->rows += 3;
					ean13(symbol, first_part, dest); break;
				default: strcpy(symbol->errtxt, "error: invalid length EAN input"); return ERROR_TOO_LONG; break;
			}
			break;
		case BARCODE_UPCA:
			if(strlen(first_part) == 11) {
				upca(symbol, first_part, dest);
			} else {
				strcpy(symbol->errtxt, "error: input wrong length");
				return ERROR_TOO_LONG;
			}
			break;
		case BARCODE_UPCA_CC:
			if(strlen(first_part) == 11) {
				symbol->encoded_data[symbol->rows][1] = '1';
				symbol->encoded_data[symbol->rows][95] = '1';
				symbol->encoded_data[symbol->rows + 1][0] = '1';
				symbol->encoded_data[symbol->rows + 1][96] = '1';
				symbol->encoded_data[symbol->rows + 2][1] = '1';
				symbol->encoded_data[symbol->rows + 2][95] = '1';
				symbol->row_height[symbol->rows] = 2;
				symbol->row_height[symbol->rows + 1] = 2;
				symbol->row_height[symbol->rows + 2] = 2;
				symbol->rows += 3;
				upca(symbol, first_part, dest);
			} else {
				strcpy(symbol->errtxt, "error: UPCA input wrong length");
				return ERROR_TOO_LONG;
			}
			break;
		case BARCODE_UPCE:
			if((strlen(first_part) >= 6) && (strlen(first_part) <= 7)) {
				upce(symbol, first_part, dest);
			} else {
				strcpy(symbol->errtxt, "error: input wrong length");
				return ERROR_TOO_LONG;
			}
			break;
		case BARCODE_UPCE_CC:
			if((strlen(first_part) >= 6) && (strlen(first_part) <= 7)) {
				symbol->encoded_data[symbol->rows][1] = '1';
				symbol->encoded_data[symbol->rows][51] = '1';
				symbol->encoded_data[symbol->rows + 1][0] = '1';
				symbol->encoded_data[symbol->rows + 1][52] = '1';
				symbol->encoded_data[symbol->rows + 2][1] = '1';
				symbol->encoded_data[symbol->rows + 2][51] = '1';
				symbol->row_height[symbol->rows] = 2;
				symbol->row_height[symbol->rows + 1] = 2;
				symbol->row_height[symbol->rows + 2] = 2;
				symbol->rows += 3;
				upce(symbol, first_part, dest);
			} else {
				strcpy(symbol->errtxt, "error: UPCE input wrong length");
				return ERROR_TOO_LONG;
			}
			break;
		case BARCODE_ISBNX:
			errno = isbn(symbol, first_part, dest);
			if(errno > 4) {
				return errno;
			}
			break;
	}
	switch(strlen(second_part))
	{
		case 0: break;
		case 2:
			add_on(second_part, dest, 1);
			concat(symbol->text, "+");
			concat(symbol->text, second_part);
			break;
		case 5:
			add_on(second_part, dest, 1);
			concat(symbol->text, "+");
			concat(symbol->text, second_part);
			break;
		default:
			strcpy(symbol->errtxt, "error: invalid length input");
			return ERROR_TOO_LONG;
			break;
	}
	
	expand(symbol, dest);

	switch(symbol->symbology) {
		case BARCODE_EANX_CC:
		case BARCODE_UPCA_CC:
		case BARCODE_UPCE_CC:
			/* shift the symbol to the right one space to allow for separator bars */
			for(i = (symbol->width + 1); i >= 1; i--) {
				symbol->encoded_data[symbol->rows - 1][i] = symbol->encoded_data[symbol->rows - 1][i - 1];
			}
			symbol->encoded_data[symbol->rows - 1][0] = '0';
			symbol->width += 2;
			break;
	}

	
	if((symbol->errtxt[0] == 'w') && (errno == 0)) {
		errno = 1; /* flag UPC-E warnings */
	}
	return errno;
}




