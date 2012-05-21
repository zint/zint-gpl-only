/* maxicode.c - Handles Maxicode */

/*
    libzint - the open source barcode library
    Copyright (C) 2010 Robin Stuart <robin@zint.org.uk>

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

/* Includes corrections thanks to Monica Swanson @ Source Technologies */

#include "common.h"
#include "maxicode.h"
#include "reedsol.h"
#include <string.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <sys/malloc.h> 
#else
#include <malloc.h> 
#endif

int maxi_codeword[144];

void maxi_do_primary_check(  )
{
	/* Handles error correction of primary message */
	unsigned char data[15];
	unsigned char results[15];
	int j;
	int datalen = 10;
	int ecclen = 10;

	rs_init_gf(0x43);
	rs_init_code(ecclen, 1);

	for(j = 0; j < datalen; j += 1)
		data[j] = maxi_codeword[j];

	rs_encode(datalen, data, results);

	for ( j = 0; j < ecclen; j += 1)
		maxi_codeword[ datalen + j] = results[ecclen - 1 - j];
	rs_free();
}

void maxi_do_secondary_chk_odd( int ecclen )
{
	/* Handles error correction of odd characters in secondary */
	unsigned char data[100];
	unsigned char results[30];
	int j;
	int datalen = 68;

	rs_init_gf(0x43);
	rs_init_code(ecclen, 1);

	if (ecclen == 20)
		datalen = 84;

	for(j = 0; j < datalen; j += 1)
		if (j & 1)  // odd
			data[(j-1)/2] = maxi_codeword[j + 20];

	rs_encode(datalen/2, data, results);

	for ( j = 0; j < (ecclen); j += 1)
		maxi_codeword[ datalen + (2 *j) + 1 + 20 ] = results[ecclen - 1 - j];
	rs_free();
}

void maxi_do_secondary_chk_even(int ecclen )
{
	/* Handles error correction of even characters in secondary */
	unsigned char data[100];
	unsigned char results[30];
	int j;
	int datalen = 68;

	if (ecclen == 20)
		datalen = 84;

	rs_init_gf(0x43);
	rs_init_code(ecclen, 1);

	for(j = 0; j < datalen + 1; j += 1)
		if (!(j & 1)) // even
			data[j/2] = maxi_codeword[j + 20];

	rs_encode(datalen/2, data, results);

	for ( j = 0; j < (ecclen); j += 1)
		maxi_codeword[ datalen + (2 *j) + 20] = results[ecclen - 1 - j];
	rs_free();
}

void maxi_bump(int set[], int character[], int bump_posn)
{
	/* Moves everything up so that a shift or latch can be inserted */
	int i;

	for(i = 143; i > bump_posn; i--) {
		set[i] = set[i - 1];
		character[i] = character[i - 1];
	}
}

