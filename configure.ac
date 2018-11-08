AC_INIT([libhomfly], [1.02r6], [mmarco@unizar.es])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall])
AM_PROG_AR
AC_PROG_CC
LT_INIT([win32-dll])
AC_CANONICAL_HOST
dnl libtool requires "-no-undefined" for win32 dll
AC_SUBST(HOMFLY_LDFLAGS)
case $host_os in
    *cygwin* | *mingw*)
        if test x"$enable_shared" = xyes; then
            HOMFLY_LDFLAGS="$HOMFLY_LDFLAGS -no-undefined"
        fi
    ;;
esac
AC_CONFIG_FILES([Makefile lib/Makefile test/Makefile])
AC_OUTPUT

