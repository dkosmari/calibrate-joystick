# libgudevxx/utils/Makefile.am

AM_CXXFLAGS = -Wall -Wextra

AM_CPPFLAGS = \
	-I../src \
	$(GLIBMM_GUDEV_CFLAGS)

LDADD = \
	../src/libgudevxx.la \
	$(GLIBMM_GUDEV_LIBS)


noinst_PROGRAMS = \
	test-device-info \
	test-enumerator \
	test-listener


test_listener_SOURCES = \
	test-listener.cpp


test_device_info_SOURCES = \
	test-device-info.cpp


test_enumerator_SOURCES = \
	test-enumerator.cpp


.PHONY: company

company: Makefile
	echo "$(AM_CPPFLAGS)" | xargs -n1 echo | sort -u > compile_flags.txt
