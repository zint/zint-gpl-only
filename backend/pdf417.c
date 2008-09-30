/* pdf417.c - Handles PDF417 stacked symbology */

/*  Zint - A barcode generating program using libpng
    Copyright (C) 2008 Robin Stuart <zint@hotmail.co.uk>
    Portions Copyright (C) 2004 Grandzebu
    Bug Fixes thanks to KL Chin <klchin@users.sourceforge.net>

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

/*  This code is adapted from "Code barre PDF 417 / PDF 417 barcode" v2.5.0
    which is Copyright (C) 2004 (Grandzebu).
    The original code can be downloaded from http://grandzebu.net/index.php */

/* NOTE: symbol->option_1 is used to specify the security level (i.e. control the
   number of check codewords)
   
   symbol->option_2 is used to adjust the width of the resulting symbol (i.e. the
   number of codeword columns not including row start and end data) */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "pdf417.h"
#include "common.h"
#include "large.h"

/* 
   Three figure numbers in comments give the location of command equivalents in the
   original Visual Basic source code file pdf417.frm
   this code retains some original (French) procedure and variable names to ease conversion */



int liste[2][1000]; /* global - okay, so I got _almost_ everything local! */

/* 866 */
int quelmode(char codeascii)
{
	int mode;
	mode = BYT;
	
	if((codeascii >= ' ') && (codeascii <= '~')) { mode = TEX; }
	if(codeascii == '\t') { mode = TEX; }
	if(codeascii == '\n') { mode = TEX; }
	if(codeascii == 13) { mode = TEX; }
	if((codeascii >= '0') && (codeascii <= '9')) { mode = NUM; }
	/* 876 */
	return mode;
}

/* 844 */
void regroupe(int *indexliste)
{
	int i, j;
	
	/* bring together same type blocks */
	if(*(indexliste) > 1) {
		i = 1;
		while(i < *(indexliste)) {
			if(liste[1][i - 1] == liste[1][i]) {
				/* bring together */
				liste[0][i - 1] = liste[0][i - 1] + liste[0][i];
				j = i + 1;
				
				/* decreace the list */
				while(j < *(indexliste)) {
					liste[0][j - 1] = liste[0][j];
					liste[1][j - 1] = liste[1][j];
					j++;
				}
				*(indexliste) = *(indexliste) - 1;
				i--;
			}
			i++;
		}
	}
	/* 865 */
}

/* 478 */
void pdfsmooth(int *indexliste)
{
	int i, this, last, next, length;
	
	for(i = 0; i < *(indexliste); i++) {
		this = liste[1][i];
		length = liste[0][i];
		if(i != 0) { last = liste[1][i - 1]; } else { last = FALSE; }
		if(i != *(indexliste) - 1) { next = liste[1][i + 1]; } else { next = FALSE; }
		
		if(this == NUM) {
			if(i == 0) { /* first block */
				if(*(indexliste) > 1) { /* and there are others */
					if((next == TEX) && (length < 8)) { liste[1][i] = TEX;}
					if((next == BYT) && (length == 1)) { liste[1][i] = BYT; }
				}
			} else {
				if(i == *(indexliste) - 1) { /* last block */
					if((last == TEX) && (length < 7)) { liste[1][i] = TEX; }
					if((last == BYT) && (length == 1)) { liste[1][i] = BYT; }
				} else { /* not first or last block */
					if(((last == BYT) && (next == BYT)) && (length < 4)) { liste[1][i] = BYT; }
					if(((last == BYT) && (next == TEX)) && (length < 4)) { liste[1][i] = TEX; }
					if(((last == TEX) && (next == BYT)) && (length < 5)) { liste[1][i] = TEX; }
					if(((last == TEX) && (next == TEX)) && (length < 8)) { liste[1][i] = TEX; }
				}
			}
		}
	}
	regroupe(indexliste);
	/* 520 */
	for(i = 0; i < *(indexliste); i++) {
		this = liste[1][i];
		length = liste[0][i];
		if(i != 0) { last = liste[1][i - 1]; } else { last = FALSE; }
		if(i != *(indexliste) - 1) { next = liste[1][i + 1]; } else { next = FALSE; }
		
		if((this == TEX) && (i > 0)) { /* not the first */
			if(i == *(indexliste) - 1) { /* the last one */
				if((last == BYT) && (length == 1)) { liste[1][i] = BYT; }
			} else { /* not the last one */
				if(((last == BYT) && (next == BYT)) && (length < 5)) { liste[1][i] = BYT; }
				if((((last == BYT) && (next != BYT)) || ((last != BYT) && (next == BYT))) && (length < 3)) {
					liste[1][i] = BYT;
				}
			}
		}
	}
	/* 540 */
	regroupe(indexliste);
}

