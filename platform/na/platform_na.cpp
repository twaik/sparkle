#include "platform_na.h"
#include "were/were_timer.h"
#include "sion_keymap.h"
#include <stdexcept>
#include <android/window.h>

//==================================================================================================

class PointerState
{
public:
    PointerState() : down(false), x(0), y(0) {}
    bool down;
    int x;
    int y;
};

class PlatformNA : public Platform
{
public:
    ~PlatformNA();
    PlatformNA(WereEventLoop *loop, struct android_app *app);
    
    int start();
    int stop();

private:
    void timeout();
    static int handleInput(struct android_app *app, AInputEvent *event);
    static void handleCmd(struct android_app *app, int32_t cmd);

private:
    WereEventLoop *_loop;
    struct android_app *_app;
    
    WereTimer *_timer;
    
    bool _draw;

    PointerState _pointer[10];
};

PlatformNA::~PlatformNA()
{
    delete _timer;
}

PlatformNA::PlatformNA(WereEventLoop *loop, struct android_app *app)
{
    _loop = loop;
    _app = app;

    _app->userData = this;
    _app->onInputEvent = &handleInput;
    _app->onAppCmd = &handleCmd;

    _draw = false;
    
    _timer = new WereTimer(_loop);
    _timer->timeout.connect(WereSimpleQueuer(loop, &PlatformNA::timeout, this));
}

int PlatformNA::start()
{
    ANativeActivity_setWindowFlags(_app->activity, AWINDOW_FLAG_FULLSCREEN, AWINDOW_FLAG_FORCE_NOT_FULLSCREEN);
    
    _timer->start(1000/60, false);
    
    return 0;
}

int PlatformNA::stop()
{
    return 0;
}

void PlatformNA::timeout()
{
    int ident;
    int events;
    struct android_poll_source *source;

    // -1 to wait forever
    while ((ident = ALooper_pollAll(0, NULL, &events, (void **)&source)) >= 0)
    {
        // Process this event.
        if (source != NULL)
            source->process(_app, source);

        // Check if we are exiting.
        if (_app->destroyRequested != 0)
        {
            _loop->exit();
            return;
        }
    }

    if (_draw)
        draw();
}

