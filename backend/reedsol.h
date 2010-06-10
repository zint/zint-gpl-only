/** 
 *
 * This is a simple Reed-Solomon encoder
 * (C) Cliff Hones 2004
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
 */

#ifndef __REEDSOL_H
#define __REEDSOL_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern void rs_init_gf(int poly);
extern void rs_init_code(int nsym, int index);
extern void rs_encode(int len, unsigned char *data, unsigned char *res);
extern void rs_encode_long(int len, unsigned int *data, unsigned int *res);
extern void rs_free(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif				/* __REEDSOL_H */
