PREFIX?=/usr
TEMPLATES=sysinfo.en.tmpl sysinfo.pt.tmpl
INSTALL_DIR=$(PREFIX)/share/pollen/

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
