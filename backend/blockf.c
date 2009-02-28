/* blockf.c - Codablock F */

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
    
    Includes bugfix thanks to rens.dol@gmail.com
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"

#define TRUE 1
#define FALSE 0
#define SHIFTA 90
#define LATCHA 91
#define SHIFTB 92
#define LATCHB 93
#define SHIFTC 94
#define LATCHC 95
#define AORB 96
#define ABORC 97
#define CANDB 98
#define CANDBB 99

#define MODEA 98
#define MODEB 100
#define MODEC 99

/* Annex A Table A.1 */
static char *C128Table[107] = {"212222", "222122", "222221", "121223", "121322", "131222", "122213",
	"122312", "132212", "221213", "221312", "231212", "112232", "122132", "122231", "113222",
	"123122", "123221", "223211", "221132", "221231", "213212", "223112", "312131", "311222",
	"321122", "321221", "312212", "322112", "322211", "212123", "212321", "232121", "111323",
	"131123", "131321", "112313", "132113", "132311", "211313", "231113", "231311", "112133",
	"112331", "132131", "113123", "113321", "133121", "313121", "211331", "231131", "213113",
	"213311", "213131", "311123", "311321", "331121", "312113", "312311", "332111", "314111",
	"221411", "431111", "111224", "111422", "121124", "121421", "141122", "141221", "112214",
	"112412", "122114", "122411", "142112", "142211", "241211", "221114", "413111", "241112",
	"134111", "111242", "121142", "121241", "114212", "124112", "124211", "411212", "421112",
	"421211", "212141", "214121", "412121", "111143", "111341", "131141", "114113", "114311",
	"411113", "411311", "113141", "114131", "311141", "411131", "211412", "211214", "211232",
	"2331112"};

int parunmodd(unsigned char llyth, char nullchar);
void grwp(int *indexliste);
void dxsmooth(int *indexliste);

int a3_convert(unsigned char source, char nullchar) {
	/* Annex A section 3 */
	if(source == nullchar) { return 64; }
	if(source < 32) { return source + 64; }
	if((source >= 32) && (source <= 127)) { return source - 32; }
	if((source >= 128) && (source <= 159)) { return (source - 128) + 64; }
	/* if source >= 160 */
	return (source - 128) - 32;
}

int character_subset_select(unsigned char source[], int input_position, char nullchar) {
	/* Section 4.5.2 - Determining the Character Subset Selector in a Row */
	if(source[input_position] == nullchar) {
		/* NULL character */
		return MODEA;
	}
	
	if((source[input_position] >= '0') && (source[input_position + 1] <= '9')) {
		/* Rule 1 */
		return MODEC;
	}
	
	if((source[input_position] >= 128) && (source[input_position] <= 160)) {
		/* Rule 2 (i) */
		return MODEA;
	}
	
	if((source[input_position] >= 0) && (source[input_position] <= 31)) {
		/* Rule 3 */
		return MODEA;
	}
	
	/* Rule 4 */
	return MODEB;
}

