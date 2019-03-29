lib_LTLIBRARIES = libsparkle.la
libsparkle_la_SOURCES =									\
	../../common/sparkle_connection.cpp					\
	../../common/sparkle_server.cpp						\
	../../common/sparkle_protocol.cpp					\
	../../common/sparkle_surface_shm.cpp				\
	../../common/sparkle_surface_ashmem.cpp				\
	../../common/were_benchmark.cpp						\
	../../were/src/were_event_source.cpp				\
	../../were/src/were_call_queue.cpp					\
	../../were/src/were_signal_handler.cpp				\
	../../were/src/were_event_loop.cpp					\
	../../were/src/were_timer.cpp						\
	../../were/src/were_socket_unix.cpp					\
	../../were/src/were_server_unix.cpp					\
	../../were/src/were_stream.cpp						\
	../../were/src/were_socket_unix_message_stream.cpp	\
	../../were/src/were_exception.cpp					\
	../../shm/shm.c										\
	../../compositor/gl/compositor_gl.cpp				\
	../../compositor/gl/texture.cpp						\
	platform_jni.cpp
libsparkle_la_CXXFLAGS = -std=gnu++11 -Wall -fexceptions -I../../were/include -I../../were/include/were -I../../
