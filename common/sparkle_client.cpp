#include "sparkle_client.h"
#include "were/were_socket_unix.h"
#include <unistd.h>

//==================================================================================================

SparkleClient::~SparkleClient()
{
    delete _socket;
}

SparkleClient::SparkleClient(WereEventLoop *loop)
{
    _loop = loop;
    
    _socket = new WereSocketUnix(_loop);
    _socket->signal_data.connect(_loop, std::bind(&SparkleClient::readData, this));
}

//==================================================================================================

WereSocketUnix *SparkleClient::socket()
{
    return _socket;
}

//==================================================================================================

void SparkleClient::connect(const std::string &path)
{
    _socket->connect(path);
}

void SparkleClient::disconnect()
{
    _socket->disconnect();
}

//==================================================================================================

void SparkleClient::readData()
{
    unsigned int bytesAvailable = _socket->bytesAvailable();
    
    while (bytesAvailable >= sizeof(uint32_t))
    {
        uint32_t size = 0;
        _socket->peek(&size, sizeof(uint32_t));
        
        if (bytesAvailable < sizeof(uint32_t) + size)
            return;
        
        _socket->receive(&size, sizeof(uint32_t));
        bytesAvailable -= sizeof(uint32_t);
    
        unsigned char *data = new unsigned char[size];
        _socket->receive(data, size);
        bytesAvailable -= size;
    
        sparkle_packet_t *packet = sparkle_packet_create(data, size);
        handlePacket(packet);
        sparkle_packet_destroy(packet);
    
        delete[] data;
    }
}

void SparkleClient::handlePacket(sparkle_packet_t *packet)
{
    uint32_t operation = sparkle_packet_get_uint32(packet);
    
    if (operation == SPARKLE_SERVER_DISPLAY_SIZE)
    {       
        int width = sparkle_packet_get_uint32(packet);
        int height = sparkle_packet_get_uint32(packet);
        displaySize(width, height);
    }
    else if (operation == SPARKLE_SERVER_POINTER_DOWN)
    {
        std::string name = sparkle_packet_get_string(packet);
        int slot = sparkle_packet_get_uint32(packet);
        int x = sparkle_packet_get_uint32(packet);
        int y = sparkle_packet_get_uint32(packet);
        pointerDown(name, slot, x, y);
    }
    else if (operation == SPARKLE_SERVER_POINTER_UP)
    {
        std::string name = sparkle_packet_get_string(packet);
        int slot = sparkle_packet_get_uint32(packet);
        int x = sparkle_packet_get_uint32(packet);
        int y = sparkle_packet_get_uint32(packet);
        pointerUp(name, slot, x, y);
    }
    else if (operation == SPARKLE_SERVER_POINTER_MOTION)
    {
        std::string name = sparkle_packet_get_string(packet);
        int slot = sparkle_packet_get_uint32(packet);
        int x = sparkle_packet_get_uint32(packet);
        int y = sparkle_packet_get_uint32(packet);
        pointerMotion(name, slot, x, y);
    }
    else if (operation == SPARKLE_SERVER_KEY_DOWN)
    {
        int code = sparkle_packet_get_uint32(packet);
        keyDown(code);
    }
    else if (operation == SPARKLE_SERVER_KEY_UP)
    {
        int code = sparkle_packet_get_uint32(packet);
        keyUp(code);
    }
}

void SparkleClient::send(sparkle_packet_t *packet)
{
    uint32_t size = sparkle_packet_position(packet);
    
    _socket->send(&size, sizeof(uint32_t));
    _socket->send(sparkle_packet_data(packet), size);
}

//==================================================================================================

void SparkleClient::registerSurfaceFile(const std::string &name, const std::string &path, int width, int height)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_CLIENT_REGISTER_SURFACE_FILE);
    sparkle_packet_add_string(packet, name.c_str());
    sparkle_packet_add_string(packet, path.c_str());
    sparkle_packet_add_uint32(packet, width);
    sparkle_packet_add_uint32(packet, height);
    send(packet);
    sparkle_packet_destroy(packet);
}

void SparkleClient::unregisterSurface(const std::string &name)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_CLIENT_UNREGISTER_SURFACE);
    sparkle_packet_add_string(packet, name.c_str());
    send(packet);
    sparkle_packet_destroy(packet);
    
    usleep(100000);
}

void SparkleClient::setSurfacePosition(const std::string &name, int x1, int y1, int x2, int y2)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_CLIENT_SET_SURFACE_POSITION);
    sparkle_packet_add_string(packet, name.c_str());
    sparkle_packet_add_uint32(packet, x1);
    sparkle_packet_add_uint32(packet, y1);
    sparkle_packet_add_uint32(packet, x2);
    sparkle_packet_add_uint32(packet, y2);
    send(packet);
    sparkle_packet_destroy(packet);
}

