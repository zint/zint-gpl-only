/* code.c - Handles Code 11, 39, 39+ and 93 */

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

/* In version 0.5 this file was 1,553 lines long! */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"


#define NASET	"0123456789-"
static char *C11Table[11] = {"111121", "211121", "121121", "221111", "112121", "212111", "122111",
	"111221", "211211", "211111", "112111"};


/* Code 39 tables checked against ISO/IEC 16388:2007 */
	
#define TCSET	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-. $/+%abcd"
/* Incorporates Table A1 */

static char *C39Table[43] = { "1112212111", "2112111121", "1122111121", "2122111111", "1112211121",
	"2112211111", "1122211111", "1112112121", "2112112111", "1122112111", "2111121121",
	"1121121121", "2121121111", "1111221121", "2111221111", "1121221111", "1111122121",
	"2111122111", "1121122111", "1111222111", "2111111221", "1121111221", "2121111211",
	"1111211221", "2111211211", "1121211211", "1111112221", "2111112211", "1121112211",
	"1111212211", "2211111121", "1221111121", "2221111111", "1211211121", "2211211111",
	"1221211111", "1211112121", "2211112111", "1221112111", "1212121111", "1212111211",
	"1211121211", "1112121211"};
/* Code 39 character assignments (Table 1) */

static char *EC39Ctrl[128] = {"%U", "$A", "$B", "$C", "$D", "$E", "$F", "$G", "$H", "$I", "$J", "$K",
	"$L", "$M", "$N", "$O", "$P", "$Q", "$R", "$S", "$T", "$U", "$V", "$W", "$X", "$Y", "$Z",
	"%A", "%B", "%C", "%D", "%E", " ", "/A", "/B", "/C", "/D", "/E", "/F", "/G", "/H", "/I", "/J",
	"/K", "/L", "-", ".", "/O", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "/Z", "%F",
	"%G", "%H", "%I", "%J", "%V", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
	"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "%K", "%L", "%M", "%N", "%O",
	"%W", "+A", "+B", "+C", "+D", "+E", "+F", "+G", "+H", "+I", "+J", "+K", "+L", "+M", "+N", "+O",
	"+P", "+Q", "+R", "+S", "+T", "+U", "+V", "+W", "+X", "+Y", "+Z", "%P", "%Q", "%R", "%S", "%T"};
/* Encoding the full ASCII character set in Code 39 (Table A2) */

static char *C93Ctrl[128] = {"bU", "aA", "aB", "aC", "aD", "aE", "aF", "aG", "aH", "aI", "aJ", "aK",
	"aL", "aM", "aN", "aO", "aP", "aQ", "aR", "aS", "aT", "aU", "aV", "aW", "aX", "aY", "aZ",
	"bA", "bB", "bC", "bD", "bE", " ", "cA", "cB", "cC", "cD", "cE", "cF", "cG", "cH", "cI", "cJ",
	"cK", "cL", "cM", "cN", "cO", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "cZ", "bF",
	"bG", "bH", "bI", "bJ", "bV", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
	"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "bK", "bL", "bM", "bN", "bO",
	"bW", "dA", "dB", "dC", "dD", "dE", "dF", "dG", "dH", "dI", "dJ", "dK", "dL", "dM", "dN", "dO",
	"dP", "dQ", "dR", "dS", "dT", "dU", "dV", "dW", "dX", "dY", "dZ", "bP", "bQ", "bR", "bS", "bT"};

static char *C93Table[47] = {"131112", "111213", "111312", "111411", "121113", "121212", "121311",
	"111114", "131211", "141111", "211113", "211212", "211311", "221112", "221211", "231111",
	"112113", "112212", "112311", "122112", "132111", "111123", "111222", "111321", "121122",
	"131121", "212112", "212211", "211122", "211221", "221121", "222111", "112122", "112221",
	"122121", "123111", "121131", "311112", "311211", "321111", "112131", "113121", "211131",
	"121221", "312111", "311121", "122211"};

/* *********************** CODE 11 ******************** */

