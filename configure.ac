#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.70])
AC_INIT([Calibrate Joystick],
        [0.5.2+],
        [https://github.com/dkosmari/calibrate-joystick/issues],
        [calibrate-joystick],
        [https://github.com/dkosmari/calibrate-joystick])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign])
LT_INIT

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_CHECK_PROGS([GLIB_COMPILE_RESOURCES], [glib-compile-resources])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.21])
AM_ICONV


AC_LANG([C++])

AX_APPEND_COMPILE_FLAGS([-std=c++20], [CXX])

# Checks for libraries.
PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0])
PKG_CHECK_MODULES([LIBEVDEVXX], [libevdevxx])
PKG_CHECK_MODULES([LIBGUDEVXX], [libgudevxx])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.

AC_SUBST([APPLICATION_ID], [none.calibrate_joystick])
AC_DEFINE([APPLICATION_ID], ["none.calibrate_joystick"], [GApplication id, must match .desktop file name])

TARBALL_NAME="${PACKAGE_TARNAME}-${PACKAGE_VERSION}.tar.gz"
AC_SUBST([TARBALL_NAME])

AC_CONFIG_FILES([calibrate-joystick.spec
                 Makefile
                 src/Makefile
                 po/Makefile.in])

AC_OUTPUT
