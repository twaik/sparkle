#ifndef SPARKLE_PROTOCOL_H
#define SPARKLE_PROTOCOL_H

#include "were/were_socket_unix_message_stream.h"

/* ================================================================================================================== */

#if 0
struct RegisterSurfaceFdRequest
{
    std::string name;
    int fd;
    int32_t width;
    int32_t height;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const RegisterSurfaceFdRequest &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, RegisterSurfaceFdRequest &data);
const uint32_t RegisterSurfaceFdRequestCode = 0x01;
#else
struct RegisterSurfaceShmRequest
{
    std::string name;
    key_t key;
    int32_t width;
    int32_t height;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const RegisterSurfaceShmRequest &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, RegisterSurfaceShmRequest &data);
const uint32_t RegisterSurfaceShmRequestCode = 0x01;
#endif

struct UnregisterSurfaceRequest
{
    std::string name;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const UnregisterSurfaceRequest &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, UnregisterSurfaceRequest &data);
const uint32_t UnregisterSurfaceRequestCode = 0x02;

struct SetSurfacePositionRequest
{
    std::string name;
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
    std::string name;
    int32_t strata;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const SetSurfaceStrataRequest &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, SetSurfaceStrataRequest &data);
const uint32_t SetSurfaceStrataRequestCode = 0x04;

struct SetSurfaceAlphaRequest
{
    std::string name;
    int32_t alpha;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const SetSurfaceAlphaRequest &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, SetSurfaceAlphaRequest &data);
const uint32_t SetSurfaceAlphaRequestCode = 0x05;

struct AddSurfaceDamageRequest
{
    std::string name;
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
    std::string surface;
    int32_t slot;
    int32_t x;
    int32_t y;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const PointerDownNotification &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, PointerDownNotification &data);
const uint32_t PointerDownNotificationCode = 0x21;

struct PointerUpNotification
{
    std::string surface;
    int32_t slot;
    int32_t x;
    int32_t y;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const PointerUpNotification &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, PointerUpNotification &data);
const uint32_t PointerUpNotificationCode = 0x22;

struct PointerMotionNotification
{
    std::string surface;
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

struct ButtonPressNotification
{
    std::string surface;
    int32_t button;
    int32_t x;
    int32_t y;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const ButtonPressNotification &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, ButtonPressNotification &data);
const uint32_t ButtonPressNotificationCode = 0x26;

struct ButtonReleaseNotification
{
    std::string surface;
    int32_t button;
    int32_t x;
    int32_t y;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const ButtonReleaseNotification &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, ButtonReleaseNotification &data);
const uint32_t ButtonReleaseNotificationCode = 0x27;

struct CursorMotionNotification
{
    std::string surface;
    int32_t x;
    int32_t y;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const CursorMotionNotification &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, CursorMotionNotification &data);
const uint32_t CursorMotionNotificationCode = 0x28;

struct RegisterSoundBufferRequest
{
    key_t key;
    int32_t size;
};
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const RegisterSoundBufferRequest &data);
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, RegisterSoundBufferRequest &data);
const uint32_t RegisterSoundBufferRequestCode = 0x31;

const uint32_t UnregisterSoundBufferRequestCode = 0x32;
const uint32_t SoundStartCode = 0x33;
const uint32_t SoundStopCode = 0x34;

/* ================================================================================================================== */

#endif /* SPARKLE_PROTOCOL_H */
