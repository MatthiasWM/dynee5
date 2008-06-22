AC_PREREQ(2.50)

AC_INIT(DyneTK, 1.7.2)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([1.6.1 foreign no-installinfo no-installman subdir-objects])
#AC_CONFIG_HEADER(engine/common/include/config.h)
AC_CONFIG_MACRO_DIR([m4])

##AC_CANONICAL_TARGET

dnl Check for programs
AC_LANG([C++])
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_LIBTOOL


# ***** DEBUGGING
AC_ARG_ENABLE(debug,AS_HELP_STRING([--enable-debug],[enable debugging @<:@default=no@:>@]))
if test "$enable_debug" = "yes"; then
    CXXFLAGS="$CXXFLAGS -g -O0 -DDEBUG"
else
    CXXFLAGS="$CXXFLAGS -O2 -DNDEBUG -s"
    INSTALL="$INSTALL -s"
fi

# Pthreads
CXXFLAGS="$CXXFLAGS -pthread"


# Newt0
AC_CHECK_LIB([newt], [main], [], AC_MSG_ERROR([Can't find libNewt0]))
for ac_path_tmp in /usr /usr/local /opt /opt/local ; do
	if test -d "$ac_path_tmp/include/newt0" && test -r "$ac_path_tmp/include/newt0"; then
		CXXFLAGS="$CXXFLAGS -I$ac_path_tmp/include/newt0";
		break;
	fi
done


# FLTK
AC_PATH_PROG(config_path, fltk-config, no)
if test $config_path = no; then
    AC_MSG_ERROR(Cannot find fltk-config! This program requires FLTK > 1.1.8)
fi

dnl determine if the installed FLTK is suitable for us
if test "`$config_path --version`" > "1.1.7"; then
		AC_MSG_NOTICE([FLTK has the right version!])
else
    AC_MSG_ERROR([Your FLTK is too old! At least FLTK-1.1.8 is required])
fi
CXXFLAGS="$CXXFLAGS -Wall `fltk-config --use-images --cxxflags`"
LDFLAGS="$LDFLAGS `fltk-config --use-images --ldflags`"

dnl Set the Flags

dnl Create the makefiles
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

eval TK_DIR="${bindir}"
eval TK_DIR="${TK_DIR}"

dnl Show a summary about important directories
echo
echo "***********************************************************"
echo "*         DyneTK - Configuration Results Summary          *"
echo "***********************************************************"
echo
echo " Code options:"
echo "       Building DyneTk in debug mode:      ${enable_debug-no}"
echo
echo
echo " Directory settings:"
echo "       DyneTk binary will be placed in:    ${TK_DIR}"
echo


echo
echo "Type make && make install"
