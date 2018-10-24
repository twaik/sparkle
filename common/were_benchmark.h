#ifndef WERE_BENCHMARK_H
#define WERE_BENCHMARK_H

/* ================================================================================================================== */

#include "were/were.h"
#include <ctime>

class WereEventLoop;
class WereTimer;

class WereBenchmark
{
public:
    ~WereBenchmark();
    WereBenchmark(WereEventLoop *loop);

    void event();

private:
    void timeout();

private:
    WereEventLoop *_loop;
    WereTimer *_timer;

    struct timespec _real1, _real2;
    struct timespec _cpu1, _cpu2;
    int _events;
};

/* ================================================================================================================== */

#endif /* WERE_BENCHMARK_H */
