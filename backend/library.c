/*  library.c - external functions of libzint

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
#include <stdlib.h>
#include <string.h>
#include "common.h"

struct zint_symbol *ZBarcode_Create()
{
	struct zint_symbol *symbol;
	int i, j;
	
	symbol = malloc(sizeof(*symbol));
	if (!symbol) return NULL;

	memset(symbol, 0, sizeof(*symbol));
	symbol->symbology = BARCODE_CODE128;
	symbol->height = 0;
	symbol->whitespace_width = 0;
	symbol->border_width = 0;
	symbol->output_options = 0;
	symbol->rows = 0;
	symbol->width = 0;
	strcpy(symbol->fgcolour, "000000");
	strcpy(symbol->bgcolour, "ffffff");
	strcpy(symbol->outfile, "out.png");
	symbol->scale = 1.0;
	symbol->option_1 = -1;
	symbol->option_2 = 0;
	symbol->option_3 = 928; // PDF_MAX
	strcpy(symbol->primary, "");
	for(i = 0; i < 178; i++) {
		for(j = 0; j < 1000; j++) {
			symbol->encoded_data[i][j] = '0';
		}
		symbol->row_height[i] = 0;
	}
	return symbol;
}


int ZBarcode_Delete(struct zint_symbol *symbol)
{
	free(symbol);
	return 0;
}

extern int eanx(struct zint_symbol *symbol, unsigned char source[]); /* EAN system barcodes */
extern int c39(struct zint_symbol *symbol, unsigned char source[]); /* Code 3 from 9 (or Code 39) */
extern int pharmazentral(struct zint_symbol *symbol, unsigned char source[]); /* Pharmazentral Nummer (PZN) */
extern int ec39(struct zint_symbol *symbol, unsigned char source[]); /* Extended Code 3 from 9 (or Code 39+) */
extern int codabar(struct zint_symbol *symbol, unsigned char source[]); /* Codabar - a simple substitution cipher */
extern int matrix_two_of_five(struct zint_symbol *symbol, unsigned char source[]); /* Code 2 of 5 Standard (& Matrix) */
extern int industrial_two_of_five(struct zint_symbol *symbol, unsigned char source[]); /* Code 2 of 5 Industrial */
extern int iata_two_of_five(struct zint_symbol *symbol, unsigned char source[]); /* Code 2 of 5 IATA */
extern int interleaved_two_of_five(struct zint_symbol *symbol, unsigned char source[]); /* Code 2 of 5 Interleaved */
extern int logic_two_of_five(struct zint_symbol *symbol, unsigned char source[]); /* Code 2 of 5 Data Logic */
extern int itf14(struct zint_symbol *symbol, unsigned char source[]); /* ITF-14 */
extern int dpleit(struct zint_symbol *symbol, unsigned char source[]); /* Deutsche Post Leitcode */
extern int dpident(struct zint_symbol *symbol, unsigned char source[]); /* Deutsche Post Identcode */
extern int c93(struct zint_symbol *symbol, unsigned char source[]); /* Code 93 - a re-working of Code 39+, generates 2 check digits */
extern int code_128(struct zint_symbol *symbol, unsigned char source[]); /* Code 128 and NVE-18 */
extern int ean_128(struct zint_symbol *symbol, unsigned char source[]); /* EAN-128 (GS1-128) */
extern int code_11(struct zint_symbol *symbol, unsigned char source[]); /* Code 11 */
extern int msi_handle(struct zint_symbol *symbol, unsigned char source[]); /* MSI Plessey */
extern int telepen(struct zint_symbol *symbol, unsigned char source[]); /* Telepen ASCII */
extern int telepen_num(struct zint_symbol *symbol, unsigned char source[]); /* Telepen Numeric */
extern int plessey(struct zint_symbol *symbol, unsigned char source[]); /* Plessey Code */
extern int pharma_one(struct zint_symbol *symbol, unsigned char source[]); /* Pharmacode One Track */
extern int flattermarken(struct zint_symbol *symbol, unsigned char source[]); /* Flattermarken */
extern int fim(struct zint_symbol *symbol, unsigned char source[]); /* Facing Identification Mark */
extern int pharma_two(struct zint_symbol *symbol, unsigned char source[]); /* Pharmacode Two Track */
extern int post_plot(struct zint_symbol *symbol, unsigned char source[]); /* Postnet */
extern int planet_plot(struct zint_symbol *symbol, unsigned char source[]); /* PLANET */
extern int imail(struct zint_symbol *symbol, unsigned char source[]); /* Intelligent Mail (aka USPS OneCode) */
extern int royal_plot(struct zint_symbol *symbol, unsigned char source[]); /* RM4SCC */
extern int australia_post(struct zint_symbol *symbol, unsigned char source[]); /* Australia Post 4-state */
extern int code16k(struct zint_symbol *symbol, unsigned char source[]); /* Code 16k */
extern int pdf417enc(struct zint_symbol *symbol, unsigned char source[]); /* PDF417 */
extern int dmatrix(struct zint_symbol *symbol, unsigned char source[]); /* Data Matrix (IEC16022) */
extern int qr_code(struct zint_symbol *symbol, unsigned char source[]); /* QR Code */
extern int micro_pdf417(struct zint_symbol *symbol, unsigned char source[]); /* Micro PDF417 */
extern int maxicode(struct zint_symbol *symbol, unsigned char source[]); /* Maxicode */
extern int rss14(struct zint_symbol *symbol, unsigned char source[]); /* RSS-14 */
extern int rsslimited(struct zint_symbol *symbol, unsigned char source[]); /* RSS Limited */
extern int rssexpanded(struct zint_symbol *symbol, unsigned char source[]); /* RSS Expanded */
extern int composite(struct zint_symbol *symbol, unsigned char source[]); /* Composite Symbology */
extern int kix_code(struct zint_symbol *symbol, unsigned char source[]); /* TNT KIX Code */
extern int aztec(struct zint_symbol *symbol, unsigned char source[]); /* Aztec Code */
extern int code32(struct zint_symbol *symbol, unsigned char source[]); /* Italian Pharmacode */
extern int codablock(struct zint_symbol *symbol, unsigned char source[]); /* Codablock F */
extern int daft_code(struct zint_symbol *symbol, unsigned char source[]); /* DAFT Code */
extern int ean_14(struct zint_symbol *symbol, unsigned char source[]); /* EAN-14 */
extern int nve_18(struct zint_symbol *symbol, unsigned char source[]); /* NVE-18 */
extern int microqr(struct zint_symbol *symbol, unsigned char source[]); /* Micro QR Code */
extern int aztec_runes(struct zint_symbol *symbol, unsigned char source[]); /* Aztec Runes */
extern int korea_post(struct zint_symbol *symbol, unsigned char source[]); /* Korea Post */

