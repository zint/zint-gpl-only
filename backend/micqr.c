/* micqr.c - Handles Micro QR Code versions M1 - M4 */

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "micqr.h"
#include "reedsol.h"

#define NUMERIC		1
#define ALPHANUM	2
#define BYTE		3
#define KANJI		4

#define QRSET	"0123456789ABCDEFGHIJKLNMOPQRSTUVWXYZ $%*+-./:"

void qrnumeric_encode(char binary[], unsigned char source[])
{ /* Encodes numeric data according to section 6.4.3 */
	
	int input_length, blocks, remainder, i;
	char block_binary[11];
	int block_value;
	
	block_value = 0;
	input_length = ustrlen(source);
	blocks = input_length / 3;
	remainder = input_length % 3;
	
	for(i = 0; i < blocks; i++) {
		block_value = ctoi(source[(i * 3)]) * 100;
		block_value += ctoi(source[(i * 3) + 1]) * 10;
		block_value += ctoi(source[(i * 3) + 2]);
		
		strcpy(block_binary, "");
		if(block_value & 0x200) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x100) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x80) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x40) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x20) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x10) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x08) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x04) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x02) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x01) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		concat(binary, block_binary);
	}
	
	if(remainder == 2) {
		block_value = ctoi(source[(i * 3)]) * 10;
		block_value += ctoi(source[(i * 3) + 1]);
	}
	if(remainder == 1) {
		block_value = ctoi(source[(i * 3)]);
	}
	
	strcpy(block_binary, "");
	switch(remainder) {
		case 2:
			if(block_value & 0x40) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
			if(block_value & 0x20) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
			if(block_value & 0x10) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		case 1:
			if(block_value & 0x08) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
			if(block_value & 0x04) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
			if(block_value & 0x02) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
			if(block_value & 0x01) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
			break;
	}
	concat(binary, block_binary);
	
	return;
}

void qralpha_encode(char binary[], unsigned char source[])
{ /* Encodes alphanumeric data according to 6.4.4 */
	
	int input_length, blocks, remainder, i;
	char block_binary[12];
	int block_value;
	
	input_length = ustrlen(source);
	blocks = input_length / 2;
	remainder = input_length % 2;
	
	for(i = 0; i < blocks; i++) {
		block_value = posn(QRSET, source[i * 2]) * 45;
		block_value += posn(QRSET, source[(i * 2) + 1]);
		
		strcpy(block_binary, "");
		if(block_value & 0x400) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x200) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x100) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x80) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x40) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x20) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x10) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x08) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x04) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x02) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x01) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		concat(binary, block_binary);
	}
	
	if(remainder == 1) {
		block_value = posn(QRSET, source[i * 2]);
		
		strcpy(block_binary, "");
		if(block_value & 0x20) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x10) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x08) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x04) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x02) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		if(block_value & 0x01) { concat(block_binary, "1"); } else { concat(block_binary, "0"); }
		concat(binary, block_binary);
	}
		
	return;
}

void qrbyte_encode(char binary[], unsigned char source[])
{ /* Encodes byte mode data according to 6.4.5 */
	
	int input_length, i;
	
	input_length = ustrlen(source);
	
	for(i = 0; i < input_length; i++) {
		if(source[i] & 0x80) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(source[i] & 0x40) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(source[i] & 0x20) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(source[i] & 0x10) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(source[i] & 0x08) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(source[i] & 0x04) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(source[i] & 0x02) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(source[i] & 0x01) { concat(binary, "1"); } else { concat(binary, "0"); }
	}
	
	return;
}

