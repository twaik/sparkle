#include "sparkle_protocol.h"
#include "sparkle_packet.h"

//==================================================================================================

int operation1(const packet_type_t *packetType)
{
    return packetType->code;
}

//==================================================================================================

const packet_type_t register_surface_file_request =
{
    .code = 0x0000,
    .pack = [](SparklePacket *packet, void *data) {
        _register_surface_file_request *_data = static_cast<_register_surface_file_request *>(data);
        SparklePacketStream stream(packet);
        stream.addString(_data->name);
        stream.addString(_data->path);
        stream.addUint32(_data->width);
        stream.addUint32(_data->height);
    },
    .unpack = [](SparklePacket *packet, void *data) {
        _register_surface_file_request *_data = static_cast<_register_surface_file_request *>(data);
        SparklePacketStream stream(packet);
        _data->name = stream.getStringPointer();
        _data->path = stream.getStringPointer();
        _data->width = stream.getUint32();
        _data->height = stream.getUint32();
    },
};

const packet_type_t unregister_surface_request =
{
    .code = 0x0001,
    .pack = [](SparklePacket *packet, void *data) {
        _unregister_surface_request *_data = static_cast<_unregister_surface_request *>(data);
        SparklePacketStream stream(packet);
        stream.addString(_data->name);
    },
    .unpack = [](SparklePacket *packet, void *data) {
        _unregister_surface_request *_data = static_cast<_unregister_surface_request *>(data);
        SparklePacketStream stream(packet);
        _data->name = stream.getStringPointer();
    },
};

const packet_type_t set_surface_position_request =
{
    .code = 0x0002,
    .pack = [](SparklePacket *packet, void *data) {
        _set_surface_position_request *_data = static_cast<_set_surface_position_request *>(data);
        SparklePacketStream stream(packet);
        stream.addString(_data->name);
        stream.addUint32(_data->x1);
        stream.addUint32(_data->y1);
        stream.addUint32(_data->x2);
        stream.addUint32(_data->y2);
    },
    .unpack = [](SparklePacket *packet, void *data) {
        _set_surface_position_request *_data = static_cast<_set_surface_position_request *>(data);
        SparklePacketStream stream(packet);
        _data->name = stream.getStringPointer();
        _data->x1 = stream.getUint32();
        _data->y1 = stream.getUint32();
        _data->x2 = stream.getUint32();
        _data->y2 = stream.getUint32();
    },
};

const packet_type_t set_surface_strata_request =
{
    .code = 0x0003,
    .pack = [](SparklePacket *packet, void *data) {
        _set_surface_strata_request *_data = static_cast<_set_surface_strata_request *>(data);
        SparklePacketStream stream(packet);
        stream.addString(_data->name);
        stream.addUint32(_data->strata);
    },
    .unpack = [](SparklePacket *packet, void *data) {
        _set_surface_strata_request *_data = static_cast<_set_surface_strata_request *>(data);
        SparklePacketStream stream(packet);
        _data->name = stream.getStringPointer();
        _data->strata = stream.getUint32();
    },
};

const packet_type_t add_surface_damage_request =
{
    .code = 0x0004,
    .pack = [](SparklePacket *packet, void *data) {
        _add_surface_damage_request *_data = static_cast<_add_surface_damage_request *>(data);
        SparklePacketStream stream(packet);
        stream.addString(_data->name);
        stream.addUint32(_data->x1);
        stream.addUint32(_data->y1);
        stream.addUint32(_data->x2);
        stream.addUint32(_data->y2);
    },
    .unpack = [](SparklePacket *packet, void *data) {
        _add_surface_damage_request *_data = static_cast<_add_surface_damage_request *>(data);
        SparklePacketStream stream(packet);
        _data->name = stream.getStringPointer();
        _data->x1 = stream.getUint32();
        _data->y1 = stream.getUint32();
        _data->x2 = stream.getUint32();
        _data->y2 = stream.getUint32();
    },
};

