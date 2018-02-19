#include "were_server_unix.h"
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/ioctl.h>

//==================================================================================================

void make_nonblock(int fd);

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
    
    make_nonblock(_fd);
    
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
        return 0;
    
    make_nonblock(_fd);
    
    WereSocketUnix *socket = new WereSocketUnix(_loop, fd);
    
    return socket;
}

//==================================================================================================

