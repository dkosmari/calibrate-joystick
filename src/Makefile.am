# src/Makefile.am


AM_CPPFLAGS = \
	$(GTKMM_CFLAGS) \
	$(GUDEV_CFLAGS) \
	-I../libgudevxx/src


AM_CXXFLAGS = \
	-Wall -Wextra -Werror


LDADD = \
	$(top_builddir)/libgudevxx/src/libgudevxx.la \
	$(GTKMM_LIBS) \
	$(GUDEV_LIBS)


bin_PROGRAMS = calibrate-joystick


calibrate_joystick_SOURCES = \
	app.cpp app.hpp \
	device_page.cpp device_page.hpp \
	joystick_listener.cpp joystick_listener.hpp \
	main.cpp \
	refresh_button.cpp refresh_button.hpp \
	utils.cpp utils.hpp


#calibrate_joystick_LDFLAGS = -rdynamic $(AM_LDFLAGS)


.PHONY: company

company: Makefile
	echo "$(AM_CPPFLAGS)" | xargs -n1 echo | sort -u > compile_flags.txt