int qrkanji_encode(char binary[], unsigned char source[])
{ /* Assumes input is in Shift-JIS format */
	int i, len, h, val, count;
	
	len = ustrlen(source);
	count = 0;
	
	for(i=0; i<len; i+=2) {
		val = (source[i] << 8) | source[i+1];
		if(val <= 0x9ffc) {
			val -= 0x8140;
		} else {
			val -= 0xc140;
		}
		h = (val >> 8) * 0xc0;
		val = (val & 0xff) + h;

		if(val & 0x1000) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(val & 0x800) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(val & 0x400) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(val & 0x200) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(val & 0x100) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(val & 0x80) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(val & 0x40) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(val & 0x20) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(val & 0x10) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(val & 0x08) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(val & 0x04) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(val & 0x02) { concat(binary, "1"); } else { concat(binary, "0"); }
		if(val & 0x01) { concat(binary, "1"); } else { concat(binary, "0"); }
		count++;
	}
	
	return count;
}

void versionm1(char binary_data[], unsigned char source[])
{
	int input_length, i, latch;
	int bits_total, bits_left, remainder;
	int data_codewords, ecc_codewords;
	unsigned char data_blocks[4], ecc_blocks[3];
	
	input_length = ustrlen(source);
	bits_total = 20;
	latch = 0;
	
	/* Character count indicator */
	if(input_length & 0x04) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
	if(input_length & 0x02) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
	if(input_length & 0x01) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
	
	qrnumeric_encode(binary_data, source);
	
	/* Add terminator */
	bits_left = bits_total - strlen(binary_data);
	if(bits_left <= 3) {
		for(i = 0; i < bits_left; i++) {
			concat(binary_data, "0");
		}
		latch = 1;
	} else {
		concat(binary_data, "000");
	}
	
	if(latch == 0) {
		/* Manage last (4-bit) block */
		bits_left = bits_total - strlen(binary_data);
		if(bits_left <= 4) {
			for(i = 0; i < bits_left; i++) {
				concat(binary_data, "0");
			}
			latch = 1;
		}
	}

	if(latch == 0) {
		/* Complete current byte */
		remainder = 8 - (strlen(binary_data) % 8);
		if(remainder == 8) { remainder = 0; }
		for(i = 0; i < remainder; i++) {
			concat(binary_data, "0");
		}
		
		/* Add padding */
		bits_left = bits_total - strlen(binary_data);
		if(bits_left > 4) {
			remainder = (bits_left - 4) / 8;
			for(i = 0; i < remainder; i++) {
				if((i % 2) == 0) { concat(binary_data, "11101100"); }
				if((i % 2) == 1) { concat(binary_data, "00010001"); }
			}
		}
		concat(binary_data, "0000");
	}
	
	data_codewords = 3;
	ecc_codewords = 2;
	
	/* Copy data into codewords */
	for(i = 0; i < (data_codewords - 1); i++) {
		data_blocks[i] = 0;
		if(binary_data[i * 8] == '1') { data_blocks[i] += 0x80; }
		if(binary_data[(i * 8) + 1] == '1') { data_blocks[i] += 0x40; }
		if(binary_data[(i * 8) + 2] == '1') { data_blocks[i] += 0x20; }
		if(binary_data[(i * 8) + 3] == '1') { data_blocks[i] += 0x10; }
		if(binary_data[(i * 8) + 4] == '1') { data_blocks[i] += 0x08; }
		if(binary_data[(i * 8) + 5] == '1') { data_blocks[i] += 0x04; }
		if(binary_data[(i * 8) + 6] == '1') { data_blocks[i] += 0x02; }
		if(binary_data[(i * 8) + 7] == '1') { data_blocks[i] += 0x01; }
	}
	data_blocks[2] = 0;
	if(binary_data[16] == '1') { data_blocks[2] += 0x08; }
	if(binary_data[17] == '1') { data_blocks[2] += 0x04; }
	if(binary_data[18] == '1') { data_blocks[2] += 0x02; }
	if(binary_data[19] == '1') { data_blocks[2] += 0x01; }
	
	/* Calculate Reed-Solomon error codewords */
	rs_init_gf(0x11d);
	rs_init_code(ecc_codewords, 1);
	rs_encode(data_codewords,data_blocks,ecc_blocks);
	rs_free();
	
	/* Add Reed-Solomon codewords to binary data */
	for(i = 0; i < ecc_codewords; i++) {
		if(ecc_blocks[i] & 0x80) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x40) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x20) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x10) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x08) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x04) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x02) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x01) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
	}
	
	return;
}

