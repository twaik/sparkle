#include "were_socket_unix.h"
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define NONBLOCK

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
        throw WereException("[%p][%s] Failed to get fd flags.", this, __PRETTY_FUNCTION__);
    flags |= O_NONBLOCK;
    if (fcntl(_fd, F_SETFL, flags) == -1)
        throw WereException("[%p][%s] Failed to set fd flags.", this, __PRETTY_FUNCTION__);
#endif
    
    _connected = true;
    
    _loop->registerEventSource(this, EPOLLIN | EPOLLET);

    were_debug("[%p][%s] Connected (accept).\n", this, __PRETTY_FUNCTION__);
}

//==================================================================================================

bool WereSocketUnix::connect(const std::string &path)
{
    if (_connected)
        return true;

    //_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    _fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_fd == -1)
        throw WereException("[%p][%s] Failed to create socket.", this, __PRETTY_FUNCTION__);
    
    struct sockaddr_un name = {};
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, path.c_str(), sizeof(name.sun_path) - 1);
    
    if (::connect(_fd, (const struct sockaddr *)&name, sizeof(struct sockaddr_un)) == -1)
    {
        close(_fd);
        return false;
    }
    
#ifdef NONBLOCK
    int flags = fcntl(_fd, F_GETFL, 0);
    if (flags == -1)
        throw WereException("[%p][%s] Failed to get fd flags.", this, __PRETTY_FUNCTION__);
    flags |= O_NONBLOCK;
    if (fcntl(_fd, F_SETFL, flags) == -1)
        throw WereException("[%p][%s] Failed to set fd flags.", this, __PRETTY_FUNCTION__);
#endif   
    
    _connected = true;
    
    _loop->registerEventSource(this, EPOLLIN | EPOLLET);
    
    signal_connected();
    
    were_debug("[%p][%s] Connected (connect).\n", this, __PRETTY_FUNCTION__);
    
    return true;
}

void WereSocketUnix::disconnect()
{
    if (!_connected)
        return;
    
    _connected = false;
    
    _loop->unregisterEventSource(this);
    
    shutdown(_fd, SHUT_RDWR);
    close(_fd);
    
    were_debug("[%p][%s] Disconnected.\n", this, __PRETTY_FUNCTION__);
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

int WereSocketUnix::send(const unsigned char *data, unsigned int size)
{
    if (!_connected)
    {
        were_debug("[%p][%s] Not connected.", this, __PRETTY_FUNCTION__);
        return -1;
    }

    return write(_fd, data, size);
}

int WereSocketUnix::receive(unsigned char *data, unsigned int size)
{
    if (!_connected)
    {
        were_debug("[%p][%s] Not connected.", this, __PRETTY_FUNCTION__);
        return -1;
    }

    return read(_fd, data, size);
}

int WereSocketUnix::peek(unsigned char *data, unsigned int size)
{
    if (!_connected)
    {
        were_debug("[%p][%s] Not connected.", this, __PRETTY_FUNCTION__);
        return -1;
    }
    
    return recv(_fd, data, size, MSG_PEEK);
}

unsigned int WereSocketUnix::bytesAvailable() const
{
    if (!_connected)
        return -1;
    
    int bytes = 0;
    if (ioctl(_fd, FIONREAD, &bytes) == -1)
        throw WereException("[%p][%s] ioctl failed.", this, __PRETTY_FUNCTION__);
    
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

int were_socket_unix_send(were_socket_unix_t *socket, const unsigned char *data, unsigned int size)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    return _socket->send(data, size);
}

int were_socket_unix_receive(were_socket_unix_t *socket, unsigned char *data, unsigned int size)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    return _socket->receive(data, size);
}

int were_socket_unix_peek(were_socket_unix_t *socket, unsigned char *data, unsigned int size)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    return _socket->peek(data, size);
}

unsigned int were_socket_unix_bytes_available(were_socket_unix_t *socket)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    return _socket->bytesAvailable();
}

void were_socket_unix_set_connection_callback(were_socket_unix_t *socket, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);

    _socket->signal_connected.connect([_loop, f, user]()
    {
        _loop->queue(std::bind(f, user));
    });
}

void were_socket_unix_set_disconnection_callback(were_socket_unix_t *socket, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);

    _socket->signal_disconnected.connect([_loop, f, user]()
    {
        _loop->queue(std::bind(f, user));
    });
}

void were_socket_unix_set_data_callback(were_socket_unix_t *socket, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    WereSocketUnix *_socket = static_cast<WereSocketUnix *>(socket);
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);

    _socket->signal_data.connect([_loop, f, user]()
    {
        _loop->queue(std::bind(f, user));
    });
}

//==================================================================================================

