#ifndef SPARKLE_C_H
#define SPARKLE_C_H

/* ================================================================================================================== */

#ifdef __cplusplus

class SparkleC;

#else

struct SparkleC;
typedef struct SparkleC SparkleC;

#endif

/* ================================================================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

SparkleC *sparkle_c_create(const char *compositor, const char *surface_name, const char *surface_file);
void sparkle_c_destroy(SparkleC *c);

int sparkle_c_fd(SparkleC *c);
void sparkle_c_process(SparkleC *c);

void *sparkle_c_surface_data(SparkleC *c);
void sparkle_c_damage(SparkleC *c, int x1, int y1, int x2, int y2);
void sparkle_c_resize_surface(SparkleC *c, int width, int height);

void sparkle_c_set_display_size_cb(SparkleC *c, void (*f)(void *user, int width, int height), void *user);

#ifdef __cplusplus
}
#endif

/* ================================================================================================================== */

#endif /* SPARKLE_C_H */
