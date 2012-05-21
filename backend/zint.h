/*  zint.h - definitions for libzint

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

#ifndef ZINT_H
#define ZINT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct zint_render_line {
	float x, y, length, width;
	struct zint_render_line *next;      /* Pointer to next line */
};

struct zint_render_string {
	float x, y, fsize;
	float width;                        /* Suggested string width, may be 0 if none recommended */
	int length;
	unsigned char *text;
	struct zint_render_string *next;    /* Pointer to next character */
};

struct zint_render_ring {
	float x, y, radius, line_width;
	struct zint_render_ring *next;      /* Pointer to next ring */
};

struct zint_render_hexagon {
	float x, y;
	struct zint_render_hexagon *next;   /* Pointer to next hexagon */
};

struct zint_render {
	float width, height;
	struct zint_render_line *lines;	 	/* Pointer to first line */
	struct zint_render_string *strings;	/* Pointer to first string */
	struct zint_render_ring *rings;         /* Pointer to first ring */
	struct zint_render_hexagon *hexagons;   /* Pointer to first hexagon */
};

struct zint_symbol {
	int symbology;
	int height;
	int whitespace_width;
	int border_width;
	int output_options;
	char fgcolour[10];
	char bgcolour[10];
	char outfile[256];
	float scale;
	int option_1;
	int option_2;
	int option_3;
	int show_hrt;
	int input_mode;
	unsigned char text[128];
	int rows;
	int width;
	char primary[128];
	unsigned char encoded_data[178][143];
	int row_height[178]; /* Largest symbol is 177x177 QR Code */
	char errtxt[100];
	char *bitmap;
	int bitmap_width;
	int bitmap_height;
	struct zint_render *rendered;
};


/* Tbarcode 7 codes */
#define BARCODE_CODE11		1
#define BARCODE_C25MATRIX	2
#define BARCODE_C25INTER	3
#define BARCODE_C25IATA		4
#define BARCODE_C25LOGIC	6
#define BARCODE_C25IND		7
#define BARCODE_CODE39		8
#define BARCODE_EXCODE39	9
#define BARCODE_EANX		13
#define BARCODE_EAN128		16
#define BARCODE_CODABAR		18
#define BARCODE_CODE128		20
#define BARCODE_DPLEIT		21
#define BARCODE_DPIDENT		22
#define BARCODE_CODE16K		23
#define BARCODE_CODE49		24
#define BARCODE_CODE93		25
#define BARCODE_FLAT		28
#define BARCODE_RSS14		29
#define BARCODE_RSS_LTD		30
#define BARCODE_RSS_EXP		31
#define BARCODE_TELEPEN		32
#define BARCODE_UPCA		34
#define BARCODE_UPCE		37
#define BARCODE_POSTNET		40
#define BARCODE_MSI_PLESSEY	47
#define BARCODE_FIM		49
#define BARCODE_LOGMARS		50
#define BARCODE_PHARMA		51
#define BARCODE_PZN		52
#define BARCODE_PHARMA_TWO	53
#define BARCODE_PDF417		55
#define BARCODE_PDF417TRUNC	56
#define BARCODE_MAXICODE	57
#define BARCODE_QRCODE		58
#define BARCODE_CODE128B	60
#define BARCODE_AUSPOST		63
#define BARCODE_AUSREPLY	66
#define BARCODE_AUSROUTE	67
#define BARCODE_AUSREDIRECT	68
#define BARCODE_ISBNX		69
#define BARCODE_RM4SCC		70
#define BARCODE_DATAMATRIX	71
#define BARCODE_EAN14		72
#define BARCODE_CODABLOCKF	74
#define BARCODE_NVE18		75
#define BARCODE_JAPANPOST	76
#define BARCODE_KOREAPOST	77
#define BARCODE_RSS14STACK	79
#define BARCODE_RSS14STACK_OMNI	80
#define BARCODE_RSS_EXPSTACK	81
#define BARCODE_PLANET		82
#define BARCODE_MICROPDF417	84
#define BARCODE_ONECODE		85
#define BARCODE_PLESSEY		86

