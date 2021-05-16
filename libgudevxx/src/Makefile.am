# src/Makefile.am

lib_LTLIBRARIES = libgudevxx.la

gudevxxdir = $(includedir)/gudevxx

gudevxx_HEADERS = \
	client.hpp \
	device.hpp \
	gobject_base.hpp \
	utils.hpp


AM_CPPFLAGS = \
	$(GUDEV_CFLAGS)


libgudevxx_la_SOURCES = \
	client.cpp client.hpp \
	device.cpp device.hpp \
	gobject_base.hpp \
	utils.hpp


libgudevxx_la_LIBADD = \
	$(GUDEV_LIBS)




.PHONY: company

company: Makefile
	echo "$(GUDEV_CFLAGS)" | xargs -n1 echo | sort -u > compile_flags.txt
