# ./Makefile.am

ACLOCAL_AMFLAGS = -I m4


EXTRA_DIST = \
	bootstrap \
	calibrate-joystick.spec.in \
	none.calibrate_joystick.desktop \
	none.calibrate_joystick-daemon.desktop \
	po/README \
	resources.xml \
	ui


SUBDIRS = src po


# .desktop entry

desktopdir = $(datadir)/applications
desktop_DATA = \
	none.calibrate_joystick.desktop \
	none.calibrate_joystick-daemon.desktop


# .gresource file

resourcesdir = $(datadir)/calibrate-joystick
resources_DATA = resources.gresource

CLEANFILES = resources.gresource

resources_DEPS = $(shell $(GLIB_COMPILE_RESOURCES) \
				--sourcedir=$(srcdir) \
				--generate-dependencies \
				$(srcdir)/resources.xml)

resources.gresource: $(srcdir)/resources.xml $(resources_DEPS)
	$(GLIB_COMPILE_RESOURCES) $< \
		--target=$@ \
		--sourcedir=$(srcdir) \
		--generate



.PHONY: rpm

TARBALL_NAME = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz
RPM_TARBALL_NAME = $(HOME)/rpmbuild/SOURCES/$(TARBALL_NAME)

rpm: calibrate-joystick.spec dist-gzip
	[ -e $(RPM_TARBALL_NAME) ] || \
		$(LN_S) $(PWD)/$(TARBALL_NAME) $(RPM_TARBALL_NAME)
	rpmbuild -ba $<
