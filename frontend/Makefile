# Linux makefile for zint - requires libzint
#
# make			compiles zint
# make install		copies binary to /usr/local/bin
# make uninstall	removes the binary
# make clean		cleans up a previous compilation and any object or editor files
#

ZINT_VERSION:=-DZINT_VERSION=\"1.99.1\"

CC := gcc
INCLUDE := -I/usr/local/include
CFLAGS := -g

prefix := /usr/local
bindir := $(prefix)/bin
DESTDIR :=

zint: main.c
	$(CC) $(INCLUDE) $(CFLAGS) $(ZINT_VERSION) -I../backend -L../backend main.c -o zint -lzint

.PHONY: install uninstall clean dist

clean:
	rm -f zint *.o *.a *~ *.png *.eps

install:
	install -D -p  zint $(DESTDIR)$(bindir)/zint

uninstall:
	rm $(DESTDIR)$(bindir)/zint