/* 547 */
void textprocess(int *chainemc, int *mclength, char chaine[], int start, int length, int block)
{
	int j, indexlistet, curtable, listet[2][5000], chainet[5000], wnet;
	char codeascii;
	
	codeascii = 0;
	wnet = 0;
	
	for(j = 0; j < 1000; j++) {
		listet[0][j] = 0;
	}
	/* listet will contain the table numbers and the value of each characters */
	for(indexlistet = 0; indexlistet < length; indexlistet++) {
		codeascii = chaine[start + indexlistet];
		switch(codeascii) {
			case '\t': listet[0][indexlistet] = 12; listet[1][indexlistet] = 12; break;
			case '\n': listet[0][indexlistet] = 8; listet[1][indexlistet] = 15; break;
			case 13: listet[0][indexlistet] = 12; listet[1][indexlistet] = 11; break;
			default: listet[0][indexlistet] = asciix[codeascii - 32];
			listet[1][indexlistet] = asciiy[codeascii - 32]; break;
		}
	}

	/* 570 */
	curtable = 1; /* default table */
	for(j = 0; j < length; j++) {
		if(listet[0][j] & curtable) { /* The character is in the current table */
			chainet[wnet] = listet[1][j];
			wnet++;
		} else { /* Obliged to change table */
			int flag = FALSE; /* True if we change table for only one character */
			if (j == (length - 1)) {
				flag = TRUE;
			} else {
				if(!(listet[0][j] & listet[0][j + 1])) { flag = TRUE; }
			}
						
			if (flag) { /* we change only one character - look for temporary switch */
				if((listet[0][j] & 1) && (curtable == 2)) { /* T_UPP */
					chainet[wnet] = 27;
					chainet[wnet + 1] = listet[1][j];
					wnet += 2;
				}
				if(listet[0][j] & 8) { /* T_PUN */
					chainet[wnet] = 29;
					chainet[wnet + 1] = listet[1][j];
					wnet += 2;
				}
				if(!(((listet[0][j] & 1) && (curtable == 2)) || (listet[0][j] & 8))) {
					/* No temporary switch available */
					flag = FALSE;
				}
			}
						
			/* 599 */
			if (!(flag)) {
				int newtable;
							
				if(j == (length - 1)) {
					newtable = listet[0][j];
				} else {
					if(!(listet[0][j] & listet[0][j + 1])) {
						newtable = listet[0][j];
					} else {
						newtable = listet[0][j] & listet[0][j + 1];
					}
				}
							
				/* Maintain the first if several tables are possible */
				switch (newtable) {
					case 3:
					case 5:
					case 7:
					case 9:
					case 11:
					case 13:
					case 15:
						newtable = 1; break;
					case 6:
					case 10:
					case 14:
						newtable = 2; break;
					case 12:
						newtable = 4; break;
				}
							
				/* 619 - select the switch */
				switch (curtable) {
					case 1: 
						switch (newtable) {
							case 2: chainet[wnet] = 27; wnet++; break;
							case 4: chainet[wnet] = 28; wnet++; break;
							case 8: chainet[wnet] = 28; wnet++; chainet[wnet] = 25; wnet++;  break;
						} break;
					case 2: 
						switch (newtable) {
							case 1: chainet[wnet] = 28; wnet++; chainet[wnet] = 28; wnet++;  break;
							case 4: chainet[wnet] = 28; wnet++;  break;
							case 8: chainet[wnet] = 28; wnet++; chainet[wnet] = 25; wnet++;  break;
						} break;
					case 4:
						switch (newtable) {
							case 1: chainet[wnet] = 28; wnet++;  break;
							case 2: chainet[wnet] = 27; wnet++;  break;
							case 8: chainet[wnet] = 25; wnet++;  break;
						} break;
					case 8:
						switch (newtable) {
							case 1: chainet[wnet] = 29; wnet++;  break;
							case 2: chainet[wnet] = 29; wnet++; chainet[wnet] = 27; wnet++;  break;
							case 4: chainet[wnet] = 29; wnet++; chainet[wnet] = 28; wnet++;  break;
						} break;
				}
				curtable = newtable;
				/* 659 - at last we add the character */
				chainet[wnet] = listet[1][j];
				wnet++;
			}
		}
	}
	
	/* 663 */
	if ((wnet % 2) > 0) {
		chainet[wnet] = 29;
		wnet++;
	}

	/* Now translate the string chainet into codewords */
	if (block > 0) {
		chainemc[*(mclength)] = 900;
		*(mclength) = *(mclength) + 1;
	}
	
	for(j = 0; j < wnet; j+= 2) {
		int cw_number;
		
		cw_number = (30 * chainet[j]) + chainet[j + 1];
		chainemc[*(mclength)] = cw_number;
		*(mclength) = *(mclength) + 1;
		
	}
}

