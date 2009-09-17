#include <QDebug>
#include <QFile>
#include <QUiLoader>

#include "sequencewindow.h"
#include <stdio.h>

SequenceWindow::SequenceWindow()
{
	setupUi(this);

	connect(btnClose, SIGNAL( clicked( bool )), SLOT(quit_now()));
	connect(btnReset, SIGNAL( clicked( bool )), SLOT(reset_preview()));
}

SequenceWindow::~SequenceWindow()
{
}

void SequenceWindow::quit_now()
{
	close();
}

void SequenceWindow::reset_preview()
{
	txtPreview->clear();
}
