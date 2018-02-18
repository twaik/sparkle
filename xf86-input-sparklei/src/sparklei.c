#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sparklei.h"

#include <X11/keysym.h>
#include <X11/extensions/XI.h>

#include <linux/version.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <xf86.h>
#include <xf86Xinput.h>
#include <exevents.h>
#include <xorgVersion.h>
#include <xkbsrv.h>

#include <X11/Xatom.h>
#include <xserver-properties.h>

#include "common/sparkle_protocol.h"

#ifndef XI_PROP_PRODUCT_ID
#define XI_PROP_PRODUCT_ID "Device Product ID"
#endif

#ifndef XI_PROP_VIRTUAL_DEVICE
#define XI_PROP_VIRTUAL_DEVICE "Virtual Device"
#endif

/* removed from server, purge when dropping support for server 1.10 */
#define XI86_SEND_DRAG_EVENTS   0x08

#define ArrayLength(a) (sizeof(a) / (sizeof((a)[0])))

#define MIN_KEYCODE 8

#define CAPSFLAG	1
#define NUMFLAG		2
#define SCROLLFLAG	4
#define MODEFLAG	8
#define COMPOSEFLAG	16

#ifndef ABS_MT_SLOT
#define ABS_MT_SLOT 0x2f
#endif

#ifndef ABS_MT_TRACKING_ID
#define ABS_MT_TRACKING_ID 0x39
#endif

#ifndef XI86_SERVER_FD
#define XI86_SERVER_FD 0x20
#endif


static int EvdevOn(DeviceIntPtr);
static void EvdevKbdCtrl(DeviceIntPtr device, KeybdCtrl *ctrl);
static int EvdevSwitchMode(ClientPtr client, DeviceIntPtr device, int mode);
static int EvdevOpenDevice(InputInfoPtr pInfo);
static void EvdevCloseDevice(InputInfoPtr pInfo);

static void EvdevInitAxesLabels(EvdevPtr pEvdev, int mode, int natoms, Atom *atoms);
static void EvdevInitButtonLabels(EvdevPtr pEvdev, int natoms, Atom *atoms);

static int EvdevSwitchMode(ClientPtr client, DeviceIntPtr device, int mode)
{
    if (mode == Relative)
        return Success;
    else
        return XI_BadMode;
}

static void
EvdevFreeMasks(EvdevPtr pEvdev)
{
    valuator_mask_free(&pEvdev->mt_mask);
}

static void
EvdevReadInput(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;
    
    were_event_loop_process_events(pEvdev->loop);

    //if (rc == -ENODEV) /* May happen after resume */
    //    xf86RemoveEnabledDevice(pInfo);
}

static void
EvdevPtrCtrlProc(DeviceIntPtr device, PtrCtrl *ctrl)
{
    /* Nothing to do, dix handles all settings */
}

static void
EvdevKbdCtrl(DeviceIntPtr device, KeybdCtrl *ctrl)
{
}

static int
EvdevAddKeyClass(DeviceIntPtr device)
{
    int rc = Success;
    XkbRMLVOSet rmlvo = {0},
                defaults;
    InputInfoPtr pInfo;

    pInfo = device->public.devicePrivate;

    XkbGetRulesDflts(&defaults);

    /* sorry, no rules change allowed for you */
    xf86ReplaceStrOption(pInfo->options, "xkb_rules", "evdev");
    rmlvo.rules = xf86SetStrOption(pInfo->options, "xkb_rules", NULL);
    rmlvo.model = xf86SetStrOption(pInfo->options, "xkb_model", defaults.model);
    rmlvo.layout = xf86SetStrOption(pInfo->options, "xkb_layout", defaults.layout);
    rmlvo.variant = xf86SetStrOption(pInfo->options, "xkb_variant", defaults.variant);
    rmlvo.options = xf86SetStrOption(pInfo->options, "xkb_options", defaults.options);

    if (!InitKeyboardDeviceStruct(device, &rmlvo, NULL, EvdevKbdCtrl))
        rc = !Success;

    XkbFreeRMLVOSet(&rmlvo, FALSE);
    XkbFreeRMLVOSet(&defaults, FALSE);

    return rc;
}

