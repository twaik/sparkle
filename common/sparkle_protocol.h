#ifndef SPARKLE_PROTOCOL_H
#define SPARKLE_PROTOCOL_H

#include "were/were_socket_unix_message_stream.h"

/* ================================================================================================================== */

struct RegisterSurfaceFdRequest
{
    std::vector<char> name;
    int fd;
    int32_t width;
    int32_t height;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const RegisterSurfaceFdRequest &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, RegisterSurfaceFdRequest &data);
const uint32_t RegisterSurfaceFdRequestCode = 0x01;

struct UnregisterSurfaceRequest
{
    std::vector<char> name;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const UnregisterSurfaceRequest &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, UnregisterSurfaceRequest &data);
const uint32_t UnregisterSurfaceRequestCode = 0x02;

struct SetSurfacePositionRequest
{
    std::vector<char> name;
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const SetSurfacePositionRequest &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, SetSurfacePositionRequest &data);
const uint32_t SetSurfacePositionRequestCode = 0x03;

struct SetSurfaceStrataRequest
{
    std::vector<char> name;
    int32_t strata;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const SetSurfaceStrataRequest &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, SetSurfaceStrataRequest &data);
const uint32_t SetSurfaceStrataRequestCode = 0x04;

struct SetSurfaceAlphaRequest
{
    std::vector<char> name;
    int32_t alpha;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const SetSurfaceAlphaRequest &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, SetSurfaceAlphaRequest &data);
const uint32_t SetSurfaceAlphaRequestCode = 0x05;

struct AddSurfaceDamageRequest
{
    std::vector<char> name;
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const AddSurfaceDamageRequest &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, AddSurfaceDamageRequest &data);
const uint32_t AddSurfaceDamageRequestCode = 0x06;

struct DisplaySizeNotification
{
    int32_t width;
    int32_t height;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const DisplaySizeNotification &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, DisplaySizeNotification &data);
const uint32_t DisplaySizeNotificationCode = 0x07;

struct PointerDownNotification
{
    std::vector<char> surface;
    int32_t slot;
    int32_t x;
    int32_t y;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const PointerDownNotification &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, PointerDownNotification &data);
const uint32_t PointerDownNotificationCode = 0x21;

struct PointerUpNotification
{
    std::vector<char> surface;
    int32_t slot;
    int32_t x;
    int32_t y;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const PointerUpNotification &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, PointerUpNotification &data);
const uint32_t PointerUpNotificationCode = 0x22;

struct PointerMotionNotification
{
    std::vector<char> surface;
    int32_t slot;
    int32_t x;
    int32_t y;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const PointerMotionNotification &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, PointerMotionNotification &data);
const uint32_t PointerMotionNotificationCode = 0x23;

struct KeyDownNotification
{
    int32_t code;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const KeyDownNotification &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, KeyDownNotification &data);
const uint32_t KeyDownNotificationCode = 0x24;

struct KeyUpNotification
{
    int32_t code;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const KeyUpNotification &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, KeyUpNotification &data);
const uint32_t KeyUpNotificationCode = 0x25;

/* ================================================================================================================== */

#endif /* SPARKLE_PROTOCOL_H */
