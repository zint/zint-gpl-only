/* qr.c Handles QR Code */

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
#ifdef _MSC_VER
#include <malloc.h>
#endif
#include "common.h"
#include <stdio.h>
#include "sjis.h"
#include "qr.h"
#include "qrrs.h"

int in_alpha(int glyph) {
	/* Returns true if input glyph is in the Alphanumeric set */
	int retval = 0;
	char cglyph = (char) glyph;
	
	if((cglyph >= '0') && (cglyph <= '9')) {
		retval = 1;
	}
	if((cglyph >= 'A') && (cglyph <= 'Z')) {
		retval = 1;
	}
	switch (cglyph) {
		case ' ':
		case '$':
		case '%':
		case '*':
		case '+':
		case '-':
		case '.':
		case '/':
		case ':':
			retval = 1;
			break;
	}
	
	return retval;
}

void define_mode(char mode[], int jisdata[], int length)
{
	/* Values placed into mode[] are: K = Kanji, B = Binary, A = Alphanumeric, N = Numeric */
	int i, mlen, j;
	
	for(i = 0; i < length; i++) {
		if(jisdata[i] > 0xff) { 
			mode[i] = 'K';
		} else {
			mode[i] = 'B';
			if(in_alpha(jisdata[i])) { mode[i] = 'A'; }
			if((jisdata[i] >= '0') && (jisdata[i] <= '9')) { mode[i] = 'N'; }
		}
	}
	
	/* If less than 6 numeric digits together then don't use numeric mode */
	for(i = 0; i < length; i++) {
		if(mode[i] == 'N') {
			if(((i != 0) && (mode[i - 1] != 'N')) || (i == 0)) {
				mlen = 0;
				while (((mlen + i) < length) && (mode[mlen + i] == 'N')) {
					mlen++;
				};
				if(mlen < 6) {
					for(j = 0; j < mlen; j++) {
						mode[i + j] = 'A';
					}
				}
			}
		}
	}
	
	/* If less than 4 alphanumeric characters together then don't use alphanumeric mode */
	for(i = 0; i < length; i++) {
		if(mode[i] == 'A') {
			if(((i != 0) && (mode[i - 1] != 'A')) || (i == 0)) {
				mlen = 0;
				while (((mlen + i) < length) && (mode[mlen + i] == 'A')) {
					mlen++;
				};
				if(mlen < 6) {
					for(j = 0; j < mlen; j++) {
						mode[i + j] = 'B';
					}
				}
			}
		}
	}
}

int estimate_binary_length(char mode[], int length)
{
	/* Make an estimate (worst case scenario) of how long the binary string will be */
	int i, count = 0;
	char current = 0;
	int a_count = 0;
	int n_count = 0;
	
	switch(mode[0]) {
		case 'K': count = 12 + 4; current = 'K'; break;
		case 'B': count = 16 + 4; current = 'B'; break;
		case 'A': count = 13 + 4; current = 'A'; break;
		case 'N': count = 14 + 4; current = 'N'; break;
	}
	
	for(i = 0; i < length; i++) {
		if(mode[i] != current) {
			if(current == 'N') {
				switch(n_count) {
					case 1: count += 4; break;
					case 2: count += 7; break;
				}
			}
			
			switch(mode[i]) {
				case 'K': count += 12 + 4; current = 'K'; break;
				case 'B': count += 16 + 4; current = 'B'; break;
				case 'A': count += 13 + 4; current = 'A'; break;
				case 'N': count += 14 + 4; current = 'N'; break;
			}
		}
		
		switch(mode[i]) {
			case 'K': count += 13; break;
			case 'B': count += 8; break;
			case 'A': a_count++; if((a_count % 2) == 0) { count += 11; a_count = 0; } break;
			case 'N': n_count++; if((n_count % 3) == 0) { count += 10; n_count = 0; } break;
		}
		
	}
	
	return count;
}

