AC_PREREQ([2.65])
AC_INIT([luadeamon], [1.0.0], [smherwig@gmail.com], [luadaemon],
    [http://smherwig.org])
AM_INIT_AUTOMAKE
LT_PREREQ([2.2])
LT_INIT

AC_CONFIG_SRCDIR([include/lua-daemon.h])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AC_PROG_INSTALL

missing_header=no
AC_CHECK_HEADERS([fcntl.h string.h unistd.h lua5.1/lua.h lua5.1/lualib.h lua5.1/lauxlib.h], 
    [], 
    [missing_header=yes])

if test "x${missing_header}" = xyes; then
    AC_MSG_ERROR([Missing required header file])
fi

AC_FUNC_FORK
AC_CHECK_FUNCS([dup2 strdup strerror])

AC_CONFIG_FILES([Makefile
            include/Makefile
            share/Makefile
            src/Makefile])

AC_OUTPUT

echo \
"-----------------------------------------------------------

${PACKAGE_NAME} version ${PACKAGE_VERSION}

Prefix: '${prefix}'

Now type 'make @<:@<taret>@:>@'
    where the optional <target> is:
        all         - build all binaries
        install     - install everything

-----------------------------------------------------------"

