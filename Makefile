# Make libzint and zint together
#
# make for QR Code support
# make zint_noqr for Zint without QR Code support

zint:
	$(MAKE) -C backend/
	$(MAKE) -C frontend/

install:
	$(MAKE) install -C backend/
	$(MAKE) install -C frontend/

uninstall:
	$(MAKE) uninstall -C frontend/
	$(MAKE) uninstall -C backend/

clean:
	$(MAKE) clean -C backend/
	$(MAKE) clean -C frontend/