const packet_type_t display_size_notification =
{
    .code = 0x1000,
    .pack = [](SparklePacket *packet, void *data) {
        _display_size_notification *_data = static_cast<_display_size_notification *>(data);
        SparklePacketStream stream(packet);
        stream.addUint32(_data->width);
        stream.addUint32(_data->height);
    },
    .unpack = [](SparklePacket *packet, void *data) {
        _display_size_notification *_data = static_cast<_display_size_notification *>(data);
        SparklePacketStream stream(packet);
        _data->width = stream.getUint32();
        _data->height = stream.getUint32();
    },
};

const packet_type_t pointer_down_notification =
{
    .code = 0x1001,
    .pack = [](SparklePacket *packet, void *data) {
        _pointer_down_notification *_data = static_cast<_pointer_down_notification *>(data);
        SparklePacketStream stream(packet);
        stream.addString(_data->surface);
        stream.addUint32(_data->slot);
        stream.addUint32(_data->x);
        stream.addUint32(_data->y);
    },
    .unpack = [](SparklePacket *packet, void *data) {
        _pointer_down_notification *_data = static_cast<_pointer_down_notification *>(data);
        SparklePacketStream stream(packet);
        _data->surface = stream.getStringPointer();
        _data->slot = stream.getUint32();
        _data->x = stream.getUint32();
        _data->y = stream.getUint32();
    },
};

const packet_type_t pointer_up_notification =
{
    .code = 0x1002,
    .pack = [](SparklePacket *packet, void *data) {
        _pointer_up_notification *_data = static_cast<_pointer_up_notification *>(data);
        SparklePacketStream stream(packet);
        stream.addString(_data->surface);
        stream.addUint32(_data->slot);
        stream.addUint32(_data->x);
        stream.addUint32(_data->y);
    },
    .unpack = [](SparklePacket *packet, void *data) {
        _pointer_up_notification *_data = static_cast<_pointer_up_notification *>(data);
        SparklePacketStream stream(packet);
        _data->surface = stream.getStringPointer();
        _data->slot = stream.getUint32();
        _data->x = stream.getUint32();
        _data->y = stream.getUint32();
    },
};

const packet_type_t pointer_motion_notification =
{
    .code = 0x1003,
    .pack = [](SparklePacket *packet, void *data) {
        _pointer_motion_notification *_data = static_cast<_pointer_motion_notification *>(data);
        SparklePacketStream stream(packet);
        stream.addString(_data->surface);
        stream.addUint32(_data->slot);
        stream.addUint32(_data->x);
        stream.addUint32(_data->y);
    },
    .unpack = [](SparklePacket *packet, void *data) {
        _pointer_motion_notification *_data = static_cast<_pointer_motion_notification *>(data);
        SparklePacketStream stream(packet);
        _data->surface = stream.getStringPointer();
        _data->slot = stream.getUint32();
        _data->x = stream.getUint32();
        _data->y = stream.getUint32();
    },
};

const packet_type_t key_down_notification =
{
    .code = 0x1004,
    .pack = [](SparklePacket *packet, void *data) {
        _key_down_notification *_data = static_cast<_key_down_notification *>(data);
        SparklePacketStream stream(packet);
        stream.addUint32(_data->code);
    },
    .unpack = [](SparklePacket *packet, void *data) {
        _key_down_notification *_data = static_cast<_key_down_notification *>(data);
        SparklePacketStream stream(packet);
        _data->code = stream.getUint32();
    },
};

const packet_type_t key_up_notification =
{
    .code = 0x1005,
    .pack = [](SparklePacket *packet, void *data) {
        _key_up_notification *_data = static_cast<_key_up_notification *>(data);
        SparklePacketStream stream(packet);
        stream.addUint32(_data->code);
    },
    .unpack = [](SparklePacket *packet, void *data) {
        _key_up_notification *_data = static_cast<_key_up_notification *>(data);
        SparklePacketStream stream(packet);
        _data->code = stream.getUint32();
    },
};

//==================================================================================================


