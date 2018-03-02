#ifndef WERE_SOCKET_UNIX_H
#define WERE_SOCKET_UNIX_H

#include "were.h"
#include "were_event_source.h"
#include "were_signal.h"
#include <string>


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
    
    int send(const unsigned char *data, unsigned int size);
    int receive(unsigned char *data, unsigned int size);
    int peek(unsigned char *data, unsigned int size);
    
    unsigned int bytesAvailable() const;
    
werethings:
    WereSignal<void ()> signal_connected;
    WereSignal<void ()> signal_disconnected;
    WereSignal<void ()> signal_data;
    
private:
    void event(uint32_t events);
    
private:
    SocketState _state;
};


#endif //WERE_SOCKET_UNIX_H

