#include "sparkle_protocol.h"

/* ================================================================================================================== */

const SparklePacketType register_surface_file_request =
{
    .code = 0x0000,
    {
        .pack = [](WereStream *stream, const void *data) {
            const register_surface_file_request_ *_data = static_cast<const register_surface_file_request_ *>(data);
            stream->pWrite(&p_string, &_data->name);
            stream->pWrite(&p_string, &_data->path);
            stream->pWrite(&p_uint32, &_data->width);
            stream->pWrite(&p_uint32, &_data->height);
        },
        .unpack = [](WereStream *stream, void *data) {
            register_surface_file_request_ *_data = static_cast<register_surface_file_request_ *>(data);
            stream->pRead(&p_string, &_data->name);
            stream->pRead(&p_string, &_data->path);
            stream->pRead(&p_uint32, &_data->width);
            stream->pRead(&p_uint32, &_data->height);
        },
    },
};

const SparklePacketType unregister_surface_request =
{
    .code = 0x0001,
    {
        .pack = [](WereStream *stream, const void *data) {
            const unregister_surface_request_ *_data = static_cast<const unregister_surface_request_ *>(data);
            stream->pWrite(&p_string, &_data->name);
        },
        .unpack = [](WereStream *stream, void *data) {
            unregister_surface_request_ *_data = static_cast<unregister_surface_request_ *>(data);
            stream->pRead(&p_string, &_data->name);
        },
    },
};

const SparklePacketType set_surface_position_request =
{
    .code = 0x0002,
    {
        .pack = [](WereStream *stream, const void *data) {
            const set_surface_position_request_ *_data = static_cast<const set_surface_position_request_ *>(data);
            stream->pWrite(&p_string, &_data->name);
            stream->pWrite(&p_uint32, &_data->x1);
            stream->pWrite(&p_uint32, &_data->y1);
            stream->pWrite(&p_uint32, &_data->x2);
            stream->pWrite(&p_uint32, &_data->y2);
        },
        .unpack = [](WereStream *stream, void *data) {
            set_surface_position_request_ *_data = static_cast<set_surface_position_request_ *>(data);
            stream->pRead(&p_string, &_data->name);
            stream->pRead(&p_uint32, &_data->x1);
            stream->pRead(&p_uint32, &_data->y1);
            stream->pRead(&p_uint32, &_data->x2);
            stream->pRead(&p_uint32, &_data->y2);
        },
    },
};

const SparklePacketType set_surface_strata_request =
{
    .code = 0x0003,
    {
        .pack = [](WereStream *stream, const void *data) {
            const set_surface_strata_request_ *_data = static_cast<const set_surface_strata_request_ *>(data);
            stream->pWrite(&p_string, &_data->name);
            stream->pWrite(&p_uint32, &_data->strata);
        },
        .unpack = [](WereStream *stream, void *data) {
            set_surface_strata_request_ *_data = static_cast<set_surface_strata_request_ *>(data);
            stream->pRead(&p_string, &_data->name);
            stream->pRead(&p_uint32, &_data->strata);
        },
    },
};

const SparklePacketType set_surface_alpha_request =
{
    .code = 0x0004,
    {
        .pack = [](WereStream *stream, const void *data) {
            const set_surface_alpha_request_ *_data = static_cast<const set_surface_alpha_request_ *>(data);
            stream->pWrite(&p_string, &_data->name);
            stream->pWrite(&p_float, &_data->alpha);
        },
        .unpack = [](WereStream *stream, void *data) {
            set_surface_alpha_request_ *_data = static_cast<set_surface_alpha_request_ *>(data);
            stream->pRead(&p_string, &_data->name);
            stream->pRead(&p_float, &_data->alpha);
        },
    },
};

const SparklePacketType add_surface_damage_request =
{
    .code = 0x0005,
    {
        .pack = [](WereStream *stream, const void *data) {
            const add_surface_damage_request_ *_data = static_cast<const add_surface_damage_request_ *>(data);
            stream->pWrite(&p_string, &_data->name);
            stream->pWrite(&p_uint32, &_data->x1);
            stream->pWrite(&p_uint32, &_data->y1);
            stream->pWrite(&p_uint32, &_data->x2);
            stream->pWrite(&p_uint32, &_data->y2);
        },
        .unpack = [](WereStream *stream, void *data) {
            add_surface_damage_request_ *_data = static_cast<add_surface_damage_request_ *>(data);
            stream->pRead(&p_string, &_data->name);
            stream->pRead(&p_uint32, &_data->x1);
            stream->pRead(&p_uint32, &_data->y1);
            stream->pRead(&p_uint32, &_data->x2);
            stream->pRead(&p_uint32, &_data->y2);
        },
    },
};

const SparklePacketType display_size_notification =
{
    .code = 0x1000,
    {
        .pack = [](WereStream *stream, const void *data) {
            const display_size_notification_ *_data = static_cast<const display_size_notification_ *>(data);
            stream->pWrite(&p_uint32, &_data->width);
            stream->pWrite(&p_uint32, &_data->height);
        },
        .unpack = [](WereStream *stream, void *data) {
            display_size_notification_ *_data = static_cast<display_size_notification_ *>(data);
            stream->pRead(&p_uint32, &_data->width);
            stream->pRead(&p_uint32, &_data->height);
        },
    },
};

