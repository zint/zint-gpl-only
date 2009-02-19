/* common.h - Header for all common functions in common.c */

/*
    libzint - the open source barcode library
    Copyright (C) 2008 Robin Stuart <robin@zint.org.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/* Used in some logic */
#define FALSE		0
#define TRUE		1

/* The most commonly used set */
#define NESET		"0123456789"

#include "zint.h"

int ustrlen(unsigned char source[]);
void ustrcpy(unsigned char target[], unsigned char source[]);
void concat(char dest[], char source[]);
void uconcat(unsigned char dest[], unsigned char source[]);
int ctoi(char source);
char itoc(int source);
void to_upper(unsigned char source[]);
int is_sane(char test_string[], unsigned char source[]);
void lookup(char set_string[], char *table[], char data, char dest[]);
int posn(char set_string[], char data);
void expand(struct zint_symbol *symbol, char data[]);
int is_stackable(int symbology);
int roundup(float input);

