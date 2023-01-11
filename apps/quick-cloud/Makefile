PREFIX?=/usr
TEMPLATES=index.en.tmpl index.pt.tmpl
INSTALL_DIR=$(PREFIX)/share/quick-cloud/

all:

install:
	# This installs the apps on the system share
	# directory.
	# The binary plugins are installed on the lib
	# directory by the makefile on the plugin
	# directory of this source tree
	mkdir -p $(INSTALL_DIR)
	install $(TEMPLATES) $(INSTALL_DIR)

clean:

.PHONY: install clean
