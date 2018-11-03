AC_INIT(pcm-sparkle, 0.1)
AC_CONFIG_MACRO_DIRS([m4])
AC_PREFIX_DEFAULT(/usr)

AM_INIT_AUTOMAKE(foreign)
AM_MAINTAINER_MODE
AM_SILENT_RULES([yes])

AC_CONFIG_HEADERS(config.h)


AC_PROG_CC
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])
AC_PROG_INSTALL
AC_DISABLE_STATIC
AM_PROG_LIBTOOL
AC_HEADER_STDC


PKG_CHECK_MODULES(ALSA, alsa >= 1.0.11)
AC_CHECK_LIB(asound, snd_pcm_ioplug_create,,
    AC_ERROR([*** libasound has no external plugin SDK]), -ldl)


test "x$prefix" = xNONE && prefix=$ac_default_prefix
test "x$exec_prefix" = xNONE && exec_prefix=$prefix

dnl ALSA plugin directory
AC_ARG_WITH(plugindir,
    AS_HELP_STRING([--with-plugindir=dir],
    [path where ALSA plugin files are stored]),
    plugindir="$withval", plugindir="")

if test -z "$plugindir"; then
    eval dir="$libdir"
    case "$dir" in
    /*) ;;
    *) dir="$dir"
    esac
    plugindir="$dir/alsa-lib"
fi
AC_DEFINE_UNQUOTED(ALSA_PLUGIN_DIR, "$plugindir", [directory containing ALSA add-on modules])
ALSA_PLUGIN_DIR="$plugindir"
AC_SUBST(ALSA_PLUGIN_DIR)

dnl ALSA data directory
AC_ARG_WITH(alsadatadir,
    AS_HELP_STRING([--with-alsadatadir=dir],
    [path where ALSA data files are stored]),
    alsadatadir="$withval", alsadatadir="")
if test -z "$alsadatadir"; then
    eval dir="$datadir"
    case "$dir" in
    /*) ;;
    *) dir="$dir"
    esac
    alsadatadir="$dir/alsa"
fi
AC_DEFINE_UNQUOTED(ALSA_DATA_DIR, "$alsadatadir", [directory containing ALSA data files])
ALSA_DATA_DIR="$alsadatadir"
AC_SUBST(ALSA_DATA_DIR)



AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

dnl Show the build conditions

echo
echo "Plugin directory: $ALSA_PLUGIN_DIR"
echo

