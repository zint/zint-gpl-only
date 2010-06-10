/* large.h - Handles binary manipulation of large numbers */

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
#ifndef __LARGE_H
#define __LARGE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void binary_load(short int reg[], char data[], const unsigned int src_len);
extern void binary_add(short int accumulator[], short int input_buffer[]);
extern void binary_subtract(short int accumulator[], short int input_buffer[]);
extern void shiftdown(short int buffer[]);
extern void shiftup(short int buffer[]);
extern short int islarger(short int accum[], short int reg[]);
extern void hex_dump(short int input_buffer[]);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LARGE_H */
