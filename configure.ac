AC_PREREQ([2.63])
AC_INIT([LED Driver], [1.1],
	[https://github.com/dsokoloski/led-driver/issues],
	[led-driver], [http://sokoloski.ca/])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([led-ctrl.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

# Checks for libraries.
AC_CHECK_LIB([rt], [clock_gettime], [], [
	AC_MSG_ERROR([clock_gettime not found in librt.])])
AC_CHECK_LIB([pthread], [pthread_create], [], [
	AC_MSG_ERROR([pthread_create not found in libpthread.])])
AC_CHECK_LIB([usb-1.0], [libusb_open], [], [
	AC_MSG_ERROR([libusb_open not found in libusb-1.0.])])

# Checks for header files.
#AC_CHECK_HEADERS([])

AC_CHECK_HEADERS([libusb-1.0/libusb.h], [], [],
[[#ifdef HAVE_LIBUSB_1_0_H
#include <libusb.h>
#endif
]])

#AC_CHECK_HEADERS([proc/readproc.h], [], [AC_MSG_ERROR([procps header files not found.])])

# Checks for typedefs, structures, and compiler characteristics.
#AC_TYPE_SIZE_T
#AC_TYPE_SSIZE_T

# Checks for library functions.
#AC_CHECK_FUNCS([memset socket strerror])

# Check if PACKAGE_URL is defined
if test -z "$PACKAGE_URL"; then
	AC_SUBST([PACKAGE_URL], [https://github.com/dsokoloski/led-driver])
fi

# Check for systemd support
AC_ARG_WITH([systemdsystemunitdir],
    [AS_HELP_STRING([--with-systemdsystemunitdir=DIR], [Directory for systemd service files])],,
    [with_systemdsystemunitdir=auto])
AS_IF([test "x$with_systemdsystemunitdir" = "xyes" -o "x$with_systemdsystemunitdir" = "xauto"], [
    def_systemdsystemunitdir=$($PKG_CONFIG --variable=systemdsystemunitdir systemd)

    AS_IF([test "x$def_systemdsystemunitdir" = "x"],
    [AS_IF([test "x$with_systemdsystemunitdir" = "xyes"],
    [AC_MSG_ERROR([systemd support requested but pkg-config unable to query systemd package])])
    with_systemdsystemunitdir=no],
    [with_systemdsystemunitdir="$def_systemdsystemunitdir"])])
AS_IF([test "x$with_systemdsystemunitdir" != "xno"],
    [AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])])
AM_CONDITIONAL([HAVE_SYSTEMD], [test "x$with_systemdsystemunitdir" != "xno"])

# Output
AC_CONFIG_FILES([Makefile led-driver.spec deploy/led-driver.init])
AC_OUTPUT

