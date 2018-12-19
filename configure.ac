#
# @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
# @date 18 November 2018
# @project Package Manager
# @file configure.ac
#

# BEGIN PREAMBLE{{{
# Require autoconf 2.69
AC_PREREQ([2.69])
AC_INIT([Pkg-mgr],[1.0],[thomas.lenz96@gmail.com])

AS_IF([test x"$CXXFLAGS" == x],[HAVE_CXXFLAGS=0],[HAVE_CXXFLAGS=1])

AC_LANG([C++])
AC_PROG_CXX

AS_IF([test x"$HAVE_CXXFLAGS" == x0],[CXXFLAGS=""],[])

# Explicitly state the source-code directory
AC_CONFIG_SRCDIR([src])

# Build Makefile.in
# Use automake warnings, and make them fatal
# Don't enforce GNU standards at the moment
# @TODO
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects nostdinc])

# Set MKDIR_P
AC_PROG_MKDIR_P

# Set PYTHON equal to a python3 interpreter for "make check"
AM_PATH_PYTHON([3.0])

# Build config.h
AC_CONFIG_HEADERS([src/include/config.h:config.in])

# Build a Makefile with standard targets
AC_CONFIG_FILES([Makefile:Makefile.in src/Makefile:src/Makefile.in tests/unit-tests/Makefile:tests/unit-tests/Makefile.in tests/executable-tests/Makefile:tests/executable-tests/Makefile.in])
# END PREAMBLE}}}

# BEGIN CHECK HEADERS{{{
# Verifies standard headers we depend on actually exist
AC_CHECK_HEADERS([stdlib.h],[],[AC_MSG_ERROR([Fatal error. The header stdlib.h cannot be found. Per C11, this should exist in the standard C library. Without it, pkg-mgr cannot be compiled.])])
AC_CHECK_HEADERS([stdio.h],[],[AC_MSG_ERROR([Fatal error. The header stdio.h cannot be found. Per C11, this should exist in the standard C library. Without it, pkg-mgr cannot be compiled.])])
AC_CHECK_HEADERS([unistd.h],[],[AC_MSG_ERROR([Fatal error. The header unistd.h cannot be found. Per POSIX Standard, this should exist in the C library. Without it, pkg-mgr cannot be compiled.])])
AC_CHECK_HEADERS([errno.h],[],[AC_MSG_ERROR([Fatal error. The header errno.h cannot be found. Per C11, this should exist in the standard C library. Without it, pkg-mgr cannot be compiled.])])
AC_CHECK_HEADERS([string.h],[],[AC_MSG_ERROR([Fatal error. The header string.h cannot be found. Per C11, this should exist in the standard C library. Without it, pkg-mgr cannot be compiled.])])
AC_CHECK_HEADERS([fcntl.h],[],[AC_MSG_ERROR([Fatal error. The header fcntl.h cannot be found. Per the POSIX Standard, this should exist in the C library. Without it, pkg-mgr cannot be compiled.])])

AC_CHECK_HEADERS([vector],[],[AC_MSG_ERROR([Fatal error. The header vector cannot be found. Per C++17, this should exist in the standard C++ library. Without it, pkg-mgr cannot be compiled.])])
AC_CHECK_HEADERS([string],[],[AC_MSG_ERROR([Fatal error. The header string cannot be found. Per C++17, this should exist in the standard C++ library. Without it, pkg-mgr cannot be compiled.])])
AC_CHECK_HEADERS([fstream],[],[AC_MSG_ERROR([Fatal error. The header fstream cannot be found. Per C++17, this should exist in the standard C++ library. Without it, pkg-mgr cannot be compiled.])])
AC_CHECK_HEADERS([cmath],[],[AC_MSG_ERROR([fatal error. The header cmath cannot be found. per c++17, this should exist in the standard c++ library. without it, pkg-mgr cannot be compiled.])])
AC_CHECK_HEADERS([map],[],[AC_MSG_ERROR([Fatal error. The header map cannot be found. Per C++17, this should exist in the standard C++ library. Without it, pkg-mgr cannot be compiled.])])
AC_CHECK_HEADERS([set],[],[AC_MSG_ERROR([Fatal error. The header set cannot be found. Per C++17, this should exist in the standard C++ library. Without it, pkg-mgr cannot be compiled.])])
# @TODO Make this optional
AC_CHECK_HEADERS([system_error],[],[AC_MSG_ERROR([Fatal error. The header system_error cannot be found. Per C++17, this should exist in the standard C++ library. Without it, pkg-mgr cannot be compiled.])])

# @TODO See if we can replace this with experimental/filesystem
# Until then, we also need to enforce C++17
#AX_CXX_COMPILE_STDCXX([17],[],[mandatory])
AX_CXX_COMPILE_STDCXX([17],[],[optional])
AM_CONDITIONAL([HAVE_CXX17],[test "x$HAVE_CXX17" = xyes])

