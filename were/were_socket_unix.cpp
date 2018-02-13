#include "were_socket_unix.h"
#include <stdexcept>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/ioctl.h>

//#define NONBLOCK

//==================================================================================================

WereSocketUnix::~WereSocketUnix()
{
    disconnect();
}

WereSocketUnix::WereSocketUnix(WereEventLoop *loop) :
    WereEventSource(loop)
{
    _connected = false;
}

WereSocketUnix::WereSocketUnix(WereEventLoop *loop, int fd) :
    WereEventSource(loop)
{
    _fd = fd;
    
#ifdef NONBLOCK
    int flags = fcntl(_fd, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error("[WereSocketUnix::WereSocketUnix] Failed to get fd flags.");
    flags |= O_NONBLOCK;
    if (fcntl(_fd, F_SETFL, flags) == -1)
        throw std::runtime_error("[WereSocketUnix::WereSocketUnix] Failed to set fd flags.");
#endif
    
    _connected = true;
    
    _loop->registerEventSource(this, EPOLLIN | EPOLLET);
}

//==================================================================================================

void WereSocketUnix::connect(const std::string &path)
{
    if (_connected)
        return;

    //_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    _fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_fd == -1)
        throw std::runtime_error("[WereSocketUnix::connect] Failed to create socket.");
    
    struct sockaddr_un name = {};
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, path.c_str(), sizeof(name.sun_path) - 1);
    
    if (::connect(_fd, (const struct sockaddr *)&name, sizeof(struct sockaddr_un)) == -1)
    {
        close(_fd);
        return;
    }
    
#ifdef NONBLOCK
    int flags = fcntl(_fd, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error("[WereSocketUnix::WereSocketUnix] Failed to get fd flags.");
    flags |= O_NONBLOCK;
    if (fcntl(_fd, F_SETFL, flags) == -1)
        throw std::runtime_error("[WereSocketUnix::WereSocketUnix] Failed to set fd flags.");
#endif   
    
    _connected = true;
    
    _loop->registerEventSource(this, EPOLLIN | EPOLLET);
    
    signal_connected();
}

void WereSocketUnix::disconnect()
{
    if (!_connected)
        return;
    
    _connected = false;
    
    _loop->unregisterEventSource(this);
    
    shutdown(_fd, SHUT_RDWR);
    close(_fd);
}

bool WereSocketUnix::connected()
{
    return _connected;
}

//==================================================================================================

void WereSocketUnix::event(uint32_t events)
{
    if (events == EPOLLIN)
        signal_data();
    else
    {
        signal_disconnected();
        disconnect();
    }
}

//==================================================================================================

int WereSocketUnix::send(void *data, int size)
{
    if (!_connected)
        return -1;

    int n = write(_fd, data, size);
    if (n == -1)
        throw WereException("[WereSocketUnix::send] Socket error (%s).", strerror(errno));
    else if (n != size)
        throw WereException("[WereSocketUnix::send n != size (%d, %d).", n, size);
    else
        return n;
}

int WereSocketUnix::receive(void *data, int size)
{
    if (!_connected)
        return -1;

    int n = read(_fd, data, size);
    if (n == -1)
        throw WereException("[WereSocketUnix::receive] Socket error (%s).", strerror(errno));
    else if (n != size)
        throw WereException("[WereSocketUnix::receive] n != size (%d, %d).", n, size);
    else
        return n;
}

int WereSocketUnix::peek(void *data, int size)
{
    if (!_connected)
        return -1;
    
    int n = recv(_fd, data, size, MSG_PEEK);
    if (n == -1)
        throw WereException("[WereSocketUnix::peek] Socket error (%s).", strerror(errno));
    else if (n != size)
        throw WereException("[WereSocketUnix::peek] n != size (%d, %d).", n, size);
    else
        return n;
}

int WereSocketUnix::bytesAvailable()
{
    if (!_connected)
        return -1;
    
    int bytes = 0;
    if (ioctl(_fd, FIONREAD, &bytes) == -1)
        throw std::runtime_error("[WereSocketUnix::bytesAvailable] ioctl failed.");
    
    //were_debug("Bytes available: %d\n", bytes);

    return bytes;
}

//==================================================================================================

were_socket_unix_t *were_socket_unix_create(were_event_loop_t *loop)
{
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);
    WereSocketUnix *_socket = new WereSocketUnix(_loop);
    return static_cast<were_socket_unix_t *>(_socket);
}

void were_socket_unix_destroy(were_socket_unix_t *socket)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    delete _socket;
}

void were_socket_unix_connect(were_socket_unix_t *socket, const char *path)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    _socket->connect(path);
}

void were_socket_unix_disconnect(were_socket_unix_t *socket)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    _socket->disconnect();
}

int were_socket_unix_connected(were_socket_unix_t *socket)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    return _socket->connected();
}

int were_socket_unix_send(were_socket_unix_t *socket, void *data, int size)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    return _socket->send(data, size);
}

int were_socket_unix_receive(were_socket_unix_t *socket, void *data, int size)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    return _socket->receive(data, size);
}

int were_socket_unix_peek(were_socket_unix_t *socket, void *data, int size)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    return _socket->peek(data, size);
}

int were_socket_unix_bytes_available(were_socket_unix_t *socket)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    return _socket->bytesAvailable();
}

void were_socket_unix_set_connection_callback(were_socket_unix_t *socket, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);
    _socket->signal_connected.connect(_loop, std::bind(f, user));
}

void were_socket_unix_set_disconnection_callback(were_socket_unix_t *socket, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);
    _socket->signal_disconnected.connect(_loop, std::bind(f, user));
}

void were_socket_unix_set_data_callback(were_socket_unix_t *socket, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);
    _socket->signal_data.connect(_loop, std::bind(f, user));
}

//==================================================================================================

