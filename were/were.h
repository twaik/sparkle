#ifndef WERE_H
#define WERE_H

#include <stdint.h>

#ifndef __ANDROID__
#include <cstdio>
#define were_message(...) ((void) fprintf(stdout, __VA_ARGS__))
#define were_error(...) ((void) fprintf(stderr, __VA_ARGS__))
#define were_debug(...) ((void) fprintf(stdout, __VA_ARGS__))
#else
#include <android/log.h>
#define were_message(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Were", __VA_ARGS__))
#define were_error(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Were", __VA_ARGS__))
#define were_debug(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Were", __VA_ARGS__))
#endif

#endif //WERE_H