void qr_binary(int datastream[], int version, int target_binlen, char mode[], int jisdata[], int length)
{
	/* Convert input data to a binary stream and add padding */
	int position = 0, debug = 0;
	int short_data_block_length, i, scheme;
	char data_block, padbits;
	int current_binlen, current_bytes;
	int toggle;
	
#ifndef _MSC_VER
	char binary[target_binlen * 8];
#else
	char* binary = (char *)_alloca(target_binlen * 8);
#endif
	strcpy(binary, "");
	
	if(version <= 9) {
		scheme = 1;
	}
	if((version >= 10) && (version <= 26)) {
		scheme = 2;
	}
	if(version >= 27) {
		scheme = 3;
	}
	
	if(debug) { 
		for(i = 0; i < length; i++) {
			printf("%c", mode[i]);
		}
		printf("\n");
	}
	
	do {
		data_block = mode[position];
		short_data_block_length = 0;
		do {
			short_data_block_length++;
		} while (((short_data_block_length + position) < length) && (mode[position + short_data_block_length] == data_block));
		
		switch(data_block) {
			case 'K':
				/* Kanji mode */
				/* Mode indicator */
				concat(binary, "1000");
				
				/* Character count indicator */
				switch(scheme) {
					case 3:
						if(short_data_block_length & 0x800) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x400) { concat(binary, "1"); } else { concat(binary, "0"); }
					case 2:
						if(short_data_block_length & 0x200) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x100) { concat(binary, "1"); } else { concat(binary, "0"); }
					case 1:
						if(short_data_block_length & 0x80) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x40) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x20) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x10) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x08) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x04) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x02) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x01) { concat(binary, "1"); } else { concat(binary, "0"); }
						break;
				}
				
				if(debug) { printf("Kanji block (length %d)\n\t", short_data_block_length); }
				
				/* Character representation */
				for(i = 0; i < short_data_block_length; i++) {
					int jis = jisdata[position + i];
					int msb, lsb, prod;
					
					if(jis > 0x9fff) { jis -= 0xc140; }
					msb = (jis & 0xff00) >> 4;
					lsb = (jis & 0xff);
					prod = (msb * 0xc0) + lsb;
					
					if(prod & 0x1000) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(prod & 0x800) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(prod & 0x400) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(prod & 0x200) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(prod & 0x100) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(prod & 0x80) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(prod & 0x40) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(prod & 0x20) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(prod & 0x10) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(prod & 0x08) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(prod & 0x04) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(prod & 0x02) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(prod & 0x01) { concat(binary, "1"); } else { concat(binary, "0"); }
					
					if(debug) { printf("0x%4X ", prod); }
				}
				
				if(debug) { printf("\n"); }
				
				break;
			case 'B':
				/* Byte mode */
				/* Mode indicator */
				concat(binary, "0100");
				
				/* Character count indicator */
				switch (scheme) {
					case 3:
					case 2:
						if(short_data_block_length & 0x8000) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x4000) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x2000) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x1000) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x800) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x400) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x200) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x100) { concat(binary, "1"); } else { concat(binary, "0"); }
					case 1:
						if(short_data_block_length & 0x80) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x40) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x20) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x10) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x08) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x04) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x02) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x01) { concat(binary, "1"); } else { concat(binary, "0"); }
						break;
				}
				
				if(debug) { printf("Byte block (length %d)\n\t", short_data_block_length); }
				
				/* Character representation */
				for(i = 0; i < short_data_block_length; i++) {
					int byte = jisdata[position + i];
					
					if(byte & 0x80) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(byte & 0x40) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(byte & 0x20) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(byte & 0x10) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(byte & 0x08) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(byte & 0x04) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(byte & 0x02) { concat(binary, "1"); } else { concat(binary, "0"); }
					if(byte & 0x01) { concat(binary, "1"); } else { concat(binary, "0"); }
					
					if(debug) { printf("0x%4X ", byte); }
				}
				
				if(debug) { printf("\n"); }
				
				break;
			case 'A':
				/* Alphanumeric mode */
				/* Mode indicator */
				concat(binary, "0010");
				
				/* Character count indicator */
				switch (scheme) {
					case 3:
						if(short_data_block_length & 0x1000) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x800) { concat(binary, "1"); } else { concat(binary, "0"); }
					case 2:
						if(short_data_block_length & 0x400) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x200) { concat(binary, "1"); } else { concat(binary, "0"); }
					case 1:
						if(short_data_block_length & 0x100) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x80) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x40) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x20) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x10) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x08) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x04) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x02) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x01) { concat(binary, "1"); } else { concat(binary, "0"); }
						break;
				}
				
				if(debug) { printf("Alpha block (length %d)\n\t", short_data_block_length); }
				
				/* Character representation */
				i = 0; 
				while ( i < short_data_block_length ) {
					int count;
					int first = 0, second = 0, prod;
					
					first = posn(RHODIUM, (char) jisdata[position + i]);
					count = 1;
					prod = first;
					
					if(mode[position + i + 1] == 'A') {
						second = posn(RHODIUM, (char) jisdata[position + i + 1]);
						count = 2;
						prod = (first * 45) + second;
					}
					
					switch(count) {
						case 2:
							if(prod & 0x400) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x200) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x100) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x80) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x40) { concat(binary, "1"); } else { concat(binary, "0"); }
						case 1:
							if(prod & 0x20) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x10) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x08) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x04) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x02) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x01) { concat(binary, "1"); } else { concat(binary, "0"); }
							break;
					}
					
					if(debug) { printf("0x%4X ", prod); }
					
					i += 2;
				};
				
				if(debug) { printf("\n"); }
				
				break;
			case 'N':
				/* Numeric mode */
				/* Mode indicator */
				concat(binary, "0001");
				
				/* Character count indicator */
				switch (scheme) {
					case 3:
						if(short_data_block_length & 0x2000) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x1000) { concat(binary, "1"); } else { concat(binary, "0"); }
					case 2:
						if(short_data_block_length & 0x800) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x400) { concat(binary, "1"); } else { concat(binary, "0"); }
					case 1:
						if(short_data_block_length & 0x200) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x100) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x80) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x40) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x20) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x10) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x08) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x04) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x02) { concat(binary, "1"); } else { concat(binary, "0"); }
						if(short_data_block_length & 0x01) { concat(binary, "1"); } else { concat(binary, "0"); }
						break;
				}
				
				if(debug) { printf("Number block (length %d)\n\t", short_data_block_length); }
				
								/* Character representation */
				i = 0; 
				while ( i < short_data_block_length ) {
					int count;
					int first = 0, second = 0, third = 0, prod;
					
					first = posn(NEON, (char) jisdata[position + i]);
					count = 1;
					prod = first;
					
					if(mode[position + i + 1] == 'N') {
						second = posn(NEON, (char) jisdata[position + i + 1]);
						count = 2;
						prod = (prod * 10) + second;
					}
					
					if(mode[position + i + 2] == 'N') {
						third = posn(NEON, (char) jisdata[position + i + 2]);
						count = 3;
						prod = (prod * 10) + third;
					}
					
					switch(count) {
						case 3:
							if(prod & 0x200) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x100) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x80) { concat(binary, "1"); } else { concat(binary, "0"); }
						case 2:
							if(prod & 0x40) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x20) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x10) { concat(binary, "1"); } else { concat(binary, "0"); }
						case 1:
							if(prod & 0x08) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x04) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x02) { concat(binary, "1"); } else { concat(binary, "0"); }
							if(prod & 0x01) { concat(binary, "1"); } else { concat(binary, "0"); }
							break;
					}
					
					if(debug) { printf("0x%4X (%d)", prod, prod); }
					
					i += 3;
				};
				
				if(debug) { printf("\n"); }
				
				break;
		}
		
		position += short_data_block_length;
	} while (position < length - 1) ;
	
	/* Terminator */
	concat(binary, "0000");

	current_binlen = strlen(binary);
	padbits = 8 - (current_binlen % 8);
	if(padbits == 8) { padbits = 0; }
	current_bytes = (current_binlen + padbits) / 8;
	
	/* Padding bits */
	for(i = 0; i < padbits; i++) {
		concat(binary, "0");
	}

	/* Put data into 8-bit codewords */
	for(i = 0; i < current_bytes; i++) {
		datastream[i] = 0x00;
		if(binary[i * 8] == '1') { datastream[i] += 0x80; }
		if(binary[i * 8 + 1] == '1') { datastream[i] += 0x40; }
		if(binary[i * 8 + 2] == '1') { datastream[i] += 0x20; }
		if(binary[i * 8 + 3] == '1') { datastream[i] += 0x10; }
		if(binary[i * 8 + 4] == '1') { datastream[i] += 0x08; }
		if(binary[i * 8 + 5] == '1') { datastream[i] += 0x04; }
		if(binary[i * 8 + 6] == '1') { datastream[i] += 0x02; }
		if(binary[i * 8 + 7] == '1') { datastream[i] += 0x01; }
	}
	
	/* Add pad codewords */
	toggle = 0;
	for(i = current_bytes; i < target_binlen; i++) {
		if(toggle == 0) {
			datastream[i] = 0xec;
			toggle = 1;
		} else {
			datastream[i] = 0x11;
			toggle = 0;
		}
	}

	if(debug) {
		for(i = 0; i < target_binlen; i++) {
			printf("0x%2X ", datastream[i]);
		}
		printf("\n");
	}
}