AC_CHECK_HEADERS([filesystem],[],[AC_MSG_ERROR([Fatal error. The header filesystem cannot be found. Per C++17, this should exist in the standard C++ library. Without it, pkg-mgr cannot be compiled.])])

AC_CHECK_HEADERS([archive.h],[],[AC_MSG_ERROR([Fatal error. The header archive.h cannot be found. pkg-mgr depend heavily on it, and its associated library, libarchive. Without it, pkg-mgr cannot be compiled.])])
AC_CHECK_HEADERS([archive_entry.h],[],[AC_MSG_ERROR([Fatal error. The header archive_entry.h cannot be found. pkg-mgr depend heavily on it, and its associated library, libarchive. Without it, pkg-mgr cannot be compiled.])])

# This one allows us to use long arguments, such as --verbose. If not, fall back to getopt, defined in unistd.h
# @TODO Make this an optional dependancy
AC_CHECK_HEADERS([getopt.h],[],[AC_MSG_ERROR([Fatal error. The header getopt.h cannot be found. This is a GNU extension to getopt. Without it, pkg-mgr cannot be compiled with support for long-form arguments, such as --verbose, but instead only with short options, such as -v.])])
# END CHECK HEADERS}}}

# BEGIN LIBRARY CHECK{{{
AC_CHECK_LIB([archive],[archive_version_number],[WITH_ARCHIVE=yes],[AC_MSG_ERROR([Fatal error. The libarchive library cannot be found. pkg-mgr depends heavily on it. Without it, pkg-mgr cannot be compiled.])])
AM_CONDITIONAL([WITH_ARCHIVE],[test x"$WITH_ARCHIVE" = xyes])
# END LIBRARY CHECK#}}}

