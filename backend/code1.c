/* code1.c - USS Code One */

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

#include "common.h"
#include "code1.h"
#include "reedsol.h"
#include "large.h"
#include <stdio.h>
#include <string.h>
#ifdef __APPLE__
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif

void horiz(struct zint_symbol *symbol, int row_no, int full)
{
	int i = !full;
	int lim = symbol->width - i;

	while (i < lim)
		set_module(symbol, row_no, i++);
}

void central_finder(struct zint_symbol *symbol, int start_row, int row_count, int full_rows)
{
	int i;

	for(i = 0; i < row_count; i++) {
		if(i < full_rows) {
			horiz(symbol, start_row + (i * 2), 1);
		} else {
			horiz(symbol, start_row + (i * 2), 0);
			if(i != row_count - 1) {
				set_module(symbol, start_row + (i * 2) + 1, 1);
				set_module(symbol, start_row + (i * 2) + 1, symbol->width - 2);
			}
		}
	}
}

void vert(struct zint_symbol *symbol, int column, int height, int top)
{
	if(top) {
		for (int i = 0; i < height; i++) {
			set_module(symbol, i, column);
		}
	} else {
		for (int i = 0; i < height; i++) {
			set_module(symbol, symbol->rows - i - 1, column);
		}
	}
}

void spigot(struct zint_symbol *symbol, int row_no)
{
	for (int i = symbol->width - 1; i > 0; i--) {
		if(module_is_set(symbol, row_no, i - 1))
			set_module(symbol, row_no, i);
	}
}

int isedi(unsigned char input)
{
	if (input == 13 || input == '*' || input == '>' || input == ' ' ||
	    ((input >= '0') && (input <= '9')) || ((input >= 'A') && (input <= 'Z')))
		return 1;

	return 0;
}

int dq4bi(unsigned char source[], int sourcelen, int position)
{
	int i;

	for(i = position; isedi(source[position + i]) && ((position + i) < sourcelen); i++);

	if((position + i) == sourcelen) {
		/* Reached end of input */
		return 0;
	}

	if (source[position + i - 1] == 13) { return 1; }
	if (source[position + i - 1] == '*') { return 1; }
	if (source[position + i - 1] == '>') { return 1; }

	return 0;
}

int c1_look_ahead_test(unsigned char source[], int sourcelen, int position, int current_mode, int gs1)
{
	float ascii_count, c40_count, text_count, edi_count, byte_count;
	char reduced_char;
	int done, best_scheme, best_count, sp;

	/* Step J */
	if(current_mode == C1_ASCII) {
		ascii_count = 0.0;
		c40_count = 1.0;
		text_count = 1.0;
		edi_count = 1.0;
		byte_count = 2.0;
	} else {
		ascii_count = 1.0;
		c40_count = 2.0;
		text_count = 2.0;
		edi_count = 2.0;
		byte_count = 3.0;
	}

	switch(current_mode) {
		case C1_C40: c40_count = 0.0; break;
		case C1_TEXT: text_count = 0.0; break;
		case C1_BYTE: byte_count = 0.0; break;
		case C1_EDI: edi_count = 0.0; break;
	}

	for(sp = position; (sp < sourcelen) && (sp <= (position + 8)); sp++) {

		if(source[sp] <= 127) { reduced_char = source[sp]; } else { reduced_char = source[sp] - 127; }

		/* Step L */
		if((source[sp] >= '0') && (source[sp] <= '9')) {
			ascii_count += 0.5;
		} else {
			ascii_count = froundup(ascii_count);
			if(source[sp] > 127) {
				ascii_count += 2.0;
			} else {
				ascii_count += 1.0;
			}
		}

		/* Step M */
		done = 0;
		if(reduced_char == ' ') { c40_count += (2.0 / 3.0); done = 1; }
		if((reduced_char >= '0') && (reduced_char <= '9')) { c40_count += (2.0 / 3.0); done = 1; }
		if((reduced_char >= 'A') && (reduced_char <= 'Z')) { c40_count += (2.0 / 3.0); done = 1; }
		if(source[sp] > 127) { c40_count += (4.0 / 3.0); }
		if(done == 0) { c40_count += (4.0 / 3.0); }

		/* Step N */
		done = 0;
		if(reduced_char == ' ') { text_count += (2.0 / 3.0); done = 1; }
		if((reduced_char >= '0') && (reduced_char <= '9')) { text_count += (2.0 / 3.0); done = 1; }
		if((reduced_char >= 'a') && (reduced_char <= 'z')) { text_count += (2.0 / 3.0); done = 1; }
		if(source[sp] > 127) { text_count += (4.0 / 3.0); }
		if(done == 0) { text_count += (4.0 / 3.0); }

		/* Step O */
		done = 0;
		if(source[sp] == 13) { edi_count += (2.0 / 3.0); done = 1; }
		if(source[sp] == '*') { edi_count += (2.0 / 3.0); done = 1; }
		if(source[sp] == '>') { edi_count += (2.0 / 3.0); done = 1; }
		if(source[sp] == ' ') { edi_count += (2.0 / 3.0); done = 1; }
		if((source[sp] >= '0') && (source[sp] <= '9')) { edi_count += (2.0 / 3.0); done = 1; }
		if((source[sp] >= 'A') && (source[sp] <= 'Z')) { edi_count += (2.0 / 3.0); done = 1; }
		if(source[sp] > 127) {
			edi_count += (13.0 / 3.0);
		} else {
			if(done == 0) {
				edi_count += (10.0 / 3.0);
			}
		}

		/* Step P */
		if(gs1 && (source[sp] == '[')) { byte_count += 3.0; } else { byte_count += 1.0; }

	}

	ascii_count = froundup(ascii_count);
	c40_count = froundup(c40_count);
	text_count = froundup(text_count);
	edi_count = froundup(edi_count);
	byte_count = froundup(byte_count);
	best_scheme = C1_ASCII;

	if(sp == sourcelen) {
		/* Step K */
		best_count = edi_count;

		if(text_count <= best_count) {
			best_count = text_count;
			best_scheme = C1_TEXT;
		}

		if(c40_count <= best_count) {
			best_count = c40_count;
			best_scheme = C1_C40;
		}

		if(ascii_count <= best_count) {
			best_count = ascii_count;
			best_scheme = C1_ASCII;
		}

		if(byte_count <= best_count) {
			best_count = byte_count;
			best_scheme = C1_BYTE;
		}
	} else {
		/* Step Q */

		if(((edi_count + 1.0 <= ascii_count) && (edi_count + 1.0 <= c40_count)) &&
			((edi_count + 1.0 <= byte_count) && (edi_count + 1.0 <= text_count))) {
				best_scheme = C1_EDI;
		}

		if((c40_count + 1.0 <= ascii_count) && (c40_count + 1.0 <= text_count)) {

			if(c40_count < edi_count) {
				best_scheme = C1_C40;
			} else {
				done = 0;
				if(c40_count == edi_count) {
					if(dq4bi(source, sourcelen, position)) {
						best_scheme = C1_EDI;
					} else {
						best_scheme = C1_C40;
					}
				}
			}
		}

		if(((text_count + 1.0 <= ascii_count) && (text_count + 1.0 <= c40_count)) &&
			((text_count + 1.0 <= byte_count) && (text_count + 1.0 <= edi_count))) {
				best_scheme = C1_TEXT;
		}

		if(((ascii_count + 1.0 <= byte_count) && (ascii_count + 1.0 <= c40_count)) &&
			((ascii_count + 1.0 <= text_count) && (ascii_count + 1.0 <= edi_count))) {
				best_scheme = C1_ASCII;
		}

		if(((byte_count + 1.0 <= ascii_count) && (byte_count + 1.0 <= c40_count)) &&
			((byte_count + 1.0 <= text_count) && (byte_count + 1.0 <= edi_count))) {
				best_scheme = C1_BYTE;
		}
	}

	//printf("\n> scores: ASCII %.2f  C40 %.2f  TEXT %.2f  EDI %.2f  BYTE %.2f\n", ascii_count, c40_count, text_count, edi_count, byte_count);

	return best_scheme;
}

