#include "sparkle_protocol.h"
#include "sparkle_packet.h"

//==================================================================================================

const packet_type_t register_surface_file_request =
{
    .code = 0x0000,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _register_surface_file_request *_data = static_cast<_register_surface_file_request *>(data);
            stream->pWrite(&p_string, &_data->name);
            stream->pWrite(&p_string, &_data->path);
            stream->pWrite(&p_uint32, &_data->width);
            stream->pWrite(&p_uint32, &_data->height);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _register_surface_file_request *_data = static_cast<_register_surface_file_request *>(data);
            stream->pRead(&p_string, &_data->name);
            stream->pRead(&p_string, &_data->path);
            stream->pRead(&p_uint32, &_data->width);
            stream->pRead(&p_uint32, &_data->height);
        },
    },
};

const packet_type_t unregister_surface_request =
{
    .code = 0x0001,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _unregister_surface_request *_data = static_cast<_unregister_surface_request *>(data);
            stream->pWrite(&p_string, &_data->name);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _unregister_surface_request *_data = static_cast<_unregister_surface_request *>(data);
            stream->pRead(&p_string, &_data->name);
        },
    },
};

const packet_type_t set_surface_position_request =
{
    .code = 0x0002,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _set_surface_position_request *_data = static_cast<_set_surface_position_request *>(data);
            stream->pWrite(&p_string, &_data->name);
            stream->pWrite(&p_uint32, &_data->x1);
            stream->pWrite(&p_uint32, &_data->y1);
            stream->pWrite(&p_uint32, &_data->x2);
            stream->pWrite(&p_uint32, &_data->y2);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _set_surface_position_request *_data = static_cast<_set_surface_position_request *>(data);
            stream->pRead(&p_string, &_data->name);
            stream->pRead(&p_uint32, &_data->x1);
            stream->pRead(&p_uint32, &_data->y1);
            stream->pRead(&p_uint32, &_data->x2);
            stream->pRead(&p_uint32, &_data->y2);
        },
    },
};

const packet_type_t set_surface_strata_request =
{
    .code = 0x0003,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _set_surface_strata_request *_data = static_cast<_set_surface_strata_request *>(data);
            stream->pWrite(&p_string, &_data->name);
            stream->pWrite(&p_uint32, &_data->strata);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _set_surface_strata_request *_data = static_cast<_set_surface_strata_request *>(data);
            stream->pRead(&p_string, &_data->name);
            stream->pRead(&p_uint32, &_data->strata);
        },
    },
};

const packet_type_t set_surface_blending_request =
{
    .code = 0x0004,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _set_surface_blending_request *_data = static_cast<_set_surface_blending_request *>(data);
            stream->pWrite(&p_string, &_data->name);
            stream->pWrite(&p_uint32, &_data->blending);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _set_surface_blending_request *_data = static_cast<_set_surface_blending_request *>(data);
            stream->pRead(&p_string, &_data->name);
            stream->pRead(&p_uint32, &_data->blending);
        },
    },
};

const packet_type_t add_surface_damage_request =
{
    .code = 0x0005,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _add_surface_damage_request *_data = static_cast<_add_surface_damage_request *>(data);
            stream->pWrite(&p_string, &_data->name);
            stream->pWrite(&p_uint32, &_data->x1);
            stream->pWrite(&p_uint32, &_data->y1);
            stream->pWrite(&p_uint32, &_data->x2);
            stream->pWrite(&p_uint32, &_data->y2);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _add_surface_damage_request *_data = static_cast<_add_surface_damage_request *>(data);
            stream->pRead(&p_string, &_data->name);
            stream->pRead(&p_uint32, &_data->x1);
            stream->pRead(&p_uint32, &_data->y1);
            stream->pRead(&p_uint32, &_data->x2);
            stream->pRead(&p_uint32, &_data->y2);
        },
    },
};

const packet_type_t display_size_notification =
{
    .code = 0x1000,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _display_size_notification *_data = static_cast<_display_size_notification *>(data);
            stream->pWrite(&p_uint32, &_data->width);
            stream->pWrite(&p_uint32, &_data->height);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _display_size_notification *_data = static_cast<_display_size_notification *>(data);
            stream->pRead(&p_uint32, &_data->width);
            stream->pRead(&p_uint32, &_data->height);
        },
    },
};