int code_11(struct zint_symbol *symbol, unsigned char source[])
{ /* Code 11 */

	unsigned int i;
	int h, c_digit, c_weight, c_count, k_digit, k_weight, k_count;
	int weight[1000], errno;
	char dest[1000];
	
	errno = 0;
	strcpy(dest, "");

	if(strlen(source) > 80) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	errno = is_sane(NASET, source);
	if(errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return errno;
	}
	c_weight = 1;
	c_count = 0;
	k_weight = 1;
	k_count = 0;

	/* start character */
	concat (dest, "112211");

	/* Draw main body of barcode */
	for(i = 0; i < strlen(source); i++) {
		lookup(NASET, C11Table, source[i], dest);
		weight[i] = ctoi(source[i]);
	}

	/* Calculate C checksum */
	for(h = (strlen(source) - 1); h >= 0; h--) {
		c_count += (c_weight * weight[h]);
		c_weight++;

		if(c_weight > 10) {
			c_weight = 1;
		}
	}
	c_digit = c_count%11;

	/* Draw C checksum */
	lookup(NASET, C11Table, itoc(c_digit), dest);
	weight[strlen(source)] = c_digit;

	/* Calculate K checksum */
	for(h = strlen(source); h >= 0; h--) {
		k_count += (k_weight * weight[h]);
		k_weight++;

		if(k_weight > 9) {
			k_weight = 1;
		}
	}
	k_digit = k_count%11;

	/* Draw K checksum */
	lookup(NASET, C11Table, itoc(k_digit), dest);

	/* Stop character */
	concat (dest, "11221");
	
	h = strlen(source);
	source[h] = itoc(c_digit);
	source[h + 1] = itoc(k_digit);
	source[h + 2] = '\0';
	expand(symbol, dest);
	strcpy(symbol->text, source);
	return errno;
}

int c39(struct zint_symbol *symbol, unsigned char source[])
{ /* Code 39 */
	unsigned int i;
	unsigned int counter;
	char check_digit;
	int h, errno;
	char dest[1000];
	
	errno = 0;
	counter = 0;
	strcpy(dest, "");

	if((symbol->option_2 < 0) || (symbol->option_2 > 1)) {
		symbol->option_2 = 0;
	}
	
	to_upper(source);
	if(strlen(source) > 45) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	errno = is_sane(TCSET , source);
	if(errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return errno;
	}

	/* Start character */
	concat(dest, "1211212111");

	for(i = 0; i < strlen(source); i++) {
		lookup(TCSET, C39Table, source[i], dest);
		counter += posn(TCSET, source[i]);
	}
	
	if((symbol->symbology == BARCODE_LOGMARS) || (symbol->option_2 == 1)) {
		
		counter = counter % 43;
		if(counter < 10) {
			check_digit = itoc(counter);
		} else {
			if(counter < 36) {
				check_digit = (counter - 10) + 'A';
			} else {
				switch(counter) {
					case 36: check_digit = '-'; break;
					case 37: check_digit = '.'; break;
					case 38: check_digit = ' '; break;
					case 39: check_digit = '$'; break;
					case 40: check_digit = '/'; break;
					case 41: check_digit = '+'; break;
					case 42: check_digit = 37; break;
				}
			}
		}
		lookup(TCSET, C39Table, check_digit, dest);
	
		/* Display a space check digit as _, otherwise it looks like an error */
		if(check_digit == ' ') {
			check_digit = '_';
		}
		
		h = strlen(source);
		source[h] = check_digit;
		source[h + 1] = '\0';
	}
	
	/* Stop character */
	concat (dest, "121121211");
	
	if(symbol->symbology == BARCODE_LOGMARS) {
		/* LOGMARS uses wider 'wide' bars than normal Code 39 */
		for(i = 0; i < strlen(dest); i++) {
			if(dest[i] == '2') {
				dest[i] = '3';
			}
		}
	}
	
	expand(symbol, dest);
	
	if((symbol->symbology == BARCODE_CODE39) || (symbol->symbology == BARCODE_CODE39_43)) {
		strcpy(symbol->text, "*");
		concat(symbol->text, source);
		concat(symbol->text, "*");
	} else {
		strcpy(symbol->text, source);
	}
	return errno;
}

int pharmazentral(struct zint_symbol *symbol, unsigned char source[])
{ /* Pharmazentral Nummer (PZN) */
	
	int i, errno;
	unsigned int h, count, check_digit;
	
	errno = 0;

	count = 0;
	h = strlen(source);
	if(h != 6) {
		strcpy(symbol->errtxt, "error: input wrong length");
		return ERROR_TOO_LONG;
	}
	errno = is_sane(NESET, source);
	if(errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return errno;
	}
	
	for (i = 0; i < h; i++)
	{
		count += (i + 2) * ctoi(source[i]);
	}

	for(i = h + 1; i >= 1; i--)
	{
		source[i] = source[i - 1];
	}
	source[0] = '-';
	
	check_digit = count%11;
	if (check_digit == 11) { check_digit = 0; }
	source[h + 1] = itoc(check_digit);
	source[h + 2] = '\0';
	errno = c39(symbol, source);
	strcpy(symbol->text, source);
	return errno;
}


