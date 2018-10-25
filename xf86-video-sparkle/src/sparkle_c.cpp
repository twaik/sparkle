#include "sparkle_c.h"
#include "were/were_event_loop.h"
#include "common/sparkle_connection.h"
#include "common/sparkle_surface_fd.h"
#include <unistd.h>


/* ================================================================================================================== */

const char *compositor = "/dev/shm/sparkle.socket";
const char *surface_name = "sparkle";
const char *surface_file = "/dev/shm/xorg";

/* ================================================================================================================== */

class SparkleC
{
public:
    ~SparkleC();
    SparkleC();

    int fd() {return loop_->fd();}
    void process() {loop_->processEvents();}

    void registerSurface();
    void unregisterSurface();

    void *surfaceData() {return surface_->data();}
    void damage(int x1, int y1, int x2, int y2);

private:
    void handleConnection();
    void handleDisconnection();
    void handleMessage(std::shared_ptr<WereSocketUnixMessage> message);

private:
    WereEventLoop *loop_;
    SparkleConnection *connection_;
    SparkleSurfaceFd *surface_;
};

SparkleC::~SparkleC()
{
    delete surface_;
    delete connection_;
    delete loop_;
}

SparkleC::SparkleC()
{
    loop_ = new WereEventLoop();
    connection_ = new SparkleConnection(loop_, compositor);
    surface_ = new SparkleSurfaceFd(surface_file, 800, 600);

    connection_->signal_connected.connect(WereSimpleQueuer(loop_, &SparkleC::handleConnection, this));
    connection_->signal_disconnected.connect(WereSimpleQueuer(loop_, &SparkleC::handleDisconnection, this));
    connection_->signal_packet.connect(WereSimpleQueuer(loop_, &SparkleC::handleMessage, this));
}

/* ================================================================================================================== */

void SparkleC::registerSurface()
{
    struct register_surface_file_request_ r2 = {surface_name, surface_file, surface_->width(), surface_->height()};
    connection_->send1(&register_surface_file_request, &r2);

    struct set_surface_position_request_ r3 = {surface_name, 0, 0, surface_->width(), surface_->height()};
    connection_->send1(&set_surface_position_request, &r3);
}

void SparkleC::unregisterSurface()
{
    struct unregister_surface_request_ r1 = {surface_name};
    connection_->send1(&unregister_surface_request, &r1);
    usleep(100000);
}

void SparkleC::handleConnection()
{
    registerSurface();
}

void SparkleC::handleDisconnection()
{
}

void SparkleC::handleMessage(std::shared_ptr<WereSocketUnixMessage> message)
{
}

void SparkleC::damage(int x1, int y1, int x2, int y2)
{
    struct add_surface_damage_request_ r1 = {surface_name, x1, y1, x2, y2};
    connection_->send1(&add_surface_damage_request, &r1);
}

/* ================================================================================================================== */

SparkleC *sparkle_c_create()
{
    return new SparkleC();
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

/* ================================================================================================================== */