#ifndef NO_PNG
int png_handle(struct zint_symbol *symbol, int rotate_angle);
#endif

extern int ps_plot(struct zint_symbol *symbol);

void error_tag(char error_string[], int error_number)
{
	char error_buffer[100];
	
	if(error_number != 0) {
		strcpy(error_buffer, error_string);
		
		if(error_number > 4) {
			strcpy(error_string, "error: ");
		} else {
			strcpy(error_string, "warning: ");
		}
		
		concat(error_string, error_buffer);
	}
}

int ZBarcode_Encode(struct zint_symbol *symbol, unsigned char *unicode)
{
	int error_number, error_buffer;
	error_number = 0;
	int input_length;
	int i, j, next;
	
	input_length = ustrlen(unicode);
	unsigned char latin1[input_length];

	if(ustrlen(unicode) == 0) {
		strcpy(symbol->errtxt, "No input data [Z00]");
		error_tag(symbol->errtxt, ERROR_INVALID_DATA);
		return ERROR_INVALID_DATA;
	}
	
	/* First check the symbology field */
	if(symbol->symbology < 1) { strcpy(symbol->errtxt, "Symbology out of range, using Code 128 [Z01]"); symbol->symbology = BARCODE_CODE128; error_number = WARN_INVALID_OPTION; }

	/* symbol->symbologys 1 to 86 are defined by tbarcode */
	if(symbol->symbology == 5) { symbol->symbology = BARCODE_C25MATRIX; }
	if((symbol->symbology >= 10) && (symbol->symbology <= 12)) { symbol->symbology = BARCODE_EANX; }
	if((symbol->symbology == 14) || (symbol->symbology == 15)) { symbol->symbology = BARCODE_EANX; }
	if(symbol->symbology == 17) { symbol->symbology = BARCODE_UPCA; }
	if(symbol->symbology == 19) { strcpy(symbol->errtxt, "Codabar 18 not supported, using Codabar [Z02]"); symbol->symbology = BARCODE_CODABAR; error_number = WARN_INVALID_OPTION; }
	if(symbol->symbology == 24) { strcpy(symbol->errtxt, "Code 49 not supported, using Code 93 [Z03]"); symbol->symbology = BARCODE_CODE93; error_number = WARN_INVALID_OPTION; }
	if(symbol->symbology == 26) { symbol->symbology = BARCODE_UPCA; }
	if(symbol->symbology == 27) { strcpy(symbol->errtxt, "UPCD1 not supported [Z04]"); error_number = ERROR_INVALID_OPTION; }
	if(symbol->symbology == 33) { symbol->symbology = BARCODE_EAN128; }
	if((symbol->symbology == 35) || (symbol->symbology == 36)) { symbol->symbology = BARCODE_UPCA; }
	if((symbol->symbology == 38) || (symbol->symbology == 39)) { symbol->symbology = BARCODE_UPCE; }
	if((symbol->symbology >= 41) && (symbol->symbology <= 45)) { symbol->symbology = BARCODE_POSTNET; }
	if(symbol->symbology == 46) { symbol->symbology = BARCODE_PLESSEY; }
	if(symbol->symbology == 48) { symbol->symbology = BARCODE_NVE18; }
	if(symbol->symbology == 54) { strcpy(symbol->errtxt, "General Parcel Code not supported, using Code 128 [Z05]"); symbol->symbology = BARCODE_CODE128; error_number = WARN_INVALID_OPTION; }
	if((symbol->symbology == 59) || (symbol->symbology == 61)) { symbol->symbology = BARCODE_CODE128; }
	if(symbol->symbology == 62) { symbol->symbology = BARCODE_CODE93; }
	if((symbol->symbology == 64) || (symbol->symbology == 65)) { symbol->symbology = BARCODE_AUSPOST; }
	if(symbol->symbology == 73) { strcpy(symbol->errtxt, "Codablock E not supported [Z06]"); error_number = ERROR_INVALID_OPTION; }
	if(symbol->symbology == 76) { strcpy(symbol->errtxt, "Japanese Postal Code not supported [Z07]"); error_number = ERROR_INVALID_OPTION; }
	if(symbol->symbology == 78) { symbol->symbology = BARCODE_RSS14; }
	if(symbol->symbology == 83) { symbol->symbology = BARCODE_PLANET; }
	if(symbol->symbology == 88) { symbol->symbology = BARCODE_EAN128; }
	if(symbol->symbology == 91) { strcpy(symbol->errtxt, "Symbology out of range, using Code 128 [Z09]"); symbol->symbology = BARCODE_CODE128; error_number = WARN_INVALID_OPTION; }
	if((symbol->symbology >= 94) && (symbol->symbology <= 96)) { strcpy(symbol->errtxt, "Symbology out of range, using Code 128 [Z10]"); symbol->symbology = BARCODE_CODE128; error_number = WARN_INVALID_OPTION; }
	if((symbol->symbology >= 98) && (symbol->symbology <= 127)) { strcpy(symbol->errtxt, "Symbology out of range, using Code 128 [Z10]"); symbol->symbology = BARCODE_CODE128; error_number = WARN_INVALID_OPTION; }
	/* Everything from 128 up is Zint-specific */
	if(symbol->symbology >= 140) { strcpy(symbol->errtxt, "Symbology out of range, using Code 128 [Z11]"); symbol->symbology = BARCODE_CODE128; error_number = WARN_INVALID_OPTION; }

	if(error_number > 4) {
		error_tag(symbol->errtxt, error_number);
		return error_number;
	} else {
		error_buffer = error_number;
	}
	
	/* Supports UTF-8 input by converting it to Latin-1 Extended ASCII */
	/* At the moment all symbologies need this but in future versions this will
	be adapted to allow automatic ECI switching for more extended character support */
	j = 0;
	i = 0;
	do {
		next = -1;
		if(unicode[i] < 128) {
			latin1[j] = unicode[i];
			j++;
			next = i + 1;
		} else {
			if(unicode[i] == 0xC2) {
				latin1[j] = unicode[i + 1];
				j++;
				next = i + 2;
			}
			if(unicode[i] == 0xC3) {
				latin1[j] = unicode[i + 1] + 64;
				j++;
				next = i + 2;
			}
		}
		if(next == -1) {
			strcpy(symbol->errtxt, "error: Invalid character in input string (only Latin-1 characters supported)");
			return ERROR_INVALID_DATA;
		}
		i = next;
	} while(i < input_length);
	latin1[j] = '\0';
	
	if(symbol->symbology == BARCODE_CODE16K) {
		symbol->whitespace_width = 16;
		symbol->border_width = 2;
		symbol->output_options = BARCODE_BIND;
	}
	
	if(symbol->symbology == BARCODE_ITF14) {
		symbol->whitespace_width = 20;
		symbol->border_width = 8;
		symbol->output_options = BARCODE_BOX;
	}

	switch(symbol->symbology) {
		case BARCODE_C25MATRIX: error_number = matrix_two_of_five(symbol, latin1); break;
		case BARCODE_C25IND: error_number = industrial_two_of_five(symbol, latin1); break;
		case BARCODE_C25INTER: error_number = interleaved_two_of_five(symbol, latin1); break;
		case BARCODE_C25IATA: error_number = iata_two_of_five(symbol, latin1); break;
		case BARCODE_C25LOGIC: error_number = logic_two_of_five(symbol, latin1); break;
		case BARCODE_DPLEIT: error_number = dpleit(symbol, latin1); break;
		case BARCODE_DPIDENT: error_number = dpident(symbol, latin1); break;
		case BARCODE_UPCA: error_number = eanx(symbol, latin1); break;
		case BARCODE_UPCE: error_number = eanx(symbol, latin1); break;
		case BARCODE_EANX: error_number = eanx(symbol, latin1); break;
		case BARCODE_EAN128: error_number = ean_128(symbol, latin1); break;
		case BARCODE_CODE39: error_number = c39(symbol, latin1); break;
		case BARCODE_PZN: error_number = pharmazentral(symbol, latin1); break;
		case BARCODE_EXCODE39: error_number = ec39(symbol, latin1); break;
		case BARCODE_CODABAR: error_number = codabar(symbol, latin1); break;
		case BARCODE_CODE93: error_number = c93(symbol, latin1); break;
		case BARCODE_LOGMARS: error_number = c39(symbol, latin1); break;
		case BARCODE_CODE128: error_number = code_128(symbol, latin1); break;
		case BARCODE_CODE128B: error_number = code_128(symbol, latin1); break;
		case BARCODE_NVE18: error_number = nve_18(symbol, latin1); break;
		case BARCODE_CODE11: error_number = code_11(symbol, latin1); break;
		case BARCODE_MSI_PLESSEY: error_number = msi_handle(symbol, latin1); break;
		case BARCODE_TELEPEN: error_number = telepen(symbol, latin1); break;
		case BARCODE_TELEPEN_NUM: error_number = telepen_num(symbol, latin1); break;
		case BARCODE_PHARMA: error_number = pharma_one(symbol, latin1); break;
		case BARCODE_PLESSEY: error_number = plessey(symbol, latin1); break;
		case BARCODE_ITF14: error_number = itf14(symbol, latin1); break;
		case BARCODE_FLAT: error_number = flattermarken(symbol, latin1); break;
		case BARCODE_FIM: error_number = fim(symbol, latin1); break;
		case BARCODE_POSTNET: error_number = post_plot(symbol, latin1); break;
		case BARCODE_PLANET: error_number = planet_plot(symbol, latin1); break;
		case BARCODE_RM4SCC: error_number = royal_plot(symbol, latin1); break;
		case BARCODE_AUSPOST: error_number = australia_post(symbol, latin1); break;
		case BARCODE_AUSREPLY: error_number = australia_post(symbol, latin1); break;
		case BARCODE_AUSROUTE: error_number = australia_post(symbol, latin1); break;
		case BARCODE_AUSREDIRECT: error_number = australia_post(symbol, latin1); break;
		case BARCODE_CODE16K: error_number = code16k(symbol, latin1); break;
		case BARCODE_PHARMA_TWO: error_number = pharma_two(symbol, latin1); break;
		case BARCODE_ONECODE: error_number = imail(symbol, latin1); break;
		case BARCODE_DATAMATRIX: error_number = dmatrix(symbol, latin1); break;
		case BARCODE_PDF417: error_number = pdf417enc(symbol, latin1); break;
		case BARCODE_PDF417TRUNC: error_number = pdf417enc(symbol, latin1); break;
		case BARCODE_QRCODE: error_number = qr_code(symbol, latin1); break;
		case BARCODE_MICROPDF417: error_number = micro_pdf417(symbol, latin1); break;
		case BARCODE_ISBNX: error_number = eanx(symbol, latin1); break;
		case BARCODE_MAXICODE: error_number = maxicode(symbol, latin1); break;
		case BARCODE_RSS14: error_number = rss14(symbol, latin1); break;
		case BARCODE_RSS14STACK: error_number = rss14(symbol, latin1); break;
		case BARCODE_RSS14STACK_OMNI: error_number = rss14(symbol, latin1); break;
		case BARCODE_RSS_LTD: error_number = rsslimited(symbol, latin1); break;
		case BARCODE_RSS_EXP: error_number = rssexpanded(symbol, latin1); break;
		case BARCODE_RSS_EXPSTACK: error_number = rssexpanded(symbol, latin1); break;
		case BARCODE_EANX_CC: error_number = composite(symbol, latin1); break;
		case BARCODE_EAN128_CC: error_number = composite(symbol, latin1); break;
		case BARCODE_RSS14_CC: error_number = composite(symbol, latin1); break;
		case BARCODE_RSS_LTD_CC: error_number = composite(symbol, latin1); break;
		case BARCODE_RSS_EXP_CC: error_number = composite(symbol, latin1); break;
		case BARCODE_UPCA_CC: error_number = composite(symbol, latin1); break;
		case BARCODE_UPCE_CC: error_number = composite(symbol, latin1); break;
		case BARCODE_RSS14STACK_CC: error_number = composite(symbol, latin1); break;
		case BARCODE_RSS14_OMNI_CC: error_number = composite(symbol, latin1); break;
		case BARCODE_RSS_EXPSTACK_CC: error_number = composite(symbol, latin1); break;
		case BARCODE_AZTEC: error_number = aztec(symbol, latin1); break;
		case BARCODE_KIX: error_number = kix_code(symbol, latin1); break;
		case BARCODE_CODE32: error_number = code32(symbol, latin1); break;
		case BARCODE_CODABLOCKF: error_number = codablock(symbol, latin1); break;
		case BARCODE_DAFT: error_number = daft_code(symbol, latin1); break;
		case BARCODE_EAN14: error_number = ean_14(symbol, latin1); break;
		case BARCODE_MICROQR: error_number = microqr(symbol, latin1); break;
		case BARCODE_AZRUNE: error_number = aztec_runes(symbol, latin1); break;
		case BARCODE_KOREAPOST: error_number = korea_post(symbol, latin1); break;
	}
	if(error_number == 0) {
		error_number = error_buffer;
	}
	error_tag(symbol->errtxt, error_number);
	/*printf("%s\n",symbol->text);*/
	return error_number;
}

