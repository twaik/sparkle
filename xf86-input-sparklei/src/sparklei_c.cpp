#include "sparklei_c.h"
#include "were/were_event_loop.h"
#include "common/sparkle_connection.h"
#include "common/sparkle_protocol.h"
#include <cstring>


/* ================================================================================================================== */

class SparkleiC
{
public:
    ~SparkleiC();
    SparkleiC(const std::string &compositor, const std::string &surfaceName);

    int fd() {return loop_->fd();}
    void process() {loop_->processEvents();}

    void (*pointer_down_callback)(void *user, int slot, int x, int y);
    void *pointer_down_user;

    void (*pointer_up_callback)(void *user, int slot);
    void *pointer_up_user;

    void (*pointer_motion_callback)(void *user, int slot, int x, int y);
    void *pointer_motion_user;

    void (*key_down_callback)(void *user, int code);
    void *key_down_user;

    void (*key_up_callback)(void *user, int code);
    void *key_up_user;

    void (*button_press_callback)(void *user, int button, int x, int y);
    void *button_press_user;

    void (*button_release_callback)(void *user, int button, int x, int y);
    void *button_release_user;

    void (*cursor_motion_callback)(void *user, int x, int y);
    void *cursor_motion_user;

private:
    void handleMessage(std::shared_ptr<WereSocketUnixMessage> message);

private:
    WereEventLoop *loop_;
    SparkleConnection *connection_;
    std::string surfaceName_;
};

SparkleiC::~SparkleiC()
{
    delete connection_;
    delete loop_;
}

SparkleiC::SparkleiC(const std::string &compositor, const std::string &surfaceName)
{
    loop_ = new WereEventLoop();
    connection_ = new SparkleConnection(loop_, compositor);
    surfaceName_ = surfaceName;

    connection_->signal_message.connect(WereSimpleQueuer(loop_, &SparkleiC::handleMessage, this));
}

/* ================================================================================================================== */

void SparkleiC::handleMessage(std::shared_ptr<WereSocketUnixMessage> message)
{
    uint32_t operation;

    WereSocketUnixMessageStream stream(message.get());
    stream >> operation;


    if (operation == PointerDownNotificationCode)
    {
        PointerDownNotification r1;
        stream >> r1;

        if (r1.surface == surfaceName_)
            pointer_down_callback(pointer_down_user, r1.slot, r1.x, r1.y);
    }
    else if (operation == PointerUpNotificationCode)
    {
        PointerUpNotification r1;
        stream >> r1;

        if (r1.surface == surfaceName_)
            pointer_up_callback(pointer_up_user, r1.slot);
    }
    else if (operation == PointerMotionNotificationCode)
    {
        PointerMotionNotification r1;
        stream >> r1;

        if (r1.surface == surfaceName_)
        {
            pointer_motion_callback(pointer_motion_user, r1.slot, r1.x, r1.y);
        }
    }
    else if (operation == KeyDownNotificationCode)
    {
        KeyDownNotification r1;
        stream >> r1;
        key_down_callback(key_down_user, r1.code);
    }
    else if (operation == KeyUpNotificationCode)
    {
        KeyUpNotification r1;
        stream >> r1;
        key_up_callback(key_up_user, r1.code);
    }
    else if (operation == ButtonPressNotificationCode)
    {
        ButtonPressNotification r1;
        stream >> r1;
        button_press_callback(button_press_user, r1.button, r1.x, r1.y);
    }
    else if (operation == ButtonReleaseNotificationCode)
    {
        ButtonReleaseNotification r1;
        stream >> r1;
        button_release_callback(button_release_user, r1.button, r1.x, r1.y);
    }
    else if (operation == CursorMotionNotificationCode)
    {
        CursorMotionNotification r1;
        stream >> r1;
        cursor_motion_callback(cursor_motion_user, r1.x, r1.y);
    }
}

/* ================================================================================================================== */

SparkleiC *sparklei_c_create(const char *compositor, const char *surface_name)
{
    return new SparkleiC(compositor, surface_name);
}

void sparklei_c_destroy(SparkleiC *c)
{
    delete c;
}

int sparklei_c_fd(SparkleiC *c)
{
    return c->fd();
}

void sparklei_c_process(SparkleiC *c)
{
    c->process();
}

void sparklei_c_set_pointer_down_cb(SparkleiC *c, void (*f)(void *user, int slot, int x, int y), void *user)
{
    c->pointer_down_callback = f;
    c->pointer_down_user = user;
}

void sparklei_c_set_pointer_up_cb(SparkleiC *c, void (*f)(void *user, int slot), void *user)
{
    c->pointer_up_callback = f;
    c->pointer_up_user = user;
}

void sparklei_c_set_pointer_motion_cb(SparkleiC *c, void (*f)(void *user, int slot, int x, int y), void *user)
{
    c->pointer_motion_callback = f;
    c->pointer_motion_user = user;
}

void sparklei_c_set_key_down_cb(SparkleiC *c, void (*f)(void *user, int code), void *user)
{
    c->key_down_callback = f;
    c->key_down_user = user;
}

void sparklei_c_set_key_up_cb(SparkleiC *c, void (*f)(void *user, int code), void *user)
{
    c->key_up_callback = f;
    c->key_up_user = user;
}

void sparklei_c_set_button_press_cb(SparkleiC *c, void (*f)(void *user, int button, int x, int y), void *user)
{
    c->button_press_callback = f;
    c->button_press_user = user;
}

void sparklei_c_set_button_release_cb(SparkleiC *c, void (*f)(void *user, int button, int x, int y), void *user)
{
    c->button_release_callback = f;
    c->button_release_user = user;
}

void sparklei_c_set_cursor_motion_cb(SparkleiC *c, void (*f)(void *user, int x, int y), void *user)
{
    c->cursor_motion_callback = f;
    c->cursor_motion_user = user;
}

/* ================================================================================================================== */
