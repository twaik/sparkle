#include "sparkles_c.h"
#include <were/were_event_loop.h>
#include <were/were_timer.h>
#include "common/sparkle_connection.h"
#include "common/sparkle_protocol.h"
#include "common/sparkle_sound_buffer.h"

/* ================================================================================================================== */

class SparklesC
{
public:
    ~SparklesC();
    SparklesC(const std::string &server);

    int fd() {return loop_->fd();}
    void process() {loop_->processEvents();}

    int start();
    int stop();
    int write(const void *data, int size);
    int pointer();

private:
    void timeout();
    void handleConnection();
    void handleDisconnection();

private:
    WereEventLoop *loop_;
    WereTimer *timer_;
    timespec startTime_;
    int frames_;
    SparkleSoundBuffer *buffer_;
    SparkleConnection *connection_;
    bool playing_;
};

SparklesC::~SparklesC()
{
    connection_->send(UnregisterSoundBufferRequestCode);

    delete connection_;
    delete buffer_;
    delete timer_;
    delete loop_;
}

SparklesC::SparklesC(const std::string &server)
{
    loop_ = new WereEventLoop();

    timer_ = new WereTimer(loop_);
    timer_->timeout.connect(WereSimpleQueuer(loop_, &SparklesC::timeout, this));

    frames_ = 0;

    buffer_ = new SparkleSoundBuffer(4041, 65536 * 4, true);

    connection_ = new SparkleConnection(loop_, server);
    connection_->signal_connected.connect(WereSimpleQueuer(loop_, &SparklesC::handleConnection, this));
    connection_->signal_disconnected.connect(WereSimpleQueuer(loop_, &SparklesC::handleDisconnection, this));

    playing_ = false;
}

/* ================================================================================================================== */

int SparklesC::start()
{
    buffer_->start();

    timer_->start(100, false);

    connection_->send(SoundStartCode);

    playing_ = true;

    return 0;
}

int SparklesC::stop()
{
    playing_ = false;

    connection_->send(SoundStopCode);

    timer_->stop();

    buffer_->stop();

    return 0;
}

int SparklesC::write(const void *data, int size)
{
    int r_size;

    buffer_->write(data, size, &r_size);

    return r_size;
}

int SparklesC::pointer()
{
    loop_->processEvents(); /* Hack to make mplayer work */

    return buffer_->readPosition() / 4;
}

void SparklesC::timeout()
{
}

void SparklesC::handleConnection()
{
    connection_->send(RegisterSoundBufferRequest({buffer_->key(), buffer_->size()}));

    if (playing_)
        connection_->send(SoundStartCode);
}

void SparklesC::handleDisconnection()
{
}

/* ================================================================================================================== */

SparklesC *sparkles_c_create(const char *server)
{
    return new SparklesC(server);
}

void sparkles_c_destroy(SparklesC *c)
{
    delete c;
}

int sparkles_c_fd(SparklesC *c)
{
    return c->fd();
}

void sparkles_c_process(SparklesC *c)
{
    c->process();
}

int sparkles_c_start(SparklesC *c)
{
    return c->start();
}

int sparkles_c_stop(SparklesC *c)
{
    return c->stop();
}

int sparkles_c_write(SparklesC *c, const void *data, int size)
{
    return c->write(data, size);
}

int sparkles_c_pointer(SparklesC *c)
{
    return c->pointer();
}

/* ================================================================================================================== */
