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




static int EvdevOn(DeviceIntPtr);
static void EvdevKbdCtrl(DeviceIntPtr device, KeybdCtrl *ctrl);
static int EvdevSwitchMode(ClientPtr client, DeviceIntPtr device, int mode);
static int EvdevOpenDevice(InputInfoPtr pInfo);
static void EvdevCloseDevice(InputInfoPtr pInfo);

static void EvdevInitAxesLabels(EvdevPtr pEvdev, int mode, int natoms, Atom *atoms);
static void EvdevInitButtonLabels(EvdevPtr pEvdev, int natoms, Atom *atoms);

static void handle_event(int fd, int ready, void *data)
{
    InputInfoPtr pInfo = (InputInfoPtr)data;
    EvdevPtr pEvdev = pInfo->private;

    if (ready)
        sparklei_c_process(pEvdev->sparkle);
}

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

    pEvdev->mt_mask = valuator_mask_new(2);

    if (!pEvdev->mt_mask) {
        xf86Msg(X_ERROR, "%s: failed to allocate MT valuator mask.\n",
                device->name);
        goto out;
    }

    atoms = malloc(2 * sizeof(Atom));

    EvdevInitAxesLabels(pEvdev, Absolute, 2, atoms);

    if (!InitValuatorClassDeviceStruct(device, 2, atoms,
                                       GetMotionHistorySize(), Absolute)) {
        xf86IDrvMsg(pInfo, X_ERROR, "failed to initialize valuator class device.\n");
        goto out;
    }

/* FIXME */
#if 1
    if (!InitTouchClassDeviceStruct(device, 10, XIDirectTouch, 2)) {
        xf86Msg(X_ERROR, "%s: failed to initialize touch class device.\n",
                device->name);
        goto out;
    }
