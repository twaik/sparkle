#ifndef WERE_SERVER_UNIX_H
#define WERE_SERVER_UNIX_H

#include "were.h"
#include "were_socket_unix.h"

//==================================================================================================

#ifdef __cplusplus

#include "were_event_source.h"
#include "were_function.h"

#include <string>

class WereServerUnix : public WereEventSource
{
public:
    ~WereServerUnix();
    WereServerUnix(WereEventLoop *loop, const std::string &path);
    
    WereSocketUnix *acceptConnection();
    
werethings:
    WereSignal<void ()> newConnection;
    
private:
    void event(uint32_t events);
    
private:
    std::string _path;
};

#endif //__cplusplus

typedef void were_server_unix_t;

#ifdef __cplusplus
extern "C" {
#endif

were_server_unix_t *were_server_unix_create(were_event_loop_t *loop, const char *path);
void were_server_unix_destroy(were_server_unix_t *server);

were_socket_unix_t *were_server_unix_accept(were_server_unix_t *server);

void were_server_unix_set_connection_callback(were_server_unix_t *server, were_event_loop_t *loop, void (*f)(void *user), void *user);


#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //WERE_SERVER_UNIX_H