void add_ecc(int fullstream[], int datastream[], int version, int data_cw, int blocks)
{
	/* Split data into blocks, add error correction and then interleave the blocks and error correction data */
	int ecc_cw = qr_total_codewords[version - 1] - data_cw;
	int short_data_block_length = data_cw / blocks;
	int qty_long_blocks = data_cw % blocks;
	int qty_short_blocks = blocks - qty_long_blocks;
	int ecc_block_length = ecc_cw / blocks;
	int i, j, length_this_block, posn, debug = 0;
	RS *rs;
	
	
#ifndef _MSC_VER
	unsigned char data_block[short_data_block_length + 2];
	unsigned char ecc_block[ecc_block_length + 2];
	int interleaved_data[data_cw + 2];
	int interleaved_ecc[ecc_cw + 2];
#else
	unsigned char* data_block = (unsigned char *)_alloca(short_data_block_length + 2);
	unsigned char* ecc_block = (unsigned char *)_alloca(ecc_block_length + 2);
	int* interleaved_data = (int *)_alloca((data_cw + 2) * sizeof(int));
	int* interleaved_ecc = (int *)_alloca((ecc_cw + 2) * sizeof(int));
#endif

	posn = 0;
	
	for(i = 0; i < blocks; i++) {
		if(i < qty_short_blocks) { length_this_block = short_data_block_length; } else { length_this_block = short_data_block_length + 1; }
		
		for(j = 0; j < ecc_block_length; j++) {
			ecc_block[j] = 0;
		}
		
		for(j = 0; j < length_this_block; j++) {
			data_block[j] = (unsigned char) datastream[posn + j];
		}
		
		rs = init_rs(8, 0x11d, 0, 1, ecc_block_length, 255 - length_this_block - ecc_block_length);
		encode_rs_char(rs, data_block, ecc_block);
		
		if(debug) {
			printf("Block %d: ", i + 1);
			for(j = 0; j < length_this_block; j++) {
				printf("%2X ", data_block[j]);
			}
			if(i < qty_short_blocks) {
				printf("   ");
			}
			printf(" // ");
			for(j = 0; j < ecc_block_length; j++) {
				printf("%2X ", ecc_block[j]);
			}
			printf("\n");
		}
		
		for(j = 0; j < short_data_block_length; j++) {
			interleaved_data[(j * blocks) + i] = (int) data_block[j];
		}
		
		if(i >= qty_short_blocks){
			interleaved_data[(short_data_block_length * blocks) + (i - qty_short_blocks)] = (int) data_block[short_data_block_length];
		}
		
		for(j = 0; j < ecc_block_length; j++) {
			interleaved_ecc[(j * blocks) + i] = (int) ecc_block[j];
		}
		
		posn += length_this_block;
	}
	free_rs_cache();
	
	for(j = 0; j < data_cw; j++) {
		fullstream[j] = interleaved_data[j];
	}
	for(j = 0; j < ecc_cw; j++) {
		fullstream[j + data_cw] = interleaved_ecc[j];
	}
	
	if(debug) {
		printf("\nData Stream: \n");
		for(j = 0; j < (data_cw + ecc_cw); j++) {
			printf("%2X ", fullstream[j]);
		}
		printf("\n");
	}
}

