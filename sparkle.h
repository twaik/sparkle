#ifndef SPARKLE_H
#define SPARKLE_H

#ifndef __ANDROID__
#include <cstdio>
#define sparkle_message(...) ((void) fprintf(stdout, __VA_ARGS__))
#define sparkle_error(...) ((void) fprintf(stderr, __VA_ARGS__))
#else
#include <android/log.h>
#define sparkle_message(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Sparkle", __VA_ARGS__))
#define sparkle_error(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Sparkle", __VA_ARGS__))
#endif

#include "were/were_function.h"
#include "utility.h"

#endif //SPARKLE_H

