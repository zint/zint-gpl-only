/* common.h - Header for all common functions in common.c */

/*
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

/* Used in some logic */
#ifndef __COMMON_H
#define __COMMON_H

#include <stdint.h>

#ifndef FALSE
#define FALSE		0
#endif

#ifndef TRUE
#define TRUE		1
#endif

/* The most commonly used set */
#define NEON	"0123456789"

#include "zint.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern int ustrlen(const uint8_t source[]);
extern void ustrcpy(uint8_t target[], const uint8_t source[]);
extern void concat(char dest[], const char source[]);
extern void uconcat(uint8_t dest[], const uint8_t source[]);
extern int ctoi(char source);
extern char itoc(int source);
extern void to_upper(uint8_t source[]);
extern int is_sane(char test_string[], uint8_t source[], int length);
extern void lookup(char set_string[], const char *table[], char data, char dest[]);
extern int posn(char set_string[], char data);
extern void expand(struct zint_symbol *symbol, char data[]);
extern int is_stackable(int symbology);
extern int is_extendable(int symbology);
extern int roundup(float input);
extern int module_is_set(struct zint_symbol *symbol, int y_coord, int x_coord);
extern void set_module(struct zint_symbol *symbol, int y_coord, int x_coord);
extern void unset_module(struct zint_symbol *symbol, int y_coord, int x_coord);
extern int istwodigits(uint8_t source[], int position);
extern float froundup(float input);
extern int parunmodd(uint8_t llyth);
extern int latin1_process(struct zint_symbol *symbol, uint8_t source[], uint8_t preprocessed[], int *length);
extern int utf8toutf16(struct zint_symbol *symbol, uint8_t source[], int vals[], int *length);

static inline void bscan(char *binary, int data, int h)
{
	for (; h; h>>=1) {
		concat(binary, data & h ? "1" : "0");
	}
}

#define nitems(a) (sizeof(a) / sizeof(a[0]))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __COMMON_H */
