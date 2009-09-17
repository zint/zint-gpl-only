#ifndef SEQUENCEWINDOW_H
#define SEQUENCEWINDOW_H

#include "ui_extSequence.h"

class SequenceWindow : public QDialog, private Ui::SequenceDialog
{
	Q_OBJECT

public:
	SequenceWindow();
	~SequenceWindow();

private slots:
	void quit_now();
	void reset_preview();
};

#endif