/* 671 */
void byteprocess(int *chainemc, int *mclength, unsigned char chaine[], int start, int length, int block)
{
	
	int i, j, k, l, longueur;
	short int accum[112], x_reg[112], y_reg[112];
	
	if(length == 1) {
		chainemc[*(mclength)] = 913;
		chainemc[*(mclength) + 1] = chaine[start];
		*(mclength) = *(mclength) + 2;
	} else {
		/* select the switch for multiple of 6 bytes */
		if (length % 6 == 0) {
			chainemc[*(mclength)] = 924;
			*(mclength) = *(mclength) + 1;
		} else {
			chainemc[*(mclength)] = 901;
			*(mclength) = *(mclength) + 1;
		}
		
		j = 0;
		while(j < length) {
			longueur = length - j;
			
			if (longueur >= 6) { /* Take groups of 6 */
				
				int cw[5];
				
				for(i = 0; i < 112; i++) {
					accum[i] = 0;
					x_reg[i] = 0;
					y_reg[i] = 0;
				}
				
				longueur = 6;
				for(k = 0; k < longueur; k++) {
					for(i = 0; i < 8; i++) {
						shiftup(y_reg);
					}
						
					if((chaine[start + j + k] & 0x80) != 0) { y_reg[7] = 1; }
					if((chaine[start + j + k] & 0x40) != 0) { y_reg[6] = 1; }
					if((chaine[start + j + k] & 0x20) != 0) { y_reg[5] = 1; }
					if((chaine[start + j + k] & 0x10) != 0) { y_reg[4] = 1; }
					if((chaine[start + j + k] & 0x08) != 0) { y_reg[3] = 1; }
					if((chaine[start + j + k] & 0x04) != 0) { y_reg[2] = 1; }
					if((chaine[start + j + k] & 0x02) != 0) { y_reg[1] = 1; }
					if((chaine[start + j + k] & 0x01) != 0) { y_reg[0] = 1; }
					
				}
				
				for(l = 0; l < 4; l++) {
				
					for(i = 0; i < 112; i++) {
						accum[i] = y_reg[i];
						y_reg[i] = 0;
						x_reg[i] = 0;
					}
					x_reg[101] = 1;
					x_reg[100] = 1;
					x_reg[99] = 1;
					x_reg[94] = 1;
					for(i = 92; i >= 0; i--) {
						y_reg[i] = islarger(accum, x_reg);
						if(y_reg[i] == 1) {
							binary_subtract(accum, x_reg);
						}
						shiftdown(x_reg);
					}
		
					cw[l] =   (accum[9] * 512) + (accum[8] * 256) +
							(accum[7] * 128) + (accum[6] * 64) + (accum[5] * 32) +
							(accum[4] * 16) + (accum[3] * 8) + (accum[2] * 4) +
							(accum[1] * 2) + accum[0];
					
				}
				
				cw[4] =   (y_reg[9] * 512) + (y_reg[8] * 256) +
						(y_reg[7] * 128) + (y_reg[6] * 64) + (y_reg[5] * 32) +
						(y_reg[4] * 16) + (y_reg[3] * 8) + (y_reg[2] * 4) +
						(y_reg[1] * 2) + y_reg[0];
				
				for(i = 0; i < 5; i++) {
					chainemc[*(mclength)] = cw[4 - i];
					*(mclength) = *(mclength) + 1;
				}
				
			} else {
				/* If there remains a group of less than 6 bytes */
				for(k = 0; k < longueur; k++) {
					chainemc[*(mclength)] = chaine[start + j + k];
					*(mclength) = *(mclength) + 1;
				}
			}
			j += longueur;
			
		}
	}
}

