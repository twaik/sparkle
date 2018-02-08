SOURCES =
HEADERS =

CXXFLAGS = \
-Wall -I..

LIBS = ../were/libwere.a

SOURCES += main.cpp

SOURCES += sparkle_server.cpp
HEADERS += sparkle_server.h

SOURCES += sparkle_packet.c
HEADERS += sparkle_packet.h

all: test


test: ${SOURCES} ${HEADERS}
	${CXX} -o test ${SOURCES} ${CXXFLAGS} ${LIBS}

