#ifndef SPARKLES_C_H
#define SPARKLES_C_H

/* ================================================================================================================== */

#ifdef __cplusplus

class SparklesC;

#else

struct SparklesC;
typedef struct SparklesC SparklesC;

#endif

/* ================================================================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

SparklesC *sparkles_c_create(const char *server);
void sparkles_c_destroy(SparklesC *c);

int sparkles_c_fd(SparklesC *c);
void sparkles_c_process(SparklesC *c);

int sparkles_c_start(SparklesC *c);
int sparkles_c_stop(SparklesC *c);
int sparkles_c_write(SparklesC *c, const void *data, int size);
int sparkles_c_pointer(SparklesC *c);

#ifdef __cplusplus
}
#endif

/* ================================================================================================================== */

#endif /* SPARKLES_C_H */
