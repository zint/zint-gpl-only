/***************************************************************************
 *   Copyright (C) 2008 by BogDan Vatra                                    *
 *   bogdan@licentia.eu                                                    *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "qzint.h"
#include <stdio.h>
#define SSET	"0123456789ABCDEF"

namespace Zint
{

static const qreal maxi_diagonal=11;
static const qreal maxi_width=1.73205807568877*maxi_diagonal/2;

QZint::QZint()
{
	m_symbol=BARCODE_CODE128;
	m_height=50;
	m_border=NO_BORDER;
	m_borderWidth=1;
	m_securityLevel=-1;
	m_pdf417CodeWords=928;
	m_fgColor=Qt::black;
	m_bgColor=Qt::white;
	m_zintSymbol=0;
	m_error=0;
	m_input_mode = UNICODE_MODE;
	m_scale = 1.0;
}

QZint::~QZint()
{
	if (m_zintSymbol)
		ZBarcode_Delete(m_zintSymbol);
}

void QZint::encode()
{
	if (m_zintSymbol)
		ZBarcode_Delete(m_zintSymbol);

	m_lastError.clear();
	m_zintSymbol = ZBarcode_Create();
	m_zintSymbol->output_options=m_border;
	m_zintSymbol->symbology=m_symbol;
	m_zintSymbol->height=m_height;
	m_zintSymbol->whitespace_width=m_whitespace;
	m_zintSymbol->border_width=m_borderWidth;
	m_zintSymbol->option_1=m_securityLevel;
	m_zintSymbol->input_mode = m_input_mode;
	m_zintSymbol->option_2=m_width;
	m_zintSymbol->option_3=m_pdf417CodeWords;
	QByteArray bstr=m_text.toAscii();
	QByteArray pstr=m_primaryMessage.left(99).toAscii();
	strcpy(m_zintSymbol->primary,pstr.data());
	int error = ZBarcode_Encode(m_zintSymbol, (unsigned char*)bstr.data());
	if (error > WARN_INVALID_OPTION)
		m_lastError=m_zintSymbol->errtxt;

	if (m_zintSymbol->symbology == BARCODE_MAXICODE)
		m_zintSymbol->height = 33;
	
	switch(m_zintSymbol->output_options) {
		case 0: m_border = NO_BORDER; break;
		case 2: m_border = BIND; break;
		case 4: m_border = BOX; break;
	}
	m_borderWidth = (BorderType)m_zintSymbol->border_width;
	m_whitespace = m_zintSymbol->whitespace_width;
}

int  QZint::symbol()
{
	return m_symbol;
}
void QZint::setSymbol(int symbol)
{
	m_symbol=symbol;
}

void QZint::setInputMode(int input_mode)
{
	m_input_mode = input_mode;
}

QString QZint::text()
{
	return m_text;
}
void QZint::setText(const QString & text)
{
	m_text=text;
}

QString QZint::primaryMessage()
{
	return m_primaryMessage;
}
void QZint::setPrimaryMessage(const QString & primaryMessage)
{
	m_primaryMessage=primaryMessage;
}

int QZint::height()
{
	encode();
	return (m_zintSymbol->height+(m_border!=NO_BORDER)?m_borderWidth*2:0)*(m_zintSymbol->symbology == BARCODE_MAXICODE?(maxi_width+1):1);
}

void QZint::setHeight(int height)
{
	m_height=height;
}

void QZint::setWidth(int width)
{
	m_width=width;
}

int QZint::width()
{
	encode();
	return (m_zintSymbol->width+(m_border==BOX)?m_borderWidth*2:0)*(m_zintSymbol->symbology == BARCODE_MAXICODE?(maxi_width+1):1);
}

float QZint::scale()
{
	return m_scale;
}

void QZint::setScale(float scale)
{
	m_scale = scale;
}

QColor QZint::fgColor()
{
	return m_fgColor;
}
void QZint::setFgColor(const QColor & fgColor)
{
	m_fgColor=fgColor;
}

QColor QZint::bgColor()
{
	return m_bgColor;
}
void QZint::setBgColor(const QColor & bgColor)
{
	m_bgColor=bgColor;
}

QZint::BorderType QZint::borderType()
{
	return m_border;
}
void QZint::setBorderType(BorderType border)
{
	m_border=border;
}

int QZint::borderWidth()
{
	return m_borderWidth;
}
void QZint::setBorderWidth(int boderWidth)
{
	if (boderWidth<1 || boderWidth>16)
		boderWidth=1;
	m_borderWidth=boderWidth;
}

void QZint::setWhitespace(int whitespace)
{
	m_whitespace = whitespace;
}

int QZint::pdf417CodeWords()
{
	return m_pdf417CodeWords;
}
void QZint::setPdf417CodeWords(int pdf417CodeWords)
{
	m_pdf417CodeWords=pdf417CodeWords;
}

int QZint::securityLevel()
{
	return m_securityLevel;
}
void QZint::setSecurityLevel(int securityLevel)
{
	m_securityLevel=securityLevel;
}

QString QZint::error_message()
{
	return m_lastError;
}

int QZint::mode()
{
	return m_securityLevel;
}
void QZint::setMode(int securityLevel)
{
	m_securityLevel=securityLevel;
}

bool QZint::save_to_file(QString filename)
{
	if (m_zintSymbol)
		ZBarcode_Delete(m_zintSymbol);
	
	QString fg_colour_hash = m_fgColor.name();
	QString bg_colour_hash = m_bgColor.name();

	m_lastError.clear();
	m_zintSymbol = ZBarcode_Create();
	m_zintSymbol->output_options=m_border;
	m_zintSymbol->symbology=m_symbol;
	m_zintSymbol->height=m_height;
	m_zintSymbol->whitespace_width=m_whitespace;
	m_zintSymbol->border_width=m_borderWidth;
	m_zintSymbol->option_1=m_securityLevel;
	m_zintSymbol->input_mode = m_input_mode;
	m_zintSymbol->option_2=m_width;
	m_zintSymbol->option_3=m_pdf417CodeWords;
	m_zintSymbol->scale=m_scale;
	QByteArray bstr=m_text.toAscii();
	QByteArray pstr=m_primaryMessage.left(99).toAscii();
	QByteArray fstr=filename.left(255).toAscii();
	strcpy(m_zintSymbol->primary,pstr.data());
	strcpy(m_zintSymbol->outfile,fstr.data());
	QByteArray fgcol=fg_colour_hash.right(6).toAscii();
	QByteArray bgcol=bg_colour_hash.right(6).toAscii();
	strcpy(m_zintSymbol->fgcolour,fgcol.data());
	strcpy(m_zintSymbol->bgcolour,bgcol.data());
	int error = ZBarcode_Encode_and_Print(m_zintSymbol, (unsigned char*)bstr.data());
	if (error > WARN_INVALID_OPTION)
		m_lastError=m_zintSymbol->errtxt;
	if(error == 0) { return true; } else { return false; }
}

int QZint::module_set(int y_coord, int x_coord)
{
	int x_char, x_sub, result;
	
	x_char = x_coord / 7;
	x_sub = x_coord % 7;
	result = 0;
	
	switch(x_sub) {
		case 0: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x01) != 0) { result = 1; } break;
		case 1: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x02) != 0) { result = 1; } break;
		case 2: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x04) != 0) { result = 1; } break;
		case 3: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x08) != 0) { result = 1; } break;
		case 4: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x10) != 0) { result = 1; } break;
		case 5: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x20) != 0) { result = 1; } break;
		case 6: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x40) != 0) { result = 1; } break;
	}
	
	return result;
}

void QZint::render(QPainter & painter, const QRectF & paintRect, AspectRatioMode mode)
{
	encode();
	bool textdone;
	int comp_offset = 0, xoffset = m_whitespace, j, main_width = 0, addon_text_height = 0;
	int yoffset = 0;
	QString caption = (const char*)m_zintSymbol->text;

	if (m_lastError.length())
	{
		painter.drawText(paintRect,Qt::AlignCenter,m_lastError);
		return;
	}

	painter.save();
	painter.setClipRect(paintRect,Qt::IntersectClip);
	painter.setRenderHint(QPainter::Antialiasing);
	qreal xtr=paintRect.x();
	qreal ytr=paintRect.y();

	int zrow_height=m_zintSymbol->height;
	int zrows=0;
	for (int i=0;i<m_zintSymbol->rows;i++)
	{
		zrow_height-=m_zintSymbol->row_height[i];
		if (!m_zintSymbol->row_height[i])
			zrows++;
	}
	if (zrows)
	{
		zrow_height/=zrows;
		for (int i=0;i<m_zintSymbol->rows;i++)
			if (!m_zintSymbol->row_height[i])
				m_zintSymbol->row_height[i]=zrow_height;
	}
	else
		m_zintSymbol->height-=zrow_height;


	qreal gwidth=m_zintSymbol->width;
	qreal gheight=m_zintSymbol->height;
	if (m_zintSymbol->symbology == BARCODE_MAXICODE)
	{
		gheight*=(maxi_width);
		gwidth*=(maxi_width+1);
	}

	qreal xsf=1;
	qreal ysf=1;
	qreal textoffset = 0;

	gwidth+=((m_border==BOX)?m_borderWidth*2:0);
	gheight+=((m_border!=NO_BORDER)?m_borderWidth*2:0);
	if(QString((const char*)m_zintSymbol->text).isEmpty() == false) {
		textoffset = 9;
		gheight += textoffset;
	} else {
		textoffset = 0;
	}
	gwidth+=m_zintSymbol->whitespace_width*2;
	switch(mode)
	{
		case IgnoreAspectRatio:
				xsf=(qreal)paintRect.width()/gwidth;
				ysf=(qreal)paintRect.height()/gheight;
			break;

		case KeepAspectRatio:
			if (paintRect.width()/gwidth<paintRect.height()/gheight)
			{
				ysf=xsf=(qreal)paintRect.width()/gwidth;
				ytr+=(qreal)(paintRect.height()-gheight*ysf)/2;
			}
			else
			{
				ysf=xsf=(qreal)paintRect.height()/gheight;
				xtr+=(qreal)(paintRect.width()-gwidth*xsf)/2;
			}
			break;

		case CenterBarCode:
				xtr+=((qreal)paintRect.width()-gwidth*xsf)/2;
				ytr+=((qreal)paintRect.height()-gheight*ysf)/2;
			break;
	}

	painter.setBackground(QBrush(m_bgColor));
	painter.fillRect(paintRect,QBrush(m_bgColor));
	painter.translate(xtr,ytr);
	painter.scale(xsf,ysf);

	QPen p;
	p.setColor(m_fgColor);
	p.setWidth(m_borderWidth);
	painter.setPen(p);

	QPainterPath pt;
	if(m_zintSymbol->symbology != BARCODE_MAXICODE) {
		/* Draw boundary bars or boxes around the symbol */
		switch(m_border)
		{
			case BOX:
				painter.fillRect(0,m_borderWidth,m_borderWidth,m_zintSymbol->height,QBrush(m_fgColor));
				painter.fillRect(m_zintSymbol->width + xoffset + xoffset + m_borderWidth,m_borderWidth,m_borderWidth,m_zintSymbol->height,QBrush(m_fgColor));
				painter.fillRect(0,0,m_zintSymbol->width + xoffset + xoffset + m_borderWidth + m_borderWidth,m_borderWidth,QBrush(m_fgColor));
				painter.fillRect(0,m_zintSymbol->height + m_borderWidth,m_zintSymbol->width + xoffset + xoffset + m_borderWidth + m_borderWidth, m_borderWidth,QBrush(m_fgColor));
				painter.translate(m_borderWidth+m_zintSymbol->whitespace_width,m_borderWidth);
				yoffset = m_borderWidth;
				break;
			case BIND:
				painter.fillRect(0,0,m_zintSymbol->width + xoffset + xoffset,m_borderWidth,QBrush(m_fgColor));
				painter.fillRect(0,m_zintSymbol->height + m_borderWidth,m_zintSymbol->width + xoffset + xoffset, m_borderWidth,QBrush(m_fgColor));
				painter.translate(m_zintSymbol->whitespace_width,m_borderWidth);
				yoffset = m_borderWidth;
				break;
	
			default:
				painter.translate(m_zintSymbol->whitespace_width,0);
				break;;
		}
	}

	while(!(module_set(m_zintSymbol->rows - 1, comp_offset))) {
		comp_offset++;
	}
	xoffset = comp_offset;
	
	/* Set up some values for displaying EAN and UPC symbols correctly */
	main_width = m_zintSymbol->width;
	if ((((m_zintSymbol->symbology == BARCODE_EANX) && (m_zintSymbol->rows == 1)) || (m_zintSymbol->symbology == BARCODE_EANX_CC))
		|| (m_zintSymbol->symbology == BARCODE_ISBNX)) {
		switch(caption.size()) {
			case 13: /* EAN 13 */
			case 16:
			case 19:
				if(m_zintSymbol->whitespace_width == 0) {
					m_zintSymbol->whitespace_width = 10;
				}
				main_width = 96 + comp_offset;
				break;
			default:
				main_width = 68 + comp_offset;
				break;
		}
	}
	
	if (((m_zintSymbol->symbology == BARCODE_UPCA) && (m_zintSymbol->rows == 1)) || (m_zintSymbol->symbology == BARCODE_UPCA_CC)) {
		if(m_zintSymbol->whitespace_width == 0) {
			m_zintSymbol->whitespace_width = 10;
		}
		main_width = 96 + comp_offset;
	}
	
	if (((m_zintSymbol->symbology == BARCODE_UPCE) && (m_zintSymbol->rows == 1)) || (m_zintSymbol->symbology == BARCODE_UPCE_CC)) {
		if(m_zintSymbol->whitespace_width == 0) {
			m_zintSymbol->whitespace_width = 10;
		}
		main_width = 51 + comp_offset;
	}
	
	p.setWidth(1);
	painter.setPen(p);

	if (m_zintSymbol->symbology == BARCODE_MAXICODE)
	{
		/* Draw Maxicode with hexagons */
		painter.save();
		painter.setRenderHint(QPainter::Antialiasing);
		for (int r=0;r<m_zintSymbol->rows;r++)
		{
			for (int c=0;c<m_zintSymbol->width;c++)
			{
				if (module_set(r, c))
				{
					qreal col=(qreal)c*(maxi_width+1)+(r%2)*((maxi_width+1)/2);
					qreal row=(qreal)r*(maxi_width+1)*0.868;
					QPainterPath pt;
					pt.moveTo(col+maxi_width/2, 	row);
					pt.lineTo(col+maxi_width, 	row+maxi_diagonal/4);
					pt.lineTo(col+maxi_width, 	row+(maxi_diagonal-maxi_diagonal/4));
					pt.lineTo(col+maxi_width/2, 	row+maxi_diagonal);
					pt.lineTo(col, 			row+(maxi_diagonal-maxi_diagonal/4));
					pt.lineTo(col, 			row+maxi_diagonal/4);
					pt.lineTo(col+maxi_width/2, 	row);
					painter.fillPath(pt,QBrush(m_fgColor));
				}
			}
		}
		p.setWidth(maxi_width);
		painter.setPen(p);
		const qreal w=maxi_width+1;
		painter.drawEllipse(QPointF(14.5*w,16.5*w*0.868),w,w);
		painter.drawEllipse(QPointF(14.5*w,16.5*w*0.868),w+w*1.5,w+w*1.5);
		painter.drawEllipse(QPointF(14.5*w,16.5*w*0.868),w+w*3,w+w*3);
		painter.restore();
	}
	else
	{
		/* Draw all other symbols with rectangles */
		int y=0;
		for (int row=0;row<m_zintSymbol->rows;row++)
		{
			for (int i=0;i<m_zintSymbol->width;i++) {
				if (module_set(row, i))
				{
					int ed = module_set(row, i);
					int linewidth=0;
					for (int j=i;j<m_zintSymbol->width;j++,linewidth++)
						if (ed != module_set(row, j))
							break;
					QColor color;
					/* switch(ed)
					{
						case 'R':
							color=qRgb(0xff,0x00,0x00);
							break;

						case 'G':
							color=qRgb(0x00,0xff,0x00);
							break;

						case 'B':
							color=qRgb(0x00,0x00,0xff);
							break;

						case 'C':
							color=qRgb(0x00,0xff,0xff);
							break;

						case 'M':
							color=qRgb(0xff,0x00,0xff);
							break;

						case 'Y':
							color=qRgb(0xff,0xff,0x00);
							break;

						default: */
							color=m_fgColor;
							/* break;
					} */
					if(!((i > main_width) && (row == m_zintSymbol->rows - 1)))  {
						painter.fillRect(i,y,linewidth,m_zintSymbol->row_height[row],QBrush(color));
					} else {
						painter.fillRect(i,y + 8,linewidth,m_zintSymbol->row_height[row] - 3,QBrush(color));
						addon_text_height = y;
					}
				}
			}
			/* Add row binding */
			if(((m_zintSymbol->symbology == BARCODE_CODE16K) || (m_zintSymbol->symbology == BARCODE_CODE49)) && (row != 0)) {
				painter.fillRect(0,y - 1,m_zintSymbol->width,2,QBrush(m_fgColor));
			}
			if(((m_zintSymbol->symbology == BARCODE_CODABLOCKF) || (m_zintSymbol->symbology == BARCODE_HIBC_BLOCKF)) && (row != 0)) {
				painter.fillRect(11,y - 1,m_zintSymbol->width - 24,2,QBrush(m_fgColor));
			}
			y+=m_zintSymbol->row_height[row];
		}
	}

	textdone = false;
	
	painter.setFont(QFont("Ariel", 4));
	if(((m_zintSymbol->symbology == BARCODE_EANX) || (m_zintSymbol->symbology == BARCODE_EANX_CC)) ||
		(m_zintSymbol->symbology == BARCODE_ISBNX)) {
		/* Add bridge and format text for EAN */
		switch(caption.size()) {
			case 8:
			case 11:
			case 14:
				painter.fillRect(0 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
				painter.fillRect(2 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
				painter.fillRect(32 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
				painter.fillRect(34 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
				painter.fillRect(64 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
				painter.fillRect(66 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
				painter.setFont(QFont("Ariel", 6));
				painter.drawText(3 + xoffset, m_zintSymbol->height + yoffset, 29, 9,Qt::AlignCenter, caption.mid(0,4));
				painter.drawText(35 + xoffset, m_zintSymbol->height + yoffset, 29, 9,Qt::AlignCenter, caption.mid(4,4));
				if(caption.size() == 11) { /* EAN-2 */ painter.drawText(76 + xoffset, addon_text_height, 20, 9,Qt::AlignCenter, caption.mid(9,2)); };
				if(caption.size() == 14) { /* EAN-5 */ painter.drawText(76 + xoffset, addon_text_height, 47, 9,Qt::AlignCenter, caption.mid(9,5)); };
				painter.setFont(QFont("Ariel", 4));
				textdone = true;
				break;
			case 13:
			case 16:
			case 19:
				painter.fillRect(0 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
				painter.fillRect(2 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
				painter.fillRect(46 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
				painter.fillRect(48 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
				painter.fillRect(92 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
				painter.fillRect(94 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
				painter.setFont(QFont("Ariel", 6));
				painter.drawText(xoffset - 7, m_zintSymbol->height + yoffset, 7, 9,Qt::AlignCenter, caption.mid(0,1));
				painter.drawText(3 + xoffset, m_zintSymbol->height + yoffset, 43, 9,Qt::AlignCenter, caption.mid(1,6));
				painter.drawText(49 + xoffset, m_zintSymbol->height + yoffset, 43, 9,Qt::AlignCenter, caption.mid(7,6));
				if(caption.size() == 16) { /* EAN-2 */ painter.drawText(104 + xoffset, addon_text_height, 20, 9,Qt::AlignCenter, caption.mid(14,2)); };
				if(caption.size() == 19) { /* EAN-5 */ painter.drawText(104 + xoffset, addon_text_height, 47, 9,Qt::AlignCenter, caption.mid(14,5)); };
				painter.setFont(QFont("Ariel", 4));
				textdone = true;
				break;
		}
		if(textdone == false) {
			painter.setFont(QFont("Ariel", 6));
			painter.drawText(0, m_zintSymbol->height, m_zintSymbol->width, 9,Qt::AlignCenter, caption);
			painter.setFont(QFont("Ariel", 4));
			textdone = true;
		}
	}
	
	if((m_zintSymbol->symbology == BARCODE_UPCA) || (m_zintSymbol->symbology == BARCODE_UPCA_CC)) {
		/* Add bridge and format text for UPC-A */
		int block_width;
		bool latch = true;
		
		j = 0 + comp_offset;
		do {
			block_width = 0;
			do {
				block_width++;
			} while (module_set(m_zintSymbol->rows - 1, j + block_width) == module_set(m_zintSymbol->rows - 1, j));
			if(latch == true) {
				/* a bar */
				painter.fillRect(j + xoffset - comp_offset,m_zintSymbol->height,block_width,5,QBrush(m_fgColor));
				latch = false;
			} else {
				/* a space */
				latch = true;
			}
			j += block_width;
		} while (j < 11 + comp_offset);
		painter.fillRect(46 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
		painter.fillRect(48 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
		latch = true;
		j = 85 + comp_offset;
		do {
			block_width = 0;
			do {
				block_width++;
			} while (module_set(m_zintSymbol->rows - 1, j + block_width) == module_set(m_zintSymbol->rows - 1, j));
			if(latch == true) {
				/* a bar */
				painter.fillRect(j + xoffset - comp_offset,m_zintSymbol->height,block_width,5,QBrush(m_fgColor));
				latch = false;
			} else {
				/* a space */
				latch = true;
			}
			j += block_width;
		} while (j < 96 + comp_offset);
		painter.drawText(xoffset - 7, m_zintSymbol->height + yoffset, 7, 7,Qt::AlignCenter, caption.mid(0,1));
		painter.drawText(96 + xoffset, m_zintSymbol->height + yoffset, 7, 7,Qt::AlignCenter, caption.mid(11,1));
		painter.setFont(QFont("Ariel", 6));
		painter.drawText(11 + xoffset, m_zintSymbol->height + yoffset, 35, 9,Qt::AlignCenter, caption.mid(1,5));
		painter.drawText(49 + xoffset, m_zintSymbol->height + yoffset, 35, 9,Qt::AlignCenter, caption.mid(6,5));
		if(caption.size() == 15) { /* EAN-2 */ painter.drawText(104 + xoffset, addon_text_height, 20, 9,Qt::AlignCenter, caption.mid(13,2)); };
		if(caption.size() == 18) { /* EAN-5 */ painter.drawText(104 + xoffset, addon_text_height, 47, 9,Qt::AlignCenter, caption.mid(13,5)); };
		painter.setFont(QFont("Ariel", 4));
		textdone = true;
	}
	
	if((m_zintSymbol->symbology == BARCODE_UPCE) || (m_zintSymbol->symbology == BARCODE_UPCE_CC)) {
		/* Add bridge and format text for UPC-E */
		painter.fillRect(0 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
		painter.fillRect(2 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
		painter.fillRect(46 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
		painter.fillRect(48 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
		painter.fillRect(50 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
		painter.drawText(xoffset - 7, m_zintSymbol->height + yoffset, 7, 7,Qt::AlignCenter, caption.mid(0,1));
		painter.drawText(51 + xoffset, m_zintSymbol->height + yoffset, 7, 7,Qt::AlignCenter, caption.mid(7,1));
		painter.setFont(QFont("Ariel", 6));
		painter.drawText(3 + xoffset, m_zintSymbol->height + yoffset, 43, 9,Qt::AlignCenter, caption.mid(1,6));
		if(caption.size() == 11) { /* EAN-2 */ painter.drawText(60 + xoffset, addon_text_height, 20, 9,Qt::AlignCenter, caption.mid(9,2)); };
		if(caption.size() == 14) { /* EAN-2 */ painter.drawText(60 + xoffset, addon_text_height, 47, 9,Qt::AlignCenter, caption.mid(9,5)); };
		painter.setFont(QFont("Ariel", 4));
		textdone = true;
	}
	
	if((caption.isEmpty() == false) && (textdone == false)) {
		/* Add text to any other symbol */
		painter.drawText(0, m_zintSymbol->height + yoffset, m_zintSymbol->width, 7,Qt::AlignCenter, caption);
	}
	painter.restore();
}

int QZint::ustrlen(unsigned char data[]) {
	/* Local replacement for strlen() with unsigned char strings */
	int i;
	for (i=0;data[i];i++);

	return i;
}

void QZint::to_upper(unsigned char source[])
{ /* Converts lower case characters to upper case in a string source[] */
	int i;

	for (i = 0; i < ustrlen(source); i++) {
		if ((source[i] >= 'a') && (source[i] <= 'z')) {
			source [i] = (source[i] - 'a') + 'A'; }
	}
}

int QZint::ctoi(char source)
{ /* Converts a character 0-9 to its equivalent integer value */
	if((source >= '0') && (source <= '9'))
		return (source - '0');
	return(source - 'A' + 10);
}

int QZint::is_stackable(int symbology) {
	/* Indicates which symbologies can have row binding */
	if(symbology < BARCODE_PDF417) { return 1; }
	if(symbology == BARCODE_CODE128B) { return 1; }
	if(symbology == BARCODE_ISBNX) { return 1; }
	if(symbology == BARCODE_EAN14) { return 1; }
	if(symbology == BARCODE_NVE18) { return 1; }
	if(symbology == BARCODE_KOREAPOST) { return 1; }
	if(symbology == BARCODE_PLESSEY) { return 1; }
	if(symbology == BARCODE_TELEPEN_NUM) { return 1; }
	if(symbology == BARCODE_ITF14) { return 1; }
	if(symbology == BARCODE_CODE32) { return 1; }
	
	return 0;
}

int QZint::is_extendable(int symbology) {
	/* Indicates which symbols can have addon */
	if(symbology == BARCODE_EANX) { return 1; }
	if(symbology == BARCODE_UPCA) { return 1; }
	if(symbology == BARCODE_UPCE) { return 1; }
	if(symbology == BARCODE_ISBNX) { return 1; }
	if(symbology == BARCODE_UPCA_CC) { return 1; }
	if(symbology == BARCODE_UPCE_CC) { return 1; }
	if(symbology == BARCODE_EANX_CC) { return 1; }
	
	return 0;
}

int QZint::roundup(float input)
{
	float remainder;
	int integer_part;
	
	integer_part = (int)input;
	remainder = input - integer_part;
	
	if(remainder > 0.1) {
		integer_part++;
	}
	
	return integer_part;
}

QString QZint::copy_to_clipboard()
{
	QString clipdata;
	clipdata.clear();
	int i, block_width, latch, r, this_row;
	float textpos, large_bar_height, preset_height, row_height, row_posn = 0.0;
	int fgred, fggrn, fgblu, bgred, bggrn, bgblu;
	float red_ink, green_ink, blue_ink, red_paper, green_paper, blue_paper;
	int textoffset, xoffset, yoffset, textdone, main_width;
	char textpart[10], addon[6];
	int large_bar_count, comp_offset;
	float addon_text_posn;
	float scaler = m_zintSymbol->scale;
	float default_text_posn;
	
	if (m_zintSymbol)
		ZBarcode_Delete(m_zintSymbol);
	
	QString fg_colour_hash = m_fgColor.name();
	QString bg_colour_hash = m_bgColor.name();

	m_lastError.clear();
	m_zintSymbol = ZBarcode_Create();
	m_zintSymbol->output_options=m_border;
	m_zintSymbol->symbology=m_symbol;
	m_zintSymbol->height=m_height;
	m_zintSymbol->whitespace_width=m_whitespace;
	m_zintSymbol->border_width=m_borderWidth;
	m_zintSymbol->option_1=m_securityLevel;
	m_zintSymbol->input_mode = m_input_mode;
	m_zintSymbol->option_2=m_width;
	m_zintSymbol->option_3=m_pdf417CodeWords;
	m_zintSymbol->scale=m_scale;
	QByteArray bstr=m_text.toAscii();
	QByteArray pstr=m_primaryMessage.left(99).toAscii();
	strcpy(m_zintSymbol->primary,pstr.data());
	QByteArray fgcol=fg_colour_hash.right(6).toAscii();
	QByteArray bgcol=bg_colour_hash.right(6).toAscii();
	strcpy(m_zintSymbol->fgcolour,fgcol.data());
	strcpy(m_zintSymbol->bgcolour,bgcol.data());
	int error = ZBarcode_Encode(m_zintSymbol, (unsigned char*)bstr.data());
	if (error > WARN_INVALID_OPTION) {
		m_lastError=m_zintSymbol->errtxt;
		return clipdata;
	}

	row_height=0;
	textdone = 0;
	main_width = m_zintSymbol->width;
	strcpy(addon, "");
	comp_offset = 0;
	addon_text_posn = 0.0;
	
	/* sort out colour options */
	to_upper((unsigned char*)m_zintSymbol->fgcolour);
	to_upper((unsigned char*)m_zintSymbol->bgcolour);
	
	fgred = (16 * ctoi(m_zintSymbol->fgcolour[0])) + ctoi(m_zintSymbol->fgcolour[1]);
	fggrn = (16 * ctoi(m_zintSymbol->fgcolour[2])) + ctoi(m_zintSymbol->fgcolour[3]);
	fgblu = (16 * ctoi(m_zintSymbol->fgcolour[4])) + ctoi(m_zintSymbol->fgcolour[5]);
	bgred = (16 * ctoi(m_zintSymbol->bgcolour[0])) + ctoi(m_zintSymbol->bgcolour[1]);
	bggrn = (16 * ctoi(m_zintSymbol->bgcolour[2])) + ctoi(m_zintSymbol->bgcolour[3]);
	bgblu = (16 * ctoi(m_zintSymbol->bgcolour[4])) + ctoi(m_zintSymbol->bgcolour[5]);
	red_ink = fgred / 256.0;
	green_ink = fggrn / 256.0;
	blue_ink = fgblu / 256.0;
	red_paper = bgred / 256.0;
	green_paper = bggrn / 256.0;
	blue_paper = bgblu / 256.0;
	
	if (m_zintSymbol->height == 0) {
		m_zintSymbol->height = 50;
	}
	
	large_bar_count = 0;
	preset_height = 0.0;
	for(i = 0; i < m_zintSymbol->rows; i++) {
		preset_height += m_zintSymbol->row_height[i];
		if(m_zintSymbol->row_height[i] == 0) {
			large_bar_count++;
		}
	}
	large_bar_height = (m_zintSymbol->height - preset_height) / large_bar_count;

	if (large_bar_count == 0) {
		m_zintSymbol->height = preset_height;
	}
	
	while(!(module_set(m_zintSymbol->rows - 1, comp_offset))) {
		comp_offset++;
	}

	/* Certain symbols need whitespace otherwise characters get chopped off the sides */
	if ((((m_zintSymbol->symbology == BARCODE_EANX) && (m_zintSymbol->rows == 1)) || (m_zintSymbol->symbology == BARCODE_EANX_CC))
		|| (m_zintSymbol->symbology == BARCODE_ISBNX)) {
		switch(ustrlen(m_zintSymbol->text)) {
			case 13: /* EAN 13 */
			case 16:
			case 19:
				if(m_zintSymbol->whitespace_width == 0) {
					m_zintSymbol->whitespace_width = 10;
				}
				main_width = 96 + comp_offset;
				break;
			default:
				main_width = 68 + comp_offset;
		}
	}
	
	if (((m_zintSymbol->symbology == BARCODE_UPCA) && (m_zintSymbol->rows == 1)) || (m_zintSymbol->symbology == BARCODE_UPCA_CC)) {
		if(m_zintSymbol->whitespace_width == 0) {
			m_zintSymbol->whitespace_width = 10;
			main_width = 96 + comp_offset;
		}
	}
	
	if (((m_zintSymbol->symbology == BARCODE_UPCE) && (m_zintSymbol->rows == 1)) || (m_zintSymbol->symbology == BARCODE_UPCE_CC)) {
		if(m_zintSymbol->whitespace_width == 0) {
			m_zintSymbol->whitespace_width = 10;
			main_width = 51 + comp_offset;
		}
	}
	
	latch = 0;
	r = 0;
	/* Isolate add-on text */
	if(is_extendable(m_zintSymbol->symbology)) {
		for(i = 0; i < ustrlen(m_zintSymbol->text); i++) {
			if (latch == 1) {
				addon[r] = m_zintSymbol->text[i];
				r++;
			}
			if (m_zintSymbol->text[i] == '+') {
				latch = 1;
			}
		}
	}
	addon[r] = '\0';
	
	if(ustrlen(m_zintSymbol->text) != 0) {
		textoffset = 9;
	} else {
		textoffset = 0;
	}
	xoffset = m_zintSymbol->border_width + m_zintSymbol->whitespace_width;
	yoffset = m_zintSymbol->border_width;
	
	/* Start writing the header */
	clipdata += "<?xml version=\"1.0\" standalone=\"no\"?>\n";
	clipdata += "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n";
	clipdata += "   \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
	if(m_zintSymbol->symbology != BARCODE_MAXICODE) {
		clipdata += QString("<svg width=\"%1\" height=\"%2\" version=\"1.1\"\n")
			.arg(roundup((m_zintSymbol->width + xoffset + xoffset) * scaler))
			.arg(roundup((m_zintSymbol->height + textoffset + yoffset + yoffset) * scaler));
	} else {
		clipdata += QString("<svg width=\"%1\" height=\"%2\" version=\"1.1\"\n")
			.arg(roundup((74.0 + xoffset + xoffset) * scaler))
			.arg(roundup((72.0 + yoffset + yoffset) * scaler));
	}
	clipdata += "   xmlns=\"http://www.w3.org/2000/svg\">\n";
	if(ustrlen(m_zintSymbol->text) != 0) {
		clipdata += "   <desc>";
		clipdata += QString((const char *)m_zintSymbol->text);
		clipdata += "\n";
	} else {
		clipdata += "   <desc>Zint Generated Symbol\n";
	}
	clipdata += "   </desc>\n";
	clipdata += "\n   <g id=\"barcode\" fill=\"#";
	clipdata += QString((const char *)m_zintSymbol->fgcolour);
	clipdata += "\">\n";

	if(m_zintSymbol->symbology != BARCODE_MAXICODE) {
		clipdata += QString("      <rect x=\"0\" y=\"0\" width=\"%1\" height=\"%2\" fill=\"#")
			.arg(roundup((m_zintSymbol->width + xoffset + xoffset) * scaler))
			.arg(roundup((m_zintSymbol->height + textoffset + yoffset + yoffset) * scaler));
		clipdata += QString((const char *)m_zintSymbol->bgcolour);
		clipdata += "\" />\n";
	} else {
		clipdata += QString("      <rect x=\"0\" y=\"0\" width=\"%1\" height=\"%2\" fill=\"#")
			.arg(roundup((74.0 + xoffset + xoffset) * scaler))
			.arg(roundup((72.0 + yoffset + yoffset) * scaler));
		clipdata += QString((const char *)m_zintSymbol->bgcolour);
		clipdata += "\" />\n";
	}

	if(((m_zintSymbol->output_options & BARCODE_BOX) != 0) || ((m_zintSymbol->output_options & BARCODE_BIND) != 0)) {
		default_text_posn = (m_zintSymbol->height + textoffset + m_zintSymbol->border_width + m_zintSymbol->border_width) * scaler;
	} else {
		default_text_posn = (m_zintSymbol->height + textoffset + m_zintSymbol->border_width) * scaler;
	}

	if(m_zintSymbol->symbology == BARCODE_MAXICODE) {
		/* Maxicode uses hexagons */
		float ax, ay, bx, by, cx, cy, dx, dy, ex, ey, fx, fy, mx, my;
		
				
		textoffset = 0.0;
		if (((m_zintSymbol->output_options & BARCODE_BOX) != 0) || ((m_zintSymbol->output_options & BARCODE_BIND) != 0)) {
			clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
				.arg(0.0, 0, 'f', 2)
				.arg(0.0, 0, 'f', 2)
				.arg((74.0 + xoffset + xoffset) * scaler, 0, 'f', 2)
				.arg(m_zintSymbol->border_width * scaler, 0, 'f', 2);
			clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
				.arg(0.0, 0, 'f', 2)
				.arg((72.0 + m_zintSymbol->border_width) * scaler, 0, 'f', 2)
				.arg((74.0 + xoffset + xoffset) * scaler, 0, 'f', 2)
				.arg(m_zintSymbol->border_width * scaler, 0, 'f', 2);
		}
		if((m_zintSymbol->output_options & BARCODE_BOX) != 0) {
			/* side bars */
			clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
				.arg(0.0, 0, 'f', 2)
				.arg(0.0, 0, 'f', 2)
				.arg(m_zintSymbol->border_width * scaler, 0, 'f', 2)
				.arg((72.0 + (2 * m_zintSymbol->border_width)) * scaler, 0, 'f', 2);
			clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
				.arg((74.0 + xoffset + xoffset - m_zintSymbol->border_width) * scaler, 0, 'f', 2)
				.arg(0.0, 0, 'f', 2)
				.arg(m_zintSymbol->border_width * scaler, 0, 'f', 2)
				.arg((72.0 + (2 * m_zintSymbol->border_width)) * scaler, 0, 'f', 2);
		}
		clipdata += QString("      <circle cx=\"%1\" cy=\"%2\" r=\"%3\" fill=\"#")
			.arg((35.76 + xoffset) * scaler, 0, 'f', 2)
			.arg((35.60 + yoffset) * scaler, 0, 'f', 2)
			.arg(10.85 * scaler, 0, 'f', 2);
		clipdata += QString((const char *)m_zintSymbol->fgcolour);
		clipdata += "\" />\n";
		clipdata += QString("      <circle cx=\"%1\" cy=\"%2\" r=\"%3\" fill=\"#")
			.arg((35.76 + xoffset) * scaler, 0, 'f', 2)
			.arg((35.60 + yoffset) * scaler, 0, 'f', 2)
			.arg(8.97 * scaler);
		clipdata += QString((const char *)m_zintSymbol->bgcolour);
		clipdata += "\" />\n";
		clipdata += QString("      <circle cx=\"%1\" cy=\"%2\" r=\"%3\" fill=\"#")
			.arg((35.76 + xoffset) * scaler, 0, 'f', 2)
			.arg((35.60 + yoffset) * scaler, 0, 'f', 2)
			.arg(7.10 * scaler, 0, 'f', 2);
		clipdata += QString((const char *)m_zintSymbol->fgcolour);
		clipdata += "\" />\n";
		clipdata += QString("      <circle cx=\"%1\" cy=\"%2\" r=\"%3\" fill=\"#")
			.arg((35.76 + xoffset) * scaler, 0, 'f', 2)
			.arg((35.60 + yoffset) * scaler, 0, 'f', 2)
			.arg(5.22 * scaler, 0, 'f', 2);
		clipdata += QString((const char *)m_zintSymbol->bgcolour);
		clipdata += "\" />\n";
		clipdata += QString("      <circle cx=\"%1\" cy=\"%2\" r=\"%3\" fill=\"#")
			.arg((35.76 + xoffset) * scaler, 0, 'f', 2)
			.arg((35.60 + yoffset) * scaler, 0, 'f', 2)
			.arg(3.31 * scaler, 0, 'f', 2);
		clipdata += QString((const char *)m_zintSymbol->fgcolour);
		clipdata += "\" />\n";
		clipdata += QString("      <circle cx=\"%1\" cy=\"%2\" r=\"%3\" fill=\"#")
			.arg((35.76 + xoffset) * scaler, 0, 'f', 2)
			.arg((35.60 + yoffset) * scaler, 0, 'f', 2)
			.arg(1.43 * scaler, 0, 'f', 2);
		clipdata += QString((const char *)m_zintSymbol->bgcolour);
		clipdata += "\" />\n";
		for(r = 0; r < m_zintSymbol->rows; r++) {
			for(i = 0; i < m_zintSymbol->width; i++) {
				if(module_set(r, i)) {
					/* Dump a hexagon */
					my = r * 2.135 + 1.43;
					ay = my + 1.0 + yoffset;
					by = my + 0.5 + yoffset;
					cy = my - 0.5 + yoffset;
					dy = my - 1.0 + yoffset;
					ey = my - 0.5 + yoffset;
					fy = my + 0.5 + yoffset;
					if(r % 2 == 1) {
						mx = (2.46 * i) + 1.23 + 1.23;
					} else {
						mx = (2.46 * i) + 1.23;
					}
					ax = mx + xoffset;
					bx = mx + 0.86 + xoffset;
					cx = mx + 0.86 + xoffset;
					dx = mx + xoffset;
					ex = mx - 0.86 + xoffset;
					fx = mx - 0.86 + xoffset;
					clipdata += QString("      <path d=\"M %1 %2 L %3 %4 L %5 %6 L %7 %8 L %9 %10 L %11 %12 Z\" />\n")
						.arg(ax * scaler, 0, 'f', 2)
						.arg(ay * scaler, 0, 'f', 2)
						.arg(bx * scaler, 0, 'f', 2)
						.arg(by * scaler, 0, 'f', 2)
						.arg(cx * scaler, 0, 'f', 2)
						.arg(cy * scaler, 0, 'f', 2)
						.arg(dx * scaler, 0, 'f', 2)
						.arg(dy * scaler, 0, 'f', 2)
						.arg(ex * scaler, 0, 'f', 2)
						.arg(ey * scaler, 0, 'f', 2)
						.arg(fx * scaler, 0, 'f', 2)
						.arg(fy * scaler, 0, 'f', 2);
				}
			}
		}
	}	
	
	if(m_zintSymbol->symbology != BARCODE_MAXICODE) {
		/* everything else uses rectangles (or squares) */
		/* Works from the bottom of the symbol up */
		int addon_latch = 0;
		
		for(r = 0; r < m_zintSymbol->rows; r++) {
			this_row = r;
			if(m_zintSymbol->row_height[this_row] == 0) {
				row_height = large_bar_height;
			} else {
				row_height = m_zintSymbol->row_height[this_row];
			}
			row_posn = 0;
			for(i = 0; i < r; i++) {
				if(m_zintSymbol->row_height[i] == 0) {
					row_posn += large_bar_height;
				} else {
					row_posn += m_zintSymbol->row_height[i];
				}
			}
			row_posn += yoffset;
			
			i = 0;
			if(module_set(this_row, 0)) {
				latch = 1;
			} else {
				latch = 0;
			}
			
			do {
				block_width = 0;
				do {
					block_width++;
				} while (module_set(this_row, i + block_width) == module_set(this_row, i));
				if((addon_latch == 0) && (r == 0) && (i > main_width)) {
					addon_text_posn = 9.0 + m_zintSymbol->border_width;
					addon_latch = 1;
				} 
				if(latch == 1) { 
					/* a bar */
					if(addon_latch == 0) {
						clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
							.arg((i + xoffset) * scaler, 0, 'f', 2)
							.arg(row_posn * scaler, 0, 'f', 2)
							.arg(block_width * scaler, 0, 'f', 2)
							.arg(row_height * scaler, 0, 'f', 2);
					} else {
						clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
							.arg((i + xoffset) * scaler, 0, 'f', 2)
							.arg((row_posn + 10.0) * scaler, 0, 'f', 2)
							.arg(block_width * scaler, 0, 'f', 2)
							.arg((row_height - 5.0) * scaler, 0, 'f', 2);
					}
					latch = 0;
				} else {
					/* a space */
					latch = 1;
				}
				i += block_width;
				
			} while (i < m_zintSymbol->width);
		}
	}
	/* That's done the actual data area, everything else is human-friendly */

	xoffset += comp_offset;
	row_posn = (row_posn + large_bar_height) * scaler;

	if ((((m_zintSymbol->symbology == BARCODE_EANX) && (m_zintSymbol->rows == 1)) || (m_zintSymbol->symbology == BARCODE_EANX_CC)) ||
		(m_zintSymbol->symbology == BARCODE_ISBNX)) {
		/* guard bar extensions and text formatting for EAN8 and EAN13 */
		switch(ustrlen(m_zintSymbol->text)) {
			case 8: /* EAN-8 */
			case 11:
			case 14:
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((0 + xoffset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2);
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((2 + xoffset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2);
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((32 + xoffset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2);
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((34 + xoffset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2);
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((64 + xoffset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2);
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((66 + xoffset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2); 
				for(i = 0; i < 4; i++) {
					textpart[i] = m_zintSymbol->text[i];
				}
				textpart[4] = '\0';
				textpos = 17;
				clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
					.arg((textpos + xoffset) * scaler, 0, 'f', 2)
					.arg(default_text_posn, 0, 'f', 2);
				clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
					.arg(11.0 * scaler, 0, 'f', 1);
				clipdata += QString((const char *)m_zintSymbol->fgcolour);
				clipdata += "\" >\n         ";
				clipdata += QString((const char *)textpart);
				clipdata += "\n      </text>\n";
				for(i = 0; i < 4; i++) {
					textpart[i] = m_zintSymbol->text[i + 4];
				}
				textpart[4] = '\0';
				textpos = 50;
				clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
					.arg((textpos + xoffset) * scaler, 0, 'f', 2)
					.arg(default_text_posn, 0, 'f', 2);
				clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
					.arg(11.0 * scaler, 0, 'f', 1);
				clipdata += QString((const char *)m_zintSymbol->fgcolour);
				clipdata += "\" >\n         ";
				clipdata += QString((const char *)textpart);
				clipdata += "\n      </text>\n";
				textdone = 1;
				switch(strlen(addon)) {
					case 2:	
						textpos = m_zintSymbol->width + xoffset - 10;
						clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
							.arg(textpos * scaler, 0, 'f', 2)
							.arg(addon_text_posn * scaler, 0, 'f', 2);
						clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
							.arg(11.0 * scaler, 0, 'f', 1);
						clipdata += QString((const char *)m_zintSymbol->fgcolour);
						clipdata += "\" >\n         ";
						clipdata += QString((const char *)addon);
						clipdata += "\n      </text>\n";
						break;
					case 5:
						textpos = m_zintSymbol->width + xoffset - 23;
						clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
							.arg(textpos * scaler, 0, 'f', 2)
							.arg(addon_text_posn * scaler, 0, 'f', 2);
						clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
							.arg(11.0 * scaler, 0, 'f', 1);
						clipdata += QString((const char *)m_zintSymbol->fgcolour);
						clipdata += "\" >\n         ";
						clipdata += QString((const char *)addon);
						clipdata += "\n      </text>\n";
						break;
				}

				break;
			case 13: /* EAN 13 */
			case 16:
			case 19:
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((0 + xoffset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2);
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((2 + xoffset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2);
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((46 + xoffset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2);
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((48 + xoffset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2);
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((92 + xoffset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2);
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((94 + xoffset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2); 
				textpart[0] = m_zintSymbol->text[0];
				textpart[1] = '\0';
				textpos = -7;
				clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
					.arg((textpos + xoffset) * scaler, 0, 'f', 2)
					.arg(default_text_posn, 0, 'f', 2);
				clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
					.arg(11.0 * scaler, 0, 'f', 1);
				clipdata += QString((const char *)m_zintSymbol->fgcolour);
				clipdata += "\" >\n         ";
				clipdata += QString((const char *)textpart);
				clipdata += "\n      </text>\n";
				for(i = 0; i < 6; i++) {
					textpart[i] = m_zintSymbol->text[i + 1];
				}
				textpart[6] = '\0';
				textpos = 24;
				clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
					.arg((textpos + xoffset) * scaler, 0, 'f', 2)
					.arg(default_text_posn, 0, 'f', 2);
				clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
					.arg(11.0 * scaler, 0, 'f', 1);
				clipdata += QString((const char *)m_zintSymbol->fgcolour);
				clipdata += "\" >\n         ";
				clipdata += QString((const char *)textpart);
				clipdata += "\n      </text>\n";
				for(i = 0; i < 6; i++) {
					textpart[i] = m_zintSymbol->text[i + 7];
				}
				textpart[6] = '\0';
				textpos = 71;
				clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
					.arg((textpos + xoffset) * scaler, 0, 'f', 2)
					.arg(default_text_posn, 0, 'f', 2);
				clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
					.arg(11.0 * scaler, 0, 'f', 1);
				clipdata += "\" >\n         ";
				clipdata += QString((const char *)textpart);
				clipdata += "\n      </text>\n";
				textdone = 1;
				switch(strlen(addon)) {
					case 2:	
						textpos = m_zintSymbol->width + xoffset - 10;
						clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
							.arg(textpos * scaler, 0, 'f', 2)
							.arg(addon_text_posn * scaler, 0, 'f', 2);
						clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
							.arg(11.0 * scaler, 0, 'f', 1);
						clipdata += QString((const char *)m_zintSymbol->fgcolour);
						clipdata += "\" >\n         ";
						clipdata += QString((const char *)addon);
						clipdata += "\n      </text>\n";
						break;
					case 5:
						textpos = m_zintSymbol->width + xoffset - 23;
						clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
							.arg(textpos * scaler, 0, 'f', 2)
							.arg(addon_text_posn * scaler, 0, 'f', 2);
						clipdata += QString("         font-family=\"Helvetica\" font-size=\"%2\" fill=\"#")
							.arg(11.0 * scaler, 0, 'f', 1);
						clipdata += QString((const char *)m_zintSymbol->fgcolour);
						clipdata += "\" >\n         ";
						clipdata += QString((const char *)addon);
						clipdata += "\n      </text>\n";
						break;
				}
				break;

		}
	}	

	if (((m_zintSymbol->symbology == BARCODE_UPCA) && (m_zintSymbol->rows == 1)) || (m_zintSymbol->symbology == BARCODE_UPCA_CC)) {
		/* guard bar extensions and text formatting for UPCA */
		latch = 1;
		
		i = 0 + comp_offset;
		do {
			block_width = 0;
			do {
				block_width++;
			} while (module_set(m_zintSymbol->rows - 1, i + block_width) == module_set(m_zintSymbol->rows - 1, i));
			if(latch == 1) {
				/* a bar */
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((i + xoffset - comp_offset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(block_width * scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2);
				latch = 0;
			} else {
				/* a space */
				latch = 1;
			}
			i += block_width;
		} while (i < 11 + comp_offset);
		clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
			.arg((46 + xoffset) * scaler, 0, 'f', 2)
			.arg(row_posn, 0, 'f', 2)
			.arg(scaler, 0, 'f', 2)
			.arg(5.0 * scaler, 0, 'f', 2);
		clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
			.arg((48 + xoffset) * scaler, 0, 'f', 2)
			.arg(row_posn, 0, 'f', 2)
			.arg(scaler, 0, 'f', 2)
			.arg(5.0 * scaler, 0, 'f', 2);
		latch = 1;
		i = 85 + comp_offset;
		do {
			block_width = 0;
			do {
				block_width++;
			} while (module_set(m_zintSymbol->rows - 1, i + block_width) == module_set(m_zintSymbol->rows - 1, i));
			if(latch == 1) {
				/* a bar */
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((i + xoffset - comp_offset) * scaler, 0, 'f', 2)
					.arg(row_posn, 0, 'f', 2)
					.arg(block_width * scaler, 0, 'f', 2)
					.arg(5.0 * scaler, 0, 'f', 2);
				latch = 0;
			} else {
				/* a space */
				latch = 1;
			}
			i += block_width;
		} while (i < 96 + comp_offset);
		textpart[0] = m_zintSymbol->text[0];
		textpart[1] = '\0';
		textpos = -5;
		clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
			.arg((textpos + xoffset) * scaler, 0, 'f', 2)
			.arg(default_text_posn, 0, 'f', 2);
		clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
			.arg(8.0 * scaler, 0, 'f', 1);
		clipdata += QString((const char *)m_zintSymbol->fgcolour);
		clipdata += "\" >\n         ";
		clipdata += QString((const char *)textpart);
		clipdata += "\n      </text>\n";
		for(i = 0; i < 5; i++) {
			textpart[i] = m_zintSymbol->text[i + 1];
		}
		textpart[5] = '\0';
		textpos = 27;
		clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
			.arg((textpos + xoffset) * scaler, 0, 'f', 2)
			.arg(default_text_posn, 0, 'f', 2);
		clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
			.arg(11.0 * scaler, 0, 'f', 1);
		clipdata += QString((const char *)m_zintSymbol->fgcolour);
		clipdata += "\" >\n         ";
		clipdata += QString((const char *)textpart);
		clipdata += "\n      </text>\n";
		for(i = 0; i < 5; i++) {
			textpart[i] = m_zintSymbol->text[i + 6];
		}
		textpart[6] = '\0';
		textpos = 68;
		clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
			.arg((textpos + xoffset) * scaler, 0, 'f', 2)
			.arg(default_text_posn, 0, 'f', 2);
		clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
			.arg(11.0 * scaler, 0, 'f', 1);
		clipdata += QString((const char *)m_zintSymbol->fgcolour);
		clipdata += "\" >\n         ";
		clipdata += QString((const char *)textpart);
		clipdata += "\n      </text>\n";
		textpart[0] = m_zintSymbol->text[11];
		textpart[1] = '\0';
		textpos = 100;
		clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
			.arg((textpos + xoffset) * scaler, 0, 'f', 2)
			.arg(default_text_posn, 0, 'f', 2);
		clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
			.arg(8.0 * scaler, 0, 'f', 1);
		clipdata += QString((const char *)m_zintSymbol->fgcolour);
		clipdata += "\" >\n         ";
		clipdata += QString((const char *)textpart);
		clipdata += "\n      </text>\n";
		textdone = 1;
		switch(strlen(addon)) {
			case 2:	
				textpos = m_zintSymbol->width + xoffset - 10;
				clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
					.arg(textpos * scaler, 0, 'f', 2)
					.arg(addon_text_posn * scaler, 0, 'f', 2);
				clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
					.arg(11.0 * scaler, 0, 'f', 1);
				clipdata += QString((const char *)m_zintSymbol->fgcolour);
				clipdata += "\" >\n         ";
				clipdata += QString((const char *)addon);
				clipdata += "\n      </text>\n";
				break;
			case 5:
				textpos = m_zintSymbol->width + xoffset - 23;
				clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
					.arg(textpos * scaler, 0, 'f', 2)
					.arg(addon_text_posn * scaler, 0, 'f', 2);
				clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
					.arg(11.0 * scaler, 0, 'f', 1);
				clipdata += QString((const char *)m_zintSymbol->fgcolour);
				clipdata += "\" >\n         ";
				clipdata += QString((const char *)addon);
				clipdata += "\n      </text>\n";
				break;
		}

	}	

	if (((m_zintSymbol->symbology == BARCODE_UPCE) && (m_zintSymbol->rows == 1)) || (m_zintSymbol->symbology == BARCODE_UPCE_CC)) {
		/* guard bar extensions and text formatting for UPCE */
		clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
			.arg((0 + xoffset) * scaler, 0, 'f', 2)
			.arg(row_posn, 0, 'f', 2)
			.arg(scaler, 0, 'f', 2)
			.arg(5.0 * scaler, 0, 'f', 2);
		clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
			.arg((2 + xoffset) * scaler, 0, 'f', 2)
			.arg(row_posn, 0, 'f', 2)
			.arg(scaler, 0, 'f', 2)
			.arg(5.0 * scaler, 0, 'f', 2);
		clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
			.arg((46 + xoffset) * scaler, 0, 'f', 2)
			.arg(row_posn, 0, 'f', 2)
			.arg(scaler, 0, 'f', 2)
			.arg(5.0 * scaler, 0, 'f', 2);
		clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
			.arg((48 + xoffset) * scaler, 0, 'f', 2)
			.arg(row_posn, 0, 'f', 2)
			.arg(scaler, 0, 'f', 2)
			.arg(5.0 * scaler, 0, 'f', 2);
		clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
			.arg((50 + xoffset) * scaler, 0, 'f', 2)
			.arg(row_posn, 0, 'f', 2)
			.arg(scaler, 0, 'f', 2)
			.arg(5.0 * scaler, 0, 'f', 2);
		textpart[0] = m_zintSymbol->text[0];
		textpart[1] = '\0';
		textpos = -5;
		clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
			.arg((textpos + xoffset) * scaler, 0, 'f', 2)
			.arg(default_text_posn, 0, 'f', 2);
		clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
			.arg(8.0 * scaler, 0, 'f', 1);
		clipdata += QString((const char *)m_zintSymbol->fgcolour);
		clipdata += "\" >\n         ";
		clipdata += QString((const char *)textpart);
		clipdata += "\n      </text>\n";
		for(i = 0; i < 6; i++) {
			textpart[i] = m_zintSymbol->text[i + 1];
		}
		textpart[6] = '\0';
		textpos = 24;
		clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
			.arg((textpos + xoffset) * scaler, 0, 'f', 2)
			.arg(default_text_posn, 0, 'f', 2);
		clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
			.arg(11.0 * scaler, 0, 'f', 1);
		clipdata += QString((const char *)m_zintSymbol->fgcolour);
		clipdata += "\" >\n         ";
		clipdata += QString((const char *)textpart);
		clipdata += "\n      </text>\n";
		textpart[0] = m_zintSymbol->text[7];
		textpart[1] = '\0';
		textpos = 55;
		clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
			.arg((textpos + xoffset) * scaler, 0, 'f', 2)
			.arg(default_text_posn, 0, 'f', 2);
		clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
			.arg(8.0 * scaler, 0, 'f', 1);
		clipdata += QString((const char *)m_zintSymbol->fgcolour);
		clipdata += "\" >\n         ";
		clipdata += QString((const char *)textpart);
		clipdata += "\n      </text>\n";
		textdone = 1;
		switch(strlen(addon)) {
			case 2:	
				textpos = m_zintSymbol->width + xoffset - 10;
				clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
					.arg(textpos * scaler, 0, 'f', 2)
					.arg(addon_text_posn * scaler, 0, 'f', 2);
				clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
					.arg(11.0 * scaler, 0, 'f', 1);
				clipdata += QString((const char *)m_zintSymbol->fgcolour);
				clipdata += "\" >\n         ";
				clipdata += QString((const char *)addon);
				clipdata += "\n      </text>\n";
				break;
			case 5:
				textpos = m_zintSymbol->width + xoffset - 23;
				clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
					.arg(textpos * scaler, 0, 'f', 2)
					.arg(addon_text_posn * scaler, 0, 'f', 2);
				clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
					.arg(11.0 * scaler, 0, 'f', 1);
				clipdata += QString((const char *)m_zintSymbol->fgcolour);
				clipdata += "\" >\n         ";
				clipdata += QString((const char *)addon);
				clipdata += "\n      </text>\n";
				break;
		}

	}

	xoffset -= comp_offset;

	switch(m_zintSymbol->symbology) {
		case BARCODE_CODABLOCKF:
		case BARCODE_HIBC_BLOCKF:
			clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
				.arg(xoffset * scaler, 0, 'f', 2)
				.arg(0.0, 0, 'f', 2)
				.arg(m_zintSymbol->width * scaler, 0, 'f', 2)
				.arg(m_zintSymbol->border_width * scaler, 0, 'f', 2);
			clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
				.arg(xoffset * scaler, 0, 'f', 2)
				.arg((m_zintSymbol->height + m_zintSymbol->border_width) * scaler, 0, 'f', 2)
				.arg(m_zintSymbol->width * scaler, 0, 'f', 2)
				.arg(m_zintSymbol->border_width * scaler, 0, 'f', 2);
			if(m_zintSymbol->rows > 1) {
				/* row binding */
				for(r = 1; r < m_zintSymbol->rows; r++) {
					clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
						.arg((xoffset + 11) * scaler, 0, 'f', 2)
						.arg(((r * row_height) + yoffset - 1) * scaler, 0, 'f', 2)
						.arg((m_zintSymbol->width - 24) * scaler, 0, 'f', 2)
						.arg(2.0 * scaler, 0, 'f', 2);
				}
			}
			break;
		case BARCODE_MAXICODE:
			/* Do nothing! (It's already been done) */
			break;
		default:
			if((m_zintSymbol->output_options & BARCODE_BIND) != 0) {
				if((m_zintSymbol->rows > 1) && (is_stackable(m_zintSymbol->symbology) == 1)) {
					/* row binding */
					for(r = 1; r < m_zintSymbol->rows; r++) {
						clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
							.arg(xoffset * scaler, 0, 'f', 2)
							.arg(((r * row_height) + yoffset - 1) * scaler, 0, 'f', 2)
							.arg(m_zintSymbol->width * scaler, 0, 'f', 2)
							.arg(2.0 * scaler, 0, 'f', 2);
					}
				}
			}
			if (((m_zintSymbol->output_options & BARCODE_BOX) != 0) || ((m_zintSymbol->output_options & BARCODE_BIND) != 0)) {
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg(0.0, 0, 'f', 2)
					.arg(0.0, 0, 'f', 2)
					.arg((m_zintSymbol->width + xoffset + xoffset) * scaler, 0, 'f', 2)
					.arg(m_zintSymbol->border_width * scaler, 0, 'f', 2);
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg(0.0, 0, 'f', 2)
					.arg((m_zintSymbol->height + m_zintSymbol->border_width) * scaler, 0, 'f', 2)
					.arg((m_zintSymbol->width + xoffset + xoffset) * scaler, 0, 'f', 2)
					.arg(m_zintSymbol->border_width * scaler, 0, 'f', 2);
			}
			if((m_zintSymbol->output_options & BARCODE_BOX) != 0) {
				/* side bars */
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%5\" />\n")
					.arg(0.0, 0, 'f', 2)
					.arg(0.0, 0, 'f', 2)
					.arg(m_zintSymbol->border_width * scaler, 0, 'f', 2)
					.arg((m_zintSymbol->height + (2 * m_zintSymbol->border_width)) * scaler, 0, 'f', 2);
				clipdata += QString("      <rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" />\n")
					.arg((m_zintSymbol->width + xoffset + xoffset - m_zintSymbol->border_width) * scaler, 0, 'f', 2)
					.arg(0.0, 0, 'f', 2)
					.arg(m_zintSymbol->border_width * scaler, 0, 'f', 2)
					.arg((m_zintSymbol->height + (2 * m_zintSymbol->border_width)) * scaler, 0, 'f', 2);
			}
			break;
	}
	
	/* Put the human readable text at the bottom */
	if((textdone == 0) && (ustrlen(m_zintSymbol->text) != 0)) {
		textpos = m_zintSymbol->width / 2.0;
		clipdata += QString("      <text x=\"%1\" y=\"%2\" text-anchor=\"middle\"\n")
			.arg((textpos + xoffset) * scaler, 0, 'f', 2)
			.arg(default_text_posn, 0, 'f', 2);
		clipdata += QString("         font-family=\"Helvetica\" font-size=\"%1\" fill=\"#")
			.arg(8.0 * scaler, 0, 'f', 1);
		clipdata += QString((const char *)m_zintSymbol->fgcolour);
		clipdata += "\" >\n         ";
		clipdata += QString((const char *)m_zintSymbol->text);
		clipdata += "\n      </text>\n";
	}
	clipdata += "   </g>\n";
	clipdata += "</svg>\n";
	
	return clipdata;
}

const QString & QZint::lastError()
{
	return m_lastError;
}

bool QZint::hasErrors()
{
	return m_lastError.length();
}

}

