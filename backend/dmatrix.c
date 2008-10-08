/* dmatrix.c - Handles Data Matrix 2-D symbology (IEC16022 ecc 200) */

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

#include <string.h>
#include "dm200.h"
#include "common.h"

int dmatrix(struct zint_symbol *symbol, unsigned char source[])
{
	/* In later versions of this code this procedure will redirect control
	dependent on the version of Data Matrix symbol required - for now it is
	just a place holder */
	int barcodelen;
	int error_number;
	
	barcodelen = ustrlen(source);
	if(barcodelen > 780) {
		strcpy(symbol->errtxt, "Input too long [711]");
		return ERROR_TOO_LONG;
	}

	error_number = iec16022ecc200(source, barcodelen, symbol);

	return error_number;
}