int data_encode_blockf(unsigned char source[], int subset_selector[], int blockmatrix[][62], int *columns_needed, int *rows_needed, int *final_mode, char nullchar, int gs1)
{
	int i, j, input_position, input_length, current_mode, current_row, error_number;
	int column_position, c, done, exit_status;
	
	error_number = 0;
	exit_status = 0;
	current_row = 0;
	current_mode = MODEA;
	input_length = ustrlen(source);
	column_position = 0;
	input_position = 0;
	done = 0;
	c = 0;
	do {
		done = 0;
		/* 'done' ensures that the instructions are followed in the correct order for each input character */
		
		if(column_position == 0) {
			/* The Beginning of a row */
			c = (*columns_needed);
			current_mode = character_subset_select(source, input_position, nullchar);
			subset_selector[current_row] = current_mode;
			if((current_row == 0) && gs1) {
				/* Section 4.4.7.1 */
				blockmatrix[current_row][column_position] = 102; /* FNC1 */
				column_position++;
				c--;
			}
		}
		
		if(gs1 && (source[input_position] == '[')) {
			blockmatrix[current_row][column_position] = 102; /* FNC1 */
			column_position++;
			c--;
			input_position++;
			done = 1;
		}
		
		if(done == 0) {
			if(c <= 2) {
				/* Annex B section 1 rule 1 */
				/* Ensure that there is sufficient encodation capacity to continue (using the rules of Annex B.2). */
				switch(current_mode) {
					case MODEA: /* Table B1 applies */
						if(parunmodd(source[input_position], nullchar) == ABORC) {
							blockmatrix[current_row][column_position] = a3_convert(source[input_position], nullchar);
							column_position++;
							c--;
							input_position++;
							done = 1;
						}
						
						if((parunmodd(source[input_position], nullchar) == SHIFTB) && (c == 1)) {
							/* Needs two symbols */
							blockmatrix[current_row][column_position] = 100; /* Code B */
							column_position++;
							c--;
							done = 1;
						}
						
						if((source[input_position] >= 244) && (done == 0)) {
							/* Needs three symbols */
							blockmatrix[current_row][column_position] = 100; /* Code B */
							column_position++;
							c--;
							if(c == 1) {
								blockmatrix[current_row][column_position] = 101; /* Code A */
								column_position++;
								c--;
							}
							done = 1;
						}
						
						if((source[input_position] >= 128) && (done == 0)) {
							/* Needs two symbols */
							if(c == 1) {
								blockmatrix[current_row][column_position] = 100; /* Code B */
								column_position++;
								c--;
								done = 1;
							}
						}
						break;
					case MODEB: /* Table B2 applies */
						if(parunmodd(source[input_position], nullchar) == ABORC) {
							blockmatrix[current_row][column_position] = a3_convert(source[input_position], nullchar);
							column_position++;
							c--;
							input_position++;
							done = 1;
						}
						
						if((parunmodd(source[input_position], nullchar) == SHIFTA) && (c == 1)) {
							/* Needs two symbols */
							blockmatrix[current_row][column_position] = 101; /* Code A */
							column_position++;
							c--;
							done = 1;
						}
						
						if(((source[input_position] >= 128) && (source[input_position] <= 159)) && (done == 0)) {
							/* Needs three symbols */
							blockmatrix[current_row][column_position] = 101; /* Code A */
							column_position++;
							c--;
							if(c == 1) {
								blockmatrix[current_row][column_position] = 100; /* Code B */
								column_position++;
								c--;
							}
							done = 1;
						}
						
						if((source[input_position] >= 160) && (done == 0)) {
							/* Needs two symbols */
							if(c == 1) {
								blockmatrix[current_row][column_position] = 101; /* Code A */
								column_position++;
								c--;
								done = 1;
							}
						}
						break;
					case MODEC: /* Table B3 applies */
						if((parunmodd(source[input_position], nullchar) != ABORC) && (c == 1)) {
							/* Needs two symbols */
							blockmatrix[current_row][column_position] = 101; /* Code A */
							column_position++;
							c--;
							done = 1;
						}
						
						if(((parunmodd(source[input_position], nullchar) == ABORC) && (parunmodd(source[input_position + 1], nullchar) != ABORC))
							&& (c == 1)) {
							/* Needs two symbols */
							blockmatrix[current_row][column_position] = 101; /* Code A */
							column_position++;
							c--;
							done = 1;
						}
						
						if(source[input_position] >= 128) {
							/* Needs three symbols */
							blockmatrix[current_row][column_position] = 101; /* Code A */
							column_position++;
							c--;
							if(c == 1) {
								blockmatrix[current_row][column_position] = 100; /* Code B */
								column_position++;
								c--;
							}
						}
						break;
				}
			}
		}
		
		if(done == 0) {
			if(((parunmodd(source[input_position], nullchar) == AORB) || (parunmodd(source[input_position], nullchar) == SHIFTA)) && (current_mode == MODEA)) {
				/* Annex B section 1 rule 2 */
				/* If in Code Subset A and the next data character can be encoded in Subset A encode the next
				character. */
				if(source[input_position] >= 128) {
					/* Extended ASCII character */
					blockmatrix[current_row][column_position] = 101; /* FNC4 */
					column_position++;
					c--;
				}
				blockmatrix[current_row][column_position] = a3_convert(source[input_position], nullchar);
				column_position++;
				c--;
				input_position++;
				done = 1;
			}
		}
		
		if(done == 0) {
			if(((parunmodd(source[input_position], nullchar) == AORB) || (parunmodd(source[input_position], nullchar) == SHIFTB)) && (current_mode == MODEB)) {
				/* Annex B section 1 rule 3 */
				/* If in Code Subset B and the next data character can be encoded in subset B, encode the next
				character. */
				if(source[input_position] >= 128) {
					/* Extended ASCII character */
					blockmatrix[current_row][column_position] = 100; /* FNC4 */
					column_position++;
					c--;
				}
				blockmatrix[current_row][column_position] = a3_convert(source[input_position], nullchar);
				column_position++;
				c--;
				input_position++;
				done = 1;
			}
		}
		
		if(done == 0) {
			if(((parunmodd(source[input_position], nullchar) == ABORC) && (parunmodd(source[input_position + 1], nullchar) == ABORC)) && (current_mode == MODEC)) {
				/* Annex B section 1 rule 4 */
				/* If in Code Subset C and the next data are 2 digits, encode them. */
				blockmatrix[current_row][column_position] = (ctoi(source[input_position]) * 10) + ctoi(source[input_position + 1]);
				column_position++;
				c--;
				input_position += 2;
				done = 1;
			}
		}
		
		if(done == 0) {
			if(((current_mode == MODEA) || (current_mode == MODEB)) && ((parunmodd(source[input_position], nullchar) == ABORC) || (gs1 && (source[input_position] == '[')))) {
				/* Count the number of numeric digits */
				/*  If 4 or more numeric data characters occur together when in subsets A or B:
				a.      If there is an even number of numeric data characters, insert a Code C character before the
				first numeric digit to change to subset C.
				b.      If there is an odd number of numeric data characters, insert a Code Set C character immedi-
				ately after the first numeric digit to change to subset C. */
				i = 0;
				j = 0;
				do {
					i++;
					if(gs1 && (source[input_position + j] == '[')) { i++; }
					j++;
				} while((parunmodd(source[input_position + j], nullchar) == ABORC) || (gs1 && (source[input_position + j] == '[')));
				i--;
				
				if(i >= 4) {
					/* Annex B section 1 rule 5 */
					if((i % 2) == 1) {
						/* Annex B section 1 rule 5a */
						blockmatrix[current_row][column_position] = 99; /* Code C */
						column_position++;
						c--;
						blockmatrix[current_row][column_position] = (ctoi(source[input_position]) * 10) + ctoi(source[input_position + 1]);
						column_position++;
						c--;
						input_position += 2;
						current_mode = MODEC;
					} else {
						/* Annex B section 1 rule 5b */
						blockmatrix[current_row][column_position] = a3_convert(source[input_position], nullchar);
						column_position++;
						c--;
						input_position++;
					}
					done = 1;
				} else {
					blockmatrix[current_row][column_position] = a3_convert(source[input_position], nullchar);
					column_position++;
					c--;
					input_position++;
					done = 1;
				}
			}
		}
		
		if(done == 0) {
			if((current_mode == MODEB) && (parunmodd(source[input_position], nullchar) == SHIFTA)) {
				/* Annex B section 1 rule 6 */
				/*  When in subset B and an ASCII control character occurs in the data:
				a.   If there is a lower case character immediately following the control character, insert a Shift
				character before the control character.
				b.   Otherwise, insert a Code A character before the control character to change to subset A. */
				if((source[input_position + 1] >= 96) && (source[input_position + 1] <= 127)) {
					/* Annex B section 1 rule 6a */
					blockmatrix[current_row][column_position] = 98; /* Shift */
					column_position++;
					c--;
					if(source[input_position] >= 128) {
						/* Extended ASCII character */
						blockmatrix[current_row][column_position] = 100; /* FNC4 */
						column_position++;
						c--;
					}
					blockmatrix[current_row][column_position] = a3_convert(source[input_position], nullchar);
					column_position++;
					c--;
					input_position++;
				} else {
					/* Annex B section 1 rule 6b */
					blockmatrix[current_row][column_position] = 101; /* Code A */
					column_position++;
					c--;
					if(source[input_position] >= 128) {
						/* Extended ASCII character */
						blockmatrix[current_row][column_position] = 100; /* FNC4 */
						column_position++;
						c--;
					}
					blockmatrix[current_row][column_position] = a3_convert(source[input_position], nullchar);
					column_position++;
					c--;
					input_position++;
					current_mode = MODEA;
				}
				done = 1;
			}
		}
		
		if(done == 0) {
			if((current_mode == MODEA) && (parunmodd(source[input_position], nullchar) == SHIFTB)) {
				/* Annex B section 1 rule 7 */
				/* When in subset A and a lower case character occurs in the data:
				a.   If following that character, a control character occurs in the data before the occurrence of
				another lower case character, insert a Shift character before the lower case character.
				b.   Otherwise, insert a Code B character before the lower case character to change to subset B. */
				if((parunmodd(source[input_position + 1], nullchar) == SHIFTA) &&
				(parunmodd(source[input_position + 2], nullchar) == SHIFTB)) {
					/* Annex B section 1 rule 7a */
					blockmatrix[current_row][column_position] = 98; /* Shift */
					column_position++;
					c--;
					if(source[input_position] >= 128) {
						/* Extended ASCII character */
						blockmatrix[current_row][column_position] = 101; /* FNC4 */
						column_position++;
						c--;
					}
					blockmatrix[current_row][column_position] = a3_convert(source[input_position], nullchar);
					column_position++;
					c--;
					input_position++;
				} else {
					/* Annex B section 1 rule 7b */
					blockmatrix[current_row][column_position] = 100; /* Code B */
					column_position++;
					c--;
					if(source[input_position] >= 128) {
						/* Extended ASCII character */
						blockmatrix[current_row][column_position] = 101; /* FNC4 */
						column_position++;
						c--;
					}
					blockmatrix[current_row][column_position] = a3_convert(source[input_position], nullchar);
					column_position++;
					c--;
					input_position++;
					current_mode = MODEB;
				}
				done = 1;
			}
		}
		
		if(done == 0) {
			if((current_mode == MODEC) && ((parunmodd(source[input_position], nullchar) != ABORC) ||
				(parunmodd(source[input_position + 1], nullchar) != ABORC))) {
				/* Annex B section 1 rule 8 */
				/*  When in subset C and a non-numeric character (or a single digit) occurs in the data, insert a Code
				A or Code B character before that character, following rules 8a and 8b to determine between code
				subsets A and B.
				a.    If an ASCII control character (eg NUL) occurs in the data before any lower case character, use
				Code A.
				b.    Otherwise use Code B. */
				if(parunmodd(source[input_position], nullchar) == SHIFTA) {
					/* Annex B section 1 rule 8a */
					blockmatrix[current_row][column_position] = 101; /* Code A */
					column_position++;
					c--;
					if(source[input_position] >= 128) {
						/* Extended ASCII character */
						blockmatrix[current_row][column_position] = 101; /* FNC4 */
						column_position++;
						c--;
					}
					blockmatrix[current_row][column_position] = a3_convert(source[input_position], nullchar);
					column_position++;
					c--;
					input_position++;
					current_mode = MODEA;
				} else {
					/* Annex B section 1 rule 8b */
					blockmatrix[current_row][column_position] = 100; /* Code B */
					column_position++;
					c--;
					if(source[input_position] >= 128) {
						/* Extended ASCII character */
						blockmatrix[current_row][column_position] = 100; /* FNC4 */
						column_position++;
						c--;
					}
					blockmatrix[current_row][column_position] = a3_convert(source[input_position], nullchar);
					column_position++;
					c--;
					input_position++;
					current_mode = MODEB;
				}
				done = 1;
			}
		}
		
		if(input_position == input_length) {
			/* End of data - Annex B rule 5a */
			if (c == 1) {
				if(current_mode == MODEA) {
					blockmatrix[current_row][column_position] = 100; /* Code B */
					current_mode = MODEB;
				} else {
					blockmatrix[current_row][column_position] = 101; /* Code A */
					current_mode = MODEA;
				}
				column_position++;
				c--;
			}
			
			if (c == 0) {
				/* Another row is needed */
				column_position = 0;
				c = (*columns_needed);
				current_row++;
				subset_selector[current_row] = MODEA;
				current_mode = MODEA;
			}
			
			if (c > 2) {
				/* Fill up the last row */
				do {
					if(current_mode == MODEA) {
						blockmatrix[current_row][column_position] = 100; /* Code B */
						current_mode = MODEB;
					} else {
						blockmatrix[current_row][column_position] = 101; /* Code A */
						current_mode = MODEA;
					}
					column_position++;
					c--;
				} while (c > 2);
			}
			
			/* If (c == 2) { do nothing } */
			
			exit_status = 1;
			*(final_mode) = current_mode;
		} else {
			if(c <= 0) {
				/* Start new row - Annex B rule 5b */
				column_position = 0;
				current_row++;
				if(current_row > 43) {
					return ERROR_TOO_LONG;
				}
			}
		}
		
	} while (exit_status == 0);
	
	if(current_row == 0) { 
		/* fill up the first row */
		for(c = column_position; c <= *(columns_needed); c++) {
			if(current_mode == MODEA) {
				blockmatrix[current_row][c] = 100; /* Code B */
				current_mode = MODEB;
			} else {
				blockmatrix[current_row][c] = 101; /* Code A */
				current_mode = MODEA;
			}
		}
		current_row++;
		/* add a second row */
		subset_selector[current_row] = MODEA;
		current_mode = MODEA;
		for(c = 0; c <= *(columns_needed) - 2; c++) {
			if(current_mode == MODEA) {
				blockmatrix[current_row][c] = 100; /* Code B */
				current_mode = MODEB;
			} else {
				blockmatrix[current_row][c] = 101; /* Code A */
				current_mode = MODEA;
			}
		}
	}
	*(rows_needed) = current_row + 1;
	
	return error_number;
}