static int
EvdevAddAbsValuatorClass(DeviceIntPtr device)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;
    int resolution = 0;
    int axnum = 0;

    Atom *atoms;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    pEvdev->mt_mask = valuator_mask_new(4);

    if (!pEvdev->mt_mask) {
        xf86Msg(X_ERROR, "%s: failed to allocate MT valuator mask.\n",
                device->name);
        goto out;
    }

    atoms = malloc(4 * sizeof(Atom));

    EvdevInitAxesLabels(pEvdev, Absolute, 4, atoms);

    if (!InitValuatorClassDeviceStruct(device, 4, atoms,
                                       GetMotionHistorySize(), Absolute)) {
        xf86IDrvMsg(pInfo, X_ERROR, "failed to initialize valuator class device.\n");
        goto out;
    }

    if (!InitTouchClassDeviceStruct(device, 10, XIDirectTouch, 4)) {
        xf86Msg(X_ERROR, "%s: failed to initialize touch class device.\n",
                device->name);
        goto out;
    }

    axnum = 0;
    xf86InitValuatorAxisStruct(device, axnum,
                                   atoms[axnum],
                                   0,
                                   0, //XXX
                                   resolution, 0, resolution, Absolute);
    xf86InitValuatorDefaults(device, axnum);

    axnum = 1;
    xf86InitValuatorAxisStruct(device, axnum,
                                   atoms[axnum],
                                   0,
                                   0, //XXX
                                   resolution, 0, resolution, Absolute);
    xf86InitValuatorDefaults(device, axnum);

    axnum = 2;
    xf86InitValuatorAxisStruct(device, axnum,
                                   atoms[axnum],
                                   0,
                                   255, //XXX
                                   resolution, 0, resolution,
                                   Absolute);

    axnum = 3;
    xf86InitValuatorAxisStruct(device, axnum,
                                   atoms[axnum],
                                   0,
                                   200, //XXX
                                   resolution, 0, resolution,
                                   Absolute);

    free(atoms);


    if (!InitPtrFeedbackClassDeviceStruct(device, EvdevPtrCtrlProc)) {
        xf86IDrvMsg(pInfo, X_ERROR,
                    "failed to initialize pointer feedback class device.\n");
        goto out;
    }

    return Success;

out:
    EvdevFreeMasks(pEvdev);
    return !Success;
}

static int
EvdevAddButtonClass(DeviceIntPtr device)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;
    Atom *labels;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    labels = malloc(1 * sizeof(Atom)); // NUM_BUTTONS * ATOM
    EvdevInitButtonLabels(pEvdev, 1, labels); // NUM_BUTTONS

    if (!InitButtonClassDeviceStruct(device, 1, labels, // NUM_BUTTONS
                                     pEvdev->btnmap))
        return !Success;

    free(labels);
    return Success;
}

static void
EvdevInitButtonMapping(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;
    int i = 0;
    for (i = 0; i < ArrayLength(pEvdev->btnmap); ++i)
    {
        pEvdev->btnmap[i] = i;
    }
}

static int
EvdevInit(DeviceIntPtr device)
{
    //InputInfoPtr pInfo;
    //EvdevPtr pEvdev;

    //pInfo = device->public.devicePrivate;
    //pEvdev = pInfo->private;

	EvdevAddKeyClass(device);
	EvdevAddButtonClass(device);
    EvdevAddAbsValuatorClass(device);

    return Success;
}

static CARD32 EvdevTimeout(OsTimerPtr timer, CARD32 time, pointer arg)
{
    InputInfoPtr      pInfo    = (InputInfoPtr)arg;
    EvdevPtr          pEvdev   = pInfo->private;
    
    //pEvdev->timer = TimerSet(pEvdev->timer, 0, 1000, EvdevTimeout, pInfo);

    return 0;
}

static int
EvdevOn(DeviceIntPtr device)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;
    int rc = Success;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;
    
    
    /* after PreInit fd is still open */
    rc = EvdevOpenDevice(pInfo);
    if (rc != Success)
        return rc;

    xf86FlushInput(pInfo->fd);
    xf86AddEnabledDevice(pInfo);
    device->public.on = TRUE;
    
    //pEvdev->timer = TimerSet(pEvdev->timer, 0, 1000, EvdevTimeout, pInfo); //FIXME

    return Success;
}

static int
EvdevProc(DeviceIntPtr device, int what)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    switch (what)
    {
    case DEVICE_INIT:
	return EvdevInit(device);

    case DEVICE_ON:
        return EvdevOn(device);

    case DEVICE_OFF:
        if (pInfo->fd != -1)
        {
            //TimerCancel(pEvdev->timer);
            //TimerFree(pEvdev->timer);
            xf86RemoveEnabledDevice(pInfo);
            EvdevCloseDevice(pInfo);
        }
	    device->public.on = FALSE;
	break;

    case DEVICE_CLOSE:
	xf86IDrvMsg(pInfo, X_INFO, "Close\n");
        EvdevCloseDevice(pInfo);
        EvdevFreeMasks(pEvdev);
	break;

    default:
        return BadValue;
    }

    return Success;
}

