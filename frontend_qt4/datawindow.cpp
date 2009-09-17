#include <QDebug>
#include <QFileDialog>
#include <QUiLoader>
#include <QStringList>

#include "datawindow.h"
#include <stdio.h>

DataWindow::DataWindow()
{
	setupUi(this);

	connect(btnCancel, SIGNAL( clicked( bool )), SLOT(quit_now()));
	connect(btnReset, SIGNAL( clicked( bool )), SLOT(clear_data()));
	connect(btnOK, SIGNAL( clicked( bool )), SLOT(okay()));
}

DataWindow::DataWindow(QString input)
{
	setupUi(this);
	txtDataInput->setPlainText(input);
	txtDataInput->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	
	connect(btnCancel, SIGNAL( clicked( bool )), SLOT(quit_now()));
	connect(btnReset, SIGNAL( clicked( bool )), SLOT(clear_data()));
	connect(btnOK, SIGNAL( clicked( bool )), SLOT(okay()));
	connect(btnFromFile, SIGNAL( clicked( bool )), SLOT(from_file()));
}

DataWindow::~DataWindow()
{
}

void DataWindow::quit_now()
{
	Valid = 0;
	close();
}

void DataWindow::clear_data()
{
	txtDataInput->clear();
}

void DataWindow::okay()
{
	Valid = 1;
	DataOutput = txtDataInput->toPlainText();
	close();
}

void DataWindow::from_file()
{
	QString fileName;
	QFileDialog fdialog;
	QFile file;
	char *streamdata;
	int streamlen;
	QString utfstream;
	
	fdialog.setFileMode(QFileDialog::ExistingFile);
	
	if(fdialog.exec()) {
		fileName = fdialog.selectedFiles().at(0);
	} else {
		return;
	}
	
	file.setFileName(fileName);
	if(!file.open(QIODevice::ReadOnly)) {
		return;
	}
	
	QDataStream input(&file);
	streamlen = input.readRawData(streamdata, 7095);
	utfstream = streamdata; /* FIXME: Does not take account of encoding scheme of input data */
	txtDataInput->setPlainText(utfstream);
	file.close();
}