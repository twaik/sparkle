#include "sparkle_keyboard.h"
#include "were/were_event_loop.h"
#include "common/sparkle_surface_file.h"
#include "ui/widget_host.h"
#include "common/sparkle_client.h"
#include "were/were_socket_unix.h"
#include "ui/widget_button.h"

#include <string.h>

//==================================================================================================

const char *surface_name = "keyboard";

struct ButtonData
{
    int x;
    int y;
    int width;
    int height;
    int code;
    const char *label;
};

const ButtonData layout[] =
{
    {0, 0, 4, 4, 9, "Esc"},
    {8, 0, 4, 4, 67, "F1"}, {12, 0, 4, 4, 68, "F2"}, {16, 0, 4, 4, 69, "F3"}, {20, 0, 4, 4, 70, "F4"},
    {26, 0, 4, 4, 71, "F5"}, {30, 0, 4, 4, 72, "F6"}, {34, 0, 4, 4, 73, "F7"}, {38, 0, 4, 4, 74, "F8"},
    {44, 0, 4, 4, 75, "F9"}, {48, 0, 4, 4, 76, "F10"}, {52, 0, 4, 4, 95, "F11"}, {56, 0, 4, 4, 96, "F12"},
    
    {0, 6, 4, 4, 49, "~\n`"}, {4, 6, 4, 4, 10, "!\n1"}, {8, 6, 4, 4, 11, "@\n2"}, {12, 6, 4, 4, 12, "#\n3"}, {16, 6, 4, 4, 13, "$\n4"}, {20, 6, 4, 4, 14, "%\n5"}, {24, 6, 4, 4, 15, "^\n6"}, {28, 6, 4, 4, 16, "&\n7"}, {32, 6, 4, 4, 17, "*\n8"}, {36, 6, 4, 4, 18, "(\n9"}, {40, 6, 4, 4, 19, ")\n0"}, {44, 6, 4, 4, 20, "_\n-"}, {48, 6, 4, 4, 21, "+\n="},
    {52, 6, 8, 4, 22, "Backspace"},
    
    {0, 10, 6, 4, 23, "Tab"},
    {6, 10, 4, 4, 24, "Q"}, {10, 10, 4, 4, 25, "W"}, {14, 10, 4, 4, 26, "E"}, {18, 10, 4, 4, 27, "R"}, {22, 10, 4, 4, 28, "T"}, {26, 10, 4, 4, 29, "Y"}, {30, 10, 4, 4, 30, "U"}, {34, 10, 4, 4, 31, "I"}, {38, 10, 4, 4, 32, "O"}, {42, 10, 4, 4, 33, "P"}, {46, 10, 4, 4, 34, "{\n["}, {50, 10, 4, 4, 35, "}\n]"},
    {54, 10, 6, 4, 51, "|\n\\"},
    
    {0, 14, 7, 4, 66, "Caps"},
    {7, 14, 4, 4, 38, "A"}, {11, 14, 4, 4, 39, "S"}, {15, 14, 4, 4, 40, "D"}, {19, 14, 4, 4, 41, "F"}, {23, 14, 4, 4, 42, "G"}, {27, 14, 4, 4, 43, "H"}, {31, 14, 4, 4, 44, "J"}, {35, 14, 4, 4, 45, "K"}, {39, 14, 4, 4, 46, "L"}, {43, 14, 4, 4, 47, ":\n;"}, {47, 14, 4, 4, 48, "\"\n\'"},
    {51, 14, 9, 4, 36, "Enter"},
    
    {0, 18, 9, 4, 50, "Shift"},
    {9, 18, 4, 4, 52, "Z"}, {13, 18, 4, 4, 53, "X"}, {17, 18, 4, 4, 54, "C"}, {21, 18, 4, 4, 55, "V"}, {25, 18, 4, 4, 56, "B"}, {29, 18, 4, 4, 57, "N"}, {33, 18, 4, 4, 58, "M"}, {37, 18, 4, 4, 59, ""}, {41, 18, 4, 4, 60, ""}, {45, 18, 4, 4, 61, ""},
    {49, 18, 11, 4, 62, "Shift"},
    
    {0, 22, 5, 4, 37, "Ctrl"}, {5, 22, 5, 4, 133, "Win"}, {10, 22, 5, 4, 64, "Alt"},
    {15, 22, 25, 4, 65, ""},
    {40, 22, 5, 4, 108, "Alt"}, {45, 22, 5, 4, 135, "Win"}, {50, 22, 5, 4, 9, ""}, {55, 22, 5, 4, 105, "Ctrl"},
    
    {62, 0, 4, 4, 107, ""}, {66, 0, 4, 4, 78, ""}, {70, 0, 4, 4, 127, ""},
    {62, 6, 4, 4, 118, ""}, {66, 6, 4, 4, 110, ""}, {70, 6, 4, 4, 112, ""},
    {62, 10, 4, 4, 119, ""}, {66, 10, 4, 4, 115, ""}, {70, 10, 4, 4, 117, ""},
    
    {66, 18, 4, 4, 98, "\x18"}, {62, 22, 4, 4, 100, "\x1b"}, {66, 22, 4, 4, 104, "\x19"}, {70, 22, 4, 4, 102, "\x1a"},
};

