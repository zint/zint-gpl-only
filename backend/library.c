/*  library.c - external functions of libzint

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
	symbol->option_3 = 928; //PDF_MAX
	symbol->rows = 0;
	symbol->width = 0;
	strcpy(symbol->fgcolour, "000000");
	strcpy(symbol->bgcolour, "ffffff");
	strcpy(symbol->outfile, "out.png");
	symbol->option_1 = -1;
	symbol->option_2 = 0;
	strcpy(symbol->primary, "");
	for(i = 0; i < 90; i++) {
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
extern int msi_plessey(struct zint_symbol *symbol, unsigned char source[]); /* MSI Plessey without check digit */
extern int msi_plessey_mod10(struct zint_symbol *symbol, unsigned char source[]); /* MSI with check as on Barcode Island */
extern int msi_plessey_mod1010(struct zint_symbol *symbol, unsigned char source[]); /* MSI with two checks as Barcode Island */
extern int msi_plessey_mod11(struct zint_symbol *symbol, unsigned char source[]); /* MSI with Mod 11 check as Wikipedia */
extern int msi_plessey_mod1110(struct zint_symbol *symbol, unsigned char source[]); /* MSI with Mod 11 and Mod 10 */
extern int telepen(struct zint_symbol *symbol, unsigned char source[]); /* Telepen ASCII */
extern int telepen_num(struct zint_symbol *symbol, unsigned char source[]); /* Telepen Numeric */
extern int plessey(struct zint_symbol *symbol, unsigned char source[]); /* Plessey Code */
extern int pharma_one(struct zint_symbol *symbol, unsigned char source[]); /* Pharmacode One Track */
/* extern int korea(struct zint_symbol *symbol, unsigned char source[]); */
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
/* extern int aztec_mesa(struct zint_symbol *symbol, unsigned char source[]); */
/* extern int ultra_sample(struct zint_symbol *symbol, unsigned char source[]); */

extern int png_plot(struct zint_symbol *symbol);
extern int ps_plot(struct zint_symbol *symbol);

