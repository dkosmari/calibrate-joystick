# libgudevxx/utils/Makefile.am


AM_CPPFLAGS = \
	-I../src \
	$(GLIBMM_GUDEV_CFLAGS)


noinst_PROGRAMS = \
	test-listener


test_listener_SOURCES = \
	test-listener.cpp

test_listener_LDADD = \
	../src/libgudevxx.la \
	$(GLIBMM_GUDEV_LIBS)



.PHONY: company

company: Makefile
	echo "$(CPPFLAGS)" | xargs -n1 echo | sort -u > compile_flags.txt
