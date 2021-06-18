# ./Makefile.am


ACLOCAL_AMFLAGS = -I m4


EXTRA_DIST = bootstrap \
	resources.xml \
	ui


SUBDIRS = src


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