int ZBarcode_Print(struct zint_symbol *symbol)
{
	int error_number;
	char output[4];

	if(strlen(symbol->outfile) > 3) {
		output[0] = symbol->outfile[strlen(symbol->outfile) - 3];
		output[1] = symbol->outfile[strlen(symbol->outfile) - 2];
		output[2] = symbol->outfile[strlen(symbol->outfile) - 1];
		output[3] = '\0';
		to_upper((unsigned char*)output);
#ifndef NO_PNG
		if(!(strcmp(output, "PNG"))) {
			error_number = png_handle(symbol, 0);
		} else {
#endif
			if(!(strcmp(output, "EPS"))) {
				error_number = ps_plot(symbol);
			} else {
				strcpy(symbol->errtxt, "Unknown output format [Z20]");
				error_tag(symbol->errtxt, ERROR_INVALID_OPTION);
				return ERROR_INVALID_OPTION;
			}
#ifndef NO_PNG
		}
#endif
	} else {
		strcpy(symbol->errtxt, "Unknown output format [Z21]");
		error_tag(symbol->errtxt, ERROR_INVALID_OPTION);
		return ERROR_INVALID_OPTION;
	}

	error_tag(symbol->errtxt, error_number);
	return error_number;
}

int ZBarcode_Print_Rotated(struct zint_symbol *symbol, int rotate_angle)
{
	int error_number;
	char output[4];
	
	if(strlen(symbol->outfile) > 3) {
		output[0] = symbol->outfile[strlen(symbol->outfile) - 3];
		output[1] = symbol->outfile[strlen(symbol->outfile) - 2];
		output[2] = symbol->outfile[strlen(symbol->outfile) - 1];
		output[3] = '\0';
		to_upper((unsigned char*)output);

#ifndef NO_PNG
		if(!(strcmp(output, "PNG"))) {
			error_number = png_handle(symbol, rotate_angle);
		} else {
#endif
			if(!(strcmp(output, "EPS"))) {
				error_number = ps_plot(symbol);
			} else {
				strcpy(symbol->errtxt, "Unknown output format [Z23]");
				error_tag(symbol->errtxt, ERROR_INVALID_OPTION);
				return ERROR_INVALID_OPTION;
			}
#ifndef NO_PNG
		}
#endif
	} else {
		strcpy(symbol->errtxt, "Unknown output format [Z24]");
		error_tag(symbol->errtxt, ERROR_INVALID_OPTION);
		return ERROR_INVALID_OPTION;
	}

	error_tag(symbol->errtxt, error_number);
	return error_number;
}

int ZBarcode_Encode_and_Print(struct zint_symbol *symbol, unsigned char *input)
{
	int error_number;
	
	error_number = 0;
	
	error_number = ZBarcode_Encode(symbol, input);
	if(error_number != 0) {
		return error_number;
	}

	error_number = ZBarcode_Print(symbol);
	return error_number;
}

int ZBarcode_Encode_and_Print_Rotated(struct zint_symbol *symbol, unsigned char *input, int rotate_angle)
{
	int error_number;
	
	error_number = 0;
	
	error_number = ZBarcode_Encode(symbol, input);
	if(error_number != 0) {
		return error_number;
	}

	error_number = ZBarcode_Print_Rotated(symbol, rotate_angle);
	return error_number;
}