/* 712 */
void numbprocess(int *chainemc, int *mclength, char chaine[], int start, int length, int block)
{
	int j, loop, longueur, dummy[100], dumlength, diviseur, nombre;
	char chainemod[50], chainemult[100], temp;
	
	strcpy(chainemod, "");
	for(loop = 0; loop <= 50; loop++) {
		dummy[loop] = 0;
	}
	
	chainemc[*(mclength)] = 902;
	*(mclength) = *(mclength) + 1;

	j = 0;
	while(j < length) {
		dumlength = 0;
		strcpy(chainemod, "");
		longueur = length - j;
		if(longueur > 44) { longueur = 44; }
		concat(chainemod, "1");
		for(loop = 1; loop <= longueur; loop++) {
			chainemod[loop] = chaine[start + loop + j - 1];
		}
		chainemod[longueur + 1] = '\0';
		do {
			diviseur = 900;
			
			/* 877 - gosub Modulo */
			strcpy(chainemult, "");
			nombre = 0;
			while(strlen(chainemod) != 0) {
				nombre *= 10;
				nombre += ctoi(chainemod[0]);
				for(loop = 0; loop < strlen(chainemod); loop++) {
					chainemod[loop] = chainemod[loop + 1];
				}
				if (nombre < diviseur) {
					if (strlen(chainemult) != 0) { concat(chainemult, "0"); }
				} else {
					temp = (nombre / diviseur) + '0';
					chainemult[strlen(chainemult) + 1] = '\0';
					chainemult[strlen(chainemult)] = temp;
				}
				nombre = nombre % diviseur;
			}
			diviseur = nombre;
			/* return to 723 */
			
			for(loop = dumlength; loop > 0; loop--) {
				dummy[loop] = dummy[loop - 1];
			}
			dummy[0] = diviseur;
			dumlength++;
			strcpy(chainemod, chainemult);
		} while(strlen(chainemult) != 0);
		for(loop = 0; loop < dumlength; loop++) {
			chainemc[*(mclength)] = dummy[loop];
			*(mclength) = *(mclength) + 1;
		}
		j += longueur;
	}
}

