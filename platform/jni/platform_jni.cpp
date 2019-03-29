#include "platform_jni.h"
#include "were/were_timer.h"
#include "compositor/gl/compositor_gl.h"
#include <stdexcept>
#include <android/window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <sys/types.h>
#include <sys/stat.h>

#define LOG(p, ...) __android_log_print(p, "SparkleNative", __VA_ARGS__)
#define LOGI(...) LOG(ANDROID_LOG_INFO, __VA_ARGS__)
#define LOGE(...) LOG(ANDROID_LOG_ERROR, __VA_ARGS__)

//==================================================================================================

class PointerState
{
public:
    PointerState() : down(false), x(0), y(0) {}
    bool down;
    int x;
    int y;
};

class PlatformJNI : public Platform
{
public:
    ~PlatformJNI() override;
    PlatformJNI(WereEventLoop *loop, JNIEnv* env, jobject instance);

    int start() override;
    int stop() override;

    bool _draw;

private:
    void timeout();

    JNIEnv* env = NULL;
    jobject instance = NULL;
    jclass cls = NULL;

    WereEventLoop *_loop;
    WereTimer *_timer;
    jmethodID processRunnablesID = NULL;
};

PlatformJNI::~PlatformJNI()
{
    delete _timer;
}

PlatformJNI::PlatformJNI(WereEventLoop *loop, JNIEnv* env, jobject instance):
_draw(false), env(env), instance(instance), _loop(loop)
{
	cls = env->GetObjectClass(instance);
	if (!cls) {
		LOGE("GetObjectClass returned NULL!!!"); 
		return;
	}
	processRunnablesID = env->GetMethodID(cls, "processRunnables", "()V");
	if (!processRunnablesID) {
		LOGE("GetMethodID returned NULL!!!");
		return;
	}
	
	jfieldID platformID = env->GetFieldID(cls, "platform", "J");
	if (!platformID) {
		LOGE("GetFieldID returned NULL!!!");
		return;
	}
	env->SetLongField(instance, platformID, (jlong) this);
	
    _timer = new WereTimer(_loop);
    _timer->timeout.connect(WereSimpleQueuer(loop, &PlatformJNI::timeout, this));
}

int PlatformJNI::start()
{
    _timer->start(1000/60, false);
    return 0;
}

int PlatformJNI::stop()
{
    return 0;
}

void PlatformJNI::timeout()
{
	env->CallVoidMethod(instance, processRunnablesID);
    if (_draw)
        draw();
}

//==================================================================================================

static void run(JNIEnv *env, jobject instance)
{
    LOGI("Native part of SparkleActivity started\n");

    umask(0);

    try
    {
        WereEventLoop *loop = new WereEventLoop();
        Platform *platform = new PlatformJNI(loop, env, instance);
        Compositor *compositor = compositor_gl_create(loop, platform, "/data/data/com.termux/files/usr/tmp/sparkle.socket");

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
    LOGI("Native part of SparkleActivity finished.\n");
}

static void onNativeWindowCreated(JNIEnv *env, jobject instance, jlong jplatform ,jobject jsurface) {
	auto *platform = reinterpret_cast<PlatformJNI *>(jplatform);
	if (platform == nullptr) {
		LOGE("platform is null");
		return;
	}
	
	ANativeWindow* surface = ANativeWindow_fromSurface(env, jsurface);
    platform->initializeForNativeDisplay(EGL_DEFAULT_DISPLAY);
    platform->initializeForNativeWindow(surface);
    platform->_draw = true;
	
}

static void onNativeWindowDestroyed(JNIEnv *env, jobject instance, jlong jplatform , jobject jsurface) {
	auto *platform = reinterpret_cast<PlatformJNI *>(jplatform);
	if (platform == nullptr) {
		LOGE("platform is null");
		return;
	}
	
    platform->_draw = false;
    platform->finishForNativeWindow();
    platform->finishForNativeDisplay();
}

static void onNativeWindowChanged(JNIEnv *env, jobject instance, jlong jplatform , jobject jsurface, jint format, jint width, jint height) {
    onNativeWindowDestroyed(env, instance, jplatform, jsurface);
    onNativeWindowCreated(env, instance, jplatform, jsurface);
}

static void onWindowFocusChanged(JNIEnv *env, jobject instance, jlong jplatform, jboolean hasFocus) {
	auto *platform = reinterpret_cast<PlatformJNI *>(jplatform);
	if (platform == nullptr) {
		LOGE("platform is null");
		return;
	}
	
	platform->_draw = (hasFocus == JNI_TRUE) ? true : false;
}

static void cursorMotion(JNIEnv *env, jobject instance, jlong jplatform, jint x, jint y) {
	auto *platform = reinterpret_cast<PlatformJNI *>(jplatform);
	if (platform == nullptr) {
		LOGE("platform is null");
		return;
	}
	
	LOGI("Cursor motion: %d x %d", x, y);
	platform->cursorMotion(x, y);
}

static void buttonPress(JNIEnv *env, jobject instance, jlong jplatform, jint button, jint x, jint y) {
	auto *platform = reinterpret_cast<PlatformJNI *>(jplatform);
	if (platform == nullptr) {
		LOGE("platform is null");
		return;
	}
	
	LOGI("Button press: %d x %d (%d)", x, y, button);
	platform->buttonPress(button, x, y);
}

static void buttonRelease(JNIEnv *env, jobject instance, jlong jplatform, jint button, jint x, jint y) {
	auto *platform = reinterpret_cast<PlatformJNI *>(jplatform);
	if (platform == nullptr) {
		LOGE("platform is null");
		return;
	}
	
	LOGI("Button release: %d x %d (%d)", x, y, button);
	platform->buttonRelease(button, x, y);
}

//==================================================================================================

static const char *className = "com/termux/app/SparkleActivity$SparkleThread";
static JNINativeMethod methods[] = {
  {"nativeRun", "()V", (void*)run },
  {"nativeOnSurfaceCreated", "(JLandroid/view/Surface;)V", (void*)onNativeWindowCreated },
  {"nativeOnSurfaceChanged", "(JLandroid/view/Surface;III)V", (void*)onNativeWindowChanged },
  {"nativeOnSurfaceDestroyed", "(JLandroid/view/Surface;)V", (void*)onNativeWindowDestroyed },
  {"nativeOnWindowFocusChanged", "(JZ)V", (void*)onWindowFocusChanged },
  
  {"nativeCursorMotion", "(JII)V", (void*)cursorMotion },
  {"nativeButtonPress", "(JIII)V", (void*)buttonPress },
  {"nativeButtonRelease", "(JIII)V", (void*)buttonRelease },
};

extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv *env;
    jclass cls;
    
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOGE("ERROR: GetEnv failed");
        return JNI_FALSE;
    }
    
    cls = env->FindClass(className);
    if (cls == NULL) {
        LOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    
    if (env->RegisterNatives(cls, methods, sizeof(methods) / sizeof(methods[0])) < 0) {
        LOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

	
	return JNI_VERSION_1_6;
}
