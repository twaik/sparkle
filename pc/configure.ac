AC_INIT([Tutorial Program], [1.0])
AC_CONFIG_MACRO_DIRS([m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE
AM_SILENT_RULES([yes])

LT_INIT([disable-static])

AC_PROG_CC
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])

CFLAGS="$CFLAGS -Wall -Wextra"
CXXFLAGS="$CXXFLAGS -Wall -Wextra"

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

