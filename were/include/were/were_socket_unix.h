#ifndef WERE_SOCKET_UNIX_H
#define WERE_SOCKET_UNIX_H

#include "were.h"
#include "were_event_source.h"
#include "were_signal.h"
#include <string>

/* ================================================================================================================== */

class WereSocketUnixMessage
{
public:
    ~WereSocketUnixMessage();
    WereSocketUnixMessage();

    std::vector<unsigned char> *data() {return &data_;}
    std::vector<int> *fds() {return &fds_;}

private:
    std::vector<unsigned char> data_;
    std::vector<int> fds_;
};

/* ================================================================================================================== */

class WereSocketUnix : public WereEventSource
{
public:
    enum SocketState {
        UnconnectedState,
        ConnectingState,
        ConnectedState,
    };
public:
    ~WereSocketUnix();
    WereSocketUnix(WereEventLoop *loop);
    WereSocketUnix(WereEventLoop *loop, int fd);

    void connect(const std::string &path);
    void disconnect();

    WereSocketUnix::SocketState state();

#if 0
    int send(const unsigned char *data, unsigned int size);
    int receive(unsigned char *data, unsigned int size);
    int peek(unsigned char *data, unsigned int size);
#endif

    unsigned int bytesAvailable() const;

    int sendMessage(WereSocketUnixMessage *message);
    int receiveMessage(WereSocketUnixMessage *message);

werethings:
    WereSignal<void ()> signal_connected;
    WereSignal<void ()> signal_disconnected;
#if 0
    WereSignal<void ()> signal_data;
#endif
    WereSignal<void (std::shared_ptr<WereSocketUnixMessage> message)> signal_message;

private:
    void event(uint32_t events);

private:
    SocketState state_;
};

/* ================================================================================================================== */

#endif /* WERE_SOCKET_UNIX_H */
