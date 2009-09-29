/* shiftjis.c - Handle conversion to Shift-JIS */

/*
    libzint - the open source barcode library
    Copyright (C) 2009 Robin Stuart <robin@zint.org.uk>

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
#include "common.h"
#include "sjis.h"

int shiftJIS(struct zint_symbol *symbol, unsigned char source[], unsigned char preprocessed[], int *length, int *kanji)
{ /* QR Code supports compression of Shift-JIS data using "Kanji" mode - this function
	attempts to convert Unicode characters to Shift-JIS to allow this */
	int bpos, jpos, error_number, i, done;
	int next;
	unsigned long int uval, jval;
	
	bpos = 0;
	jpos = 0;
	error_number = 0;
	next = 0;
	
	do {
		uval = 0;
		jval = 0;
		done = 0;
		
		if(source[bpos] <= 0x7f) {
			/* 1 byte mode (7-bit ASCII) */
			uval = source[bpos];
			next = bpos + 1;
			preprocessed[jpos] = uval;
			jpos++;
			done = 1;
		}
		
		if(done == 0) {
			if((source[bpos] >= 0x80) && (source[bpos] <= 0xbf)) {
				strcpy(symbol->errtxt, "Corrupt Unicode data");
				return ERROR_INVALID_DATA;
			}
		}
		
		if(done == 0) {
			if((source[bpos] >= 0xc0) && (source[bpos] <= 0xc1)) {
				strcpy(symbol->errtxt, "Overlong encoding not supported");
				return ERROR_INVALID_DATA;
			}
		}
		
		if(done == 0) {
			if((source[bpos] >= 0xc2) && (source[bpos] <= 0xdf)) {
				/* 2 byte mode (Latin 1) */
				uval = ((source[bpos] & 0x1f) << 6) + (source[bpos + 1] & 0x3f);
				next = bpos + 2;
				preprocessed[jpos] = uval;
				jpos++;
				done = 1;
			}
		}
		
		if(done == 0) {
			if((source[bpos] >= 0xe0) && (source[bpos] <= 0xef)) {
				/* 3 byte mode (Japanese) */
				uval = ((source[bpos] & 0x0f) << 12) + ((source[bpos + 1] & 0x3f) << 6) + (source[bpos + 2] & 0x3f);
				next = bpos + 3;
				*kanji = 1;

				for(i = 0; i < 6843; i++) {
					if(sjis_lookup[i * 2] == uval) {
						jval = sjis_lookup[(i * 2) + 1];
					}
				}
		
				if(jval == 0) {
					strcpy(symbol->errtxt, "Invalid Shift JIS character");
					return ERROR_INVALID_DATA;
				}
				
				preprocessed[jpos] = (jval & 0xff00) >> 8;
				preprocessed[jpos + 1] = (jval & 0xff);
				
				/* printf("Unicode value U+%04X = Shift JIS value 0x%04X\n", uval, jval); */
				
				jpos += 2;
				done = 1;
			}
		}
		
		if(done == 0) {
			if(source[bpos] >= 0xf0) {
				strcpy(symbol->errtxt, "Unicode sequences of more than 3 bytes not supported");
				return ERROR_INVALID_DATA;
			}
		}
		
		bpos = next;
		
	} while(bpos < *length);
	*length = jpos;
	
	return error_number;
}
