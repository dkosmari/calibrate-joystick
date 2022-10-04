# src/Makefile.am


RESOURCES_ARCHIVE = @PACKAGE_NAME@.gresource


AM_CPPFLAGS = \
	$(GTKMM_CFLAGS) \
	$(LIBGUDEVXX_CFLAGS) \
	$(LIBEVDEVXX_CFLAGS) \
	-DRESOURCES_DIR=\"$(datadir)/$(PACKAGE_NAME)\" \
	-DLOCALEDIR=\"$(localedir)\"


AM_CXXFLAGS = \
	-Wall -Wextra -Werror


LDADD = \
	$(GTKMM_LIBS) \
	$(LIBGUDEVXX_LIBS) \
	$(LIBEVDEVXX_LIBS) \
	$(LIBINTL) \
	$(LTLIBICONV)

bin_PROGRAMS = calibrate-joystick


calibrate_joystick_SOURCES = \
	app.cpp app.hpp \
	axis_canvas.cpp axis_canvas.hpp \
	axis_info.cpp axis_info.hpp \
	device_page.cpp device_page.hpp \
	main.cpp \
	utils.cpp utils.hpp


CLEANFILES = $(RESOURCES_ARCHIVE)

all-local: $(RESOURCES_ARCHIVE)


$(RESOURCES_ARCHIVE):
	$(LN_S) -f $(abs_top_builddir)/$(RESOURCES_ARCHIVE) $(builddir)/$(RESOURCES_ARCHIVE)


.PHONY: company

company: compile_flags.txt

compile_flags.txt: Makefile
	echo "$(AM_CPPFLAGS) $(CPPFLAGS) $(DEFS)" \
		| xargs -n1 echo | sort -u \
		> $(srcdir)/compile_flags.txt
