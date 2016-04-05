AC_PREREQ([2.63])
AC_INIT([LED Driver], [1.0],
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

# Output
AC_CONFIG_FILES([Makefile led-driver.spec led-driver.init])
AC_OUTPUT

