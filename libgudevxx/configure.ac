#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.70])
AC_INIT([libgudevxx], [0.1], [])
AC_CONFIG_SRCDIR([src/client.hpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE
LT_INIT

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_LANG([C++])

AX_CXX_COMPILE_STDCXX_17([noext])

# Checks for libraries.
PKG_CHECK_MODULES([GUDEV], [gudev-1.0])
PKG_CHECK_MODULES([GLIBMM_GUDEV], [glibmm-2.4 gudev-1.0])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 utils/Makefile])
AC_OUTPUT
