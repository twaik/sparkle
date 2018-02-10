#ifndef WERE_EVENT_LOOP_H
#define WERE_EVENT_LOOP_H

//==================================================================================================

#ifdef __cplusplus

#include "were.h"
#include <sys/epoll.h>
#include <functional>
#include <vector>

class WereEventSource;
class WereCallQueue;
class WereSignalHandler;

class WereEventLoop
{
public:
    ~WereEventLoop();
    WereEventLoop(bool handleSignals = false);
    
    int fd();
    
    void registerEventSource(WereEventSource *source, uint32_t events);
    void unregisterEventSource(WereEventSource *source);
    
    void run();
    void exit();
    
    void processEvents();
    
    void queue(const std::function<void ()> &f);

private:
    
private:
    int _epoll;
    bool _exit;
    
    WereCallQueue *_queue;
    WereSignalHandler *_signal;
};

#endif //__cplusplus

//==================================================================================================

typedef void were_event_loop_t;

#ifdef __cplusplus
extern "C" {
#endif
    
were_event_loop_t *were_event_loop_create();
void were_event_loop_destroy(were_event_loop_t *loop);

int were_event_loop_fd(were_event_loop_t *loop);
void were_event_loop_process_events(were_event_loop_t *loop);

#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //WERE_EVENT_LOOP_H

