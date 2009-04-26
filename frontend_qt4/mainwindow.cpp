/***************************************************************************
 *   Copyright (C) 2008 by BogDan Vatra <bogdan@licentia.eu>               *
 *   Copyright (C) 2009 by Robin Stuart <robin@zint.org.uk>                *
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
#include <stdio.h>

MainWindow::MainWindow(QWidget* parent, Qt::WFlags fl)
		: QWidget(parent, fl)
{
	char bstyle_text[][50] = {
		"Australia Post Standard Customer",
		"Australia Post Reply-Paid",
		"Australia Post Routing Code",
		"Australia Post Redirect Code",
		"Aztec Code",
		"Aztec Runes",
		"Code 11",
		"Code 128",
		"Code 16k",
		"Code 2 of 5 Data Logic",
		"Code 2 of 5 IATA",
		"Code 2 of 5 Industrial",
		"Code 2 of 5 Interleaved",
		"Code 2 of 5 Matrix",
		"Code 32 (Italian Pharmacode)",
		"Code 39",
		"Code 39 Extended",
		"Code 93", 
		"Codabar",
		"Codablock-F",
		"Databar",
		"Databar Expanded",
		"Databar Expanded Stacked",
		"Databar Limited",
		"Databar Stacked",
		"Databar Stacked Omnidirectional",
		"Data Matrix",
		"Deutsche Post Identcode",
		"Deutsche Post Leitcode",
		"Dutch Post KIX",
		"EAN-14",
		"European Article Number (EAN)",
		"FIM",
		"Flattermarken",
		"ITF-14",
		"International Standard Book Number (ISBN)",
		"Japanese Postal Barcode",
		"Korean Postal Barcode",
		"LOGMARS",
		"Maxicode",
		"MicroPDF417",
		"Micro QR Code",
		"MSI Plessey",
		"NVE-18",
		"PDF417",
		"Pharmacode",
		"Pharmacode 2-track",
		"Pharma Zentralnummer (PZN)",
		"PLANET",
		"Postnet",
		"QR Code",
		"Royal Mail 4-state Barcode",
		"Telepen",
		"Telepen Numeric",
		"UK Plessey",
		"UPC-A",
		"UPC-E",
		"USPS One Code"
	};
	setupUi(this);
	view->setScene(new QGraphicsScene);
	m_fgcolor=qRgb(0,0,0);
	m_bgcolor=qRgb(0xff,0xff,0xff);
	for (int i=0;i<metaObject()->enumerator(0).keyCount();i++) {
		bstyle->addItem(metaObject()->enumerator(0).key(i));
		bstyle->setItemText(i,bstyle_text[i]);
	}
	bstyle->setCurrentIndex(7);
	change_options();
	update_preview();
	view->scene()->addItem(&m_bc);
	connect(bstyle, SIGNAL(currentIndexChanged( int )), SLOT(change_options()));
	connect(bstyle, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(heightb, SIGNAL(valueChanged( int )), SLOT(update_preview()));
	connect(bwidth,  SIGNAL(valueChanged( int )), SLOT(update_preview()));
	connect(codewords,  SIGNAL(valueChanged( int )), SLOT(update_preview()));
	connect(btype, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(txtData, SIGNAL(textChanged( const QString& )), SLOT(update_preview()));
	connect(txtComposite, SIGNAL(textChanged()), SLOT(update_preview()));
	connect(chkComposite, SIGNAL(stateChanged( int )), SLOT(composite_enable()));
	connect(chkComposite, SIGNAL(stateChanged( int )), SLOT(update_preview()));
	connect(cmbCompType, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(radC128EAN, SIGNAL(toggled( bool )), SLOT(composite_ean_check()));
	connect(radC128Stand, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radC128CSup, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radC128EAN, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radC128HIBC, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(cmbPDFECC, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(cmbPDFCols, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(radPDFTruncated, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radPDFStand, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radPDFHIBC, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radAztecSize, SIGNAL(toggled( bool )), SLOT(aztec_size()));
	connect(radAztecECC, SIGNAL(toggled( bool )), SLOT(aztec_errorcorrect()));
	connect(radAztecAuto, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radAztecSize, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radAztecECC, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(cmbAztecSize, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(cmbAztecECC, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(chkAztecMode, SIGNAL(stateChanged( int )), SLOT(update_preview()));
	connect(cmbMSICheck, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(radC39Stand, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radC39Check, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radC39HIBC, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(cmbMPDFCols, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(radMPDFStand, SIGNAL(toggled( bool )), SLOT(update_preview()));
	connect(radC16kStand, SIGNAL(toggled( bool )), SLOT(update_preview()));
	connect(radCodaStand, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radCodaGS1, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radCodaHIBC, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(cmbDMMode, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(cmbDMMode, SIGNAL(currentIndexChanged( int )), SLOT(datamatrix_options()));
	connect(radDM200Stand, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radDM200GS1, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radDM200HIBC, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(cmbDM200Size, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(cmbDMNon200Size, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(radQRAuto, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radQRSize, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radQRECC, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radQRSize, SIGNAL(toggled( bool )), SLOT(qr_size()));
	connect(radQRECC, SIGNAL(toggled( bool )), SLOT(qr_errorcorrect()));
	connect(cmbQRSize, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(cmbQRECC, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(chkQRHIBC, SIGNAL(stateChanged( int )), SLOT(update_preview()));
	connect(radMQRAuto, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radMQRSize, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radMQRECC, SIGNAL(clicked( bool )), SLOT(update_preview()));
	connect(radMQRSize, SIGNAL(toggled( bool )), SLOT(mqr_size()));
	connect(radMQRECC, SIGNAL(toggled( bool )), SLOT(mqr_errorcorrect()));
	connect(cmbMQRSize, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(cmbMQRECC, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(cmbMaxiMode, SIGNAL(currentIndexChanged( int )), SLOT(update_preview()));
	connect(cmbMaxiMode, SIGNAL(currentIndexChanged( int )), SLOT(maxi_primary()));
	connect(txtMaxiPrimary, SIGNAL(textChanged( const QString& )), SLOT(update_preview()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_fgcolor_clicked()
{
	m_fgcolor=QColorDialog::getColor(m_fgcolor,this);
	update_preview();
}

void MainWindow::on_bgcolor_clicked()
{
	m_bgcolor=QColorDialog::getColor(m_bgcolor,this);
	update_preview();
}

void MainWindow::change_options()
{
	bool options;

	options = false;

	if(metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_CODE128)
	{
		options = true;
		grpC128->show();
		chkComposite->setText(tr("Add 2D Component (GS1-128 only)"));
	} else {
		grpC128->hide();
		chkComposite->setText(tr("Add 2D Component"));
	}

	if(metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_PDF417)
	{
		options = true;
		grpPDF417->show();
	} else {
		grpPDF417->hide();
	}

	if(metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_MICROPDF417)
	{
		options = true;
		grpMicroPDF->show();
	} else {
		grpMicroPDF->hide();
	}

	if(metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_AZTEC)
	{
		options = true;
		grpAztec->show();
	} else {
		grpAztec->hide();
	}

	if(metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_MSI_PLESSEY)
	{
		options = true;
		grpMSICheck->show();
	} else {
		grpMSICheck->hide();
	}

	if((metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_CODE39) ||
		(metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_EXCODE39))
	{
		options = true;
		grpC39->show();
	} else {
		grpC39->hide();
	}

	if(metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_EXCODE39) {
		if(radC39HIBC->isChecked() == true) {
			radC39HIBC->setChecked(false);
			radC39Stand->setChecked(true);
		}
		radC39HIBC->setEnabled(false);
	} else {
		radC39HIBC->setEnabled(true);
	}

	if(metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_CODABLOCKF)
	{
		options = true;
		grpCodablock->show();
	} else {
		grpCodablock->hide();
	}

	if(metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_CODE16K)
	{
		options = true;
		grpC16k->show();
	} else {
		grpC16k->hide();
	}

	if(metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_DATAMATRIX)
	{
		options = true;
		datamatrix_options();
		grpDM->show();
	} else {
		grpDM->hide();
	}

	if(metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_QRCODE)
	{
		options = true;
		grpQR->show();
	} else {
		grpQR->hide();
	}

	if(metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_MICROQR)
	{
		options = true;
		grpMQR->show();
	} else {
		grpMQR->hide();
	}

	if(metaObject()->enumerator(0).value(bstyle->currentIndex()) == BARCODE_MAXICODE)
	{
		grpMaxiCode->show();
	} else {
		grpMaxiCode->hide();
	}


	if(options == true) {
		lblNoOption->hide();
	} else {
		lblNoOption->show();
	}

	switch(metaObject()->enumerator(0).value(bstyle->currentIndex()))
	{
		case BARCODE_CODE128:
		case BARCODE_EANX:
		case BARCODE_UPCA:
		case BARCODE_UPCE:
		case BARCODE_RSS14:
		case BARCODE_RSS_LTD:
		case BARCODE_RSS_EXP:
		case BARCODE_RSS14STACK:
		case BARCODE_RSS14STACK_OMNI:
		case BARCODE_RSS_EXPSTACK:
			grpComposite->show();
			break;
		default:
			chkComposite->setChecked(false);
			grpComposite->hide();
			break;
	}
}

void MainWindow::composite_enable()
{
	if(chkComposite->isChecked() == true) {
		lblCompType->setEnabled(true);
		cmbCompType->setEnabled(true);
		lblComposite->setEnabled(true);
		txtComposite->setEnabled(true);
		radC128EAN->setChecked(true);
	} else {
		lblCompType->setEnabled(false);
		cmbCompType->setEnabled(false);
		lblComposite->setEnabled(false);
		txtComposite->setEnabled(false);
	}
}

void MainWindow::composite_ean_check()
{
	if(radC128EAN->isChecked() == false) {
		chkComposite->setChecked(false);
	}
}

void MainWindow::aztec_size()
{
	if(radAztecSize->isChecked() == true) {
		cmbAztecSize->setEnabled(true);
	} else {
		cmbAztecSize->setEnabled(false);
	}
}

void MainWindow::aztec_errorcorrect()
{
	if(radAztecECC->isChecked() == true) {
		cmbAztecECC->setEnabled(true);
	} else {
		cmbAztecECC->setEnabled(false);
	}
}

void MainWindow::qr_size()
{
	if(radQRSize->isChecked() == true) {
		cmbQRSize->setEnabled(true);
	} else {
		cmbQRSize->setEnabled(false);
	}
}

void MainWindow::mqr_errorcorrect()
{
	if(radMQRECC->isChecked() == true) {
		cmbMQRECC->setEnabled(true);
	} else {
		cmbMQRECC->setEnabled(false);
	}
}

void MainWindow::mqr_size()
{
	if(radMQRSize->isChecked() == true) {
		cmbMQRSize->setEnabled(true);
	} else {
		cmbMQRSize->setEnabled(false);
	}
}

void MainWindow::qr_errorcorrect()
{
	if(radQRECC->isChecked() == true) {
		cmbQRECC->setEnabled(true);
	} else {
		cmbQRECC->setEnabled(false);
	}
}

void MainWindow::datamatrix_options()
{
	if(cmbDMMode->currentIndex() == 0) {
		grpDMNon200->hide();
		grpDM200->show();
	} else {
		grpDM200->hide();
		grpDMNon200->show();
	}
}

void MainWindow::maxi_primary()
{
	if(cmbMaxiMode->currentIndex() == 0) {
		lblMaxiPrimary->setEnabled(true);
		txtMaxiPrimary->setEnabled(true);
	} else {
		lblMaxiPrimary->setEnabled(false);
		txtMaxiPrimary->setEnabled(false);
	}
}

void MainWindow::update_preview()
{
	QString error;
	m_bc.ar=(Zint::QZint::AspectRatioMode)1;
	if(chkComposite->isChecked() == true) {
		m_bc.bc.setPrimaryMessage(txtData->text());
		m_bc.bc.setText(txtComposite->toPlainText());
	} else {
		m_bc.bc.setText(txtData->text());
		/*m_bc.bc.setPrimaryMessage(txtComposite->text());*/
	}
	m_bc.bc.setSecurityLevel(0);
	m_bc.bc.setWidth(0);
	m_bc.bc.setInputMode(UNICODE_MODE);
	switch(metaObject()->enumerator(0).value(bstyle->currentIndex()))
	{
		case BARCODE_CODE128:
			if(radC128Stand->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_CODE128);
			}
			if(radC128CSup->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_CODE128B);
			}
			if(radC128EAN->isChecked() == true) {
				if(chkComposite->isChecked() == true) {
					m_bc.bc.setSymbol(BARCODE_EAN128_CC);
				} else {
					m_bc.bc.setSymbol(BARCODE_EAN128);
				}
			}
			if(radC128HIBC->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_HIBC_128);
			}
			break;
		case BARCODE_EANX:
			if(chkComposite->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_EANX_CC);
			} else {
				m_bc.bc.setSymbol(BARCODE_EANX);
			}
			break;
		case BARCODE_UPCA:
			if(chkComposite->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_UPCA_CC);
			} else {
				m_bc.bc.setSymbol(BARCODE_UPCA);
			}
			break;
		case BARCODE_UPCE:
			if(chkComposite->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_UPCE_CC);
			} else {
				m_bc.bc.setSymbol(BARCODE_UPCE);
			}
			break;
		case BARCODE_RSS14:
			if(chkComposite->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_RSS14_CC);
			} else {
				m_bc.bc.setSymbol(BARCODE_RSS14);
			}
			break;
		case BARCODE_RSS_LTD:
			if(chkComposite->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_RSS_LTD_CC);
			} else {
				m_bc.bc.setSymbol(BARCODE_RSS_LTD);
			}
			break;
		case BARCODE_RSS_EXP:
			if(chkComposite->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_RSS_EXP_CC);
			} else {
				m_bc.bc.setSymbol(BARCODE_RSS_EXP);
			}
			break;
		case BARCODE_RSS14STACK:
			if(chkComposite->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_RSS14STACK_CC);
			} else {
				m_bc.bc.setSymbol(BARCODE_RSS14STACK);
			}
			break;
		case BARCODE_RSS14STACK_OMNI:
			if(chkComposite->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_RSS14_OMNI_CC);
			} else {
				m_bc.bc.setSymbol(BARCODE_RSS14STACK_OMNI);
			}
			break;
		case BARCODE_RSS_EXPSTACK:
			if(chkComposite->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_RSS_EXPSTACK_CC);
			} else {
				m_bc.bc.setSymbol(BARCODE_RSS_EXPSTACK);
			}
			break;
		case BARCODE_PDF417:
			m_bc.bc.setWidth(cmbPDFCols->currentIndex());
			m_bc.bc.setSecurityLevel(cmbPDFECC->currentIndex() - 1);
			m_bc.bc.setPdf417CodeWords(codewords->value());
			if(radPDFStand->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_PDF417);
			}
			if(radPDFTruncated->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_PDF417TRUNC);
			}
			if(radPDFHIBC->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_HIBC_PDF);
			}
			break;
		case BARCODE_MICROPDF417:
			m_bc.bc.setWidth(cmbMPDFCols->currentIndex());
			if(radMPDFStand->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_MICROPDF417);
			}
			if(radMPDFHIBC->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_HIBC_MICPDF);
			}
			break;
		case BARCODE_AZTEC:
			m_bc.bc.setSymbol(BARCODE_AZTEC);
			if(radAztecSize->isChecked() == true) {
				m_bc.bc.setWidth(cmbAztecSize->currentIndex() + 1);
			}
			if(radAztecECC->isChecked() == true) {
				m_bc.bc.setSecurityLevel(cmbAztecECC->currentIndex() + 1);
			}
			if(chkAztecMode->isChecked() == true) {
				m_bc.bc.setInputMode(GS1_MODE);
			}
			break;
		case MSI_PLESSEY:
			m_bc.bc.setSymbol(BARCODE_MSI_PLESSEY);
			m_bc.bc.setWidth(cmbMSICheck->currentIndex());
			break;
		case BARCODE_CODE39:
			if(radC39HIBC->isChecked() == false) {
				m_bc.bc.setSymbol(BARCODE_CODE39);
				if(radC39Check->isChecked() == true) {
					m_bc.bc.setWidth(1);
				}
			} else {
				m_bc.bc.setSymbol(BARCODE_HIBC_39);
			}
			break;
		case BARCODE_EXCODE39:
			m_bc.bc.setSymbol(BARCODE_EXCODE39);
			if(radC39Check->isChecked() == true) {
				m_bc.bc.setWidth(1);
			}
			break;
		case BARCODE_CODE16K:
			m_bc.bc.setSymbol(BARCODE_CODE16K);
			if(radC16kStand->isChecked() == true) {
				m_bc.bc.setInputMode(UNICODE_MODE);
			} else {
				m_bc.bc.setInputMode(GS1_MODE);
			}
			break;
		case BARCODE_CODABLOCKF:
			if(radCodaGS1->isChecked() == true) {
				m_bc.bc.setInputMode(GS1_MODE);
			}
			if(radCodaHIBC->isChecked() == false) {
				m_bc.bc.setSymbol(BARCODE_CODABLOCKF);
			} else {
				m_bc.bc.setSymbol(BARCODE_HIBC_BLOCKF);
			}
			break;
		case BARCODE_DATAMATRIX:
			m_bc.bc.setSecurityLevel(cmbDMMode->currentIndex() + 1);
			if(cmbDMMode->currentIndex() == 0) {
				/* ECC 200 */
				if(radDM200HIBC->isChecked() == true) {
					m_bc.bc.setSymbol(BARCODE_HIBC_DM);
				} else {
					m_bc.bc.setSymbol(BARCODE_DATAMATRIX);
				}
				if(radDM200GS1->isChecked() == true) {
					m_bc.bc.setInputMode(GS1_MODE);
				}
				m_bc.bc.setWidth(cmbDM200Size->currentIndex());
			} else {
				/* Not ECC 200 */
				m_bc.bc.setSymbol(BARCODE_DATAMATRIX);
				m_bc.bc.setWidth(cmbDMNon200Size->currentIndex());
			}
			break;
		case BARCODE_QRCODE:
			if(chkQRHIBC->isChecked() == true) {
				m_bc.bc.setSymbol(BARCODE_HIBC_QR);
			} else {
				m_bc.bc.setSymbol(BARCODE_QRCODE);
			}
			if(radQRSize->isChecked() == true) {
				m_bc.bc.setWidth(cmbQRSize->currentIndex() + 1);
			}
			if(radQRECC->isChecked() == true) {
				m_bc.bc.setSecurityLevel(cmbQRECC->currentIndex() + 1);
			}
			break;
		case BARCODE_MICROQR:
			m_bc.bc.setSymbol(BARCODE_MICROQR);
			if(radMQRSize->isChecked() == true) {
				m_bc.bc.setWidth(cmbMQRSize->currentIndex() + 1);
			}
			if(radMQRECC->isChecked() == true) {
				m_bc.bc.setSecurityLevel(cmbMQRECC->currentIndex() + 1);
			}
			break;
		case BARCODE_MAXICODE:
			m_bc.bc.setSymbol(BARCODE_MAXICODE);
			if(cmbMaxiMode->currentIndex() == 0) {
				m_bc.bc.setSecurityLevel(2);
				m_bc.bc.setPrimaryMessage(txtMaxiPrimary->text());
			} else {
				m_bc.bc.setSecurityLevel(cmbMaxiMode->currentIndex() + 3);
			}
			break;
		default:
			m_bc.bc.setSymbol(metaObject()->enumerator(0).value(bstyle->currentIndex()));
			break;
	}
	if(chkComposite->isChecked() == true) {
		m_bc.bc.setSecurityLevel(cmbCompType->currentIndex());
	}
	m_bc.bc.setBorderType((Zint::QZint::BorderType)(btype->currentIndex()*2));
	m_bc.bc.setBorderWidth(bwidth->value());
	m_bc.bc.setHeight(heightb->value());
	m_bc.bc.setFgColor(m_fgcolor);
	m_bc.bc.setBgColor(m_bgcolor);
	m_bc.update();
	view->scene()->update();
}


