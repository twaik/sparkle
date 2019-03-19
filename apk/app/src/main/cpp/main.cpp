#include "were/were_event_loop.h"
#include "platform/na/platform_na.h"
#include "compositor/gl/compositor_gl.h"
#include "common/were_benchmark.h"
#include "sound/sles/sound_sles.h"
#include <sys/stat.h>
#include <stdexcept>

void android_main(struct android_app *app)
{
    were_message("android_main started.\n");

    umask(0);

    std::string internalDataPath(app->activity->internalDataPath);

    try
    {
        WereEventLoop *loop = new WereEventLoop();
        Platform *platform = platform_na_create(loop, app);
        Compositor *compositor = compositor_gl_create(loop, platform, internalDataPath + "/usr/tmp/sparkle.socket");

        platform->start();
        loop->run();
        platform->stop();

        delete compositor;
        delete platform;
        delete loop;
    }
    catch (const std::exception &e)
    {
        were_error("%s\n", e.what());
    }

    ANativeActivity_finish(app->activity);

    were_message("android_main finished.\n");
}