const packet_type_t pointer_down_notification =
{
    .code = 0x1001,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _pointer_down_notification *_data = static_cast<_pointer_down_notification *>(data);
            stream->pWrite(&p_string, &_data->surface);
            stream->pWrite(&p_uint32, &_data->slot);
            stream->pWrite(&p_uint32, &_data->x);
            stream->pWrite(&p_uint32, &_data->y);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _pointer_down_notification *_data = static_cast<_pointer_down_notification *>(data);
            stream->pRead(&p_string, &_data->surface);
            stream->pRead(&p_uint32, &_data->slot);
            stream->pRead(&p_uint32, &_data->x);
            stream->pRead(&p_uint32, &_data->y);
        },
    },
};

const packet_type_t pointer_up_notification =
{
    .code = 0x1002,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _pointer_up_notification *_data = static_cast<_pointer_up_notification *>(data);
            stream->pWrite(&p_string, &_data->surface);
            stream->pWrite(&p_uint32, &_data->slot);
            stream->pWrite(&p_uint32, &_data->x);
            stream->pWrite(&p_uint32, &_data->y);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _pointer_up_notification *_data = static_cast<_pointer_up_notification *>(data);
            stream->pRead(&p_string, &_data->surface);
            stream->pRead(&p_uint32, &_data->slot);
            stream->pRead(&p_uint32, &_data->x);
            stream->pRead(&p_uint32, &_data->y);
        },
    },
};

const packet_type_t pointer_motion_notification =
{
    .code = 0x1003,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _pointer_motion_notification *_data = static_cast<_pointer_motion_notification *>(data);
            stream->pWrite(&p_string, &_data->surface);
            stream->pWrite(&p_uint32, &_data->slot);
            stream->pWrite(&p_uint32, &_data->x);
            stream->pWrite(&p_uint32, &_data->y);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _pointer_motion_notification *_data = static_cast<_pointer_motion_notification *>(data);
            stream->pRead(&p_string, &_data->surface);
            stream->pRead(&p_uint32, &_data->slot);
            stream->pRead(&p_uint32, &_data->x);
            stream->pRead(&p_uint32, &_data->y);
        },
    },
};

const packet_type_t key_down_notification =
{
    .code = 0x1004,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _key_down_notification *_data = static_cast<_key_down_notification *>(data);
            stream->pWrite(&p_uint32, &_data->code);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _key_down_notification *_data = static_cast<_key_down_notification *>(data);
            stream->pRead(&p_uint32, &_data->code);
        },
    },
};

const packet_type_t key_up_notification =
{
    .code = 0x1005,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _key_up_notification *_data = static_cast<_key_up_notification *>(data);
            stream->pWrite(&p_uint32, &_data->code);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _key_up_notification *_data = static_cast<_key_up_notification *>(data);
            stream->pRead(&p_uint32, &_data->code);
        },
    },
};

//==================================================================================================

const packet_type_t sound_data =
{
    .code = 0x2000,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _sound_data *_data = static_cast<_sound_data *>(data);
            stream->pWrite(&p_uint32, &_data->size);
            stream->write(_data->data, _data->size);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _sound_data *_data = static_cast<_sound_data *>(data);
            stream->pRead(&p_uint32, &_data->size);
            _data->data = reinterpret_cast<const unsigned char *>(stream->getData(_data->size));
        },
    },
};

const packet_type_t sound_start =
{
    .code = 0x2001,
    {
        .pack = [](SparklePacketStream *stream, void *data) {},
        .unpack = [](SparklePacketStream *stream, void *data) {},
    },
};

const packet_type_t sound_stop =
{
    .code = 0x2002,
    {
        .pack = [](SparklePacketStream *stream, void *data) {},
        .unpack = [](SparklePacketStream *stream, void *data) {},
    },
};

//==================================================================================================

const packet_type_t key_down_request =
{
    .code = 0x0006,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _key_down_request *_data = static_cast<_key_down_request *>(data);
            stream->pWrite(&p_uint32, &_data->code);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _key_down_request *_data = static_cast<_key_down_request *>(data);
            stream->pRead(&p_uint32, &_data->code);
        },
    },
};

const packet_type_t key_up_request =
{
    .code = 0x0007,
    {
        .pack = [](SparklePacketStream *stream, void *data) {
            _key_up_request *_data = static_cast<_key_up_request *>(data);
            stream->pWrite(&p_uint32, &_data->code);
        },
        .unpack = [](SparklePacketStream *stream, void *data) {
            _key_up_request *_data = static_cast<_key_up_request *>(data);
            stream->pRead(&p_uint32, &_data->code);
        },
    },
};

//==================================================================================================

