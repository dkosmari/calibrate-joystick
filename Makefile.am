# ./Makefile.am

ACLOCAL_AMFLAGS = -I m4


EXTRA_DIST = \
	bootstrap \
	calibrate-joystick.desktop \
	calibrate-joystick.spec.in \
	resources.xml \
	ui


SUBDIRS = src po


# .desktop entry

desktopdir = $(datadir)/applications
desktop_DATA = calibrate-joystick.desktop


# .gresource file

resourcesdir = $(datadir)/calibrate-joystick
resources_DATA = resources.gresource

CLEANFILES = resources.gresource

resources_DEPS = $(shell $(GLIB_COMPILE_RESOURCES) \
				--sourcedir=$(srcdir) \
				--generate-dependencies \
				resources.xml)

resources.gresource: resources.xml $(resources_DEPS)
	$(GLIB_COMPILE_RESOURCES) $< \
		--target=$@ \
		--sourcedir=$(srcdir) \
		--generate


# RPM packaging

.PHONY: rpm

TARBALL_NAME = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz
RPM_TARBALL_NAME = $(HOME)/rpmbuild/SOURCES/$(TARBALL_NAME)

rpm: calibrate-joystick.spec dist-gzip
	[ -e $(RPM_TARBALL_NAME) ] || \
		$(LN_S) $(PWD)/$(TARBALL_NAME) $(RPM_TARBALL_NAME)
	rpmbuild -ba $<
