#ifndef WERE_SERVER_UNIX_H
#define WERE_SERVER_UNIX_H

#include "were.h"
#include "were_event_source.h"
#include "were_socket_unix.h"
#include "were_signal.h"
#include <string>

//==================================================================================================

class WereServerUnix : public WereEventSource
{
public:
    ~WereServerUnix();
    WereServerUnix(WereEventLoop *loop, const std::string &path);
    
    WereSocketUnix *accept();
    
werethings:
    WereSignal<void ()> newConnection;
    
private:
    void event(uint32_t events);
    
private:
    std::string _path;
};

//==================================================================================================

#endif //WERE_SERVER_UNIX_H

