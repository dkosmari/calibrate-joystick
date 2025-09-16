#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.70])
AC_INIT([Calibrate Joystick],
        [0.6.0-rc1],
        [https://github.com/dkosmari/calibrate-joystick/issues],
        [calibrate-joystick],
        [https://github.com/dkosmari/calibrate-joystick])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign subdir-objects])
LT_INIT

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


AC_TYPE_SIZE_T
AC_TYPE_UINT64_T


GLIB_GSETTINGS


PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0])

USE_SYSTEM_LIBEVDEVXX=yes
AC_ARG_ENABLE([system-libevdevxx],
              [AS_HELP_STRING([--disable-system-libevdevxx], [ignore libevdevxx from the system.])],
              [USE_SYSTEM_LIBEVDEVXX=$enableval])
AS_VAR_IF([USE_SYSTEM_LIBEVDEVXX], [yes],
          [PKG_CHECK_MODULES([LIBEVDEVXX], [libevdevxx], [], [USE_SYSTEM_LIBEVDEVXX=no])])


USE_SYSTEM_LIBGUDEVXX=yes
AC_ARG_ENABLE([system-libgudevxx],
              [AS_HELP_STRING([--disable-system-libgudevxx], [ignore libgudevxx from the system.])],
              [USE_SYSTEM_LIBGUDEVXX=$enableval])
AS_VAR_IF([USE_SYSTEM_LIBGUDEVXX], [yes],
          [PKG_CHECK_MODULES([LIBGUDEVXX], [libgudevxx], [], [USE_SYSTEM_LIBGUDEVXX=no])])


APPLICATION_ID="none.calibrate_joystick"
AC_SUBST([APPLICATION_ID])
AC_DEFINE_UNQUOTED([APPLICATION_ID],
                   ["$APPLICATION_ID"],
                   [GApplication id, must match .desktop file name])

RESOURCE_PREFIX="/none/calibrate_joystick"
AC_SUBST([RESOURCE_PREFIX])
AC_DEFINE_UNQUOTED([RESOURCE_PREFIX],
                   ["$RESOURCE_PREFIX"],
                   [Prefix used in the .gresource file.])


TARBALL_NAME="${PACKAGE_TARNAME}-${PACKAGE_VERSION}.tar.gz"
AC_SUBST([TARBALL_NAME])


EXTERNAL_SUBDIRS=

AS_VAR_IF([USE_SYSTEM_LIBEVDEVXX], [no],
          [
              PKG_CHECK_MODULES([LIBEVDEV], [libevdev])
              AC_MSG_NOTICE([Using built-in libevdevxx.])
              EXTERNAL_SUBDIRS+=" external/libevdevxx"
              AX_SUBDIRS_CONFIGURE([external/libevdevxx],
                                   [[--disable-shared]])
          ])
AM_CONDITIONAL([INTERNAL_LIBEVDEVXX], [test "x$USE_SYSTEM_LIBEVDEVXX" = xno])

AS_VAR_IF([USE_SYSTEM_LIBGUDEVXX], [no],
          [
              PKG_CHECK_MODULES([LIBGUDEV], [gudev-1.0])
              AC_MSG_NOTICE([Using built-in libgudevxx.])
              EXTERNAL_SUBDIRS+=" external/libgudevxx"
              AX_SUBDIRS_CONFIGURE([external/libgudevxx],
                                   [[--disable-shared]])
          ])
AM_CONDITIONAL([INTERNAL_LIBGUDEVXX], [test "x$USE_SYSTEM_LIBGUDEVXX" = xno])

AC_SUBST([EXTERNAL_SUBDIRS])


AC_CONFIG_FILES([calibrate-joystick.xml
                 calibrate-joystick.gschema.xml
                 Makefile
                 po/Makefile.in])

AC_OUTPUT

