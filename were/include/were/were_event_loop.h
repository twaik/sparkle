#ifndef WERE_EVENT_LOOP_H
#define WERE_EVENT_LOOP_H

//==================================================================================================

#ifdef __cplusplus

#include "were.h"
#include <sys/epoll.h>
#include <functional>
#include <vector>
#include <thread>

class WereEventSource;
class WereCallQueue;

class WereEventLoop
{
public:
    ~WereEventLoop();
    WereEventLoop();
    
    int fd();
    
    void registerEventSource(WereEventSource *source, uint32_t events);
    void unregisterEventSource(WereEventSource *source);
    
    void run();
    void runThread();
    void exit();
    
    void processEvents();
    
    void queue(const std::function<void ()> &f);

private:
    
private:
    int _epoll;
    bool _exit;
    
    WereCallQueue *_queue;
    
    std::thread _thread;
};

#else

struct WereEventLoop;
typedef struct WereEventLoop WereEventLoop;

#endif //__cplusplus

//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif
    
WereEventLoop *were_event_loop_create();
void were_event_loop_destroy(WereEventLoop *loop);

int were_event_loop_fd(WereEventLoop *loop);
void were_event_loop_process_events(WereEventLoop *loop);

#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //WERE_EVENT_LOOP_H