int maxi_text_process(int mode, unsigned char source[], int length)
{
	/* Format text according to Appendix A */

	/* This code doesn't make use of [Lock in C], [Lock in D]
	and [Lock in E] and so is not always the most efficient at
	compressing data, but should suffice for most applications */

	int set[144], character[144], i, j, done, count, current_set;

	if(length > 138) {
		return ERROR_TOO_LONG;
	}

	for(i = 0; i < 144; i++) {
		set[i] = -1;
		character[i] = 0;
	}

	for (i = 0; i < length; i++) {
		/* Look up characters in table from Appendix A - this gives
		 value and code set for most characters */
		set[i] = maxiCodeSet[source[i]];
		character[i] = maxiSymbolChar[source[i]];
	}

	/* If a character can be represented in more than one code set,
	pick which version to use */
	if(set[0] == 0) {
		if(character[0] == 13) {
			character[0] = 0;
		}
		set[0] = 1;
	}

	for(i = 1; i < length; i++) {
		if(set[i] == 0) {
			done = 0;
			/* Special character */
			if(character[i] == 13) {
				/* Carriage Return */
				if(set[i - 1] == 5) {
					character[i] = 13;
					set[i] = 5;
				} else {
					if((i != length - 1) && (set[i + 1] == 5)) {
						character[i] = 13;
						set[i] = 5;
					} else {
						character[i] = 0;
						set[i] = 1;
					}
				}
				done = 1;
			}

			if((character[i] == 28) && (done == 0)) {
				/* FS */
				if(set[i - 1] == 5) {
					character[i] = 32;
					set[i] = 5;
				} else {
					set[i] = set[i - 1];
				}
				done = 1;
			}

			if((character[i] == 29) && (done == 0)) {
				/* GS */
				if(set[i - 1] == 5) {
					character[i] = 33;
					set[i] = 5;
				} else {
					set[i] = set[i - 1];
				}
				done = 1;
			}

			if((character[i] == 30) && (done == 0)) {
				/* RS */
				if(set[i - 1] == 5) {
					character[i] = 34;
					set[i] = 5;
				} else {
					set[i] = set[i - 1];
				}
				done = 1;
			}

			if((character[i] == 32) && (done == 0)) {
				/* Space */
				if(set[i - 1] == 1) {
					character[i] = 32;
					set[i] = 1;
				}
				if(set[i - 1] == 2) {
					character[i] = 47;
					set[i] = 2;
				}
				if(set[i - 1] >= 3) {
					if(i != length - 1) {
						if(set[i + 1] == 1) {
							character[i] = 32;
							set[i] = 1;
						}
						if(set[i + 1] == 2) {
							character[i] = 47;
							set[i] = 2;
						}
						if(set[i + 1] >= 3) {
							character[i] = 59;
							set[i] = set[i - 1];
						}
					} else {
						character[i] = 59;
						set[i] = set[i - 1];
					}
				}
				done = 1;
			}

			if((character[i] == 44) && (done == 0)) {
				/* Comma */
				if(set[i - 1] == 2) {
					character[i] = 48;
					set[i] = 2;
				} else {
					if((i != length - 1) && (set[i + 1] == 2)) {
						character[i] = 48;
						set[i] = 2;
					} else {
						set[i] = 1;
					}
				}
				done = 1;
			}

			if((character[i] == 46) && (done == 0)) {
				/* Full Stop */
				if(set[i - 1] == 2) {
					character[i] = 49;
					set[i] = 2;
				} else {
					if((i != length - 1) && (set[i + 1] == 2)) {
						character[i] = 49;
						set[i] = 2;
					} else {
						set[i] = 1;
					}
				}
				done = 1;
			}

			if((character[i] == 47) && (done == 0)) {
				/* Slash */
				if(set[i - 1] == 2) {
					character[i] = 50;
					set[i] = 2;
				} else {
					if((i != length - 1) && (set[i + 1] == 2)) {
						character[i] = 50;
						set[i] = 2;
					} else {
						set[i] = 1;
					}
				}
				done = 1;
			}

			if((character[i] == 58) && (done == 0)) {
				/* Colon */
				if(set[i - 1] == 2) {
					character[i] = 51;
					set[i] = 2;
				} else {
					if((i != length - 1) && (set[i + 1] == 2)) {
						character[i] = 51;
						set[i] = 2;
					} else {
						set[i] = 1;
					}
				}
				done = 1;
			}
		}
	}

	for(i = length; i < 144; i++) {
		/* Add the padding */
		if(set[length - 1] == 2) {
			set[i] = 2;
		} else {
			set[i] = 1;
		}
		character[i] = 33;
	}

	/* Find candidates for number compression */
	if((mode == 2) || (mode ==3)) { j = 0; } else { j = 9; }
		/* Number compression not allowed in primary message */
	count = 0;
	for(i = j; i < 143; i++) {
		if((set[i] == 1) && ((character[i] >= 48) && (character[i] <= 57))) {
			/* Character is a number */
			count++;
		} else {
			count = 0;
		}
		if(count == 9) {
			/* Nine digits in a row can be compressed */
			set[i] = 6;
			set[i - 1] = 6;
			set[i - 2] = 6;
			set[i - 3] = 6;
			set[i - 4] = 6;
			set[i - 5] = 6;
			set[i - 6] = 6;
			set[i - 7] = 6;
			set[i - 8] = 6;
			count = 0;
		}
	}

	/* Add shift and latch characters */
	current_set = 1;
	i = 0;
	do {

		if(set[i] != current_set) {
			switch(set[i]) {
				case 1:
					if(set[i + 1] == 1) {
						if(set[i + 2] == 1) {
							if(set[i + 3] == 1) {
								/* Latch A */
								maxi_bump(set, character, i);
								character[i] = 63;
								current_set = 1;
								length++;
							} else {
								/* 3 Shift A */
								maxi_bump(set, character, i);
								character[i] = 57;
								length++;
								i += 2;
							}
						} else {
							/* 2 Shift A */
							maxi_bump(set, character, i);
							character[i] = 56;
							length++;
							i++;
						}
					} else {
						/* Shift A */
						maxi_bump(set, character, i);
						character[i] = 59;
						length++;
					}
					break;
				case 2:
					if(set[i + 1] == 2) {
						/* Latch B */
						maxi_bump(set, character, i);
						character[i] = 63;
						current_set = 2;
						length++;
					} else {
						/* Shift B */
						maxi_bump(set, character, i);
						character[i] = 59;
						length++;
					}
					break;
				case 3:
					/* Shift C */
					maxi_bump(set, character, i);
					character[i] = 60;
					length++;
					break;
				case 4:
					/* Shift D */
					maxi_bump(set, character, i);
					character[i] = 61;
					length++;
					break;
				case 5:
					/* Shift E */
					maxi_bump(set, character, i);
					character[i] = 62;
					length++;
					break;
				case 6:
					/* Number Compressed */
					/* Do nothing */
					break;
			}
			i++;
		}
		i++;
	} while(i < 145);

	/* Number compression has not been forgotten! - It's handled below */
	i = 0;
	do {
		if (set[i] == 6) {
			/* Number compression */
			char substring[11];
			int value;

			for(j = 0; j < 10; j++) {
				substring[j] = character[i + j];
			}
			substring[10] = '\0';
			value = atoi(substring);

			character[i] = 31; /* NS */
			character[i + 1] = (value & 0x3f000000) >> 24;
			character[i + 2] = (value & 0xfc0000) >> 18;
			character[i + 3] = (value & 0x3f000) >> 12;
			character[i + 4] = (value & 0xfc0) >> 6;
			character[i + 5] = (value & 0x3f);

			i += 6;
			for(j = i; j < 140; j++) {
				set[j] = set[j + 3];
				character[j] = character[j + 3];
			}
			length -= 3;
		} else {
			i++;
		}
	} while (i <= 143);

	if(((mode ==2) || (mode == 3)) && (length > 84)) {
		return ERROR_TOO_LONG;
	}

	if(((mode == 4) || (mode == 6)) && (length > 93)) {
		return ERROR_TOO_LONG;
	}

	if((mode == 5) && (length > 77)) {
		return ERROR_TOO_LONG;
	}


	/* Copy the encoded text into the codeword array */
	if((mode == 2) || (mode == 3)) {
		for(i = 0; i < 84; i++) { /* secondary only */
			maxi_codeword[i + 20] = character[i];
		}
	}

	if((mode == 4) || (mode == 6)) {
		for(i = 0; i < 9; i++) { /* primary */
			maxi_codeword[i + 1] = character[i];
		}
		for(i = 0; i < 84; i++) { /* secondary */
			maxi_codeword[i + 20] = character[i + 9];
		}
	}

	if(mode == 5) {
		for(i = 0; i < 9; i++) { /* primary */
			maxi_codeword[i + 1] = character[i];
		}
		for(i = 0; i < 68; i++) { /* secondary */
			maxi_codeword[i + 20] = character[i + 9];
		}
	}

	return 0;
}