const int layoutWidth = 74;
const int layoutHeight = 26;

//==================================================================================================

SparkleKeyboard::~SparkleKeyboard()
{
    _client->unregisterSurface(surface_name);
    
    
    delete[] _buttons;
    delete _host;
    delete _surface;
    delete _client;
    delete _loop;
}

SparkleKeyboard::SparkleKeyboard()
{
    _loop = new WereEventLoop(true);
    
    _client = new SparkleClient(_loop, "/dev/shm/sparkle.socket");
    _client->socket()->signal_connected.connect(_loop, std::bind(&SparkleKeyboard::handleConnection, this));
    _client->pointerDown.connect(_loop, std::bind(&SparkleKeyboard::pointerDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    _client->pointerUp.connect(_loop, std::bind(&SparkleKeyboard::pointerUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    
    _surface = new SparkleSurfaceFile("/dev/shm/keyboard", 800, 240, true);
    //memset(_surface->data(), 0x00, _surface->width()*_surface->height()*4);
    
    _host = new WidgetHost(_loop, _surface->data(), _surface->width(), _surface->width(), _surface->height());
    _host->damage.connect(_loop, std::bind(&SparkleKeyboard::hostDamage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

    
    unsigned int n = sizeof(layout) / sizeof(ButtonData);
    
    _buttons = new WidgetButton[n];
    
    for (unsigned int i = 0; i < n; ++i)
    {
        _buttons[i].setLabel(layout[i].label);
        
        CoordinateC x1(1.0 * layout[i].x / layoutWidth, 0);
        CoordinateC y1(1.0 * layout[i].y / layoutHeight, 0);
        CoordinateC x2(1.0 * (layout[i].x + layout[i].width) / layoutWidth, 0);
        CoordinateC y2(1.0 * (layout[i].y + layout[i].height) / layoutHeight, 0);

        _host->addWidget(&_buttons[i], 
            RectangleC(PointC(x1, y1), PointC(x2, y2))
        );
        
        _buttons[i].pressed.connect(_loop, std::bind(&SparkleKeyboard::keyPressed, this, layout[i].code));
        _buttons[i].released.connect(_loop, std::bind(&SparkleKeyboard::keyReleased, this, layout[i].code));
    }

}

//==================================================================================================

void SparkleKeyboard::start()
{
    _client->connect();
    _loop->run();
}

//==================================================================================================

void SparkleKeyboard::handleConnection()
{
    _client->registerSurfaceFile(surface_name, "/dev/shm/keyboard", _surface->width(), _surface->height());
    _client->setSurfacePosition(surface_name, 0, 600 - 240, _surface->width(), 600);
    _client->addSurfaceDamage(surface_name, 0, 0, _surface->width(), _surface->height());
    fprintf(stderr, "Connected\n");
}

void SparkleKeyboard::hostDamage(int x1, int y1, int x2, int y2)
{
    _client->addSurfaceDamage(surface_name, x1, y1, x2, y2);
}

//==================================================================================================

void SparkleKeyboard::pointerDown(const std::string &name, int slot, int x, int y)
{
    if (name == surface_name)
    {
        _host->pointerDown(slot, x, y);
    }
}

void SparkleKeyboard::pointerUp(const std::string &name, int slot, int x, int y)
{
    if (name == surface_name)
    {
        _host->pointerUp(slot, x, y);
    }
}
    
//==================================================================================================

void SparkleKeyboard::keyPressed(int code)
{
    _client->keyPress(code);
}

void SparkleKeyboard::keyReleased(int code)
{
    _client->keyRelease(code);
}

//==================================================================================================


