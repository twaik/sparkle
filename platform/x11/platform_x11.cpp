#include "platform_x11.h"
#include "were/were_timer.h"
#include <stdexcept>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/* ================================================================================================================== */

const int w_x = 100;
const int w_y = 100;
const int w_width = 800;
const int w_height = 600;
const char *w_title = "Sparkle";

/* ================================================================================================================== */

class PlatformX11 : public Platform
{
public:
    ~PlatformX11();
    PlatformX11(WereEventLoop *loop);
    int start();
    int stop();

private:
    void timeout();
    int openDisplay();
    int closeDisplay();
    int createWindow();
    int destroyWindow();
    int processEvents();

private:
    WereEventLoop *_loop;
    WereTimer *_timer;
    Display *_display;
    Window _window;
};

PlatformX11::~PlatformX11()
{
    delete _timer;
    destroyWindow();
    closeDisplay();
}

PlatformX11::PlatformX11(WereEventLoop *loop)
{
    _loop = loop;

    _timer = new WereTimer(_loop);
    _timer->timeout.connect(WereSimpleQueuer(loop, &PlatformX11::timeout, this));

    _display = 0;
    _window = 0;
}

int PlatformX11::start()
{
    if (openDisplay() != 0)
        throw std::runtime_error("[PlatformX11::start] Failed to open display.");

    initializeForNativeDisplay(_display);
    //FIXME BUG!

    if (createWindow() != 0)
        throw std::runtime_error("[PlatformX11::start] Failed to create window.");

    initializeForNativeWindow(_window);

    _timer->start(1000/60, false);

    return 0;
}

int PlatformX11::stop()
{
    _timer->stop();

    finishForNativeWindow();
    finishForNativeDisplay();

    return 0;
}

void PlatformX11::timeout()
{
    processEvents();

    draw();
}

int PlatformX11::openDisplay()
{
    _display = XOpenDisplay(0);

    if (_display == 0)
        return -1;

    return 0;
}

int PlatformX11::closeDisplay()
{
    if (_display != 0)
    {
        XCloseDisplay(_display);
        _display = 0;
    }

    return 0;
}

int PlatformX11::createWindow()
{
    if (_display == 0)
        return -1;

    int scrnum = DefaultScreen(_display);
    Window root = RootWindow(_display, scrnum);

    Visual *visual;
    int depth;

    if(getVID.connected())
    {
        XVisualInfo visTemplate;
        visTemplate.visualid = getVID();

        int num_visuals;
        XVisualInfo *visInfo = XGetVisualInfo(_display, VisualIDMask, &visTemplate, &num_visuals);
        if (!visInfo)
            return -1;

        visual = visInfo->visual;
        depth = visInfo->depth;

        XFree(visInfo);
    }
    else
    {
        were_debug("[%p][%s] Using default visual.\n", this, __PRETTY_FUNCTION__);
        visual = DefaultVisual(_display, scrnum);
        depth = DefaultDepth(_display, scrnum);
    }

    XSetWindowAttributes attr;
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap(_display, root, visual, AllocNone);
    attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

    unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

    _window = XCreateWindow(_display, root, 0, 0, w_width, w_height, 0, depth, InputOutput, visual, mask, &attr);

    if (!_window)
        return -1;

    XSizeHints sizehints;
    sizehints.x = w_x;
    sizehints.y = w_y;
    sizehints.width  = w_width;
    sizehints.height = w_height;
    sizehints.flags = USSize | USPosition;
    XSetNormalHints(_display, _window, &sizehints);
    XSetStandardProperties(_display, _window, w_title, w_title, None, (char **)NULL, 0, &sizehints);

    XMapWindow(_display, _window);

    return 0;
}

int PlatformX11::destroyWindow()
{
    if (_window != 0)
    {
        XDestroyWindow(_display, _window);
        _window = 0;
    }

    return 0;
}

int PlatformX11::processEvents()
{
    while (XPending(_display))
    {
        XEvent event;
        XNextEvent(_display, &event);

        switch (event.type)
        {
            case ButtonPress:
            {
                if (event.xbutton.type == ButtonPress)
                {
                    int button = event.xbutton.button;
                    if (button == 3)
                        button = 2;
                    else if (button == 2)
                        button = 3;

                    buttonPress(button, event.xbutton.x, event.xbutton.y);
                }
                break;
            }
            case ButtonRelease:
            {
                if (event.xbutton.type == ButtonRelease)
                {
                    int button = event.xbutton.button;
                    if (button == 3)
                        button = 2;
                    else if (button == 2)
                        button = 3;

                    buttonRelease(button, event.xbutton.x, event.xbutton.y);
                }
                break;
            }
            case MotionNotify:
            {
                cursorMotion(event.xbutton.x, event.xbutton.y);
                break;
            }
            case KeyPress:
            {
                keyDown(event.xkey.keycode);
                break;
            }
            case KeyRelease:
            {
                keyUp(event.xkey.keycode);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    return 0;
}

/* ================================================================================================================== */

Platform *platform_x11_create(WereEventLoop *loop)
{
    return new PlatformX11(loop);
}

/* ================================================================================================================== */
