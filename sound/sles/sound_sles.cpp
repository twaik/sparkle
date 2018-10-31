#include "sound_sles.h"
#include "were/were_event_loop.h"
#include "common/sparkle_connection.h"
#include "common/sparkle_protocol.h"
#include "common/sparkle_server.h"
#include "common/sparkle_sound_buffer.h"
#include <SLES/OpenSLES.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

/* ================================================================================================================== */

SoundSLES::~SoundSLES()
{
    stop();

    (*playerObject)->Destroy(playerObject);
    (*outputmixObject)->Destroy(outputmixObject);
    (*engineObject)->Destroy(engineObject);

    delete server_;
}

SoundSLES::SoundSLES(WereEventLoop *loop, const std::string &file)
{
    loop_ = loop;

    server_ = new SparkleServer(loop_, file);
    server_->signal_connected.connect(WereSimpleQueuer(loop, &SoundSLES::connection, this));
    server_->signal_disconnected.connect(WereSimpleQueuer(loop, &SoundSLES::disconnection, this));
    server_->signal_packet.connect(WereSimpleQueuer(loop, &SoundSLES::packet, this));
    buffer_ = nullptr;

    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    checkResult(result);

    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    checkResult(result);

    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    checkResult(result);

    // create output mix
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputmixObject, 0, NULL, NULL);
    checkResult(result);

    result = (*outputmixObject)->Realize(outputmixObject, SL_BOOLEAN_FALSE);
    checkResult(result);

    // create audio player

    locator_bufferqueue.locatorType = DATALOCATOR_BUFFERQUEUE;
    locator_bufferqueue.numBuffers = 255;
    locator_outputmix.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    locator_outputmix.outputMix = outputmixObject;

    pcm.formatType = SL_DATAFORMAT_PCM;
    pcm.numChannels = 2;
    pcm.samplesPerSec = SL_SAMPLINGRATE_44_1;
    pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    pcm.containerSize = 16;
    pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;

    audiosrc.pLocator = &locator_bufferqueue;
    audiosrc.pFormat = &pcm;
    audiosnk.pLocator = &locator_outputmix;
    audiosnk.pFormat = NULL;


    SLInterfaceID ids[1] = {IID_BUFFERQUEUE};
    SLboolean flags[1] = {SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &playerObject, &audiosrc, &audiosnk, 1, ids, flags);
    checkResult(result);

    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    checkResult(result);

    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
    checkResult(result);

    result = (*playerObject)->GetInterface(playerObject, IID_BUFFERQUEUE, &playerBufferqueue);
    checkResult(result);

    result = (*playerBufferqueue)->RegisterCallback(playerBufferqueue, callback, this);
    checkResult(result);

#if 1
    state = SL_PLAYSTATE_STOPPED;
    result = (*playerPlay)->SetPlayState(playerPlay, state);
    checkResult(result);
#endif
}

void SoundSLES::start()
{
    if (state == SL_PLAYSTATE_PLAYING)
        return;

    were_debug("Starting playback...\n");
    state = SL_PLAYSTATE_PLAYING;
    SLresult result = (*playerPlay)->SetPlayState(playerPlay, state);
    checkResult(result);

    queue();
}

void SoundSLES::stop()
{
    if (state == SL_PLAYSTATE_STOPPED)
        return;

    were_debug("Stopping playback...\n");
    state = SL_PLAYSTATE_STOPPED;
    SLresult result = (*playerPlay)->SetPlayState(playerPlay, state);
    checkResult(result);
    result = (*playerBufferqueue)->Clear(playerBufferqueue);
    checkResult(result);
}

void SoundSLES::queue()
{
    if (buffer_ == nullptr)
        return;

    //were_debug("rpos %d\n", *buffer_->readPosition());

    void *data;
    int r_size;
    buffer_->get(&data, 16384 * 4, &r_size);

    SLresult result = (*playerBufferqueue)->Enqueue(playerBufferqueue, data, r_size);
    checkResult(result);
}

void SoundSLES::callback(BufferQueueItf playerBufferqueue, void *data)
{
    SoundSLES *sound = reinterpret_cast<SoundSLES *>(data);

    sound->queue();
}

/* ================================================================================================================== */

void SoundSLES::connection(std::shared_ptr <SparkleConnection> client)
{
    were_debug("Sound: connection.\n");
    stop();
}

void SoundSLES::disconnection(std::shared_ptr <SparkleConnection> client)
{
    were_debug("Sound: disconnection.\n");
    stop();
    if (buffer_ != nullptr)
    {
        delete buffer_;
        buffer_ = nullptr;
    }
}

void SoundSLES::packet(std::shared_ptr<SparkleConnection> client, std::shared_ptr<WereSocketUnixMessage> message)
{
    uint32_t operation;

    WereSocketUnixMessageStream stream(message.get());
    stream >> operation;

    if (operation == RegisterSoundBufferRequestCode)
    {
        stop();
        if (buffer_ != nullptr)
        {
            delete buffer_;
            buffer_ = nullptr;
        }

        RegisterSoundBufferRequest r1;
        stream >> r1;

        buffer_ = new SparkleSoundBuffer(r1.key, r1.size, false);
    }
    else if (operation == UnregisterSoundBufferRequestCode)
    {
        stop();
        if (buffer_ != nullptr)
        {
            delete buffer_;
            buffer_ = nullptr;
        }
    }
    else if (operation == SoundStartCode)
        start();
    else if (operation == SoundStopCode)
        stop();
}

/* ================================================================================================================== */

#if 0
void SoundSLES::beep()
{
    const int SINE_FRAMES = 44100 / 20;

    typedef struct
    {
        short left;
        short right;
    } frame_t;

    unsigned i;
    float pi2 = 3.14 * 2;
    float hz = 441*5;
    float sr = 44100;

	frame_t sine[SINE_FRAMES];

    for (i = 0; i < SINE_FRAMES; ++i) {
        sine[i].left = sin((float) (i  / (sr / hz)) * pi2 ) * 32000.0;
        sine[i].right = sine[i].left;
    }

    frame_t *buffer;
    unsigned size;

    buffer = sine;
    size = sizeof(sine);

    SLresult result = (*playerBufferqueue)->Enqueue(playerBufferqueue, buffer, size);
    checkResult(result);
}
#endif

/* ================================================================================================================== */
