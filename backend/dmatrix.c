/* dmatrix.c - Handles Data Matrix 2-D symbology (IEC16022 ecc 200) */

/*
    libzint - the open source barcode library
    Copyright (C) 2008 Robin Stuart <zint@hotmail.co.uk>

    This file is a hacked-up copy of:
 * IEC16022 bar code generation by
 * Adrian Kennard, Andrews & Arnold Ltd
 * with help from Cliff Hones on the RS coding
 *
 * (c) 2004 Adrian Kennard, Andrews & Arnold Ltd
 * (c) 2006 Stefan Schmidt <stefan@datenfreihafen.org>

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

/* The original version of this code is available at:
   http://www.datenfreihafen.org/projects/iec16022.html */

#define IEC16022_VERSION "0.2"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "dm200.h"
#include "common.h"


 // simple checked response malloc
void *safemalloc(int n)
{
	void *p = malloc(n);
	if (!p) {
		fprintf(stderr, "Malloc(%d) failed\n", n);
		exit(1);
	}
	return p;
}

int dmatrix(struct zint_symbol *symbol, unsigned char source[])
{
	int W = 0, H = 0;
	int ecc = 0;
	int barcodelen = 0;
	char *encoding = 0;
	int len = 0, maxlen = 0, ecclen = 0;
	unsigned char *grid = 0;
	char size[3], eccstr[3];
	
	strcpy(size, "");
	strcpy(eccstr, "200");
	
	/* if (strlen(barcode) == 0) {		// read from file
		FILE *f = fopen(infile, "rb");
		barcode = safemalloc(4001);
		if (!f) {
			strcpy(symbol->errtxt, "error: could not open file");
			return 8;
		}
		barcodelen = fread(barcode, 1, 4000, f);
		if (barcodelen < 0) {
			strcpy(symbol->errtxt, "error: could not read file");
			return 8;
		}
		barcode[barcodelen] = 0;	// null terminate anyway
		fclose(f); 
} else */
		barcodelen = ustrlen(source);
		if(barcodelen > 780) {
			strcpy(symbol->errtxt, "Input too long [711]");
			return ERROR_TOO_LONG;
		}
	// check parameters
	if (strlen(size) != 0) {
		char *x = strchr(size, 'x');
		W = atoi(size);
		if (x)
			H = atoi(x + 1);
		if (!H)
			W = H;
	}
/*	if (eccstr) */
		ecc = atoi(eccstr);
		
		/* Yes I _have_ commented out large blocks of code! - odd size Data Matrix support
		   may be included in a later release but the code for it isn't needed here */
		
/*	if (W & 1) {		// odd size
		if (W != H || W < 9 || W > 49) {
			strcpy(symbol->errtxt, "error: invalid Data Matrix size");
			return ERROR_INVALID_OPTION;
		}
		if (!eccstr) {
			if (W >= 17)
				ecc = 140;
			else if (W >= 13)
				ecc = 100;
			else if (W >= 11)
				ecc = 80;
			else
				ecc = 0;
		}
		if (ecc && ecc != 50 && ecc != 80 && ecc != 100 && ecc != 140 ||
		    ecc == 50 && W < 11 || ecc == 80 && W < 13 || ecc == 100
		    && W < 13 || ecc == 140 && W < 17) {
			strcpy(symbol->errtxt, "error: invalid ecc value");
			return ERROR_INVALID_OPTION;
		}

	} else if (W) {		 // even size
		if (W < H) {
			int t = W;
			W = H;
			H = t;
		}
		if (!eccstr)
			ecc = 200;
		if (ecc != 200) {
			strcpy(symbol->errtxt, "error: invalid size for ecc 200");
			return ERROR_INVALID_OPTION;
		}
	} 

	else {			// auto size
		if (!eccstr)
			// default is even sizes only unless explicit ecc set to force odd
			// sizes
			ecc = 200;
	} */

	// processing stamps
	/*if ((W & 1) || ecc < 200) {	// odd sizes
		strcpy(symbol->errtxt, "error: odd sizes not supported");
		return ERROR_INVALID_OPTION;
	} else { */		// even sizes
		grid = iec16022ecc200(&W, &H, &encoding, barcodelen, source, &len, &maxlen, &ecclen);
	/*} */

	// output
	if (!grid || !W) {
		strcpy(symbol->errtxt, "Data Matrix encoding error [722]");
		return ERROR_ENCODING_PROBLEM;
	}
	int y;
	/*for (y = H - 1; y >= 0; y--) {
		int x;
		for (x = 0; x < W; x++)
			printf("%c",
				grid[W * y + x] ? '*' : ' ');
		printf("\n");
	}*/
	
	symbol->rows = H;
	symbol->width = W;
	
	for(y = H - 1; y >= 0; y--) {
		int x;
		for(x = 0; x < W; x++) {
			if(grid[W * y + x]) {
				symbol->encoded_data[(H - y) - 1][x] = '1'; }
			else {
				symbol->encoded_data[(H - y) - 1][x] = '0'; }
		}
		symbol->row_height[(H - y) - 1] = 1;
	}
	
	return 0;
}