static int
EvdevProbe(InputInfoPtr pInfo)
{
    //EvdevPtr pEvdev = pInfo->private;

    pInfo->flags |= XI86_SEND_DRAG_EVENTS;
    pInfo->type_name = XI_TOUCHSCREEN; //XXX
    //pInfo->type_name = XI_KEYBOARD;

    return Success;
}

static void SparkleiPacketHandler(void *user, sparkle_packet_t *packet)
{
    InputInfoPtr      pInfo    = (InputInfoPtr)user;
    EvdevPtr          pEvdev   = pInfo->private;
    
    sparkle_packet_stream_t *stream = sparkle_packet_stream_create(packet);
    
    uint32_t operation = sparkle_packet_stream_get_uint32(stream);
    
    if (operation == SPARKLE_SERVER_POINTER_DOWN)
    {
        const char *name = sparkle_packet_stream_get_string(stream);
        int slot = sparkle_packet_stream_get_uint32(stream);
        int x = sparkle_packet_stream_get_uint32(stream);
        int y = sparkle_packet_stream_get_uint32(stream);
        
        if (strcmp(name, pEvdev->surface_name) == 0)
        {
            valuator_mask_set(pEvdev->mt_mask, 0, x);
            valuator_mask_set(pEvdev->mt_mask, 1, y);
            valuator_mask_set(pEvdev->mt_mask, 2, 1);
            valuator_mask_set(pEvdev->mt_mask, 3, 1);
            xf86PostTouchEvent(pInfo->dev, slot + 1, 18, 0, pEvdev->mt_mask);
            pEvdev->slot_state[1] = 1;
            valuator_mask_zero(pEvdev->mt_mask);
        }
    }
    else if (operation == SPARKLE_SERVER_POINTER_UP)
    {
        const char *name = sparkle_packet_stream_get_string(stream);
        int slot = sparkle_packet_stream_get_uint32(stream);
        int x = sparkle_packet_stream_get_uint32(stream);
        int y = sparkle_packet_stream_get_uint32(stream);
        
        if (strcmp(name, pEvdev->surface_name) == 0)
        {
            xf86PostTouchEvent(pInfo->dev, slot + 1, 20, 0, pEvdev->mt_mask);
            pEvdev->slot_state[1] = 0;
            valuator_mask_zero(pEvdev->mt_mask);
        }
    }
    else if (operation == SPARKLE_SERVER_POINTER_MOTION)
    {
        const char *name = sparkle_packet_stream_get_string(stream);
        int slot = sparkle_packet_stream_get_uint32(stream);
        int x = sparkle_packet_stream_get_uint32(stream);
        int y = sparkle_packet_stream_get_uint32(stream);
        
        if (strcmp(name, pEvdev->surface_name) == 0)
        {
            valuator_mask_set(pEvdev->mt_mask, 0, x);
            valuator_mask_set(pEvdev->mt_mask, 1, y);
            if (pEvdev->slot_state[1] == 1)
            {
                xf86PostTouchEvent(pInfo->dev, slot + 1, 19, 0, pEvdev->mt_mask);
            }
            valuator_mask_zero(pEvdev->mt_mask);
        }
    }
    else if (operation == SPARKLE_SERVER_KEY_DOWN)
    {
        int code = sparkle_packet_stream_get_uint32(stream);
        xf86PostKeyboardEvent(pInfo->dev, code, 1);
    }
    else if (operation == SPARKLE_SERVER_KEY_UP)
    {
        int code = sparkle_packet_stream_get_uint32(stream);
        xf86PostKeyboardEvent(pInfo->dev, code, 0);
    }
    
    
    sparkle_packet_stream_destroy(stream);
}

static int
EvdevOpenDevice(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;

    if (!(pInfo->flags & XI86_SERVER_FD) && pInfo->fd < 0)
    {
        pEvdev->loop = were_event_loop_create();
        pInfo->fd = were_event_loop_fd(pEvdev->loop);
        
        pEvdev->sparkle = sparkle_connection_create(pEvdev->loop, pEvdev->compositor);
        sparkle_connection_add_packet_callback(pEvdev->sparkle, pEvdev->loop, SparkleiPacketHandler, pInfo);
    }

    if (pInfo->fd < 0) {
        xf86IDrvMsg(pInfo, X_ERROR, "Unable to open device\n");
        return BadValue;
    }

    return Success;
}

