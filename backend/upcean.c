/*  upcean.c - Handles UPC, EAN and ISBN

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

#define SODIUM	"0123456789+"
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

char upc_check(char source[])
{ /* Calculate the correct check digit for a UPC barcode */
	unsigned int i, count, check_digit;

	count = 0;

	for (i = 0; i < strlen(source); i++) {
		count += ctoi(source[i]);

		if (!(i & 1)) {
			count += 2 * (ctoi(source[i]));
		}
	}

	check_digit = 10 - (count%10);
	if (check_digit == 10) { check_digit = 0; }
	return itoc(check_digit);
}

void upca_draw(char source[], char dest[])
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

		lookup(NEON, EANsetA, source[i], dest);
	}

	/* stop character */
	concat (dest, "111");
}

void upca(struct zint_symbol *symbol, unsigned char source[], char dest[])
{ /* Make a UPC A barcode when we haven't been given the check digit */
	int length;
	char gtin[15];

	strcpy(gtin, (char*)source);
	length = strlen(gtin);
	gtin[length] = upc_check(gtin);
	gtin[length + 1] = '\0';
	upca_draw(gtin, dest);
	ustrcpy(symbol->text, (unsigned char*)gtin);
}

void upce(struct zint_symbol *symbol, unsigned char source[], char dest[])
{ /* UPC E is a zero-compressed version of UPC A */
	unsigned int i, num_system;
	char emode, equivalent[12], check_digit, parity[8], temp[8];
	char hrt[9];

	/* Two number systems can be used - system 0 and system 1 */
	if(ustrlen(source) == 7) {
		switch(source[0]) {
			case '0': num_system = 0; break;
			case '1': num_system = 1; break;
			default: num_system = 0; source[0] = '0'; break;
		}
		strcpy(temp, (char*)source);
		strcpy(hrt, (char*)source);
		for(i = 1; i <= 7; i++) {
			source[i - 1] = temp[i];
		}
	}
	else {
		num_system = 0;
		hrt[0] = '0';
		hrt[1] = '\0';
		concat(hrt, (char*)source);
	}

	/* Expand the zero-compressed UPCE code to make a UPCA equivalent (EN Table 5) */
	emode = source[5];
	for(i = 0; i < 11; i++) {
		equivalent[i] = '0';
	}
	if(num_system == 1) { equivalent[0] = temp[0]; }
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
				strcpy(symbol->errtxt, "Invalid UPC-E data");
			}
			break;
		case '4':
			equivalent[3] = source[2];
			equivalent[4] = source[3];
			equivalent[10] = source[4];
			if(source[3] == '0') {
				/* Note 2 - "X4 shall not be equal to 0" */
				strcpy(symbol->errtxt, "Invalid UPC-E data");
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
				strcpy(symbol->errtxt, "Invalid UPC-E data");
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

	for(i = 0; i <= ustrlen(source); i++) {
		switch(parity[i]) {
			case 'A': lookup(NEON, EANsetA, source[i], dest); break;
			case 'B': lookup(NEON, EANsetB, source[i], dest); break;
		}
	}

	/* stop character */
	concat (dest, "111111");

	hrt[7] = check_digit;
	hrt[8] = '\0';
	ustrcpy(symbol->text, (unsigned char*)hrt);
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
	if(ustrlen(source) == 2)
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

	for(i = 0; i < ustrlen(source); i++)
	{
		switch(parity[i]) {
			case 'A': lookup(NEON, EANsetA, source[i], dest); break;
			case 'B': lookup(NEON, EANsetB, source[i], dest); break;
		}

		/* Glyph separator */
		if(i != (ustrlen(source) - 1))
		{
			concat (dest, "11");
		}
	}
}


/* ************************ EAN-13 ****************** */

