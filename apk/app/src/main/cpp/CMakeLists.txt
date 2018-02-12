
cmake_minimum_required(VERSION 3.4.1)

set(SPARKLE_ROOT "../../../../../")

# build native_app_glue as a static lib
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Werror")

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(sparkle SHARED
            main.cpp
            ${SPARKLE_ROOT}/were/were_event_source.cpp
            ${SPARKLE_ROOT}/were/were_call_queue.cpp
            ${SPARKLE_ROOT}/were/were_signal_handler.cpp
            ${SPARKLE_ROOT}/were/were_event_loop.cpp
            ${SPARKLE_ROOT}/were/were_timer.cpp
            ${SPARKLE_ROOT}/were/were_socket_unix.cpp
            ${SPARKLE_ROOT}/were/were_server_unix.cpp
            ${SPARKLE_ROOT}/platform/na/platform_na.cpp
            ${SPARKLE_ROOT}/compositor/gl/compositor_gl.cpp
            ${SPARKLE_ROOT}/compositor/gl/texture.cpp
            ${SPARKLE_ROOT}/common/sparkle_packet.c
            ${SPARKLE_ROOT}/common/sparkle_server.cpp
            ${SPARKLE_ROOT}/common/sparkle_surface_file.cpp
            ${SPARKLE_ROOT}/common/were_benchmark.cpp
            ${SPARKLE_ROOT}/sound/sles/sound_sles.cpp
)

target_include_directories(sparkle PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue
    ${SPARKLE_ROOT}/
)

# add lib dependencies
target_link_libraries(sparkle
    android
    native_app_glue
    log
    EGL
    GLESv2
    OpenSLES
)

