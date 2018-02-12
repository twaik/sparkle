#include "were/were_event_loop.h"
#include "platform/na/platform_na.h"
#include "compositor/gl/compositor_gl.h"
#include "common/were_benchmark.h"
#include "sound/sles/sound_sles.h"
#include <sys/stat.h>
#include <stdexcept>


//#include "sound/sles/sound_sles.h"

void android_main(struct android_app *app)
{
    umask(0);

    try
    {
        WereEventLoop *loop = new WereEventLoop();
        Platform *platform = platform_na_create(loop, app);
        Compositor *compositor = compositor_gl_create(loop, platform);
        SoundSLES *sound = new SoundSLES(loop);
        
        WereBenchmark *test = new WereBenchmark(loop);
        compositor->frame.connect(loop, std::bind(&WereBenchmark::event, test));

        platform->start();
        loop->run();
        platform->stop();

        delete sound;
        delete test;
        delete compositor;
        delete platform;
        delete loop;
    }
    catch (const std::runtime_error &e)
    {
        were_error("%s\n", e.what());
    }


    sparkle_message("android_main finished.\n");
}

