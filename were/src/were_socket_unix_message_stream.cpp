#include "were_socket_unix_message_stream.h"

/* ================================================================================================================== */

WereSocketUnixMessageStream::~WereSocketUnixMessageStream()
{
}

WereSocketUnixMessageStream::WereSocketUnixMessageStream(WereSocketUnixMessage *message) :
    WereStream(message->data())
{
    vector_ = message->fds();
    writePosition_ = 0;
    readPosition_ = 0;
}

void WereSocketUnixMessageStream::writeFD(int fd)
{
    vector_->push_back(fd);
    writePosition_ += 1;
}

void WereSocketUnixMessageStream::readFD(int *fd)
{
    *fd = vector_->at(readPosition_);
    readPosition_ += 1;
}

/* ================================================================================================================== */
