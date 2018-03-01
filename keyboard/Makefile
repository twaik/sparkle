all: test

SOURCES = 
HEADERS = 

CXXFLAGS = -Wall -O2 -I../were/include -I..
LIBS = ../were/src/.libs/libwere.a -lpthread

SOURCES += main.cpp

SOURCES += ../common/sparkle_connection.cpp
HEADERS += ../common/sparkle_connection.h

SOURCES += ../common/sparkle_packet.cpp
HEADERS += ../common/sparkle_packet.h

SOURCES += ../common/sparkle_surface_file.cpp
HEADERS += ../common/sparkle_surface_file.h

SOURCES += ../ui/were_painter.cpp
HEADERS += ../ui/were_painter.h

SOURCES += ../ui/widget_host.cpp
HEADERS += ../ui/widget_host.h

SOURCES += sparkle_keyboard.cpp
HEADERS += sparkle_keyboard.h

SOURCES += ../ui/widget_button.cpp
HEADERS += ../ui/widget_button.h

SOURCES += ../common/sparkle_protocol.cpp
HEADERS += ../common/sparkle_protocol.h


test: ${SOURCES} ${HEADERS}
	${CXX} -o test ${CXXFLAGS} ${SOURCES} ${LIBS}

