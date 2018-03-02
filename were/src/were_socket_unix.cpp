#include "were_socket_unix.h"
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>

//==================================================================================================

WereSocketUnix::~WereSocketUnix()
{
    disconnect();
}

WereSocketUnix::WereSocketUnix(WereEventLoop *loop) :
    WereEventSource(loop)
{
    _fd = -1;
    _state = UnconnectedState;
}

WereSocketUnix::WereSocketUnix(WereEventLoop *loop, int fd) :
    WereEventSource(loop)
{
    _fd = fd;
    _loop->registerEventSource(this, EPOLLIN | EPOLLET);
    _state = ConnectedState;
    
    setBlocking(false);
    
    were_debug("[%p][%s] Connected (accept).\n", this, __PRETTY_FUNCTION__);
}

//==================================================================================================

void WereSocketUnix::connect(const std::string &path)
{
    if (_state != UnconnectedState)
        return;

    //_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    _fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_fd == -1)
        throw WereException("[%p][%s] Failed to create socket.", this, __PRETTY_FUNCTION__);
    
    setBlocking(false);
    
    struct sockaddr_un name = {};
    memset(&name, 0, sizeof(name));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, path.c_str(), sizeof(name.sun_path) - 1);
    
    if (::connect(_fd, (const struct sockaddr *)&name, sizeof(struct sockaddr_un)) == -1)
    {
        //FIXME EINPROGRESS is OK
        //were_debug("[%p][%s] Connect error (%d, %s).\n", this, __PRETTY_FUNCTION__, errno, strerror(errno));
        disconnect();
        return;
    }
    
    _loop->registerEventSource(this, EPOLLIN | EPOLLOUT | EPOLLET);
    _state = ConnectingState;
}

void WereSocketUnix::disconnect()
{
    if (_state != UnconnectedState)
        _loop->unregisterEventSource(this);
    
    if (_fd != -1)
    {
        shutdown(_fd, SHUT_RDWR);
        close(_fd);
        _fd = -1;
    }
    
    if (_state == ConnectedState)
    {
        _state = UnconnectedState; 
        signal_disconnected();
        were_debug("[%p][%s] Disconnected.\n", this, __PRETTY_FUNCTION__);
    }
    
    _state = UnconnectedState; 
}

WereSocketUnix::SocketState WereSocketUnix::state()
{
    return _state;
}

//==================================================================================================

void WereSocketUnix::event(uint32_t events)
{
    if (!(events == EPOLLIN || events == EPOLLOUT || events == (EPOLLIN | EPOLLOUT)))
    {
        were_debug("[%p][%s] Unknown event type (%d), DISCONNECTING.\n", this, __PRETTY_FUNCTION__, events);
        disconnect();
        return;
    }
    
    if (events & EPOLLIN)
    {
        signal_data();
    }
    
    if (events & EPOLLOUT)
    {
        if (_state == ConnectingState)
        {
            int result;
            socklen_t resultLength = sizeof(result);
            
            if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, &result, &resultLength) == -1)
                throw WereException("[%p][%s] Failed to get connection result.", this, __PRETTY_FUNCTION__);
            
            if (result == 0)
            {
                _state = ConnectedState;
                signal_connected();
                were_debug("[%p][%s] Connected (connect).\n", this, __PRETTY_FUNCTION__);
            }
            else
            {
                disconnect();
                were_debug("[%p][%s] Connection failed (%d, %s).\n", this, __PRETTY_FUNCTION__, result, strerror(result));
            }
        }
    }
}

//==================================================================================================

int WereSocketUnix::send(const unsigned char *data, unsigned int size)
{
    if (_state != ConnectedState)
    {
        were_debug("[%p][%s] Not connected.", this, __PRETTY_FUNCTION__);
        return -1;
    }

    return ::send(_fd, data, size, MSG_NOSIGNAL);
}

int WereSocketUnix::receive(unsigned char *data, unsigned int size)
{
    if (_state != ConnectedState)
    {
        were_debug("[%p][%s] Not connected.", this, __PRETTY_FUNCTION__);
        return -1;
    }

    return read(_fd, data, size);
}

int WereSocketUnix::peek(unsigned char *data, unsigned int size)
{
    if (_state != ConnectedState)
    {
        were_debug("[%p][%s] Not connected.", this, __PRETTY_FUNCTION__);
        return -1;
    }
    
    return recv(_fd, data, size, MSG_PEEK);
}

unsigned int WereSocketUnix::bytesAvailable() const
{
    if (_state != ConnectedState)
        return 0;
    
    int bytes = 0;
    if (ioctl(_fd, FIONREAD, &bytes) == -1)
        throw WereException("[%p][%s] ioctl failed.", this, __PRETTY_FUNCTION__);
    
    //were_debug("Bytes available: %d\n", bytes);

    return bytes;
}

//==================================================================================================

