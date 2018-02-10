#include "sparkle_server.h"
#include "were/were_server_unix.h"
#include "common/sparkle_protocol.h"


//==================================================================================================

SparkleServer::~SparkleServer()
{
    for (unsigned int i = 0; i < _clients.size(); ++i)
    {
        WereSocketUnix *client = _clients[i];
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
    WereSocketUnix *client = _server->accept();
    if (client == 0)
        return;
    
    client->signal_data.connect(_loop, std::bind(&SparkleServer::handleData, this, client));
    client->signal_disconnected.connect(_loop, std::bind(&SparkleServer::handleDisconnection, this, client));
    _clients.push_back(client);
    
    connection();
}

void SparkleServer::handleDisconnection(WereSocketUnix *client)
{
}

void SparkleServer::handleData(WereSocketUnix *client)
{
    unsigned int bytesAvailable = client->bytesAvailable();
    
    while (bytesAvailable >= sizeof(uint32_t))
    {
        uint32_t size = 0;
        client->peek(&size, sizeof(uint32_t));
        
        if (bytesAvailable < sizeof(uint32_t) + size)
            return;
        
        client->receive(&size, sizeof(uint32_t));
        bytesAvailable -= sizeof(uint32_t);
    
        unsigned char *data = new unsigned char[size];
        client->receive(data, size);
        bytesAvailable -= size;
    
        sparkle_packet_t *packet = sparkle_packet_create((unsigned char *)data, size);
        
        handlePacket(packet);
    
        sparkle_packet_destroy(packet);
    
        delete[] data;
    }
}

void SparkleServer::handlePacket(sparkle_packet_t *packet)
{
    uint32_t operation = sparkle_packet_get_uint32(packet);
    
    if (operation == SPARKLE_CLIENT_REGISTER_SURFACE_FILE)
    {
        std::string name = sparkle_packet_get_string(packet);
        std::string path = sparkle_packet_get_string(packet);
        int width = sparkle_packet_get_uint32(packet);
        int height = sparkle_packet_get_uint32(packet);
        registerSurfaceFile(name, path, width, height);
    }
    else if (operation == SPARKLE_CLIENT_UNREGISTER_SURFACE)
    {
        std::string name = sparkle_packet_get_string(packet);
        unregisterSurface(name);
    }
    else if (operation == SPARKLE_CLIENT_SET_SURFACE_POSITION)
    {
        std::string name = sparkle_packet_get_string(packet);
        int x1 = sparkle_packet_get_uint32(packet);
        int y1 = sparkle_packet_get_uint32(packet);
        int x2 = sparkle_packet_get_uint32(packet);
        int y2 = sparkle_packet_get_uint32(packet);
        setSurfacePosition(name, x1, y1, x2, y2);
    }
    else if (operation == SPARKLE_CLIENT_ADD_SURFACE_DAMAGE)
    {
        std::string name = sparkle_packet_get_string(packet);
        int x1 = sparkle_packet_get_uint32(packet);
        int y1 = sparkle_packet_get_uint32(packet);
        int x2 = sparkle_packet_get_uint32(packet);
        int y2 = sparkle_packet_get_uint32(packet);
        addSurfaceDamage(name, x1, y1, x2, y2);
    }
    else if (operation == SPARKLE_CLIENT_KEY_PRESS)
    {
        int code = sparkle_packet_get_uint32(packet);
        keyPress(code);
        keyDown(code);
    }
    else if (operation == SPARKLE_CLIENT_KEY_RELEASE)
    {
        int code = sparkle_packet_get_uint32(packet);
        keyRelease(code);
        keyUp(code);
    }
}

void SparkleServer::broadcast(sparkle_packet_t *packet)
{
    uint32_t size = sparkle_packet_position(packet);
    
    std::vector<WereSocketUnix *>::iterator it = _clients.begin();
    
    while (it != _clients.end())
    {
        WereSocketUnix *client = *it;
        
        if (!client->connected())
        {
            it = _clients.erase(it);
        }
        else
        {
            client->send(&size, sizeof(uint32_t));
            client->send(sparkle_packet_data(packet), size);
            ++it;
        }
    }
}

//==================================================================================================

void SparkleServer::displaySize(int width, int height)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SERVER_DISPLAY_SIZE);
    sparkle_packet_add_uint32(packet, width);
    sparkle_packet_add_uint32(packet, height);
    broadcast(packet);
    sparkle_packet_destroy(packet);
}

void SparkleServer::pointerDown(const std::string &surface, int slot, int x, int y)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SERVER_POINTER_DOWN);
    sparkle_packet_add_string(packet, surface.c_str());
    sparkle_packet_add_uint32(packet, slot);
    sparkle_packet_add_uint32(packet, x);
    sparkle_packet_add_uint32(packet, y);
    broadcast(packet);
    sparkle_packet_destroy(packet);
}

void SparkleServer::pointerUp(const std::string &surface, int slot, int x, int y)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SERVER_POINTER_UP);
    sparkle_packet_add_string(packet, surface.c_str());
    sparkle_packet_add_uint32(packet, slot);
    sparkle_packet_add_uint32(packet, x);
    sparkle_packet_add_uint32(packet, y);
    broadcast(packet);
    sparkle_packet_destroy(packet);
}

void SparkleServer::pointerMotion(const std::string &surface, int slot, int x, int y)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SERVER_POINTER_MOTION);
    sparkle_packet_add_string(packet, surface.c_str());
    sparkle_packet_add_uint32(packet, slot);
    sparkle_packet_add_uint32(packet, x);
    sparkle_packet_add_uint32(packet, y);
    broadcast(packet);
    sparkle_packet_destroy(packet);
}

void SparkleServer::keyDown(int code)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SERVER_KEY_DOWN);
    sparkle_packet_add_uint32(packet, code);
    broadcast(packet);
    sparkle_packet_destroy(packet);
}

void SparkleServer::keyUp(int code)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SERVER_KEY_UP);
    sparkle_packet_add_uint32(packet, code);
    broadcast(packet);
    sparkle_packet_destroy(packet);
}

//==================================================================================================

