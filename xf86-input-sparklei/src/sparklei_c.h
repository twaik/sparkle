#ifndef SPARKLEI_C_H
#define SPARKLEI_C_H

/* ================================================================================================================== */

#ifdef __cplusplus

class SparkleiC;

#else

struct SparkleiC;
typedef struct SparkleiC SparkleiC;

#endif

/* ================================================================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

SparkleiC *sparklei_c_create(const char *compositor, const char *surface_name);
void sparklei_c_destroy(SparkleiC *c);

int sparklei_c_fd(SparkleiC *c);
void sparklei_c_process(SparkleiC *c);

void sparklei_c_set_pointer_down_cb(SparkleiC *c, void (*f)(void *user, int slot, int x, int y), void *user);
void sparklei_c_set_pointer_up_cb(SparkleiC *c, void (*f)(void *user, int slot), void *user);
void sparklei_c_set_pointer_motion_cb(SparkleiC *c, void (*f)(void *user, int slot, int x, int y), void *user);
void sparklei_c_set_key_down_cb(SparkleiC *c, void (*f)(void *user, int code), void *user);
void sparklei_c_set_key_up_cb(SparkleiC *c, void (*f)(void *user, int code), void *user);

#ifdef __cplusplus
}
#endif

/* ================================================================================================================== */

#endif /* SPARKLEI_C_H */