int c1_encode(struct zint_symbol *symbol, unsigned char source[], unsigned int target[], int length)
{
	int current_mode, next_mode;
	int sp, tp, gs1, latch;
	int c40_buffer[6], c40_p;
	int text_buffer[6], text_p;
	int edi_buffer[6], edi_p;
	char decimal_binary[40];
	int byte_start = 0;

	sp = 0;
	tp = 0;
	latch = 0;
	memset(c40_buffer, 0, 6);
	c40_p = 0;
	memset(text_buffer, 0, 6);
	text_p = 0;
	memset(edi_buffer, 0, 6);
	edi_p = 0;
	strcpy(decimal_binary, "");

	if(symbol->input_mode == GS1_MODE) { gs1 = 1; } else { gs1 = 0; }
	if(gs1) { target[tp] = 232; tp++; } /* FNC1 */

	/* Step A */
	current_mode = C1_ASCII;
	next_mode = C1_ASCII;

	do {
		if(current_mode != next_mode) {
			/* Change mode */
			switch(next_mode) {
				case C1_C40: target[tp] = 230; tp++; break;
				case C1_TEXT: target[tp] = 239; tp++; break;
				case C1_EDI: target[tp] = 238; tp++; break;
				case C1_BYTE: target[tp] = 231; tp++; break;
			}
		}

		if (current_mode != C1_BYTE && next_mode == C1_BYTE)
			byte_start = tp;

		current_mode = next_mode;

		if(current_mode == C1_ASCII) { /* Step B - ASCII encodation */
			next_mode = C1_ASCII;

			if (length - sp >= 21) { /* Step B1 */
				int j = 0;

				for(int i = 0; i < 21; i++) {
					if((source[sp + i] >= '0') && (source[sp + i] <= '9')) { j++; }
				}

				if (j == 21) {
					next_mode = C1_DECIMAL;
					strcpy(decimal_binary, "1111");
				}
			}

			if(next_mode == C1_ASCII && length - sp >= 13) { /* Step B2 */
				int j = 0;

				for(int i = 0; i < 13; i++) {
					if (source[sp + i] >= '0' && source[sp + i] <= '9')
						j++;
				}

				if (j == 13) {
					latch = 0;
					for (int i = sp + 13; i < length; i++) {
						if(!((source[sp + i] >= '0') && (source[sp + i] <= '9'))) { latch = 1; }
					}

					if (!latch) {
						next_mode = C1_DECIMAL;
						strcpy(decimal_binary, "1111");
					}
				}
			}

			if(next_mode == C1_ASCII) { /* Step B3 */
				if(istwodigits(source, sp) && ((sp + 1) != length)) {
					target[tp] = (10 * ctoi(source[sp])) + ctoi(source[sp + 1]) + 130;
					tp++;
					sp += 2;
				} else {
					if (gs1 && source[sp] == '[') {
						if (length - sp >= 15) { /* Step B4 */
							int j = 0;

							for(int i = 0; i < 15; i++) {
								if (source[sp + i] >= '0' && source[sp + i] <= '9')
									j++;
							}

							if (j == 15) {
								target[tp] = 236; /* FNC1 and change to Decimal */
								tp++; sp++;
								next_mode = C1_DECIMAL;
							}
						}

						if(length - sp >= 7) { /* Step B5 */
							int j = 0;

							for(int i = 0; i < 7; i++) {
								if (source[sp + i] >= '0' && source[sp + i] <= '9')
									j++;
							}

							if (j == 7) {
								latch = 0;
								for (int i = sp + 7; i < length; i++) {
									if(!(source[sp + i] >= '0' && source[sp + i] <= '9'))
										latch = 1;
								}

								if(!latch) {
									target[tp] = 236; /* FNC1 and change to Decimal */
									tp++; sp++;
									next_mode = C1_DECIMAL;
								}
							}
						}
					}

					if (next_mode == C1_ASCII) {
						/* Step B6 */
						next_mode = c1_look_ahead_test(source, length, sp, current_mode, gs1);

						if(next_mode == C1_ASCII) {
							if(source[sp] > 127) {
								/* Step B7 */
								target[tp] = 235; tp++; /* FNC4 */
								target[tp] = (source[sp] - 128) + 1; tp++; sp++;
							} else {
								/* Step B8 */
								if((gs1) && (source[sp] == '[')) {
									target[tp] = 232; tp++; sp++; /* FNC1 */
								} else {
									target[tp] = source[sp] + 1; tp++; sp++;
								}
							}
						}
					}
				}
			}
		}

		if(current_mode == C1_C40) { /* Step C - C40 encodation */
			int shift_set, value, done = 0, latch = 0;

			next_mode = C1_C40;
			if(c40_p == 0) {
				if(length - sp >= 12) {
					int j = 0;

					for(int i = 0; i < 12; i++) {
						if (source[sp + i] >= '0' && source[sp + i] <= '9')
							j++;
					}

					if (j == 12) {
						next_mode = C1_ASCII; done = 1;
					}
				}

				if(length - sp >= 8) {
					int j = 0;

					for (int i = 0; i < 8; i++) {
						if(source[sp + i] >= '0' && source[sp + i] <= '9')
							j++;
					}

					if (length - sp == 8) {
						latch = 1;
					} else {
						latch = 1;
						for(j = sp + 8; j < length; j++) {
							if (source[j] <= '0' || source[j] >= '9')
								latch = 0;
						}
					}

					if ((j == 8) && latch) {
						next_mode = C1_ASCII; done = 1;
					}
				}

				if(!(done)) {
					next_mode = c1_look_ahead_test(source, length, sp, current_mode, gs1);
				}
			}

			if(next_mode != C1_C40) {
				target[tp] = 255; tp++; /* Unlatch */
			} else {
				if(source[sp] > 127) {
					c40_buffer[c40_p] = 1; c40_p++;
					c40_buffer[c40_p] = 30; c40_p++; /* Upper Shift */
					shift_set = c40_shift[source[sp] - 128];
					value = c40_value[source[sp] - 128];
				} else {
					shift_set = c40_shift[source[sp]];
					value = c40_value[source[sp]];
				}

				if(gs1 && (source[sp] == '[')) {
					shift_set = 2;
					value = 27; /* FNC1 */
				}

				if(shift_set != 0) {
					c40_buffer[c40_p] = shift_set - 1; c40_p++;
				}
				c40_buffer[c40_p] = value; c40_p++;

				if(c40_p >= 3) {
					int iv;

					iv = (1600 * c40_buffer[0]) + (40 * c40_buffer[1]) + (c40_buffer[2]) + 1;
					target[tp] = iv / 256; tp++;
					target[tp] = iv % 256; tp++;

					c40_buffer[0] = c40_buffer[3];
					c40_buffer[1] = c40_buffer[4];
					c40_buffer[2] = c40_buffer[5];
					c40_buffer[3] = 0;
					c40_buffer[4] = 0;
					c40_buffer[5] = 0;
					c40_p -= 3;
				}
				sp++;
			}
		}

		if(current_mode == C1_TEXT) { /* Step D - Text encodation */
			int shift_set, value, done = 0, latch = 0;

			next_mode = C1_TEXT;
			if (text_p == 0) {
				if (length - sp >= 12) {
					int j = 0;

					for(int i = 0; i < 12; i++) {
						if (source[sp + i] >= '0' && source[sp + i] <= '9')
							j++;
					}

					if (j == 12) {
						next_mode = C1_ASCII; done = 1;
					}
				}

				if (length - sp >= 8) {
					int j = 0;

					for (int i = 0; i < 8; i++) {
						if (source[sp + i] >= '0' && source[sp + i] <= '9')
							j++;
					}

					if(length - sp == 8) {
						latch = 1;
					} else {
						latch = 1;
						for(j = sp + 8; j < length; j++) {
							if((source[j] <= '0') || (source[j] >= '9')) { latch = 0; }
						}
					}

					if (j == 8 && latch) {
						next_mode = C1_ASCII; done = 1;
					}
				}

				if(!(done)) {
					next_mode = c1_look_ahead_test(source, length, sp, current_mode, gs1);
				}
			}

			if(next_mode != C1_TEXT) {
				target[tp] = 255; tp++; /* Unlatch */
			} else {
				if(source[sp] > 127) {
					text_buffer[text_p] = 1; text_p++;
					text_buffer[text_p] = 30; text_p++; /* Upper Shift */
					shift_set = text_shift[source[sp] - 128];
					value = text_value[source[sp] - 128];
				} else {
					shift_set = text_shift[source[sp]];
					value = text_value[source[sp]];
				}

				if(gs1 && (source[sp] == '[')) {
					shift_set = 2;
					value = 27; /* FNC1 */
				}

				if(shift_set != 0) {
					text_buffer[text_p] = shift_set - 1; text_p++;
				}
				text_buffer[text_p] = value; text_p++;

				if(text_p >= 3) {
					int iv;

					iv = (1600 * text_buffer[0]) + (40 * text_buffer[1]) + (text_buffer[2]) + 1;
					target[tp] = iv / 256; tp++;
					target[tp] = iv % 256; tp++;

					text_buffer[0] = text_buffer[3];
					text_buffer[1] = text_buffer[4];
					text_buffer[2] = text_buffer[5];
					text_buffer[3] = 0;
					text_buffer[4] = 0;
					text_buffer[5] = 0;
					text_p -= 3;
				}
				sp++;
			}
		}

		if(current_mode == C1_EDI) { /* Step E - EDI Encodation */
			int value = 0, latch = 0;

			next_mode = C1_EDI;
			if (edi_p == 0) {
				if (length - sp >= 12) {
					int j = 0;

					for (int i = 0; i < 12; i++) {
						if (source[sp + i] >= '0' && source[sp + i] <= '9')
							j++;
					}

					if (j == 12) {
						next_mode = C1_ASCII;
					}
				}

				if (length - sp >= 8) {
					int j = 0;

					for(int i = 0; i < 8; i++) {
						if (source[sp + i] >= '0' && source[sp + i] <= '9')
							j++;
					}

					if (length - sp == 8) {
						latch = 1;
					} else {
						latch = 1;
						for (j = sp + 8; j < length; j++) {
							if (source[j] <= '0' || source[j] >= '9')
								latch = 0;
						}
					}

					if (j == 8 && latch) {
						next_mode = C1_ASCII;
					}
				}

				if(!((isedi(source[sp]) && isedi(source[sp + 1])) && isedi(source[sp + 2]))) {
					next_mode = C1_ASCII;
				}
			}

			if(next_mode != C1_EDI) {
				target[tp] = 255; tp++; /* Unlatch */
			} else {
				if(source[sp] == 13) { value = 0; }
				if(source[sp] == '*') { value = 1; }
				if(source[sp] == '>') { value = 2; }
				if(source[sp] == ' ') { value = 3; }
				if((source[sp] >= '0') && (source[sp] <= '9')) { value = source[sp] - '0' + 4; }
				if((source[sp] >= 'A') && (source[sp] <= 'Z')) { value = source[sp] - 'A' + 14; }

				edi_buffer[edi_p] = value; edi_p++;

				if(edi_p >= 3) {
					int iv;

					iv = (1600 * edi_buffer[0]) + (40 * edi_buffer[1]) + (edi_buffer[2]) + 1;
					target[tp] = iv / 256; tp++;
					target[tp] = iv % 256; tp++;

					edi_buffer[0] = edi_buffer[3];
					edi_buffer[1] = edi_buffer[4];
					edi_buffer[2] = edi_buffer[5];
					edi_buffer[3] = 0;
					edi_buffer[4] = 0;
					edi_buffer[5] = 0;
					edi_p -= 3;
				}
				sp++;
			}
		}

		if(current_mode == C1_DECIMAL) { /* Step F - Decimal encodation */
			int value, decimal_count, data_left;

			next_mode = C1_DECIMAL;

			data_left = length - sp;
			decimal_count = 0;

			if (data_left >= 1) {
				if (source[sp] >= '0' && source[sp] <= '9')
					decimal_count = 1;
			}
			if (data_left >= 2) {
				if (decimal_count == 1 && source[sp + 1] >= '0' && source[sp + 1] <= '9')
					decimal_count = 2;
			}
			if (data_left >= 3) {
				if (decimal_count == 2 && source[sp + 2] >= '0' && source[sp + 2] <= '9')
					decimal_count = 3;
			}

			if (decimal_count != 3) {
				int bits_left_in_byte, target_count;
				int sub_target;
				/* Finish Decimal mode and go back to ASCII */

				concat(decimal_binary, "111111"); /* Unlatch */

				target_count = 3;
				if (strlen(decimal_binary) <= 16)
					target_count = 2;
				if (strlen(decimal_binary) <= 8)
					target_count = 1;
				bits_left_in_byte = 8 * target_count - strlen(decimal_binary);
				if (bits_left_in_byte == 8)
					bits_left_in_byte = 0;

				if(bits_left_in_byte == 2)
					concat(decimal_binary, "01");

				if((bits_left_in_byte == 4) || (bits_left_in_byte == 6)) {
					if(decimal_count >= 1) {
						int sub_value = ctoi(source[sp]) + 1;
						bscan(decimal_binary, sub_value, 0x08);
						sp++;
					} else {
						concat(decimal_binary, "1111");
					}
				}

				if(bits_left_in_byte == 6) {
					concat(decimal_binary, "01");
				}

				/* Binary buffer is full - transfer to target */
				if(target_count >= 1) {
					sub_target = 0;
					if(decimal_binary[0] == '1') { sub_target += 128; }
					if(decimal_binary[1] == '1') { sub_target += 64; }
					if(decimal_binary[2] == '1') { sub_target += 32; }
					if(decimal_binary[3] == '1') { sub_target += 16; }
					if(decimal_binary[4] == '1') { sub_target += 8; }
					if(decimal_binary[5] == '1') { sub_target += 4; }
					if(decimal_binary[6] == '1') { sub_target += 2; }
					if(decimal_binary[7] == '1') { sub_target += 1; }
					target[tp] = sub_target; tp++;
				}
				if(target_count >= 2) {
					sub_target = 0;
					if(decimal_binary[8] == '1') { sub_target += 128; }
					if(decimal_binary[9] == '1') { sub_target += 64; }
					if(decimal_binary[10] == '1') { sub_target += 32; }
					if(decimal_binary[11] == '1') { sub_target += 16; }
					if(decimal_binary[12] == '1') { sub_target += 8; }
					if(decimal_binary[13] == '1') { sub_target += 4; }
					if(decimal_binary[14] == '1') { sub_target += 2; }
					if(decimal_binary[15] == '1') { sub_target += 1; }
					target[tp] = sub_target; tp++;
				}
				if(target_count == 3) {
					sub_target = 0;
					if(decimal_binary[16] == '1') { sub_target += 128; }
					if(decimal_binary[17] == '1') { sub_target += 64; }
					if(decimal_binary[18] == '1') { sub_target += 32; }
					if(decimal_binary[19] == '1') { sub_target += 16; }
					if(decimal_binary[20] == '1') { sub_target += 8; }
					if(decimal_binary[21] == '1') { sub_target += 4; }
					if(decimal_binary[22] == '1') { sub_target += 2; }
					if(decimal_binary[23] == '1') { sub_target += 1; }
					target[tp] = sub_target; tp++;
				}

				next_mode = C1_ASCII;
			} else {
				/* There are three digits - convert the value to binary */
				value = (100 * ctoi(source[sp])) + (10 * ctoi(source[sp + 1])) + ctoi(source[sp + 2]) + 1;

				bscan(decimal_binary, value, 0x200);
				sp+= 3;
			}

			if(strlen(decimal_binary) >= 24) {
				int target1 = 0, target2 = 0, target3 = 0;
				char temp_binary[40];

				/* Binary buffer is full - transfer to target */
				if(decimal_binary[0] == '1') { target1 += 128; }
				if(decimal_binary[1] == '1') { target1 += 64; }
				if(decimal_binary[2] == '1') { target1 += 32; }
				if(decimal_binary[3] == '1') { target1 += 16; }
				if(decimal_binary[4] == '1') { target1 += 8; }
				if(decimal_binary[5] == '1') { target1 += 4; }
				if(decimal_binary[6] == '1') { target1 += 2; }
				if(decimal_binary[7] == '1') { target1 += 1; }
				if(decimal_binary[8] == '1') { target2 += 128; }
				if(decimal_binary[9] == '1') { target2 += 64; }
				if(decimal_binary[10] == '1') { target2 += 32; }
				if(decimal_binary[11] == '1') { target2 += 16; }
				if(decimal_binary[12] == '1') { target2 += 8; }
				if(decimal_binary[13] == '1') { target2 += 4; }
				if(decimal_binary[14] == '1') { target2 += 2; }
				if(decimal_binary[15] == '1') { target2 += 1; }
				if(decimal_binary[16] == '1') { target3 += 128; }
				if(decimal_binary[17] == '1') { target3 += 64; }
				if(decimal_binary[18] == '1') { target3 += 32; }
				if(decimal_binary[19] == '1') { target3 += 16; }
				if(decimal_binary[20] == '1') { target3 += 8; }
				if(decimal_binary[21] == '1') { target3 += 4; }
				if(decimal_binary[22] == '1') { target3 += 2; }
				if(decimal_binary[23] == '1') { target3 += 1; }
				target[tp] = target1; tp++;
				target[tp] = target2; tp++;
				target[tp] = target3; tp++;

				strcpy(temp_binary, "");
				if(strlen(decimal_binary) > 24) {
					for(int i = 0; i <= (strlen(decimal_binary) - 24); i++) {
						temp_binary[i] = decimal_binary[i + 24];
					}
					strcpy(decimal_binary, temp_binary);
				}
			}
		}

		if(current_mode == C1_BYTE) {
			next_mode = C1_BYTE;

			if(gs1 && (source[sp] == '[')) {
				next_mode = C1_ASCII;
			} else {
				if(source[sp] <= 127) {
					next_mode = c1_look_ahead_test(source, length, sp, current_mode, gs1);
				}
			}

			if(next_mode != C1_BYTE) {
				/* Insert byte field length */
				if((tp - byte_start) <= 249) {
					for (int i = tp; i >= byte_start; i--) {
						target[i + 1] = target[i];
					}
					target[byte_start] = (tp - byte_start);
					tp++;
				} else {
					for (int i = tp; i >= byte_start; i--) {
						target[i + 2] = target[i];
					}
					target[byte_start] = 249 + ((tp - byte_start) / 250);
					target[byte_start + 1] = ((tp - byte_start) % 250);
					tp += 2;
				}
			} else {
				target[tp] = source[sp];
				tp++;
				sp++;
			}
		}

		if(tp > 1480) {
			/* Data is too large for symbol */
			strcpy(symbol->errtxt, "Input data too long");
			return 0;
		}
	} while (sp < length);

	/* Empty buffers */
	if(c40_p == 2) {
		int iv;

		c40_buffer[2] = 1;
		iv = (1600 * c40_buffer[0]) + (40 * c40_buffer[1]) + (c40_buffer[2]) + 1;
		target[tp] = iv / 256; tp++;
		target[tp] = iv % 256; tp++;
		target[tp] = 255; tp++; /* Unlatch */
	}
	if(c40_p == 1) {
		int iv;

		c40_buffer[1] = 1;
		c40_buffer[2] = 31; /* Pad */
		iv = (1600 * c40_buffer[0]) + (40 * c40_buffer[1]) + (c40_buffer[2]) + 1;
		target[tp] = iv / 256; tp++;
		target[tp] = iv % 256; tp++;
		target[tp] = 255; tp++; /* Unlatch */
	}
	if(text_p == 2) {
		int iv;

		text_buffer[2] = 1;
		iv = (1600 * text_buffer[0]) + (40 * text_buffer[1]) + (text_buffer[2]) + 1;
		target[tp] = iv / 256; tp++;
		target[tp] = iv % 256; tp++;
		target[tp] = 255; tp++; /* Unlatch */
	}
	if(text_p == 1) {
		int iv;

		text_buffer[1] = 1;
		text_buffer[2] = 31; /* Pad */
		iv = (1600 * text_buffer[0]) + (40 * text_buffer[1]) + (text_buffer[2]) + 1;
		target[tp] = iv / 256; tp++;
		target[tp] = iv % 256; tp++;
		target[tp] = 255; tp++; /* Unlatch */
	}

	if(current_mode == C1_DECIMAL) {
		int bits_left_in_byte, target_count;
		int sub_target;
		/* Finish Decimal mode and go back to ASCII */

		concat(decimal_binary, "111111"); /* Unlatch */

		target_count = 3;
		if(strlen(decimal_binary) <= 16) { target_count = 2; }
		if(strlen(decimal_binary) <= 8) { target_count = 1; }
		bits_left_in_byte = (8 * target_count) - strlen(decimal_binary);
		if(bits_left_in_byte == 8) { bits_left_in_byte = 0; }

		if(bits_left_in_byte == 2) {
			concat(decimal_binary, "01");
		}

		if((bits_left_in_byte == 4) || (bits_left_in_byte == 6)) {
			concat(decimal_binary, "1111");
		}

		if(bits_left_in_byte == 6) {
			concat(decimal_binary, "01");
		}

		/* Binary buffer is full - transfer to target */
		if(target_count >= 1) {
			sub_target = 0;
			if(decimal_binary[0] == '1') { sub_target += 128; }
			if(decimal_binary[1] == '1') { sub_target += 64; }
			if(decimal_binary[2] == '1') { sub_target += 32; }
			if(decimal_binary[3] == '1') { sub_target += 16; }
			if(decimal_binary[4] == '1') { sub_target += 8; }
			if(decimal_binary[5] == '1') { sub_target += 4; }
			if(decimal_binary[6] == '1') { sub_target += 2; }
			if(decimal_binary[7] == '1') { sub_target += 1; }
			target[tp] = sub_target; tp++;
		}
		if(target_count >= 2) {
			sub_target = 0;
			if(decimal_binary[8] == '1') { sub_target += 128; }
			if(decimal_binary[9] == '1') { sub_target += 64; }
			if(decimal_binary[10] == '1') { sub_target += 32; }
			if(decimal_binary[11] == '1') { sub_target += 16; }
			if(decimal_binary[12] == '1') { sub_target += 8; }
			if(decimal_binary[13] == '1') { sub_target += 4; }
			if(decimal_binary[14] == '1') { sub_target += 2; }
			if(decimal_binary[15] == '1') { sub_target += 1; }
			target[tp] = sub_target; tp++;
		}
		if(target_count == 3) {
			sub_target = 0;
			if(decimal_binary[16] == '1') { sub_target += 128; }
			if(decimal_binary[17] == '1') { sub_target += 64; }
			if(decimal_binary[18] == '1') { sub_target += 32; }
			if(decimal_binary[19] == '1') { sub_target += 16; }
			if(decimal_binary[20] == '1') { sub_target += 8; }
			if(decimal_binary[21] == '1') { sub_target += 4; }
			if(decimal_binary[22] == '1') { sub_target += 2; }
			if(decimal_binary[23] == '1') { sub_target += 1; }
			target[tp] = sub_target; tp++;
		}
	}

	if(current_mode == C1_BYTE) {
		/* Insert byte field length */
		if((tp - byte_start) <= 249) {
			for(int i = tp; i >= byte_start; i--) {
				target[i + 1] = target[i];
			}
			target[byte_start] = (tp - byte_start);
			tp++;
		} else {
			for(int i = tp; i >= byte_start; i--) {
				target[i + 2] = target[i];
			}
			target[byte_start] = 249 + ((tp - byte_start) / 250);
			target[byte_start + 1] = ((tp - byte_start) % 250);
			tp += 2;
		}
	}

	/* Re-check length of data */
	if(tp > 1480) {
		/* Data is too large for symbol */
		strcpy(symbol->errtxt, "Input data too long");
		return 0;
	}
	/*
	printf("targets:\n");
	for(i = 0; i < tp; i++) {
		printf("[%d]", target[i]);
	}
	printf("\n");
	*/
	return tp;
}

