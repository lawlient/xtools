INSTALL_BIN_PREFIX = /usr/local/bin
INSTALL_CONFIG_PREFIX = /usr/local/share/pandoc

CSS = $(wildcard css/)

install:
	mkdir -p $(INSTALL_BIN_PREFIX) && install md2html $(INSTALL_BIN_PREFIX)/md2html && install md2pdf $(INSTALL_BIN_PREFIX)/md2pdf
	mkdir -p $(INSTALL_CONFIG_PREFIX)/css && install $(CSS)/* $(INSTALL_CONFIG_PREFIX)/css

uninstall:
	rm $(INSTALL_BIN_PREFIX)/md2html
	rm $(INSTALL_CONFIG_PREFIX) -r

