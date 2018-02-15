#include "sparkle_server.h"
#include "were/were_server_unix.h"
#include "sparkle_connection.h"


//==================================================================================================

SparkleServer::~SparkleServer()
{
    for (unsigned int i = 0; i < _clients.size(); ++i)
    {
        SparkleConnection *client = _clients[i];
        delete client;
    }
    
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
    
    SparkleConnection *client = new SparkleConnection(_loop, socket);
    
    client->signal_disconnected.connect(_loop, std::bind(&SparkleServer::handleDisconnection, this, client));
    client->signal_packet.connect(_loop, std::bind(&SparkleServer::handlePacket, this, client, _1));

    _clients.push_back(client);
    
    signal_connected(client);
}

void SparkleServer::handleDisconnection(SparkleConnection *client)
{
}

void SparkleServer::handlePacket(SparkleConnection *client, SparklePacket packet)
{
    signal_packet(client, packet);
}

void SparkleServer::broadcast(const SparklePacket &packet)
{
    std::vector<SparkleConnection *>::iterator it = _clients.begin();
    
    while (it != _clients.end())
    {
        SparkleConnection *client = *it;
        
        if (!client->connected())
        {
            delete client;
            it = _clients.erase(it);
        }
        else
        {
            client->send(packet);
            ++it;
        }
    }
}

//==================================================================================================

