#ifndef WERE_SIGNAL_H
#define WERE_SIGNAL_H

/*
    signal.connect([loop, w](int x, int y)
    {
        loop->queue(cb, w.lock(), x, y);
    });

    signal.connect([loop](int x, int y)
    {
        loop->queue(x, y);
    });

    signal.connect(WereSimpleQueuer(&Object::method, object, x, y));

    signal.connect([this, client](std::shared_ptr<SparklePacket> packet)
    {
        _loop->queue(std::bind(&SparkleServer::handlePacket, this, client, packet));
    });
*/

#include "were_event_loop.h"

#include <vector>
#include <functional>

/* ================================================================================================================== */

template <typename Signature> class WereSignal;
template <typename ... Args>
class WereSignal<void (Args ...)>
{
public:

    void operator()(Args ... args)
    {
        for (auto it = _f.begin(); it != _f.end(); ++it)
            (*it)(args ...);
    }

    void connect(const std::function<void (Args ...)> &f)
    {
        _f.push_back(f);
    }

private:
    std::vector< std::function<void (Args ...)> > _f;
};

/* ================================================================================================================== */

template <typename ... Args>
std::function<void (Args ...)> WereSimpleQueuer(WereEventLoop *loop, void (*f)(Args ... args))
{
    return [loop, f](Args ... args)
    {
        loop->queue(std::bind(f, args...));
    };
}

template <typename ... Args, typename T>
std::function<void (Args ...)> WereSimpleQueuer(WereEventLoop *loop, void (T::*f)(Args ... args), T *o)
{
    return [loop, f, o](Args ... args)
    {
        loop->queue(std::bind(f, o, args...));
    };
}

/* ================================================================================================================== */

#endif /* WERE_SIGNAL_H */