int PlatformNA::handleInput(struct android_app *app, AInputEvent *event)
{
    PlatformNA *platform = reinterpret_cast<PlatformNA *>(app->userData);

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
    {
        int action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;

        if (action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_POINTER_DOWN)
        {
            int pointerIndex = (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            int x = AMotionEvent_getX(event, pointerIndex);
            int y = AMotionEvent_getY(event, pointerIndex);
            int slot = AMotionEvent_getPointerId(event, pointerIndex);
            platform->_pointer[slot].down = true;
            platform->_pointer[slot].x = x;
            platform->_pointer[slot].y = y;
            platform->pointerDown(slot, x, y);
            //were_message("DOWN %d %d %d\n", slot, x, y);
        }
        else if (action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_POINTER_UP)
        {
            int pointerIndex = (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            int x = AMotionEvent_getX(event, pointerIndex);
            int y = AMotionEvent_getY(event, pointerIndex);
            int slot = AMotionEvent_getPointerId(event, pointerIndex);
            platform->_pointer[slot].down = false;
            platform->_pointer[slot].x = x;
            platform->_pointer[slot].y = y;
            platform->pointerUp(slot, x, y);
            //were_message("UP %d %d %d\n", slot, x, y);
        }
        else if (action == AMOTION_EVENT_ACTION_MOVE)
        {
            int pointerCount = AMotionEvent_getPointerCount(event);
            for(int i = 0; i < pointerCount; ++i)
            {
                int x = AMotionEvent_getX(event, i);
                int y = AMotionEvent_getY(event, i);
                int slot = AMotionEvent_getPointerId(event, i);

                if (x != platform->_pointer[slot].x || y != platform->_pointer[slot].y)
                {
                    platform->_pointer[slot].x = x;
                    platform->_pointer[slot].y = y;
                    platform->pointerMotion(slot, x, y);
                    //were_message("MOVE %d %d %d\n", slot, x, y);
                }
            }
        }

        return 1;
    }
    else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
    {
        int action = AKeyEvent_getAction(event);
        int code = AKeyEvent_getKeyCode(event);
        //int code = KeyEvent_getScanCode(event);

        //were_message("KEY %d\n", code);
        
        code = sion_keymap[code];

        if (code == 0)
            return 0;

        if (action == AKEY_EVENT_ACTION_DOWN)
        {
            platform->keyDown(code);
        }
        else if (action == AKEY_EVENT_ACTION_UP)
        {
            platform->keyUp(code);
        }

        return 1;
    }

    return 0;
}

void PlatformNA::handleCmd(struct android_app *app, int32_t cmd)
{
    PlatformNA *platform = reinterpret_cast<PlatformNA *>(app->userData);

    switch (cmd)
    {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            //engine->app->savedState = malloc(sizeof(struct saved_state));
            //*((struct saved_state*)engine->app->savedState) = engine->state;
            //engine->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            //if (engine->app->window != NULL) {
            //    engine_init_display(engine);
            //    engine_draw_frame(engine);
            //}
            if (app->window != NULL)
            {
                platform->initializeForNativeDisplay(EGL_DEFAULT_DISPLAY);
                platform->initializeForNativeWindow(app->window);
                platform->_draw = true;
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            //engine_term_display(engine);
            platform->_draw = false;
            platform->finishForNativeWindow();
            platform->finishForNativeDisplay();
            break;
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            //if (engine->accelerometerSensor != NULL) {
            //    ASensorEventQueue_enableSensor(engine->sensorEventQueue,
            //                                   engine->accelerometerSensor);
            //    // We'd like to get 60 events per second (in us).
            //    ASensorEventQueue_setEventRate(engine->sensorEventQueue,
            //                                   engine->accelerometerSensor,
            //                                   (1000L/60)*1000);
            //}
            break;
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            //if (engine->accelerometerSensor != NULL) {
            //    ASensorEventQueue_disableSensor(engine->sensorEventQueue,
            //                                    engine->accelerometerSensor);
            //}
            // Also stop animating.
            //engine->animating = 0;
            //engine_draw_frame(engine);
            break;
    }
}

#if 0
void PlatformNA::setImmersive()
{
/*
@Override
public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
    if (hasFocus) {
        decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }
}
*/

    JNIEnv *env = 0;

	_app->activity->vm->AttachCurrentThread(&env, NULL); //XXX Check attached

	jclass activityClass = env->FindClass("android/app/NativeActivity");
	jmethodID getWindow = env->GetMethodID(activityClass, "getWindow", "()Landroid/view/Window;");

	jclass windowClass = env->FindClass("android/view/Window");
	jmethodID getDecorView = env->GetMethodID(windowClass, "getDecorView", "()Landroid/view/View;");

	jclass viewClass = env->FindClass("android/view/View");
	jmethodID setSystemUiVisibility = env->GetMethodID(viewClass, "setSystemUiVisibility", "(I)V");

	jobject window = env->CallObjectMethod(_app->activity->clazz, getWindow);
	jobject decorView = env->CallObjectMethod(window, getDecorView);

    jfieldID flag1ID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_LAYOUT_STABLE", "I");
    jfieldID flag2ID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION", "I");
    jfieldID flag3ID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN", "I");
    jfieldID flag4ID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I");
    jfieldID flag5ID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_FULLSCREEN", "I");
    jfieldID flag6ID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I");

    int flag1 = env->GetStaticIntField(viewClass, flag1ID);
    int flag2 = env->GetStaticIntField(viewClass, flag2ID);
    int flag3 = env->GetStaticIntField(viewClass, flag3ID);
    int flag4 = env->GetStaticIntField(viewClass, flag4ID);
    int flag5 = env->GetStaticIntField(viewClass, flag5ID);
    int flag6 = env->GetStaticIntField(viewClass, flag6ID);
    int flag = flag1 | flag2 | flag3 | flag4 | flag5 | flag6;

	env->CallVoidMethod(decorView, setSystemUiVisibility, flag);

	_app->activity->vm->DetachCurrentThread();
}
#endif

//==================================================================================================

Platform *platform_na_create(WereEventLoop *loop, struct android_app *app)
{
    return new PlatformNA(loop, app);
}

//==================================================================================================

