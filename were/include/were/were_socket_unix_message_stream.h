#ifndef WERE_SOCKET_UNIX_MESSAGE_STREAM_H
#define WERE_SOCKET_UNIX_MESSAGE_STREAM_H

#include "were_stream.h"
#include "were_socket_unix.h"

/* ================================================================================================================== */

class WereSocketUnixMessageStream : public WereStream
{
public:
    ~WereSocketUnixMessageStream();
    WereSocketUnixMessageStream(WereSocketUnixMessage *message);

    void writeFD(int fd);
    void readFD(int *fd);

private:

private:
    std::vector<int> *vector_;
    unsigned int writePosition_;
    unsigned int readPosition_;
};

/* ================================================================================================================== */

#endif /* WERE_SOCKET_UNIX_MESSAGE_STREAM_H */
