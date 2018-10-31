#ifndef SOUND_SLES_H
#define SOUND_SLES_H

#include <SLES/OpenSLES.h>

#ifdef ANDROID
#define USE_ANDROID_SIMPLE_BUFFER_QUEUE     // change to #undef for compatibility testing
#endif

#ifdef USE_ANDROID_SIMPLE_BUFFER_QUEUE
#include <SLES/OpenSLES_Android.h>
#endif

#include <list>
#include <vector>
//#include <memory> //FIXME

//==================================================================================================

#ifdef USE_ANDROID_SIMPLE_BUFFER_QUEUE
#define DATALOCATOR_BUFFERQUEUE SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE
#define IID_BUFFERQUEUE SL_IID_ANDROIDSIMPLEBUFFERQUEUE
#define BufferQueueItf SLAndroidSimpleBufferQueueItf
#define BufferQueueState SLAndroidSimpleBufferQueueState
#define INDEX index
#else
#define DATALOCATOR_BUFFERQUEUE SL_DATALOCATOR_BUFFERQUEUE
#define IID_BUFFERQUEUE SL_IID_BUFFERQUEUE
#define BufferQueueItf SLBufferQueueItf
#define BufferQueueState SLBufferQueueState
#define INDEX playIndex
#endif

#define checkResult(r) do { if ((r) != SL_RESULT_SUCCESS) were_error("error %d at %s:%d\n", \
    (int) r, __FILE__, __LINE__); } while (0)

/* ================================================================================================================== */

class WereEventLoop;
class SparkleConnection;
class SparkleServer;
class WereSocketUnixMessage;
class SparkleSoundBuffer;

class SoundSLES
{
public:
    ~SoundSLES();
    SoundSLES(WereEventLoop *loop, const std::string &file);

private:
    void start();
    void stop();
    void queue();
    static void callback(BufferQueueItf playerBufferqueue, void *data);

    void connection(std::shared_ptr <SparkleConnection> client);
    void disconnection(std::shared_ptr <SparkleConnection> client);
    void packet(std::shared_ptr<SparkleConnection> client, std::shared_ptr<WereSocketUnixMessage> message);

private:
    WereEventLoop *loop_;
    SparkleServer *server_;
    SparkleSoundBuffer *buffer_;

    SLObjectItf engineObject;
    SLEngineItf engineEngine;
    SLObjectItf outputmixObject;
    SLDataSource audiosrc;
    SLDataSink audiosnk;
    SLDataFormat_PCM pcm;
    SLDataLocator_OutputMix locator_outputmix;
    SLDataLocator_BufferQueue locator_bufferqueue;
    SLObjectItf playerObject;
    SLPlayItf playerPlay;
    BufferQueueItf playerBufferqueue;
    SLuint32 state;
};

/* ================================================================================================================== */

#endif /* SOUND_SLES_H */
