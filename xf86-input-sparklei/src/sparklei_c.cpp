#include "sparklei_c.h"
#include "were/were_event_loop.h"
#include "common/sparkle_connection.h"
#include <cstring>


/* ================================================================================================================== */

const char *compositor = "/dev/shm/sparkle.socket";
const char *surface_name = "sparkle";
const char *surface_file = "/dev/shm/xorg";

/* ================================================================================================================== */

class SparkleiC
{
public:
    ~SparkleiC();
    SparkleiC();

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

private:
    void handleMessage(std::shared_ptr<WereSocketUnixMessage> message);

private:
    WereEventLoop *loop_;
    SparkleConnection *connection_;
};

SparkleiC::~SparkleiC()
{
    delete connection_;
    delete loop_;
}

SparkleiC::SparkleiC()
{
    loop_ = new WereEventLoop();
    connection_ = new SparkleConnection(loop_, compositor);

    connection_->signal_packet.connect(WereSimpleQueuer(loop_, &SparkleiC::handleMessage, this));
}

/* ================================================================================================================== */

void SparkleiC::handleMessage(std::shared_ptr<WereSocketUnixMessage> message)
{
    uint32_t operation;

    WereStream stream(message->data());
    stream.pRead(&p_uint32, &operation);

    if (operation == pointer_down_notification.code)
    {
        struct pointer_down_notification_ r1;
        stream.pRead(&pointer_down_notification.packer, &r1);

        if (strcmp(r1.surface, surface_name) == 0)
            pointer_down_callback(pointer_down_user, r1.slot, r1.x, r1.y);
    }
    else if (operation == pointer_up_notification.code)
    {
        struct pointer_up_notification_ r1;
        stream.pRead(&pointer_up_notification.packer, &r1);

        if (strcmp(r1.surface, surface_name) == 0)
            pointer_up_callback(pointer_up_user, r1.slot);
    }
    else if (operation == pointer_motion_notification.code)
    {
        struct pointer_motion_notification_ r1;
        stream.pRead(&pointer_motion_notification.packer, &r1);

        if (strcmp(r1.surface, surface_name) == 0)
            pointer_motion_callback(pointer_motion_user, r1.slot, r1.x, r1.y);
    }
    else if (operation == key_down_notification.code)
    {
        struct key_down_notification_ r1;
        stream.pRead(&key_down_notification.packer, &r1);
        key_down_callback(key_down_user, r1.code);
    }
    else if (operation == key_up_notification.code)
    {
        struct key_up_notification_ r1;
        stream.pRead(&key_up_notification.packer, &r1);
        key_up_callback(key_up_user, r1.code);
    }
}

/* ================================================================================================================== */

SparkleiC *sparklei_c_create(void)
{
    return new SparkleiC();
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

/* ================================================================================================================== */