void versionm2(char binary_data[], unsigned char source[], int char_system, int ecc_mode)
{
	int input_length, i, latch;
	int bits_total, bits_left, remainder;
	int data_codewords, ecc_codewords;
	unsigned char data_blocks[6], ecc_blocks[7];
	
	input_length = ustrlen(source);
	latch = 0;
	
	if(ecc_mode == 1) { bits_total = 40; }
	if(ecc_mode == 2) { bits_total = 32; }
	
	/* Mode indicator */
	if(char_system == NUMERIC) { concat(binary_data, "0"); }
	if(char_system == ALPHANUM) { concat(binary_data, "1"); }
	
	/* Character count indicator */
	if(char_system == NUMERIC) {
		if(input_length & 0x08) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
	}
	if(input_length & 0x04) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
	if(input_length & 0x02) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
	if(input_length & 0x01) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
	
	if(char_system == NUMERIC) { qrnumeric_encode(binary_data, source); }
	if(char_system == ALPHANUM) { qralpha_encode(binary_data, source); }
	
	/* Add terminator */
	bits_left = bits_total - strlen(binary_data);
	if(bits_left <= 5) {
		for(i = 0; i < bits_left; i++) {
			concat(binary_data, "0");
		}
		latch = 1;
	} else {
		concat(binary_data, "00000");
	}

	if(latch == 0) {
		/* Complete current byte */
		remainder = 8 - (strlen(binary_data) % 8);
		if(remainder == 8) { remainder = 0; }
		for(i = 0; i < remainder; i++) {
			concat(binary_data, "0");
		}
		
		/* Add padding */
		bits_left = bits_total - strlen(binary_data);
		remainder = bits_left / 8;
		for(i = 0; i < remainder; i++) {
			if((i % 2) == 0) { concat(binary_data, "11101100"); }
			if((i % 2) == 1) { concat(binary_data, "00010001"); }
		}
	}
	
	if(ecc_mode == 1) { data_codewords = 5; ecc_codewords = 5; }
	if(ecc_mode == 2) { data_codewords = 4; ecc_codewords = 6; }
	
	/* Copy data into codewords */
	for(i = 0; i < data_codewords; i++) {
		data_blocks[i] = 0;
		if(binary_data[i * 8] == '1') { data_blocks[i] += 0x80; }
		if(binary_data[(i * 8) + 1] == '1') { data_blocks[i] += 0x40; }
		if(binary_data[(i * 8) + 2] == '1') { data_blocks[i] += 0x20; }
		if(binary_data[(i * 8) + 3] == '1') { data_blocks[i] += 0x10; }
		if(binary_data[(i * 8) + 4] == '1') { data_blocks[i] += 0x08; }
		if(binary_data[(i * 8) + 5] == '1') { data_blocks[i] += 0x04; }
		if(binary_data[(i * 8) + 6] == '1') { data_blocks[i] += 0x02; }
		if(binary_data[(i * 8) + 7] == '1') { data_blocks[i] += 0x01; }
	}
	
	/* Calculate Reed-Solomon error codewords */
	rs_init_gf(0x11d);
	rs_init_code(ecc_codewords, 1);
	rs_encode(data_codewords,data_blocks,ecc_blocks);
	rs_free();
	
	/* Add Reed-Solomon codewords to binary data */
	for(i = 0; i < ecc_codewords; i++) {
		if(ecc_blocks[i] & 0x80) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x40) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x20) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x10) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x08) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x04) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x02) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x01) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
	}
	
	return;
}

