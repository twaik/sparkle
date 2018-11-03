AM_CPPFLAGS = -I../../were/include -I../..
AM_CFLAGS = -Wall -g @ALSA_CFLAGS@
AM_LDFLAGS = -module -avoid-version -export-dynamic -no-undefined $(LDFLAGS_NOUNDEFINED)

asound_module_pcm_sparkle_LTLIBRARIES = libasound_module_pcm_sparkle.la

libasound_module_pcm_sparkle_la_SOURCES = pcm_sparkle.c 			\
					sparkles_c.cpp 				\
					sparkles_c.h				\
					../../common/sparkle_sound_buffer.cpp	\
					../../common/sparkle_sound_buffer.h	\
					../../shm/shm.c				\
					../../shm/shm.h				\
					../../common/sparkle_connection.cpp	\
					../../common/sparkle_connection.h	\
					../../common/sparkle_protocol.cpp	\
					../../common/sparkle_protocol.h


asound_module_pcm_sparkledir = @ALSA_PLUGIN_DIR@

libasound_module_pcm_sparkle_la_LIBADD = @ALSA_LIBS@ ../../were/src/libwere.la