void maxi_do_primary_2(char postcode[], int country, int service)
{
	/* Format structured primary for Mode 2 */
	int postcode_length, postcode_num, i;

	for(i = 0; i < 10; i++) {
		if((postcode[i] < '0') || (postcode[i] > '9')) {
			postcode[i] = '\0';
		}
	}

	postcode_length = strlen(postcode);
	postcode_num = atoi(postcode);

	maxi_codeword[0] = ((postcode_num & 0x03) << 4) | 2;
	maxi_codeword[1] = ((postcode_num & 0xfc) >> 2);
	maxi_codeword[2] = ((postcode_num & 0x3f00) >> 8);
	maxi_codeword[3] = ((postcode_num & 0xfc000) >> 14);
	maxi_codeword[4] = ((postcode_num & 0x3f00000) >> 20);
	maxi_codeword[5] = ((postcode_num & 0x3c000000) >> 26) | ((postcode_length & 0x3) << 4);
	maxi_codeword[6] = ((postcode_length & 0x3c) >> 2) | ((country & 0x3) << 4);
	maxi_codeword[7] = (country & 0xfc) >> 2;
	maxi_codeword[8] = ((country & 0x300) >> 8) | ((service & 0xf) << 2);
	maxi_codeword[9] = ((service & 0x3f0) >> 4);
}

