#include "sparkle_protocol.h"

/* ================================================================================================================== */

#if 0
#if 0
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const RegisterSurfaceFdRequest &data)
{
    stream << RegisterSurfaceFdRequestCode;
    stream << data.name;
    stream.writeFD(data.fd);
    stream << data.width;
    stream << data.height;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, RegisterSurfaceFdRequest &data)
{
    stream >> data.name;
    stream.readFD(&data.fd);
    stream >> data.width;
    stream >> data.height;
    return stream;
}
#else
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const RegisterSurfaceShmRequest &data)
{
    stream << RegisterSurfaceShmRequestCode;
    stream << data.name;
    stream << data.key;
    stream << data.width;
    stream << data.height;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, RegisterSurfaceShmRequest &data)
{
    stream >> data.name;
    stream >> data.key;
    stream >> data.width;
    stream >> data.height;
    return stream;
}
#endif
#else
WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const RegisterSurfaceAshmemRequest &data)
{
    stream << RegisterSurfaceAshmemRequestCode;
    stream << data.name;
    stream.writeFD(data.fd);
    stream << data.width;
    stream << data.height;
    return stream;
}
WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, RegisterSurfaceAshmemRequest &data)
{
    stream >> data.name;
    stream.readFD(&data.fd);
    stream >> data.width;
    stream >> data.height;
    return stream;
}
#endif

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const UnregisterSurfaceRequest &data)
{
    stream << UnregisterSurfaceRequestCode;
    stream << data.name;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, UnregisterSurfaceRequest &data)
{
    stream >> data.name;
    return stream;
}

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const SetSurfacePositionRequest &data)
{
    stream << SetSurfacePositionRequestCode;
    stream << data.name;
    stream << data.x1;
    stream << data.y1;
    stream << data.x2;
    stream << data.y2;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, SetSurfacePositionRequest &data)
{
    stream >> data.name;
    stream >> data.x1;
    stream >> data.y1;
    stream >> data.x2;
    stream >> data.y2;
    return stream;
}

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const SetSurfaceStrataRequest &data)
{
    stream << SetSurfaceStrataRequestCode;
    stream << data.name;
    stream << data.strata;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, SetSurfaceStrataRequest &data)
{
    stream >> data.name;
    stream >> data.strata;
    return stream;
}

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const SetSurfaceAlphaRequest &data)
{
    stream << SetSurfaceAlphaRequestCode;
    stream << data.name;
    stream << data.alpha;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, SetSurfaceAlphaRequest &data)
{
    stream >> data.name;
    stream >> data.alpha;
    return stream;
}

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const AddSurfaceDamageRequest &data)
{
    stream << AddSurfaceDamageRequestCode;
    stream << data.name;
    stream << data.x1;
    stream << data.y1;
    stream << data.x2;
    stream << data.y2;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, AddSurfaceDamageRequest &data)
{
    stream >> data.name;
    stream >> data.x1;
    stream >> data.y1;
    stream >> data.x2;
    stream >> data.y2;
    return stream;
}

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const DisplaySizeNotification &data)
{
    stream << DisplaySizeNotificationCode;
    stream << data.width;
    stream << data.height;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, DisplaySizeNotification &data)
{
    stream >> data.width;
    stream >> data.height;
    return stream;
}

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const PointerDownNotification &data)
{
    stream << PointerDownNotificationCode;
    stream << data.surface;
    stream << data.slot;
    stream << data.x;
    stream << data.y;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, PointerDownNotification &data)
{
    stream >> data.surface;
    stream >> data.slot;
    stream >> data.x;
    stream >> data.y;
    return stream;
}

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const PointerUpNotification &data)
{
    stream << PointerUpNotificationCode;
    stream << data.surface;
    stream << data.slot;
    stream << data.x;
    stream << data.y;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, PointerUpNotification &data)
{
    stream >> data.surface;
    stream >> data.slot;
    stream >> data.x;
    stream >> data.y;
    return stream;
}

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const PointerMotionNotification &data)
{
    stream << PointerMotionNotificationCode;
    stream << data.surface;
    stream << data.slot;
    stream << data.x;
    stream << data.y;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, PointerMotionNotification &data)
{
    stream >> data.surface;
    stream >> data.slot;
    stream >> data.x;
    stream >> data.y;
    return stream;
}

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const KeyDownNotification &data)
{
    stream << KeyDownNotificationCode;
    stream << data.code;
    return stream;
};

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, KeyDownNotification &data)
{
    stream >> data.code;
    return stream;
};

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const KeyUpNotification &data)
{
    stream << KeyUpNotificationCode;
    stream << data.code;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, KeyUpNotification &data)
{
    stream >> data.code;
    return stream;
}

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const ButtonPressNotification &data)
{
    stream << ButtonPressNotificationCode;
    stream << data.surface;
    stream << data.button;
    stream << data.x;
    stream << data.y;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, ButtonPressNotification &data)
{
    stream >> data.surface;
    stream >> data.button;
    stream >> data.x;
    stream >> data.y;
    return stream;
}

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const ButtonReleaseNotification &data)
{
    stream << ButtonReleaseNotificationCode;
    stream << data.surface;
    stream << data.button;
    stream << data.x;
    stream << data.y;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, ButtonReleaseNotification &data)
{
    stream >> data.surface;
    stream >> data.button;
    stream >> data.x;
    stream >> data.y;
    return stream;
}

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const CursorMotionNotification &data)
{
    stream << CursorMotionNotificationCode;
    stream << data.surface;
    stream << data.x;
    stream << data.y;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, CursorMotionNotification &data)
{
    stream >> data.surface;
    stream >> data.x;
    stream >> data.y;
    return stream;
}

/* ================================================================================================================== */

WereSocketUnixMessageStream &operator<<(WereSocketUnixMessageStream &stream, const RegisterSoundBufferRequest &data)
{
    stream << RegisterSoundBufferRequestCode;
    stream << data.key;
    stream << data.size;
    return stream;
}

WereSocketUnixMessageStream &operator>>(WereSocketUnixMessageStream &stream, RegisterSoundBufferRequest &data)
{
    stream >> data.key;
    stream >> data.size;
    return stream;
}

/* ================================================================================================================== */