/* 366 */
int pdf417(struct zint_symbol *symbol, unsigned char chaine[])
{
	int i, k, j, indexchaine, indexliste, mode, longueur, loop, mccorrection[520], offset;
	int total, chainemc[2700], mclength, c1, c2, c3, dummy[35], codeerr;
	char codebarre[100], pattern[580];

	codeerr = 0;

	/* 456 */
	indexliste = 0;
	indexchaine = 0;
	
	mode = quelmode(chaine[indexchaine]);
	
	for(i = 0; i < 1000; i++) {
		liste[0][i] = 0;
	}
	
	/* 463 */
	do {
		liste[1][indexliste] = mode;
		while ((liste[1][indexliste] == mode) && (indexchaine < ustrlen(chaine))) {
			liste[0][indexliste]++;
			indexchaine++;
			mode = quelmode(chaine[indexchaine]);
		}
		indexliste++;
	} while (indexchaine < ustrlen(chaine));
	
	/* 474 */
	pdfsmooth(&indexliste);
	
	/* 541 - now compress the data */
	indexchaine = 0;
	mclength = 0;
	for(i = 0; i < indexliste; i++) {
		switch(liste[1][i]) {
			case TEX: /* 547 - text mode */
				textprocess(chainemc, &mclength, chaine, indexchaine, liste[0][i], i);
				break;
			case BYT: /* 670 - octet stream mode */
				byteprocess(chainemc, &mclength, chaine, indexchaine, liste[0][i], i);
				break;
			case NUM: /* 712 - numeric mode */
				numbprocess(chainemc, &mclength, chaine, indexchaine, liste[0][i], i);
				break;
		}
		indexchaine = indexchaine + liste[0][i];
	}

	/* 752 - Now take care of the number of CWs per row */
	if (symbol->option_1 < 0) {
		/* note that security level 8 is never used automatically */
		symbol->option_1 = 7;
		if(mclength <= 1280) { symbol->option_1 = 6; }
		if(mclength <= 640) { symbol->option_1 = 5; }
		if(mclength <= 320) { symbol->option_1 = 4; }
		if(mclength <= 160) { symbol->option_1 = 3; }
		if(mclength <= 40) { symbol->option_1 = 2; }
	}
	k = 1;
	for(loop = 1; loop <= (symbol->option_1 + 1); loop++)
	{
		k *= 2;
	}

	longueur = mclength;
	if(symbol->option_2 > 30) { symbol->option_2 = 30; }
	if(symbol->option_2 < 1) {
		/* This is a much more simple formula to Grand Zebu's - 
		   it does not try to make the symbol square */
		symbol->option_2 = 0.5 + sqrt((longueur + k) / 3);
	}
	if(((longueur + k) / symbol->option_2) > 90) {
		/* stop the symbol from becoming too high */
		symbol->option_2 = symbol->option_2 + 1;
	}

	/* Reduce the correction level if there isn't room */
	/* while((longueur + k > PDF_MAX) && (symbol->option_1 > 0)) {
		symbol->option_1 = symbol->option_1 - 1;
		for(loop = 0; loop <= (symbol->option_1 + 1); loop++)
		{
			k *= 2;
		}
	} */
	/* this bit of the code would allow Zint to happily encode 2698 code words with
	only 2 check digits, so I have abandoned it! - Zint now insists on a proportional
	amount of check data unless overruled by the user */
	
	if(longueur + k > symbol->option_3) {
		return 2;
	}
	if(((longueur + k) / symbol->option_2) > 90) {
		return 4;
	}
	
	/* 781 - Padding calculation */
	longueur = mclength + 1 + k;
	i = 0;
	if ((longueur / symbol->option_2) < 3) {
		i = (symbol->option_2 * 3) - longueur; /* A bar code must have at least three rows */
	} else {
		if((longueur % symbol->option_2) > 0) { i = symbol->option_2 - (longueur % symbol->option_2); }
	}
	/* We add the padding */
	while (i > 0) {
		chainemc[mclength] = 900;
		mclength++;
		i--;
	}
	/* we add the length descriptor */
	for(i = mclength; i > 0; i--) {
		chainemc[i] = chainemc[i - 1];
	}
	chainemc[0] = mclength + 1;
	mclength++;

	/* 796 - we now take care of the Reed Solomon codes */
	switch(symbol->option_1) {
		case 1: offset = 2; break;
		case 2: offset = 6; break;
		case 3: offset = 14; break;
		case 4: offset = 30; break;
		case 5: offset = 62; break;
		case 6: offset = 126; break;
		case 7: offset = 254; break;
		case 8: offset = 510; break;
		default: offset = 0; break;
	}

	longueur = mclength;
	for(loop = 0; loop < 520; loop++) {
		mccorrection[loop] = 0;
	}
	total = 0;
	for(i = 0; i < longueur; i++) {
		total = (chainemc[i] + mccorrection[k - 1]) % 929;
		for(j = k - 1; j >= 0; j--) {
			if(j == 0) {
				mccorrection[j] = (929 - (total * coefrs[offset + j]) % 929) % 929;
			} else {
				mccorrection[j] = (mccorrection[j - 1] + 929 - (total * coefrs[offset + j]) % 929) % 929;
			}
		}
	}
	
	for(j = 0; j < k; j++) {
		if(mccorrection[j] != 0) { mccorrection[j] = 929 - mccorrection[j]; }
	}
	/* we add these codes to the string */
	for(i = k - 1; i >= 0; i--) {
		chainemc[mclength] = mccorrection[i];
		mclength++;
	}
	
	/* 818 - The CW string is finished */
	c1 = (mclength / symbol->option_2 - 1) / 3;
	c2 = symbol->option_1 * 3 + (mclength / symbol->option_2 - 1) % 3;
	c3 = symbol->option_2 - 1;
	
	/* we now encode each row */
	for(i = 0; i <= (mclength / symbol->option_2) - 1; i++) {
		for(j = 0; j < symbol->option_2 ; j++) {
			dummy[j + 1] = chainemc[i * symbol->option_2 + j];
		}
		k = (i / 3) * 30;
		switch(i % 3) {
				/* follows this pattern from US Patent 5,243,655: 
			Row 0: L0 (row #, # of rows)         R0 (row #, # of columns)
			Row 1: L1 (row #, security level)    R1 (row #, # of rows)
			Row 2: L2 (row #, # of columns)      R2 (row #, security level)
			Row 3: L3 (row #, # of rows)         R3 (row #, # of columns)
				etc. */
			case 0:
				dummy[0] = k + c1;
				dummy[symbol->option_2 + 1] = k + c3;
				break;
			case 1:
				dummy[0] = k + c2;
				dummy[symbol->option_2 + 1] = k + c1;
				break;
			case 2:
				dummy[0] = k + c3;
				dummy[symbol->option_2 + 1] = k + c2;
				break;
		}
		strcpy(codebarre, "+*"); /* Start with a start char and a separator */
		if(symbol->symbology == BARCODE_PDF417TRUNC) {
			/* truncated - so same as before except knock off the last 5 chars */
			for(j = 0; j <= symbol->option_2;  j++) {
				switch(i % 3) {
					case 1: offset = 929; break;
					case 2: offset = 1858; break;
					default: offset = 0; break;
				}
				concat(codebarre, codagemc[offset + dummy[j]]);
				concat(codebarre, "*");
			}
		} else {
			/* normal PDF417 symbol */
			for(j = 0; j <= symbol->option_2 + 1;  j++) {
				switch(i % 3) {
					case 1: offset = 929; /* cluster(3) */ break;
					case 2: offset = 1858; /* cluster(6) */ break;
					default: offset = 0; /* cluster(0) */ break;
				}
				concat(codebarre, codagemc[offset + dummy[j]]);
				concat(codebarre, "*");
			}
			concat(codebarre, "-");
		}
		
		strcpy(pattern, "");
		for(loop = 0; loop < strlen(codebarre); loop++) {
			lookup(BRSET, PDFttf, codebarre[loop], pattern);
		}
		for(loop = 0; loop < strlen(pattern); loop++) {
			symbol->encoded_data[i][loop] = pattern[loop];
		}
		symbol->row_height[i] = 3;
	}
	symbol->rows = (mclength / symbol->option_2);
	symbol->width = strlen(pattern);
	
	/* 843 */
	return codeerr;
}