/* Tbarcode 8 codes */
#define BARCODE_TELEPEN_NUM	87
#define BARCODE_ITF14		89
#define BARCODE_KIX		90
#define BARCODE_AZTEC		92
#define BARCODE_DAFT		93
#define BARCODE_MICROQR		97

/* Tbarcode 9 codes */
#define BARCODE_HIBC_128	98
#define BARCODE_HIBC_39		99
#define BARCODE_HIBC_DM		102
#define BARCODE_HIBC_QR		104
#define BARCODE_HIBC_PDF	106
#define BARCODE_HIBC_MICPDF	108
#define BARCODE_HIBC_BLOCKF	110
#define BARCODE_HIBC_AZTEC	112

/* Zint specific */
#define BARCODE_AZRUNE		128
#define BARCODE_CODE32		129
#define BARCODE_EANX_CC		130
#define BARCODE_EAN128_CC	131
#define BARCODE_RSS14_CC	132
#define BARCODE_RSS_LTD_CC	133
#define BARCODE_RSS_EXP_CC	134
#define BARCODE_UPCA_CC		135
#define BARCODE_UPCE_CC		136
#define BARCODE_RSS14STACK_CC	137
#define BARCODE_RSS14_OMNI_CC	138
#define BARCODE_RSS_EXPSTACK_CC	139
#define BARCODE_CHANNEL		140
#define BARCODE_CODEONE		141
#define BARCODE_GRIDMATRIX	142

#define BARCODE_NO_ASCII	1
#define BARCODE_BIND		2
#define BARCODE_BOX		4
#define BARCODE_STDOUT		8
#define READER_INIT		16
#define SMALL_TEXT		32

#define DATA_MODE	0
#define UNICODE_MODE	1
#define GS1_MODE	2
#define KANJI_MODE	3
#define SJIS_MODE	4

#define DM_SQUARE	100

#define WARN_INVALID_OPTION	2
#define ERROR_TOO_LONG		5
#define ERROR_INVALID_DATA	6
#define ERROR_INVALID_CHECK	7
#define ERROR_INVALID_OPTION	8
#define ERROR_ENCODING_PROBLEM	9
#define ERROR_FILE_ACCESS	10
#define ERROR_MEMORY		11

#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32)
#  if defined (DLL_EXPORT) || defined(PIC) || defined(_USRDLL)
# 	 define ZINT_EXTERN __declspec(dllexport)
#  elif defined(ZINT_DLL)
#	 define ZINT_EXTERN __declspec(dllimport)
#  else
#    define ZINT_EXTERN extern
#  endif
#else
#  define ZINT_EXTERN extern
#endif

ZINT_EXTERN struct zint_symbol *ZBarcode_Create(void);
ZINT_EXTERN void ZBarcode_Clear(struct zint_symbol *symbol);
ZINT_EXTERN void ZBarcode_Delete(struct zint_symbol *symbol);

ZINT_EXTERN int ZBarcode_Encode(struct zint_symbol *symbol, unsigned char *input, int length);
ZINT_EXTERN int ZBarcode_Encode_File(struct zint_symbol *symbol, char *filename);
ZINT_EXTERN int ZBarcode_Print(struct zint_symbol *symbol, int rotate_angle);
ZINT_EXTERN int ZBarcode_Encode_and_Print(struct zint_symbol *symbol, unsigned char *input, int length, int rotate_angle);
ZINT_EXTERN int ZBarcode_Encode_File_and_Print(struct zint_symbol *symbol, char *filename, int rotate_angle);

ZINT_EXTERN int ZBarcode_Render(struct zint_symbol *symbol, float width, float height);

ZINT_EXTERN int ZBarcode_Buffer(struct zint_symbol *symbol, int rotate_angle);
ZINT_EXTERN int ZBarcode_Encode_and_Buffer(struct zint_symbol *symbol, unsigned char *input, int length, int rotate_angle);
ZINT_EXTERN int ZBarcode_Encode_File_and_Buffer(struct zint_symbol *symbol, char *filename, int rotate_angle);

ZINT_EXTERN int ZBarcode_ValidID(int symbol_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ZINT_H */
