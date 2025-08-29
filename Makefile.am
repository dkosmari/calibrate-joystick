# ./Makefile.am

ACLOCAL_AMFLAGS = -I m4


EXTRA_DIST = \
	@APPLICATION_ID@.desktop \
	@APPLICATION_ID@_daemon.desktop \
	@PACKAGE@.spec.in \
	@PACKAGE@.xml \
	bootstrap \
	po/README \
	README.md \
	ui


SUBDIRS = $(EXTERNAL_SUBDIRS) src po


# .desktop entry

desktopdir = $(datadir)/applications
desktop_DATA = \
	$(APPLICATION_ID).desktop \
	$(APPLICATION_ID)_daemon.desktop


# .gresource file

RESOURCES_XML = $(srcdir)/@PACKAGE@.xml
RESOURCES_ARCHIVE = @PACKAGE@.gresource

resourcesdir = $(datadir)/@PACKAGE@
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

rpm: @PACKAGE@.spec dist-gzip
	[ -e $(RPM_TARBALL_NAME) ] || \
		$(LN_S) $(PWD)/$(TARBALL_NAME) $(RPM_TARBALL_NAME)
	rpmbuild -ba $<