void SparkleClient::addSurfaceDamage(const std::string &name, int x1, int y1, int x2, int y2)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_CLIENT_ADD_SURFACE_DAMAGE);
    sparkle_packet_add_string(packet, name.c_str());
    sparkle_packet_add_uint32(packet, x1);
    sparkle_packet_add_uint32(packet, y1);
    sparkle_packet_add_uint32(packet, x2);
    sparkle_packet_add_uint32(packet, y2);
    send(packet);
    sparkle_packet_destroy(packet);
}
    
//==================================================================================================

template <typename ... Args>
void conv1(void *f, void *user, Args ... args)
{
    typedef void (*PROC)(void *user, Args ...);
    PROC _f = reinterpret_cast<PROC>(f);
    _f(user, args ...);
}

template <typename ... Args>
void conv2(void *f, void *user, const std::string &name, Args ... args)
{
    typedef void (*PROC)(void *user, const char *name, Args ...);
    PROC _f = reinterpret_cast<PROC>(f);
    _f(user, name.c_str(), args ...);
}

//==================================================================================================

sparkle_client_t *sparkle_client_create(were_event_loop_t *loop)
{
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);
    SparkleClient *_client = new SparkleClient(_loop);
    
    return static_cast<sparkle_client_t *>(_client);
}

void sparkle_client_destroy(sparkle_client_t *client)
{
    SparkleClient *_client = static_cast<SparkleClient *>(client);
    
    delete _client;
}

void sparkle_client_set_connection_callback(sparkle_client_t *client, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    SparkleClient *_client = static_cast<SparkleClient *>(client);
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);
    _client->socket()->signal_connected.connect(_loop, std::bind(f, user));
}

void sparkle_client_set_disconnection_callback(sparkle_client_t *client, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    SparkleClient *_client = static_cast<SparkleClient *>(client);
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);
    _client->socket()->signal_disconnected.connect(_loop, std::bind(f, user));
}

void sparkle_client_set_operation_callback(sparkle_client_t *client, were_event_loop_t *loop, int operation, void *f, void *user)
{
    SparkleClient *_client = static_cast<SparkleClient *>(client);
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);
    
    if (operation == SPARKLE_SERVER_DISPLAY_SIZE)
        _client->displaySize.connect(_loop, std::bind(conv1<int, int>, f, user, std::placeholders::_1, std::placeholders::_2));
    else if (operation == SPARKLE_SERVER_POINTER_DOWN)
        _client->pointerDown.connect(_loop, std::bind(conv2<int, int, int>, f, user, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    else if (operation == SPARKLE_SERVER_POINTER_UP)
        _client->pointerUp.connect(_loop, std::bind(conv2<int, int, int>, f, user, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    else if (operation == SPARKLE_SERVER_POINTER_MOTION)
        _client->pointerMotion.connect(_loop, std::bind(conv2<int, int, int>, f, user, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    else if (operation == SPARKLE_SERVER_KEY_DOWN)
        _client->keyDown.connect(_loop, std::bind(conv1<int>, f, user, std::placeholders::_1));
    else if (operation == SPARKLE_SERVER_KEY_UP)
        _client->keyUp.connect(_loop, std::bind(conv1<int>, f, user, std::placeholders::_1));
}

void sparkle_client_connect(sparkle_client_t *client, const char *path)
{
    SparkleClient *_client = static_cast<SparkleClient *>(client);
    _client->connect(path);
}

void sparkle_client_disconnect(sparkle_client_t *client)
{
    SparkleClient *_client = static_cast<SparkleClient *>(client);
    _client->disconnect();
}

void sparkle_client_register_surface_file(sparkle_client_t *client, const char *name, const char *path, int width, int height)
{
    SparkleClient *_client = static_cast<SparkleClient *>(client);
    _client->registerSurfaceFile(name, path, width, height);
}

void sparkle_client_unregister_surface(sparkle_client_t *client, const char *name)
{
    SparkleClient *_client = static_cast<SparkleClient *>(client);
    _client->unregisterSurface(name);
}

void sparkle_client_set_surface_position(sparkle_client_t *client, const char *name, int x1, int y1, int x2, int y2)
{
    SparkleClient *_client = static_cast<SparkleClient *>(client);
    _client->setSurfacePosition(name, x1, y1, x2, y2);
}

void sparkle_client_add_surface_damage(sparkle_client_t *client, const char *name, int x1, int y1, int x2, int y2)
{
    SparkleClient *_client = static_cast<SparkleClient *>(client);
    _client->addSurfaceDamage(name, x1, y1, x2, y2);
}

//==================================================================================================