void place_finder(unsigned char grid[], int size, int x, int y)
{
	int xp, yp;
	
	int finder[] = {
		1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 1,
		1, 0, 1, 1, 1, 0, 1,
		1, 0, 1, 1, 1, 0, 1,
		1, 0, 1, 1, 1, 0, 1,
		1, 0, 0, 0, 0, 0, 1,
		1, 1, 1, 1, 1, 1, 1
	};
	
	for(xp = 0; xp < 7; xp++) {
		for(yp = 0; yp < 7; yp++) {
			if (finder[xp + (7 * yp)] == 1) {
				grid[((yp + y) * size) + (xp + x)] = 0x11;
			} else {
				grid[((yp + y) * size) + (xp + x)] = 0x10;
			}
		}
	}
}

void place_align(unsigned char grid[], int size, int x, int y)
{
	int xp, yp;
	
	int alignment[] = {
		1, 1, 1, 1, 1,
		1, 0, 0, 0, 1,
		1, 0, 1, 0, 1,
		1, 0, 0, 0, 1,
		1, 1, 1, 1, 1
	};
	
	x -= 2;
	y -= 2; /* Input values represent centre of pattern */
	
	for(xp = 0; xp < 5; xp++) {
		for(yp = 0; yp < 5; yp++) {
			if (alignment[xp + (5 * yp)] == 1) {
				grid[((yp + y) * size) + (xp + x)] = 0x11;
			} else {
				grid[((yp + y) * size) + (xp + x)] = 0x10;
			}
		}
	}
}

