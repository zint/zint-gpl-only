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

#ifndef BARCODERENDER_H
#define BARCODERENDER_H
#include <QImage>
#include <QColor>

#include "zint.h"

namespace Zint
{

class BareCode
{
public:

enum BorderType{NO_BORDER=0, BIND=1, BOX=2};
enum AspectRatioMode{IgnoreAspectRatio=0, KeepAspectRatio=1, CenterBarCode=2};

public:
	BareCode();
	~BareCode();

	void setSymbol(int symbol);
	void setText(const QString & text);
	void setPrimaryMessage(const QString & primaryMessage);
	void setHeight(int height);
	int height();
	void setBorder(BorderType border);
	void setBorderWidth(int boderWidth);
	void setWidth(int width);
	int width();
	void setSecurityLevel(int securityLevel);
	void setPdf417CodeWords(int pdf417CodeWords);
	void setFgColor(const QColor & fgColor);
	void setBgColor(const QColor & bgColor);

	void render(QPainter & painter, const QRectF & paintRect, AspectRatioMode mode=IgnoreAspectRatio, qreal scaleFactor=1);

	const QString & lastError();
	bool hasErrors();

private:
	void encode();

private:
	int m_symbol;
	QString m_text;
	QString m_primaryMessage;
	int m_height;
	BorderType m_border;
	int m_boderWidth;
	int m_width;
	int m_securityLevel;
	int m_pdf417CodeWords;
	QColor m_fgColor;
	QColor m_bgColor;
	QString m_lastError;
	zint_symbol * m_zintSymbol;
};
}
#endif
