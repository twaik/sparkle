SOURCES =
HEADERS =

CXXFLAGS = \
-Wall -I..

LIBS = ../were/libwere.a

SOURCES += main.cpp

SOURCES += sparkle_client.cpp
HEADERS += sparkle_client.h

SOURCES += sparkle_packet.c
HEADERS += sparkle_packet.h

all: test


test: ${SOURCES} ${HEADERS}
	${CXX} -o test ${SOURCES} ${CXXFLAGS} ${LIBS}

