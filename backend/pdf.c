/* pdf.c - PDF output */

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

#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "common.h"

#define SSET	"0123456789ABCDEF"

/* This file has expanded quite a bit since version 1.5 in order to accomodate
   the formatting rules for EAN and UPC symbols as set out in EN 797:1995 - the
   down side of this support is that the code is now vertually unreadable! */
struct pdf_file {
    int obj_offset;
    int num_objects;
    int xref_tbl[20];
    int content_stream_len;
    FILE *file_handle;
};
int vprinttopdf(struct pdf_file* pdffile,char *str,va_list arglist){
    int bytes_printed = vfprintf(pdffile->file_handle,str,arglist);
    pdffile->obj_offset+=bytes_printed;
    return bytes_printed;
}
int printtopdf(struct pdf_file* pdffile,char *str,...){
    va_list arglist;
    va_start(arglist,str);
    return  vprinttopdf(pdffile,str,arglist);
}
int printnewobj(struct pdf_file* pdffile,char *str,...){
    pdffile->xref_tbl[pdffile->num_objects++]=pdffile->obj_offset;
    va_list arglist;
    va_start(arglist,str);
    return vprinttopdf(pdffile,str,arglist);
}
int print_stream(struct pdf_file* pdffile,char *str,...){
    va_list arglist;
    va_start(arglist,str);
    int bytes_printed = printtopdf(pdffile,str,arglist);
    pdffile->content_stream_len+=bytes_printed;
    return bytes_printed;
}
void init_pdf(struct pdf_file* pdffile){
    printtopdf(pdffile,"%%PDF-1.6\n");

    // Catalog object    
    printnewobj(pdffile,"1 0 obj<</Type /Catalog /Pages 2 0 R>>\nendobj\n");

    // Pages Object
    printnewobj(pdffile,"2 0 obj <</Type /Pages /Kids [3 0 R]  /Count 1 /MediaBox [0 0 500 800]>>\nendobj\n");

    //Page 1
    printnewobj(pdffile,"3 0 obj<</Type /Page /Parent 2 0 R /Contents 6 0 R /Resources 5 0 R>>\nendobj\n");

    //write font obj
    printnewobj(pdffile,"4 0 obj<</Type /Font /Subtype /Type1 /BaseFont /Helvetica>>\nendobj\n");

    //write resources obj
    printnewobj(pdffile,"5 0 obj<</Font <</F1 4 0 R>>>>\nendobj\n");
   
    

}
void finishPDF(struct pdf_file* pdffile){
    fprintf(pdffile->file_handle,"xref\n");
    fprintf(pdffile->file_handle,"0 %d\n0000000000 65535 f \n",pdffile->num_objects);
    for(int i=0;i<pdffile->num_objects;i++){
        fprintf(pdffile->file_handle,"%010d 0000 n \n",pdffile->xref_tbl[i]);
    }
    fprintf(pdffile->file_handle,"trailer <</Size %d/Root 1 0 R>>\nstartxref\n%d\n",pdffile->num_objects,pdffile->obj_offset);
    fprintf(pdffile->file_handle,"%%%%EOF");
    
}
int pdf_plot(struct zint_symbol *symbol)
{
	int i, block_width, latch, r, this_row;
	float textpos, large_bar_height, preset_height, row_height, row_posn;
	FILE *fpdf;
        struct pdf_file tpdffile;
        struct pdf_file* pdffile=&tpdffile;
	int fgred, fggrn, fgblu, bgred, bggrn, bgblu;
	float red_ink, green_ink, blue_ink, red_paper, green_paper, blue_paper;
	int error_number = 0;
	int textoffset, xoffset, yoffset, textdone, main_width;
	char textpart[10], addon[6];
	int large_bar_count, comp_offset;
	float addon_text_posn;
	float scaler = symbol->scale;
	float default_text_posn;
	int plot_text = 1;
	const char *locale = NULL;

	row_height=0;
	textdone = 0;
	main_width = symbol->width;
	strcpy(addon, "");
	comp_offset = 0;
	addon_text_posn = 0.0;

	if((symbol->output_options & BARCODE_STDOUT) != 0) {
		fpdf = stdout;
	} else {
		fpdf = fopen(symbol->outfile, "w");
	}
	if(fpdf == NULL) {
		strcpy(symbol->errtxt, "Could not open output file");
		return ZERROR_FILE_ACCESS;
	}
        // initialize pdffile
         pdffile->file_handle = fpdf;
         pdffile->num_objects=0;
         pdffile->obj_offset=0;
         init_pdf(pdffile);
	/* sort out colour options */
	to_upper((unsigned char*)symbol->fgcolour);
	to_upper((unsigned char*)symbol->bgcolour);

	if(strlen(symbol->fgcolour) != 6) {
		strcpy(symbol->errtxt, "Malformed foreground colour target");
		return ZERROR_INVALID_OPTION;
	}
	if(strlen(symbol->bgcolour) != 6) {
		strcpy(symbol->errtxt, "Malformed background colour target");
		return ZERROR_INVALID_OPTION;
	}
	error_number = is_sane(SSET, (unsigned char*)symbol->fgcolour, strlen(symbol->fgcolour));
	if (error_number == ZERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Malformed foreground colour target");
		return ZERROR_INVALID_OPTION;
	}
	error_number = is_sane(SSET, (unsigned char*)symbol->bgcolour, strlen(symbol->bgcolour));
	if (error_number == ZERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "Malformed background colour target");
		return ZERROR_INVALID_OPTION;
	}
	locale = setlocale(LC_ALL, "C");

	fgred = (16 * ctoi(symbol->fgcolour[0])) + ctoi(symbol->fgcolour[1]);
	fggrn = (16 * ctoi(symbol->fgcolour[2])) + ctoi(symbol->fgcolour[3]);
	fgblu = (16 * ctoi(symbol->fgcolour[4])) + ctoi(symbol->fgcolour[5]);
	bgred = (16 * ctoi(symbol->bgcolour[0])) + ctoi(symbol->bgcolour[1]);
	bggrn = (16 * ctoi(symbol->bgcolour[2])) + ctoi(symbol->bgcolour[3]);
	bgblu = (16 * ctoi(symbol->bgcolour[4])) + ctoi(symbol->bgcolour[5]);
	red_ink = fgred / 256.0;
	green_ink = fggrn / 256.0;
	blue_ink = fgblu / 256.0;
	red_paper = bgred / 256.0;
	green_paper = bggrn / 256.0;
	blue_paper = bgblu / 256.0;

	if (symbol->height == 0) {
		symbol->height = 50;
	}

	large_bar_count = 0;
	preset_height = 0.0;
	for(i = 0; i < symbol->rows; i++) {
		preset_height += symbol->row_height[i];
		if(symbol->row_height[i] == 0) {
			large_bar_count++;
		}
	}
	large_bar_height = (symbol->height - preset_height) / large_bar_count;

	if (large_bar_count == 0) {
		symbol->height = preset_height;
	}

	while(!(module_is_set(symbol, symbol->rows - 1, comp_offset))) {
		comp_offset++;
	}

	/* Certain symbols need whitespace otherwise characters get chopped off the sides */
	if ((((symbol->symbology == BARCODE_EANX) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_EANX_CC))
		|| (symbol->symbology == BARCODE_ISBNX)) {
		switch(ustrlen(symbol->text)) {
			case 13: /* EAN 13 */
			case 16:
			case 19:
				if(symbol->whitespace_width == 0) {
					symbol->whitespace_width = 10;
				}
				main_width = 96 + comp_offset;
				break;
			default:
				main_width = 68 + comp_offset;
		}
	}

	if (((symbol->symbology == BARCODE_UPCA) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCA_CC)) {
		if(symbol->whitespace_width == 0) {
			symbol->whitespace_width = 10;
			main_width = 96 + comp_offset;
		}
	}

	if (((symbol->symbology == BARCODE_UPCE) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCE_CC)) {
		if(symbol->whitespace_width == 0) {
			symbol->whitespace_width = 10;
			main_width = 51 + comp_offset;
		}
	}

	latch = 0;
	r = 0;
	/* Isolate add-on text */
	if(is_extendable(symbol->symbology)) {
		for(i = 0; i < ustrlen(symbol->text); i++) {
			if (latch == 1) {
				addon[r] = symbol->text[i];
				r++;
			}
			if (symbol->text[i] == '+') {
				latch = 1;
			}
		}
	}
	addon[r] = '\0';

	if((symbol->show_hrt == 0) || (ustrlen(symbol->text) == 0)) {
		plot_text = 0;
	}
	if(plot_text) {
		textoffset = 9;
	} else {
		textoffset = 0;
	}
	xoffset = symbol->border_width + symbol->whitespace_width;
	yoffset = symbol->border_width;

	/* Start writing the header */
	fprintf(fpdf, "%%!PS-Adobe-3.0 EPSF-3.0\n");
	fprintf(fpdf, "%%%%Creator: Zint %s\n", ZINT_VERSION);
	if(ustrlen(symbol->text) != 0) {
		fprintf(fpdf, "%%%%Title: %s\n",symbol->text);
	} else {
		fprintf(fpdf, "%%%%Title: Zint Generated Symbol\n");
	}
	fprintf(fpdf, "%%%%Pages: 0\n");
	if(symbol->symbology != BARCODE_MAXICODE) {
		fprintf(fpdf, "%%%%BoundingBox: 0 0 %d %d\n", roundup((symbol->width + xoffset + xoffset) * scaler), roundup((symbol->height + textoffset + yoffset + yoffset) * scaler));
	} else {
		fprintf(fpdf, "%%%%BoundingBox: 0 0 %d %d\n", roundup((74.0 + xoffset + xoffset) * scaler), roundup((72.0 + yoffset + yoffset) * scaler));
	}
	fprintf(fpdf, "%%%%EndComments\n");

	/* Definitions */
	fprintf(fpdf, "/TL { setlinewidth m l stroke } bind def\n");
	fprintf(fpdf, "/TC { m 0 360 arc 360 0 arcn fill } bind def\n");
	fprintf(fpdf, "/TH { 0 setlinewidth m l l l l l closepath fill } bind def\n");
	fprintf(fpdf, "/TB { 2 copy } bind def\n");
	fprintf(fpdf, "/TR { newpath 4 1 roll exch m 1 index 0 rlineto 0 exch rlineto neg 0 rlineto closepath fill } bind def\n");
	fprintf(fpdf, "/TE { pop pop } bind def\n");

	fprintf(fpdf, "newpath\n");

	/* Now the actual representation */
	fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
	fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_paper, green_paper, blue_paper);
	fprintf(fpdf, "%.2f 0.00 TB 0.00 %.2f TR\n", (symbol->height + textoffset + yoffset + yoffset) * scaler, (symbol->width + xoffset + xoffset) * scaler);

	if(((symbol->output_options & BARCODE_BOX) != 0) || ((symbol->output_options & BARCODE_BIND) != 0)) {
		default_text_posn = 0.5 * scaler;
	} else {
		default_text_posn = (symbol->border_width + 0.5) * scaler;
	}

	if(symbol->symbology == BARCODE_MAXICODE) {
		/* Maxicode uses hexagons */
		float ax, ay, bx, by, cx, cy, dx, dy, ex, ey, fx, fy, mx, my;


		textoffset = 0.0;
		if (((symbol->output_options & BARCODE_BOX) != 0) || ((symbol->output_options & BARCODE_BIND) != 0)) {
			fprintf(fpdf, "TE\n");
			fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
			fprintf(fpdf, "%.2f %.2f TB %.2f %.2f TR\n", symbol->border_width * scaler, textoffset * scaler, 0.0, (74.0 + xoffset + xoffset) * scaler);
			fprintf(fpdf, "%.2f %.2f TB %.2f %.2f TR\n", symbol->border_width * scaler, (textoffset + 72.0 + symbol->border_width) * scaler, 0.0, (74.0 + xoffset + xoffset) * scaler);
		}
		if((symbol->output_options & BARCODE_BOX) != 0) {
			/* side bars */
			fprintf(fpdf, "TE\n");
			fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
			fprintf(fpdf, "%.2f %.2f TB %.2f %.2f TR\n", (72.0 + (2 * symbol->border_width)) * scaler, textoffset * scaler, 0.0, symbol->border_width * scaler);
			fprintf(fpdf, "%.2f %.2f TB %.2f %.2f TR\n", (72.0 + (2 * symbol->border_width)) * scaler, textoffset * scaler, (74.0 + xoffset + xoffset - symbol->border_width) * scaler, symbol->border_width * scaler);
		}

		fprintf(fpdf, "TE\n");
		fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
		fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
		fprintf(fpdf, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f TC\n", (35.76 + xoffset) * scaler, (35.60 + yoffset) * scaler, 10.85 * scaler, (35.76 + xoffset) * scaler, (35.60 + yoffset) * scaler, 8.97 * scaler, (44.73 + xoffset) * scaler, (35.60 + yoffset) * scaler);
		fprintf(fpdf, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f TC\n", (35.76 + xoffset) * scaler, (35.60 + yoffset) * scaler, 7.10 * scaler, (35.76 + xoffset) * scaler, (35.60 + yoffset) * scaler, 5.22 * scaler, (40.98 + xoffset) * scaler, (35.60 + yoffset) * scaler);
		fprintf(fpdf, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f TC\n", (35.76 + xoffset) * scaler, (35.60 + yoffset) * scaler, 3.31 * scaler, (35.76 + xoffset) * scaler, (35.60 + yoffset) * scaler, 1.43 * scaler, (37.19 + xoffset) * scaler, (35.60 + yoffset) * scaler);
		for(r = 0; r < symbol->rows; r++) {
			for(i = 0; i < symbol->width; i++) {
				if(module_is_set(symbol, r, i)) {
					/* Dump a hexagon */
					my = ((symbol->rows - r - 1)) * 2.135 + 1.43;
					ay = my + 1.0 + yoffset;
					by = my + 0.5 + yoffset;
					cy = my - 0.5 + yoffset;
					dy = my - 1.0 + yoffset;
					ey = my - 0.5 + yoffset;
					fy = my + 0.5 + yoffset;

					mx = 2.46 * i + 1.23 + (r & 1 ? 1.23 : 0);

					ax = mx + xoffset;
					bx = mx + 0.86 + xoffset;
					cx = mx + 0.86 + xoffset;
					dx = mx + xoffset;
					ex = mx - 0.86 + xoffset;
					fx = mx - 0.86 + xoffset;
					fprintf(fpdf, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f TH\n", ax * scaler, ay * scaler, bx * scaler, by * scaler, cx * scaler, cy * scaler, dx * scaler, dy * scaler, ex * scaler, ey * scaler, fx * scaler, fy * scaler);
				}
			}
		}
	}

	if(symbol->symbology != BARCODE_MAXICODE) {
		/* everything else uses rectangles (or squares) */
		/* Works from the bottom of the symbol up */
		int addon_latch = 0;

		for(r = 0; r < symbol->rows; r++) {
			this_row = symbol->rows - r - 1; /* invert r otherwise plots upside down */
			if(symbol->row_height[this_row] == 0) {
				row_height = large_bar_height;
			} else {
				row_height = symbol->row_height[this_row];
			}
			row_posn = 0;
			for(i = 0; i < r; i++) {
				if(symbol->row_height[symbol->rows - i - 1] == 0) {
					row_posn += large_bar_height;
				} else {
					row_posn += symbol->row_height[symbol->rows - i - 1];
				}
			}
			row_posn += (textoffset + yoffset);

			fprintf(fpdf, "TE\n");
			fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
			fprintf(fpdf, "%.2f %.2f ", row_height * scaler, row_posn * scaler);
			i = 0;
			if(module_is_set(symbol, this_row, 0)) {
				latch = 1;
			} else {
				latch = 0;
			}

			do {
				block_width = 0;
				do {
					block_width++;
				} while (module_is_set(symbol, this_row, i + block_width) == module_is_set(symbol, this_row, i));
				if((addon_latch == 0) && (r == 0) && (i > main_width)) {
					fprintf(fpdf, "TE\n");
					fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
					fprintf(fpdf, "%.2f %.2f ", (row_height - 5.0) * scaler, (row_posn - 5.0) * scaler);
					addon_text_posn = row_posn + row_height - 8.0;
					addon_latch = 1;
				}
				if(latch == 1) {
					/* a bar */
					fprintf(fpdf, "TB %.2f %.2f TR\n", (i + xoffset) * scaler, block_width * scaler);
					latch = 0;
				} else {
					/* a space */
					latch = 1;
				}
				i += block_width;

			} while (i < symbol->width);
		}
	}
	/* That's done the actual data area, everything else is human-friendly */

	xoffset += comp_offset;

	if (plot_text) {
		if ((((symbol->symbology == BARCODE_EANX) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_EANX_CC)) ||
			(symbol->symbology == BARCODE_ISBNX)) {
			/* guard bar extensions and text formatting for EAN8 and EAN13 */
			switch(ustrlen(symbol->text)) {
				case 8: /* EAN-8 */
				case 11:
				case 14:
					fprintf(fpdf, "TE\n");
					fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
					fprintf(fpdf, "%.2f %.2f ", 5.0 * scaler, (4.0 + yoffset) * scaler);
					fprintf(fpdf, "TB %.2f %.2f TR\n", (0 + xoffset) * scaler, 1 * scaler);
					fprintf(fpdf, "TB %.2f %.2f TR\n", (2 + xoffset) * scaler, 1 * scaler);
					fprintf(fpdf, "TB %.2f %.2f TR\n", (32 + xoffset) * scaler, 1 * scaler);
					fprintf(fpdf, "TB %.2f %.2f TR\n", (34 + xoffset) * scaler, 1 * scaler);
					fprintf(fpdf, "TB %.2f %.2f TR\n", (64 + xoffset) * scaler, 1 * scaler);
					fprintf(fpdf, "TB %.2f %.2f TR\n", (66 + xoffset) * scaler, 1 * scaler);
					for(i = 0; i < 4; i++) {
						textpart[i] = symbol->text[i];
					}
					textpart[4] = '\0';
					fprintf(fpdf, "TE\n");
					fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
					fprintf(fpdf, "matrix currentmatrix\n");
					fprintf(fpdf, "/Helvetica findfont\n");
					fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
					textpos = 17;
					fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", (textpos + xoffset) * scaler, default_text_posn);
					fprintf(fpdf, " (%s) stringwidth\n", textpart);
					fprintf(fpdf, "pop\n");
					fprintf(fpdf, "-2 div 0 rmoveto\n");
					fprintf(fpdf, " (%s) show\n", textpart);
					fprintf(fpdf, "setmatrix\n");
					for(i = 0; i < 4; i++) {
						textpart[i] = symbol->text[i + 4];
					}
					textpart[4] = '\0';
					fprintf(fpdf, "matrix currentmatrix\n");
					fprintf(fpdf, "/Helvetica findfont\n");
					fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
					textpos = 50;
					fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", (textpos + xoffset) * scaler, default_text_posn);
					fprintf(fpdf, " (%s) stringwidth\n", textpart);
					fprintf(fpdf, "pop\n");
					fprintf(fpdf, "-2 div 0 rmoveto\n");
					fprintf(fpdf, " (%s) show\n", textpart);
					fprintf(fpdf, "setmatrix\n");
					textdone = 1;
					switch(strlen(addon)) {
						case 2:
							fprintf(fpdf, "matrix currentmatrix\n");
							fprintf(fpdf, "/Helvetica findfont\n");
							fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
							textpos = xoffset + 86;
							fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos * scaler, addon_text_posn * scaler);
							fprintf(fpdf, " (%s) stringwidth\n", addon);
							fprintf(fpdf, "pop\n");
							fprintf(fpdf, "-2 div 0 rmoveto\n");
							fprintf(fpdf, " (%s) show\n", addon);
							fprintf(fpdf, "setmatrix\n");
							break;
						case 5:
							fprintf(fpdf, "matrix currentmatrix\n");
							fprintf(fpdf, "/Helvetica findfont\n");
							fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
							textpos = xoffset + 100;
							fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos * scaler, addon_text_posn * scaler);
							fprintf(fpdf, " (%s) stringwidth\n", addon);
							fprintf(fpdf, "pop\n");
							fprintf(fpdf, "-2 div 0 rmoveto\n");
							fprintf(fpdf, " (%s) show\n", addon);
							fprintf(fpdf, "setmatrix\n");
							break;
					}

					break;
				case 13: /* EAN 13 */
				case 16:
				case 19:
					fprintf(fpdf, "TE\n");
					fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
					fprintf(fpdf, "%.2f %.2f ", 5.0 * scaler, (4.0 + yoffset) * scaler);
					fprintf(fpdf, "TB %.2f %.2f TR\n", (0 + xoffset) * scaler, 1 * scaler);
					fprintf(fpdf, "TB %.2f %.2f TR\n", (2 + xoffset) * scaler, 1 * scaler);
					fprintf(fpdf, "TB %.2f %.2f TR\n", (46 + xoffset) * scaler, 1 * scaler);
					fprintf(fpdf, "TB %.2f %.2f TR\n", (48 + xoffset) * scaler, 1 * scaler);
					fprintf(fpdf, "TB %.2f %.2f TR\n", (92 + xoffset) * scaler, 1 * scaler);
					fprintf(fpdf, "TB %.2f %.2f TR\n", (94 + xoffset) * scaler, 1 * scaler);
					textpart[0] = symbol->text[0];
					textpart[1] = '\0';
					fprintf(fpdf, "TE\n");
					fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
					fprintf(fpdf, "matrix currentmatrix\n");
					fprintf(fpdf, "/Helvetica findfont\n");
					fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
					textpos = -7;
					fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", (textpos + xoffset) * scaler, default_text_posn);
					fprintf(fpdf, " (%s) stringwidth\n", textpart);
					fprintf(fpdf, "pop\n");
					fprintf(fpdf, "-2 div 0 rmoveto\n");
					fprintf(fpdf, " (%s) show\n", textpart);
					fprintf(fpdf, "setmatrix\n");
					for(i = 0; i < 6; i++) {
						textpart[i] = symbol->text[i + 1];
					}
					textpart[6] = '\0';
					fprintf(fpdf, "matrix currentmatrix\n");
					fprintf(fpdf, "/Helvetica findfont\n");
					fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
					textpos = 24;
					fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", (textpos + xoffset) * scaler, default_text_posn);
					fprintf(fpdf, " (%s) stringwidth\n", textpart);
					fprintf(fpdf, "pop\n");
					fprintf(fpdf, "-2 div 0 rmoveto\n");
					fprintf(fpdf, " (%s) show\n", textpart);
					fprintf(fpdf, "setmatrix\n");
					for(i = 0; i < 6; i++) {
						textpart[i] = symbol->text[i + 7];
					}
					textpart[6] = '\0';
					fprintf(fpdf, "matrix currentmatrix\n");
					fprintf(fpdf, "/Helvetica findfont\n");
					fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
					textpos = 71;
					fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", (textpos + xoffset) * scaler, default_text_posn);
					fprintf(fpdf, " (%s) stringwidth\n", textpart);
					fprintf(fpdf, "pop\n");
					fprintf(fpdf, "-2 div 0 rmoveto\n");
					fprintf(fpdf, " (%s) show\n", textpart);
					fprintf(fpdf, "setmatrix\n");
					textdone = 1;
					switch(strlen(addon)) {
						case 2:
							fprintf(fpdf, "matrix currentmatrix\n");
							fprintf(fpdf, "/Helvetica findfont\n");
							fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
							textpos = xoffset + 114;
							fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos * scaler, addon_text_posn * scaler);
							fprintf(fpdf, " (%s) stringwidth\n", addon);
							fprintf(fpdf, "pop\n");
							fprintf(fpdf, "-2 div 0 rmoveto\n");
							fprintf(fpdf, " (%s) show\n", addon);
							fprintf(fpdf, "setmatrix\n");
							break;
						case 5:
							fprintf(fpdf, "matrix currentmatrix\n");
							fprintf(fpdf, "/Helvetica findfont\n");
							fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
							textpos = xoffset + 128;
							fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos * scaler, addon_text_posn * scaler);
							fprintf(fpdf, " (%s) stringwidth\n", addon);
							fprintf(fpdf, "pop\n");
							fprintf(fpdf, "-2 div 0 rmoveto\n");
							fprintf(fpdf, " (%s) show\n", addon);
							fprintf(fpdf, "setmatrix\n");
							break;
					}
					break;

			}
		}

		if (((symbol->symbology == BARCODE_UPCA) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCA_CC)) {
			/* guard bar extensions and text formatting for UPCA */
			fprintf(fpdf, "TE\n");
			fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
			fprintf(fpdf, "%.2f %.2f ", 5.0 * scaler, (4.0 + yoffset) * scaler);
			latch = 1;

			i = 0 + comp_offset;
			do {
				block_width = 0;
				do {
					block_width++;
				} while (module_is_set(symbol, symbol->rows - 1, i + block_width) == module_is_set(symbol, symbol->rows - 1, i));
				if(latch == 1) {
					/* a bar */
					fprintf(fpdf, "TB %.2f %.2f TR\n", (i + xoffset - comp_offset) * scaler, block_width * scaler);
					latch = 0;
				} else {
					/* a space */
					latch = 1;
				}
				i += block_width;
			} while (i < 11 + comp_offset);
			fprintf(fpdf, "TB %.2f %.2f TR\n", (46 + xoffset) * scaler, 1 * scaler);
			fprintf(fpdf, "TB %.2f %.2f TR\n", (48 + xoffset) * scaler, 1 * scaler);
			latch = 1;
			i = 85 + comp_offset;
			do {
				block_width = 0;
				do {
					block_width++;
				} while (module_is_set(symbol, symbol->rows - 1, i + block_width) == module_is_set(symbol, symbol->rows - 1, i));
				if(latch == 1) {
					/* a bar */
					fprintf(fpdf, "TB %.2f %.2f TR\n", (i + xoffset - comp_offset) * scaler, block_width * scaler);
					latch = 0;
				} else {
					/* a space */
					latch = 1;
				}
				i += block_width;
			} while (i < 96 + comp_offset);
			textpart[0] = symbol->text[0];
			textpart[1] = '\0';
			fprintf(fpdf, "TE\n");
			fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
			fprintf(fpdf, "matrix currentmatrix\n");
			fprintf(fpdf, "/Helvetica findfont\n");
			fprintf(fpdf, "%.2f scalefont setfont\n", 8.0 * scaler);
			textpos = -5;
			fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", (textpos + xoffset) * scaler, default_text_posn);
			fprintf(fpdf, " (%s) stringwidth\n", textpart);
			fprintf(fpdf, "pop\n");
			fprintf(fpdf, "-2 div 0 rmoveto\n");
			fprintf(fpdf, " (%s) show\n", textpart);
			fprintf(fpdf, "setmatrix\n");
			for(i = 0; i < 5; i++) {
				textpart[i] = symbol->text[i + 1];
			}
			textpart[5] = '\0';
			fprintf(fpdf, "matrix currentmatrix\n");
			fprintf(fpdf, "/Helvetica findfont\n");
			fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
			textpos = 27;
			fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", (textpos + xoffset) * scaler, default_text_posn);
			fprintf(fpdf, " (%s) stringwidth\n", textpart);
			fprintf(fpdf, "pop\n");
			fprintf(fpdf, "-2 div 0 rmoveto\n");
			fprintf(fpdf, " (%s) show\n", textpart);
			fprintf(fpdf, "setmatrix\n");
			for(i = 0; i < 5; i++) {
				textpart[i] = symbol->text[i + 6];
			}
			textpart[6] = '\0';
			fprintf(fpdf, "matrix currentmatrix\n");
			fprintf(fpdf, "/Helvetica findfont\n");
			fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
			textpos = 68;
			fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", (textpos + xoffset) * scaler, default_text_posn);
			fprintf(fpdf, " (%s) stringwidth\n", textpart);
			fprintf(fpdf, "pop\n");
			fprintf(fpdf, "-2 div 0 rmoveto\n");
			fprintf(fpdf, " (%s) show\n", textpart);
			fprintf(fpdf, "setmatrix\n");
			textpart[0] = symbol->text[11];
			textpart[1] = '\0';
			fprintf(fpdf, "matrix currentmatrix\n");
			fprintf(fpdf, "/Helvetica findfont\n");
			fprintf(fpdf, "%.2f scalefont setfont\n", 8.0 * scaler);
			textpos = 100;
			fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", (textpos + xoffset) * scaler, default_text_posn);
			fprintf(fpdf, " (%s) stringwidth\n", textpart);
			fprintf(fpdf, "pop\n");
			fprintf(fpdf, "-2 div 0 rmoveto\n");
			fprintf(fpdf, " (%s) show\n", textpart);
			fprintf(fpdf, "setmatrix\n");
			textdone = 1;
			switch(strlen(addon)) {
				case 2:
					fprintf(fpdf, "matrix currentmatrix\n");
					fprintf(fpdf, "/Helvetica findfont\n");
					fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
					textpos = xoffset + 116;
					fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos * scaler, addon_text_posn * scaler);
					fprintf(fpdf, " (%s) stringwidth\n", addon);
					fprintf(fpdf, "pop\n");
					fprintf(fpdf, "-2 div 0 rmoveto\n");
					fprintf(fpdf, " (%s) show\n", addon);
					fprintf(fpdf, "setmatrix\n");
					break;
				case 5:
					fprintf(fpdf, "matrix currentmatrix\n");
					fprintf(fpdf, "/Helvetica findfont\n");
					fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
					textpos = xoffset + 130;
					fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos * scaler, addon_text_posn * scaler);
					fprintf(fpdf, " (%s) stringwidth\n", addon);
					fprintf(fpdf, "pop\n");
					fprintf(fpdf, "-2 div 0 rmoveto\n");
					fprintf(fpdf, " (%s) show\n", addon);
					fprintf(fpdf, "setmatrix\n");
					break;
			}

		}

		if (((symbol->symbology == BARCODE_UPCE) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCE_CC)) {
			/* guard bar extensions and text formatting for UPCE */
			fprintf(fpdf, "TE\n");
			fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
			fprintf(fpdf, "%.2f %.2f ", 5.0 * scaler, (4.0 + yoffset) * scaler);
			fprintf(fpdf, "TB %.2f %.2f TR\n", (0 + xoffset) * scaler, 1 * scaler);
			fprintf(fpdf, "TB %.2f %.2f TR\n", (2 + xoffset) * scaler, 1 * scaler);
			fprintf(fpdf, "TB %.2f %.2f TR\n", (46 + xoffset) * scaler, 1 * scaler);
			fprintf(fpdf, "TB %.2f %.2f TR\n", (48 + xoffset) * scaler, 1 * scaler);
			fprintf(fpdf, "TB %.2f %.2f TR\n", (50 + xoffset) * scaler, 1 * scaler);
			textpart[0] = symbol->text[0];
			textpart[1] = '\0';
			fprintf(fpdf, "TE\n");
			fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
			fprintf(fpdf, "matrix currentmatrix\n");
			fprintf(fpdf, "/Helvetica findfont\n");
			fprintf(fpdf, "%.2f scalefont setfont\n", 8.0 * scaler);
			textpos = -5;
			fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", (textpos + xoffset) * scaler, default_text_posn);
			fprintf(fpdf, " (%s) stringwidth\n", textpart);
			fprintf(fpdf, "pop\n");
			fprintf(fpdf, "-2 div 0 rmoveto\n");
			fprintf(fpdf, " (%s) show\n", textpart);
			fprintf(fpdf, "setmatrix\n");
			for(i = 0; i < 6; i++) {
				textpart[i] = symbol->text[i + 1];
			}
			textpart[6] = '\0';
			fprintf(fpdf, "matrix currentmatrix\n");
			fprintf(fpdf, "/Helvetica findfont\n");
			fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
			textpos = 24;
			fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", (textpos + xoffset) * scaler, default_text_posn);
			fprintf(fpdf, " (%s) stringwidth\n", textpart);
			fprintf(fpdf, "pop\n");
			fprintf(fpdf, "-2 div 0 rmoveto\n");
			fprintf(fpdf, " (%s) show\n", textpart);
			fprintf(fpdf, "setmatrix\n");
			textpart[0] = symbol->text[7];
			textpart[1] = '\0';
			fprintf(fpdf, "matrix currentmatrix\n");
			fprintf(fpdf, "/Helvetica findfont\n");
			fprintf(fpdf, "%.2f scalefont setfont\n", 8.0 * scaler);
			textpos = 55;
			fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", (textpos + xoffset) * scaler, default_text_posn);
			fprintf(fpdf, " (%s) stringwidth\n", textpart);
			fprintf(fpdf, "pop\n");
			fprintf(fpdf, "-2 div 0 rmoveto\n");
			fprintf(fpdf, " (%s) show\n", textpart);
			fprintf(fpdf, "setmatrix\n");
			textdone = 1;
			switch(strlen(addon)) {
				case 2:
					fprintf(fpdf, "matrix currentmatrix\n");
					fprintf(fpdf, "/Helvetica findfont\n");
					fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
					textpos = xoffset + 70;
					fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos * scaler, addon_text_posn * scaler);
					fprintf(fpdf, " (%s) stringwidth\n", addon);
					fprintf(fpdf, "pop\n");
					fprintf(fpdf, "-2 div 0 rmoveto\n");
					fprintf(fpdf, " (%s) show\n", addon);
					fprintf(fpdf, "setmatrix\n");
					break;
				case 5:
					fprintf(fpdf, "matrix currentmatrix\n");
					fprintf(fpdf, "/Helvetica findfont\n");
					fprintf(fpdf, "%.2f scalefont setfont\n", 11.0 * scaler);
					textpos = xoffset + 84;
					fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", textpos * scaler, addon_text_posn * scaler);
					fprintf(fpdf, " (%s) stringwidth\n", addon);
					fprintf(fpdf, "pop\n");
					fprintf(fpdf, "-2 div 0 rmoveto\n");
					fprintf(fpdf, " (%s) show\n", addon);
					fprintf(fpdf, "setmatrix\n");
					break;
			}

		}
	} /* if (plot_text) */

	xoffset -= comp_offset;

	switch(symbol->symbology) {
		case BARCODE_MAXICODE:
			/* Do nothing! (It's already been done) */
			break;
		default:
			if((symbol->output_options & BARCODE_BIND) != 0) {
				if((symbol->rows > 1) && (is_stackable(symbol->symbology) == 1)) {
					/* row binding */
					fprintf(fpdf, "TE\n");
					fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
					for(r = 1; r < symbol->rows; r++) {
						fprintf(fpdf, "%.2f %.2f TB %.2f %.2f TR\n", 2.0 * scaler, ((r * row_height) + textoffset + yoffset - 1) * scaler, xoffset * scaler, symbol->width * scaler);
					}
				}
			}
			if (((symbol->output_options & BARCODE_BOX) != 0) || ((symbol->output_options & BARCODE_BIND) != 0)) {
				fprintf(fpdf, "TE\n");
				fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
				fprintf(fpdf, "%.2f %.2f TB %.2f %.2f TR\n", symbol->border_width * scaler, textoffset * scaler, 0.0, (symbol->width + xoffset + xoffset) * scaler);
				fprintf(fpdf, "%.2f %.2f TB %.2f %.2f TR\n", symbol->border_width * scaler, (textoffset + symbol->height + symbol->border_width) * scaler, 0.0, (symbol->width + xoffset + xoffset) * scaler);
			}
			if((symbol->output_options & BARCODE_BOX) != 0) {
				/* side bars */
				fprintf(fpdf, "TE\n");
				fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
				fprintf(fpdf, "%.2f %.2f TB %.2f %.2f TR\n", (symbol->height + (2 * symbol->border_width)) * scaler, textoffset * scaler, 0.0, symbol->border_width * scaler);
				fprintf(fpdf, "%.2f %.2f TB %.2f %.2f TR\n", (symbol->height + (2 * symbol->border_width)) * scaler, textoffset * scaler, (symbol->width + xoffset + xoffset - symbol->border_width) * scaler, symbol->border_width * scaler);
			}
			break;
	}

	/* Put the human readable text at the bottom */
	if(plot_text && (textdone == 0)) {
		fprintf(fpdf, "TE\n");
		fprintf(fpdf, "%.2f %.2f %.2f setrgbcolor\n", red_ink, green_ink, blue_ink);
		fprintf(fpdf, "matrix currentmatrix\n");
		fprintf(fpdf, "/Helvetica findfont\n");
		fprintf(fpdf, "%.2f scalefont setfont\n", 8.0 * scaler);
		textpos = symbol->width / 2.0;
		fprintf(fpdf, " 0 0 moveto %.2f %.2f translate 0.00 rotate 0 0 moveto\n", (textpos + xoffset) * scaler, default_text_posn);
		fprintf(fpdf, " (%s) stringwidth\n", symbol->text);
		fprintf(fpdf, "pop\n");
		fprintf(fpdf, "-2 div 0 rmoveto\n");
		fprintf(fpdf, " (%s) show\n", symbol->text);
		fprintf(fpdf, "setmatrix\n");
	}
	fprintf(fpdf, "\nshowpage\n");

	fclose(fpdf);

	if (locale)
		setlocale(LC_ALL, locale);

	return error_number;
}

