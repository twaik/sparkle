#include "were/were_event_loop.h"
#include "platform/x11/platform_x11.h"
#include "compositor/gl/compositor_gl.h"
#include "common/were_benchmark.h"

int main(int argc, char *argv[])
{
    WereEventLoop *loop = new WereEventLoop();


    Platform *platform = platform_x11_create(loop);
    Compositor *compositor = compositor_gl_create(loop, platform);
    
    WereBenchmark *test = new WereBenchmark(loop);
    compositor->frame.connect(loop, std::bind(&WereBenchmark::event, test));

    platform->start();
    
    loop->run();
    
    platform->stop();
    
    delete test;
    delete compositor;
    delete platform;
    delete loop;

    sparkle_message("Done\n");

    return 0;
}

