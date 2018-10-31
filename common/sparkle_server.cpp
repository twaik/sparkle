#include "sparkle_server.h"
#include "were/were_server_unix.h"
#include "were/were_stream.h"
#include "sparkle_connection.h"

/* ================================================================================================================== */

SparkleServer::~SparkleServer()
{
    delete _server;
}

SparkleServer::SparkleServer(WereEventLoop *loop, const std::string &path)
{
    _loop = loop;

    _server = new WereServerUnix(_loop, path);

    _server->signal_connection.connect(WereSimpleQueuer(loop, &SparkleServer::handleConnection, this));
}

/* ================================================================================================================== */

void SparkleServer::handleConnection()
{
    WereSocketUnix *socket = _server->accept();
    if (socket == 0)
        return;

    std::shared_ptr<SparkleConnection> client(new SparkleConnection(_loop, socket));

    std::weak_ptr<SparkleConnection> client_weak = client;

    client->signal_disconnected.connect([this, client_weak]()
    {
        _loop->queue(std::bind(&SparkleServer::handleDisconnection, this, client_weak.lock()));
    });

    client->signal_message.connect([this, client_weak](std::shared_ptr<WereSocketUnixMessage> message)
    {
        _loop->queue(std::bind(&SparkleServer::handleMessage, this, client_weak.lock(), message));
    });

    _clients.insert(client);

    signal_connected(client);
}

void SparkleServer::handleDisconnection(std::shared_ptr<SparkleConnection> client)
{
    signal_disconnected(client);

    _clients.erase(client);
}

void SparkleServer::handleMessage(std::shared_ptr<SparkleConnection> client, std::shared_ptr<WereSocketUnixMessage> message)
{
    signal_packet(client, message);
}

void SparkleServer::broadcast(WereSocketUnixMessage *message)
{
    for (auto it = _clients.begin(); it != _clients.end(); ++it)
        (*it)->send(message);
}

/* ================================================================================================================== */
