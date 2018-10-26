#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <alsa/asoundlib.h>
#include <alsa/pcm_external.h>

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h> //XXX

//==============================================================================

const char *server_path = "/proc/1/root/data/data/com.sion.sparkle/files/sparkle-sound.socket";

#define NONBLOCK

//==============================================================================

typedef struct snd_pcm_sparkle
{
	snd_pcm_ioplug_t io;

	unsigned int frame_bytes;

    struct timespec start;

    int fd;
    int play;

} snd_pcm_sparkle_t;

struct sparkle_packet_header
{
    int operation;
};

//==============================================================================

static void sparkle_connect(snd_pcm_sparkle_t *sparkle)
{
    if (sparkle->fd != -1)
        return;

    int fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (fd == -1)
        return;

    struct sockaddr_un name;
    memset(&name, 0x00, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, server_path, sizeof(name.sun_path) - 1);

    if (connect(fd, (const struct sockaddr *)&name, sizeof(struct sockaddr_un)) == -1)
    {
        fprintf(stderr, "[Sparkle sound] Failed to connect (%s).\n", strerror(errno));
        close(fd);
        sparkle->fd = -1;
        return;
    }

#ifdef NONBLOCK
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
#endif

    sparkle->fd = fd;

    fprintf(stderr, "[Sparkle sound] Connected.\n");
}

static void sparkle_disconnect(snd_pcm_sparkle_t *sparkle)
{
    if (sparkle->fd == -1)
        return;

    shutdown(sparkle->fd, SHUT_RDWR);
    close(sparkle->fd);

    sparkle->fd = -1;

    fprintf(stderr, "[Sparkle sound] Disconnected.\n");
}

static void sparkle_send_data(snd_pcm_sparkle_t *sparkle, const void *data, int$
{
    uint32_t operation = 0x33;
    uint32_t size_ = size;

    struct iovec iov[3];
    iov[0].iov_base = &operation;
    iov[0].iov_len = sizeof(uint32_t);
    iov[1].iov_base = &size_;
    iov[1].iov_len = sizeof(uint32_t);
    iov[2].iov_base = (void *)data;
    iov[2].iov_len = size;

    struct msghdr msg = {0};
    msg.msg_iov = iov;
    msg.msg_iovlen = 3; /* iov array size */

    int ret = sendmsg(sparkle->fd, &msg, 0);
    if (ret == -1)
        sparkle_disconnect(sparkle);
}

static void sparkle_send_op(snd_pcm_sparkle_t *sparkle, uint32_t operation)
{
    struct iovec iov[1];
    iov[0].iov_base = &operation;
    iov[0].iov_len = sizeof(uint32_t);

    struct msghdr msg = {0};
    msg.msg_iov = iov;
    msg.msg_iovlen = 1; /* iov array size */

    int ret = sendmsg(sparkle->fd, &msg, 0);
    if (ret == -1)
        sparkle_disconnect(sparkle);
}

static void sparkle_send_start(snd_pcm_sparkle_t *sparkle)
{
    sparkle_send_op(sparkle, 0x31);
}

static void sparkle_send_stop(snd_pcm_sparkle_t *sparkle)
{
    sparkle_send_op(sparkle, 0x32);
}

//==============================================================================

static snd_pcm_sframes_t sparkle_write(snd_pcm_ioplug_t *io,
				   const snd_pcm_channel_area_t *areas,
				   snd_pcm_uframes_t offset,
				   snd_pcm_uframes_t size)
{
    snd_pcm_sparkle_t *sparkle = io->private_data;

    const char *buf;
    //ssize_t result;

    /* we handle only an interleaved buffer */
    buf = (char *)areas->addr + (areas->first + areas->step * offset) / 8;

    //result = write(oss->fd, buf, size);

    int max_frames = 4096 / sparkle->frame_bytes;
    if (size > max_frames)
        size = max_frames;

    sparkle_send_data(sparkle, buf, size * sparkle->frame_bytes);

    //if (result <= 0)
    //  return result;

    return size;
}

static snd_pcm_sframes_t sparkle_read(snd_pcm_ioplug_t *io,
				  const snd_pcm_channel_area_t *areas,
				  snd_pcm_uframes_t offset,
				  snd_pcm_uframes_t size)
{
	snd_pcm_sparkle_t *sparkle = io->private_data;

	//char *buf;
	ssize_t result;

	/* we handle only an interleaved buffer */
	//buf = (char *)areas->addr + (areas->first + areas->step * offset) / 8;
	size *= sparkle->frame_bytes;

	//result = read(oss->fd, buf, size);

    result = size;

	if (result <= 0)
		return result;

	return result / sparkle->frame_bytes;
}

static snd_pcm_sframes_t sparkle_pointer(snd_pcm_ioplug_t *io)
{
	snd_pcm_sparkle_t *sparkle = io->private_data;

    if (sparkle->play == 0)
        return 0;

    struct timespec current;
    clock_gettime(CLOCK_REALTIME, &current);

    uint64_t elapsed = 0;
    elapsed += 1000LL * (current.tv_sec - sparkle->start.tv_sec);
    elapsed += (current.tv_nsec - sparkle->start.tv_nsec) / 1000000;

    int frames = elapsed * 44100 / 1000;

	return frames;
}

//==============================================================================

static int sparkle_start(snd_pcm_ioplug_t *io)
{
	snd_pcm_sparkle_t *sparkle = io->private_data;

    clock_gettime(CLOCK_REALTIME, &sparkle->start);

    sparkle->play = 1;

    sparkle_connect(sparkle);

    sparkle_send_start(sparkle);

	return 0;
}

static int sparkle_stop(snd_pcm_ioplug_t *io)
{
	snd_pcm_sparkle_t *sparkle = io->private_data;

    sparkle->play = 0;

    sparkle_send_stop(sparkle);

	return 0;
}

//==============================================================================

static int sparkle_drain(snd_pcm_ioplug_t *io)
{
	//snd_pcm_sparkle_t *sparkle = io->private_data;

	return 0;
}

static int sparkle_prepare(snd_pcm_ioplug_t *io)
{
	//snd_pcm_sparkle_t *sparkle = io->private_data;

	return 0;
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

	return 0;
}

#define ARRAY_SIZE(ary)	(sizeof(ary)/sizeof(ary[0]))

static int sparkle_hw_constraint(snd_pcm_sparkle_t *sparkle)
{
	snd_pcm_ioplug_t *io = &sparkle->io;

	static const snd_pcm_access_t access_list[] = {
		SND_PCM_ACCESS_RW_INTERLEAVED,
	};

	unsigned int nformats;
	unsigned int format[5];
	unsigned int nchannels;
	//unsigned int channel[6];

	/* period and buffer bytes must be power of two */
	static const unsigned int bytes_list[] = {
		1U<<8, 1U<<9, 1U<<10, 1U<<11, 1U<<12, 1U<<13, 1U<<14, 1U<<15,
		1U<<16, 1U<<17, 1U<<18, 1U<<19, 1U<<20, 1U<<21, 1U<<22, 1U<<23
	};
	//int i, err, tmp;
    int err;

	/* access type - interleaved only */
	if ((err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_ACCESS,
						 ARRAY_SIZE(access_list), access_list)) < 0)
		return err;

	/* supported formats */
    nformats = 0;
    format[nformats++] = SND_PCM_FORMAT_S16_LE;

	if ((err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_FORMAT,
						 nformats, format)) < 0)
		return err;

	/* supported channels */
	nchannels = 0;
	if (!nchannels) /* assume 2ch stereo */
		err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_CHANNELS, 2, 2);

	if (err < 0)
		return err;

	/* supported rates */
	err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_RATE, 44100, 44100);

	if (err < 0)
		return err;

	/* period size (in power of two) */
	err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_PERIOD_BYTES, ARRAY_SIZE(bytes_list), bytes_list);
	if (err < 0)
		return err;

	/* periods */
	err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_PERIODS, 2, 1024);
	if (err < 0)
		return err;

	/* buffer size (in power of two) */
	err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_BUFFER_BYTES, ARRAY_SIZE(bytes_list), bytes_list);
	if (err < 0)
		return err;

	return 0;
}

