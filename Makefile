SOURCES =
HEADERS =

CXXFLAGS = 
CXXFLAGS += -Wall -O2 -I. -std=c++11

LIBS = -lX11 -lEGL -lGLESv2 were/libwere.a -lpthread

SOURCES += main.cpp


SOURCES += platform/x11/platform_x11.cpp
HEADERS += platform/x11/platform_x11.h

SOURCES += compositor/gl/compositor_gl.cpp
HEADERS += compositor/gl/compositor_gl.h

SOURCES += compositor/gl/texture.cpp
HEADERS += compositor/gl/texture.h

SOURCES += common/sparkle_packet.cpp
HEADERS += common/sparkle_packet.h

SOURCES += common/sparkle_surface_file.cpp
HEADERS += common/sparkle_surface_file.h

SOURCES += common/sparkle_server.cpp
HEADERS += common/sparkle_server.h

SOURCES += common/were_benchmark.cpp
HEADERS += common/were_benchmark.h

SOURCES += common/sparkle_connection.cpp
HEADERS += common/sparkle_connection.h

all: test


test: ${SOURCES} ${HEADERS}
	${CXX} -o test ${SOURCES} ${CXXFLAGS} ${LIBS}

