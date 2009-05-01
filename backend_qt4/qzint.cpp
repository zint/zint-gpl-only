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

namespace Zint
{

static const qreal maxi_diagonal=11;
static const qreal maxi_width=1.73205807568877*maxi_diagonal/2;

QZint::QZint()
{
	m_symbol=BARCODE_CODE128;
	m_height=50;
	m_border=NO_BORDER;
	m_boderWidth=1;
	m_securityLevel=-1;
	m_pdf417CodeWords=928;
	m_fgColor=Qt::black;
	m_bgColor=Qt::white;
	m_zintSymbol=0;
	m_error=0;
	m_input_mode = UNICODE_MODE;
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
	m_zintSymbol->output_options=0;
	m_zintSymbol->whitespace_width=0;
	m_zintSymbol->border_width=m_boderWidth;
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
	return (m_zintSymbol->height+(m_border!=NO_BORDER)?m_boderWidth*2:0)*(m_zintSymbol->symbology == BARCODE_MAXICODE?(maxi_width+1):1);
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
	return (m_zintSymbol->width+(m_border==BOX)?m_boderWidth*2:0)*(m_zintSymbol->symbology == BARCODE_MAXICODE?(maxi_width+1):1);
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
	return m_boderWidth;
}
void QZint::setBorderWidth(int boderWidth)
{
	if (boderWidth<1 || boderWidth>16)
		boderWidth=1;
	m_boderWidth=boderWidth;
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

int QZint::mode()
{
	return m_securityLevel;
}
void QZint::setMode(int securityLevel)
{
	m_securityLevel=securityLevel;
}

void QZint::render(QPainter & painter, const QRectF & paintRect, AspectRatioMode mode)
{
	encode();
	bool textdone;
	int comp_offset = 0, xoffset = 0, j, main_width = 0, addon_text_height = 0;
	QString caption = (const char*)m_zintSymbol->text;

	if (m_lastError.length())
	{
		painter.drawText(paintRect,Qt::AlignCenter,m_lastError);
		return;
	}

	painter.save();
	painter.setClipRect(paintRect,Qt::IntersectClip);
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

	gwidth+=((m_border==BOX)?m_boderWidth*2:0);
	gheight+=((m_border!=NO_BORDER)?m_boderWidth*2:0);
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
	p.setWidth(m_boderWidth);
	painter.setPen(p);

	QPainterPath pt;
	switch(m_border)
	{
		case BOX:
			painter.drawLine(m_boderWidth/2,m_boderWidth/2, gwidth-m_boderWidth/2, m_boderWidth/2);
			painter.drawLine(gwidth-m_boderWidth/2, m_boderWidth/2, gwidth-m_boderWidth/2, gheight-m_boderWidth/2);
			painter.drawLine(m_boderWidth/2, gheight-m_boderWidth/2, gwidth-m_boderWidth/2, gheight-m_boderWidth/2);
			painter.drawLine(m_boderWidth/2, gheight-m_boderWidth/2, m_boderWidth/2,m_boderWidth/2);
			painter.translate(m_boderWidth+m_zintSymbol->whitespace_width,m_boderWidth);
			break;

		case BIND:
			painter.drawLine(m_boderWidth/2,m_boderWidth/2,gwidth-m_boderWidth/2, m_boderWidth/2);
			painter.drawLine(m_boderWidth/2,gheight-m_boderWidth/2,gwidth-m_boderWidth/2, gheight-m_boderWidth/2);
			painter.translate(m_zintSymbol->whitespace_width,m_boderWidth);
			break;

		default:
			painter.translate(m_zintSymbol->whitespace_width,0);
			break;;
	}

	while(m_zintSymbol->encoded_data[m_zintSymbol->rows - 1][comp_offset] != '1') {
		comp_offset++;
	}
	xoffset += comp_offset;
	
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
			main_width = 96 + comp_offset;
		}
	}
	
	if (((m_zintSymbol->symbology == BARCODE_UPCE) && (m_zintSymbol->rows == 1)) || (m_zintSymbol->symbology == BARCODE_UPCE_CC)) {
		if(m_zintSymbol->whitespace_width == 0) {
			m_zintSymbol->whitespace_width = 10;
			main_width = 51 + comp_offset;
		}
	}
	
	p.setWidth(1);
	painter.setPen(p);

