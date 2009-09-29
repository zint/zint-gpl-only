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
#include "shiftjis.h"

QRcode *encode(int security, int size, const unsigned char *intext, int kanji, int gs1, int input_length)
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

	code = QRcode_encodeString((char*)intext, version, level, hint, 1);

	return code;
}

int qr_code(struct zint_symbol *symbol, unsigned char source[], int length)
{
	QRcode *code;
	int i, j;
	int kanji, gs1;
	int error_number;
	
#ifndef _MSC_VER
        unsigned char local_source[length];
#else
        unsigned char local_source = (unsigned char*)_alloca(length);
#endif
	
	if(symbol->input_mode == GS1_MODE) { gs1 = 1; } else { gs1 = 0; }

	if(gs1) {
		strcpy(symbol->errtxt, "GS1 mode not yet supported in QR Code");
		return ERROR_INVALID_OPTION;
	}
	
	for(i = 0; i < length; i++) {
		if(source[i] == '\0') {
			strcpy(symbol->errtxt, "QR Code not yet able to handle NULL characters");
			return ERROR_INVALID_DATA;
		}
	}
	
	/* The following to be replaced by ECI handling */
	switch(symbol->input_mode) {
		case DATA_MODE:
			for(i = 0; i < length; i++) {
				local_source[i] = source[i];
			}
			local_source[length] = '\0';
			kanji = 0;
			break;
		case UNICODE_MODE:
			error_number = shiftJIS(symbol, source, local_source, &length, &kanji);
			if(error_number != 0) { return error_number; }
			break;
	}

	code = encode(symbol->option_1, symbol->option_2, local_source, kanji, gs1, length);
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

