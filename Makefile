# Make libzint and zint together

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

