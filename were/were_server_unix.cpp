#include "were_server_unix.h"
#include <stdexcept>
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
    
    _fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (_fd == -1)
        throw std::runtime_error("[WereServer] Failed to create socket.");
    
    struct sockaddr_un name = {};
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, _path.c_str(), sizeof(name.sun_path) - 1);
    
    if (bind(_fd, (const struct sockaddr *)&name, sizeof(struct sockaddr_un)) == -1)
        throw std::runtime_error("[WereServer] Failed to bind socket.");

    if (listen(_fd, 4) == -1)
        throw std::runtime_error("[WereServer] Failed to listen socket.");
    
    
    _loop->registerEventSource(this, EPOLLIN | EPOLLET);
}

//==================================================================================================

void WereServerUnix::event(uint32_t events)
{
    if (events == EPOLLIN)
        newConnection();
}

//==================================================================================================

WereSocketUnix *WereServerUnix::accept()
{
    int fd = ::accept(_fd, NULL, NULL);
    if (fd == -1)
        throw std::runtime_error("[WereServer::accept] Failed to accept connection.");
    
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
    
    _server->newConnection.connect(_loop, std::bind(f, user));
}

were_socket_unix_t *were_server_unix_accept(were_server_unix_t *server)
{
    WereServerUnix *_server = static_cast<WereServerUnix *>(server);
    
    WereSocketUnix *_socket = _server->accept();
    
    return static_cast<were_socket_unix_t *>(_socket);
}

//==================================================================================================