void versionm3(char binary_data[], unsigned char source[], int char_system, int ecc_mode)
{
	int input_length, i, latch;
	int bits_total, bits_left, remainder;
	int data_codewords, ecc_codewords;
	unsigned char data_blocks[12], ecc_blocks[9];
	int sjis_count;
	
	input_length = ustrlen(source);
	latch = 0;
	
	if(ecc_mode == 1) { bits_total = 84; }
	if(ecc_mode == 2) { bits_total = 68; }
	
	/* Mode indicator */
	if(char_system == NUMERIC) { concat(binary_data, "00"); }
	if(char_system == ALPHANUM) { concat(binary_data, "01"); }
	if(char_system == BYTE) { concat(binary_data, "10"); }
	if(char_system == KANJI) { concat(binary_data, "11"); }
	
	/* Character count indicator */
	if(char_system == KANJI) {
		concat(binary_data, "XXX"); /* Place holder */
	} else {
		if(char_system == NUMERIC) {
			if(input_length & 0x10) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		}
		if(input_length & 0x08) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(input_length & 0x04) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(input_length & 0x02) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(input_length & 0x01) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
	}
	
	if(char_system == NUMERIC) { qrnumeric_encode(binary_data, source); }
	if(char_system == ALPHANUM) { qralpha_encode(binary_data, source); }
	if(char_system == BYTE) { qrbyte_encode(binary_data, source); }
	if(char_system == KANJI) { sjis_count = qrkanji_encode(binary_data, source); }
	
	if(char_system == KANJI) {
		if(sjis_count & 0x04) { binary_data[2] = '1'; } else { binary_data[2] = '0'; }
		if(sjis_count & 0x02) { binary_data[3] = '1'; } else { binary_data[3] = '0'; }
		if(sjis_count & 0x01) { binary_data[4] = '1'; } else { binary_data[4] = '0'; }
	}
	
	/* Add terminator */
	bits_left = bits_total - strlen(binary_data);
	if(bits_left <= 7) {
		for(i = 0; i < bits_left; i++) {
			concat(binary_data, "0");
		}
		latch = 1;
	} else {
		concat(binary_data, "0000000");
	}
	
	if(latch == 0) {
		/* Manage last (4-bit) block */
		bits_left = bits_total - strlen(binary_data);
		if(bits_left <= 4) {
			for(i = 0; i < bits_left; i++) {
				concat(binary_data, "0");
			}
			latch = 1;
		}
	}
	
	if(latch == 0) {
		/* Complete current byte */
		remainder = 8 - (strlen(binary_data) % 8);
		if(remainder == 8) { remainder = 0; }
		for(i = 0; i < remainder; i++) {
			concat(binary_data, "0");
		}
		
		/* Add padding */
		bits_left = bits_total - strlen(binary_data);
		if(bits_left > 4) {
			remainder = (bits_left - 4) / 8;
			for(i = 0; i < remainder; i++) {
				if((i % 2) == 0) { concat(binary_data, "11101100"); }
				if((i % 2) == 1) { concat(binary_data, "00010001"); }
			}
		}
		concat(binary_data, "0000");
	}
	
	if(ecc_mode == 1) { data_codewords = 11; ecc_codewords = 6; }
	if(ecc_mode == 2) { data_codewords = 9; ecc_codewords = 8; }
	
	/* Copy data into codewords */
	for(i = 0; i < (data_codewords - 1); i++) {
		data_blocks[i] = 0;
		if(binary_data[i * 8] == '1') { data_blocks[i] += 0x80; }
		if(binary_data[(i * 8) + 1] == '1') { data_blocks[i] += 0x40; }
		if(binary_data[(i * 8) + 2] == '1') { data_blocks[i] += 0x20; }
		if(binary_data[(i * 8) + 3] == '1') { data_blocks[i] += 0x10; }
		if(binary_data[(i * 8) + 4] == '1') { data_blocks[i] += 0x08; }
		if(binary_data[(i * 8) + 5] == '1') { data_blocks[i] += 0x04; }
		if(binary_data[(i * 8) + 6] == '1') { data_blocks[i] += 0x02; }
		if(binary_data[(i * 8) + 7] == '1') { data_blocks[i] += 0x01; }
	}
	
	if(ecc_mode == 1) {
		data_blocks[11] = 0;
		if(binary_data[80] == '1') { data_blocks[2] += 0x08; }
		if(binary_data[81] == '1') { data_blocks[2] += 0x04; }
		if(binary_data[82] == '1') { data_blocks[2] += 0x02; }
		if(binary_data[83] == '1') { data_blocks[2] += 0x01; }
	}
	
	if(ecc_mode == 2) {
		data_blocks[9] = 0;
		if(binary_data[64] == '1') { data_blocks[2] += 0x08; }
		if(binary_data[65] == '1') { data_blocks[2] += 0x04; }
		if(binary_data[66] == '1') { data_blocks[2] += 0x02; }
		if(binary_data[67] == '1') { data_blocks[2] += 0x01; }
	}
	
	/* Calculate Reed-Solomon error codewords */
	rs_init_gf(0x11d);
	rs_init_code(ecc_codewords, 1);
	rs_encode(data_codewords,data_blocks,ecc_blocks);
	rs_free();
	
	/* Add Reed-Solomon codewords to binary data */
	for(i = 0; i < ecc_codewords; i++) {
		if(ecc_blocks[i] & 0x80) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x40) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x20) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x10) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x08) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x04) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x02) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x01) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
	}
	
	return;
}

