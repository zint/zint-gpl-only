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

#include <QDebug>
#include <QGraphicsScene>
#include <QImage>
#include <QColorDialog>


#include "mainwindow.h"


MainWindow::MainWindow(QWidget* parent, Qt::WFlags fl)
		: QWidget(parent, fl)
{
	setupUi(this);
	view->setScene(new QGraphicsScene);
	m_fgcolor=qRgb(0,0,0);
	m_bgcolor=qRgb(0xff,0xff,0xff);
	for (int i=0;i<metaObject()->enumerator(0).keyCount();i++)
		bstyle->addItem(metaObject()->enumerator(0).key(i));
	on_generate_clicked();
	view->scene()->addItem(&m_bc);
	connect(bstyle, SIGNAL(currentIndexChanged( int )), SLOT(on_generate_clicked()));
	connect(heightb, SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(widthb,  SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(security,  SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(scaleFactor,  SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(bwidth,  SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(codewords,  SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(iwidth,  SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(iheight,  SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(btype, SIGNAL(currentIndexChanged( int )), SLOT(on_generate_clicked()));
	connect(primary, SIGNAL(textChanged( const QString& )), SLOT(on_generate_clicked()));
	connect(text, SIGNAL(textChanged()), SLOT(on_generate_clicked()));

}

MainWindow::~MainWindow()
{
}

void MainWindow::on_fgcolor_clicked()
{
	m_fgcolor=QColorDialog::getColor(m_fgcolor,this);
	on_generate_clicked();
}

void MainWindow::on_bgcolor_clicked()
{
	m_bgcolor=QColorDialog::getColor(m_bgcolor,this);
	on_generate_clicked();
}

void MainWindow::on_generate_clicked()
{
	QString error;
	m_bc.scaleFactor=scaleFactor->value();
	m_bc.w=iwidth->value();
	m_bc.h=iheight->value();
	m_bc.bc.setText(text->toPlainText());
	m_bc.bc.setPrimaryMessage(primary->text());
	m_bc.bc.setSymbol(metaObject()->enumerator(0).value(bstyle->currentIndex()));
	switch(m_bc.bc.symbol())
	{
		case  BARCODE_MSI_PLESSEY:
			m_bc.bc.setMsiExtraSymbology(msiPlessey->currentIndex());
			break;
		case  BARCODE_CODE39:
			m_bc.bc.setCode39ExtraSymbology(code39->currentIndex());
			break;
		case  BARCODE_EXCODE39:
			m_bc.bc.setExcode39ExtraSymbology(code39->currentIndex());
			break;
	}
	m_bc.bc.setBorderType((Zint::QZint::BorderType)btype->currentIndex());
	m_bc.bc.setBorderWidth(bwidth->value());
	m_bc.bc.setHeight(heightb->value());
	m_bc.bc.setWidth(widthb->value());
	m_bc.bc.setSecurityLevel(security->value());
	m_bc.bc.setPdf417CodeWords(codewords->value());
	m_bc.bc.setFgColor(m_fgcolor);
	m_bc.bc.setBgColor(m_bgcolor);
	m_bc.update();
	view->scene()->update();
}

