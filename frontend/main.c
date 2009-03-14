/* main.c - Command line handling routines for Zint */

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
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <zint.h>

#define NESET "0123456789"

void types(void) {
	printf( 	"  1: Code 11                51: Pharma One-Track         87: Telepen Numeric\n"
			"  2: Standard 2 of 5        52: PZN                      89: ITF-14\n"
			"  3: Interleaved 2 of 5     53: Pharma Two-Track         90: KIX Code\n"
			"  4: IATA 2 of 5            55: PDF417                   92: Aztec Code\n"
			"  6: Data Logic             56: PDF417 Trunc             93: DAFT Code\n"
			"  7: Industrial 2 of 5      57: Maxicode                 97: Micro QR Code\n"
			"  8: Code 39                58: QR Code                  98: HIBC Code 128\n"
			"  9: Extended Code 39       60: Code 128-B               99: HIBC Code 39\n"
			" 13: EAN                    63: AP Standard Customer     102: HIBC Data Matrix\n"
			" 16: GS1-128                66: AP Reply Paid            104: HIBC QR Code\n"
			" 18: Codabar                67: AP Routing               106: HIBC PDF417\n"
			" 20: Code 128               68: AP Redirection           108: HIBC MicroPDF417\n"
			" 21: Leitcode               69: ISBN                     110: HIBC Codablock-F\n"
			" 22: Identcode              70: RM4SCC                   128: Aztec Runes\n"
			" 23: Code 16k               71: Data Matrix              129: Code 23\n"
			" 25: Code 93                72: EAN-14                   130: Comp + EAN\n"
			" 28: Flattermarken          74: Codablock-F              131: Comp + GS1-128\n"
			" 29: Databar-14             75: NVE-18                   132: Comp + Databar-14\n"
			" 30: Databar Limited        76: Japanese Post            133: Comp + Databar Ltd\n"
			" 31: Databar Extended       77: Korea Post               134: Comp + Databar Ext\n"
			" 32: Telepen Alpha          79: Databar-14 Stack         135: Comp + UPC-A\n"
			" 34: UPC-A                  80: Databar-14 Stack Omni    136: Comp + UPC-E\n"
			" 37: UPC-E                  81: Databar Extended Stack   137: Comp + Databar-14 Stack\n"
			" 40: Postnet                82: Planet                   138: Comp + Databar Stack Omni\n"
			" 47: MSI Plessey            84: MicroPDF                 139: Comp + Databar Ext Stack\n"
			" 49: FIM                    85: USPS OneCode\n"
			" 50: Logmars                86: UK Plessey\n"
	);
}

void usage(void)
{
	printf(
		"Zint version %s\n"
		"Encode input data in a barcode and save as a PNG or EPS file.\n\n"
		"  -h, --help            Display this message.\n"
		"  -t, --types           Display table of barcode types\n"
		"  -o, --output=FILE     Write image to FILE. (default is out.png)\n"
		"  -d, --data=DATA       Barcode content.\n"
		"  -b, --barcode=NUMBER  Number of barcode type (default is 20 (=Code128)).\n"
		"  --height=NUMBER       Height of symbol in multiples of x-dimension.\n"
		"  -w, --whitesp=NUMBER  Width of whitespace in multiples of x-dimension.\n"
		"  --border=NUMBER       Width of border in multiples of x-dimension.\n"
		"  --box                 Add a box.\n"
		"  --bind                Add boundary bars.\n"
		"  -r, --reverse         Reverse colours (white on black).\n"
		"  --fg=COLOUR           Specify a foreground colour.\n"
		"  --bg=COLOUR           Specify a background colour.\n"
		"  --scale=NUMBER        Adjust size of output image.\n"
		"  --directpng           Send PNG output to stdout\n"
		"  --directeps           Send EPS output to stdout\n"
		"  --rotate=NUMBER       Rotate symbol (PNG output only).\n"
		"  --cols=NUMBER         (PDF417) Number of columns.\n"
		"  --vers=NUMBER         (QR Code) Version\n"
		"  --secure=NUMBER       (PDF417 and QR Code) Error correction level.\n"
		"  --primary=STRING      (Maxicode and Composite) Structured primary message.\n"
		"  --mode=NUMBER         (Maxicode and Composite) Set encoding mode.\n"
		"  --null=NUMBER         Character to represent NULL.\n"
		"  --gs1                 Treat input as GS1 data\n"
		"  --kanji               Treat input as Kanji characters in Unicode\n"
		"  --sjis                Treat input as Shift-JIS\n"
	, ZINT_VERSION);
}

int ustrlen(unsigned char data[]) {
	/* Local replacement for strlen() with unsigned char strings */
	int i;
	
	i = -1;
	do { i++; } while (data[i] != '\0');

	return i;
}

