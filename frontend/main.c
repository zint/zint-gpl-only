/* main.c - Command line handling routines for Zint */

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
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <zint.h>

#define NESET "0123456789"

void usage(void)
{
	printf(
		"Zint version %s\n"
		"Encode input data in a barcode and save as a PNG or EPS file.\n\n"
		"  -h, --help            Display this message.\n"
		"  -o, --output=FILE     Write image to FILE. (default is out.png)\n"
		"  -d, --data=DATA       Barcode content.\n"
		"  -b, --barcode=NUMBER  Number of barcode type (default is 20 (=Code128)).\n"
		"  --height=HEIGHT       Height of my_symbol in pixels.\n"
		"  -w, --whitesp=NUMBER  Width of whitespace in pixels.\n"
		"  --border=NUMBER       Width of border in pixels.\n"
		"  --box                 Add a box.\n"
		"  --bind                Add boundary bars.\n"
		"  -r, --reverse         Reverse colours (white on black).\n"
		"  --fg=COLOUR           Specify a foreground colour.\n"
		"  --bg=COLOUR           Specify a background colour.\n"
		"  --rotate=NUMBER       Rotate symbol (PNG output only).\n"
		"  --cols=NUMBER         (PDF417) Number of columns.\n"
		"  --vers=NUMBER         (QR Code) Version\n"
		"  --secure=NUMBER       (PDF417 and QR Code) Error correction level.\n"
		"  --primary=STRING      (Maxicode and Composite) Structured primary message.\n"
		"  --mode=NUMBER         (Maxicode and Composite) Set encoding mode.\n"
	, VERSION);
}

int main(int argc, char **argv)
{
	struct zint_symbol *my_symbol;
	int i, mode, stack_row;
	int c;
	int errornum;
	int rotate_angle;
	
	errornum = 0;
	rotate_angle = 0;
	my_symbol = ZBarcode_Create();

	if(argc == 1) {
		usage();
		exit(1);
	}

	while(1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help", 0, 0, 'h'},
			{"bind", 0, 0, 0},
			{"box", 0, 0, 0},
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
			{"case", 0, 0, 'c'},
			{"mode=", 1, 0, 0},
			{"primary=", 1, 0, 0},
			{0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "hb:w:d:o:i:rcmp", long_options, &option_index);
		if(c == -1) break; 
		
		switch(c) {
			case 0: 
				if(!strcmp(long_options[option_index].name, "bind")) {
					my_symbol->output_options = BARCODE_BIND;
				}
				if(!strcmp(long_options[option_index].name, "box")) {
					my_symbol->output_options = BARCODE_BOX;
				}
				if(!strcmp(long_options[option_index].name, "fg=")) {
					strncpy(my_symbol->fgcolour, optarg, 7);
				}
				if(!strcmp(long_options[option_index].name, "bg=")) {
					strncpy(my_symbol->bgcolour, optarg, 7);
				}
				if(!strcmp(long_options[option_index].name, "border=")) {
					errornum = is_sane(NESET, optarg);
					if(errornum == ERROR_INVALID_DATA) {
						fprintf(stderr, "Invalid border width\n");
						exit(1);
					}
					if((atoi(optarg) >= 0) && (atoi(optarg) <= 1000)) {
						my_symbol->border_width = atoi(optarg);
					} else {
						fprintf(stderr, "Border width out of range\n");
					}
				}
				if(!strcmp(long_options[option_index].name, "height=")) {
					errornum = is_sane(NESET, optarg);
					if(errornum == ERROR_INVALID_DATA) {
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
					errornum = is_sane(NESET, optarg);
					if(errornum == ERROR_INVALID_DATA) {
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
				
			case 'b':
				errornum = is_sane(NESET, optarg);
				if(errornum == ERROR_INVALID_DATA) {
					printf("Invalid barcode type\n");
					exit(1);
				}
				my_symbol->symbology = atoi(optarg);
				break;
				
			case 'w':
				errornum = is_sane(NESET, optarg);
				if(errornum == ERROR_INVALID_DATA) {
					printf("Invalid whitespace value\n");
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
					if(ZBarcode_Encode_and_Print(my_symbol, optarg) != 0) {
						printf("%s\n", my_symbol->errtxt);
						return 1;
					}
				} else {
					if(ZBarcode_Encode_and_Print_Rotated(my_symbol, optarg, rotate_angle) != 0) {
						printf("%s\n", my_symbol->errtxt);
						return 1;
					}
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
				printf("?? getopt error 0%o\n", c);
		} 
	}
	
		if (optind < argc) {
		printf("Invalid option ");
		while (optind < argc)
			printf("%s", argv[optind++]);
		printf("\n");
	}
	
	if(strcmp(my_symbol->errtxt, "")) {
		printf(my_symbol->errtxt);
		printf("\n");
	}
	
	ZBarcode_Delete(my_symbol); 
	
	return errornum;
}