/* 345 */
int pdf417enc(struct zint_symbol *symbol, unsigned char source[])
{
	int codeerr, errno;

	errno = 0;
	
	if((symbol->option_1 < -1) || (symbol->option_1 > 8)) {
		strcpy(symbol->errtxt, "warning: security value out of range");
		symbol->option_1 = -1;
		errno = WARN_INVALID_OPTION;
	}
	if((symbol->option_2 < 0) || (symbol->option_2 > 30)) {
		strcpy(symbol->errtxt, "warning: number of columns out of range");
		symbol->option_2 = 0;
		errno = WARN_INVALID_OPTION;
	}

	/* 349 */
	codeerr = pdf417(symbol, source);
	
	/* 352 */
	if(codeerr != 0) {
		switch(codeerr) {
			case 1:
				strcpy(symbol->errtxt, "error: no such file or file unreadable (PDF error 1)");
				errno = ERROR_INVALID_OPTION;
				break;
			case 2:
				strcpy(symbol->errtxt, "error: input string too long (PDF error 2)");
				errno = ERROR_TOO_LONG;
				break;
			case 3:
				strcpy(symbol->errtxt, "error: number of codewords per row too small (PDF error 3)");
				errno = WARN_INVALID_OPTION;
				break;
			case 4:
				strcpy(symbol->errtxt, "error: data too long for specified number of columns (PDF error 4)");
				errno = ERROR_TOO_LONG;
				break;
			default:
				strcpy(symbol->errtxt, "error: something strange happened");
				errno = ERROR_ENCODING_PROBLEM;
				break;
		}
	}
	
	/* 364 */
	return errno;
}