void setup_grid(unsigned char* grid, int size, int version)
{
	int i, toggle = 1;
	int loopsize, x, y, xcoord, ycoord;

	/* Add timing patterns */
	for(i = 0; i < size; i++) {
		if(toggle == 1) {
			grid[(6 * size) + i] = 0x21;
			grid[(i * size) + 6] = 0x21;
			toggle = 0;
		} else {
			grid[(6 * size) + i] = 0x20;
			grid[(i * size) + 6] = 0x20;
			toggle = 1;
		}
	}
	
	/* Add finder patterns */
	place_finder(grid, size, 0, 0);
	place_finder(grid, size, 0, size - 7);
	place_finder(grid, size, size - 7, 0);
	
	/* Add separators */
	for(i = 0; i < 7; i++) {
		grid[(7 * size) + i] = 0x10;
		grid[(i * size) + 7] = 0x10;
		grid[(7 * size) + (size - 1 - i)] = 0x10;
		grid[(i * size) + (size - 8)] = 0x10;
		grid[((size - 8) * size) + i] = 0x10;
		grid[((size - 1 - i) * size) + 7] = 0x10;
	}
	grid[(7 * size) + 7] = 0x10;
	grid[(7 * size) + (size - 8)] = 0x10;
	grid[((size - 8) * size) + 7] = 0x10;
	
	/* Add alignment patterns */
	if(version != 1) {
		/* Version 1 does not have alignment patterns */
		
		loopsize = qr_align_loopsize[version - 1];
		for(x = 0; x < loopsize; x++) {
			for(y = 0; y < loopsize; y++) {
				xcoord = qr_table_e1[((version - 2) * 7) + x];
				ycoord = qr_table_e1[((version - 2) * 7) + y];
				
				if(!(grid[(ycoord * size) + xcoord] & 0x10)) {
					place_align(grid, size, xcoord, ycoord);
				}
			}
		}
	}
	
	/* Reserve space for format information */
	for(i = 0; i < 8; i++) {
		grid[(8 * size) + i] += 0x20;
		grid[(i * size) + 8] += 0x20;
		grid[(8 * size) + (size - 1 - i)] = 0x20;
		grid[((size - 1 - i) * size) + 8] = 0x20;
	}
	grid[(8 * size) + 8] += 20;
	grid[((size - 1 - 7) * size) + 8] = 0x21; /* Dark Module from Figure 25 */
	
	/* Reserve space for version information */
	if(version >= 7) {
		for(i = 0; i < 6; i++) {
			grid[((size - 9) * size) + i] = 0x20;
			grid[((size - 10) * size) + i] = 0x20;
			grid[((size - 11) * size) + i] = 0x20;
			grid[(i * size) + (size - 9)] = 0x20;
			grid[(i * size) + (size - 10)] = 0x20;
			grid[(i * size) + (size - 11)] = 0x20;
		}
	}
}

int cwbit(int* datastream, int i) {
	int word = i / 8;
	int bit = i % 8;
	int resultant = 0;
	
	switch(bit) {
		case 0: if(datastream[word] & 0x80) { resultant = 1; } else { resultant = 0; } break;
		case 1: if(datastream[word] & 0x40) { resultant = 1; } else { resultant = 0; } break;
		case 2: if(datastream[word] & 0x20) { resultant = 1; } else { resultant = 0; } break;
		case 3: if(datastream[word] & 0x10) { resultant = 1; } else { resultant = 0; } break;
		case 4: if(datastream[word] & 0x08) { resultant = 1; } else { resultant = 0; } break;
		case 5: if(datastream[word] & 0x04) { resultant = 1; } else { resultant = 0; } break;
		case 6: if(datastream[word] & 0x02) { resultant = 1; } else { resultant = 0; } break;
		case 7: if(datastream[word] & 0x01) { resultant = 1; } else { resultant = 0; } break;
	}
	
	return resultant;
}

void populate_grid(unsigned char* grid, int size, int* datastream, int cw)
{
	int direction = 1; /* up */
	int row = 0; /* right hand side */
	
	int i, n, x, y;
	
	n = cw * 8;
	y = size - 1;
	i = 0;
	do {
		x = (size - 2) - (row * 2);
		if(x < 6)
			x--; /* skip over vertical timing pattern */

		if(!(grid[(y * size) + (x + 1)] & 0xf0)) {
			if (cwbit(datastream, i)) {
				grid[(y * size) + (x + 1)] = 0x01;
			} else {
				grid[(y * size) + (x + 1)] = 0x00;
			}
			i++;
		}
		
		if(i < n) {
			if(!(grid[(y * size) + x] & 0xf0)) {
				if (cwbit(datastream, i)) {
					grid[(y * size) + x] = 0x01;
				} else {
					grid[(y * size) + x] = 0x00;
				}
				i++;
			}
		}
		
		if(direction) { y--; } else { y++; }
		if(y == -1) {
			/* reached the top */
			row++;
			y = 0;
			direction = 0;
		}
		if(y == size) {
			/* reached the bottom */
			row++;
			y = size - 1;
			direction = 1;
		}
	} while (i < n);
}