void block_copy(struct zint_symbol *symbol, char grid[][120], int start_row, int start_col, int height, int width, int row_offset, int col_offset) {
	for(int i = start_row; i < (start_row + height); i++) {
		for(int j = start_col; j < (start_col + width); j++) {
			if(grid[i][j] == '1') {
				set_module(symbol, i + row_offset, j + col_offset);
			}
		}
	}
}

int code_one(struct zint_symbol *symbol, unsigned char source[], int length)
{
	int size = 1, data_blocks;

	char datagrid[136][120];
	int sub_version = 0;

	if((symbol->option_2 < 0) || (symbol->option_2 > 10)) {
		strcpy(symbol->errtxt, "Invalid symbol size");
		return ERROR_INVALID_OPTION;
	}

	if(symbol->option_2 == 9) {
		/* Version S */
		int codewords;
		short int elreg[112];
		unsigned int data[15], ecc[15];
		int stream[30];
		int block_width;

		if(length > 18) {
			strcpy(symbol->errtxt, "Input data too long");
			return ERROR_TOO_LONG;
		}
		if(is_sane(NEON, source, length) == ERROR_INVALID_DATA) {
			strcpy(symbol->errtxt, "Invalid input data (Version S encodes numeric input only)");
			return ERROR_INVALID_DATA;
		}

		sub_version = 3; codewords = 12; block_width = 6; /* Version S-30 */
		if(length <= 12) { sub_version = 2; codewords = 8; block_width = 4; } /* Version S-20 */
		if(length <= 6) { sub_version = 1; codewords = 4; block_width = 2; } /* Version S-10 */

		binary_load(elreg, (char *)source, length);
		hex_dump(elreg);

		for(int i = 0; i < 15; i++) {
			data[i] = 0;
			ecc[i] = 0;
		}

		for(int i = 0; i < codewords; i++) {
			data[codewords - i - 1] += 1 * elreg[(i * 5)];
			data[codewords - i - 1] += 2 * elreg[(i * 5) + 1];
			data[codewords - i - 1] += 4 * elreg[(i * 5) + 2];
			data[codewords - i - 1] += 8 * elreg[(i * 5) + 3];
			data[codewords - i - 1] += 16 * elreg[(i * 5) + 4];
		}

		rs_init_gf(0x25);
		rs_init_code(codewords, 1);
		rs_encode_long(codewords, data, ecc);
		rs_free();

		for(int i = 0; i < codewords; i++) {
			stream[i] = data[i];
			stream[i + codewords] = ecc[codewords - i - 1];
		}

		for(int i = 0; i < 136; i++)
			for(int j = 0; j < 120; j++)
				datagrid[i][j] = '0';

		for(int i = 0, row = 0; row < 2; row++) {
			for(int col = 0; col < block_width; col++) {
				if(stream[i] & 0x10)
					datagrid[row * 2][col * 5] = '1';
				if(stream[i] & 0x08)
					datagrid[row * 2][(col * 5) + 1] = '1';
				if(stream[i] & 0x04)
					datagrid[row * 2][(col * 5) + 2] = '1';
				if(stream[i] & 0x02)
					datagrid[(row * 2) + 1][col * 5] = '1';
				if(stream[i] & 0x01)
					datagrid[(row * 2) + 1][(col * 5) + 1] = '1';
				if(stream[i + 1] & 0x10)
					datagrid[row * 2][(col * 5) + 3] = '1';
				if(stream[i + 1] & 0x08)
					datagrid[row * 2][(col * 5) + 4] = '1';
				if(stream[i + 1] & 0x04)
					datagrid[(row * 2) + 1][(col * 5) + 2] = '1';
				if(stream[i + 1] & 0x02)
					datagrid[(row * 2) + 1][(col * 5) + 3] = '1';
				if(stream[i + 1] & 0x01)
					datagrid[(row * 2) + 1][(col * 5) + 4] = '1';
				i += 2;
			}
		}

		size = 9;
		symbol->rows = 8;
		symbol->width = 10 * sub_version + 1;
	}

	if(symbol->option_2 == 10) {
		/* Version T */
		unsigned int data[40], ecc[25];
		unsigned int stream[65];
		int data_length;
		int data_cw, ecc_cw, block_width;

		for(int i = 0; i < 40; i++)
			data[i] = 0;

		data_length = c1_encode(symbol, source, data, length);

		if(data_length == 0) {
			return ERROR_TOO_LONG;
		}

		if(data_length > 38) {
			strcpy(symbol->errtxt, "Input data too long");
			return ERROR_TOO_LONG;
		}

		size = 10;
		sub_version = 3; data_cw = 38; ecc_cw = 22; block_width = 12;
		if(data_length <= 24) { sub_version = 2; data_cw = 24; ecc_cw = 16; block_width = 8; }
		if(data_length <= 10) { sub_version = 1; data_cw = 10; ecc_cw = 10; block_width = 4; }

		for(int i = data_length; i < data_cw; i++) {
			data[i] = 129; /* Pad */
		}

		/* Calculate error correction data */
		rs_init_gf(0x12d);
		rs_init_code(ecc_cw, 1);
		rs_encode_long(data_cw, data, ecc);
		rs_free();

		/* "Stream" combines data and error correction data */
		for(int i = 0; i < data_cw; i++)
			stream[i] = data[i];

		for(int i = 0; i < ecc_cw; i++)
			stream[data_cw + i] = ecc[ecc_cw - i - 1];

		for (int i = 0; i < 136; i++)
			for (int j = 0; j < 120; j++)
				datagrid[i][j] = '0';

		for (int i = 0, row = 0; row < 5; row++) {
			for(int col = 0; col < block_width; col++) {
				if(stream[i] & 0x80)
					datagrid[row * 2][col * 4] = '1';
				if(stream[i] & 0x40)
					datagrid[row * 2][(col * 4) + 1] = '1';
				if(stream[i] & 0x20)
					datagrid[row * 2][(col * 4) + 2] = '1';
				if(stream[i] & 0x10)
					datagrid[row * 2][(col * 4) + 3] = '1';
				if(stream[i] & 0x08)
					datagrid[(row * 2) + 1][col * 4] = '1';
				if(stream[i] & 0x04)
					datagrid[(row * 2) + 1][(col * 4) + 1] = '1';
				if(stream[i] & 0x02)
					datagrid[(row * 2) + 1][(col * 4) + 2] = '1';
				if(stream[i] & 0x01)
					datagrid[(row * 2) + 1][(col * 4) + 3] = '1';
				i++;
			}
		}

		symbol->rows = 16;
		symbol->width = (sub_version * 16) + 1;
	}

	if((symbol->option_2 != 9) && (symbol->option_2 != 10)) {
		/* Version A to H */
		unsigned int data[1500], ecc[600];
		unsigned int sub_data[190], sub_ecc[75];
		unsigned int stream[2100];
		int data_length;

		for (int i = 0; i < 1500; i++)
			data[i] = 0;

		data_length = c1_encode(symbol, source, data, length);

		if (data_length == 0) {
			return ERROR_TOO_LONG;
		}

		for (int i = 7; i >= 0; i--) {
			if(c1_data_length[i] >= data_length) {
				size = i + 1;
			}
		}

		if (symbol->option_2 > size) {
			size = symbol->option_2;
		}

		for (int i = data_length; i < c1_data_length[size - 1]; i++) {
			data[i] = 129; /* Pad */
		}

		/* Calculate error correction data */
		data_length = c1_data_length[size - 1];
		for (int i = 0; i < 190; i++)
			sub_data[i] = 0;

		for (int i = 0; i < 75; i++)
			sub_ecc[i] = 0;

		data_blocks = c1_blocks[size - 1];

		rs_init_gf(0x12d);
		rs_init_code(c1_ecc_blocks[size - 1], 0);

		for(int i = 0; i < data_blocks; i++) {
			for(int j = 0; j < c1_data_blocks[size - 1]; j++) {

				sub_data[j] = data[j * data_blocks + i];
			}
			rs_encode_long(c1_data_blocks[size - 1], sub_data, sub_ecc);
			for(int j = 0; j < c1_ecc_blocks[size - 1]; j++) {
				ecc[c1_ecc_length[size - 1] - (j * data_blocks + i) - 1] = sub_ecc[j];
			}
		}
		rs_free();

		/* "Stream" combines data and error correction data */
		for(int i = 0; i < data_length; i++)
			stream[i] = data[i];

		for(int i = 0; i < c1_ecc_length[size - 1]; i++)
			stream[data_length + i] = ecc[i];

		for(int i = 0; i < 136; i++)
			for(int j = 0; j < 120; j++)
				datagrid[i][j] = '0';

		for(int i = 0, row = 0; row < c1_grid_height[size - 1]; row++) {
			for(int col = 0; col < c1_grid_width[size - 1]; col++) {
				if(stream[i] & 0x80)
					datagrid[row * 2][col * 4] = '1';
				if(stream[i] & 0x40)
					datagrid[row * 2][(col * 4) + 1] = '1';
				if(stream[i] & 0x20)
					datagrid[row * 2][(col * 4) + 2] = '1';
				if(stream[i] & 0x10)
					datagrid[row * 2][(col * 4) + 3] = '1';
				if(stream[i] & 0x08)
					datagrid[(row * 2) + 1][col * 4] = '1';
				if(stream[i] & 0x04)
					datagrid[(row * 2) + 1][(col * 4) + 1] = '1';
				if(stream[i] & 0x02)
					datagrid[(row * 2) + 1][(col * 4) + 2] = '1';
				if(stream[i] & 0x01)
					datagrid[(row * 2) + 1][(col * 4) + 3] = '1';
				i++;
			}
		}

		symbol->rows = c1_height[size - 1];
		symbol->width = c1_width[size - 1];
	}

	switch(size) {
		case 1: /* Version A */
			central_finder(symbol, 6, 3, 1);
			vert(symbol, 4, 6, 1);
			vert(symbol, 12, 5, 0);
			set_module(symbol, 5, 12);
			spigot(symbol, 0);
			spigot(symbol, 15);
			block_copy(symbol, datagrid, 0, 0, 5, 4, 0, 0);
			block_copy(symbol, datagrid, 0, 4, 5, 12, 0, 2);
			block_copy(symbol, datagrid, 5, 0, 5, 12, 6, 0);
			block_copy(symbol, datagrid, 5, 12, 5, 4, 6, 2);
			break;
		case 2: /* Version B */
			central_finder(symbol, 8, 4, 1);
			vert(symbol, 4, 8, 1);
			vert(symbol, 16, 7, 0);
			set_module(symbol, 7, 16);
			spigot(symbol, 0);
			spigot(symbol, 21);
			block_copy(symbol, datagrid, 0, 0, 7, 4, 0, 0);
			block_copy(symbol, datagrid, 0, 4, 7, 16, 0, 2);
			block_copy(symbol, datagrid, 7, 0, 7, 16, 8, 0);
			block_copy(symbol, datagrid, 7, 16, 7, 4, 8, 2);
			break;
		case 3: /* Version C */
			central_finder(symbol, 11, 4, 2);
			vert(symbol, 4, 11, 1);
			vert(symbol, 26, 13, 1);
			vert(symbol, 4, 10, 0);
			vert(symbol, 26, 10, 0);
			spigot(symbol, 0);
			spigot(symbol, 27);
			block_copy(symbol, datagrid, 0, 0, 10, 4, 0, 0);
			block_copy(symbol, datagrid, 0, 4, 10, 20, 0, 2);
			block_copy(symbol, datagrid, 0, 24, 10, 4, 0, 4);
			block_copy(symbol, datagrid, 10, 0, 10, 4, 8, 0);
			block_copy(symbol, datagrid, 10, 4, 10, 20, 8, 2);
			block_copy(symbol, datagrid, 10, 24, 10, 4, 8, 4);
			break;
		case 4: /* Version D */
			central_finder(symbol, 16, 5, 1);
			vert(symbol, 4, 16, 1);
			vert(symbol, 20, 16, 1);
			vert(symbol, 36, 16, 1);
			vert(symbol, 4, 15, 0);
			vert(symbol, 20, 15, 0);
			vert(symbol, 36, 15, 0);
			spigot(symbol, 0);
			spigot(symbol, 12);
			spigot(symbol, 27);
			spigot(symbol, 39);
			block_copy(symbol, datagrid, 0, 0, 15, 4, 0, 0);
			block_copy(symbol, datagrid, 0, 4, 15, 14, 0, 2);
			block_copy(symbol, datagrid, 0, 18, 15, 14, 0, 4);
			block_copy(symbol, datagrid, 0, 32, 15, 4, 0, 6);
			block_copy(symbol, datagrid, 15, 0, 15, 4, 10, 0);
			block_copy(symbol, datagrid, 15, 4, 15, 14, 10, 2);
			block_copy(symbol, datagrid, 15, 18, 15, 14, 10, 4);
			block_copy(symbol, datagrid, 15, 32, 15, 4, 10, 6);
			break;
		case 5: /* Version E */
			central_finder(symbol, 22, 5, 2);
			vert(symbol, 4, 22, 1);
			vert(symbol, 26, 24, 1);
			vert(symbol, 48, 22, 1);
			vert(symbol, 4, 21, 0);
			vert(symbol, 26, 21, 0);
			vert(symbol, 48, 21, 0);
			spigot(symbol, 0);
			spigot(symbol, 12);
			spigot(symbol, 39);
			spigot(symbol, 51);
			block_copy(symbol, datagrid, 0, 0, 21, 4, 0, 0);
			block_copy(symbol, datagrid, 0, 4, 21, 20, 0, 2);
			block_copy(symbol, datagrid, 0, 24, 21, 20, 0, 4);
			block_copy(symbol, datagrid, 0, 44, 21, 4, 0, 6);
			block_copy(symbol, datagrid, 21, 0, 21, 4, 10, 0);
			block_copy(symbol, datagrid, 21, 4, 21, 20, 10, 2);
			block_copy(symbol, datagrid, 21, 24, 21, 20, 10, 4);
			block_copy(symbol, datagrid, 21, 44, 21, 4, 10, 6);
			break;
		case 6: /* Version F */
			central_finder(symbol, 31, 5, 3);
			vert(symbol, 4, 31, 1);
			vert(symbol, 26, 35, 1);
			vert(symbol, 48, 31, 1);
			vert(symbol, 70, 35, 1);
			vert(symbol, 4, 30, 0);
			vert(symbol, 26, 30, 0);
			vert(symbol, 48, 30, 0);
			vert(symbol, 70, 30, 0);
			spigot(symbol, 0);
			spigot(symbol, 12);
			spigot(symbol, 24);
			spigot(symbol, 45);
			spigot(symbol, 57);
			spigot(symbol, 69);
			block_copy(symbol, datagrid, 0, 0, 30, 4, 0, 0);
			block_copy(symbol, datagrid, 0, 4, 30, 20, 0, 2);
			block_copy(symbol, datagrid, 0, 24, 30, 20, 0, 4);
			block_copy(symbol, datagrid, 0, 44, 30, 20, 0, 6);
			block_copy(symbol, datagrid, 0, 64, 30, 4, 0, 8);
			block_copy(symbol, datagrid, 30, 0, 30, 4, 10, 0);
			block_copy(symbol, datagrid, 30, 4, 30, 20, 10, 2);
			block_copy(symbol, datagrid, 30, 24, 30, 20, 10, 4);
			block_copy(symbol, datagrid, 30, 44, 30, 20, 10, 6);
			block_copy(symbol, datagrid, 30, 64, 30, 4, 10, 8);
			break;
		case 7: /* Version G */
			central_finder(symbol, 47, 6, 2);
			vert(symbol, 6, 47, 1);
			vert(symbol, 27, 49, 1);
			vert(symbol, 48, 47, 1);
			vert(symbol, 69, 49, 1);
			vert(symbol, 90, 47, 1);
			vert(symbol, 6, 46, 0);
			vert(symbol, 27, 46, 0);
			vert(symbol, 48, 46, 0);
			vert(symbol, 69, 46, 0);
			vert(symbol, 90, 46, 0);
			spigot(symbol, 0);
			spigot(symbol, 12);
			spigot(symbol, 24);
			spigot(symbol, 36);
			spigot(symbol, 67);
			spigot(symbol, 79);
			spigot(symbol, 91);
			spigot(symbol, 103);
			block_copy(symbol, datagrid, 0, 0, 46, 6, 0, 0);
			block_copy(symbol, datagrid, 0, 6, 46, 19, 0, 2);
			block_copy(symbol, datagrid, 0, 25, 46, 19, 0, 4);
			block_copy(symbol, datagrid, 0, 44, 46, 19, 0, 6);
			block_copy(symbol, datagrid, 0, 63, 46, 19, 0, 8);
			block_copy(symbol, datagrid, 0, 82, 46, 6, 0, 10);
			block_copy(symbol, datagrid, 46, 0, 46, 6, 12, 0);
			block_copy(symbol, datagrid, 46, 6, 46, 19, 12, 2);
			block_copy(symbol, datagrid, 46, 25, 46, 19, 12, 4);
			block_copy(symbol, datagrid, 46, 44, 46, 19, 12, 6);
			block_copy(symbol, datagrid, 46, 63, 46, 19, 12, 8);
			block_copy(symbol, datagrid, 46, 82, 46, 6, 12, 10);
			break;
		case 8: /* Version H */
			central_finder(symbol, 69, 6, 3);
			vert(symbol, 6, 69, 1);
			vert(symbol, 26, 73, 1);
			vert(symbol, 46, 69, 1);
			vert(symbol, 66, 73, 1);
			vert(symbol, 86, 69, 1);
			vert(symbol, 106, 73, 1);
			vert(symbol, 126, 69, 1);
			vert(symbol, 6, 68, 0);
			vert(symbol, 26, 68, 0);
			vert(symbol, 46, 68, 0);
			vert(symbol, 66, 68, 0);
			vert(symbol, 86, 68, 0);
			vert(symbol, 106, 68, 0);
			vert(symbol, 126, 68, 0);
			spigot(symbol, 0);
			spigot(symbol, 12);
			spigot(symbol, 24);
			spigot(symbol, 36);
			spigot(symbol, 48);
			spigot(symbol, 60);
			spigot(symbol, 87);
			spigot(symbol, 99);
			spigot(symbol, 111);
			spigot(symbol, 123);
			spigot(symbol, 135);
			spigot(symbol, 147);
			block_copy(symbol, datagrid, 0, 0, 68, 6, 0, 0);
			block_copy(symbol, datagrid, 0, 6, 68, 18, 0, 2);
			block_copy(symbol, datagrid, 0, 24, 68, 18, 0, 4);
			block_copy(symbol, datagrid, 0, 42, 68, 18, 0, 6);
			block_copy(symbol, datagrid, 0, 60, 68, 18, 0, 8);
			block_copy(symbol, datagrid, 0, 78, 68, 18, 0, 10);
			block_copy(symbol, datagrid, 0, 96, 68, 18, 0, 12);
			block_copy(symbol, datagrid, 0, 114, 68, 6, 0, 14);
			block_copy(symbol, datagrid, 68, 0, 68, 6, 12, 0);
			block_copy(symbol, datagrid, 68, 6, 68, 18, 12, 2);
			block_copy(symbol, datagrid, 68, 24, 68, 18, 12, 4);
			block_copy(symbol, datagrid, 68, 42, 68, 18, 12, 6);
			block_copy(symbol, datagrid, 68, 60, 68, 18, 12, 8);
			block_copy(symbol, datagrid, 68, 78, 68, 18, 12, 10);
			block_copy(symbol, datagrid, 68, 96, 68, 18, 12, 12);
			block_copy(symbol, datagrid, 68, 114, 68, 6, 12, 14);
			break;
		case 9: /* Version S */
			horiz(symbol, 5, 1);
			horiz(symbol, 7, 1);
			set_module(symbol, 6, 0);
			set_module(symbol, 6, symbol->width - 1);
			unset_module(symbol, 7, 1);
			unset_module(symbol, 7, symbol->width - 2);
			switch(sub_version) {
				case 1: /* Version S-10 */
					set_module(symbol, 0, 5);
					block_copy(symbol, datagrid, 0, 0, 4, 5, 0, 0);
					block_copy(symbol, datagrid, 0, 5, 4, 5, 0, 1);
					break;
				case 2: /* Version S-20 */
					set_module(symbol, 0, 10);
					set_module(symbol, 4, 10);
					block_copy(symbol, datagrid, 0, 0, 4, 10, 0, 0);
					block_copy(symbol, datagrid, 0, 10, 4, 10, 0, 1);
					break;
				case 3: /* Version S-30 */
					set_module(symbol, 0, 15);
					set_module(symbol, 4, 15);
					set_module(symbol, 6, 15);
					block_copy(symbol, datagrid, 0, 0, 4, 15, 0, 0);
					block_copy(symbol, datagrid, 0, 15, 4, 15, 0, 1);
					break;
			}
			break;
		case 10: /* Version T */
			horiz(symbol, 11, 1);
			horiz(symbol, 13, 1);
			horiz(symbol, 15, 1);
			set_module(symbol, 12, 0);
			set_module(symbol, 12, symbol->width - 1);
			set_module(symbol, 14, 0);
			set_module(symbol, 14, symbol->width - 1);
			unset_module(symbol, 13, 1);
			unset_module(symbol, 13, symbol->width - 2);
			unset_module(symbol, 15, 1);
			unset_module(symbol, 15, symbol->width - 2);
			switch(sub_version) {
				case 1: /* Version T-16 */
					set_module(symbol, 0, 8);
					set_module(symbol, 10, 8);
					block_copy(symbol, datagrid, 0, 0, 10, 8, 0, 0);
					block_copy(symbol, datagrid, 0, 8, 10, 8, 0, 1);
					break;
				case 2: /* Version T-32 */
					set_module(symbol, 0, 16);
					set_module(symbol, 10, 16);
					set_module(symbol, 12, 16);
					block_copy(symbol, datagrid, 0, 0, 10, 16, 0, 0);
					block_copy(symbol, datagrid, 0, 16, 10, 16, 0, 1);
					break;
				case 3: /* Verion T-48 */
					set_module(symbol, 0, 24);
					set_module(symbol, 10, 24);
					set_module(symbol, 12, 24);
					set_module(symbol, 14, 24);
					block_copy(symbol, datagrid, 0, 0, 10, 24, 0, 0);
					block_copy(symbol, datagrid, 0, 24, 10, 24, 0, 1);
					break;
			}
			break;
	}

	for(int i = 0; i < symbol->rows; i++) {
		symbol->row_height[i] = 1;
	}

	return 0;
}
