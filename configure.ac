AC_PREREQ([2.63])
AC_INIT([led-driver], [1.0], [https://github.com/dsokoloski/led-driver/issues])
AC_CONFIG_SRCDIR([main.cpp])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
LT_INIT
AC_SUBST([LIBTOOL_DEPS])

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL

# Checks for libraries.
AC_CHECK_LIB([rt], [clock_gettime], [], [
	AC_MSG_ERROR([clock_gettime not found in librt.])])
AC_CHECK_LIB([pthread], [pthread_create], [], [
	AC_MSG_ERROR([pthread_create not found in libpthread.])])
AC_CHECK_LIB([usb-1.0], [libusb_open], [], [
	AC_MSG_ERROR([libusb_open not found in libusb-1.0.])])

# Checks for header files.
AC_CHECK_HEADERS([])

AC_CHECK_HEADERS([libusb-1.0/libusb.h], [], [],
[[#ifdef HAVE_LIBUSB_1_0_H
#include <libusb.h>
#endif
]])

# Checks for typedefs, structures, and compiler characteristics.
#AC_TYPE_SIZE_T
#AC_TYPE_SSIZE_T

# Checks for library functions.
#AC_CHECK_FUNCS([memset socket strerror])

# Output
AC_CONFIG_FILES([Makefile led-driver.spec])
AC_OUTPUT