int evaluate(unsigned char *grid, int size, int pattern)
{
	int x, y, block;
	int result = 0;
	char state;
	int p;
	int dark_mods;
	int percentage, k;
	
#ifndef _MSC_VER
	char local[size * size];
#else
	char* local = (char *)_alloca((size * size) * sizeof(char));
#endif

	for(x = 0; x < size; x++) {
		for(y = 0; y < size; y++) {
			switch(pattern) {
				case 0: if (grid[(y * size) + x] & 0x01) { local[(y * size) + x] = '1'; } else { local[(y * size) + x] = '0'; } break;
				case 1: if (grid[(y * size) + x] & 0x02) { local[(y * size) + x] = '1'; } else { local[(y * size) + x] = '0'; } break;
				case 2: if (grid[(y * size) + x] & 0x04) { local[(y * size) + x] = '1'; } else { local[(y * size) + x] = '0'; } break;
				case 3: if (grid[(y * size) + x] & 0x08) { local[(y * size) + x] = '1'; } else { local[(y * size) + x] = '0'; } break;
				case 4: if (grid[(y * size) + x] & 0x10) { local[(y * size) + x] = '1'; } else { local[(y * size) + x] = '0'; } break;
				case 5: if (grid[(y * size) + x] & 0x20) { local[(y * size) + x] = '1'; } else { local[(y * size) + x] = '0'; } break;
				case 6: if (grid[(y * size) + x] & 0x40) { local[(y * size) + x] = '1'; } else { local[(y * size) + x] = '0'; } break;
				case 7: if (grid[(y * size) + x] & 0x80) { local[(y * size) + x] = '1'; } else { local[(y * size) + x] = '0'; } break;
			}
		}
	}
	
	/* Test 1: Adjacent modules in row/column in same colour */
	/* Vertical */
	for(x = 0; x < size; x++) {
		state = local[x];
		block = 0;
		for(y = 0; y < size; y++) {
			if(local[(y * size) + x] == state) {
				block++;
			} else {
				if(block > 5) {
					result += (3 + block);
				}
				block = 0;
				state = local[(y * size) + x];
			}
		}
		if(block > 5) {
			result += (3 + block);
		}
	}
	
	/* Horizontal */
	for(y = 0; y < size; y++) {
		state = local[y * size];
		block = 0;
		for(x = 0; x < size; x++) {
			if(local[(y * size) + x] == state) {
				block++;
			} else {
				if(block > 5) {
					result += (3 + block);
				}
				block = 0;
				state = local[(y * size) + x];
			}
		}
		if(block > 5) {
			result += (3 + block);
		}
	}
	
	/* Test 2 is not implimented */
	
	/* Test 3: 1:1:3:1:1 ratio pattern in row/column */
	/* Vertical */
	for(x = 0; x < size; x++) {
		for(y = 0; y < (size - 7); y++) {
			p = 0;
			if(local[(y * size) + x] == '1') { p += 0x40; }
			if(local[((y + 1) * size) + x] == '1') { p += 0x20; }
			if(local[((y + 2) * size) + x] == '1') { p += 0x10; }
			if(local[((y + 3) * size) + x] == '1') { p += 0x08; }
			if(local[((y + 4) * size) + x] == '1') { p += 0x04; }
			if(local[((y + 5) * size) + x] == '1') { p += 0x02; }
			if(local[((y + 6) * size) + x] == '1') { p += 0x01; }
			if(p == 0x5d) {
				result += 40;
			}
		}
	}
	
	/* Horizontal */
	for(y = 0; y < size; y++) {
		for(x = 0; x < (size - 7); x++) {
			p = 0;
			if(local[(y * size) + x] == '1') { p += 0x40; }
			if(local[(y * size) + x + 1] == '1') { p += 0x20; }
			if(local[(y * size) + x + 2] == '1') { p += 0x10; }
			if(local[(y * size) + x + 3] == '1') { p += 0x08; }
			if(local[(y * size) + x + 4] == '1') { p += 0x04; }
			if(local[(y * size) + x + 5] == '1') { p += 0x02; }
			if(local[(y * size) + x + 6] == '1') { p += 0x01; }
			if(p == 0x5d) {
				result += 40;
			}
		}
	}
	
	/* Test 4: Proportion of dark modules in entire symbol */
	dark_mods = 0;
	for(x = 0; x < size; x++) {
		for(y = 0; y < size; y++) {
			if(local[(y * size) + x] == '1') {
				dark_mods++;
			}
		}
	}
	percentage = 100 * (dark_mods / (size * size));
	if(percentage <= 50) {
		k = ((100 - percentage) - 50) / 5;
	} else {
		k = (percentage - 50) / 5;
	}
	
	result += 10 * k;
	
	return result;
}


