#include "were_socket_unix.h"
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>

/* ================================================================================================================== */

WereSocketUnixMessage::~WereSocketUnixMessage()
{
}

WereSocketUnixMessage::WereSocketUnixMessage()
{
}

/* ================================================================================================================== */

WereSocketUnix::~WereSocketUnix()
{
    disconnect();
}

WereSocketUnix::WereSocketUnix(WereEventLoop *loop) :
    WereEventSource(loop)
{
    _fd = -1;
    state_ = UnconnectedState;
}

WereSocketUnix::WereSocketUnix(WereEventLoop *loop, int fd) :
    WereEventSource(loop)
{
    _fd = fd;
    _loop->registerEventSource(this, EPOLLIN | EPOLLET);
    state_ = ConnectedState;

    setBlocking(false);

    were_debug("[%p][%s] Connected (accept).\n", this, __PRETTY_FUNCTION__);
}

/* ================================================================================================================== */

void WereSocketUnix::connect(const std::string &path)
{
    if (state_ != UnconnectedState)
        return;

    _fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (_fd == -1)
        throw WereException("[%p][%s] Failed to create socket.", this, __PRETTY_FUNCTION__);

    setBlocking(false);

    struct sockaddr_un name = {};
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, path.c_str(), sizeof(name.sun_path) - 1);

    if (::connect(_fd, (const struct sockaddr *)&name, sizeof(struct sockaddr_un)) == -1)
    {
        /* FIXME EINPROGRESS is OK */
        //were_debug("[%p][%s] Connect error (%d, %s).\n", this, __PRETTY_FUNCTION__, errno, strerror(errno));
        disconnect();
        return;
    }

    _loop->registerEventSource(this, EPOLLIN | EPOLLOUT | EPOLLET);
    state_ = ConnectingState;
}

void WereSocketUnix::disconnect()
{
    if (state_ != UnconnectedState)
        _loop->unregisterEventSource(this);

    if (_fd != -1)
    {
        shutdown(_fd, SHUT_RDWR);
        close(_fd);
        _fd = -1;
    }

    if (state_ == ConnectedState)
    {
        state_ = UnconnectedState;
        signal_disconnected();
        were_debug("[%p][%s] Disconnected.\n", this, __PRETTY_FUNCTION__);
    }

    state_ = UnconnectedState;
}

WereSocketUnix::SocketState WereSocketUnix::state()
{
    return state_;
}

/* ================================================================================================================== */

void WereSocketUnix::event(uint32_t events)
{
    if (!(events == EPOLLIN || events == EPOLLOUT || events == (EPOLLIN | EPOLLOUT))) /* FIXME */
    {
        were_debug("[%p][%s] Unknown event type (%d), DISCONNECTING.\n", this, __PRETTY_FUNCTION__, events);
        disconnect();
        return;
    }

    if (events & EPOLLIN)
    {
#if 0
        if (receiveMessage(message.get()) != -1)
            signal_message(message);
#else
        while (bytesAvailable() != 0)
        {
            std::shared_ptr<WereSocketUnixMessage> message(new WereSocketUnixMessage());

            if (receiveMessage(message.get()) != -1)
                signal_message(message);
        }
#endif
    }

    if (events & EPOLLOUT)
    {
        if (state_ == ConnectingState)
        {
            int result;
            socklen_t resultLength = sizeof(result);

            if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, &result, &resultLength) == -1)
                throw WereException("[%p][%s] Failed to get connection result.", this, __PRETTY_FUNCTION__);

            if (result == 0)
            {
                state_ = ConnectedState;
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

/* ================================================================================================================== */

#if 0

int WereSocketUnix::send(const unsigned char *data, unsigned int size)
{
    if (state_ != ConnectedState)
    {
        were_debug("[%p][%s] Not connected.", this, __PRETTY_FUNCTION__);
        return -1;
    }

    return ::send(_fd, data, size, MSG_NOSIGNAL);
}

int WereSocketUnix::receive(unsigned char *data, unsigned int size)
{
    if (state_ != ConnectedState)
    {
        were_debug("[%p][%s] Not connected.", this, __PRETTY_FUNCTION__);
        return -1;
    }

    return read(_fd, data, size);
}

int WereSocketUnix::peek(unsigned char *data, unsigned int size)
{
    if (state_ != ConnectedState)
    {
        were_debug("[%p][%s] Not connected.", this, __PRETTY_FUNCTION__);
        return -1;
    }

    return recv(_fd, data, size, MSG_PEEK);
}

#endif

unsigned int WereSocketUnix::bytesAvailable() const
{
    if (state_ != ConnectedState)
        return 0;

    int bytes = 0;
    if (ioctl(_fd, FIONREAD, &bytes) == -1)
        throw WereException("[%p][%s] ioctl failed.", this, __PRETTY_FUNCTION__);

    return bytes;
}

int WereSocketUnix::sendMessage(WereSocketUnixMessage *message)
{
    if (state_ != ConnectedState)
    {
        were_debug("[%p][%s] Not connected.\n", this, __PRETTY_FUNCTION__);
        return -1;
    }

    struct iovec iov[1];
    iov[0].iov_base = message->data()->data();
    iov[0].iov_len = message->data()->size();

    struct msghdr msg = {};
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    char *buffer = nullptr;

    if (message->fds()->size() > 0)
    {
        int bufferSize = CMSG_SPACE(message->fds()->size() * sizeof(int));

        buffer = new char[bufferSize];

        msg.msg_control = buffer;
        msg.msg_controllen = bufferSize;

        struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(message->fds()->size() * sizeof(int));
        int *fdptr = (int *)CMSG_DATA(cmsg);
        memcpy(fdptr, message->fds()->data(), message->fds()->size() * sizeof(int));
    }

    pthread_mutex_lock(&lock);
    int ret = sendmsg(_fd, &msg, 0);
    pthread_mutex_unlock(&lock);

    if (ret == -1)
    {
        were_debug("[%p][%s] Failed to send message, DISCONNECTING.\n", this, __PRETTY_FUNCTION__);
        disconnect();
    }

    if (buffer != nullptr)
        delete[] buffer;

    return ret;
}

int WereSocketUnix::receiveMessage(WereSocketUnixMessage *message)
{
    if (state_ != ConnectedState)
    {
        were_debug("[%p][%s] Not connected.\n", this, __PRETTY_FUNCTION__);
        return -1;
    }

    message->data()->resize(256);

    struct iovec iov[1];
    iov[0].iov_base = message->data()->data();
    iov[0].iov_len = message->data()->size();

    struct msghdr msg = {};
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    int bufferSize = CMSG_SPACE(4 * sizeof(int));
    char *buffer = new char[bufferSize];
    msg.msg_control = buffer;
    msg.msg_controllen = bufferSize;

    pthread_mutex_lock(&lock);
    int ret = recvmsg(_fd, &msg, 0);
    pthread_mutex_unlock(&lock);

    if (ret == -1)
    {
        were_debug("[%p][%s] Failed to receive message, DISCONNECTING.\n", this, __PRETTY_FUNCTION__);
        disconnect();
    }

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    if (cmsg != nullptr)
    {
        int *fdptr = (int *)CMSG_DATA(cmsg);
        int nfds = cmsg->cmsg_len - sizeof(struct cmsghdr); /* XXX */

        for (int i = 0; i < nfds; ++i)
            message->fds()->push_back(fdptr[i]);
    }

    if (buffer != nullptr)
        delete[] buffer;

    return ret;
}

/* ================================================================================================================== */
