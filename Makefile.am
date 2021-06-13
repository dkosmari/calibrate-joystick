# ./Makefile.am


ACLOCAL_AMFLAGS = -I m4


EXTRA_DIST = bootstrap \
	resources.xml


SUBDIRS = src


resoucedir = $(datadir)/calibrate-joystick
resouce_DATA = resources.gresource

CLEAN_FILES = resources.gresource


.xml.gresource:
	$(GLIB_COMPILE_RESOURCES) $< --target=$@