char ean_check(char source[])
{ /* Calculate the correct check digit for a EAN-13 barcode */
	int i;
	unsigned int h, count, check_digit;

	count = 0;

	h = strlen(source);
	for (i = h - 1; i >= 0; i--) {
		count += ctoi(source[i]);

		if (i & 1) {
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
	char gtin[15];

	strcpy(parity, "");
	strcpy(gtin, (char*)source);

	/* Add the appropriate check digit */
	length = strlen(gtin);
	gtin[length] = ean_check(gtin);
	gtin[length + 1] = '\0';

	/* Get parity for first half of the symbol */
	lookup(SODIUM, EAN13Parity, gtin[0], parity);

	/* Now get on with the cipher */
	half_way = 7;

	/* start character */
	concat (dest, "111");
	length = strlen(gtin);
	for(i = 1; i <= length; i++)
	{
		if (i == half_way)
		{
			/* middle character - separates manufacturer no. from product no. */
			/* also inverses right hand characters */
			concat (dest, "11111");
		}

		if(((i > 1) && (i < 7)) && (parity[i - 2] == 'B'))
		{
			lookup(NEON, EANsetB, gtin[i], dest);
		}
		else
		{
			lookup(NEON, EANsetA, gtin[i], dest);
		}
	}

	/* stop character */
	concat (dest, "111");

	ustrcpy(symbol->text, (unsigned char*)gtin);
}

void ean8(struct zint_symbol *symbol, unsigned char source[], char dest[])
{ /* Make an EAN-8 barcode when we haven't been given the check digit */
  /* EAN-8 is basically the same as UPC-A but with fewer digits */
	int length;
	char gtin[10];

	strcpy(gtin, (char*)source);
	length = strlen(gtin);
	gtin[length] = upc_check(gtin);
	gtin[length + 1] = '\0';
	upca_draw(gtin, dest);
	ustrcpy(symbol->text, (unsigned char*)gtin);
}

char isbn13_check(unsigned char source[]) /* For ISBN(13) only */
{
	unsigned int i, weight, sum, check, h;

	sum = 0;
	weight = 1;
	h = ustrlen(source) - 1;

	for(i = 0; i < h; i++)
	{
		sum += ctoi(source[i]) * weight;
		if(weight == 1) weight = 3; else weight = 1;
	}

	check = sum % 10;
	check = 10 - check;
	if(check == 10) check = 0;
	return itoc(check);
}

char isbn_check(unsigned char source[]) /* For ISBN(10) and SBN only */
{
	unsigned int i, weight, sum, check, h;
	char check_char;

	sum = 0;
	weight = 1;
	h = ustrlen(source) - 1;

	for(i = 0; i < h; i++)
	{
		sum += ctoi(source[i]) * weight;
		weight++;
	}

	check = sum % 11;
	check_char = itoc(check);
	if(check == 10) { check_char = 'X'; }
	return check_char;
}

int isbn(struct zint_symbol *symbol, unsigned char source[], const unsigned int src_len, char dest[]) /* Make an EAN-13 barcode from an SBN or ISBN */
{
	int i, error_number;
	char check_digit;

	to_upper(source);
	error_number = is_sane("0123456789X", source, src_len);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in input");
		return error_number;
	}

	/* Input must be 9, 10 or 13 characters */
	if(((src_len < 9) || (src_len > 13)) || ((src_len > 10) && (src_len < 13)))
	{
		strcpy(symbol->errtxt, "Input wrong length");
		return ERROR_TOO_LONG;
	}

	if(src_len == 13) /* Using 13 character ISBN */
	{
		if(!(((source[0] == '9') && (source[1] == '7')) &&
				     ((source[2] == '8') || (source[2] == '9'))))
		{
			strcpy(symbol->errtxt, "Invalid ISBN");
			return ERROR_INVALID_DATA;
		}

		check_digit = isbn13_check(source);
		if (source[src_len - 1] != check_digit)
		{
			strcpy(symbol->errtxt, "Incorrect ISBN check");
			return ERROR_INVALID_CHECK;
		}
		source[12] = '\0';

		ean13(symbol, source, dest);
	}

	if(src_len == 10) /* Using 10 digit ISBN */
	{
		check_digit = isbn_check(source);
		if(check_digit != source[src_len - 1])
		{
			strcpy(symbol->errtxt, "Incorrect ISBN check");
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

	if(src_len == 9) /* Using 9 digit SBN */
	{
		/* Add leading zero */
		for(i = 10; i > 0; i--)
		{
			source[i] = source[i - 1];
		}
		source[0] = '0';

		/* Verify check digit */
		check_digit = isbn_check(source);
		if(check_digit != source[ustrlen(source) - 1])
		{
			strcpy(symbol->errtxt, "Incorrect SBN check");
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

	return 0;
}

void ean_leading_zeroes(struct zint_symbol *symbol, unsigned char source[], unsigned char local_source[]) {
	/* Add leading zeroes to EAN and UPC strings */
	unsigned char first_part[20], second_part[20], zfirst_part[20], zsecond_part[20];
	int with_addon = 0;
	int first_len = 0, second_len = 0, zfirst_len = 0, zsecond_len = 0, i, h;

	h = ustrlen(source);
	for(i = 0; i < h; i++) {
		if(source[i] == '+') {
			with_addon = 1;
		} else {
			if(with_addon == 0) {
				first_len++;
			} else {
				second_len++;
			}
		}
	}

	ustrcpy(first_part, (unsigned char *)"");
	ustrcpy(second_part, (unsigned char *)"");
	ustrcpy(zfirst_part, (unsigned char *)"");
	ustrcpy(zsecond_part, (unsigned char *)"");

	/* Split input into two strings */
	for(i = 0; i < first_len; i++) {
		first_part[i] = source[i];
		first_part[i + 1] = '\0';
	}

	for(i = 0; i < second_len; i++) {
		second_part[i] = source[i + first_len + 1];
		second_part[i + 1] = '\0';
	}

	/* Calculate target lengths */
	if(second_len <= 5) { zsecond_len = 5; }
	if(second_len <= 2) { zsecond_len = 2; }
	if(second_len == 0) { zsecond_len = 0; }
	switch(symbol->symbology) {
		case BARCODE_EANX:
		case BARCODE_EANX_CC:
			if(first_len <= 12) { zfirst_len = 12; }
			if(first_len <= 7) { zfirst_len = 7; }
			if(second_len == 0) {
				if(first_len <= 5) { zfirst_len = 5; }
				if(first_len <= 2) { zfirst_len = 2; }
			}
			break;
		case BARCODE_UPCA:
		case BARCODE_UPCA_CC:
			zfirst_len = 11;
			break;
		case BARCODE_UPCE:
		case BARCODE_UPCE_CC:
			if(first_len == 7) { zfirst_len = 7; }
			if(first_len <= 6) { zfirst_len = 6; }
			break;
		case BARCODE_ISBNX:
			if(first_len <= 9) { zfirst_len = 9; }
			break;
	}


	/* Add leading zeroes */
	for(i = 0; i < (zfirst_len - first_len); i++) {
		uconcat(zfirst_part, (unsigned char *)"0");
	}
	uconcat(zfirst_part, first_part);
	for(i = 0; i < (zsecond_len - second_len); i++) {
		uconcat(zsecond_part, (unsigned char *)"0");
	}
	uconcat(zsecond_part, second_part);

	/* Copy adjusted data back to local_source */
	uconcat(local_source, zfirst_part);
	if(zsecond_len != 0) {
		uconcat(local_source, (unsigned char *)"+");
		uconcat(local_source, zsecond_part);
	}
}

int eanx(struct zint_symbol *symbol, unsigned char source[], int src_len)
{
	/* splits string to parts before and after '+' parts */
	unsigned char first_part[20] = { 0 }, second_part[20] = { 0 }, dest[1000] = { 0 };
	unsigned char local_source[20] = { 0 };
	unsigned int latch, reader, writer, with_addon;
	int error_number, i;


	with_addon = FALSE;
	latch = FALSE;
	writer = 0;

	if(src_len > 19) {
		strcpy(symbol->errtxt, "Input too long");
		return ERROR_TOO_LONG;
	}
	if(symbol->symbology != BARCODE_ISBNX) {
		/* ISBN has it's own checking routine */
		error_number = is_sane("0123456789+", source, src_len);
		if(error_number == ERROR_INVALID_DATA) {
			strcpy(symbol->errtxt, "Invalid characters in data");
			return error_number;
		}
	} else {
		error_number = is_sane("0123456789Xx", source, src_len);
		if(error_number == ERROR_INVALID_DATA) {
			strcpy(symbol->errtxt, "Invalid characters in input");
			return error_number;
		}
	}


	/* Add leading zeroes */
	ustrcpy(local_source, (unsigned char *)"");
	if(symbol->symbology == BARCODE_ISBNX) {
		to_upper(local_source);
	}

	ean_leading_zeroes(symbol, source, local_source);

	for(reader = 0; reader <= ustrlen(local_source); reader++)
	{
		if(source[reader] == '+') { with_addon = TRUE; }
	}

	reader = 0;
	if(with_addon) {
		do {
			if(local_source[reader] == '+') {
				first_part[writer] = '\0';
				latch = TRUE;
				reader++;
				writer = 0;
			}

			if(latch) {
				second_part[writer] = local_source[reader];
				reader++;
				writer++;
			} else {
				first_part[writer] = local_source[reader];
				reader++;
				writer++;
			}
		} while (reader <= ustrlen(local_source));
	} else {
		strcpy((char*)first_part, (char*)local_source);
	}


	switch(symbol->symbology)
	{
		case BARCODE_EANX:
			switch(ustrlen(first_part))
			{
				case 2: add_on(first_part, (char*)dest, 0); ustrcpy(symbol->text, first_part); break;
				case 5: add_on(first_part, (char*)dest, 0); ustrcpy(symbol->text, first_part); break;
				case 7: ean8(symbol, first_part, (char*)dest); break;
				case 12: ean13(symbol, first_part, (char*)dest); break;
				default: strcpy(symbol->errtxt, "Invalid length input"); return ERROR_TOO_LONG; break;
			}
			break;
		case BARCODE_EANX_CC:
			switch(ustrlen(first_part))
			{ /* Adds vertical separator bars according to ISO/IEC 24723 section 11.4 */
				case 7: set_module(symbol, symbol->rows, 1);
					set_module(symbol, symbol->rows, 67);
					set_module(symbol, symbol->rows + 1, 0);
					set_module(symbol, symbol->rows + 1, 68);
					set_module(symbol, symbol->rows + 2, 1);
					set_module(symbol, symbol->rows + 1, 67);
					symbol->row_height[symbol->rows] = 2;
					symbol->row_height[symbol->rows + 1] = 2;
					symbol->row_height[symbol->rows + 2] = 2;
					symbol->rows += 3;
					ean8(symbol, first_part, (char*)dest); break;
				case 12:set_module(symbol, symbol->rows, 1);
					set_module(symbol, symbol->rows, 95);
					set_module(symbol, symbol->rows + 1, 0);
					set_module(symbol, symbol->rows + 1, 96);
					set_module(symbol, symbol->rows + 2, 1);
					set_module(symbol, symbol->rows + 2, 95);
					symbol->row_height[symbol->rows] = 2;
					symbol->row_height[symbol->rows + 1] = 2;
					symbol->row_height[symbol->rows + 2] = 2;
					symbol->rows += 3;
					ean13(symbol, first_part, (char*)dest); break;
					default: strcpy(symbol->errtxt, "Invalid length EAN input"); return ERROR_TOO_LONG; break;
			}
			break;
		case BARCODE_UPCA:
			if(ustrlen(first_part) == 11) {
				upca(symbol, first_part, (char*)dest);
			} else {
				strcpy(symbol->errtxt, "Input wrong length");
				return ERROR_TOO_LONG;
			}
			break;
		case BARCODE_UPCA_CC:
			if(ustrlen(first_part) == 11) {
				set_module(symbol, symbol->rows, 1);
				set_module(symbol, symbol->rows, 95);
				set_module(symbol, symbol->rows + 1, 0);
				set_module(symbol, symbol->rows + 1, 96);
				set_module(symbol, symbol->rows + 2, 1);
				set_module(symbol, symbol->rows + 2, 95);
				symbol->row_height[symbol->rows] = 2;
				symbol->row_height[symbol->rows + 1] = 2;
				symbol->row_height[symbol->rows + 2] = 2;
				symbol->rows += 3;
				upca(symbol, first_part, (char*)dest);
			} else {
				strcpy(symbol->errtxt, "UPCA input wrong length");
				return ERROR_TOO_LONG;
			}
			break;
		case BARCODE_UPCE:
			if((ustrlen(first_part) >= 6) && (ustrlen(first_part) <= 7)) {
				upce(symbol, first_part, (char*)dest);
			} else {
				strcpy(symbol->errtxt, "Input wrong length");
				return ERROR_TOO_LONG;
			}
			break;
		case BARCODE_UPCE_CC:
			if((ustrlen(first_part) >= 6) && (ustrlen(first_part) <= 7)) {
				set_module(symbol, symbol->rows, 1);
				set_module(symbol, symbol->rows, 51);
				set_module(symbol, symbol->rows + 1, 0);
				set_module(symbol, symbol->rows + 1, 52);
				set_module(symbol, symbol->rows + 2, 1);
				set_module(symbol, symbol->rows + 2, 51);
				symbol->row_height[symbol->rows] = 2;
				symbol->row_height[symbol->rows + 1] = 2;
				symbol->row_height[symbol->rows + 2] = 2;
				symbol->rows += 3;
				upce(symbol, first_part, (char*)dest);
			} else {
				strcpy(symbol->errtxt, "UPCE input wrong length");
				return ERROR_TOO_LONG;
			}
			break;
		case BARCODE_ISBNX:
			error_number = isbn(symbol, first_part, ustrlen(first_part), (char*)dest);
			if(error_number > 4) {
				return error_number;
			}
			break;
	}
	switch(ustrlen(second_part))
	{
		case 0: break;
		case 2:
			add_on(second_part, (char*)dest, 1);
			uconcat(symbol->text, (unsigned char*)"+");
			uconcat(symbol->text, second_part);
			break;
		case 5:
			add_on(second_part, (char*)dest, 1);
			uconcat(symbol->text, (unsigned char*)"+");
			uconcat(symbol->text, second_part);
			break;
		default:
			strcpy(symbol->errtxt, "Invalid length input");
			return ERROR_TOO_LONG;
			break;
	}

	expand(symbol, (char*)dest);

	switch(symbol->symbology) {
		case BARCODE_EANX_CC:
		case BARCODE_UPCA_CC:
		case BARCODE_UPCE_CC:
			/* shift the symbol to the right one space to allow for separator bars */
			for(i = (symbol->width + 1); i >= 1; i--) {
				if(module_is_set(symbol, symbol->rows - 1, i - 1)) {
					set_module(symbol, symbol->rows - 1, i);
				} else {
					unset_module(symbol, symbol->rows - 1, i);
				}
			}
			unset_module(symbol, symbol->rows - 1, 0);
			symbol->width += 2;
			break;
	}


	if((symbol->errtxt[0] == 'w') && (error_number == 0)) {
		error_number = 1; /* flag UPC-E warnings */
	}
	return error_number;
}




