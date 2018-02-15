#ifndef SOUND_SLES_H
#define SOUND_SLES_H

#include <SLES/OpenSLES.h>

#ifdef ANDROID
#define USE_ANDROID_SIMPLE_BUFFER_QUEUE     // change to #undef for compatibility testing
#endif

#ifdef USE_ANDROID_SIMPLE_BUFFER_QUEUE
#include <SLES/OpenSLES_Android.h>
#endif

#include "common/sparkle_packet.h"
#include <list>
#include <vector>

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
    
//==================================================================================================

class WereEventLoop;
class SparkleConnection;
class SparkleServer;


class SoundSLES
{
public:
    ~SoundSLES();
    SoundSLES(WereEventLoop *loop);
    
    void beep();

private:
    void connection();
    void disconnection();
    void data();
    void checkQueue();
    void clearQueue();
    static void callback(BufferQueueItf playerBufferqueue, void *data);

private:
    WereEventLoop *_loop;
    SparkleServer *_server;

    void packet(SparkleConnection *client, SparklePacket packet);
    
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
    
    std::list<SparklePacket> _queue;
    bool busy;
};

//==================================================================================================

#endif //SOUND_SLES_H