void versionm4(char binary_data[], unsigned char source[], int char_system, int ecc_mode)
{
	int input_length, i, latch;
	int bits_total, bits_left, remainder;
	int data_codewords, ecc_codewords;
	unsigned char data_blocks[17], ecc_blocks[15];
	int sjis_count;
	
	input_length = ustrlen(source);
	latch = 0;
	
	if(ecc_mode == 1) { bits_total = 128; }
	if(ecc_mode == 2) { bits_total = 112; }
	if(ecc_mode == 3) { bits_total = 80; }
	
	/* Mode indicator */
	if(char_system == NUMERIC) { concat(binary_data, "000"); }
	if(char_system == ALPHANUM) { concat(binary_data, "001"); }
	if(char_system == BYTE) { concat(binary_data, "010"); }
	if(char_system == KANJI) { concat(binary_data, "011"); }
	
	/* Character count indicator */
	if(char_system == KANJI) {
		concat(binary_data, "XXXX"); /* Place holder */
	} else {
		if(char_system == NUMERIC) {
			if(input_length & 0x20) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		}
		if(input_length & 0x10) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(input_length & 0x08) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(input_length & 0x04) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(input_length & 0x02) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(input_length & 0x01) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
	}
	
	if(char_system == NUMERIC) { qrnumeric_encode(binary_data, source); }
	if(char_system == ALPHANUM) { qralpha_encode(binary_data, source); }
	if(char_system == BYTE) { qrbyte_encode(binary_data, source); }
	if(char_system == KANJI) { sjis_count = qrkanji_encode(binary_data, source); }
	
	if(char_system == KANJI) {
		if(sjis_count & 0x08) { binary_data[3] = '1'; } else { binary_data[3] = '0'; }
		if(sjis_count & 0x04) { binary_data[4] = '1'; } else { binary_data[4] = '0'; }
		if(sjis_count & 0x02) { binary_data[5] = '1'; } else { binary_data[5] = '0'; }
		if(sjis_count & 0x01) { binary_data[6] = '1'; } else { binary_data[6] = '0'; }
	}
	
	/* Add terminator */
	bits_left = bits_total - strlen(binary_data);
	if(bits_left <= 9) {
		for(i = 0; i < bits_left; i++) {
			concat(binary_data, "0");
		}
		latch = 1;
	} else {
		concat(binary_data, "000000000");
	}
	
	if(latch == 0) {
		/* Complete current byte */
		remainder = 8 - (strlen(binary_data) % 8);
		if(remainder == 8) { remainder = 0; }
		for(i = 0; i < remainder; i++) {
			concat(binary_data, "0");
		}
	
		/* Add padding */
		bits_left = bits_total - strlen(binary_data);
		remainder = bits_left / 8;
		for(i = 0; i < remainder; i++) {
			if((i % 2) == 0) { concat(binary_data, "11101100"); }
			if((i % 2) == 1) { concat(binary_data, "00010001"); }
		}
	}
	
	if(ecc_mode == 1) { data_codewords = 16; ecc_codewords = 8; }
	if(ecc_mode == 2) { data_codewords = 14; ecc_codewords = 10; }
	if(ecc_mode == 3) { data_codewords = 10; ecc_codewords = 14; }
	
	/* Copy data into codewords */
	for(i = 0; i < data_codewords; i++) {
		data_blocks[i] = 0;
		if(binary_data[i * 8] == '1') { data_blocks[i] += 0x80; }
		if(binary_data[(i * 8) + 1] == '1') { data_blocks[i] += 0x40; }
		if(binary_data[(i * 8) + 2] == '1') { data_blocks[i] += 0x20; }
		if(binary_data[(i * 8) + 3] == '1') { data_blocks[i] += 0x10; }
		if(binary_data[(i * 8) + 4] == '1') { data_blocks[i] += 0x08; }
		if(binary_data[(i * 8) + 5] == '1') { data_blocks[i] += 0x04; }
		if(binary_data[(i * 8) + 6] == '1') { data_blocks[i] += 0x02; }
		if(binary_data[(i * 8) + 7] == '1') { data_blocks[i] += 0x01; }
	}
	
	/* Calculate Reed-Solomon error codewords */
	rs_init_gf(0x11d);
	rs_init_code(ecc_codewords, 1);
	rs_encode(data_codewords,data_blocks,ecc_blocks);
	rs_free();
	
	/* Add Reed-Solomon codewords to binary data */
	for(i = 0; i < ecc_codewords; i++) {
		if(ecc_blocks[i] & 0x80) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x40) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x20) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x10) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x08) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x04) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x02) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
		if(ecc_blocks[i] & 0x01) { concat(binary_data, "1"); } else { concat(binary_data, "0"); }
	}
	
	return;
}

