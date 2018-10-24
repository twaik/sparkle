#include "were_benchmark.h"
#include "were/were_timer.h"
#include <unistd.h>

/* ================================================================================================================== */

WereBenchmark::~WereBenchmark()
{
    delete _timer;
}

WereBenchmark::WereBenchmark(WereEventLoop *loop)
{
    _loop = loop;
    _events = 0;

    _timer = new WereTimer(_loop);
    _timer->timeout.connect(WereSimpleQueuer(loop, &WereBenchmark::timeout, this));

    clock_gettime(CLOCK_MONOTONIC, &_real1);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &_cpu1);

    _timer->start(5000, false);
}

void WereBenchmark::timeout()
{
    clock_gettime(CLOCK_MONOTONIC, &_real2);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &_cpu2);

    uint64_t elapsed_real = 0;
    elapsed_real += 1000000000LL * (_real2.tv_sec - _real1.tv_sec);
    elapsed_real += _real2.tv_nsec - _real1.tv_nsec;

    uint64_t elapsed_cpu = 0;
    elapsed_cpu += 1000000000LL * (_cpu2.tv_sec - _cpu1.tv_sec);
    elapsed_cpu += _cpu2.tv_nsec - _cpu1.tv_nsec;

    float loopPerformance = 1.0 * (elapsed_real / 1000000) / 5000;
    float cpuLoad = 100.0 * (elapsed_cpu / 1000) / (elapsed_real / 1000);


    were_message("Loop: %.2f, CPU: %.2f, Events: %d.\n", loopPerformance, cpuLoad, _events / 5);

    _real1 = _real2;
    _cpu1 = _cpu2;
    _events = 0;
}

void WereBenchmark::event()
{
    _events += 1;
}

/* ================================================================================================================== */
