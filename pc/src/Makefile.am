#AM_CFLAGS =
#AM_CXXFLAGS =
#AM_CPPFLAGS =


#noinst_LTLIBRARIES = libsiski.la
#libsiski_la_SOURCES = siski.cpp

#libarchsiski_la_LTLIBRARIES = libarchsiski.la
#libarchsiski_ladir = "/home/sion/"
#libarchsiski_la_SOURCES = siski.cpp

AM_CPPFLAGS = -I../../were/include -I../..

bin_PROGRAMS = test

test_LDADD = ../../were/src/libwere.la -lEGL -lGLESv2 -lpthread -lX11

test_SOURCES = \
	main.cpp				\
	../../platform/x11/platform_x11.cpp	\
	../../platform/platform.h		\
	../../compositor/gl/compositor_gl.cpp	\
	../../compositor/compositor.h		\
	../../compositor/gl/texture.cpp		\
	../../compositor/gl/texture.h		\
	../../common/were_benchmark.cpp		\
	../../common/were_benchmark.h		\
	../../common/sparkle_protocol.cpp   \
	../../common/sparkle_protocol.h		\
	../../common/sparkle_server.cpp		\
	../../common/sparkle_server.h		\
	../../common/sparkle_connection.cpp	\
	../../common/sparkle_connection.h	\
	../../common/sparkle_surface_fd.cpp	\
	../../common/sparkle_surface_fd.h

#_LDFLAGS =
#_LIBADD =

