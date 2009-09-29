/*  gridmtx.c - Grid Matrix

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

/* This file impliments Grid Matrix as specified in
   AIM Global Document Number AIMD014 Rev. 1.63 Revised 9 Dec 2008 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _MSC_VER
#include <malloc.h> 
#endif
#include "common.h"
#include "reedsol.h"
#include "gridmtx.h"

int grid_matrix(struct zint_symbol *symbol, unsigned char source[])
{
	
	strcpy(symbol->errtxt, "Grid Matrix not yet implemented");
	return ERROR_INVALID_OPTION;
}