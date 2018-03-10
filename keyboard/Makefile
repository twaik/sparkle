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

SOURCES += ../were-graphics/were_painter.cpp
HEADERS += ../were-graphics/were_painter.h

SOURCES += ../were-graphics/were_widget.cpp
HEADERS += ../were-graphics/were_widget.h

SOURCES += sparkle_keyboard.cpp
HEADERS += sparkle_keyboard.h

SOURCES += ../were-graphics/were_button.cpp
HEADERS += ../were-graphics/were_button.h

SOURCES += ../common/sparkle_protocol.cpp
HEADERS += ../common/sparkle_protocol.h


test: ${SOURCES} ${HEADERS}
	${CXX} -o test ${CXXFLAGS} ${SOURCES} ${LIBS}

clean:
	rm -rf test