/* ************** EXTENDED CODE 39 *************** */

int ec39(struct zint_symbol *symbol, unsigned char source[])
{ /* Extended Code 39 - ISO/IEC 16388:2007 Annex A */

	char buffer[100];
	unsigned int i;
	strcpy(buffer, "");
	int ascii_value;
	int errno;
	
	errno = 0;

	if(strlen(source) > 45) {
		/* only stops strings which are far too long - actual length of the barcode
		depends on the type of data being encoded - if it's too long it's picked up
		by c39() */
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	
	
	for(i = 0; i < strlen(source); i++) {
		if(source[i] > 127) {
			/* Cannot encode extended ASCII */
			strcpy(symbol->errtxt, "error: invalid characters in input data");
			return ERROR_INVALID_DATA;
		}
	}
	
	/* Creates a buffer string and places control characters into it */
	for(i = 0; i < strlen(source); i++) {
		ascii_value = source[i];
		concat(buffer, EC39Ctrl[ascii_value]);
	}

	/* Then sends the buffer to the C39 function */
	errno = c39(symbol, buffer);
	
	strcpy(symbol->text, source);
	return errno;
}

/* ******************** CODE 93 ******************* */

int c93(struct zint_symbol *symbol, unsigned char source[])
{ /* Code 93 is an advancement on Code 39 and the definition is a lot tighter */

  /* TCSET includes the extra characters a, b, c and d to represent Code 93 specific
     shift characters 1, 2, 3 and 4 respectively. These characters are never used by
     c39() and ec39() */

	unsigned int i;
	int h, weight, c, k, values[100], errno;
	char buffer[100], temp[2];
	char set_copy[] = TCSET;
	strcpy(buffer, "");
	int ascii_value;
	char dest[1000];
	
	errno = 0;
	strcpy(dest, "");

	if(strlen(source) > 45) {
		/* This stops rediculously long input - the actual length of the barcode
		depends on the type of data */
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	
	for(i = 0; i < strlen(source); i++) {
		if(source[i] > 127) {
			/* Cannot encode extended ASCII */
			strcpy(symbol->errtxt, "error: invalid characters in input data");
			return ERROR_INVALID_DATA;
		}
	}
	
	/* Start character */
	concat(dest, "111141");

	/* Message Content */
	for(i = 0; i < strlen(source); i++) {
		ascii_value = source[i];
		concat(buffer, C93Ctrl[ascii_value]);
	}

	/* Now we can check the true length of the barcode */
	if(strlen(buffer) > 45) {
		strcpy(symbol->errtxt, "error: input too long");
		return ERROR_TOO_LONG;
	}
	
	for(i = 0; i < strlen(buffer); i++) {
		values[i] = posn(TCSET, buffer[i]);
	}

	/* Putting the data into dest[] is not done until after check digits are calculated */

	/* Check digit C */

	c = 0;
	weight = 1;
	for(h = strlen(buffer) - 1; h >= 0; h--)
	{
		c += values[h] * weight;
		weight ++;
		if(weight == 21)
		{
			weight = 1;
		}
	}
	c = c % 47;

	/* Because concat() requires a string as its second argument the check digit is converted
	   to a character which is then put in temp[] before being added to buffer[] - its
	   a bit long winded but avoids putting yet another function into common.c */

	values[strlen(buffer)] = c;
	temp[0] = set_copy[c];
	temp[1] = '\0';
	concat(buffer, temp);

	/* Check digit K */
	k = 0;
	weight = 1;
	for(h = strlen(buffer) - 1; h >= 0; h--)
	{
		k += values[h] * weight;
		weight ++;
		if(weight == 16)
		{
			weight = 1;
		}
	}
	k = k % 47;
	temp[0] = set_copy[k];
	temp[1] = '\0';
	concat(buffer, temp);

	for(i = 0; i < strlen(buffer); i++) {
		lookup(TCSET, C93Table, buffer[i], dest);
	}

	/* Stop character */
	concat(dest, "1111411");
	
	h = strlen(source);
	source[h] = set_copy[c];
	source[h + 1] = set_copy[k];
	source[h + 2] = '\0';
	expand(symbol, dest);
	strcpy(symbol->text, source);
	return errno;
}
