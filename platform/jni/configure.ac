AC_PREREQ([2.60])
AC_INIT([libsparkle], [0.1.2], [])

AC_CONFIG_AUX_DIR([.])
AC_PROG_CXX
AC_PROG_CC

AM_INIT_AUTOMAKE([])

if test $cross_compiling = no; then
  AM_MISSING_PROG(HELP2MAN, help2man)
else
  HELP2MAN=:
fi

LT_INIT([])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
