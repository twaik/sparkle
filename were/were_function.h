#ifndef WERE_FUNCTION_H
#define WERE_FUNCTION_H

#include "were_event_loop.h"
#include <stdexcept>
#include <functional>
#include <vector>

//==================================================================================================

#define werethings public

//==================================================================================================

template <typename Signature> class WereFunction;
template <typename R, typename ... Args>
class WereFunction<R (Args ...)>
{
public:
    
    R operator()(Args ... args)
    {
        if (!_f)
            throw std::runtime_error("[WereFunction::operator()] Not implemented.");

        return _f(args ...);
    }

    void connect(const std::function<R (Args ...)> &f)
    {
        _f = f;
    }

    bool connected()
    {
        return _f != nullptr;
    }

private:
    std::function<R (Args ...)> _f;
};

//==================================================================================================

template <typename Signature> class WereCall;
template <typename ... Args>
class WereCall<void (Args ...)>
{
public:
    WereEventLoop *loop;
    std::function<void (Args ...)> f;
};

//==================================================================================================

template <typename Signature> class WereSignal;
template <typename ... Args>
class WereSignal<void (Args ...)>
{
public:

    void operator()(Args ... args)
    {
        for (unsigned int i = 0; i < _f.size(); ++i)
        {
            _f[i].loop->queue(std::bind(_f[i].f, args ...));
        }
    }

    void connect(WereEventLoop *loop, const std::function<void (Args ...)> &f)
    {
        WereCall<void (Args ...)> call;
        call.loop = loop;
        call.f = f;
        _f.push_back(call);
    }

    bool connected()
    {
        return _f.size() > 0;
    }

private:
    std::vector< WereCall<void (Args ...)> > _f;
};

//==================================================================================================

#endif //WERE_FUNCTION_H