void maxi_do_primary_3(char postcode[], int country, int service)
{
	/* Format structured primary for Mode 3 */
	int i, h;

	h = strlen(postcode);
	to_upper((unsigned char*)postcode);
	for(i = 0; i < h; i++) {
		if((postcode[i] >= 'A') && (postcode[i] <= 'Z')) {
			/* (Capital) letters shifted to Code Set A values */
			postcode[i] -= 64;
		}
		if(((postcode[i] == 27) || (postcode[i] == 31)) || ((postcode[i] == 33) || (postcode[i] >= 59))) {
			/* Not a valid postcode character */
			postcode[i] = ' ';
		}
		/* Input characters lower than 27 (NUL - SUB) in postcode are
		interpreted as capital letters in Code Set A (e.g. LF becomes 'J') */
	}

	maxi_codeword[0] = ((postcode[5] & 0x03) << 4) | 3;
	maxi_codeword[1] = ((postcode[4] & 0x03) << 4) | ((postcode[5] & 0x3c) >> 2);
	maxi_codeword[2] = ((postcode[3] & 0x03) << 4) | ((postcode[4] & 0x3c) >> 2);
	maxi_codeword[3] = ((postcode[2] & 0x03) << 4) | ((postcode[3] & 0x3c) >> 2);
	maxi_codeword[4] = ((postcode[1] & 0x03) << 4) | ((postcode[2] & 0x3c) >> 2);
	maxi_codeword[5] = ((postcode[0] & 0x03) << 4) | ((postcode[1] & 0x3c) >> 2);
	maxi_codeword[6] = ((postcode[0] & 0x3c) >> 2) | ((country & 0x3) << 4);
	maxi_codeword[7] = (country & 0xfc) >> 2;
	maxi_codeword[8] = ((country & 0x300) >> 8) | ((service & 0xf) << 2);
	maxi_codeword[9] = ((service & 0x3f0) >> 4);
}

