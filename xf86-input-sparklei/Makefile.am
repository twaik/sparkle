# Provide an sdk location that is writable by the evdev module
DISTCHECK_CONFIGURE_FLAGS = --with-sdkdir='$${includedir}/xorg'

SUBDIRS = src
MAINTAINERCLEANFILES = ChangeLog INSTALL

.PHONY: ChangeLog INSTALL

INSTALL:
	$(INSTALL_CMD)

ChangeLog:
	$(CHANGELOG_CMD)

dist-hook: ChangeLog INSTALL
