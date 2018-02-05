#include "were/were_event_loop.h"
#include "platform/na/platform_na.h"
#include "compositor/gl/compositor_gl.h"
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

        platform->start();
        loop->run();
        platform->stop();

        delete compositor;
        delete platform;
        delete loop;
    }
    catch (const std::runtime_error &e)
    {
        sparkle_message("%s\n", e.what());
    }


    sparkle_message("android_main finished.\n");
}

