LOCAL_PATH := $(call my-dir)
SPARKLE_C_INCLUDES := $(LOCAL_PATH)/ $(LOCAL_PATH)/were/include $(LOCAL_PATH)/were/include/were

ifeq (0,1)
include external/xorg-server/modules.mk
endif

include $(CLEAR_VARS)

LOCAL_MODULE    := libsparkle_common
LOCAL_SRC_FILES	:= 									\
	common/sparkle_connection.cpp					\
	common/sparkle_server.cpp						\
	common/sparkle_protocol.cpp						\
	common/sparkle_surface_shm.cpp					\
	common/sparkle_surface_ashmem.cpp				\
	common/were_benchmark.cpp						\
	were/src/were_event_source.cpp					\
	were/src/were_call_queue.cpp					\
	were/src/were_signal_handler.cpp				\
	were/src/were_event_loop.cpp					\
	were/src/were_timer.cpp							\
	were/src/were_socket_unix.cpp					\
	were/src/were_server_unix.cpp					\
	were/src/were_stream.cpp						\
	were/src/were_socket_unix_message_stream.cpp	\
	were/src/were_exception.cpp						\
	shm/shm.c
LOCAL_CXXFLAGS	:= -std=gnu++11 -Wall -fexceptions
LOCAL_C_INCLUDES:= $(SPARKLE_C_INCLUDES)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libsparkle
LOCAL_CFLAGS    := -DHAVE_CONFIG_H
LOCAL_CXXFLAGS	:= -std=gnu++11 -Wall -fexceptions
LOCAL_LDFLAGS	:= -u ANativeActivity_onCreate
LOCAL_LDLIBS	:= -landroid -llog -lEGL -lGLESv2 -lOpenSLES
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/src $(GLOBAL_X11_INCLUDES) $(SPARKLE_C_INCLUDES)
LOCAL_C_INCLUDES += external/libandroidjni/jutils external/libandroidjni/src external/native_app_glue
LOCAL_SRC_FILES := \
	apk/app/src/main/cpp/main.cpp					\
	platform/na/platform_na.cpp						\
	compositor/gl/compositor_gl.cpp					\
	compositor/gl/texture.cpp						\
	sound/sles/sound_sles.cpp						\
	common/sparkle_protocol.cpp						\
	common/sparkle_sound_buffer.cpp

LOCAL_STATIC_LIBRARIES := libsparkle_common android_native_app_glue
include $(BUILD_SHARED_LIBRARY)

ifeq (0,1)
include $(CLEAR_VARS)
LOCAL_MODULE    := sparkle_drv
LOCAL_CFLAGS    := -DHAVE_CONFIG_H
LOCAL_C_INCLUDES := 					\
	$(LOCAL_PATH) 						\
	$(LOCAL_PATH)/src 					\
	$(SPARKLE_C_INCLUDES) 				\
	$(LOCAL_PATH)/xf86-video-sparkle/	\
	$(LOCAL_PATH)/xf86-video-sparkle/src

LOCAL_SRC_FILES := \
	xf86-video-sparkle/src/dummy_cursor.c \
	xf86-video-sparkle/src/dummy_driver.c \
	xf86-video-sparkle/src/sparkle_c.cpp
LOCAL_STATIC_LIBRARIES := libsparkle_common
include $(BUILD_XORG_MODULE)

include $(CLEAR_VARS)
LOCAL_MODULE    := sparklei_drv
LOCAL_CFLAGS    := -DHAVE_CONFIG_H
LOCAL_C_INCLUDES := 					\
	$(LOCAL_PATH) 						\
	$(LOCAL_PATH)/src 					\
	$(SPARKLE_C_INCLUDES) 				\
	$(LOCAL_PATH)/xf86-video-sparkle/	\
	$(LOCAL_PATH)/xf86-video-sparkle/src

LOCAL_SRC_FILES := 							\
	xf86-input-sparklei/src/sparklei.c		\
	xf86-input-sparklei/src/sparklei_c.cpp	\

LOCAL_STATIC_LIBRARIES := libsparkle_common
include $(BUILD_XORG_MODULE)
endif

$(call import-module,android/native_app_glue)
