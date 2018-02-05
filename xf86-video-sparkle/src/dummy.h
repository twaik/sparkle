
#define SPARKLE_MODE

/* All drivers should typically include these */
#include "xf86.h"
#include "xf86_OSproc.h"

#include "xf86Cursor.h"

#ifdef XvExtension
#include "xf86xv.h"
#include <X11/extensions/Xv.h>
#endif
#include <string.h>

#include "compat-api.h"

#ifdef SPARKLE_MODE
#include "common/sparkle_surface_file.h"
#include "were/were_event_loop.h"
#include "common/sparkle_client.h"
#endif

/* Supported chipsets */
typedef enum {
    DUMMY_CHIP
} DUMMYType;

/* function prototypes */

extern Bool DUMMYSwitchMode(SWITCH_MODE_ARGS_DECL);
extern void DUMMYAdjustFrame(ADJUST_FRAME_ARGS_DECL);

/* in dummy_cursor.c */
extern Bool DUMMYCursorInit(ScreenPtr pScrn);
extern void DUMMYShowCursor(ScrnInfoPtr pScrn);
extern void DUMMYHideCursor(ScrnInfoPtr pScrn);

/* globals */
typedef struct _color
{
    int red;
    int green;
    int blue;
} dummy_colors;

typedef struct dummyRec 
{
#ifndef SPARKLE_MODE
    /* options */
    OptionInfoPtr Options;
#endif
    Bool swCursor;
    /* proc pointer */
    CloseScreenProcPtr CloseScreen;
    xf86CursorInfoPtr CursorInfo;

    Bool DummyHWCursorShown;
    int cursorX, cursorY;
    int cursorFG, cursorBG;

    dummy_colors colors[1024];
#ifndef SPARKLE_MODE
    Bool        (*CreateWindow)() ;     /* wrapped CreateWindow */
#endif
    Bool prop;
#ifdef SPARKLE_MODE
    sparkle_surface_file_t *surface;
    were_event_loop_t *were;
    sparkle_client_t *client;

    CreateScreenResourcesProcPtr CreateScreenResources;
    DamagePtr damage;
    ScreenBlockHandlerProcPtr BlockHandler;

    DisplayModePtr modes;
    int desiredWidth;
    int desiredHeight;
    
    OsTimerPtr timer;
    
    char *compositor;
    char *surface_name;
    char *surface_file;
#endif
} DUMMYRec, *DUMMYPtr;

/* The privates of the DUMMY driver */
#define DUMMYPTR(p)	((DUMMYPtr)((p)->driverPrivate))