int codablock(struct zint_symbol *symbol, unsigned char source[])
{
	int error_number, input_length, i, j, k;
	int rows_needed, columns_needed;
	int min_module_height;
	int last_mode, this_mode, final_mode;
	float estimate_codelength;
	int blockmatrix[44][62];
	char row_pattern[750];
	int subset_selector[44], row_indicator[44], row_check[44];
	long int k1_sum, k2_sum;
	int k1_check, k2_check;
	int gs1;
	
	error_number = 0;
	input_length = ustrlen(source);
	final_mode = MODEA;
	
	if(input_length > 5450) {
		strcpy(symbol->errtxt, "Input data too long [741]");
		return ERROR_TOO_LONG;
	}
	
	if(symbol->input_mode == GS1_MODE) { gs1 = 1; } else { gs1 = 0; }
	
	/* Make a guess at how many characters will be needed to encode the data */
	estimate_codelength = 0.0;
	last_mode = AORB; /* Codablock always starts with Code A */
	for(i = 0; i < input_length; i++) {
		this_mode = parunmodd(source[i], symbol->nullchar);
		if(this_mode != last_mode) {
			estimate_codelength += 1.0;
		}
		if(this_mode != ABORC) {
			estimate_codelength += 1.0;
		} else {
			estimate_codelength += 0.5;
		}
		if(source[i] > 127) {
			estimate_codelength += 1.0;
		}
		last_mode = this_mode;
	}
	
	/* Decide symbol size based on the above guess */
	rows_needed = 0.5 + sqrt((estimate_codelength + 2) / 1.45);
	if(rows_needed < 2) { rows_needed = 2; }
	if(rows_needed > 44) { rows_needed = 44; }
	columns_needed = (estimate_codelength + 2) / rows_needed;
	if(columns_needed < 4) { columns_needed = 4; }
	if(columns_needed > 62) { 
		strcpy(symbol->errtxt, "Input data too long [742]");
		return ERROR_TOO_LONG;
	}
	
	/* Encode the data */
	error_number = data_encode_blockf(source, subset_selector, blockmatrix, &columns_needed, &rows_needed, &final_mode, symbol->nullchar, gs1);
	if(error_number > 0) {
		if(error_number == ERROR_TOO_LONG) {
			strcpy(symbol->errtxt, "Input data too long [743]");
		}
		return error_number;
	}
	
	/* Add check digits - Annex F */
	k1_sum = 0;
	k2_sum = 0;
	for(i = 0; i < input_length; i++) {
		k1_sum += (i + 1) * source[i];
		k2_sum += i * source[i];
	}
	k1_check = k1_sum % 86;
	k2_check = k2_sum % 86;
	if((final_mode == MODEA) || (final_mode == MODEB)) {
		k1_check = k1_check + 64;
		if(k1_check > 95) { k1_check -= 96; }
		k2_check = k2_check + 64;
		if(k2_check > 95) { k2_check -= 96; }
	}
	blockmatrix[rows_needed - 1][columns_needed - 2] = k1_check;
	blockmatrix[rows_needed - 1][columns_needed - 1] = k2_check;
	
	/* Calculate row height (4.6.1.a) */
	min_module_height = (0.55 * (columns_needed + 3)) + 3;
	if(min_module_height < 8) { min_module_height = 8; }
	
	/* Encode the Row Indicator in the First Row of the Symbol - Table D2 */
	if(subset_selector[0] == 99) {
		/* Code C */
		row_indicator[0] = rows_needed - 2;
	} else {
		/* Code A or B */
		row_indicator[0] = rows_needed + 62;
		
		if(row_indicator[0] > 95) {
			row_indicator[0] -= 95;
		}
	}
	
	/* Encode the Row Indicator in the Second and Subsequent Rows of the Symbol - Table D3 */
	for(i = 1; i < rows_needed; i++) {
		/* Note that the second row is row number 1 because counting starts from 0 */
		if(subset_selector[i] == 99) {
			/* Code C */
			row_indicator[i] = i + 42;
		} else {
			/* Code A or B */
			if( i < 6 )
				row_indicator[i] = i + 10;
			else
				row_indicator[i] = i + 20;
		}
	}
	
	/* Calculate row check digits - Annex E */
	for(i = 0; i < rows_needed; i++) {
		k = 103;
		k += subset_selector[i];
		k += 2 * row_indicator[i];
		for(j = 0; j < columns_needed; j++) {
			k+= (j + 3) * blockmatrix[i][j];
		}
		row_check[i] = k % 103;
	}
	
	/* Resolve the data into patterns and place in symbol structure */
	for(i = 0; i < rows_needed; i++) {
		int writer, flip_flop;
		
		printf("row %d: ",i);
		printf("103 %d %d [", subset_selector[i], row_indicator[i]);
		for(j = 0; j < columns_needed; j++) {
			printf("%d ",blockmatrix[i][j]);
		}
		printf("] %d 106\n", row_check[i]);
		
		strcpy(row_pattern, "");
		/* Start character */
		concat(row_pattern, C128Table[103]); /* Always Start A */
		
		concat(row_pattern, C128Table[subset_selector[i]]);
		concat(row_pattern, C128Table[row_indicator[i]]);
		
		for(j = 0; j < columns_needed; j++) {
			concat(row_pattern, C128Table[blockmatrix[i][j]]);
		}
		
		concat(row_pattern, C128Table[row_check[i]]);
		
		/* Stop character */
		concat(row_pattern, C128Table[106]);
		
		/* Write the information into the symbol */
		writer = 0;
		flip_flop = 1;
		for (j = 0; j < strlen(row_pattern); j++) {
			for(k = 0; k < ctoi(row_pattern[j]); k++) {
				if(flip_flop == 1) {
					symbol->encoded_data[i][writer] = '1';
					writer++;
				} else {
					symbol->encoded_data[i][writer] = '0';
					writer++;
				}
			}
			if(flip_flop == 0) { flip_flop = 1; } else { flip_flop = 0; }
		}
		symbol->row_height[i] = min_module_height + 2;
	}
	
	symbol->border_width = 2;
	symbol->output_options = BARCODE_BIND;
	symbol->rows = rows_needed;
	symbol->width = (11 * (columns_needed + 5)) + 2;
	
	return error_number;
}
