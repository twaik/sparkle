#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef EVDEV_H
#define EVDEV_H

#include <linux/types.h>

#include <xorg-server.h>
#include <xf86Xinput.h>
#include <xf86_OSproc.h>
#include <xkbstr.h>
#include "were/were_event_loop.h"
#include "common/sparkle_client.h"



#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) < 18
#define LogMessageVerbSigSafe xf86MsgVerb
#endif

#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 23
#define HAVE_THREADED_INPUT	1
#endif

#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 24
#define BLOCK_HANDLER_ARGS     	void *data, void *waitTime
#define WAKEUP_HANDLER_ARGS	void *data, int i
#else
#define BLOCK_HANDLER_ARGS	pointer data, struct timeval **waitTime, pointer LastSelectMask
#define WAKEUP_HANDLER_ARGS	void *data, int i, pointer LastSelectMask
#endif


#define EVDEV_MAXBUTTONS 32
#define EVDEV_MAXQUEUE 32



#ifndef MAX_VALUATORS
#define MAX_VALUATORS 36
#endif

#ifndef XI_PROP_DEVICE_NODE
#define XI_PROP_DEVICE_NODE "Device Node"
#endif

#define LONG_BITS (sizeof(long) * 8)

/* Number of longs needed to hold the given number of bits */
#define NLONGS(x) (((x) + LONG_BITS - 1) / LONG_BITS)



/* Function key mode */
enum fkeymode {
    FKEYMODE_UNKNOWN = 0,
    FKEYMODE_FKEYS,       /* function keys send function keys */
    FKEYMODE_MMKEYS,      /* function keys send multimedia keys */
};


enum ButtonAction {
    BUTTON_RELEASE = 0,
    BUTTON_PRESS = 1
};


typedef struct {
    ValuatorMask *mt_mask;
    unsigned char btnmap[32];
    
    OsTimerPtr timer;
    were_event_loop_t *loop;
    sparkle_client_t *client;
    
    int slot_state[10];
    
    char *compositor;
    char *surface_name;

} EvdevRec, *EvdevPtr;


#endif

