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
	char bstyle_text[][40] = {
		"Code 11", "Code 2 of 5 Matrix", "Code 2 of 5 Interleaved", "IATA Code 2 of 5",
  "Code 2 of 5 Data Logic", "Code 2 of 5 Industrial", "Code 39", "Extended Code 39",
  "EAN", "EAN-128", "Codabar", "Code 128", "DP Leitcode", "DP Identcode",
  "Code 16k", "Code 93", "Flattermarken", "Databar",
  "Databar Limited", "Databar Expanded", "Telepen", "UPC-A", "UPC-E",
  "Postnet", "MSI Plessey", "FIM", "LOGMARS", "Pharmacode", "PZN",
  "Pharmacode 2-track", "PDF417", "PDF417 Truncated", "Maxicode",
  "QR Code", "Code 128 Subset B", "Auspost Standard", "Auspost Reply",
  "Auspost Route", "Auspost Redirect", "ISBN", "Royal Mail 4-state",
  "Data Matrix", "EAN-14", "Codablock-F", "NVE-18", "Databar Stacked",
  "Databar Stacked Omni", "Databar Expanded Stacked", "PLANET",
  "MicroPDF417", "USPS One Code", "UK Plessey", "Telepen Numeric",
  "ITF-14", "Dutch Post KIX", "Aztec Code", "Micro QR Code", "Aztec Runes",
  "Code 32", "Composite EAN", "Composite EAN-128", "Composite Databar",
  "Composite Databar Limited", "Composite Databar Expanded",
  "Composite UPC-A", "Composite UPC-E", "Composite Databar Stacked",
  "Composite Databar Stacked Omni", "Composite Databar Expanded Stacked"
	};
	setupUi(this);
	view->setScene(new QGraphicsScene);
	m_fgcolor=qRgb(0,0,0);
	m_bgcolor=qRgb(0xff,0xff,0xff);
	for (int i=0;i<metaObject()->enumerator(0).keyCount();i++) {
		bstyle->addItem(metaObject()->enumerator(0).key(i));
		bstyle->setItemText(i,bstyle_text[i]);
	}
	bstyle->setCurrentIndex(11);
	on_generate_clicked();
	view->scene()->addItem(&m_bc);
	connect(bstyle, SIGNAL(currentIndexChanged( int )), SLOT(on_generate_clicked()));
	connect(heightb, SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(widthb,  SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(security,  SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(bwidth,  SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(codewords,  SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(iwidth,  SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(iheight,  SIGNAL(valueChanged( int )), SLOT(on_generate_clicked()));
	connect(btype, SIGNAL(currentIndexChanged( int )), SLOT(on_generate_clicked()));
	connect(primary, SIGNAL(textChanged( const QString& )), SLOT(on_generate_clicked()));
	connect(text, SIGNAL(textChanged()), SLOT(on_generate_clicked()));
	connect(aspectRatio, SIGNAL(currentIndexChanged( int )), SLOT(on_generate_clicked()));

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
	m_bc.w=iwidth->value();
	m_bc.h=iheight->value();
	m_bc.ar=(Zint::QZint::AspectRatioMode)aspectRatio->currentIndex();
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
	m_bc.bc.setBorderType((Zint::QZint::BorderType)(btype->currentIndex()*2));
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

void MainWindow::on_zoomIn_clicked()
{
	view->scale(1.2,1.2);
}

void MainWindow::on_zoomOut_clicked()
{
	view->scale(0.9,0.9);
}

void MainWindow::on_rotateAc_clicked()
{
	view->rotate(-5);
}
void MainWindow::on_rotateC_clicked()
{
	view->rotate(5);
}
void MainWindow::on_reset_clicked()
{
	view->resetTransform();
}
