# src/Makefile.am


AM_CPPFLAGS = \
	$(GTKMM_CFLAGS) \
	$(LIBGUDEVXX_CFLAGS) \
	$(LIBEVDEVXX_CFLAGS) \
	-DRESOURCE_DIR=\"$(datadir)/calibrate-joystick\" \
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


CLEANFILES = resources.gresource

all-local: resources.gresource


resources.gresource:
	$(LN_S) -f $(abs_top_builddir)/resources.gresource $(builddir)/resources.gresource


.PHONY: company

company: compile_flags.txt

compile_flags.txt: Makefile
	echo "$(AM_CPPFLAGS) $(CPPFLAGS) $(DEFS)" \
		| xargs -n1 echo | sort -u \
		> $(srcdir)/compile_flags.txt
