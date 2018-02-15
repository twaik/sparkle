AM_CFLAGS = $(XORG_CFLAGS) $(CWARNFLAGS)
AM_CPPFLAGS =-I$(top_srcdir)/include -I../..

@DRIVER_NAME@_drv_la_LTLIBRARIES = @DRIVER_NAME@_drv.la
@DRIVER_NAME@_drv_la_LDFLAGS = -module -avoid-version
@DRIVER_NAME@_drv_la_LIBADD = $(MTDEV_LIBS) $(UDEV_LIBS) $(LIBEVDEV_LIBS) ../../were/libwere.a
@DRIVER_NAME@_drv_ladir = @inputdir@

@DRIVER_NAME@_drv_la_SOURCES = @DRIVER_NAME@.c \
                               @DRIVER_NAME@.h \
                               ../../common/sparkle_connection.cpp \
			                   ../../common/sparkle_connection.h \
                               ../../common/sparkle_packet.cpp \
			                   ../../common/sparkle_packet.h

