# Linux makefile for libzint
#
# make			compiles with QR Code support
# make libzint_noqr	compiles without QR Code support
# make install		copies to /usr/local/lib
# make uninstall	removes library
# make clean		cleans up a previous compilation and any object or editor files
#

CC := gcc
INCLUDE := -I/usr/local/include
CFLAGS := -g

prefix := /usr/local
includedir := $(prefix)/include
libdir := $(prefix)/lib
DESTDIR :=

COMMON:= common.c png.c library.c ps.c large.c reedsol.c
COMMON_OBJ:= common.o png.o library.o ps.o large.o reedsol.o
ONEDIM:= code.c code128.c 2of5.c upcean.c telepen.c medical.c plessey.c rss.c
ONEDIM_OBJ:= code.o code128.o 2of5.o upcean.o telepen.o medical.o plessey.o rss.o
POSTAL:= postal.c auspost.c imail.c
POSTAL_OBJ:= postal.o auspost.o imail.o
TWODIM:= code16k.c dmatrix.c dm200.c pdf417.c qr.c maxicode.c composite.c aztec.c
TWODIM_OBJ:= code16k.o dmatrix.o dm200.o pdf417.o qr.o maxicode.o composite.o aztec.o
LIBS:= `libpng12-config --I_opts --L_opts --ldflags` -lz -lm

ifeq ($(WITHOUT_QR),true)
DEFINES:=
else
DEFINES:= -DWITH_QR
LIBS+= -lqrencode
endif
 
libzint: code.c code128.c 2of5.c upcean.c medical.c telepen.c plessey.c postal.c auspost.c imail.c code16k.c dmatrix.c dm200.c reedsol.c pdf417.c maxicode.c rss.c common.c png.c library.c ps.c qr.c large.c composite.c aztec.c
	$(CC) -fPIC $(CFLAGS) -c $(ONEDIM)
	$(CC) -fPIC $(CFLAGS) -c $(POSTAL)
	$(CC) -fPIC $(DEFINES) $(CFLAGS) -c $(TWODIM)
	$(CC) -fPIC $(CFLAGS) -c $(COMMON)
	$(CC) $(CFLAGS) -shared -Wl,-soname,libzint.so -o libzint.so.1.6.3 $(INCLUDE) $(COMMON_OBJ) $(ONEDIM_OBJ) $(TWODIM_OBJ) $(POSTAL_OBJ) $(LIBS)
	ln -s libzint.so.* libzint.so

.PHONY: install uninstall clean dist

install:
	test "$(UID)" = "0" && ldconfig -n $(PWD) || true
	install -d $(DESTDIR)$(libdir)
	mv libzint.* $(DESTDIR)$(libdir)
	install -D -p --mode=0644 zint.h $(DESTDIR)$(includedir)/zint.h

uninstall:
	rm $(DESTDIR)$(libdir)libzint.*
	rm $(DESTDIR)$(includedir)/zint.h

clean:
	rm -f libzint.* *.o *.a *~