const SparklePacketType pointer_down_notification =
{
    .code = 0x1001,
    {
        .pack = [](WereStream *stream, const void *data) {
            const pointer_down_notification_ *_data = static_cast<const pointer_down_notification_ *>(data);
            stream->pWrite(&p_string, &_data->surface);
            stream->pWrite(&p_uint32, &_data->slot);
            stream->pWrite(&p_uint32, &_data->x);
            stream->pWrite(&p_uint32, &_data->y);
        },
        .unpack = [](WereStream *stream, void *data) {
            pointer_down_notification_ *_data = static_cast<pointer_down_notification_ *>(data);
            stream->pRead(&p_string, &_data->surface);
            stream->pRead(&p_uint32, &_data->slot);
            stream->pRead(&p_uint32, &_data->x);
            stream->pRead(&p_uint32, &_data->y);
        },
    },
};

const SparklePacketType pointer_up_notification =
{
    .code = 0x1002,
    {
        .pack = [](WereStream *stream, const void *data) {
            const pointer_up_notification_ *_data = static_cast<const pointer_up_notification_ *>(data);
            stream->pWrite(&p_string, &_data->surface);
            stream->pWrite(&p_uint32, &_data->slot);
            stream->pWrite(&p_uint32, &_data->x);
            stream->pWrite(&p_uint32, &_data->y);
        },
        .unpack = [](WereStream *stream, void *data) {
            pointer_up_notification_ *_data = static_cast<pointer_up_notification_ *>(data);
            stream->pRead(&p_string, &_data->surface);
            stream->pRead(&p_uint32, &_data->slot);
            stream->pRead(&p_uint32, &_data->x);
            stream->pRead(&p_uint32, &_data->y);
        },
    },
};

const SparklePacketType pointer_motion_notification =
{
    .code = 0x1003,
    {
        .pack = [](WereStream *stream, const void *data) {
            const pointer_motion_notification_ *_data = static_cast<const pointer_motion_notification_ *>(data);
            stream->pWrite(&p_string, &_data->surface);
            stream->pWrite(&p_uint32, &_data->slot);
            stream->pWrite(&p_uint32, &_data->x);
            stream->pWrite(&p_uint32, &_data->y);
        },
        .unpack = [](WereStream *stream, void *data) {
            pointer_motion_notification_ *_data = static_cast<pointer_motion_notification_ *>(data);
            stream->pRead(&p_string, &_data->surface);
            stream->pRead(&p_uint32, &_data->slot);
            stream->pRead(&p_uint32, &_data->x);
            stream->pRead(&p_uint32, &_data->y);
        },
    },
};

const SparklePacketType key_down_notification =
{
    .code = 0x1004,
    {
        .pack = [](WereStream *stream, const void *data) {
            const key_down_notification_ *_data = static_cast<const key_down_notification_ *>(data);
            stream->pWrite(&p_uint32, &_data->code);
        },
        .unpack = [](WereStream *stream, void *data) {
            key_down_notification_ *_data = static_cast<key_down_notification_ *>(data);
            stream->pRead(&p_uint32, &_data->code);
        },
    },
};

const SparklePacketType key_up_notification =
{
    .code = 0x1005,
    {
        .pack = [](WereStream *stream, const void *data) {
            const key_up_notification_ *_data = static_cast<const key_up_notification_ *>(data);
            stream->pWrite(&p_uint32, &_data->code);
        },
        .unpack = [](WereStream *stream, void *data) {
            key_up_notification_ *_data = static_cast<key_up_notification_ *>(data);
            stream->pRead(&p_uint32, &_data->code);
        },
    },
};

/* ================================================================================================================== */

const SparklePacketType sound_data =
{
    .code = 0x2000,
    {
        .pack = [](WereStream *stream, const void *data) {
            const sound_data_ *_data = static_cast<const sound_data_ *>(data);
            stream->pWrite(&p_uint32, &_data->size);
            stream->write(_data->data, _data->size);
        },
        .unpack = [](WereStream *stream, void *data) {
            sound_data_ *_data = static_cast<sound_data_ *>(data);
            stream->pRead(&p_uint32, &_data->size);
            _data->data = reinterpret_cast<const unsigned char *>(stream->get(_data->size));
        },
    },
};

const SparklePacketType sound_start =
{
    .code = 0x2001,
    {
        .pack = [](WereStream *stream, const void *data) {},
        .unpack = [](WereStream *stream, void *data) {},
    },
};

const SparklePacketType sound_stop =
{
    .code = 0x2002,
    {
        .pack = [](WereStream *stream, const void *data) {},
        .unpack = [](WereStream *stream, void *data) {},
    },
};

/* ================================================================================================================== */

const SparklePacketType key_down_request =
{
    .code = 0x0006,
    {
        .pack = [](WereStream *stream, const void *data) {
            const key_down_request_ *_data = static_cast<const key_down_request_ *>(data);
            stream->pWrite(&p_uint32, &_data->code);
        },
        .unpack = [](WereStream *stream, void *data) {
            key_down_request_ *_data = static_cast<key_down_request_ *>(data);
            stream->pRead(&p_uint32, &_data->code);
        },
    },
};

const SparklePacketType key_up_request =
{
    .code = 0x0007,
    {
        .pack = [](WereStream *stream, const void *data) {
            const key_up_request_ *_data = static_cast<const key_up_request_ *>(data);
            stream->pWrite(&p_uint32, &_data->code);
        },
        .unpack = [](WereStream *stream, void *data) {
            key_up_request_ *_data = static_cast<key_up_request_ *>(data);
            stream->pRead(&p_uint32, &_data->code);
        },
    },
};

/* ================================================================================================================== */
