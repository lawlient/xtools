INSTALL_BIN_PREFIX = /usr/local/bin
INSTALL_CONFIG_PREFIX = /usr/local/share/pandoc

CSS := $(wildcard ./css/*.css)


.PHONY: install
install: pre styles script

pre:
	./configure

# first, name is css, it's conflict with path name
styles: $(CSS)
	@echo $(CSS)
	mkdir -p $(INSTALL_CONFIG_PREFIX)/css
	install $(CSS) $(INSTALL_CONFIG_PREFIX)/css

script: md2html md2pdf
	mkdir -p $(INSTALL_BIN_PREFIX) && install md2html $(INSTALL_BIN_PREFIX)/md2html && install md2pdf $(INSTALL_BIN_PREFIX)/md2pdf

uninstall:
	rm $(INSTALL_BIN_PREFIX)/md2html
	rm $(INSTALL_BIN_PREFIX)/md2pdf
	rm $(INSTALL_CONFIG_PREFIX) -r

