#include "were_server_unix.h"
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

//==================================================================================================

WereServerUnix::~WereServerUnix()
{
    _loop->unregisterEventSource(this);
    
    shutdown(_fd, SHUT_RDWR);
    close(_fd);
    
    unlink(_path.c_str());
}

WereServerUnix::WereServerUnix(WereEventLoop *loop, const std::string &path) :
    WereEventSource(loop)
{
    _path = path;
    
    unlink(_path.c_str());
    
    //_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    _fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_fd == -1)
        throw WereException("[%p][%s] Failed to create socket.", this, __PRETTY_FUNCTION__);
    
    struct sockaddr_un name = {};
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, _path.c_str(), sizeof(name.sun_path) - 1);
    
    if (bind(_fd, (const struct sockaddr *)&name, sizeof(struct sockaddr_un)) == -1)
        throw WereException("[%p][%s] Failed to bind socket (%s).", this, __PRETTY_FUNCTION__, strerror(errno));

    if (listen(_fd, 4) == -1)
        throw WereException("[%p][%s] Failed to listen socket.", this, __PRETTY_FUNCTION__);
    
    
    _loop->registerEventSource(this, EPOLLIN | EPOLLET);
}

//==================================================================================================

void WereServerUnix::event(uint32_t events)
{
    if (events == EPOLLIN)
        newConnection();
    else
        throw WereException("[%p][%s] Unknown event type.", this, __PRETTY_FUNCTION__);
}

//==================================================================================================

WereSocketUnix *WereServerUnix::accept()
{
    int fd = ::accept(_fd, NULL, NULL);
    if (fd == -1)
        throw WereException("[%p][%s] Failed to accept connection.", this, __PRETTY_FUNCTION__);
    
    WereSocketUnix *socket = new WereSocketUnix(_loop, fd);
    
    return socket;
}

//==================================================================================================

were_server_unix_t *were_server_unix_create(were_event_loop_t *loop, const char *path)
{
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);
    WereServerUnix *_server = new WereServerUnix(_loop, path);
    return static_cast<were_socket_unix_t *>(_server);
}

void were_server_unix_destroy(were_socket_unix_t *server)
{
    WereServerUnix *_server = static_cast<WereServerUnix *>(server);
    delete _server;
}

void were_server_unix_set_connection_callback(were_server_unix_t *server, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    WereServerUnix *_server = static_cast<WereServerUnix *>(server);
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);

    _server->newConnection.connect([_loop, f, user]()
    {
        _loop->queue(std::bind(f, user));
    });
}

were_socket_unix_t *were_server_unix_accept(were_server_unix_t *server)
{
    WereServerUnix *_server = static_cast<WereServerUnix *>(server);
    WereSocketUnix *_socket = _server->accept();
    return static_cast<were_socket_unix_t *>(_socket);
}

//==================================================================================================

