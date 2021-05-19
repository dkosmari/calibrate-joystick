# src/Makefile.am


AM_CPPFLAGS = \
	$(GUDEV_CFLAGS)

LIBADD = $(GUDEV_LIBS)


lib_LTLIBRARIES = libgudevxx.la

gudevxxdir = $(includedir)/gudevxx

gudevxx_HEADERS = \
	client.hpp \
	device.hpp \
	gobject_base.hpp \
	utils.hpp


libgudevxx_la_SOURCES = \
	client.cpp client.hpp \
	device.cpp device.hpp \
	enumerator.cpp enumerator.hpp \
	gobject_base.hpp \
	utils.hpp




.PHONY: company

company: Makefile
	echo "$(AM_CPPFLAGS)" | xargs -n1 echo | sort -u > compile_flags.txt
