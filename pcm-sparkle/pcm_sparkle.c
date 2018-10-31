#include "sparkles_c.h"
#include <alsa/asoundlib.h>
#include <alsa/pcm_external.h>

#define ARRAY_SIZE(ary) (sizeof(ary)/sizeof(ary[0]))

/* ================================================================================================================== */

typedef struct snd_pcm_sparkle
{
    snd_pcm_ioplug_t io;
    unsigned int frame_bytes;
    SparklesC *sparkle;
} snd_pcm_sparkle_t;

/* ================================================================================================================== */

static snd_pcm_sframes_t sparkle_write(snd_pcm_ioplug_t *io,
                                       const snd_pcm_channel_area_t *areas,
                                       snd_pcm_uframes_t offset,
                                       snd_pcm_uframes_t size)
{
    snd_pcm_sparkle_t *sparkle = io->private_data;

    const char *buf;
    ssize_t result;

    /* we handle only an interleaved buffer */
    buf = (char *)areas->addr + (areas->first + areas->step * offset) / 8;
    size *= sparkle->frame_bytes;

    //result = write(oss->fd, buf, size);
    result = sparkles_c_write(sparkle->sparkle, buf, size);

    if (result <= 0)
        return result;

    return result / sparkle->frame_bytes;
}

static snd_pcm_sframes_t sparkle_read(snd_pcm_ioplug_t *io,
                                      const snd_pcm_channel_area_t *areas,
                                      snd_pcm_uframes_t offset,
                                      snd_pcm_uframes_t size)
{
    //snd_pcm_sparkle_t *sparkle = io->private_data;

    //char *buf;
    //ssize_t result;

    /* we handle only an interleaved buffer */
    //buf = (char *)areas->addr + (areas->first + areas->step * offset) / 8;
    //size *= sparkle->frame_bytes;

    //result = read(oss->fd, buf, size);

    //if (result <= 0)
    //    return result;

    return size;
}

static snd_pcm_sframes_t sparkle_pointer(snd_pcm_ioplug_t *io)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    snd_pcm_sparkle_t *sparkle = io->private_data;

    if (io->state == SND_PCM_STATE_XRUN)
        return -EPIPE;

    if (io->state != SND_PCM_STATE_RUNNING)
        return 0;

    return sparkles_c_pointer(sparkle->sparkle);
}

static int sparkle_start(snd_pcm_ioplug_t *io)
{
    snd_pcm_sparkle_t *sparkle = io->private_data;

    return sparkles_c_start(sparkle->sparkle);
}

static int sparkle_stop(snd_pcm_ioplug_t *io)
{
    snd_pcm_sparkle_t *sparkle = io->private_data;

    return sparkles_c_stop(sparkle->sparkle);
}

static int sparkle_hw_params(snd_pcm_ioplug_t *io,
                             snd_pcm_hw_params_t *params ATTRIBUTE_UNUSED)
{
    snd_pcm_sparkle_t *sparkle = io->private_data;

    sparkle->frame_bytes = (snd_pcm_format_physical_width(io->format) * io->channels) / 8;

    if (io->format != SND_PCM_FORMAT_S16_LE)
    {
        fprintf(stderr, "SPARKLE SOUND: Unsupported format (%s)!\n", snd_pcm_format_name(io->format));
        return -EINVAL;
    }

    fprintf(stderr, "SPARKLE SOUND: buffer_size_frames=%ld frame_size_bytes=%d period_size_frames=%ld\n",
            io->buffer_size, sparkle->frame_bytes, io->period_size);

    return 0;
}

static int sparkle_poll_revents(snd_pcm_ioplug_t * io,
                                  struct pollfd *pfd,
                                  unsigned int nfds,
                                  unsigned short *revents)
{
    snd_pcm_sparkle_t *sparkle = io->private_data;

    sparkles_c_process(sparkle->sparkle);

    *revents = io->stream == SND_PCM_STREAM_PLAYBACK ? POLLOUT : POLLIN;

    return 0;
}