#endif

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
    int n_buttons = 3;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    labels = malloc(n_buttons * sizeof(Atom)); // NUM_BUTTONS * ATOM
    EvdevInitButtonLabels(pEvdev, n_buttons, labels); // NUM_BUTTONS

    if (!InitButtonClassDeviceStruct(device, n_buttons, labels, // NUM_BUTTONS
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

static int
EvdevOn(DeviceIntPtr device)
{
    InputInfoPtr pInfo;
    //EvdevPtr pEvdev;
    int rc = Success;

    pInfo = device->public.devicePrivate;
    //pEvdev = pInfo->private;

    rc = EvdevOpenDevice(pInfo);
    if (rc != Success)
        return rc;

    xf86AddEnabledDevice(pInfo);
    device->public.on = TRUE;

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
            xf86RemoveEnabledDevice(pInfo);
            EvdevCloseDevice(pInfo);
            device->public.on = FALSE;
            break;

        case DEVICE_CLOSE:
            xf86IDrvMsg(pInfo, X_INFO, "Close.\n");
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

    //pInfo->flags |= XI86_SEND_DRAG_EVENTS;
    pInfo->type_name = "Sparkle"; //XXX

    return Success;
}

static void SparkleiPointerDown(void *user, int slot, int x, int y)
{
    InputInfoPtr      pInfo    = (InputInfoPtr)user;
    EvdevPtr          pEvdev   = pInfo->private;

    valuator_mask_set(pEvdev->mt_mask, 0, x);
    valuator_mask_set(pEvdev->mt_mask, 1, y);
    xf86PostTouchEvent(pInfo->dev, slot + 1, 18, 0, pEvdev->mt_mask);
    pEvdev->slot_state[slot] = 1;
    valuator_mask_zero(pEvdev->mt_mask);
}

static void SparkleiPointerUp(void *user, int slot)
{
    InputInfoPtr      pInfo    = (InputInfoPtr)user;
    EvdevPtr          pEvdev   = pInfo->private;

    xf86PostTouchEvent(pInfo->dev, slot + 1, 20, 0, pEvdev->mt_mask);
    pEvdev->slot_state[slot] = 0;
    valuator_mask_zero(pEvdev->mt_mask);
}

static void SparkleiPointerMotion(void *user, int slot, int x, int y)
{
    InputInfoPtr      pInfo    = (InputInfoPtr)user;
    EvdevPtr          pEvdev   = pInfo->private;

    valuator_mask_set(pEvdev->mt_mask, 0, x);
    valuator_mask_set(pEvdev->mt_mask, 1, y);
    if (pEvdev->slot_state[slot] == 1)
        xf86PostTouchEvent(pInfo->dev, slot + 1, 19, 0, pEvdev->mt_mask);
    valuator_mask_zero(pEvdev->mt_mask);
}

static void SparkleiRMB(void *user, int state)
{
    InputInfoPtr      pInfo    = (InputInfoPtr)user;
    EvdevPtr          pEvdev   = pInfo->private;

    //xf86PostMotionEventM(pInfo->dev, Absolute, pEvdev->mt_mask);

    if (state == 1 && pEvdev->rmb_state == 0)
    {
        pEvdev->rmb_state = 1;
        xf86PostButtonEvent(pInfo->dev, Absolute, 3, 1, 0, 0);
    }
    else if (state == 0 && pEvdev->rmb_state == 1)
    {
        pEvdev->rmb_state = 0;
        xf86PostButtonEvent(pInfo->dev, Absolute, 3, 0, 0, 0);
    }
}

static void SparkleiKeyDown(void *user, int code)
{
    InputInfoPtr      pInfo    = (InputInfoPtr)user;

    if (code == 122)
        SparkleiRMB(user, 1);
    else
        xf86PostKeyboardEvent(pInfo->dev, code, 1);
}

static void SparkleiKeyUp(void *user, int code)
{
    InputInfoPtr      pInfo    = (InputInfoPtr)user;

    if (code == 122)
        SparkleiRMB(user, 0);
    else
        xf86PostKeyboardEvent(pInfo->dev, code, 0);
}

static void SparkleiButtonPress(void *user, int button, int x, int y)
{
    InputInfoPtr      pInfo    = (InputInfoPtr)user;
    //EvdevPtr          pEvdev   = pInfo->private;

    if (button == 3)
        button = 2;
    else if (button == 2)
        button = 3;

    //valuator_mask_set(pEvdev->mt_mask, 0, x);
    //valuator_mask_set(pEvdev->mt_mask, 1, y);
    xf86PostButtonEvent(pInfo->dev, Absolute, button, 1, 0, 0);
    //valuator_mask_zero(pEvdev->mt_mask);
}

static void SparkleiButtonRelease(void *user, int button, int x, int y)
{
    InputInfoPtr      pInfo    = (InputInfoPtr)user;
    //EvdevPtr          pEvdev   = pInfo->private;

    if (button == 3)
        button = 2;
    else if (button == 2)
        button = 3;

    //valuator_mask_set(pEvdev->mt_mask, 0, x);
    //valuator_mask_set(pEvdev->mt_mask, 1, y);
    xf86PostButtonEvent(pInfo->dev, Absolute, button, 0, 0, 0);
    //valuator_mask_zero(pEvdev->mt_mask);
}

static void SparkleiCursorMotion(void *user, int x, int y)
{
    InputInfoPtr      pInfo    = (InputInfoPtr)user;
    EvdevPtr          pEvdev   = pInfo->private;

    valuator_mask_set(pEvdev->mt_mask, 0, x);
    valuator_mask_set(pEvdev->mt_mask, 1, y);
    xf86PostMotionEventM(pInfo->dev, Absolute, pEvdev->mt_mask);
    valuator_mask_zero(pEvdev->mt_mask);
}

static int
EvdevOpenDevice(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;

    if (pEvdev->sparkle == NULL)
    {
        pEvdev->sparkle = sparklei_c_create(pEvdev->compositor, pEvdev->surface_name);
        SetNotifyFd(sparklei_c_fd(pEvdev->sparkle), handle_event, X_NOTIFY_READ, pInfo);

        sparklei_c_set_pointer_down_cb(pEvdev->sparkle, SparkleiPointerDown, pInfo);
        sparklei_c_set_pointer_up_cb(pEvdev->sparkle, SparkleiPointerUp, pInfo);
        sparklei_c_set_pointer_motion_cb(pEvdev->sparkle, SparkleiPointerMotion, pInfo);
        sparklei_c_set_key_down_cb(pEvdev->sparkle, SparkleiKeyDown, pInfo);
        sparklei_c_set_key_up_cb(pEvdev->sparkle, SparkleiKeyUp, pInfo);
        sparklei_c_set_button_press_cb(pEvdev->sparkle, SparkleiButtonPress, pInfo);
        sparklei_c_set_button_release_cb(pEvdev->sparkle, SparkleiButtonRelease, pInfo);
        sparklei_c_set_cursor_motion_cb(pEvdev->sparkle, SparkleiCursorMotion, pInfo);
    }

    return Success;
}

static void EvdevCloseDevice(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;

    if (pEvdev->sparkle != NULL)
    {
        RemoveNotifyFd(sparklei_c_fd(pEvdev->sparkle));
        sparklei_c_destroy(pEvdev->sparkle);
        pEvdev->sparkle = NULL;
    }
}

static void EvdevUnInit(InputDriverPtr drv, InputInfoPtr pInfo, int flags)
{
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
}

static void EvdevInitButtonLabels(EvdevPtr pEvdev, int natoms, Atom *atoms)
{
#if 1
    Atom atom;
    int button = 0;

    atom = XIGetKnownProperty(BTN_LABEL_PROP_BTN_UNKNOWN);

    for (button = 0; button < natoms; ++button)
    {
        atoms[button] = atom;
    }
#else
    atoms[0] = XIGetKnownProperty(BTN_LABEL_PROP_BTN_0);
    atoms[1] = XIGetKnownProperty(BTN_LABEL_PROP_BTN_1);
    atoms[2] = XIGetKnownProperty(BTN_LABEL_PROP_BTN_2);
#endif
}