int apply_bitmask(unsigned char *grid, int size)
{
	int x, y;
	unsigned char p;
	int pattern, penalty[8];
	int best_val, best_pattern;
	int bit;
	
#ifndef _MSC_VER
	unsigned char mask[size * size];
	unsigned char eval[size * size];
#else
	unsigned char* mask = (unsigned char *)_alloca((size * size) * sizeof(unsigned char));
	unsigned char* eval = (unsigned char *)_alloca((size * size) * sizeof(unsigned char));
#endif

	/* Perform data masking */
	for(x = 0; x < size; x++) {
		for(y = 0; y < size; y++) {
			mask[(y * size) + x] = 0x00;
			
			if (!(grid[(y * size) + x] & 0xf0)) {
				if(((y + x) % 2) == 0) { mask[(y * size) + x] += 0x01; }
				if((y % 2) == 0) { mask[(y * size) + x] += 0x02; }
				if((x % 3) == 0) { mask[(y * size) + x] += 0x04; }
				if(((y + x) % 3) == 0) { mask[(y * size) + x] += 0x08; }
				if((((y / 2) + (x / 3)) % 2) == 0) { mask[(y * size) + x] += 0x10; }
				if((((y * x) % 2) + ((y * x) % 3)) == 0) { mask[(y * size) + x] += 0x20; }
				if(((((y * x) % 2) + ((y * x) % 3)) % 2) == 0) { mask[(y * size) + x] += 0x40; }
				if(((((y + x) % 2) + ((y * x) % 3)) % 2) == 0) { mask[(y * size) + x] += 0x80; }
			}
		}
	}
	
	for(x = 0; x < size; x++) {
		for(y = 0; y < size; y++) {
			if(grid[(y * size) + x] & 0x01) { p = 0xff; } else { p = 0x00; }
			
			eval[(y * size) + x] = mask[(y * size) + x] ^ p;
		}
	}
	
	
	/* Evaluate result */
	for(pattern = 0; pattern < 8; pattern++) {
		penalty[pattern] = evaluate(eval, size, pattern);
	}
	
	best_pattern = 0;
	best_val = penalty[0];
	for(pattern = 1; pattern < 8; pattern++) {
		if(penalty[pattern] < best_val) {
			best_pattern = pattern;
			best_val = penalty[pattern];
		}
	}
	
	/* Apply mask */
	for(x = 0; x < size; x++) {
		for(y = 0; y < size; y++) {
			bit = 0;
			switch(best_pattern) {
				case 0: if(mask[(y * size) + x] & 0x01) { bit = 1; } break;
				case 1: if(mask[(y * size) + x] & 0x02) { bit = 1; } break;
				case 2: if(mask[(y * size) + x] & 0x04) { bit = 1; } break;
				case 3: if(mask[(y * size) + x] & 0x08) { bit = 1; } break;
				case 4: if(mask[(y * size) + x] & 0x10) { bit = 1; } break;
				case 5: if(mask[(y * size) + x] & 0x20) { bit = 1; } break;
				case 6: if(mask[(y * size) + x] & 0x40) { bit = 1; } break;
				case 7: if(mask[(y * size) + x] & 0x80) { bit = 1; } break;
			}
			if(bit == 1) {
				if(grid[(y * size) + x] & 0x01) {
					grid[(y * size) + x] = 0x00;
				} else {
					grid[(y * size) + x] = 0x01;
				}
			}
		}
	}
	
	return best_pattern;
}

void add_format_info(unsigned char *grid, int size, int ecc_level, int pattern)
{
	/* Add format information to grid */
	
	int format = pattern;
	unsigned int seq;
	int i;
	
	switch(ecc_level) {
		case LEVEL_L: format += 0x08; break;
		case LEVEL_Q: format += 0x18; break;
		case LEVEL_H: format += 0x10; break;
	}

	seq = qr_annex_c[format];
	
	for(i = 0; i < 6; i++) {
		grid[(i * size) + 8] += (seq >> i) & 0x01;
	}
	
	for(i = 0; i < 8; i++) {
		grid[(8 * size) + (size - i - 1)] += (seq >> i) & 0x01;
	}
	
	for(i = 0; i < 6; i++) {
		grid[(8 * size) + (5 - i)] += (seq >> (i + 9)) & 0x01;
	}
	
	for(i = 0; i < 7; i++) {
		grid[(((size - 7) + i) * size) + 8] += (seq >> (i + 8)) & 0x01;
	}
	
	grid[(7 * size) + 8] += (seq >> 6) & 0x01;
	grid[(8 * size) + 8] += (seq >> 7) & 0x01;
	grid[(8 * size) + 7] += (seq >> 8) & 0x01;
}

void add_version_info(unsigned char *grid, int size, int version)
{
	/* Add version information */
	int i;
	
	long int version_data = qr_annex_d[version - 7];
	for(i = 0; i < 6; i++) {
		grid[((size - 11) * size) + i] += (version_data >> (17 - (i * 3))) & 0x01;
		grid[((size - 10) * size) + i] += (version_data >> (16 - (i * 3))) & 0x01;
		grid[((size - 9) * size) + i] += (version_data >> (15 - (i * 3))) & 0x01;
		grid[(i * size) + (size - 11)] += (version_data >> (17 - (i * 3))) & 0x01;
		grid[(i * size) + (size - 10)] += (version_data >> (16 - (i * 3))) & 0x01;
		grid[(i * size) + (size - 9)] += (version_data >> (15 - (i * 3))) & 0x01;
	}
}

