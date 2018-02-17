#ifndef WERE_FUNCTION_H
#define WERE_FUNCTION_H

#include <functional>

//==================================================================================================

template <typename Signature> class WereFunction;
template <typename R, typename ... Args>
class WereFunction<R (Args ...)>
{
public:
    
    R operator()(Args ... args)
    {
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

#endif //WERE_FUNCTION_H

