/* plessey.c - Handles Plessey and MSI Plessey */

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
#include "common.h"


#define SSET	"0123456789ABCDEF"
static char *PlessTable[16] = {"13131313", "31131313", "13311313", "31311313", "13133113", "31133113",
	"13313113", "31313113", "13131331", "31131331", "13311331", "31311331", "13133131",
	"31133131", "13313131", "31313131"};

static char *MSITable[10] = {"12121212", "12121221", "12122112", "12122121", "12211212", "12211221",
	"12212112", "12212121", "21121212", "21121221"};


int plessey(struct zint_symbol *symbol, unsigned char source[])
{ /* Not MSI/Plessey but the older Plessey standard */

	unsigned int i, check;
	unsigned char *checkptr;
	static char grid[9] = {1,1,1,1,0,1,0,0,1};
	char dest[1000];
	int error_number;
	
	error_number = 0;
	strcpy(dest, "");
	
	if(ustrlen(source) > 65) {
		strcpy(symbol->errtxt, "Input too long [861]");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(SSET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data [862]");
		return error_number;
	}
	checkptr = calloc (1, ustrlen(source) * 4 + 8);

	/* Start character */
	concat(dest, "31311331");

	/* Data area */
	for(i = 0; i <= ustrlen(source); i++)
	{
		check = posn(SSET, source[i]);
		lookup(SSET, PlessTable, source[i], dest);
		checkptr[4*i] = check & 1;
		checkptr[4*i+1] = (check >> 1) & 1;
		checkptr[4*i+2] = (check >> 2) & 1;
		checkptr[4*i+3] = (check >> 3) & 1;
	}

	/* CRC check digit code adapted from code by Leonid A. Broukhis
	   used in GNU Barcode */

	for (i=0; i < 4*ustrlen(source); i++) {
		int j;
		if (checkptr[i])
			for (j = 0; j < 9; j++)
				checkptr[i+j] ^= grid[j];
	}

	for (i = 0; i < 8; i++) {
		switch(checkptr[ustrlen(source) * 4 + i])
		{
			case 0: concat(dest, "13"); break;
			case 1: concat(dest, "31"); break;
		}
	}

	/* Stop character */
	concat(dest, "331311313");
	
	expand(symbol, dest);
	strcpy(symbol->text, (char*)source);
	free(checkptr);
	return error_number;
}

int msi_plessey(struct zint_symbol *symbol, unsigned char source[])
{ /* Plain MSI Plessey - does not calculate any check character */

	unsigned int i;
	int error_number;
	char dest[1000];
	
	error_number = 0;
	strcpy(dest, "");

	if(ustrlen(source) > 55) {
		strcpy(symbol->errtxt, "Input too long [471]");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data [472]");
		return error_number;
	}

	/* start character */
	concat (dest, "21");

	for(i = 0; i <= ustrlen(source); i++)
	{
		lookup(NESET, MSITable, source[i], dest);
	}

	/* Stop character */
	concat (dest, "121");
	
	expand(symbol, dest);
	strcpy(symbol->text, (char*)source);
	return error_number;
}

int msi_plessey_mod10(struct zint_symbol *symbol, unsigned char source[])
{ /* MSI Plessey with Modulo 10 check digit - algorithm from Barcode Island
	http://www.barcodeisland.com/ */

	unsigned int i, wright, dau, pedwar, pump;
	char un[200], tri[200];
	int error_number, h;
	char dest[1000];
	
	error_number = 0;
	strcpy(dest, "");

	if(ustrlen(source) > 55) { 
		strcpy(symbol->errtxt, "Input too long [473]");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data [474]");
		return error_number;
	}

	/* start character */
	concat (dest, "21");

	/* draw data section */
	for(i = 0; i < ustrlen(source); i++)
	{
		lookup(NESET, MSITable, source[i], dest);
	}

	/* caluculate check digit */
	wright = 0;
	if((ustrlen(source)%2) == 0)
	{
		for(i = 1; i < ustrlen(source); i+=2)
		{
			un[wright] = source[i];
			wright ++;
		}
	}
	else
	{
		for(i = 0; i < ustrlen(source); i+=2)
		{
			un[wright] = source[i];
			wright ++;
		}
	}
	un[wright] = '\0';

	dau = atoi(un);
	dau *= 2;

	sprintf(tri,"%d",dau);

	pedwar = 0;
	for(i = 0; i < strlen(tri); i++)
	{
		pedwar += ctoi(tri[i]);
	}


	if((ustrlen(source)%2) == 0)
	{
		for(i = 0; i < ustrlen(source); i+=2)
		{
			pedwar += ctoi(source[i]);
		}
	}
	else
	{
		for(i = 1; i < ustrlen(source); i+=2)
		{
			pedwar += ctoi(source[i]);
		}
	}

	pump = (10 - pedwar%10);
	if(pump == 10)
	{
		pump = 0;
	}

	/* draw check digit */
	lookup(NESET, MSITable, itoc(pump), dest);

	/* Stop character */
	concat (dest, "121");
	
	h = ustrlen(source);
	source[h] = itoc(pump);
	source[h + 1] = '\0';
	expand(symbol, dest);
	strcpy(symbol->text, (char*)source);
	return error_number;
}

int msi_plessey_mod1010(struct zint_symbol *symbol, unsigned char source[])
{ /* MSI Plessey with two Modulo 10 check digits - algorithm from
	Barcode Island http://www.barcodeisland.com/ */

	unsigned int i, wright, dau, pedwar, pump, chwech;
	char un[200], tri[200];
	int error_number, h;
	char dest[1000];
	
	error_number = 0;
	strcpy(dest, "");
	
	if(ustrlen(source) > 55) { /* No Entry Stack Smashers! */
		strcpy(symbol->errtxt, "Input too long [475]");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if (error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data [476]");
		return error_number;
	}

	/* start character */
	concat (dest, "21");

	/* draw data section */
	for(i = 0; i < ustrlen(source); i++)
	{
		lookup(NESET, MSITable, source[i], dest);
	}

	/* calculate first check digit */
	wright = 0;
	if((ustrlen(source)%2) == 0)
	{
		for(i = 1; i < ustrlen(source); i+=2)
		{
			un[wright] = source[i];
			wright ++;
		}
	}
	else
	{
		for(i = 0; i < ustrlen(source); i+=2)
		{
			un[wright] = source[i];
			wright ++;
		}
	}
	un[wright] = '\0';

	dau = atoi(un);
	dau *= 2;

	sprintf(tri,"%d",dau);

	pedwar = 0;
	for(i = 0; i < strlen(tri); i++)
	{
		pedwar += ctoi(tri[i]);
	}


	if((ustrlen(source)%2) == 0)
	{
		for(i = 0; i < ustrlen(source); i+=2)
		{
			pedwar += ctoi(source[i]);
		}
	}
	else
	{
		for(i = 1; i < ustrlen(source); i+=2)
		{
			pedwar += ctoi(source[i]);
		}
	}

	pump = (10 - pedwar%10);
	if(pump == 10)
	{
		pump = 0;
	}

	/* calculate second check digit */
	wright = 0;
	if((ustrlen(source)%2) == 0)
	{
		for(i = 0; i < ustrlen(source); i+=2)
		{
			un[wright] = source[i];
			wright ++;
		}
	}
	else
	{
		for(i = 1; i < ustrlen(source); i+=2)
		{
			un[wright] = source[i];
			wright ++;
		}
	}
	un[wright] = itoc(pump);
	wright++;
	un[wright] = '\0';

	dau = atoi(un);
	dau *= 2;

	sprintf(tri,"%d",dau);

	pedwar = 0;
	for(i = 0; i < strlen(tri); i++)
	{
		pedwar += ctoi(tri[i]);
	}


	if((ustrlen(source)%2) == 0)
	{
		for(i = 1; i < ustrlen(source); i+=2)
		{
			pedwar += ctoi(source[i]);
		}
	}
	else
	{
		for(i = 0; i < ustrlen(source); i+=2)
		{
			pedwar += ctoi(source[i]);
		}
	}

	chwech = (10 - pedwar%10);
	if(chwech == 10)
	{
		chwech = 0;
	}

	/* Draw check digits */
	lookup(NESET, MSITable, itoc(pump), dest);
	lookup(NESET, MSITable, itoc(chwech), dest);

	/* Stop character */
	concat (dest, "121");
	
	h = ustrlen(source);
	source[h] = itoc(pump);
	source[h + 1] = itoc(chwech);
	source[h + 2] = '\0';
	expand(symbol, dest);
	strcpy(symbol->text, (char*)source);
	return error_number;
}


int msi_plessey_mod11(struct zint_symbol *symbol, unsigned char source[])
{
	/* Calculate a Modulo 11 check digit using the system discussed on Wikipedia - 
	see http://en.wikipedia.org/wiki/Talk:MSI_Barcode */
	/* uses the IBM weight system */
	
	int i, weight, x, check, h;
	int error_number;
	char dest[1000];
	
	error_number = 0;
	strcpy(dest, "");

	if(ustrlen(source) > 55) {
		strcpy(symbol->errtxt, "Input too long [477]");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if(error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data [478]");
		return error_number;
	}
	
	/* start character */
	concat (dest, "21");
	
	/* draw data section */
	for(i = 0; i < ustrlen(source); i++)
	{
		lookup(NESET, MSITable, source[i], dest);
	}

	/* calculate check digit */
	x = 0;
	weight = 2;
	for(i = (ustrlen(source) - 1); i >= 0; i--) {
		x += weight * ctoi(source[i]);
		weight++;
		if(weight > 7) {
			weight = 2;
		}
	}
	
	check = (11 - (x % 11)) % 11;
	if(check == 10) {
		lookup(NESET, MSITable, '1', dest);
		lookup(NESET, MSITable, '0', dest);
	} else {
		lookup(NESET, MSITable, itoc(check), dest);
	}
	
	h = ustrlen(source);
	if(check == 10) {
		source[h] = '1';
		source[h + 1] = '0';
		source[h + 2] = '\0';
	} else {
		source[h] = itoc(check);
		source[h + 1] = '\0';
	}
	
	/* stop character */
	concat (dest, "121");
	
	expand(symbol, dest);
	strcpy(symbol->text, (char*)source);
	return error_number;
}

int msi_plessey_mod1110(struct zint_symbol *symbol, unsigned char source[])
{
	/* Combining the Barcode Island and Wikipedia code */
	/* Verified against http://www.bokai.com/BarcodeJSP/applet/BarcodeSampleApplet.htm */
	/* Weighted using the IBM system */
	
	int i, weight, x, check, wright, dau, pedwar, pump, h;
	char un[200], tri[200];
	int error_number;
	char dest[1000];
	
	error_number = 0;
	strcpy(dest, "");

	if(ustrlen(source) > 55) {
		strcpy(symbol->errtxt, "Input too long [479]");
		return ERROR_TOO_LONG;
	}
	error_number = is_sane(NESET, source);
	if (error_number == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Invalid characters in data [47A]");
		return error_number;
	}
	
	/* start character */
	concat (dest, "21");
	
	/* draw data section */
	for(i = 0; i < ustrlen(source); i++)
	{
		lookup(NESET, MSITable, source[i], dest);
	}

	/* calculate first (mod 11) digit */
	wright = ustrlen(source);
	x = 0;
	weight = 2;
	for(i = (ustrlen(source) - 1); i >= 0; i--) {
		x += weight * ctoi(source[i]);
		weight++;
		if(weight > 7) {
			weight = 2;
		}
	}
	
	check = (11 - (x % 11)) % 11;
	if(check == 10) {
		lookup(NESET, MSITable, '1', dest);
		lookup(NESET, MSITable, '0', dest);
		source[wright] = '1';
		source[wright + 1] = '0';
		source[wright + 2] = '\0';
	} else {
		lookup(NESET, MSITable, itoc(check), dest);
		source[wright] = itoc(check);
		source[wright + 1] = '\0';
	}
	
	/* caluculate second (mod 10) check digit */
	wright = 0;
	if((ustrlen(source)%2) == 0)
	{
		for(i = 1; i < ustrlen(source); i+=2)
		{
			un[wright] = source[i];
			wright ++;
		}
	}
	else
	{
		for(i = 0; i < ustrlen(source); i+=2)
		{
			un[wright] = source[i];
			wright ++;
		}
	}
	un[wright] = '\0';

	dau = atoi(un);
	dau *= 2;

	sprintf(tri,"%d",dau);

	pedwar = 0;
	for(i = 0; i < strlen(tri); i++)
	{
		pedwar += ctoi(tri[i]);
	}


	if((ustrlen(source)%2) == 0)
	{
		for(i = 0; i < ustrlen(source); i+=2)
		{
			pedwar += ctoi(source[i]);
		}
	}
	else
	{
		for(i = 1; i < ustrlen(source); i+=2)
		{
			pedwar += ctoi(source[i]);
		}
	}

	pump = (10 - pedwar%10);
	if(pump == 10)
	{
		pump = 0;
	}

	/* draw check digit */
	lookup(NESET, MSITable, itoc(pump), dest);

	/* stop character */
	concat (dest, "121");
	
	h = ustrlen(source);
	source[h] = itoc(pump);
	source[h + 1] = '\0';

	expand(symbol, dest);
	strcpy(symbol->text, (char*)source);
	return error_number;
}

int msi_handle(struct zint_symbol *symbol, unsigned char source[]) {
	int error_number;

	error_number=0;

	if((symbol->option_2 < 0) || (symbol->option_2 > 4)) {
		symbol->option_2 = 0;
	}

	switch(symbol->option_2) {
		case 0: error_number = msi_plessey(symbol, source); break;
		case 1: error_number = msi_plessey_mod10(symbol, source); break;
		case 2: error_number = msi_plessey_mod1010(symbol, source); break;
		case 3: error_number = msi_plessey_mod11(symbol, source); break;
		case 4: error_number = msi_plessey_mod1110(symbol, source); break;
	}
	
	return error_number;
}
