# ./Makefile.am

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	@APPLICATION_ID@.desktop \
	@APPLICATION_ID@Daemon.desktop \
	@PACKAGE@.gresource.xml.in \
	@PACKAGE@.gschema.xml.in \
	bootstrap \
	po/README.md \
	README.md \
	ui


SUBDIRS = @EXTERNAL_SUBDIRS@ po


# .desktop file

desktopdir = $(datadir)/applications
desktop_DATA = \
	$(APPLICATION_ID).desktop \
	$(APPLICATION_ID)Daemon.desktop


# .gresource file

gresourcedir = $(datadir)/@PACKAGE@
gresource_DATA = @PACKAGE@.gresource
gresource_DEPS = $(shell $(GLIB_COMPILE_RESOURCES) \
				--sourcedir=$(srcdir) \
				--generate-dependencies \
				$(gresource_DATA).xml)

$(gresource_DATA): $(gresource_DATA).xml $(gresources_DEPS)
	$(GLIB_COMPILE_RESOURCES) $< \
		--target=$@ \
		--sourcedir=$(srcdir) \
		--generate


# .gschema file
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


.PHONY: run run-daemon company


install-exec-hook:
install-data-hook:
if INTERNAL_LIBEVDEVXX
	$(info Removing libevdevxx)
	$(MAKE) -C external/libevdevxx uninstall DESTDIR=$(DESTDIR)
endif INTERNAL_LIBEVDEVXX
if INTERNAL_LIBGUDEVXX
	$(info Removing libgudevxx)
	$(MAKE) -C external/libgudevxx uninstall DESTDIR=$(DESTDIR)
endif INTERNAL_LIBGUDEVXX


# Note: to run without installing, the schema needs to be compiled.
gschemas.compiled: $(gsettings_SCHEMAS)
	$(GLIB_COMPILE_SCHEMAS) --targetdir=$(builddir) $(srcdir)

run: all gschemas.compiled
	GSETTINGS_SCHEMA_DIR=. ./calibrate-joystick

run-daemon: all gschemas.compiled
	GSETTINGS_SCHEMA_DIR=. ./calibrate-joystick -d


company: compile_flags.txt

compile_flags.txt: Makefile
	printf "%s" "$(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS)" | xargs -n1 | sort -u > compile_flags.txt
	$(CPP) -xc++ /dev/null -E -Wp,-v 2>&1 | sed -n 's,^ ,-I,p' >> compile_flags.txt


CLEANFILES = $(gresource_DATA)

MOSTLYCLEANFILES = gschemas.compiled
