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
#include <QPainter>

#include "barcoderender.h"

namespace Zint
{

BareCode::BareCode()
{
	m_symbol=BARCODE_CODE128;
	m_height=50;
	m_border=NO_BORDER;
	m_boderWidth=0;
	m_width=0;
	m_securityLevel=-1;
	m_pdf417CodeWords=928;
	m_fgColor=Qt::black;
	m_bgColor=Qt::white;
	m_zintSymbol=0;
}

BareCode::~BareCode()
{
	if (m_zintSymbol)
		ZBarcode_Delete(m_zintSymbol);
}

void BareCode::encode()
{
	if (m_zintSymbol)
		ZBarcode_Delete(m_zintSymbol);

	m_lastError.clear();
	m_zintSymbol = ZBarcode_Create();
	m_zintSymbol->symbology=m_symbol;
	m_zintSymbol->height=m_height;
	m_zintSymbol->output_options=0;
	m_zintSymbol->whitespace_width=0;
	m_zintSymbol->border_width=0;
	m_zintSymbol->option_1=m_securityLevel;
	m_zintSymbol->option_2=m_width;
	m_zintSymbol->option_3=m_pdf417CodeWords;
	QByteArray bstr=m_text.toAscii();
	QByteArray pstr=m_primaryMessage.left(99).toAscii();
	strcpy(m_zintSymbol->primary,pstr.data());
	int error = ZBarcode_Encode(m_zintSymbol, (unsigned char*)bstr.data());
	if (error > WARN_INVALID_OPTION)
		m_lastError=m_zintSymbol->errtxt;
	else
	{
		if (m_zintSymbol->symbology != BARCODE_MAXICODE)
		{
			int preset_height = 0;

			for (int i = 0; i < m_zintSymbol->rows; i++)
				preset_height += m_zintSymbol->row_height[i];

			if (preset_height)
				m_zintSymbol->height = preset_height;
		}
		else
			m_zintSymbol->height = 33;
	}
}

void BareCode::setSymbol(int symbol)
{
	m_symbol=symbol;
}

void BareCode::setText(const QString & text)
{
	m_text=text;
}

void BareCode::setPrimaryMessage(const QString & primaryMessage)
{
	m_primaryMessage=primaryMessage;
}

void BareCode::setHeight(int height)
{
	m_height=height;
}

int BareCode::height()
{
	encode();
	return m_zintSymbol->height+(m_border==BOX)?m_boderWidth*2:0;
}

void BareCode::setBorder(BorderType border)
{
	m_border=border;
}

void BareCode::setBorderWidth(int boderWidth)
{
	m_boderWidth=boderWidth;
}

void BareCode::setWidth(int width)
{
	m_width=width;
}

int BareCode::width()
{
	encode();
	return m_zintSymbol->width+(m_border!=NO_BORDER)?m_boderWidth*2:0;
}

void BareCode::setSecurityLevel(int securityLevel)
{
	m_securityLevel=securityLevel;
}

void BareCode::setPdf417CodeWords(int pdf417CodeWords)
{
	m_pdf417CodeWords=pdf417CodeWords;
}

void BareCode::setFgColor(const QColor & fgColor)
{
	m_fgColor=fgColor;
}

void BareCode::setBgColor(const QColor & bgColor)
{
	m_bgColor=bgColor;
}

const QString & BareCode::lastError()
{
	return m_lastError;
}

bool BareCode::hasErrors()
{
	return m_lastError.length();
}

void BareCode::render(QPainter & painter, const QRectF & paintRect, AspectRatioMode mode, qreal scaleFactor)
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

	qreal xsf=scaleFactor;
	qreal ysf=scaleFactor;

	qreal gwidth=scaleFactor*m_zintSymbol->width;
	qreal gheight=scaleFactor*m_zintSymbol->height;
	if (m_zintSymbol->symbology == BARCODE_MAXICODE)
	{
		gheight*=12;
		gwidth*=12;
	}

	gwidth+=scaleFactor*((m_border==BOX)?m_boderWidth*2:0);
	gheight+=scaleFactor*((m_border!=NO_BORDER)?m_boderWidth*2:0);

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
			painter.drawLine(gwidth-m_boderWidth/2, m_boderWidth/2,gwidth-m_boderWidth/2, gheight-m_boderWidth/2);
			painter.drawLine(gwidth-m_boderWidth/2, gheight-m_boderWidth/2, m_boderWidth/2, gheight-m_boderWidth/2);
			painter.drawLine(m_boderWidth/2, gheight-m_boderWidth/2, m_boderWidth/2,m_boderWidth/2);
			painter.translate(m_boderWidth,m_boderWidth);
			break;

		case BIND:
			painter.drawLine(m_boderWidth/2,m_boderWidth/2,gwidth-m_boderWidth/2, m_boderWidth/2);
			painter.drawLine(m_boderWidth/2,gheight-m_boderWidth/2,gwidth-m_boderWidth/2, gheight-m_boderWidth/2);
			painter.translate(0,m_boderWidth);
			break;

		default:
			break;;
	}

	p.setWidth(1);
	painter.setPen(p);

	if (1==m_zintSymbol->rows && !m_zintSymbol->row_height[0])
		m_zintSymbol->row_height[0]=m_zintSymbol->height;

	if (m_zintSymbol->symbology == BARCODE_MAXICODE)
	{
		painter.save();
		painter.setRenderHint(QPainter::Antialiasing);

		for (int row=0;row<m_zintSymbol->rows;row++)
			for (int i=0;i<m_zintSymbol->width;i++)
				if (m_zintSymbol->encoded_data[row][i]=='1')
				{
					QPainterPath pt;
					pt.moveTo(i*12+4, row*12);
					pt.lineTo(i*12+4+4, row*12);
					pt.lineTo(i*12+12, row*12+4);
					pt.lineTo(i*12+12, row*12+4+4);
					pt.lineTo(i*12+4+4, row*12+12);
					pt.lineTo(i*12+4, row*12+12);
					pt.lineTo(i*12, row*12+4+4);
					pt.lineTo(i*12, row*12+4);
					pt.lineTo(i*12+4, row*12);
					painter.fillPath(pt,QBrush(m_fgColor));
				}

		p.setWidth(8);
		painter.setPen(p);
		painter.drawEllipse(QPointF((qreal)m_zintSymbol->width/2*12,m_zintSymbol->height/2*12),12+4,12+4);
		painter.drawEllipse(QPointF((qreal)m_zintSymbol->width/2*12,m_zintSymbol->height/2*12),12+8+8+4,12+8+8+4);
		painter.drawEllipse(QPointF((qreal)m_zintSymbol->width/2*12,m_zintSymbol->height/2*12),12+8+8+8+8+4,12+8+8+8+8+4);
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

}

