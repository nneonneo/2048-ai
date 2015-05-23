#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_CONFIG_MACRO_DIR([m4])

AC_PREREQ([2.69])
AC_INIT([2048 AI], [1.0], [https://github.com/nneonneo/2048-ai/issues], [2048-ai], [https://github.com/nneonneo/2048-ai])
AC_CONFIG_SRCDIR([2048.cpp])
AC_CONFIG_HEADERS([config.h])

AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([], [optional])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CC
AC_PROG_MKDIR_P

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/time.h unistd.h unordered_map tr1/unordered_map])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_UINT16_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday strchr arc4random_uniform drand48])

AC_OUTPUT(Makefile)
