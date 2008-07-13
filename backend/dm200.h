/** 
 *
 * IEC16022 bar code generation
 * Adrian Kennard, Andrews & Arnold Ltd
 * with help from Cliff Hones on the RS coding
 *
 * (c) 2004 Adrian Kennard, Andrews & Arnold Ltd
 * (c) 2006 Stefan Schmidt <stefan@datenfreihafen.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 *
 * Main encoding function
 * Returns the grid (malloced) containing the matrix. L corner at 0,0.
 * Takes suggested size in *Wptr, *Hptr, or 0,0. Fills in actual size.
 * Takes barcodelen and barcode to be encoded
 * Note, if *encodingptr is null, then fills with auto picked (malloced) 
 * encoding.
 * If lenp not null, then the length of encoded data before any final unlatch 
 * or pad is stored.
 * If maxp not null, then the max storage of this size code is stored
 * If eccp not null, then the number of ecc bytes used in this size is stored
 * Returns 0 on error (writes to stderr with details).
 *
 */

#ifndef __IEC16022ECC200_H
#define __IEC16022ECC200_H

unsigned char *iec16022ecc200(int *Wptr, int *Hptr, char **encodingptr,
			      int barcodelen, unsigned char *barcode,
			      int *lenp, int *maxp, int *eccp);
#define MAXBARCODE 3116

#endif				/* __IEC16022ECC200_H */