int validator(char test_string[], char source[])
{ /* Verifies that a string only uses valid characters */
	unsigned int i, j, latch;

	for(i = 0; i < strlen(source); i++) {
		latch = 0;
		for(j = 0; j < strlen(test_string); j++) {
			if (source[i] == test_string[j]) { latch = 1; } }
			if (!(latch)) { 
				return ERROR_INVALID_DATA; }
	}
	
	return 0;
}

int main(int argc, char **argv)
{
	struct zint_symbol *my_symbol;
	int c;
	int error_number;
	int rotate_angle;
	int generated;
	
	error_number = 0;
	rotate_angle = 0;
	generated = 0;
	my_symbol = ZBarcode_Create();
	my_symbol->input_mode = UNICODE_MODE;

	if(argc == 1) {
		usage();
		exit(1);
	}

	while(1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help", 0, 0, 'h'},
			{"types", 0, 0, 't'},
			{"bind", 0, 0, 0},
			{"box", 0, 0, 0},
			{"directeps", 0, 0, 0},
			{"directpng", 0, 0, 0},
			{"directsvg", 0, 0, 0},
			{"barcode=", 1, 0, 'b'},
			{"height=", 1, 0, 0},
			{"whitesp=", 1, 0, 'w'},
			{"border=", 1, 0, 0},
			{"data=", 1, 0, 'd'},
			{"output=", 1, 0, 'o'},
			{"fg=", 1, 0, 0},
			{"bg=", 1, 0, 0},
			{"cols=", 1, 0, 0},
			{"vers=", 1, 0, 0},
			{"rotate=", 1, 0, 0},
			{"secure=", 1, 0, 0},
			{"reverse", 1, 0, 'r'},
			{"mode=", 1, 0, 0},
			{"primary=", 1, 0, 0},
			{"scale=", 1, 0, 0},
			{"null=", 1, 0, 0},
			{"gs1", 0, 0, 0},
			{"kanji", 0, 0, 0},
			{"sjis", 0, 0, 0},
			{0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "htb:w:d:o:i:rcmp", long_options, &option_index);
		if(c == -1) break; 
		
		switch(c) {
			case 0: 
				if(!strcmp(long_options[option_index].name, "bind")) {
					my_symbol->output_options += BARCODE_BIND;
				}
				if(!strcmp(long_options[option_index].name, "box")) {
					my_symbol->output_options += BARCODE_BOX;
				}
				if(!strcmp(long_options[option_index].name, "directeps")) {
					my_symbol->output_options += BARCODE_STDOUT;
					strncpy(my_symbol->outfile, "dummy.eps", 10);
				}
				if(!strcmp(long_options[option_index].name, "directpng")) {
					my_symbol->output_options += BARCODE_STDOUT;
					strncpy(my_symbol->outfile, "dummy.png", 10);
				}
				if(!strcmp(long_options[option_index].name, "directsvg")) {
					my_symbol->output_options += BARCODE_STDOUT;
					strncpy(my_symbol->outfile, "dummy.svg", 10);
				}
				if(!strcmp(long_options[option_index].name, "gs1")) {
					my_symbol->input_mode = GS1_MODE;
				}
				if(!strcmp(long_options[option_index].name, "kanji")) {
					my_symbol->input_mode = KANJI_MODE;
				}
				if(!strcmp(long_options[option_index].name, "sjis")) {
					my_symbol->input_mode = SJIS_MODE;
				}
				if(!strcmp(long_options[option_index].name, "fg=")) {
					strncpy(my_symbol->fgcolour, optarg, 7);
				}
				if(!strcmp(long_options[option_index].name, "bg=")) {
					strncpy(my_symbol->bgcolour, optarg, 7);
				}
				if(!strcmp(long_options[option_index].name, "scale=")) {
					my_symbol->scale = (float)(atof(optarg));
					if(my_symbol->scale < 0.01) {
						/* Zero and negative values are not permitted */
						fprintf(stderr, "Invalid scale value\n");
						my_symbol->scale = 1.0;
					}
				}
				if(!strcmp(long_options[option_index].name, "border=")) {
					error_number = validator(NESET, optarg);
					if(error_number == ERROR_INVALID_DATA) {
						fprintf(stderr, "Invalid border width\n");
						exit(1);
					}
					if((atoi(optarg) >= 0) && (atoi(optarg) <= 1000)) {
						my_symbol->border_width = atoi(optarg);
					} else {
						fprintf(stderr, "Border width out of range\n");
					}
				}
				if(!strcmp(long_options[option_index].name, "null=")) {
					error_number = validator(NESET, optarg);
					if(error_number == ERROR_INVALID_DATA) {
						fprintf(stderr, "Invalid NULL replacement\n");
						exit(1);
					}
					if((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
						my_symbol->nullchar = atoi(optarg);
					} else {
						fprintf(stderr, "Invalid NULL replacement\n");
					}
				}
				if(!strcmp(long_options[option_index].name, "height=")) {
					error_number = validator(NESET, optarg);
					if(error_number == ERROR_INVALID_DATA) {
						fprintf(stderr, "Invalid symbol height\n");
						exit(1);
					}
					if((atoi(optarg) >= 1) && (atoi(optarg) <= 1000)) {
						my_symbol->height = atoi(optarg);
					} else {
						fprintf(stderr, "Symbol height out of range\n");
					}
				}

				if(!strcmp(long_options[option_index].name, "cols=")) {
					if((atoi(optarg) >= 1) && (atoi(optarg) <= 30)) {
						my_symbol->option_2 = atoi(optarg);
					} else {
						fprintf(stderr, "Number of columns out of range\n");
					}
				}
				if(!strcmp(long_options[option_index].name, "vers=")) {
					if((atoi(optarg) >= 1) && (atoi(optarg) <= 40)) {
						my_symbol->option_2 = atoi(optarg);
					} else {
						fprintf(stderr, "Invalid QR Code version\n");
					}
				}
				if(!strcmp(long_options[option_index].name, "secure=")) {
					if((atoi(optarg) >= 1) && (atoi(optarg) <= 8)) {
						my_symbol->option_1 = atoi(optarg);
					} else {
						fprintf(stderr, "ECC level out of range\n");
					}
				}
				if(!strcmp(long_options[option_index].name, "primary=")) {
					if(strlen(optarg) <= 90) {
						strcpy(my_symbol->primary, optarg);
					} else {
						fprintf(stderr, "Primary data string too long");
					}
				}
				if(!strcmp(long_options[option_index].name, "mode=")) {
					/* Don't allow specification of modes 2 and 3 - do it
					automagically instead */
					if((optarg[0] >= '0') && (optarg[0] <= '6')) {
						my_symbol->option_1 = optarg[0] - '0';
					} else {
						fprintf(stderr, "Invalid mode\n");
					}
				}
				if(!strcmp(long_options[option_index].name, "rotate=")) {
					/* Only certain inputs allowed */
					error_number = validator(NESET, optarg);
					if(error_number == ERROR_INVALID_DATA) {
						fprintf(stderr, "Invalid rotation parameter\n");
						exit(1);
					}
					switch(atoi(optarg)) {
						case 90: rotate_angle = 90; break;
						case 180: rotate_angle = 180; break;
						case 270: rotate_angle = 270; break;
						default: rotate_angle = 0; break;
					}
				}
				break;
				
			case 'h':
				usage();
				break;
				
			case 't':
				types();
				break;
				
			case 'b':
				error_number = validator(NESET, optarg);
				if(error_number == ERROR_INVALID_DATA) {
					fprintf(stderr, "Invalid barcode type\n");
					exit(1);
				}
				my_symbol->symbology = atoi(optarg);
				break;
				
			case 'w':
				error_number = validator(NESET, optarg);
				if(error_number == ERROR_INVALID_DATA) {
					fprintf(stderr, "Invalid whitespace value\n");
					exit(1);
				}
				if((atoi(optarg) >= 0) && (atoi(optarg) <= 1000)) {
					my_symbol->whitespace_width = atoi(optarg);
				} else {
					fprintf(stderr, "Whitespace value out of range");
				}
				break;
				
			case 'd': /* we have some data! */
				if(rotate_angle == 0) {
					error_number = ZBarcode_Encode_and_Print(my_symbol, (unsigned char*)optarg);
				} else {
					error_number = ZBarcode_Encode_and_Print_Rotated(my_symbol, (unsigned char*)optarg, rotate_angle);
				}
				generated = 1;
				if(error_number != 0) {
					fprintf(stderr, "%s\n", my_symbol->errtxt);
					ZBarcode_Delete(my_symbol);
					return 1;
				}
				break;
				
			case 'o':
				strncpy(my_symbol->outfile, optarg, 250);
				break;
				
			case 'r':
				strcpy(my_symbol->fgcolour, "ffffff");
				strcpy(my_symbol->bgcolour, "000000");
				break;
				
			case '?':
				break;
				
			default:
				fprintf(stderr, "?? getopt error 0%o\n", c);
		} 
	}
	
		if (optind < argc) {
		fprintf(stderr, "Invalid option ");
		while (optind < argc)
			fprintf(stderr, "%s", argv[optind++]);
		fprintf(stderr, "\n");
	}
	
	if(generated == 0) {
		fprintf(stderr, "error: No data received, no symbol generated\n");
	}
	
	ZBarcode_Delete(my_symbol); 
	
	return error_number;
}
