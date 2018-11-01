#ifndef WERE_H
#define WERE_H

#include "were_exception.h"

#ifdef __ANDROID__
#include <android/log.h>
#define were_message(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Were", __VA_ARGS__))
#define were_error(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Were", __VA_ARGS__))
#define were_debug(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Were", __VA_ARGS__))
#else
#include <cstdio>
#define were_message(...) ((void) fprintf(stdout, __VA_ARGS__))
#define were_error(...) ((void) fprintf(stderr, __VA_ARGS__))
#define were_debug(...) ((void) fprintf(stdout, __VA_ARGS__))
#endif

#define werethings public

inline void were_timestamp(timespec *ts)
{
    if (ts == nullptr)
    {
        timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        were_timestamp(&now);
    }
    else
        were_message("[%ld.%.9ld] ", ts->tv_sec, ts->tv_nsec);
}

#endif /* WERE_H */