# BEGIN C++17 FILESYSTEM LIBRARY CHECK{{{
# Because stdc++fs has no functions with C linkage, we need to do this workaround
AC_MSG_CHECKING([for the C++17 filesystem library])
SAVED_LIBS=$LIBS
LIBS="$LIBS -lstdc++fs"
AC_LINK_IFELSE(
               [AC_LANG_PROGRAM([#include <filesystem>],
               [std::filesystem::exists("/")])],
               [TEST_LIBS="$TEST_LIBS -lstdc++fs"] [WITH_STDCXXFS="yes"],
               [WITH_STDCXXFS="no"]
              )

# Return our libs to their prior state
LIBS=$SAVED_LIBS

# Act on the result of the link test above
AS_IF([test x"$WITH_STDCXXFS" = xyes],[
    AC_DEFINE([WITH_STDCXXFS],[1],[Define to 1 if you have the filesystem library (-lstdc++fs).])
])
AM_CONDITIONAL([WITH_STDCXXFS],[test "x$WITH_STDCXXFS" = xyes])

# Report the result
AC_MSG_RESULT([$WITH_STDCXXFS])
# END C++17 FILESYSTEM LIBRARY CHECH}}}

# BEGIN DEFAULT DEFINITIONS{{{
# Get our package's default configuration options
AC_ARG_VAR([DEFAULT_VERBOSITY],[Sets the default verbosity the program should use. Unless specifically set by the command-line, this is also used as the verbosity while processing  configuration files.])
AC_ARG_VAR([DEFAULT_SMART_OP],[Tells the program whether or not to use "Smart Mode" by default. This mode helps prevent accidental file overwrites or deletions. THIS IS CURRENTLY NOT IMPLEMENTED!!!!])
AC_ARG_VAR([DEFAULT_TAR_LIBRARY_PATH],[Sets the default path which the program will look for packages. This is relative to the system root, and is *not* relative to the prefix given to configure.])
AC_ARG_VAR([DEFAULT_INSTALLED_PKG_PATH],[Sets the default path to use to store information on which packages are installed on the system. This is relative to the system root, and is *not* relative to the prefix given to configure.])

# @TODO Consider using configure's sysconf instead for this. Should be an easy refactor.
AC_ARG_VAR([DEFAULT_GLOBAL_CONFIG_PATH],[Sets the default path to pull up when looking for the global configuration file. This must be absolute, and is not relative to the prefix given to configure.])

AC_ARG_VAR([DEFAULT_USER_CONFIG_PATH],[Sets the default path to pull up when looking for user configuration files. This is relative to the user's home directory. It is *not* affected by configure's prefix. nor is it affected by the system root options.])

# @TODO Consider using configure's prefix instead for this. Should be an easy refactor.
AC_ARG_VAR([DEFAULT_SYSTEM_ROOT],[Sets the default system root, which is attached to each relevant path, such as the tar library path or the global configuration path.])

AC_ARG_VAR([DEFAULT_EXCLUDED_FILES],[Sets the default files which we should ignore when installing or uninstalling packages. This has no effect on the files we use to mark installed packages. THIS IS NOT CURRENTLY IMPLEMENTED, AND LIKELY NEVER WILL BE IN CONFIGURE!! This is only here as a placeholder in case I do decide to implement it.])

# Process and define them
AC_MSG_CHECKING([for the default verbosity])
AS_IF([test "x$DEFAULT_VERBOSITY" != x],
      [defaultVerbosity=$DEFAULT_VERBOSITY],
      [defaultVerbosity=2]
     )
AC_SUBST([defaultVerbosity],["$defaultVerbosity"])
AC_MSG_RESULT([$defaultVerbosity])

AC_MSG_CHECKING([for the default smart operation])
AS_IF([test "x$DEFAULT_SMART_OP" != x],
      [defaultSmartOp=$DEFAULT_SMART_OP],
      [defaultSmartOp=true]
     )
AC_SUBST([defaultSmartOp],["$defaultSmartOp"])
AC_MSG_RESULT([$defaultSmartOp])

AC_MSG_CHECKING([for the default tar library path])
AS_IF([test "x$DEFAULT_TAR_LIBRARY_PATH" != x],
      [defaultTarLibPath="$DEFAULT_TAR_LIBRARY_PATH"],
      [defaultTarLibPath="\${prefix}/var/lib/pkg-mgr/pkgs/"]
     )
AC_SUBST([defaultTarLibPath],["$defaultTarLibPath"])
AC_MSG_RESULT([$defaultTarLibPath])

AC_MSG_CHECKING([for the installed package index path])
AS_IF([test "x$DEFAULT_INSTALLED_PKG_PATH" != x],
      [defaultInstalledPkgPath="$DEFAULT_INSTALLED_PKG_PATH"],
      [defaultInstalledPkgPath="\${prefix}/var/lib/pkg-mgr/installed/"]
     )
AC_SUBST([defaultInstalledPkgPath],["$defaultInstalledPkgPath"])
AC_MSG_RESULT([$defaultInstalledPkgPath])

AC_MSG_CHECKING([for the default global configuration file path])
AS_IF([test "x$DEFAULT_GLOBAL_CONFIG_PATH" != x],
      [defaultGlobalConfigPath="$DEFAULT_GLOBAL_CONFIG_PATH"],
      [defaultGlobalConfigPath="\${sysconfdir}/"]
     )
AC_SUBST([defaultGlobalConfigPath],["$defaultGlobalConfigPath"])
AC_MSG_RESULT([$defaultGlobalConfigPath])

AC_MSG_CHECKING([for the default user configuration file path])
AS_IF([test "x$DEFAULT_USER_CONFIG_PATH" != x],
      [defaultUserConfigPath="$DEFAULT_USER_CONFIG_PATH"],
      [defaultUserConfigPath="/.config/"]
     )
AC_SUBST([defaultUserConfigPath],["$defaultUserConfigPath"])
AC_MSG_RESULT([$defaultUserConfigPath])

AC_MSG_CHECKING([for the default system root])
AS_IF([test "x$DEFAULT_SYSTEM_ROOT" != x],
      [defaultSystemRoot="$DEFAULT_SYSTEM_ROOT"],
      [defaultSystemRoot="\${prefix}/"]
     )
AC_SUBST([defaultSystemRoot],["$defaultSystemRoot"])
AC_MSG_RESULT([$defaultSystemRoot])

AC_MSG_CHECKING([for the default excluded files])
AS_IF([test "x$DEFAULT_EXCLUDED_FILES" != x],
      [defaultExcludedFiles=$DEFAULT_EXCLUDED_FILES],
      [defaultExcludedFiles={}]
     )
AC_SUBST([defaultExcludedFiles],["$defaultExcludedFiles"])
AC_MSG_RESULT([$defaultExcludedFiles])

# END DEFAULT DEFINITIONS}}}

# BEGIN CHECK LIBRARY FUNCTIONS{{{
# Checks for library functions.
AC_CHECK_FUNC([pow],[],[AC_MSG_ERROR([Fatal error. The function pow, provided by cmath/math.h, cannot be found. Per C11 and C++17, this should exist in the standard library. Without it, pkg-mgr cannot be compiled.])])
# @TODO Make this an optional dependency
AC_CHECK_FUNC([strerror],[],[AC_MSG_ERROR([Fatal error. The function strerror, provided by cstring/string.h, cannot be found. Per C11 and C++17, this should exist in the standard library. Without it, pkg-mgr cannot be compiled.])])
# END CHECK LIBRARY FUNCTIONS}}}

# Aaaannnndddddd generate!!
AC_OUTPUT
