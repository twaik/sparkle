#ifndef WERE_SIGNAL_HANDLER_H
#define WERE_SIGNAL_HANDLER_H

#include "were.h"
#include "were_event_source.h"
#include "were_signal.h"

/* ================================================================================================================== */

class WereSignalHandler : public WereEventSource
{
public:
    ~WereSignalHandler();
    WereSignalHandler(WereEventLoop *loop);

werethings:
    WereSignal<void ()> terminate;

private:
    void event(uint32_t events);
};

/* ================================================================================================================== */

#endif /* WERE_SIGNAL_HANDLER_H */