int micro_pdf417(struct zint_symbol *symbol, unsigned char chaine[])
{ /* like PDF417 only much smaller! */
	
	int i, k, j, indexchaine, indexliste, mode, longueur, mccorrection[50], offset;
	int total, chainemc[2700], mclength, dummy[5], codeerr;
	char codebarre[100], pattern[580];
	int variant, LeftRAPStart, CentreRAPStart, RightRAPStart, StartCluster;
	int LeftRAP, CentreRAP, RightRAP, Cluster, writer, flip, loop;

	/* Encoding starts out the same as PDF417, so use the same code */
	codeerr = 0;
	
	/* 456 */
	indexliste = 0;
	indexchaine = 0;
	
	mode = quelmode(chaine[indexchaine]);
	
	for(i = 0; i < 1000; i++) {
		liste[0][i] = 0;
	}
	
	/* 463 */
	do {
		liste[1][indexliste] = mode;
		while ((liste[1][indexliste] == mode) && (indexchaine < ustrlen(chaine))) {
			liste[0][indexliste]++;
			indexchaine++;
			mode = quelmode(chaine[indexchaine]);
		}
		indexliste++;
	} while (indexchaine < ustrlen(chaine));
	
	/* 474 */
	pdfsmooth(&indexliste);
	
	/* 541 - now compress the data */
	indexchaine = 0;
	mclength = 0;
	for(i = 0; i < indexliste; i++) {
		switch(liste[1][i]) {
			case TEX: /* 547 - text mode */
				textprocess(chainemc, &mclength, chaine, indexchaine, liste[0][i], i);
				break;
			case BYT: /* 670 - octet stream mode */
				byteprocess(chainemc, &mclength, chaine, indexchaine, liste[0][i], i);
				break;
			case NUM: /* 712 - numeric mode */
				numbprocess(chainemc, &mclength, chaine, indexchaine, liste[0][i], i);
				break;
		}
		indexchaine = indexchaine + liste[0][i];
	}

	/* This is where it all changes! */
	
	if(mclength > 126) {
		strcpy(symbol->errtxt, "error: input data too long");
		return ERROR_TOO_LONG;
	}
	if(symbol->option_2 > 4) {
		strcpy(symbol->errtxt, "warning: specified width out of range");
		symbol->option_2 = 0;
		codeerr = WARN_INVALID_OPTION;
	}
	
	/* Now figure out which variant of the symbol to use and load values accordingly */
	
	variant = 0;
	
	if((symbol->option_2 == 1) && (mclength > 20)) {
		/* the user specified 1 column but the data doesn't fit - go to automatic */
		symbol->option_2 = 0;
		strcpy(symbol->errtxt, "warning: specified symbol size too small for data");
		codeerr = WARN_INVALID_OPTION;
	}
	
	if((symbol->option_2 == 2) && (mclength > 37)) {
		/* the user specified 2 columns but the data doesn't fit - go to automatic */
		symbol->option_2 = 0;
		strcpy(symbol->errtxt, "warning: specified symbol size too small for data");
		codeerr = WARN_INVALID_OPTION;
	}
	
	if((symbol->option_2 == 3) && (mclength > 82)) {
		/* the user specified 3 columns but the data doesn't fit - go to automatic */
		symbol->option_2 = 0;
		strcpy(symbol->errtxt, "warning: specified symbol size too small for data");
		codeerr = WARN_INVALID_OPTION;
	}
	
	if(symbol->option_2 == 1) {
		/* the user specified 1 column and the data does fit */
		variant = 6;
		if(mclength <= 16) { variant = 5; }
		if(mclength <= 12) { variant = 4; }
		if(mclength <= 10) { variant = 3; }
		if(mclength <= 7) { variant = 2; }
		if(mclength <= 4) { variant = 1; }
	}

	if(symbol->option_2 == 2) {
		/* the user specified 2 columns and the data does fit */
		variant = 13;
		if(mclength <= 33) { variant = 12; }
		if(mclength <= 29) { variant = 11; }
		if(mclength <= 24) { variant = 10; }
		if(mclength <= 19) { variant = 9; }
		if(mclength <= 13) { variant = 8; }
		if(mclength <= 8) { variant = 7; }
	}

	if(symbol->option_2 == 3) {
		/* the user specified 3 columns and the data does fit */
		variant = 23;
		if(mclength <= 70) { variant = 22; }
		if(mclength <= 58) { variant = 21; }
		if(mclength <= 46) { variant = 20; }
		if(mclength <= 34) { variant = 19; }
		if(mclength <= 24) { variant = 18; }
		if(mclength <= 18) { variant = 17; }
		if(mclength <= 14) { variant = 16; }
		if(mclength <= 10) { variant = 15; }
		if(mclength <= 6) { variant = 14; }
	}

	if(symbol->option_2 == 4) {
		/* the user specified 4 columns and the data does fit */
		variant = 34;
		if(mclength <= 108) { variant = 33; }
		if(mclength <= 90) { variant = 32; }
		if(mclength <= 72) { variant = 31; }
		if(mclength <= 54) { variant = 30; }
		if(mclength <= 39) { variant = 29; }
		if(mclength <= 30) { variant = 28; }
		if(mclength <= 24) { variant = 27; }
		if(mclength <= 18) { variant = 26; }
		if(mclength <= 12) { variant = 25; }
		if(mclength <= 8) { variant = 24; }
	}

	if(variant == 0) {
		/* Zint can choose automatically from all available variations */
		for(i = 27; i >= 0; i--) {
			
			if(MicroAutosize[i] >= mclength) {
				variant = MicroAutosize[i + 28];
			}
		}
	}
	
	/* Now we have the variant we can load the data */
	variant --;
	symbol->option_2 = MicroVariants[variant]; /* columns */
	symbol->rows = MicroVariants[variant + 34]; /* rows */
	k = MicroVariants[variant + 68]; /* number of EC CWs */
	longueur = (symbol->option_2 * symbol->rows) - k; /* number of non-EC CWs */
	i = longueur - mclength; /* amount of padding required */
	offset = MicroVariants[variant + 102]; /* coefficient offset */
	
	/* We add the padding */
	while (i > 0) {
		chainemc[mclength] = 900;
		mclength++;
		i--;
	}
	
	/* Reed-Solomon error correction */
	longueur = mclength;
	for(loop = 0; loop < 50; loop++) {
		mccorrection[loop] = 0;
	}
	total = 0;
	for(i = 0; i < longueur; i++) {
		total = (chainemc[i] + mccorrection[k - 1]) % 929;
		for(j = k - 1; j >= 0; j--) {
			if(j == 0) {
				mccorrection[j] = (929 - (total * Microcoeffs[offset + j]) % 929) % 929;
			} else {
				mccorrection[j] = (mccorrection[j - 1] + 929 - (total * Microcoeffs[offset + j]) % 929) % 929;
			}
		}
	}
	
	for(j = 0; j < k; j++) {
		if(mccorrection[j] != 0) { mccorrection[j] = 929 - mccorrection[j]; }
	}
	/* we add these codes to the string */
	for(i = k - 1; i >= 0; i--) {
		chainemc[mclength] = mccorrection[i];
		mclength++;
	}
	
	/* Now get the RAP (Row Address Pattern) start values */
	LeftRAPStart = RAPTable[variant];
	CentreRAPStart = RAPTable[variant + 34];
	RightRAPStart = RAPTable[variant + 68];
	StartCluster = RAPTable[variant + 102] / 3;
	
	/* That's all values loaded, get on with the encoding */
	
	LeftRAP = LeftRAPStart;
	CentreRAP = CentreRAPStart;
	RightRAP = RightRAPStart;
	Cluster = StartCluster; /* Cluster can be 0, 1 or 2 for Cluster(0), Cluster(3) and Cluster(6) */
	
	for(i = 0; i < symbol->rows; i++) {
		strcpy(codebarre, "");
		offset = 929 * Cluster;
		for(j = 0; j < 5; j++) {
			dummy[j] = 0;
		}
		for(j = 0; j < symbol->option_2 ; j++) {
			dummy[j + 1] = chainemc[i * symbol->option_2 + j];
		}
		/* Copy the data into codebarre */
		concat(codebarre, RAPLR[LeftRAP]);
		concat(codebarre, "1");
		concat(codebarre, codagemc[offset + dummy[1]]);
		concat(codebarre, "1");
		if(symbol->option_2 == 3) {
			concat(codebarre, RAPC[CentreRAP]);
		}
		if(dummy[2] != 0) {
			concat(codebarre, "1");
			concat(codebarre, codagemc[offset + dummy[2]]);
			concat(codebarre, "1");
		}
		if(symbol->option_2 == 4) {
			concat(codebarre, RAPC[CentreRAP]);
		}
		if(dummy[3] != 0) {
			concat(codebarre, "1");
			concat(codebarre, codagemc[offset + dummy[3]]);
			concat(codebarre, "1");
		}
		if(dummy[4] != 0) {
			concat(codebarre, "1");
			concat(codebarre, codagemc[offset + dummy[4]]);
			concat(codebarre, "1");
		}
		concat(codebarre, RAPLR[RightRAP]);
		concat(codebarre, "1"); /* stop */
		
		/* Now codebarre is a mixture of letters and numbers */
		
		writer = 0;
		flip = 1;
		strcpy(pattern, "");
		for(loop = 0; loop < strlen(codebarre); loop++) {
			if((codebarre[loop] >= '0') && (codebarre[loop] <= '9')) {
				for(k = 0; k < ctoi(codebarre[loop]); k++) {
					if(flip == 0) {
						pattern[writer] = '0';
					} else {
						pattern[writer] = '1';
					}
					writer++;
				}
				pattern[writer] = '\0';
				if(flip == 0) {
					flip = 1;
				} else {
					flip = 0;
				}
			} else {
				lookup(BRSET, PDFttf, codebarre[loop], pattern);
				writer += 5;
			}
		}
		symbol->width = writer;
		
		/* so now pattern[] holds the string of '1's and '0's. - copy this to the symbol */
		for(loop = 0; loop < strlen(pattern); loop++) {
			symbol->encoded_data[i][loop] = pattern[loop];
		}
		symbol->row_height[i] = 2;
		
		/* Set up RAPs and Cluster for next row */
		LeftRAP++;
		CentreRAP++;
		RightRAP++;
		Cluster++;
		
		if(LeftRAP == 53) {
			LeftRAP = 1;
		}
		if(CentreRAP == 53) {
			CentreRAP = 1;
		}
		if(RightRAP == 53) {
			RightRAP = 1;
		}
		if(Cluster == 3) {
			Cluster = 0;
		}
	}
	
	return codeerr;
}

