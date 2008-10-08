/* aztec.c - Handles Aztec 2D Symbols */

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

#include <stdio.h>
#include <string.h>
#include "common.h"
#include "aztec.h"
#include "reedsol.h"

void mapshorten(int *charmap, int *typemap, int start, int length)
{ /* Shorten the string by one character */
	int i;
	
	for(i = start + 1; i < (length - 1); i++) {
		charmap[i] = charmap[i + 1];
		typemap[i] = typemap[i + 1];
	}
}

void insert(char binary_string[], int posn, char newbit)
{ /* Insert a character into the middle of a string at position posn */
	int i, end;
	
	end = strlen(binary_string);
	for(i = end; i > posn; i--) {
		binary_string[i] = binary_string[i - 1];
	}
	binary_string[posn] = newbit;
}

int aztec_text_process(unsigned char source[], char binary_string[])
{ /* Encode input data into a binary string */
	int i, j, k, bytes;
	int charmap[ustrlen(source)], typemap[ustrlen(source)], maplength;
	int curtable, newtable, lasttable, chartype;
	int blockmap[2][ustrlen(source)], blocks;


	/* Lookup input string in encoding table */
	for(i = 0; i < ustrlen(source); i++) {
		if(source[i] > 127) {
			charmap[i] = source[i];
			typemap[i] = BINARY;
		} else {
			charmap[i] = AztecSymbolChar[source[i]];
			typemap[i] = AztecCodeSet[source[i]];
		}
	}
	maplength = ustrlen(source);
	
	/* Look for double character encoding possibilities */
	i = 0;
	do{
		if(((charmap[i] == 300) && (charmap[i + 1] == 11)) && ((typemap[i] == PUNC) && (typemap[i + 1] == PUNC))) {
			/* CR LF combination */
			charmap[i] = 2;
			typemap[i] = PUNC;
			mapshorten(charmap, typemap, i, maplength);
			maplength--;
		}
		
		if(((charmap[i] == 302) && (charmap[i + 1] == 1)) && ((typemap[i] == 24) && (typemap[i + 1] == 23))) {
			/* . SP combination */
			charmap[i] = 3;
			typemap[i] = PUNC;
			mapshorten(charmap, typemap, i, maplength);
			maplength--;
		}
		
		if(((charmap[i] == 301) && (charmap[i + 1] == 1)) && ((typemap[i] == 24) && (typemap[i + 1] == 23))) {
			/* , SP combination */
			charmap[i] = 4;
			typemap[i] = PUNC;
			mapshorten(charmap, typemap, i, maplength);
			maplength--;
		}
		
		if(((charmap[i] == 21) && (charmap[i + 1] == 1)) && ((typemap[i] == PUNC) && (typemap[i + 1] == 23))) {
			/* : SP combination */
			charmap[i] = 5;
			typemap[i] = PUNC;
			mapshorten(charmap, typemap, i, maplength);
			maplength--;
		}
		
		i++;
	}while(i < (maplength - 1));
	
	/* look for blocks of characters which use the same table */
	blocks = 0;
	blockmap[0][0] = typemap[0];
	blockmap[1][0] = 1;
	for(i = 1; i < maplength; i++) {
		if(typemap[i] == typemap[i - 1]) {
			blockmap[1][blocks]++;
		} else {
			blocks++;
			blockmap[0][blocks] = typemap[i];
			blockmap[1][blocks] = 1;
		}
	}
	
	if(blockmap[0][0] & 1) { blockmap[0][0] = 1; }
	if(blockmap[0][0] & 2) { blockmap[0][0] = 2; }
	if(blockmap[0][0] & 4) { blockmap[0][0] = 4; }
	if(blockmap[0][0] & 8) { blockmap[0][0] = 8; }
	
	if(blocks > 1) {
		/* look for adjacent blocks which can use the same table (left to right search) */
		for(i = 1; i < blocks; i++) {
			if(blockmap[0][i] & blockmap[0][i - 1]) {
				blockmap[0][i] = (blockmap[0][i] & blockmap[0][i - 1]);
			}
		}
		
		if(blockmap[0][blocks - 1] & 1) { blockmap[0][blocks - 1] = 1; }
		if(blockmap[0][blocks - 1] & 2) { blockmap[0][blocks - 1] = 2; }
		if(blockmap[0][blocks - 1] & 4) { blockmap[0][blocks - 1] = 4; }
		if(blockmap[0][blocks - 1] & 8) { blockmap[0][blocks - 1] = 8; }
		
		/* look for adjacent blocks which can use the same table (right to left search) */
		for(i = blocks - 1; i > 0; i--) {
			if(blockmap[0][i] & blockmap[0][i + 1]) {
				blockmap[0][i] = (blockmap[0][i] & blockmap[0][i + 1]);
			}
		}
		
		/* determine the encoding table for characters which do not fit with adjacent blocks */
		for(i = 1; i < blocks; i++) {
			if(blockmap[0][i] & 8) { blockmap[0][i] = 8; }
			if(blockmap[0][i] & 4) { blockmap[0][i] = 4; }
			if(blockmap[0][i] & 2) { blockmap[0][i] = 2; }
			if(blockmap[0][i] & 1) { blockmap[0][i] = 1; }
		}
		
		/* Combine blocks of the same type */
		i = 0;
		do{
			if(blockmap[0][i] == blockmap[0][i + 1]) {
				blockmap[1][i] += blockmap[1][i + 1];
				for(j = i + 1; j < blocks; j++) {
					blockmap[0][j] = blockmap[0][j + 1];
					blockmap[1][j] = blockmap[1][j + 1];
				}
				blocks--;
			} else {
				i++;
			}
		} while (i < blocks);
	}
	
	/* Put the adjusted block data back into typemap */
	j = 0;
	for(i = 0; i <= blocks; i++) {
		if((blockmap[1][i] < 3) && (blockmap[0][i] != 32)) { /* Shift character(s) needed */
			for(k = 0; k < blockmap[1][i]; k++) {
				typemap[j + k] = blockmap[0][i] + 64;
			}
		} else { /* Latch character (or byte mode) needed */
			for(k = 0; k < blockmap[1][i]; k++) {
				typemap[j + k] = blockmap[0][i];
			}
		}
		j += blockmap[1][i];
	}
	
	/* Don't shift an initial capital letter */
	if(typemap[0] == 65) { typemap[0] = 1; };
	
	/* Problem characters (those that appear in different tables with different values) can now be resolved into their tables */
	for(i = 0; i < maplength; i++) {
		if(charmap[i] >= 300) {
			curtable = typemap[i];
			if(curtable > 64) {
				curtable -= 64;
			}
			switch(charmap[i]) {
				case 300: /* Carriage Return */
					switch(curtable) {
						case PUNC: charmap[i] = 1; break;
						case MIXED: charmap[i] = 14; break;
					}
					break;
				case 301: /* Comma */
					switch(curtable) {
						case PUNC: charmap[i] = 17; break;
						case DIGIT: charmap[i] = 12; break;
					}
					break;
				case 302: /* Full Stop */
					switch(curtable) {
						case PUNC: charmap[i] = 19; break;
						case DIGIT: charmap[i] = 13; break;
					}
					break;
			}
		}
	}
	
	for(i = 0; i < 20000; i++) {
		binary_string[i] = '\0';
	}
	
	curtable = UPPER; /* start with UPPER table */
	lasttable = UPPER;
	for(i = 0; i < maplength; i++) {
		newtable = curtable;
		if(typemap[i] != curtable) {
			/* Change table */
			if(curtable == BINARY) {
				/* If ending binary mode the current table is the same as when entering binary mode */
				curtable = lasttable;
				newtable = lasttable;
			}
			if(typemap[i] > 64) {
				/* Shift character */
				switch(typemap[i]) {
					case (64 + UPPER): /* To UPPER */
						switch(curtable) {
							case LOWER: /* US */
								concat(binary_string, hexbit[28]);
								break;
							case MIXED: /* UL */
								concat(binary_string, hexbit[29]);
								newtable = UPPER;
								break;
							case PUNC: /* UL */
								concat(binary_string, hexbit[31]);
								newtable = UPPER;
								break;
							case DIGIT: /* US */
								concat(binary_string, pentbit[15]);
								break;
						}
						break;
					case (64 + LOWER): /* To LOWER */
						switch(curtable) {
							case UPPER: /* LL */
								concat(binary_string, hexbit[28]);
								newtable = LOWER;
								break;
							case MIXED: /* LL */
								concat(binary_string, hexbit[28]);
								newtable = LOWER;
								break;
							case PUNC: /* UL LL */
								concat(binary_string, hexbit[31]);
								concat(binary_string, hexbit[28]);
								newtable = LOWER;
								break;
							case DIGIT: /* UL LL */
								concat(binary_string, pentbit[14]);
								concat(binary_string, hexbit[28]);
								newtable = LOWER;
								break;
						}
						break;
					case (64 + MIXED): /* To MIXED */
						switch(curtable) {
							case UPPER: /* ML */
								concat(binary_string, hexbit[29]);
								newtable = MIXED;
								break;
							case LOWER: /* ML */
								concat(binary_string, hexbit[29]);
								newtable = MIXED;
								break;
							case PUNC: /* UL ML */
								concat(binary_string, hexbit[31]);
								concat(binary_string, hexbit[29]);
								newtable = MIXED;
								break;
							case DIGIT: /* UL ML */
								concat(binary_string, pentbit[14]);
								concat(binary_string, hexbit[29]);
								newtable = MIXED;
								break;
						}
						break;
					case (64 + PUNC): /* To PUNC */
						switch(curtable) {
							case UPPER: /* PS */
								concat(binary_string, hexbit[0]);
								break;
							case LOWER: /* PS */
								concat(binary_string, hexbit[0]);
								break;
							case MIXED: /* PS */
								concat(binary_string, hexbit[0]);
								break;
							case DIGIT: /* PS */
								concat(binary_string, pentbit[0]);
								break;
						}
						break;
					case (64 + DIGIT): /* To DIGIT */
						switch(curtable) {
							case UPPER: /* DL */
								concat(binary_string, hexbit[30]);
								newtable = DIGIT;
								break;
							case LOWER: /* DL */
								concat(binary_string, hexbit[30]);
								newtable = DIGIT;
								break;
							case MIXED: /* UL DL */
								concat(binary_string, hexbit[29]);
								concat(binary_string, hexbit[30]);
								newtable = DIGIT;
								break;
							case PUNC: /* UL DL */
								concat(binary_string, hexbit[31]);
								concat(binary_string, hexbit[30]);
								newtable = DIGIT;
								break;
						}
						break;
				}
			} else {
				/* Latch character */
				switch(typemap[i]) {
					case UPPER: /* To UPPER */
						switch(curtable) {
							case LOWER: /* ML UL */
								concat(binary_string, hexbit[29]);
								concat(binary_string, hexbit[29]);
								newtable = UPPER;
								break;
							case MIXED: /* UL */
								concat(binary_string, hexbit[29]);
								newtable = UPPER;
								break;
							case PUNC: /* UL */
								concat(binary_string, hexbit[31]);
								newtable = UPPER;
								break;
							case DIGIT: /* UL */
								concat(binary_string, pentbit[14]);
								newtable = UPPER;
								break;
						}
						break;
					case LOWER: /* To LOWER */
						switch(curtable) {
							case UPPER: /* LL */
								concat(binary_string, hexbit[28]);
								newtable = LOWER;
								break;
							case MIXED: /* LL */
								concat(binary_string, hexbit[28]);
								newtable = LOWER;
								break;
							case PUNC: /* UL LL */
								concat(binary_string, hexbit[31]);
								concat(binary_string, hexbit[28]);
								newtable = LOWER;
								break;
							case DIGIT: /* UL LL */
								concat(binary_string, pentbit[14]);
								concat(binary_string, hexbit[28]);
								newtable = LOWER;
								break;
						}
						break;
					case MIXED: /* To MIXED */
						switch(curtable) {
							case UPPER: /* ML */
								concat(binary_string, hexbit[29]);
								newtable = MIXED;
								break;
							case LOWER: /* ML */
								concat(binary_string, hexbit[29]);
								newtable = MIXED;
								break;
							case PUNC: /* UL ML */
								concat(binary_string, hexbit[31]);
								concat(binary_string, hexbit[29]);
								newtable = MIXED;
								break;
							case DIGIT: /* UL ML */
								concat(binary_string, pentbit[14]);
								concat(binary_string, hexbit[29]);
								newtable = MIXED;
								break;
						}
						break;
					case PUNC: /* To PUNC */
						switch(curtable) {
							case UPPER: /* ML PL */
								concat(binary_string, hexbit[29]);
								concat(binary_string, hexbit[30]);
								newtable = PUNC;
								break;
							case LOWER: /* ML PL */
								concat(binary_string, hexbit[29]);
								concat(binary_string, hexbit[30]);
								newtable = PUNC;
								break;
							case MIXED: /* PL */
								concat(binary_string, hexbit[30]);
								newtable = PUNC;
								break;
							case DIGIT: /* UL ML PL */
								concat(binary_string, pentbit[14]);
								concat(binary_string, hexbit[29]);
								concat(binary_string, hexbit[30]);
								newtable = PUNC;
								break;
						}
						break;
					case DIGIT: /* To DIGIT */
						switch(curtable) {
							case UPPER: /* DL */
								concat(binary_string, hexbit[30]);
								newtable = DIGIT;
								break;
							case LOWER: /* DL */
								concat(binary_string, hexbit[30]);
								newtable = DIGIT;
								break;
							case MIXED: /* UL DL */
								concat(binary_string, hexbit[29]);
								concat(binary_string, hexbit[30]);
								newtable = DIGIT;
								break;
							case PUNC: /* UL DL */
								concat(binary_string, hexbit[31]);
								concat(binary_string, hexbit[30]);
								newtable = DIGIT;
								break;
						}
						break;
					case BINARY: /* To BINARY */
						lasttable = curtable;
						switch(curtable) {
							case UPPER: /* BS */
								concat(binary_string, hexbit[31]);
								newtable = BINARY;
								break;
							case LOWER: /* BS */
								concat(binary_string, hexbit[31]);
								newtable = BINARY;
								break;
							case MIXED: /* BS */
								concat(binary_string, hexbit[31]);
								newtable = BINARY;
								break;
							case PUNC: /* UL BS */
								concat(binary_string, hexbit[31]);
								concat(binary_string, hexbit[31]);
								newtable = BINARY;
								break;
							case DIGIT: /* UL BS */
								concat(binary_string, pentbit[14]);
								concat(binary_string, hexbit[31]);
								newtable = BINARY;
								break;
						}
		
						bytes = 0;
						do{
							bytes++;
						}while(typemap[i + (bytes - 1)] == BINARY);
		
						if(bytes > 2079) {
							return ERROR_TOO_LONG;
						}
		
						if(bytes > 31) { /* Put 00000 followed by 11-bit number of bytes less 31 */
							int adjusted;
			
							adjusted = bytes - 31;
							concat(binary_string, "00000");
							if(adjusted & 0x400) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(adjusted & 0x200) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(adjusted & 0x100) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(adjusted & 0x80) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(adjusted & 0x40) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(adjusted & 0x20) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(adjusted & 0x10) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(adjusted & 0x08) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(adjusted & 0x04) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(adjusted & 0x02) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(adjusted & 0x01) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
						} else { /* Put 5-bit number of bytes */
							if(bytes & 0x10) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(bytes & 0x08) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(bytes & 0x04) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(bytes & 0x02) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
							if(bytes & 0x01) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
						}
		
						break;
				}
			}
		}
		/* Add data to the binary string */
		curtable = newtable;
		chartype = typemap[i];
		if(chartype > 64) { chartype -= 64; }
		switch(chartype) {
			case UPPER:
			case LOWER:
			case MIXED:
			case PUNC:
				concat(binary_string, hexbit[charmap[i]]);
				break;
			case DIGIT:
				concat(binary_string, pentbit[charmap[i]]);
				break;
			case BINARY:
				if(charmap[i] & 0x80) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
				if(charmap[i] & 0x40) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
				if(charmap[i] & 0x20) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
				if(charmap[i] & 0x10) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
				if(charmap[i] & 0x08) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
				if(charmap[i] & 0x04) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
				if(charmap[i] & 0x02) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
				if(charmap[i] & 0x01) { concat(binary_string, "1"); } else { concat(binary_string, "0"); }
				break;
		}

	}
	
	if(strlen(binary_string) > 14970) {
		return ERROR_TOO_LONG;
	}
	
	return 0;
}

