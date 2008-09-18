# Make libzint and zint together
#
# make for QR Code support
# make zint_noqr for Zint without QR Code support

zint:
	$(MAKE) -C backend/
	$(MAKE) -C frontend/

zint_noqr:
	$(MAKE) NO_QR=true -C backend/
	$(MAKE) -C frontend/

install:
	$(MAKE) install -C backend/
	$(MAKE) install -C frontend/

clean:
	$(MAKE) clean -C backend/
	$(MAKE) clean -C frontend/