static void
EvdevCloseDevice(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;

    if (!(pInfo->flags & XI86_SERVER_FD) && pInfo->fd >= 0)
    {
        sparkle_connection_destroy(pEvdev->sparkle);
        were_event_loop_destroy(pEvdev->loop);
        pInfo->fd = -1;
    }
}

static void
EvdevUnInit(InputDriverPtr drv, InputInfoPtr pInfo, int flags)
{
    //EvdevPtr pEvdev = pInfo ? pInfo->private : NULL;

    xf86DeleteInput(pInfo, flags);
}

static EvdevPtr
EvdevAlloc(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = calloc(sizeof(EvdevRec), 1);

    if (!pEvdev)
        return NULL;

    return pEvdev;
}

static int
EvdevPreInit(InputDriverPtr drv, InputInfoPtr pInfo, int flags)
{
    EvdevPtr pEvdev;
    int rc = BadAlloc;

    if (!(pEvdev = EvdevAlloc(pInfo)))
        goto error;
    

    pInfo->private = pEvdev;
    pInfo->type_name = "UNKNOWN";
    pInfo->device_control = EvdevProc;
    pInfo->read_input = EvdevReadInput;
    pInfo->switch_mode = EvdevSwitchMode;
    
    pEvdev->compositor = xf86SetStrOption(pInfo->options, "Compositor", NULL);
    if (pEvdev->compositor == NULL)
        goto error;
    pEvdev->surface_name = xf86SetStrOption(pInfo->options, "SurfaceName", NULL);
    if (pEvdev->surface_name == NULL)
        goto error;

    rc = EvdevOpenDevice(pInfo);
    if (rc != Success)
        goto error;

    EvdevInitButtonMapping(pInfo);

    if (EvdevProbe(pInfo)) {
        rc = BadMatch;
        goto error;
    }
    

    return Success;

error:
    EvdevCloseDevice(pInfo);
    return rc;
}

_X_EXPORT InputDriverRec SPARKLEI = {
    1,
    "sparklei",
    NULL,
    EvdevPreInit,
    EvdevUnInit,
    NULL,
    NULL,
#ifdef XI86_DRV_CAP_SERVER_FD
    XI86_DRV_CAP_SERVER_FD
#endif
};

static void
EvdevUnplug(pointer	p)
{
}

static pointer
EvdevPlug(pointer	module,
          pointer	options,
          int		*errmaj,
          int		*errmin)
{
    xf86AddInputDriver(&SPARKLEI, module, 0);
    return module;
}

static XF86ModuleVersionInfo EvdevVersionRec =
{
    "sparklei",
    MODULEVENDORSTRING,
    MODINFOSTRING1,
    MODINFOSTRING2,
    XORG_VERSION_CURRENT,
    PACKAGE_VERSION_MAJOR, PACKAGE_VERSION_MINOR, PACKAGE_VERSION_PATCHLEVEL,
    ABI_CLASS_XINPUT,
    ABI_XINPUT_VERSION,
    MOD_CLASS_XINPUT,
    {0, 0, 0, 0}
};

_X_EXPORT XF86ModuleData sparkleiModuleData =
{
    &EvdevVersionRec,
    EvdevPlug,
    EvdevUnplug
};

static void EvdevInitAxesLabels(EvdevPtr pEvdev, int mode, int natoms, Atom *atoms)
{
    atoms[0] = XIGetKnownProperty(AXIS_LABEL_PROP_ABS_X);
    atoms[1] = XIGetKnownProperty(AXIS_LABEL_PROP_ABS_Y);
    atoms[2] = XIGetKnownProperty(AXIS_LABEL_PROP_ABS_MT_TOUCH_MAJOR);
    atoms[2] = XIGetKnownProperty(AXIS_LABEL_PROP_ABS_MT_TOUCH_MINOR);
}

static void EvdevInitButtonLabels(EvdevPtr pEvdev, int natoms, Atom *atoms)
{
    Atom atom;
    int button = 0;

    atom = XIGetKnownProperty(BTN_LABEL_PROP_BTN_UNKNOWN);

    for (button = 0; button < natoms; ++button)
    {
        atoms[button] = atom;
    }
}

