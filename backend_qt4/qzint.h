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
#include <QColor>
#include <QPainter>

#include "zint.h"

namespace Zint
{

class QZint
#ifndef NO_QT_KEYWORDS
 : public QObject
#endif
{
#ifndef NO_QT_KEYWORDS
Q_OBJECT
	Q_PROPERTY(int symbol READ symbol WRITE setSymbol)
	Q_PROPERTY(QString text READ text WRITE setText)
	Q_PROPERTY(QString primaryMessage READ primaryMessage WRITE setPrimaryMessage)
	Q_PROPERTY(int height READ height WRITE setHeight)
	Q_PROPERTY(int width READ width WRITE setWidth)
	Q_PROPERTY(QColor fgColor READ fgColor WRITE setFgColor)
	Q_PROPERTY(QColor bgColor READ bgColor WRITE setBgColor)
	Q_PROPERTY(BorderType borderType READ borderType WRITE setBorderType)
	Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth)
	Q_PROPERTY(int pdf417CodeWords READ pdf417CodeWords WRITE setPdf417CodeWords)
	Q_PROPERTY(int securityLevel READ securityLevel WRITE setSecurityLevel)
	Q_PROPERTY(int msiExtraSymbology READ msiExtraSymbology WRITE setMsiExtraSymbology)
	Q_PROPERTY(int code39ExtraSymbology READ code39ExtraSymbology WRITE setCode39ExtraSymbology)
	Q_PROPERTY(int excode39ExtraSymbology READ excode39ExtraSymbology WRITE setExcode39ExtraSymbology)
	Q_ENUMS(BorderType) 
#endif

public:

enum BorderType{NO_BORDER=0, BIND=1, BOX=2};
enum AspectRatioMode{IgnoreAspectRatio=0, KeepAspectRatio=1, CenterBarCode=2};

public:
	QZint();
	~QZint();

	int  symbol();
	void setSymbol(int symbol);

	QString text();
	void setText(const QString & text);

	QString primaryMessage();
	void setPrimaryMessage(const QString & primaryMessage);

	void setHeight(int height);
	int height();

	void setWidth(int width);
	int width();

	QColor fgColor();
	void setFgColor(const QColor & fgColor);

	QColor bgColor();
	void setBgColor(const QColor & bgColor);

	BorderType borderType();
	void setBorderType(BorderType border);

	int borderWidth();
	void setBorderWidth(int boderWidth);

	int pdf417CodeWords();
	void setPdf417CodeWords(int pdf417CodeWords);

	int securityLevel();
	void setSecurityLevel(int securityLevel);

	int msiExtraSymbology();
	void setMsiExtraSymbology(int msiSymbologyNumber);

	int  code39ExtraSymbology();
	void setCode39ExtraSymbology(int m_code39SymbologyNumber);

	int excode39ExtraSymbology();
	void setExcode39ExtraSymbology(int excode39SymbologyNumber);

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
	int m_msiSymbologyNumber;
	int m_code39SymbologyNumber;
	int m_excode39SymbologyNumber;
	QColor m_fgColor;
	QColor m_bgColor;
	QString m_lastError;
	int m_error;
	zint_symbol * m_zintSymbol;
};
}
#endif