int microqr(struct zint_symbol *symbol, unsigned char source[])
{
	int symbol_size;
	int char_system, input_length;
	char binary_data[200];
	int latch;
	char bitmask[17][17];
	char imagemap[17][17];
	char candidate[17][17];
	char pattern_bit;
	int width, i, j, pattern_no;
	int sum1, sum2, evaluation[4], format, format_full;
	char formatstr[16];
	
	/* Analise input data and select encoding method - zint does not attempt to
	optimise the symbol by switching encoding method part way through the symbol,
	but merely chooses an encoding method for the whole symbol */
	input_length = ustrlen(source);
	char_system = BYTE;
	symbol_size = 0;
	if(is_sane(QRSET, source) == 0) { char_system = ALPHANUM; }
	if(is_sane(NESET, source) == 0) { char_system = NUMERIC; }
	if(symbol->input_mode == KANJI_MODE) { char_system = KANJI; }
	if(symbol->input_mode == SJIS_MODE) { char_system = KANJI; }
	width = 0;
	format = 0;
	
	if(symbol->option_1 == 4) {
		strcpy(symbol->errtxt, "Error correction level H not available for Micro QR symbols");
		return ERROR_INVALID_OPTION;
	}
	
	if((symbol->option_1 < 1) || (symbol->option_1 > 4)) {
		symbol->option_1 = 1;
	}
	
	/* Check that the data is not too long */
	/* Note that there is no switching between error correction levels - this decision is left
	   to the user: invalid combinations fail */
	latch = 0;
	switch(symbol->option_1) {
		case 1: /* ECC Level L */
			switch(char_system) {
				case NUMERIC: if(input_length > 35) latch = 1; break;
				case ALPHANUM: if(input_length > 21) latch = 1; break;
				case BYTE: if(input_length > 15) latch = 1; break;
				case KANJI: if(input_length > 18) latch = 1; break;
			}
			break;
		case 2: /* ECC Level M */
			switch(char_system) {
				case NUMERIC: if(input_length > 30) latch = 1; break;
				case ALPHANUM: if(input_length > 18) latch = 1; break;
				case BYTE: if(input_length > 13) latch = 1; break;
				case KANJI: if(input_length > 16) latch = 1; break;
			}
			break;
		case 3: /* ECC Level Q */
			symbol_size = 4; /* Only size M4 supports level Q */
			switch(char_system) {
				case NUMERIC: if(input_length > 21) latch = 1; break;
				case ALPHANUM: if(input_length > 13) latch = 1; break;
				case BYTE: if(input_length > 9) latch = 1; break;
				case KANJI: if(input_length > 10) latch = 1; break;
			}
			break;
	}
	
	if(latch == 1) {
		strcpy(symbol->errtxt, "Input data too long");
		return ERROR_TOO_LONG;
	}
	
	/* Decide symbol size */
	if(symbol_size == 0) {
		if(symbol->option_1 == 1) { /* ECC Level L */
			switch(char_system) {
				case NUMERIC:
					symbol_size = 4;
					if(input_length <= 23) { symbol_size = 3; }
					if(input_length <= 10) { symbol_size = 2; }
					if(input_length <= 5) { symbol_size = 1; }
					break;
				case ALPHANUM:
					symbol_size = 4;
					if(input_length <= 14) { symbol_size = 3; }
					if(input_length <= 6) { symbol_size = 2; }
					break;
				case BYTE:
					symbol_size = 4;
					if(input_length <= 9) { symbol_size = 3; }
					break;
				case KANJI:
					symbol_size = 4;
					if(input_length <= 12) { symbol_size = 3; }
			}
		} else { /* ECC Level M */
			switch(char_system) {
				case NUMERIC:
					symbol_size = 4;
					if(input_length <= 18) { symbol_size = 3; }
					if(input_length <= 8) { symbol_size = 2; }
					break;
				case ALPHANUM:
					symbol_size = 4;
					if(input_length <= 11) { symbol_size = 3; }
					if(input_length <= 5) { symbol_size = 2; }
					break;
				case BYTE:
					symbol_size = 4;
					if(input_length <= 7) { symbol_size = 3; }
					break;
				case KANJI:
					symbol_size = 4;
					if(input_length <= 8) { symbol_size = 3; }
			}
		}
	}
	
	strcpy(binary_data, "");
	switch(symbol_size) {
		case 1: versionm1(binary_data, source); break;
		case 2: versionm2(binary_data, source, char_system, symbol->option_1); break;
		case 3: versionm3(binary_data, source, char_system, symbol->option_1); break;
		case 4: versionm4(binary_data, source, char_system, symbol->option_1); break;
	}
	
	switch(symbol_size) {
		case 1: width = 11; break;
		case 2: width = 13; break;
		case 3: width = 15; break;
		case 4: width = 17; break;
	}
	
	for(i = 0; i < 17; i++) {
		for(j = 0; j < 17; j++) {
			bitmask[i][j] = '0';
			imagemap[i][j] = '0';
			candidate[i][j] = '0';
		}
	}
	
	
	/* "bitmask" seperates data area */
	for(i = 1; i < width; i++) {
		for(j = 1; j < width; j++) {
			bitmask[i][j] = '1';
		}
	}
	
	for(i = 1; i < 9; i++) {
		for(j = 1; j < 9; j++) {
			bitmask[i][j] = '0';
		}
	}
	
	/* Copy data into symbol grid */
	for(i = 0; i < width; i++) {
		for(j = 0; j < width; j++) {
			if(bitmask[i][j] == '1') {
				switch(symbol_size) {
					case 1: imagemap[i][j] = binary_data[fig11m1[(i * width) + j]]; break;
					case 2: imagemap[i][j] = binary_data[fig11m2[(i * width) + j]]; break;
					case 3: imagemap[i][j] = binary_data[fig11m3[(i * width) + j]]; break;
					case 4: imagemap[i][j] = binary_data[fig11m4[(i * width) + j]]; break;
				}
			}
		}
	}
	
	/* XOR with data masks and evaluate */
	for(pattern_no = 0; pattern_no < 4; pattern_no++) {
		for(i = 0; i < width; i++) {
			for(j = 0; j < width; j++) {
				pattern_bit = '0';
				candidate[i][j] = '0';
				switch(pattern_no) {
					case 0: if((i % 2) == 0) { pattern_bit = '1'; } break;
					case 1: if((((i / 2) + (j / 3)) % 2) == 0) { pattern_bit = '1'; } break;
					case 2: if((((i * j) % 2 + (i * j) % 3) % 2) == 0) { pattern_bit = '1'; } break;
					case 3: if((((i + j) % 2 + (i * j) % 3) % 2) == 0) { pattern_bit = '1'; } break;
				}
				if(bitmask[i][j] == '1') {
					if(pattern_bit != imagemap[i][j]) { candidate[i][j] = '1'; }
				}
			}
		}
		
		sum1 = 0;
		sum2 = 0;
		for(i = 1; i < width; i++) {
			if(candidate[i][width - 1] == '1') { sum1++; }
			if(candidate[width - 1][i] == '1') { sum2++; }
		}
		
		if(sum1 <= sum2) { evaluation[pattern_no] = (sum1 * 16) + sum2; } else { evaluation[pattern_no] = (sum2 * 16) + sum1; }
	}
	
	/* Choose best data mask */
	j = evaluation[0];
	pattern_no = 0;
	for(i = 1; i < 4; i++) {
		if(evaluation[i] > j) {
			pattern_no = i;
			j = evaluation[i];
		}
	}
	
	/* Apply data mask */
	for(i = 0; i < width; i++) {
		for(j = 0; j < width; j++) {
			pattern_bit = '0';
			candidate[i][j] = '0';
			switch(pattern_no) {
				case 0: if((i % 2) == 0) { pattern_bit = '1'; } break;
				case 1: if((((i / 2) + (j / 3)) % 2) == 0) { pattern_bit = '1'; } break;
				case 2: if((((i * j) % 2 + (i * j) % 3) % 2) == 0) { pattern_bit = '1'; } break;
				case 3: if((((i + j) % 2 + (i * j) % 3) % 2) == 0) { pattern_bit = '1'; } break;
			}
			if(bitmask[i][j] == '1') {
				if(pattern_bit != imagemap[i][j]) { candidate[i][j] = '1'; }
			}
		}
	}
	
	/* Calculate format data */
	switch(symbol_size) {
		case 1: format = 0;
			break;
		case 2: switch(symbol->option_1) {
				case 1: format = 1; break;
				case 2: format = 2; break;
			}
			break;
		case 3: switch(symbol->option_1) {
				case 1: format = 3; break;
				case 2: format = 4; break;
			}
			break;
		case 4: switch(symbol->option_1) {
				case 1: format = 5; break;
				case 2: format = 6; break;
				case 3: format = 7; break;
			}
			break;
	}
	
	format *= 4;
	format += pattern_no;
	format_full = tablec1[format];
	
	strcpy(formatstr, "");
	if(format_full & 0x2000) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x1000) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x800) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x400) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x200) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x100) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x80) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x80) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x40) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x20) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x10) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x08) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x04) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x02) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	if(format_full & 0x01) { concat(formatstr, "1"); } else { concat(formatstr, "0"); }
	
	/* Add format data to symbol */
	for(i = 0; i < 8; i++) {
		candidate[i + 1][8] = formatstr[i];
	}
	for(i = 0; i < 7; i++) {
		candidate[8][7 - i] = formatstr[i + 8];
	}
	
	/* Add timer pattern */
	for(i = 0; i < width; i += 2) {
		candidate[i][0] = '1';
		candidate[0][i] = '1';
	}
	
	/* Add finder pattern */
	for(i = 0; i < 7; i ++) {
		for(j = 0; j < 7; j++) {
			if(finder[(i * 7) + j] == 1) {
				candidate[i][j] = '1';
			}
		}
	}
	
	/* Copy data into symbol */
	for(i = 0; i < width; i++) {
		for(j = 0; j < width; j++) {
			symbol->encoded_data[i][j] = candidate[i][j];
		}
		symbol->row_height[i] = 1;
	}
	symbol->rows = width;
	symbol->width = width;
	
	return 0;
}