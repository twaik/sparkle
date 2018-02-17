#include "sparkle_server.h"
#include "were/were_server_unix.h"
#include "sparkle_connection.h"


//==================================================================================================

SparkleServer::~SparkleServer()
{
    _clients.clear();
    
    delete _server;
}

SparkleServer::SparkleServer(WereEventLoop *loop, const std::string &path)
{
    _loop = loop;
    
    _server = new WereServerUnix(_loop, path);
    
    _server->newConnection.connect(_loop, std::bind(&SparkleServer::handleConnection, this));
}

//==================================================================================================

void SparkleServer::handleConnection()
{
    WereSocketUnix *socket = _server->accept();
    if (socket == 0)
        return;
    
    std::shared_ptr<SparkleConnection> client(new SparkleConnection(_loop, socket));
    
    client->signal_disconnected.connect(_loop, std::bind(&SparkleServer::handleDisconnection, this, client));
    client->signal_packet.connect(_loop, std::bind(&SparkleServer::handlePacket, this, client, _1));

    _clients.insert(client);
    
    signal_connected(client);
}

void SparkleServer::handleDisconnection(std::shared_ptr<SparkleConnection> client)
{
    _clients.erase(client);
}

void SparkleServer::handlePacket(std::shared_ptr<SparkleConnection> client, std::shared_ptr<SparklePacket> packet)
{
    signal_packet(client, packet);
}

void SparkleServer::broadcast(SparklePacket *packet)
{
    std::set< std::shared_ptr<SparkleConnection> >::iterator it;
    
    for (it = _clients.begin(); it != _clients.end(); ++it)
        (*it)->send(packet);
}

//==================================================================================================

