all: test

SOURCES = 
HEADERS = 

CXXFLAGS = -I..

SOURCES += main.cpp

SOURCES += ../common/sparkle_client.c
HEADERS += ../common/sparkle_client.h

SOURCES += ../common/sparkle_packet.c
HEADERS += ../common/sparkle_packet.h

SOURCES += widget_host.cpp
HEADERS += widget_host.h

SOURCES += widget_button.cpp
HEADERS += widget_button.h

SOURCES += keyboard.cpp
HEADERS += keyboard.h


test: ${SOURCES} ${HEADERS}
	${CXX} -o test ${CXXFLAGS} ${SOURCES}

