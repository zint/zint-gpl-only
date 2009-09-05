/* qr.c Handles QR Code by utilising libqrencode */

/*
    libzint - the open source barcode library
    Copyright (C) 2008 Robin Stuart <robin@zint.org.uk>
    Copyright (C) 2006 Kentaro Fukuchi <fukuchi@megaui.net>

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

#ifndef NO_QR
#include <qrencode.h>
#include <stdio.h>

QRcode *encode(int security, int size, const unsigned char *intext, int kanji, int gs1, char nullchar, int input_length)
{
	int version;
	QRecLevel level;
	QRencodeMode hint;
	QRcode *code;

	hint = 0;

	if(kanji) {
		hint = QR_MODE_KANJI;
	}
	/* if(gs1) {
		hint = QR_MODE_GS1;
	} - for future expansion */
	if(hint == 0) {
		hint = QR_MODE_8;
	}

	level = QR_ECLEVEL_L;
	if((security >= 1) && (security <= 4)) {
		switch (security) {
			case 1: level = QR_ECLEVEL_L; break;
			case 2: level = QR_ECLEVEL_M; break;
			case 3: level = QR_ECLEVEL_Q; break;
			case 4: level = QR_ECLEVEL_H; break;
		}
	}
	
	if((size >= 1) && (size <= 40)) {
		version = size;
	} else {
		version = 0;
	}

	if(nullchar == '\0') {
		/* No NULL characters in data */
		code = QRcode_encodeString((char*)intext, version, level, hint, 1);
	} else {
		/* NULL characters in data */
		code = QRcode_encodeString8bit((char*)intext, version, level);
		/* code = QRcode_encodeString8bit((char*)intext, version, level, input_length); */
	}

	return code;
}

int qr_code(struct zint_symbol *symbol, unsigned char source[])
{
	QRcode *code;
	/*int errno = 0;*/
	int i, j;
	int kanji, gs1;
	int input_length;
	char nullify;
	
	input_length = ustrlen(source);
	nullify = symbol->nullchar;
	if((symbol->input_mode == KANJI_MODE) || (symbol->input_mode == SJIS_MODE)) { kanji = 1; } else { kanji = 0; }
	if(symbol->input_mode == GS1_MODE) { gs1 = 1; } else { gs1 = 0; }

	/* Null character handling */
	j = 0;
	if(nullify != '\0') {
		for(i = 0; i < input_length; i++) {
			if(source[i] == nullify) {
				source[i] = '\0';
				j++;
			}
		}
	}

	if(j == 0) {
		/* nullchar was set but there are no NULL characters in the input data */
		nullify = '\0';
	}

	code = encode(symbol->option_1, symbol->option_2, source, kanji, gs1, nullify, input_length);
	if(code == NULL) {
		strcpy(symbol->errtxt, "libqrencode failed to encode the input data");
		return ERROR_ENCODING_PROBLEM;
	}
	
	symbol->width = code->width;
	symbol->rows = code->width;
	
	for(i = 0; i < code->width; i++) {
		for(j = 0; j < code->width; j++) {
			if((*(code->data + (i * code->width) + j) & 0x01) != 0) {
				set_module(symbol, i, j);
			}
		}
		symbol->row_height[i] = 1;
	}
	
	QRcode_free(code);
	
	return 0;
}

#else
/* Handler if no QR Encode library is available */
int qr_code(struct zint_symbol *symbol, unsigned char source[])
{
	strcpy(symbol->errtxt, "QR Code library <qrencode> not available");
	return ERROR_INVALID_OPTION;
}
#endif

