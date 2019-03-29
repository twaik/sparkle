LOCAL_PATH := $(call my-dir)
SPARKLE_C_INCLUDES := $(LOCAL_PATH)/ $(LOCAL_PATH)/were/include $(LOCAL_PATH)/were/include/were

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
LOCAL_LDFLAGS	:= -u JNI_OnLoad
LOCAL_LDLIBS	:= -landroid -llog -lEGL -lGLESv2 -lOpenSLES
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/src $(SPARKLE_C_INCLUDES)
LOCAL_SRC_FILES := \
	platform/jni/platform_jni.cpp					\
	compositor/gl/compositor_gl.cpp					\
	compositor/gl/texture.cpp						\
	common/sparkle_protocol.cpp

LOCAL_STATIC_LIBRARIES := libsparkle_common
include $(BUILD_SHARED_LIBRARY)