	if (m_zintSymbol->symbology == BARCODE_MAXICODE)
	{
		painter.save();
		painter.setRenderHint(QPainter::Antialiasing);
		for (int r=0;r<m_zintSymbol->rows;r++)
		{
			for (int c=0;c<m_zintSymbol->width;c++)
			{
				if (m_zintSymbol->encoded_data[r][c]=='1')
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
		int y=0;
		for (int row=0;row<m_zintSymbol->rows;row++)
		{
			for (int i=0;i<m_zintSymbol->width;i++)
				if (m_zintSymbol->encoded_data[row][i]!='0')
				{
					int ed=m_zintSymbol->encoded_data[row][i];
					int linewidth=0;
					for (int j=i;j<m_zintSymbol->width;j++,linewidth++)
						if (ed!=m_zintSymbol->encoded_data[row][j])
							break;
					QColor color;
					switch(ed)
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

						default:
							color=m_fgColor;
							break;
					}
					if(!((i > main_width) && (row == m_zintSymbol->rows - 1)))  {
						painter.fillRect(i,y,linewidth,m_zintSymbol->row_height[row],QBrush(color));
					} else {
						painter.fillRect(i,y + 8,linewidth,m_zintSymbol->row_height[row] - 3,QBrush(color));
						addon_text_height = y;
					}
				}
			y+=m_zintSymbol->row_height[row];
		}
	}

	textdone = false;
	
	painter.setFont(QFont("Ariel", 4));
	if(((m_zintSymbol->symbology == BARCODE_EANX) || (m_zintSymbol->symbology == BARCODE_EANX_CC)) ||
		(m_zintSymbol->symbology == BARCODE_ISBNX)) {
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
				painter.drawText(3 + xoffset, m_zintSymbol->height, 29, 9,Qt::AlignCenter, caption.mid(0,4));
				painter.drawText(35 + xoffset, m_zintSymbol->height, 29, 9,Qt::AlignCenter, caption.mid(4,4));
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
				painter.drawText(xoffset - 7, m_zintSymbol->height, 7, 9,Qt::AlignCenter, caption.mid(0,1));
				painter.drawText(3 + xoffset, m_zintSymbol->height, 43, 9,Qt::AlignCenter, caption.mid(1,6));
				painter.drawText(49 + xoffset, m_zintSymbol->height, 43, 9,Qt::AlignCenter, caption.mid(7,6));
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
		int block_width;
		bool latch = true;
		
		j = 0 + comp_offset;
		do {
			block_width = 0;
			do {
				block_width++;
			} while (m_zintSymbol->encoded_data[m_zintSymbol->rows - 1][j + block_width] == m_zintSymbol->encoded_data[m_zintSymbol->rows - 1][j]);
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
			} while (m_zintSymbol->encoded_data[m_zintSymbol->rows - 1][j + block_width] == m_zintSymbol->encoded_data[m_zintSymbol->rows - 1][j]);
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
		painter.drawText(xoffset - 7, m_zintSymbol->height, 7, 7,Qt::AlignCenter, caption.mid(0,1));
		painter.drawText(96 + xoffset, m_zintSymbol->height, 7, 7,Qt::AlignCenter, caption.mid(11,1));
		painter.setFont(QFont("Ariel", 6));
		painter.drawText(11 + xoffset, m_zintSymbol->height, 35, 9,Qt::AlignCenter, caption.mid(1,5));
		painter.drawText(49 + xoffset, m_zintSymbol->height, 35, 9,Qt::AlignCenter, caption.mid(6,5));
		if(caption.size() == 15) { /* EAN-2 */ painter.drawText(104 + xoffset, addon_text_height, 20, 9,Qt::AlignCenter, caption.mid(13,2)); };
		if(caption.size() == 18) { /* EAN-5 */ painter.drawText(104 + xoffset, addon_text_height, 47, 9,Qt::AlignCenter, caption.mid(13,5)); };
		painter.setFont(QFont("Ariel", 4));
		textdone = true;
	}
	
	if((m_zintSymbol->symbology == BARCODE_UPCE) || (m_zintSymbol->symbology == BARCODE_UPCE_CC)) {
		painter.fillRect(0 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
		painter.fillRect(2 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
		painter.fillRect(46 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
		painter.fillRect(48 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
		painter.fillRect(50 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
		painter.drawText(xoffset - 7, m_zintSymbol->height, 7, 7,Qt::AlignCenter, caption.mid(0,1));
		painter.drawText(51 + xoffset, m_zintSymbol->height, 7, 7,Qt::AlignCenter, caption.mid(7,1));
		painter.setFont(QFont("Ariel", 6));
		painter.drawText(3 + xoffset, m_zintSymbol->height, 43, 9,Qt::AlignCenter, caption.mid(1,6));
		if(caption.size() == 11) { /* EAN-2 */ painter.drawText(60 + xoffset, addon_text_height, 20, 9,Qt::AlignCenter, caption.mid(9,2)); };
		if(caption.size() == 14) { /* EAN-2 */ painter.drawText(60 + xoffset, addon_text_height, 47, 9,Qt::AlignCenter, caption.mid(9,5)); };
		painter.setFont(QFont("Ariel", 4));
		textdone = true;
	}
	
	if((caption.isEmpty() == false) && (textdone == false)) {
		painter.drawText(0, m_zintSymbol->height, m_zintSymbol->width, 7,Qt::AlignCenter, caption);
	}
	painter.restore();
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

