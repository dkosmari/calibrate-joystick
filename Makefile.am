# ./Makefile.am

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	@APPLICATION_ID@.desktop \
	@APPLICATION_ID@_daemon.desktop \
	@PACKAGE@.spec.in \
	@PACKAGE@.xml.in \
	@PACKAGE@.gschema.xml.in \
	bootstrap \
	po/README.md \
	README.md \
	ui


SUBDIRS = @EXTERNAL_SUBDIRS@ po


# .desktop entry

desktopdir = $(datadir)/applications
desktop_DATA = \
	$(APPLICATION_ID).desktop \
	$(APPLICATION_ID)_daemon.desktop


# .gresource file

RESOURCES_XML = @PACKAGE@.xml
RESOURCES_ARCHIVE = @PACKAGE@.gresource

resourcesdir = $(datadir)/@PACKAGE@
resources_DATA = $(RESOURCES_ARCHIVE)

resources_DEPS = $(shell $(GLIB_COMPILE_RESOURCES) \
				--sourcedir=$(srcdir) \
				--generate-dependencies \
				$(RESOURCES_XML))

$(RESOURCES_ARCHIVE): $(RESOURCES_XML) $(resources_DEPS)
	$(GLIB_COMPILE_RESOURCES) $< \
		--target=$@ \
		--sourcedir=$(srcdir) \
		--generate


gsettings_SCHEMAS = @PACKAGE@.gschema.xml
@GSETTINGS_RULES@


if INTERNAL_LIBEVDEVXX
LIBEVDEVXX_CFLAGS += $(LIBEVDEV_CFLAGS) -I$(srcdir)/external/libevdevxx/include
LIBEVDEVXX_LIBS += -Lexternal/libevdevxx -levdevxx $(LIBEVDEV_LIBS)
endif INTERNAL_LIBEVDEVXX

if INTERNAL_LIBGUDEVXX
LIBGUDEVXX_CFLAGS += $(LIBGUDEV_CFLAGS) -I$(srcdir)/external/libgudevxx/include
LIBGUDEVXX_LIBS += -Lexternal/libgudevxx -lgudevxx $(LIBGUDEV_LIBS)
endif INTERNAL_LIBGUDEVXX


AM_CPPFLAGS = \
	$(GTKMM_CFLAGS) \
	$(LIBGUDEVXX_CFLAGS) \
	$(LIBEVDEVXX_CFLAGS) \
	-DRESOURCES_DIR=\"$(datadir)/$(PACKAGE)\" \
	-DLOCALEDIR=\"$(localedir)\"

AM_CXXFLAGS = \
	-Wall -Wextra -Werror

LIBS = \
	$(GTKMM_LIBS) \
	$(LIBGUDEVXX_LIBS) \
	$(LIBEVDEVXX_LIBS) \
	$(LIBINTL) \
	$(LTLIBICONV)


bin_PROGRAMS = calibrate-joystick


calibrate_joystick_SOURCES = \
	src/app.cpp \
	src/app.hpp \
	src/axis_canvas.cpp \
	src/axis_canvas.hpp \
	src/axis_info.cpp \
	src/axis_info.hpp \
	src/colors.hpp \
	src/controller_db.cpp \
	src/controller_db.hpp \
	src/device_page.cpp \
	src/device_page.hpp \
	src/main.cpp \
	src/settings.cpp \
	src/settings.hpp \
	src/utils.hpp


.PHONY: run run-daemon rpm company


# todo: find the name of the compiled schema file, write a rule
gschemas.compiled: $(gsettings_SCHEMAS)
	$(GLIB_COMPILE_SCHEMAS) --targetdir=$(builddir) $(srcdir)


run: all gschemas.compiled
	GSETTINGS_SCHEMA_DIR=. ./calibrate-joystick

run-daemon: all gschemas.compiled
	GSETTINGS_SCHEMA_DIR=. ./calibrate-joystick -d


RPM_TARBALL_NAME = $(HOME)/rpmbuild/SOURCES/$(TARBALL_NAME)

rpm: @PACKAGE@.spec dist-gzip
	[ -e $(RPM_TARBALL_NAME) ] || \
		$(LN_S) $(PWD)/$(TARBALL_NAME) $(RPM_TARBALL_NAME)
	rpmbuild -ba $<


company: compile_flags.txt

compile_flags.txt: Makefile
	printf "%s" "$(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS)" | xargs -n1 | sort -u > compile_flags.txt
	$(CPP) -xc++ /dev/null -E -Wp,-v 2>&1 | sed -n 's,^ ,-I,p' >> compile_flags.txt


CLEANFILES = $(RESOURCES_ARCHIVE) 
MOSTLYCLEANFILES = gschemas.compiled