int ZBarcode_Encode(struct zint_symbol *symbol, unsigned char *input)
{
	int errno;
	errno = 0;

	/* First check the symbology field */
	if(symbol->symbology < 1) { strcpy(symbol->errtxt, "Symbology out of range, using Code 128"); symbol->symbology = BARCODE_CODE128; errno = WARN_INVALID_OPTION; }

	/* symbol->symbologys 1 to 86 are defined by tbarcode */
	if(symbol->symbology == 5) { symbol->symbology = BARCODE_C25MATRIX; }
	if((symbol->symbology >= 10) && (symbol->symbology <= 12)) { symbol->symbology = BARCODE_EANX; }
	if((symbol->symbology == 14) || (symbol->symbology == 15)) { symbol->symbology = BARCODE_EANX; }
	if(symbol->symbology == 17) { symbol->symbology = BARCODE_UPCA; }
	if(symbol->symbology == 19) { strcpy(symbol->errtxt, "Codabar 18 not supported, using Codabar"); symbol->symbology = BARCODE_CODABAR; errno = WARN_INVALID_OPTION; }
	if(symbol->symbology == 24) { strcpy(symbol->errtxt, "Code 49 not supported, using Code 93"); symbol->symbology = BARCODE_CODE93; errno = WARN_INVALID_OPTION; }
	if(symbol->symbology == 26) { symbol->symbology = BARCODE_UPCA; }
	if(symbol->symbology == 27) { strcpy(symbol->errtxt, "UPCD1 not supported"); errno = ERROR_INVALID_OPTION; }
	if(symbol->symbology == 33) { symbol->symbology = BARCODE_EAN128; }
	if((symbol->symbology == 35) || (symbol->symbology == 36)) { symbol->symbology = BARCODE_UPCA; }
	if((symbol->symbology == 38) || (symbol->symbology == 39)) { symbol->symbology = BARCODE_UPCE; }
	if((symbol->symbology >= 41) && (symbol->symbology <= 45)) { symbol->symbology = BARCODE_POSTNET; }
	if(symbol->symbology == 46) { symbol->symbology = BARCODE_PLESSEY; }
	if(symbol->symbology == 48) { symbol->symbology = BARCODE_NVE18; }
	if(symbol->symbology == 54) { strcpy(symbol->errtxt, "General Parcel Code not supported, using Code 128"); symbol->symbology = BARCODE_CODE128; errno = WARN_INVALID_OPTION; }
	if((symbol->symbology == 59) || (symbol->symbology == 61)) { symbol->symbology = BARCODE_CODE128; }
	if(symbol->symbology == 62) { symbol->symbology = BARCODE_CODE93; }
	if((symbol->symbology == 64) || (symbol->symbology == 65)) { symbol->symbology = BARCODE_AUSPOST; }
	if(symbol->symbology == 73) { strcpy(symbol->errtxt, "Codablock E not supported"); errno = ERROR_INVALID_OPTION; }
	if(symbol->symbology == 74) { strcpy(symbol->errtxt, "Codablock F not supported"); errno = ERROR_INVALID_OPTION; }
	if(symbol->symbology == 76) { strcpy(symbol->errtxt, "Japanese Postal Code not supported"); errno = ERROR_INVALID_OPTION; }
	if(symbol->symbology == 77) { strcpy(symbol->errtxt, "Korean Postal Code not supported"); errno = ERROR_INVALID_OPTION; }
	if(symbol->symbology == 78) { symbol->symbology = BARCODE_RSS14; }
	if(symbol->symbology == 83) { symbol->symbology = BARCODE_PLANET; }
	/* leave a gap for future expansion of tbarcode */
	if((symbol->symbology >= 87) && (symbol->symbology <= 99)) { strcpy(symbol->errtxt, "Symbology out of range, using Code 128\n"); symbol->symbology = BARCODE_CODE128; errno = WARN_INVALID_OPTION; }
	/* Everything from 100 up is Zint-specific */
	if(symbol->symbology == 107) { strcpy(symbol->errtxt, "Aztec Code not yet supported"); errno = ERROR_INVALID_OPTION; }
	if(symbol->symbology == 108) { strcpy(symbol->errtxt, "Supercode not yet supported"); errno = ERROR_INVALID_OPTION; }
	if(symbol->symbology == 109) { strcpy(symbol->errtxt, "Ultracode not yet supported"); errno = ERROR_INVALID_OPTION; }
	if(symbol->symbology >= 120) { strcpy(symbol->errtxt, "Symbology out of range, using Code 128"); symbol->symbology = BARCODE_CODE128; errno = WARN_INVALID_OPTION; }

	if(errno > 4) {
		return errno;
	}

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
		case BARCODE_C25MATRIX: errno = matrix_two_of_five(symbol, input); break;
		case BARCODE_C25IND: errno = industrial_two_of_five(symbol, input); break;
		case BARCODE_C25INTER: errno = interleaved_two_of_five(symbol, input); break;
		case BARCODE_C25IATA: errno = iata_two_of_five(symbol, input); break;
		case BARCODE_C25LOGIC: errno = logic_two_of_five(symbol, input); break;
		case BARCODE_DPLEIT: errno = dpleit(symbol, input); break;
		case BARCODE_DPIDENT: errno = dpident(symbol, input); break;
		case BARCODE_UPCA: errno = eanx(symbol, input); break;
		case BARCODE_UPCE: errno = eanx(symbol, input); break;
		case BARCODE_EANX: errno = eanx(symbol, input); break;
		case BARCODE_EAN128: errno = ean_128(symbol, input); break;
		case BARCODE_CODE39: errno = c39(symbol, input); break;
		case BARCODE_PZN: errno = pharmazentral(symbol, input); break;
		case BARCODE_EXCODE39: errno = ec39(symbol, input); break;
		case BARCODE_CODABAR: errno = codabar(symbol, input); break;
		case BARCODE_CODE93: errno = c93(symbol, input); break;
		case BARCODE_LOGMARS: errno = c39(symbol, input); break;
		case BARCODE_CODE128: errno = code_128(symbol, input); break;
		case BARCODE_CODE128B: errno = code_128(symbol, input); break;
		case BARCODE_NVE18: errno = code_128(symbol, input); break;
		case BARCODE_CODE11: errno = code_11(symbol, input); break;
		case BARCODE_MSI_PLESSEY: errno = msi_plessey(symbol, input); break;
		case BARCODE_MSI_10: errno = msi_plessey_mod10(symbol, input); break;
		case BARCODE_MSI_10_10: errno = msi_plessey_mod1010(symbol, input); break;
		case BARCODE_MSI_11: errno = msi_plessey_mod11(symbol, input); break;
		case BARCODE_MSI_11_10: errno = msi_plessey_mod1110(symbol, input); break;
		case BARCODE_TELEPEN: errno = telepen(symbol, input); break;
		case BARCODE_TELEPEN_NUM: errno = telepen_num(symbol, input); break;
		case BARCODE_PHARMA: errno = pharma_one(symbol, input); break;
		case BARCODE_PLESSEY: errno = plessey(symbol, input); break;
		case BARCODE_ITF14: errno = itf14(symbol, input); break;
		/* case BARCODE_KOREAPOST: errno = korea(symbol, input); break; */
		case BARCODE_FLAT: errno = flattermarken(symbol, input); break;
		case BARCODE_FIM: errno = fim(symbol, input); break;
		case BARCODE_POSTNET: errno = post_plot(symbol, input); break;
		case BARCODE_PLANET: errno = planet_plot(symbol, input); break;
		case BARCODE_RM4SCC: errno = royal_plot(symbol, input); break;
		case BARCODE_AUSPOST: errno = australia_post(symbol, input); break;
		case BARCODE_AUSREPLY: errno = australia_post(symbol, input); break;
		case BARCODE_AUSROUTE: errno = australia_post(symbol, input); break;
		case BARCODE_AUSREDIRECT: errno = australia_post(symbol, input); break;
		case BARCODE_CODE16K: errno = code16k(symbol, input); break;
		case BARCODE_PHARMA_TWO: errno = pharma_two(symbol, input); break;
		case BARCODE_ONECODE: errno = imail(symbol, input); break;
		case BARCODE_DATAMATRIX: errno = dmatrix(symbol, input); break;
		case BARCODE_PDF417: errno = pdf417enc(symbol, input); break;
		case BARCODE_PDF417TRUNC: errno = pdf417enc(symbol, input); break;
		case BARCODE_QRCODE: errno = qr_code(symbol, input); break;
		case BARCODE_MICROPDF417: errno = micro_pdf417(symbol, input); break;
		case BARCODE_ISBNX: errno = eanx(symbol, input); break;
		case BARCODE_CODE39_43: errno = c39(symbol, input); break;
		case BARCODE_EXCODE39_43: errno = ec39(symbol, input); break;
		case BARCODE_MAXICODE: errno = maxicode(symbol, input); break;
		case BARCODE_RSS14: errno = rss14(symbol, input); break;
		case BARCODE_RSS14STACK: errno = rss14(symbol, input); break;
		case BARCODE_RSS14STACK_OMNI: errno = rss14(symbol, input); break;
		case BARCODE_RSS_LTD: errno = rsslimited(symbol, input); break;
		case BARCODE_RSS_EXP: errno = rssexpanded(symbol, input); break;
		case BARCODE_RSS_EXPSTACK: errno = rssexpanded(symbol, input); break;
		case BARCODE_EANX_CC: errno = composite(symbol, input); break;
		case BARCODE_EAN128_CC: errno = composite(symbol, input); break;
		case BARCODE_RSS14_CC: errno = composite(symbol, input); break;
		case BARCODE_RSS_LTD_CC: errno = composite(symbol, input); break;
		case BARCODE_RSS_EXP_CC: errno = composite(symbol, input); break;
		case BARCODE_UPCA_CC: errno = composite(symbol, input); break;
		case BARCODE_UPCE_CC: errno = composite(symbol, input); break;
		case BARCODE_RSS14STACK_CC: errno = composite(symbol, input); break;
		case BARCODE_RSS14_OMNI_CC: errno = composite(symbol, input); break;
		case BARCODE_RSS_EXPSTACK_CC: errno = composite(symbol, input); break;
		/* case BARCODE_AZTEC: errno = aztec_mesa(symbol, input); break; */
		/* case BARCODE_ULTRA: errno = ultra(symbol, input); break; */
	}

	return errno;
}

