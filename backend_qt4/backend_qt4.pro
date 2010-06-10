TEMPLATE = lib

CONFIG += dll

macx{
	CONFIG -= dll
	CONFIG += lib_bundle
}

TARGET = QtZint

INCLUDEPATH += ../backend

#EDIT THIS !!!!
DEFINES += NO_PNG NO_QR ZINT_VERSION="2.3.0"

!contains(DEFINES, NO_PNG) {
    SOURCES += ../backend/png.c
    LIBS += -lpng
}

contains(DEFINES, QR_SYSTEM){
    LIBS += -lqrencode
}

contains(DEFINES, QR){

INCLUDEPATH += qrencode

HEADERS += qrencode/bitstream.h \
           qrencode/mask.h \
           qrencode/qrencode.h \
           qrencode/qrencode_inner.h \
           qrencode/qrinput.h \
           qrencode/qrspec.h \
           qrencode/rscode.h \
           qrencode/split.h 

SOURCES += qrencode/bitstream.c \
           qrencode/mask.c \
           qrencode/qrencode.c \
           qrencode/qrinput.c \
           qrencode/qrspec.c \
           qrencode/rscode.c \
           qrencode/split.c 
}

HEADERS +=  ../backend/aztec.h \
            ../backend/code49.h \
            ../backend/common.h \
            ../backend/composite.h \
            ../backend/dm200.h \
            ../backend/dmatrix.h \
            ../backend/font.h \
            ../backend/gs1.h \
            ../backend/large.h \
            ../backend/maxicode.h \
            ../backend/maxipng.h \
            ../backend/micqr.h \
            ../backend/pdf417.h \
            ../backend/reedsol.h \
            ../backend/rss.h \
            ../backend/sjis.h \
            ../backend/zint.h \
            qzint.h

SOURCES += ../backend/2of5.c \
           ../backend/auspost.c \
           ../backend/aztec.c \
           ../backend/blockf.c \
           ../backend/code.c \
           ../backend/code128.c \
           ../backend/code16k.c \
           ../backend/code49.c \
           ../backend/common.c \
           ../backend/composite.c \
           ../backend/dm200.c \
           ../backend/dmatrix.c \
           ../backend/gs1.c \
           ../backend/imail.c \
           ../backend/large.c \
           ../backend/library.c \
           ../backend/maxicode.c \
           ../backend/medical.c \
           ../backend/micqr.c \
           ../backend/pdf417.c \
           ../backend/plessey.c \
           ../backend/postal.c \
           ../backend/ps.c \
           ../backend/reedsol.c \
           ../backend/rss.c \
           ../backend/svg.c \
           ../backend/telepen.c \
           ../backend/upcean.c \
           ../backend/qr.c \
           ../backend/dllversion.c \
           ../backend/code1.c \
           qzint.cpp

VERSION = 2.3.0

#DESTDIR = .

include.path = $$[ZINT_INSTALL_HEADERS]
include.files = ../backend/zint.h qzint.h

target.path = $$[ZINT_INSTALL_LIBS]

INSTALLS += target include