int aztec(struct zint_symbol *symbol, unsigned char source[])
{
	int x, y, i, j, k, data_blocks, ecc_blocks, layers, total_bits;
	char binary_string[20000], bit_pattern[20045], descriptor[42];
	char adjusted_string[20000];
	unsigned int data_part[1500], ecc_part[840];
	unsigned char desc_data[4], desc_ecc[6];
	int err_code, ecc_level, compact, data_length, data_maxsize, codeword_size, adjusted_length;
	
	memset(binary_string,0,20000);
	memset(adjusted_string,0,20000);

	err_code = aztec_text_process(source, binary_string);

	if(err_code != 0) {
		strcpy(symbol->errtxt, "Input too long or too many extended ASCII characters [921]");
		return err_code;
	}
	
	if(!((symbol->option_1 >= -1) && (symbol->option_1 <= 4))) {
		strcpy(symbol->errtxt, "Invalid error correction level - using default instead [922]");
		err_code = WARN_INVALID_OPTION;
		symbol->option_1 = -1;
	}
	
	ecc_level = symbol->option_1;
	
	if(ecc_level == -1) {
		ecc_level = 2;
	}
	
	data_length = strlen(binary_string);

	layers = 0; /* Keep compiler happy! */
	data_maxsize = 0; /* Keep compiler happy! */
	if(symbol->option_2 == 0) { /* The size of the symbol can be determined by Zint */
		do {
			/* Decide what size symbol to use - the smallest that fits the data */
			compact = 0; /* 1 = Aztec Compact, 0 = Normal Aztec */
			layers = 0;
			switch(ecc_level) {
				/* For each level of error correction work out the smallest symbol which
				the data will fit in */
				case 1: for(i = 32; i > 0; i--) {
						if(data_length < Aztec10DataSizes[i - 1]) {
							layers = i;
							compact = 0;
							data_maxsize = Aztec10DataSizes[i - 1];
						}
					}
					for(i = 4; i > 0; i--) {
						if(data_length < AztecCompact10DataSizes[i - 1]) {
							layers = i;
							compact = 1;
							data_maxsize = AztecCompact10DataSizes[i - 1];
						}
					}
					break;
				case 2: for(i = 32; i > 0; i--) {
						if(data_length < Aztec23DataSizes[i - 1]) {
							layers = i;
							compact = 0;
							data_maxsize = Aztec23DataSizes[i - 1];
						}
					}
					for(i = 4; i > 0; i--) {
						if(data_length < AztecCompact23DataSizes[i - 1]) {
							layers = i;
							compact = 1;
							data_maxsize = AztecCompact23DataSizes[i - 1];
						}
					}
					break;
				case 3: for(i = 32; i > 0; i--) {
						if(data_length < Aztec36DataSizes[i - 1]) {
							layers = i;
							compact = 0;
							data_maxsize = Aztec36DataSizes[i - 1];
						}
					}
					for(i = 4; i > 0; i--) {
						if(data_length < AztecCompact36DataSizes[i - 1]) {
							layers = i;
							compact = 1;
							data_maxsize = AztecCompact36DataSizes[i - 1];
						}
					}
					break;
				case 4: for(i = 32; i > 0; i--) {
						if(data_length < Aztec50DataSizes[i - 1]) {
							layers = i;
							compact = 0;
							data_maxsize = Aztec50DataSizes[i - 1];
						}
					}
					for(i = 4; i > 0; i--) {
						if(data_length < AztecCompact50DataSizes[i - 1]) {
							layers = i;
							compact = 1;
							data_maxsize = AztecCompact50DataSizes[i - 1];
						}
					}
					break;
			}
		
			if(layers == 0) { /* Couldn't find a symbol which fits the data */
				strcpy(symbol->errtxt, "Input too long (too many bits for selected ECC) [923]");
				return ERROR_TOO_LONG;
			}
			
			/* Determine codeword bitlength - Table 3 */
			codeword_size = 6; /* if (layers <= 2) */
			if((layers >= 3) && (layers <= 8)) { codeword_size = 8; }
			if((layers >= 9) && (layers <= 22)) { codeword_size = 10; }
			if(layers >= 23) { codeword_size = 12; }
		
			for(i = 0; i <= data_length; i++) {
				adjusted_string[i] = binary_string[i];
			}
			
			/* Data string can't have all '0's or all '1's in a block */
			i = 0;
			do{
				int count0, count1;
			
				count0 = 0;
				count1 = 0;
				for(j = 0; j < codeword_size; j++) {
					if(adjusted_string[j + i] == '0') { count0++; } else { count1++; }
				}
				if(count0 == codeword_size) { /* If all 0s insert a '1' */
					insert(adjusted_string, i+(codeword_size - 1), '1');
				}
				if(count1 == codeword_size) { /* If all 1s insert a '0' */
					insert(adjusted_string, i+(codeword_size - 1), '0');
				}
				i += codeword_size;
			} while ((i + codeword_size) < strlen(adjusted_string));
			adjusted_length = strlen(adjusted_string);
			
		} while(adjusted_length > data_maxsize);
		/* This loop will only repeat on the rare occasions when the rule about not having all 1s or all 0s
		means that the binary string has had to be lengthened beyond the maximum number of bits that can
		be encoded in a symbol of the selected size */

	} else { /* The size of the symbol has been specified by the user */
		
		if((symbol->option_2 >= 1) && (symbol->option_2 <= 4)) {
			compact = 1;
			layers = symbol->option_2;
		}
		if((symbol->option_2 >= 5) && (symbol->option_2 <= 36)) {
			compact = 0;
			layers = symbol->option_2 - 4;
		}
		if((symbol->option_2 < 0) || (symbol->option_2 > 36)) {
			strcpy(symbol->errtxt, "Invalid Aztec Code size [924]");
			return ERROR_INVALID_OPTION;
		}

		/* Determine codeword bitlength - Table 3 */
		if((layers >= 0) && (layers <= 2)) { codeword_size = 6; }
		if((layers >= 3) && (layers <= 8)) { codeword_size = 8; }
		if((layers >= 9) && (layers <= 22)) { codeword_size = 10; }
		if(layers >= 23) { codeword_size = 12; }
		
		for(i = 0; i <= data_length; i++) {
			adjusted_string[i] = binary_string[i];
		}
		
		/* Data string can't have all '0's or all '1's in a block */
		i = 0;
		do{
			int count0, count1;
			
			count0 = 0;
			count1 = 0;
			for(j = 0; j < codeword_size; j++) {
				if(adjusted_string[j + i] == '0') { count0++; } else { count1++; }
			}
			if(count0 == codeword_size) { /* If all 0s insert a '1' */
				insert(adjusted_string, i+(codeword_size - 1), '1');
			}
			if(count1 == codeword_size) { /* If all 1s insert a '0' */
				insert(adjusted_string, i+(codeword_size - 1), '0');
			}
			i += codeword_size;
		} while ((i + codeword_size) < strlen(adjusted_string));
		adjusted_length = strlen(adjusted_string);
		
		/* Check if the data actually fits into the selected symbol size */
		if (compact) {
			data_maxsize = codeword_size * (AztecCompactSizes[layers - 1] - 3);
		} else {
			data_maxsize = codeword_size * (AztecSizes[layers - 1] - 3);
		}

		if(adjusted_length > data_maxsize) {
			strcpy(symbol->errtxt, "Data too long for specified Aztec Code symbol size [925]");
			return ERROR_TOO_LONG;
		}
	}
	
	/* The value of i is used from above to indicate the number of bits to the end of the last
	used data block - this is now a set value */

	/* Insert extra '1's to pad out the final block of data */
	for(j = 0; j < (i - adjusted_length); j++) {
		concat(adjusted_string, "1");
	}
	adjusted_length = i;
	
	/* But be careful that the final block of data doesn't contain all '1's */
	k = 0;
	for(j = (i - codeword_size); j < i; j++) {
		if(adjusted_string[j] == '1') { k++; }
	}
	if(k == codeword_size) { /* Change the very last bit */
		adjusted_string[adjusted_length - 1] = '0';
	}
	
	data_blocks = adjusted_length / codeword_size;
	
	if(compact) {
		ecc_blocks = AztecCompactSizes[layers - 1] - data_blocks;
	} else {
		ecc_blocks = AztecSizes[layers - 1] - data_blocks;
	}
	
	/* Copy across data into separate integers */
	memset(data_part,0,1500*sizeof(int));
	memset(ecc_part,0,840*sizeof(int));
	
	/* Split into codewords and calculate reed-colomon error correction codes */
	switch(codeword_size) {
		case 6:
			for(i = 0; i < data_blocks; i++) {
				if(adjusted_string[i * codeword_size] == '1') { data_part[i] += 32; }
				if(adjusted_string[(i * codeword_size) + 1] == '1') { data_part[i] += 16; }
				if(adjusted_string[(i * codeword_size) + 2] == '1') { data_part[i] += 8; }
				if(adjusted_string[(i * codeword_size) + 3] == '1') { data_part[i] += 4; }
				if(adjusted_string[(i * codeword_size) + 4] == '1') { data_part[i] += 2; }
				if(adjusted_string[(i * codeword_size) + 5] == '1') { data_part[i] += 1; }
			}
			rs_init_gf(0x43);
			rs_init_code(ecc_blocks, 1);
			rs_encode_long(data_blocks, data_part, ecc_part);
			for(i = (ecc_blocks - 1); i >= 0; i--) {
				if(ecc_part[i] & 0x20) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x10) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x08) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x04) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x02) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x01) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
			}
			break;
		case 8:
			for(i = 0; i < data_blocks; i++) {
				if(adjusted_string[i * codeword_size] == '1') { data_part[i] += 128; }
				if(adjusted_string[(i * codeword_size) + 1] == '1') { data_part[i] += 64; }
				if(adjusted_string[(i * codeword_size) + 2] == '1') { data_part[i] += 32; }
				if(adjusted_string[(i * codeword_size) + 3] == '1') { data_part[i] += 16; }
				if(adjusted_string[(i * codeword_size) + 4] == '1') { data_part[i] += 8; }
				if(adjusted_string[(i * codeword_size) + 5] == '1') { data_part[i] += 4; }
				if(adjusted_string[(i * codeword_size) + 6] == '1') { data_part[i] += 2; }
				if(adjusted_string[(i * codeword_size) + 7] == '1') { data_part[i] += 1; }
			}
			rs_init_gf(0x12d);
			rs_init_code(ecc_blocks, 1);
			rs_encode_long(data_blocks, data_part, ecc_part);
			for(i = (ecc_blocks - 1); i >= 0; i--) {
				if(ecc_part[i] & 0x80) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x40) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x20) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x10) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x08) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x04) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x02) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x01) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
			}
			break;
		case 10:
			for(i = 0; i < data_blocks; i++) {
				if(adjusted_string[i * codeword_size] == '1') { data_part[i] += 512; }
				if(adjusted_string[(i * codeword_size) + 1] == '1') { data_part[i] += 256; }
				if(adjusted_string[(i * codeword_size) + 2] == '1') { data_part[i] += 128; }
				if(adjusted_string[(i * codeword_size) + 3] == '1') { data_part[i] += 64; }
				if(adjusted_string[(i * codeword_size) + 4] == '1') { data_part[i] += 32; }
				if(adjusted_string[(i * codeword_size) + 5] == '1') { data_part[i] += 16; }
				if(adjusted_string[(i * codeword_size) + 6] == '1') { data_part[i] += 8; }
				if(adjusted_string[(i * codeword_size) + 7] == '1') { data_part[i] += 4; }
				if(adjusted_string[(i * codeword_size) + 8] == '1') { data_part[i] += 2; }
				if(adjusted_string[(i * codeword_size) + 9] == '1') { data_part[i] += 1; }
			}
			rs_init_gf(0x409);
			rs_init_code(ecc_blocks, 1);
			rs_encode_long(data_blocks, data_part, ecc_part);
			for(i = (ecc_blocks - 1); i >= 0; i--) {
				if(ecc_part[i] & 0x200) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x100) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x80) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x40) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x20) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x10) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x08) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x04) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x02) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x01) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
			}
			break;
		case 12:
			for(i = 0; i < data_blocks; i++) {
				if(adjusted_string[i * codeword_size] == '1') { data_part[i] += 2048; }
				if(adjusted_string[(i * codeword_size) + 1] == '1') { data_part[i] += 1024; }
				if(adjusted_string[(i * codeword_size) + 2] == '1') { data_part[i] += 512; }
				if(adjusted_string[(i * codeword_size) + 3] == '1') { data_part[i] += 256; }
				if(adjusted_string[(i * codeword_size) + 4] == '1') { data_part[i] += 128; }
				if(adjusted_string[(i * codeword_size) + 5] == '1') { data_part[i] += 64; }
				if(adjusted_string[(i * codeword_size) + 6] == '1') { data_part[i] += 32; }
				if(adjusted_string[(i * codeword_size) + 7] == '1') { data_part[i] += 16; }
				if(adjusted_string[(i * codeword_size) + 8] == '1') { data_part[i] += 8; }
				if(adjusted_string[(i * codeword_size) + 9] == '1') { data_part[i] += 4; }
				if(adjusted_string[(i * codeword_size) + 10] == '1') { data_part[i] += 2; }
				if(adjusted_string[(i * codeword_size) + 11] == '1') { data_part[i] += 1; }
			}
			rs_init_gf(0x1069);
			rs_init_code(ecc_blocks, 1);
			rs_encode_long(data_blocks, data_part, ecc_part);
			for(i = (ecc_blocks - 1); i >= 0; i--) {
				if(ecc_part[i] & 0x800) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x400) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x200) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x100) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x80) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x40) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x20) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x10) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x08) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x04) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x02) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
				if(ecc_part[i] & 0x01) { concat(adjusted_string, "1"); } else { concat(adjusted_string, "0"); }
			}
			break;
	}
	
	/* Invert the data so that actual data is on the outside and reed-solomon on the inside */
	memset(bit_pattern,'0',20045);
	
	total_bits = (data_blocks + ecc_blocks) * codeword_size;
	for(i = 0; i < total_bits; i++) {
		bit_pattern[i] = adjusted_string[total_bits - i - 1];
	}
	
	/* Now add the symbol descriptor */
	memset(descriptor,0,42);
	memset(desc_data,0,4);
	memset(desc_ecc,0,6);

	if(compact) {
		/* The first 2 bits represent the number of layers minus 1 */
		if((layers - 1) & 0x02) { descriptor[0] = '1'; }
		if((layers - 1) & 0x01) { descriptor[1] = '1'; }
		/* The next 6 bits represent the number of data blocks minus 1 */
		if((data_blocks - 1) & 0x20) { descriptor[2] = '1'; }
		if((data_blocks - 1) & 0x10) { descriptor[3] = '1'; }
		if((data_blocks - 1) & 0x08) { descriptor[4] = '1'; }
		if((data_blocks - 1) & 0x04) { descriptor[5] = '1'; }
		if((data_blocks - 1) & 0x02) { descriptor[6] = '1'; }
		if((data_blocks - 1) & 0x01) { descriptor[7] = '1'; }
	} else {
		/* The first 5 bits represent the number of layers minus 1 */
		if((layers - 1) & 0x10) { descriptor[0] = '1'; }
		if((layers - 1) & 0x08) { descriptor[1] = '1'; }
		if((layers - 1) & 0x04) { descriptor[2] = '1'; }
		if((layers - 1) & 0x02) { descriptor[3] = '1'; }
		if((layers - 1) & 0x01) { descriptor[4] = '1'; }
		/* The next 11 bits represent the number of data blocks minus 1 */
		if((data_blocks - 1) & 0x400) { descriptor[5] = '1'; }
		if((data_blocks - 1) & 0x200) { descriptor[6] = '1'; }
		if((data_blocks - 1) & 0x100) { descriptor[7] = '1'; }
		if((data_blocks - 1) & 0x80) { descriptor[8] = '1'; }
		if((data_blocks - 1) & 0x40) { descriptor[9] = '1'; }
		if((data_blocks - 1) & 0x20) { descriptor[10] = '1'; }
		if((data_blocks - 1) & 0x10) { descriptor[11] = '1'; }
		if((data_blocks - 1) & 0x08) { descriptor[12] = '1'; }
		if((data_blocks - 1) & 0x04) { descriptor[13] = '1'; }
		if((data_blocks - 1) & 0x02) { descriptor[14] = '1'; }
		if((data_blocks - 1) & 0x01) { descriptor[15] = '1'; }
	}
	
	/* Split into 4-bit codewords */
	for(i = 0; i < 4; i++) {
		if(descriptor[i * 4] == '1') { desc_data[i] += 8; }
		if(descriptor[(i * 4) + 1] == '1') { desc_data[i] += 4; }
		if(descriptor[(i * 4) + 2] == '1') { desc_data[i] += 2; }
		if(descriptor[(i * 4) + 3] == '1') { desc_data[i] += 1; }
	}
	
	/* Add reed-solomon error correction with Galois field GF(16) and prime modulus
	x^4 + x + 1 (section 7.2.3)*/

	rs_init_gf(0x13);
	if(compact) {
		rs_init_code(5, 1);
		rs_encode(2, desc_data, desc_ecc);
		for(i = 0; i < 5; i++) {
			if(desc_ecc[i] & 0x08) { descriptor[(i * 4) + 8] = '1'; }
			if(desc_ecc[i] & 0x04) { descriptor[(i * 4) + 9] = '1'; }
			if(desc_ecc[i] & 0x02) { descriptor[(i * 4) + 10] = '1'; }
			if(desc_ecc[i] & 0x01) { descriptor[(i * 4) + 11] = '1'; }
		}
	} else {
		rs_init_code(6, 1);
		rs_encode(4, desc_data, desc_ecc);
		for(i = 0; i < 6; i++) {
			if(desc_ecc[i] & 0x08) { descriptor[(i * 4) + 16] = '1'; }
			if(desc_ecc[i] & 0x04) { descriptor[(i * 4) + 17] = '1'; }
			if(desc_ecc[i] & 0x02) { descriptor[(i * 4) + 18] = '1'; }
			if(desc_ecc[i] & 0x01) { descriptor[(i * 4) + 19] = '1'; }
		}
	}
	
	/* Merge descriptor with the rest of the symbol */
	for(i = 0; i < 40; i++) {
		if(compact) {
			bit_pattern[2000 + i] = descriptor[i];
		} else {
			bit_pattern[20000 + i] = descriptor[i];
		}
	}

	/* Plot all of the data into the symbol in pre-defined spiral pattern */
	if(compact) {
		for(y = AztecCompactOffset[layers - 1]; y < (28 - AztecCompactOffset[layers - 1]); y++) {
			for(x = AztecCompactOffset[layers - 1]; x < (28 - AztecCompactOffset[layers - 1]); x++) {
				if(CompactAztecMap[(y * 27) + x] == 1) {
					symbol->encoded_data[y - AztecCompactOffset[layers - 1]][x - AztecCompactOffset[layers - 1]] = '1';
				}
				if(CompactAztecMap[(y * 27) + x] >= 2) {
					if(bit_pattern[CompactAztecMap[(y * 27) + x] - 2] == '1') {
						symbol->encoded_data[y - AztecCompactOffset[layers - 1]][x - AztecCompactOffset[layers - 1]] = '1';
					}
				}
			}
			symbol->row_height[y - AztecCompactOffset[layers - 1]] = 1;
		}
		symbol->rows = 27 - (2 * AztecCompactOffset[layers - 1]);
		symbol->width = 27 - (2 * AztecCompactOffset[layers - 1]);
	} else {
		for(y = AztecOffset[layers - 1]; y < (152 - AztecOffset[layers - 1]); y++) {
			for(x = AztecOffset[layers - 1]; x < (152 - AztecOffset[layers - 1]); x++) {
				if(AztecMap[(y * 151) + x] == 1) {
					symbol->encoded_data[y - AztecOffset[layers - 1]][x - AztecOffset[layers - 1]] = '1';
				}
				if(AztecMap[(y * 151) + x] >= 2) {
					if(bit_pattern[AztecMap[(y * 151) + x] - 2] == '1') {
						symbol->encoded_data[y - AztecOffset[layers - 1]][x - AztecOffset[layers - 1]] = '1';
					}
				}
			}
			symbol->row_height[y - AztecOffset[layers - 1]] = 1;
		}
		symbol->rows = 151 - (2 * AztecOffset[layers - 1]);
		symbol->width = 151 - (2 * AztecOffset[layers - 1]);
	}
	
	return err_code;
}
