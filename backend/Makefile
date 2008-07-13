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
COMMON:= common.c png.c library.c ps.c large.c reedsol.c
COMMON_OBJ:= common.o png.o library.o ps.o large.o reedsol.o
ONEDIM:= code.c code128.c 2of5.c upcean.c telepen.c medical.c plessey.c rss.c
ONEDIM_OBJ:= code.o code128.o 2of5.o upcean.o telepen.o medical.o plessey.o rss.o
POSTAL:= postal.c auspost.c imail.c
POSTAL_OBJ:= postal.o auspost.o imail.o
TWODIM:= code16k.c dmatrix.c dm200.c pdf417.c maxicode.c composite.c
TWODIM_OBJ:= code16k.o dmatrix.o dm200.o pdf417.o maxicode.o composite.o
LIBS:= `libpng12-config --I_opts --L_opts --ldflags` -lz -lm

libzint: code.c code128.c 2of5.c upcean.c medical.c telepen.c plessey.c postal.c auspost.c imail.c code16k.c dmatrix.c dm200.c reedsol.c pdf417.c maxicode.c rss.c common.c png.c library.c ps.c qr.c large.c composite.c
	$(CC) -fPIC -g -c $(ONEDIM)
	$(CC) -fPIC -g -c $(POSTAL)
	$(CC) -fPIC -g -c $(TWODIM) qr.c
	$(CC) -fPIC -g -c $(COMMON)
	$(CC) -g -shared -Wl,-soname,libzint.so -o libzint.so.1.6.0 $(INCLUDE) $(COMMON_OBJ) $(ONEDIM_OBJ) $(TWODIM_OBJ) qr.o $(POSTAL_OBJ) $(LIBS) -lqrencode

libzint_noqr: code.c code128.c 2of5.c upcean.c medical.c telepen.c plessey.c postal.c auspost.c imail.c code16k.c dmatrix.c dm200.c reedsol.c pdf417.c maxicode.c rss.c common.c png.c library.c ps.c no_qr.c large.c composite.c
	$(CC) -fPIC -g -c $(ONEDIM)
	$(CC) -fPIC -g -c $(POSTAL)
	$(CC) -fPIC -g -c $(TWODIM) no_qr.c
	$(CC) -fPIC -g -c $(COMMON)
	$(CC) -g -shared -Wl,-soname,libzint.so -o libzint.so.1.6.0 $(INCLUDE) $(COMMON_OBJ) $(ONEDIM_OBJ) $(TWODIM_OBJ) no_qr.o $(POSTAL_OBJ) $(LIBS)


.PHONY: install uninstall clean dist

install:
	ldconfig -n $(PWD)
	mv libzint.* /usr/local/lib
	cp zint.h /usr/local/include

uninstall:
	rm /usr/local/lib/libzint.*
	rm /usr/local/include/zint.h

clean:
	rm -f libzint.* *.o *.a *~