static int sparkle_hw_constraint(snd_pcm_sparkle_t *sparkle)
{
    snd_pcm_ioplug_t *io = &sparkle->io;

    static const snd_pcm_access_t access_list[] = {
        SND_PCM_ACCESS_RW_INTERLEAVED,
    };

    static const unsigned int formats[] = {
        SND_PCM_FORMAT_S16_LE,
    };

    /* period and buffer bytes must be power of two */
#if 0
#if 0
    static const unsigned int bytes_list[] = {
        1U<<8, 1U<<9, 1U<<10, 1U<<11, 1U<<12, 1U<<13, 1U<<14, 1U<<15,
        1U<<16, 1U<<17, 1U<<18, 1U<<19, 1U<<20, 1U<<21, 1U<<22, 1U<<23
    };
#else
    static const unsigned int bytes_list[] = {
        1U<<19, 1U<<20,
    };
#endif
#endif

    int err;

    /* access type - interleaved only */
    err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_ACCESS, ARRAY_SIZE(access_list), access_list);
    if (err < 0)
        return err;

    err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_FORMAT, ARRAY_SIZE(formats), formats);
    if (err < 0)
        return err;

    /* supported channels */
    err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_CHANNELS, 2, 2);
    if (err < 0)
        return err;

    /* supported rates */
    err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_RATE, 44100, 44100);
    if (err < 0)
        return err;

    /* period size (in power of two) */
    //err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_PERIOD_BYTES, ARRAY_SIZE(bytes_list), bytes_list);
    err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_PERIOD_BYTES, 1U<<16, 1U<<16);
    if (err < 0)
        return err;

    /* periods */
    err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_PERIODS, 4, 4);
    if (err < 0)
        return err;

    /* buffer size (in power of two) */
    //err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_BUFFER_BYTES, ARRAY_SIZE(bytes_list), bytes_list);
    err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_BUFFER_BYTES, 1U<<18, 1U<<18);
    if (err < 0)
        return err;

    return 0;
}

/* ================================================================================================================== */

static int sparkle_close(snd_pcm_ioplug_t *io)
{
    snd_pcm_sparkle_t *sparkle = io->private_data;

    sparkles_c_destroy(sparkle->sparkle);
    free(sparkle);

    return 0;
}

/* ================================================================================================================== */

static const snd_pcm_ioplug_callback_t sparkle_playback_callback = {
    .start = sparkle_start,
    .stop = sparkle_stop,
    .transfer = sparkle_write,
    .pointer = sparkle_pointer,
    .close = sparkle_close,
    .hw_params = sparkle_hw_params,
    .poll_revents = sparkle_poll_revents,
};

static const snd_pcm_ioplug_callback_t sparkle_capture_callback = {
    .start = sparkle_start,
    .stop = sparkle_stop,
    .transfer = sparkle_read,
    .pointer = sparkle_pointer,
    .close = sparkle_close,
    .hw_params = sparkle_hw_params,
    .poll_revents = sparkle_poll_revents,
};

/* ================================================================================================================== */

SND_PCM_PLUGIN_DEFINE_FUNC(sparkle)
{
    snd_config_iterator_t i, next;

    const char *server = NULL;

    int err;
    snd_pcm_sparkle_t *sparkle;

    snd_config_for_each(i, next, conf)
    {
        snd_config_t *n = snd_config_iterator_entry(i);
        const char *id;

        if (snd_config_get_id(n, &id) < 0)
            continue;
        if (strcmp(id, "comment") == 0 || strcmp(id, "type") == 0 || strcmp(id, "hint") == 0)
            continue;
        if (strcmp(id, "server") == 0)
        {
            if (snd_config_get_string(n, &server) < 0)
            {
                SNDERR("Invalid type for %s", id);
                return -EINVAL;
            }
            continue;
        }
        SNDERR("Unknown field %s", id);
        return -EINVAL;
    }

    if (!server)
        return -EINVAL;

    sparkle = calloc(1, sizeof(*sparkle));
    if (!sparkle)
        return -ENOMEM;

    sparkle->sparkle = sparkles_c_create(server);

    //signal(SIGPIPE, SIG_IGN);


    sparkle->io.version = SND_PCM_IOPLUG_VERSION;
    sparkle->io.name = "ALSA <-> Sparkle PCM I/O Plugin";
    sparkle->io.poll_fd = sparkles_c_fd(sparkle->sparkle);
    sparkle->io.poll_events = POLLIN;
    sparkle->io.callback = stream == SND_PCM_STREAM_PLAYBACK ? &sparkle_playback_callback : &sparkle_capture_callback;
    sparkle->io.private_data = sparkle;

    err = snd_pcm_ioplug_create(&sparkle->io, name, stream, mode);
    if (err < 0)
        goto error;

    err = sparkle_hw_constraint(sparkle);
    if (err < 0) {
        snd_pcm_ioplug_delete(&sparkle->io);
        return err;
    }

    *pcmp = sparkle->io.pcm;

    return 0;

error:
    sparkles_c_destroy(sparkle->sparkle);
    free(sparkle);
    return err;
}

/* ================================================================================================================== */

SND_PCM_PLUGIN_SYMBOL(sparkle);

/* ================================================================================================================== */
