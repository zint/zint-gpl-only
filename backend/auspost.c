/* auspost.c - Handles Australia Post 4-State Barcode */

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

#define GDSET 	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz #"

static char *AusNTable[10] = {"00", "01", "02", "10", "11", "12", "20", "21", "22", "30"};

static char *AusCTable[64] = {"222", "300", "301", "302", "310", "311", "312", "320", "321", "322",
	"000", "001", "002", "010", "011", "012", "020", "021", "022", "100", "101", "102", "110",
	"111", "112", "120", "121", "122", "200", "201", "202", "210", "211", "212", "220", "221",
	"023", "030", "031", "032", "033", "103", "113", "123", "130", "131", "132", "133", "203",
	"213", "223", "230", "231", "232", "233", "303", "313", "323", "330", "331", "332", "333",
	"003", "013"};

static char *AusBarTable[64] = {"000", "001", "002", "003", "010", "011", "012", "013", "020", "021",
	"022", "023", "030", "031", "032", "033", "100", "101", "102", "103", "110", "111", "112",
	"113", "120", "121", "122", "123", "130", "131", "132", "133", "200", "201", "202", "203",
	"210", "211", "212", "213", "220", "221", "222", "223", "230", "231", "232", "233", "300",
	"301", "302", "303", "310", "311", "312", "313", "320", "321", "322", "323", "330", "331",
	"332", "333"};

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "reedsol.h"

static inline char convert_pattern(char data, int shift)
{
	return (data - '0') << shift;
}

void rs_error(char data_pattern[])
{
	/* Adds Reed-Solomon error correction to auspost */

	int reader, triple_writer = 0;
	char triple[31], inv_triple[31];
	unsigned char result[5];

	for(reader = 2; reader < strlen(data_pattern); reader += 3, triple_writer++)
	{
		triple[triple_writer] = convert_pattern(data_pattern[reader], 4)
			+ convert_pattern(data_pattern[reader + 1], 2)
			+ convert_pattern(data_pattern[reader + 2], 0);
	}

	for(reader = 0; reader < triple_writer; reader++)
	{
		inv_triple[reader] = triple[(triple_writer - 1) - reader];
	}

	rs_init_gf(0x43);
	rs_init_code(4, 1);
	rs_encode(triple_writer, (unsigned char*) inv_triple, result);

	for(reader = 4; reader > 0; reader--)
	{
		concat(data_pattern, AusBarTable[(int)result[reader - 1]]);
	}
	rs_free();
}

int australia_post(struct zint_symbol *symbol, unsigned char source[], int length)
{
	/* Handles Australia Posts's 4 State Codes */
	/* Customer Standard Barcode, Barcode 2 or Barcode 3 system determined automatically
	   (i.e. the FCC doesn't need to be specified by the user) dependent
	   on the length of the input string */

	/* The contents of data_pattern conform to the following standard:
	   0 = Tracker, Ascender and Descender
	   1 = Tracker and Ascender
	   2 = Tracker and Descender
	   3 = Tracker only */
	int error_number, zeroes;
	int writer;
	unsigned int loopey, reader, h;

	char data_pattern[200];
	char fcc[3] = {0, 0}, dpid[10];
	char localstr[30];

	error_number = 0;
        strcpy(localstr, "");

	/* Do all of the length checking first to avoid stack smashing */
	if(symbol->symbology == BARCODE_AUSPOST) {
		/* Format control code (FCC) */
		switch(length)
		{
			case 8:
				strcpy(fcc, "11");
				break;
			case 16:
				error_number = is_sane(NEON, source, length);
			case 13:
				strcpy(fcc, "59");
				break;
			case 23:
				error_number = is_sane(NEON, source, length);
			case 18:
				strcpy(fcc, "62");
				break;
			default:
				strcpy(symbol->errtxt, "Auspost input is wrong length");
				return ERROR_TOO_LONG;
				break;
		}
		if(error_number == ERROR_INVALID_DATA) {
			strcpy(symbol->errtxt, "Invalid characters in data");
			return error_number;
		}
	} else {
		if(length > 8) {
			strcpy(symbol->errtxt, "Auspost input is too long");
			return ERROR_TOO_LONG;
		}
		switch(symbol->symbology) {
			case BARCODE_AUSREPLY: strcpy(fcc, "45"); break;
			case BARCODE_AUSROUTE: strcpy(fcc, "87"); break;
			case BARCODE_AUSREDIRECT: strcpy(fcc, "92"); break;
		}

		/* Add leading zeros as required */
		zeroes = 8 - length;
		memset(localstr, '0', zeroes);
		localstr[8] = '\0';
	}

	concat(localstr, (char*)source);
	h = strlen(localstr);
	error_number = is_sane(GDSET, (unsigned char *)localstr, h);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data");
		return error_number;
	}

	/* Verifiy that the first 8 characters are numbers */
	memcpy(dpid, localstr, 8);
	dpid[8] = '\0';
	error_number = is_sane(NEON, (unsigned char *)dpid, strlen(dpid));
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in DPID");
		return error_number;
	}

	/* Start character */
	strcpy(data_pattern, "13");

	/* Encode the FCC */
	for(reader = 0; reader < 2; reader++)
	{
		lookup(NEON, AusNTable, fcc[reader], data_pattern);
	}

	/* printf("AUSPOST FCC: %s  ", fcc); */

	/* Delivery Point Identifier (DPID) */
	for(reader = 0; reader < 8; reader++)
	{
		lookup(NEON, AusNTable, dpid[reader], data_pattern);
	}

	/* Customer Information */
	if(h > 8)
	{
		if((h == 13) || (h == 18)) {
			for(reader = 8; reader < h; reader++) {
				lookup(GDSET, AusCTable, localstr[reader], data_pattern);
			}
		}
		else if((h == 16) || (h == 23)) {
			for(reader = 8; reader < h; reader++) {
				lookup(NEON, AusNTable, localstr[reader], data_pattern);
			}
		}
	}

	/* Filler bar */
	h = strlen(data_pattern);
	if(h == 22) {
		concat(data_pattern, "3");
	}
	else if(h == 37) {
		concat(data_pattern, "3");
	}
	else if(h == 52) {
		concat(data_pattern, "3");
	}

	/* Reed Solomon error correction */
	rs_error(data_pattern);

	/* Stop character */
	concat(data_pattern, "13");

	/* Turn the symbol into a bar pattern ready for plotting */
	writer = 0;
	h = strlen(data_pattern);
	for(loopey = 0; loopey < h; loopey++)
	{
		if((data_pattern[loopey] == '1') || (data_pattern[loopey] == '0'))
		{
			set_module(symbol, 0, writer);
		}
		set_module(symbol, 1, writer);
		if((data_pattern[loopey] == '2') || (data_pattern[loopey] == '0'))
		{
			set_module(symbol, 2, writer);
		}
		writer += 2;
	}

	symbol->row_height[0] = 3;
	symbol->row_height[1] = 2;
	symbol->row_height[2] = 3;

	symbol->rows = 3;
	symbol->width = writer - 1;

	return error_number;
}

