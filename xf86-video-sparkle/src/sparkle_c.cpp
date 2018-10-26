#include "sparkle_c.h"
#include "were/were_event_loop.h"
#include "common/sparkle_connection.h"
#include "common/sparkle_surface_fd.h"
#include "common/sparkle_protocol.h"
#include <unistd.h>
#include <cstring>


/* ================================================================================================================== */

class SparkleC
{
public:
    ~SparkleC();
    SparkleC(const std::string &compositor, const std::string &surfaceName, const std::string &surfaceFile);

    int fd() {return loop_->fd();}
    void process() {loop_->processEvents();}

    void registerSurface();
    void unregisterSurface();

    void resizeSurface(int width, int height);

    void *surfaceData() {return surface_->data();}
    void damage(int x1, int y1, int x2, int y2);

    void (*display_size_callback)(void *user, int width, int height);
    void *display_size_user;

private:
    void handleConnection();
    void handleDisconnection();
    void handleMessage(std::shared_ptr<WereSocketUnixMessage> message);

private:
    WereEventLoop *loop_;
    SparkleConnection *connection_;
    SparkleSurfaceFd *surface_;
    std::string surfaceName_;
    std::string surfaceFile_;
    bool registered_;
};

SparkleC::~SparkleC()
{
    unregisterSurface();

    delete surface_;
    delete connection_;
    delete loop_;
}

SparkleC::SparkleC(const std::string &compositor, const std::string &surfaceName, const std::string &surfaceFile)
{
    loop_ = new WereEventLoop();
    connection_ = new SparkleConnection(loop_, compositor);
    surface_ = new SparkleSurfaceFd(surfaceFile, 800, 600);
    surfaceName_ = surfaceName;
    surfaceFile_ = surfaceFile;

    connection_->signal_connected.connect(WereSimpleQueuer(loop_, &SparkleC::handleConnection, this));
    connection_->signal_disconnected.connect(WereSimpleQueuer(loop_, &SparkleC::handleDisconnection, this));
    connection_->signal_message.connect(WereSimpleQueuer(loop_, &SparkleC::handleMessage, this));
}

/* ================================================================================================================== */

void SparkleC::registerSurface()
{
    connection_->send(RegisterSurfaceFdRequest({surfaceName_, surface_->fd(), surface_->width(), surface_->height()}));
    connection_->send(SetSurfacePositionRequest({surfaceName_, 0, 0, surface_->width(), surface_->height()}));
    registered_ = true;
}

void SparkleC::unregisterSurface()
{
    connection_->send(UnregisterSurfaceRequest({surfaceName_}));
    usleep(100000);
    registered_ = false;
}

void SparkleC::resizeSurface(int width, int height)
{
    if (registered_)
    {
        unregisterSurface();
        delete surface_;
        surface_ = new SparkleSurfaceFd(surfaceFile_, width, height);
        registerSurface();
    }
    else
    {
        delete surface_;
        surface_ = new SparkleSurfaceFd(surfaceFile_, width, height);
    }
}

void SparkleC::handleConnection()
{
    registerSurface();
}

void SparkleC::handleDisconnection()
{
    registered_ = false;
}

void SparkleC::handleMessage(std::shared_ptr<WereSocketUnixMessage> message)
{
    uint32_t operation;

    WereSocketUnixMessageStream stream(message.get());
    stream >> operation;

    if (operation == DisplaySizeNotificationCode)
    {
        DisplaySizeNotification r1;
        stream >> r1;

        display_size_callback(display_size_user, r1.width, r1.height);
    }
}

void SparkleC::damage(int x1, int y1, int x2, int y2)
{
    connection_->send(AddSurfaceDamageRequest({surfaceName_, x1, y1, x2, y2}));
}

/* ================================================================================================================== */

SparkleC *sparkle_c_create(const char *compositor, const char *surface_name, const char *surface_file)
{
    return new SparkleC(compositor, surface_name, surface_file);
}

void sparkle_c_destroy(SparkleC *c)
{
    delete c;
}

int sparkle_c_fd(SparkleC *c)
{
    return c->fd();
}

void sparkle_c_process(SparkleC *c)
{
    c->process();
}

void *sparkle_c_surface_data(SparkleC *c)
{
    return c->surfaceData();
}

void sparkle_c_damage(SparkleC *c, int x1, int y1, int x2, int y2)
{
    c->damage(x1, y1, x2, y2);
}

void sparkle_c_resize_surface(SparkleC *c, int width, int height)
{
    c->resizeSurface(width, height);
}

void sparkle_c_set_display_size_cb(SparkleC *c, void (*f)(void *user, int width, int height), void *user)
{
    c->display_size_callback = f;
    c->display_size_user = user;
}

/* ================================================================================================================== */
