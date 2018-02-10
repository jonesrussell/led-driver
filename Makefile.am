AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = autogen.sh deploy include hidapi

DISTCHECK_CONFIGURE_FLAGS = \
	--with-systemdsystemunitdir=$$dc_install_base/$(systemdsystemunitdir)

if HAVE_SYSTEMD
systemdsystemunit_DATA = \
	deploy/led-driver.service
endif

AM_CFLAGS = ${CFLAGS} -I ./include -I ./hidapi -I /usr/include/libusb-1.0
AM_CXXFLAGS = ${CXXFLAGS} -I ./include -I ./hidapi -I /usr/include/libusb-1.0

lib_LTLIBRARIES = libled-driver.la
libled_driver_la_SOURCES = ld-abstract.cpp ld-lightpack.cpp lib-sequence.cpp lib-socket.cpp ./hidapi/linux/hid-libusb.c
libled_driver_la_CFLAGS = ${AM_CFLAGS}
libled_driver_la_CXXFLAGS = ${AM_CXXFLAGS}

bin_PROGRAMS = led-ctrl
led_ctrl_SOURCES = led-ctrl.cpp
led_ctrl_LDADD = libled-driver.la
led_ctrl_CXXFLAGS = ${AM_CXXFLAGS}

sbin_PROGRAMS = led-server
led_server_SOURCES = led-server.cpp led-server-thread.cpp
led_server_LDADD = libled-driver.la
led_server_CXXFLAGS = ${AM_CXXFLAGS}

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