int qr_code(struct zint_symbol *symbol, unsigned char source[], int length)
{
	int error_number, i, j, glyph, est_binlen;
	int ecc_level, autosize, version, max_cw, target_binlen, blocks, size;
	int bitmask;
	
#ifndef _MSC_VER
	int utfdata[length + 1];
	int jisdata[length + 1];
	char mode[length + 1];
#else
	int* utfdata = (int *)_alloca((length + 1) * sizeof(int));
	int* jisdata = (int *)_alloca((length + 1) * sizeof(int));
	char* mode = (char *)_alloca(length + 1);
#endif
	
	switch(symbol->input_mode) {
		case DATA_MODE:
			for(i = 0; i < length; i++) {
				jisdata[i] = (int)source[i];
			}
			break;
		default:
			/* Convert Unicode input to Shift-JIS */
			error_number = utf8toutf16(symbol, source, utfdata, &length);
			if(error_number != 0) { return error_number; }
			
			for(i = 0; i < length; i++) {
				if(utfdata[i] <= 0xff) {
					jisdata[i] = utfdata[i];
				} else {
					j = 0;
					glyph = 0;
					do {
						if(sjis_lookup[j * 2] == utfdata[i]) {
							glyph = sjis_lookup[(j * 2) + 1];
						}
						j++;
					} while ((j < 6843) && (glyph == 0));
					if(glyph == 0) {
						strcpy(symbol->errtxt, "Invalid character in input data");
						return ERROR_INVALID_DATA;
					}
					jisdata[i] = glyph;
				}
			}
			break;
	}
	
	define_mode(mode, jisdata, length);
	est_binlen = estimate_binary_length(mode, length);
	
	ecc_level = LEVEL_L;
	max_cw = 2956;
	if((symbol->option_1 >= 1) && (symbol->option_1 <= 4)) {
		switch (symbol->option_1) {
			case 1: ecc_level = LEVEL_L; max_cw = 2956; break;
			case 2: ecc_level = LEVEL_M; max_cw = 2334; break;
			case 3: ecc_level = LEVEL_Q; max_cw = 1666; break;
			case 4: ecc_level = LEVEL_H; max_cw = 1276; break;
		}
	}
	
	if(est_binlen > (8 * max_cw)) {
		strcpy(symbol->errtxt, "Input too long for selected error correction level");
		return ERROR_TOO_LONG;
	}
	
	autosize = 40;
	for(i = 39; i >= 0; i--) {
		switch(ecc_level) {
			case LEVEL_L:
				if ((8 * qr_data_codewords_L[i]) >= est_binlen) {
					autosize = i + 1;
				}
				break;
			case LEVEL_M:
				if ((8 * qr_data_codewords_M[i]) >= est_binlen) {
					autosize = i + 1;
				}
				break;
			case LEVEL_Q:
				if ((8 * qr_data_codewords_Q[i]) >= est_binlen) {
					autosize = i + 1;
				}
				break;
			case LEVEL_H:
				if ((8 * qr_data_codewords_H[i]) >= est_binlen) {
					autosize = i + 1;
				}
				break;
		}
	}
	
	if((symbol->option_2 >= 1) && (symbol->option_2 <= 40)) {
		if (symbol->option_2 > autosize) {
			version = symbol->option_2;
		} else {
			version = autosize;
		}
	} else {
		version = autosize;
	}

	target_binlen = qr_data_codewords_L[version - 1]; blocks = qr_blocks_L[version - 1];
	switch(ecc_level) {
		case LEVEL_M: target_binlen = qr_data_codewords_M[version - 1]; blocks = qr_blocks_M[version - 1]; break;
		case LEVEL_Q: target_binlen = qr_data_codewords_Q[version - 1]; blocks = qr_blocks_Q[version - 1]; break;
		case LEVEL_H: target_binlen = qr_data_codewords_H[version - 1]; blocks = qr_blocks_H[version - 1]; break;
	}
	
#ifndef _MSC_VER
	int datastream[target_binlen + 1];
	int fullstream[qr_total_codewords[version - 1] + 1];
#else
	int* datastream = (int *)_alloca((target_binlen + 1) * sizeof(int));
	int* fullstream = (int *)_alloca((qr_total_codewords[version - 1] + 1) * sizeof(int));
#endif

	qr_binary(datastream, version, target_binlen, mode, jisdata, length);
	add_ecc(fullstream, datastream, version, target_binlen, blocks);
	
	size = qr_sizes[version - 1];
#ifndef _MSC_VER
	unsigned char grid[size * size];
#else
	unsigned char* grid = (unsigned char *)_alloca((size * size) * sizeof(unsigned char));
#endif
	
	for(i = 0; i < size; i++) {
		for(j = 0; j < size; j++) {
			grid[(i * size) + j] = 0;
		}
	}
	
	setup_grid(grid, size, version);
	populate_grid(grid, size, fullstream, qr_total_codewords[version - 1]);
	bitmask = apply_bitmask(grid, size);
	add_format_info(grid, size, ecc_level, bitmask);
	if(version >= 7) {
		add_version_info(grid, size, version);
	}
	
	symbol->width = size;
	symbol->rows = size;
	
	for(i = 0; i < size; i++) {
		for(j = 0; j < size; j++) {
			if(grid[(i * size) + j] & 0x01) {
				set_module(symbol, i, j);
			}
		}
		symbol->row_height[i] = 1;
	}
	
	return 0;
}

