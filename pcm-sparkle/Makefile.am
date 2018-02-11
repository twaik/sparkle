asound_module_pcm_sparkle_LTLIBRARIES = libasound_module_pcm_sparkle.la

asound_module_pcm_sparkledir = @ALSA_PLUGIN_DIR@

AM_CFLAGS = -Wall -g @ALSA_CFLAGS@ -I..
AM_LDFLAGS = -module -avoid-version -export-dynamic -no-undefined $(LDFLAGS_NOUNDEFINED)

libasound_module_pcm_sparkle_la_SOURCES = pcm_sparkle.c

libasound_module_pcm_sparkle_la_LIBADD = @ALSA_LIBS@


