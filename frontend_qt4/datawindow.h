#ifndef DATAWINDOW_H
#define DATAWINDOW_H

#include "ui_extData.h"

class DataWindow : public QDialog, private Ui::DataDialog
{
	Q_OBJECT

public:
	DataWindow();
	DataWindow(QString input);
	~DataWindow();
	int Valid;
	QString DataOutput;

private slots:
	void quit_now();
	void clear_data();
	void okay();
	void from_file();
};

#endif
