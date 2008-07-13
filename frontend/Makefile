# Linux makefile for zint - requires libzint
#
# make			compiles zint
# make install		copies binary to /usr/local/bin
# make uninstall	removes the binary
# make clean		cleans up a previous compilation and any object or editor files
#

CC := gcc
INCLUDE := -I/usr/local/include

zint: main.c
	$(CC) $(INCLUDE) -g main.c -o zint -lzint

.PHONY: install uninstall clean dist

clean:
	rm -f zint *.o *.a *~ *.png *.eps

install:
	mv zint /usr/local/bin

uninstall:
	rm /usr/local/bin/zint