//==============================================================================

static int sparkle_close(snd_pcm_ioplug_t *io)
{
	snd_pcm_sparkle_t *sparkle = io->private_data;

    if (sparkle->fd != -1)
    {
        shutdown(sparkle->fd, SHUT_RDWR);
        close(sparkle->fd);
        sparkle->fd = -1;
    }

	free(sparkle);

	return 0;
}

//==============================================================================

static const snd_pcm_ioplug_callback_t sparkle_playback_callback = {
	.start = sparkle_start,
	.stop = sparkle_stop,
	.transfer = sparkle_write,
	.pointer = sparkle_pointer,
	.close = sparkle_close,
	.hw_params = sparkle_hw_params,
	.prepare = sparkle_prepare,
	.drain = sparkle_drain,
};

static const snd_pcm_ioplug_callback_t sparkle_capture_callback = {
	.start = sparkle_start,
	.stop = sparkle_stop,
	.transfer = sparkle_read,
	.pointer = sparkle_pointer,
	.close = sparkle_close,
	.hw_params = sparkle_hw_params,
	.prepare = sparkle_prepare,
	.drain = sparkle_drain,
};

//==============================================================================

SND_PCM_PLUGIN_DEFINE_FUNC(sparkle)
{
	snd_config_iterator_t i, next;

	const char *device;

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
		if (strcmp(id, "device") == 0)
        {
			if (snd_config_get_string(n, &device) < 0)
            {
				SNDERR("Invalid type for %s", id);
				return -EINVAL;
			}
			continue;
		}
		SNDERR("Unknown field %s", id);
		return -EINVAL;
	}

	sparkle = calloc(1, sizeof(*sparkle));
	if (!sparkle)
    {
		SNDERR("cannot allocate");
		return -ENOMEM;
	}


    sparkle->fd = -1;
    sparkle->play = 0;
    signal(SIGPIPE, SIG_IGN);


	sparkle->io.version = SND_PCM_IOPLUG_VERSION;
	sparkle->io.name = "ALSA <-> SPARKLE PCM I/O Plugin";
	sparkle->io.callback = stream == SND_PCM_STREAM_PLAYBACK ? &sparkle_playback_callback : &sparkle_capture_callback;
	sparkle->io.private_data = sparkle;

	err = snd_pcm_ioplug_create(&sparkle->io, name, stream, mode);
	if (err < 0)
		goto error;

	if ((err = sparkle_hw_constraint(sparkle)) < 0) {
		snd_pcm_ioplug_delete(&sparkle->io);
		return err;
	}

	*pcmp = sparkle->io.pcm;

	return 0;

 error:
	free(sparkle);
	return err;
}

//==============================================================================

SND_PCM_PLUGIN_SYMBOL(sparkle);

//==============================================================================


