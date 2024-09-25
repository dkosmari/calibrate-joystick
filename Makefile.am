# ./Makefile.am

ACLOCAL_AMFLAGS = -I m4


EXTRA_DIST = \
	@APPLICATION_ID@.desktop \
	@APPLICATION_ID@_daemon.desktop \
	@PACKAGE_TARNAME@.spec.in \
	@PACKAGE_TARNAME@.xml \
	bootstrap \
	po/README \
	README.md \
	ui


SUBDIRS = src po


# .desktop entry

desktopdir = $(datadir)/applications
desktop_DATA = \
	$(APPLICATION_ID).desktop \
	$(APPLICATION_ID)_daemon.desktop


# .gresource file

RESOURCES_XML = $(srcdir)/@PACKAGE_TARNAME@.xml
RESOURCES_ARCHIVE = @PACKAGE_TARNAME@.gresource

resourcesdir = $(datadir)/@PACKAGE_TARNAME@
resources_DATA = $(RESOURCES_ARCHIVE)

CLEANFILES = $(RESOURCES_ARCHIVE)

resources_DEPS = $(shell $(GLIB_COMPILE_RESOURCES) \
				--sourcedir=$(srcdir) \
				--generate-dependencies \
				$(RESOURCES_XML))

$(RESOURCES_ARCHIVE): $(RESOURCES_XML) $(resources_DEPS)
	$(GLIB_COMPILE_RESOURCES) $< \
		--target=$@ \
		--sourcedir=$(srcdir) \
		--generate



.PHONY: rpm

RPM_TARBALL_NAME = $(HOME)/rpmbuild/SOURCES/$(TARBALL_NAME)

rpm: @PACKAGE_TARNAME@.spec dist-gzip
	[ -e $(RPM_TARBALL_NAME) ] || \
		$(LN_S) $(PWD)/$(TARBALL_NAME) $(RPM_TARBALL_NAME)
	rpmbuild -ba $<
