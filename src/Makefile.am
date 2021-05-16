# src/Makefile.am


AM_CPPFLAGS = \
	$(DEPS_CFLAGS)


AM_CXXFLAGS = \
	-Wall -Wextra -Werror


bin_PROGRAMS = calibrate-joystick


calibrate_joystick_SOURCES = \
	main.cpp \
	utils.cpp utils.hpp


calibrate_joystick_LDADD = \
	$(DEPS_LIBS)


#calibrate_joystick_LDFLAGS = -rdynamic $(AM_LDFLAGS)
