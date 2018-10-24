#include "were_exception.h"
#include <cstdarg>

/* ================================================================================================================== */

WereException::WereException(const char *format, ...)
{
    char buffer[1024];

    va_list ap;
    va_start(ap, format);
    vsnprintf(buffer, 1024, format, ap);
    va_end(ap);

    _what = std::string(buffer);
}

const char *WereException::what() const throw()
{
    return _what.c_str();
}

/* ================================================================================================================== */
