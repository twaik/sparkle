#include "sparkle_connection.h"
#include "were/were_socket_unix.h"
#include "were/were_timer.h"

/* ================================================================================================================== */

SparkleConnection::~SparkleConnection()
{
    if (_connectTimer)
        delete _connectTimer;
    delete _socket;
}

SparkleConnection::SparkleConnection(WereEventLoop *loop, const std::string &path)
{
    _loop = loop;
    _path = path;

    _socket = new WereSocketUnix(_loop);
    _socket->signal_connected.connect(WereSimpleQueuer(loop, &SparkleConnection::handleConnection, this));
    _socket->signal_disconnected.connect(WereSimpleQueuer(loop, &SparkleConnection::handleDisconnection, this));
    _socket->signal_message.connect(WereSimpleQueuer(loop, &SparkleConnection::handleMessage, this));

    _connectTimer = new WereTimer(_loop);
    _connectTimer->timeout.connect(WereSimpleQueuer(loop, &SparkleConnection::connect, this));

    _connectTimer->start(1000, false);
}

SparkleConnection::SparkleConnection(WereEventLoop *loop, WereSocketUnix *socket)
{
    _loop = loop;

    _socket = socket;
    _socket->signal_connected.connect(WereSimpleQueuer(loop, &SparkleConnection::handleConnection, this));
    _socket->signal_disconnected.connect(WereSimpleQueuer(loop, &SparkleConnection::handleDisconnection, this));
    _socket->signal_message.connect(WereSimpleQueuer(loop, &SparkleConnection::handleMessage, this));

    _connectTimer = nullptr;
}

/* ================================================================================================================== */

bool SparkleConnection::connected()
{
    return _socket->state() == WereSocketUnix::ConnectedState;
}

/* ================================================================================================================== */

void SparkleConnection::connect()
{
    _socket->connect(_path);
}

void SparkleConnection::handleConnection()
{
    signal_connected();
}

void SparkleConnection::handleDisconnection()
{
    signal_disconnected();
}

void SparkleConnection::handleMessage(std::shared_ptr<WereSocketUnixMessage> message)
{
    signal_packet(message);
}

/* ================================================================================================================== */

void SparkleConnection::send(WereSocketUnixMessage *message)
{
    if (_socket->state() != WereSocketUnix::ConnectedState)
        return;

    _socket->sendMessage(message);
}

void SparkleConnection::send1(const SparklePacketType *packetType, void *data)
{
    WereSocketUnixMessage message;

    WereStream stream(message.data());
    stream.pWrite(&p_uint32, &packetType->code);
    stream.pWrite(&packetType->packer, data);

    send(&message);
}

/* ================================================================================================================== */
