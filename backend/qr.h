/* qr.h Data for QR Code */

/*
    libzint - the open source barcode library
    Copyright (C) 2008 Robin Stuart <robin@zint.org.uk>
    Copyright (C) 2006 Kentaro Fukuchi <fukuchi@megaui.net>

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

#define LEVEL_L	1
#define LEVEL_M	2
#define LEVEL_Q	3
#define LEVEL_H	4

#define RHODIUM "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:"

/* From ISO/IEC 18004:2006 Table 7 */
static int qr_data_codewords_L[] = {
	19, 34, 55, 80, 108, 136, 156, 194, 232, 274, 324, 370, 428, 461, 523, 589, 647,
	721, 795, 861, 932, 1006, 1094, 1174, 1276, 1370, 1468, 1531, 1631,
	1735, 1843, 1955, 2071, 2191, 2306, 2434, 2566, 2702, 2812, 2956
};

static int qr_data_codewords_M[] = {
	16, 28, 44, 64, 86, 108, 124, 154, 182, 216, 254, 290, 334, 365, 415, 453, 507,
	563, 627, 669, 714, 782, 860, 914, 1000, 1062, 1128, 1193, 1267,
	1373, 1455, 1541, 1631, 1725, 1812, 1914, 1992, 2102, 2216, 2334
};

static int qr_data_codewords_Q[] = {
	13, 22, 34, 48, 62, 76, 88, 110, 132, 154, 180, 206, 244, 261, 295, 325, 367,
	397, 445, 485, 512, 568, 614, 664, 718, 754, 808, 871, 911,
	985, 1033, 1115, 1171, 1231, 1286, 1354, 1426, 1502, 1582, 1666
};

static int qr_data_codewords_H[] = {
	9, 16, 26, 36, 46, 60, 66, 86, 100, 122, 140, 158, 180, 197, 223, 253, 283,
	313, 341, 385, 406, 442, 464, 514, 538, 596, 628, 661, 701,
	745, 793, 845, 901, 961, 986, 1054, 1096, 1142, 1222, 1276
};

static int qr_total_codewords[] = {
	26, 44, 70, 100, 134, 172, 196, 242, 292, 346, 404, 466, 532, 581, 655, 733, 815,
	901, 991, 1085, 1156, 1258, 1364, 1474, 1588, 1706, 1828, 1921, 2051,
	2185, 2323, 2465, 2611, 2761, 2876, 3034, 3196, 3362, 3532, 3706
};

static int qr_blocks_L[] = {
	1, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 4, 6, 6, 6, 6, 7, 8, 8, 9, 9, 10, 12, 12,
	12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25
};

static int qr_blocks_M[] = {
	1, 1, 1, 2, 2, 4, 4, 4, 5, 5, 5, 8, 9, 9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20,
	21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49
};

static int qr_blocks_Q[] = {
	1, 1, 2, 2, 4, 4, 6, 6, 8, 8, 8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25,
	27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68
};

static int qr_blocks_H[] = {
	1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30,
	32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81
};

static int qr_sizes[] = {
	21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77, 81, 85, 89, 93, 97,
	101, 105, 109, 113, 117, 121, 125, 129, 133, 137, 141, 145, 149, 153, 157, 161, 165, 169, 173, 177
};
