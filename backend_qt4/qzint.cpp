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
	m_excode39SymbologyNumber=m_code39SymbologyNumber=m_msiSymbologyNumber=0;
	m_error=0;
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
	switch (m_symbol)
	{
		case BARCODE_MSI_PLESSEY:
			m_zintSymbol->option_2=m_msiSymbologyNumber;
			break;

		case BARCODE_CODE39:
			m_zintSymbol->option_2=m_code39SymbologyNumber;
			break;
		
		case BARCODE_EXCODE39:
			m_zintSymbol->option_2=m_excode39SymbologyNumber;
			break;

		default:
			m_zintSymbol->option_2=m_width;
			break;
	}
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

int QZint::msiExtraSymbology()
{
	return m_msiSymbologyNumber;
}
void QZint::setMsiExtraSymbology(int msiSymbologyNumber)
{
	m_msiSymbologyNumber=msiSymbologyNumber;
}

int  QZint::code39ExtraSymbology()
{
	return m_code39SymbologyNumber;
}
void QZint::setCode39ExtraSymbology(int m_code39SymbologyNumber)
{
	m_code39SymbologyNumber=m_code39SymbologyNumber;
}

int QZint::excode39ExtraSymbology()
{
	return m_excode39SymbologyNumber;
}
void QZint::setExcode39ExtraSymbology(int excode39SymbologyNumber)
{
	m_excode39SymbologyNumber=excode39SymbologyNumber;
}

void QZint::render(QPainter & painter, const QRectF & paintRect, AspectRatioMode mode)
{
	encode();

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
		gheight*=(maxi_width+1);
		gwidth*=(maxi_width+1);
	}

	qreal xsf=1;
	qreal ysf=1;

	gwidth+=((m_border==BOX)?m_boderWidth*2:0);
	gheight+=((m_border!=NO_BORDER)?m_boderWidth*2:0);
	gwidth+=m_zintSymbol->whitespace_width*2;
	switch(mode)
	{
		case IgnoreAspectRatio:
				xsf=(qreal)paintRect.width()/gwidth;
				ysf=(qreal)paintRect.height()/gheight;
			break;

		case KeepAspectRatio:
			if (paintRect.width()-gwidth<paintRect.height()-gheight)
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
					qreal row=(qreal)r*(maxi_width+1);
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
		painter.drawEllipse(QPointF((qreal)m_zintSymbol->width/2*w,(qreal)m_zintSymbol->height/2*w),w,w);
		painter.drawEllipse(QPointF((qreal)m_zintSymbol->width/2*w,(qreal)m_zintSymbol->height/2*w),w+w*1.5,w+w*1.5);
		painter.drawEllipse(QPointF((qreal)m_zintSymbol->width/2*w,(qreal)m_zintSymbol->height/2*w),w+w*3,w+w*3);
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
					painter.fillRect(i,y,linewidth,m_zintSymbol->row_height[row],QBrush(color));
				}
			y+=m_zintSymbol->row_height[row];
		}
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