int maxicode(struct zint_symbol *symbol, unsigned char source[], int length)
{
	int i, j, block, bit, mode, countrycode = 0, service = 0, lp = 0;
	int bit_pattern[7], internal_error = 0, eclen, error_number;
	char postcode[12], countrystr[4], servicestr[4];

#ifndef _MSC_VER
        unsigned char local_source[length + 1];
#else
        unsigned char* local_source = (unsigned char*)_alloca(length + 1);
#endif

	mode = symbol->option_1;
	strcpy(postcode, "");
	strcpy(countrystr, "");
	strcpy(servicestr, "");

	/* The following to be replaced by ECI handling */
	switch(symbol->input_mode) {
		case DATA_MODE:
		case GS1_MODE:
			memcpy(local_source, source, length);
			local_source[length] = '\0';
			break;
		case UNICODE_MODE:
			error_number = latin1_process(symbol, source, local_source, &length);
			if(error_number != 0) { return error_number; }
			break;
	}
	memset(maxi_codeword, 0, sizeof(maxi_codeword));

	if(mode == -1) { /* If mode is unspecified */
		lp = strlen(symbol->primary);
		if(lp == 0) {
			mode = 4;
		} else {
			mode = 2;
			for(i = 0; i < 10 && i < lp; i++) {
				if((symbol->primary[i] < 48) || (symbol->primary[i] > 57)) {
					mode = 3;
					break;
				}
			}
		}
	}

	if((mode < 2) || (mode > 6)) { /* Only codes 2 to 6 supported */
		strcpy(symbol->errtxt, "Invalid Maxicode Mode");
		return ERROR_INVALID_OPTION;
	}

	if((mode == 2) || (mode == 3)) { /* Modes 2 and 3 need data in symbol->primary */
		if(lp == 0){ /* Mode set manually means lp doesn't get set */
			lp = strlen( symbol->primary );
		}
		if(lp != 15) {
			strcpy(symbol->errtxt, "Invalid Primary String");
			return ERROR_INVALID_DATA;
		}

		for(i = 9; i < 15; i++) { /* check that country code and service are numeric */
			if((symbol->primary[i] < '0') || (symbol->primary[i] > '9')) {
				strcpy(symbol->errtxt, "Invalid Primary String");
				return ERROR_INVALID_DATA;
			}
		}

		memcpy(postcode, symbol->primary, 9);
		postcode[9] = '\0';

		if(mode == 2) {
			for(i = 0; i < 10; i++) {
				if(postcode[i] == ' ') {
					postcode[i] = '\0';
				}
			}
		}
		else if(mode == 3) { postcode[6] = '\0'; }

		countrystr[0] = symbol->primary[9];
		countrystr[1] = symbol->primary[10];
		countrystr[2] = symbol->primary[11];
		countrystr[3] = '\0';

		servicestr[0] = symbol->primary[12];
		servicestr[1] = symbol->primary[13];
		servicestr[2] = symbol->primary[14];
		servicestr[3] = '\0';

		countrycode = atoi(countrystr);
		service = atoi(servicestr);

		if(mode == 2) { maxi_do_primary_2(postcode, countrycode, service); }
		if(mode == 3) { maxi_do_primary_3(postcode, countrycode, service); }
	} else {
		maxi_codeword[0] = mode;
	}

	i = maxi_text_process(mode, local_source, length);
	if(i == ERROR_TOO_LONG ) {
		strcpy(symbol->errtxt, "Input data too long");
		return i;
	}

	/* All the data is sorted - now do error correction */
	maxi_do_primary_check();  /* always EEC */

	if ( mode == 5 )
		eclen = 56;   // 68 data codewords , 56 error corrections
	else
		eclen = 40;  // 84 data codewords,  40 error corrections

	maxi_do_secondary_chk_even(eclen/2);  // do error correction of even
	maxi_do_secondary_chk_odd(eclen/2);   // do error correction of odd

	/* Copy data into symbol grid */
	for(i = 0; i < 33; i++) {
		for(j = 0; j < 30; j++) {
			block = (MaxiGrid[(i * 30) + j] + 5) / 6;
			bit = (MaxiGrid[(i * 30) + j] + 5) % 6;

			if(block != 0) {

				bit_pattern[0] =  (maxi_codeword[block - 1] & 0x20) >> 5;
				bit_pattern[1] =  (maxi_codeword[block - 1] & 0x10) >> 4;
				bit_pattern[2] =  (maxi_codeword[block - 1] & 0x8) >> 3;
				bit_pattern[3] =  (maxi_codeword[block - 1] & 0x4) >> 2;
				bit_pattern[4] =  (maxi_codeword[block - 1] & 0x2) >> 1;
				bit_pattern[5] =  (maxi_codeword[block - 1] & 0x1);

				if(bit_pattern[bit] != 0) {
					set_module(symbol, i, j);
				}
			}
		}
	}

	/* Add orientation markings */
	set_module(symbol, 0, 28); // Top right filler
	set_module(symbol, 0, 29);
	set_module(symbol, 9, 10); // Top left marker
	set_module(symbol, 9, 11);
	set_module(symbol, 10, 11);
	set_module(symbol, 15, 7); // Left hand marker
	set_module(symbol, 16, 8);
	set_module(symbol, 16, 20); // Right hand marker
	set_module(symbol, 17, 20);
	set_module(symbol, 22, 10); // Bottom left marker
	set_module(symbol, 23, 10);
	set_module(symbol, 22, 17); // Bottom right marker
	set_module(symbol, 23, 17);

	symbol->width = 30;
	symbol->rows = 33;

	return internal_error;
}
