TEMPLATE = app

CONFIG += warn_on \
	  thread \
          qt

TARGET = qtFontend

FORMS += mainWindow.ui

SOURCES += main.cpp \
 mainwindow.cpp \
 barcodeitem.cpp

HEADERS += mainwindow.h \
 barcodeitem.h

LIBS += -lQtZint
