#ifndef WERE_EXCEPTION_H
#define WERE_EXCEPTION_H

#include <exception>
#include <string>

class WereException : public std::exception
{
public:
    WereException(const char *format, ...);

    const char *what() const throw();
    
private:
    std::string _what;
};

#endif //WERE_EXCEPTION_H

