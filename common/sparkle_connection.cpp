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
    return _socket->state() == WereSocketUnix::ConnectedState;
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
            were_debug("[%p][%s] receive (size) failed, DISCONNECTING.\n", this, __PRETTY_FUNCTION__);
            _socket->disconnect();
            return;
        }
        bytesAvailable -= sizeof(uint32_t);
        
        uint32_t dataSize = size - sizeof(SparklePacketHeader);
    
        std::shared_ptr<SparklePacket> packet(new SparklePacket());
        
        if (_socket->receive(reinterpret_cast<unsigned char *>(packet->header()), sizeof(SparklePacketHeader)) != sizeof(SparklePacketHeader))
        {
            were_debug("[%p][%s] receive (header) failed, DISCONNECTING.\n", this, __PRETTY_FUNCTION__);
            _socket->disconnect();
            return;
        }
        bytesAvailable -= sizeof(SparklePacketHeader);
        
        unsigned char *data = packet->allocate(dataSize);
        
        if (_socket->receive(data, dataSize) != dataSize)
        {
            were_debug("[%p][%s] receive (data) failed, DISCONNECTING.\n", this, __PRETTY_FUNCTION__);
            _socket->disconnect();
            return;
        }
        bytesAvailable -= dataSize;
        
        //were_debug("RECV %d\n", packet->size());
        signal_packet(packet);
    }
}

//==================================================================================================

void SparkleConnection::send(SparklePacket *packet)
{
    //were_debug("SEND %d\n", packet->size());
    
    if (_socket->state() != WereSocketUnix::ConnectedState)
        return;
    
    uint32_t size = sizeof(SparklePacketHeader) + packet->size();
    
    if (_socket->send(reinterpret_cast<unsigned char *>(&size), sizeof(uint32_t)) != sizeof(uint32_t))
    {
        were_debug("[%p][%s] n != size (sending size), DISCONNECTING.", this, __PRETTY_FUNCTION__);
        _socket->disconnect();
    }
    
    if (_socket->send(reinterpret_cast<unsigned char *>(packet->header()), sizeof(SparklePacketHeader)) != sizeof(SparklePacketHeader))
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

void SparkleConnection::send1(const SparklePacketType *packetType, void *data)
{
    SparklePacket packet;
    packet.header()->operation = packetType->code;
    
    SparklePacketStream stream(&packet);
    stream.pWrite(&packetType->packer, data);
    
    send(&packet);
}

void SparkleConnection::unpack1(const SparklePacketType *packetType, SparklePacket *packet, void *data)
{
    SparklePacketStream stream(packet);
    stream.pRead(&packetType->packer, data);
}

//==================================================================================================

SparkleConnection *sparkle_connection_create(WereEventLoop *loop, const char *path)
{
    return new SparkleConnection(loop, path);
}

void sparkle_connection_destroy(SparkleConnection *connection)
{
    delete connection;
}

void sparkle_connection_send(SparkleConnection *connection, SparklePacket *packet)
{
    connection->send(packet);
}

void sparkle_connection_send1(SparkleConnection *connection, const SparklePacketType *packetType, void *data)
{
    connection->send1(packetType, data);
}

void sparkle_connection_unpack1(const SparklePacketType *packetType, SparklePacket *packet, void *data)
{
    SparkleConnection::unpack1(packetType, packet, data);
}


void sparkle_connection_add_connection_callback(SparkleConnection *connection, WereEventLoop *loop, void (*f)(void *user), void *user)
{
    connection->signal_connected.connect([loop, f, user]()
    {
        loop->queue(std::bind(f, user));
    });
}

void sparkle_connection_add_disconnection_callback(SparkleConnection *connection, WereEventLoop *loop, void (*f)(void *user), void *user)
{
    connection->signal_disconnected.connect([loop, f, user]()
    {
        loop->queue(std::bind(f, user));
    });
}

static void sparkle_packet_callback(std::shared_ptr<SparklePacket> packet, void (*f)(void *user, SparklePacket *packet), void *user)
{
    (*f)(user, packet.get());
}

void sparkle_connection_add_packet_callback(SparkleConnection *connection, WereEventLoop *loop, void (*f)(void *user, SparklePacket *packet), void *user)
{
    connection->signal_packet.connect([loop, f, user](std::shared_ptr<SparklePacket> packet)
    {
        loop->queue(std::bind(sparkle_packet_callback, packet, f, user));
    });
}

//==================================================================================================

