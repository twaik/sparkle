ANDROID_CXX = /home/sion/android/toolchains/weston/bin/arm-linux-androideabi-g++

SOURCES = 
HEADERS = 
OBJECTS = 

HEADERS += were.h

HEADERS += were_function.h

SOURCES += were_event_source.cpp
HEADERS += were_event_source.h
OBJECTS += were_event_source.o

SOURCES += were_event_loop.cpp
HEADERS += were_event_loop.h
OBJECTS += were_event_loop.o

SOURCES += were_timer.cpp
HEADERS += were_timer.h
OBJECTS += were_timer.o

SOURCES += were_server_unix.cpp
HEADERS += were_server_unix.h
OBJECTS += were_server_unix.o

SOURCES += were_socket_unix.cpp
HEADERS += were_socket_unix.h
OBJECTS += were_socket_unix.o

SOURCES += were_call_queue.cpp
HEADERS += were_call_queue.h
OBJECTS += were_call_queue.o

CXXFLAGS = 
CXXFLAGS += -Wall
CXXFLAGS += -fPIC

all: libwere.a

libwere.a: ${OBJECTS} ${HEADERS}
	ar rcs libwere.a ${OBJECTS}

%.o: %.cpp ${HEADERS}
	${CXX} ${CXXFLAGS} -o $@ -c $<

clean:
	rm -rf libwere.a *.o