int ZBarcode_Print(struct zint_symbol *symbol)
{
	int errno;
	char output[4];

	if(strlen(symbol->outfile) > 3) {
		output[0] = symbol->outfile[strlen(symbol->outfile) - 3];
		output[1] = symbol->outfile[strlen(symbol->outfile) - 2];
		output[2] = symbol->outfile[strlen(symbol->outfile) - 1];
		output[3] = '\0';
		to_upper(output);
#ifndef NO_PNG
		if(!(strcmp(output, "PNG"))) {
			if(symbol->symbology == BARCODE_MAXICODE) {
				errno = maxi_png_plot(symbol);
			} else {
				errno = png_plot(symbol);
			}
		} else {
#endif
			if(!(strcmp(output, "EPS"))) {
				errno = ps_plot(symbol);
			} else {
				strcpy(symbol->errtxt, "error: unknown output format");
				return ERROR_INVALID_OPTION;
			}
#ifndef NO_PNG
		}
#endif
	} else {
		strcpy(symbol->errtxt, "error: unknown output format");
		return ERROR_INVALID_OPTION;
	}

	return errno;
}

int ZBarcode_Encode_and_Print(struct zint_symbol *symbol, unsigned char *input)
{
	int errno;

	errno = 0;

	errno = ZBarcode_Encode(symbol, input);
	if(errno != 0) {
		return errno;
	}

	errno = ZBarcode_Print(symbol);
	return errno;
}
