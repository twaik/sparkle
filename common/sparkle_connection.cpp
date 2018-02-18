#include "sparkle_connection.h"
#include "were/were_socket_unix.h"
#include "were/were_timer.h"

//==================================================================================================

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
    _socket->signal_data.connect(WereSimpleQueuer(loop, &SparkleConnection::handleData, this));
    
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
    _socket->signal_data.connect(WereSimpleQueuer(loop, &SparkleConnection::handleData, this));
    
    _connectTimer = 0;
}

//==================================================================================================

bool SparkleConnection::connected()
{
    return _socket->connected();
}

//==================================================================================================

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

void SparkleConnection::handleData()
{
    unsigned int bytesAvailable = _socket->bytesAvailable();
    
    while (bytesAvailable >= sizeof(uint32_t))
    {
        uint32_t size = 0;
        if (_socket->peek(reinterpret_cast<unsigned char *>(&size), sizeof(uint32_t)) != sizeof(uint32_t))
        {
            were_debug("[%p][%s] peek failed, DISCONNECTING.\n", this, __PRETTY_FUNCTION__);
            _socket->disconnect();
            return;
        }
        
        if (bytesAvailable < sizeof(uint32_t) + size)
            return;
        
        if (_socket->receive(reinterpret_cast<unsigned char *>(&size), sizeof(uint32_t)) != sizeof(uint32_t))
        {
            were_debug("[%p][%s] receive (header) failed, DISCONNECTING.\n", this, __PRETTY_FUNCTION__);
            _socket->disconnect();
            return;
        }
        bytesAvailable -= sizeof(uint32_t);
    
        std::shared_ptr<SparklePacket> packet(new SparklePacket(size));
        unsigned char *data = packet->allocate(size);
        
        if (_socket->receive(data, size) != size)
        {
            were_debug("[%p][%s] receive (data) failed, DISCONNECTING.\n", this, __PRETTY_FUNCTION__);
            _socket->disconnect();
            return;
        }
        bytesAvailable -= size;
        
        //were_debug("RECV %d\n", packet->size());
        signal_packet(packet);
    }
}

//==================================================================================================

void SparkleConnection::send(SparklePacket *packet)
{
    //were_debug("SEND %d\n", packet->size());
    
    if (!_socket->connected())
        return;
    
    uint32_t size = packet->size();
    
    if (_socket->send(reinterpret_cast<unsigned char *>(&size), sizeof(uint32_t)) != sizeof(uint32_t))
    {
        were_debug("[%p][%s] n != size (sending header), DISCONNECTING.", this, __PRETTY_FUNCTION__);
        _socket->disconnect();
    }
    
    if (_socket->send(packet->data(), packet->size()) != packet->size())
    {
        were_debug("[%p][%s] n != size (sending data), DISCONNECTING.", this, __PRETTY_FUNCTION__);
        _socket->disconnect();
    }
}

//==================================================================================================

sparkle_connection_t *sparkle_connection_create(were_event_loop_t *loop, const char *path)
{
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);
    SparkleConnection *_connection = new SparkleConnection(_loop, path);
    return static_cast<sparkle_connection_t *>(_connection);
}

void sparkle_connection_destroy(sparkle_connection_t *connection)
{
    SparkleConnection *_connection = static_cast<SparkleConnection *>(connection);
    delete _connection;
}

void sparkle_connection_send(sparkle_connection_t *connection, sparkle_packet_t *packet)
{
    SparkleConnection *_connection = static_cast<SparkleConnection *>(connection);
    SparklePacket *_packet = static_cast<SparklePacket *>(packet);
    _connection->send(_packet);
}

void sparkle_connection_add_connection_callback(sparkle_connection_t *connection, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    SparkleConnection *_connection = static_cast<SparkleConnection *>(connection);
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);

    _connection->signal_connected.connect([_loop, f, user]()
    {
        _loop->queue(std::bind(f, user));
    });
}

void sparkle_connection_add_disconnection_callback(sparkle_connection_t *connection, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    SparkleConnection *_connection = static_cast<SparkleConnection *>(connection);
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);

    _connection->signal_disconnected.connect([_loop, f, user]()
    {
        _loop->queue(std::bind(f, user));
    });
}

static void sparkle_packet_callback(std::shared_ptr<SparklePacket> packet, void (*f)(void *user, sparkle_packet_t *packet), void *user)
{
    (*f)(user, static_cast<sparkle_packet_t *>(packet.get()));
}

void sparkle_connection_add_packet_callback(sparkle_connection_t *connection, were_event_loop_t *loop, void (*f)(void *user, sparkle_packet_t *packet), void *user)
{
    SparkleConnection *_connection = static_cast<SparkleConnection *>(connection);
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);

    _connection->signal_packet.connect([_loop, f, user](std::shared_ptr<SparklePacket> packet)
    {
        _loop->queue(std::bind(sparkle_packet_callback, packet, f, user));
    });
}

//==================================================================================================

