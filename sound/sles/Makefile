ANDROID_CXX = /home/sion/android/toolchains/weston/bin/arm-linux-androideabi-g++

SOURCES = 
HEADERS = 
OBJECTS = 

SOURCES += main.cpp

SOURCES += sound_sles.cpp
HEADERS += sound_sles.h

SOURCES += ../../were/were_event_loop.cpp
HEADERS += ../../were/were_event_loop.h

SOURCES += ../../were/were_call_queue.cpp
HEADERS += ../../were/were_call_queue.h

SOURCES += ../../were/were_signal_handler.cpp
HEADERS += ../../were/were_signal_handler.h

SOURCES += ../../were/were_event_source.cpp
HEADERS += ../../were/were_event_source.h

SOURCES += ../../were/were_server_unix.cpp
HEADERS += ../../were/were_server_unix.h

SOURCES += ../../were/were_socket_unix.cpp
HEADERS += ../../were/were_socket_unix.h



CXXFLAGS = 
CXXFLAGS += -Wall -O2 -I../.. -std=c++11
CXXFLAGS += -fPIC

all: sles

sles: ${SOURCES} ${HEADERS}
	${ANDROID_CXX} ${CXXFLAGS} -o sles ${SOURCES} -lOpenSLES -pie -llog


