#include "were/were_event_loop.h"
#include "were/were_signal_handler.h"
#include "platform/x11/platform_x11.h"
#include "compositor/gl/compositor_gl.h"
#include "common/were_benchmark.h"

int main(int argc, char *argv[])
{
    WereEventLoop *loop = new WereEventLoop();
    WereSignalHandler *sig = new WereSignalHandler(loop);
    sig->terminate.connect(WereSimpleQueuer(loop, &WereEventLoop::exit, loop));

    Platform *platform = platform_x11_create(loop);
    Compositor *compositor = compositor_gl_create(loop, platform, "/tmp/sparkle.socket");

    WereBenchmark *test = new WereBenchmark(loop);
    compositor->frame.connect(WereSimpleQueuer(loop, &WereBenchmark::event, test));

    platform->start();

    loop->run();

    platform->stop();

    delete test;
    delete compositor;
    delete platform;
    delete sig;
    delete loop;

    were_message("Done.\n");

    return 0;
}

