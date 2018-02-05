#include "window_keyboard.h"

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
    
    {0, 6, 4, 4, 49, "~"}, {4, 6, 4, 4, 10, "1"}, {8, 6, 4, 4, 11, "2"}, {12, 6, 4, 4, 12, "3"}, {16, 6, 4, 4, 13, "4"}, {20, 6, 4, 4, 14, "5"}, {24, 6, 4, 4, 15, "6"}, {28, 6, 4, 4, 16, "7"}, {32, 6, 4, 4, 17, "8"}, {36, 6, 4, 4, 18, "9"}, {40, 6, 4, 4, 19, ""}, {44, 6, 4, 4, 20, ""}, {48, 6, 4, 4, 21, ""},
    {52, 6, 8, 4, 22, ""},
    
    {0, 10, 6, 4, 23, "Tab"},
    {6, 10, 4, 4, 24, "Q"}, {10, 10, 4, 4, 25, "W"}, {14, 10, 4, 4, 26, "E"}, {18, 10, 4, 4, 27, "R"}, {22, 10, 4, 4, 28, "T"}, {26, 10, 4, 4, 29, "Y"}, {30, 10, 4, 4, 30, "U"}, {34, 10, 4, 4, 31, "I"}, {38, 10, 4, 4, 32, "O"}, {42, 10, 4, 4, 33, "P"}, {46, 10, 4, 4, 34, ""}, {50, 10, 4, 4, 35, ""},
    {54, 10, 6, 4, 51, ""},
    
    {0, 14, 7, 4, 66, "Caps"},
    {7, 14, 4, 4, 38, "A"}, {11, 14, 4, 4, 39, "S"}, {15, 14, 4, 4, 40, "D"}, {19, 14, 4, 4, 41, "F"}, {23, 14, 4, 4, 42, "G"}, {27, 14, 4, 4, 43, "H"}, {31, 14, 4, 4, 44, "J"}, {35, 14, 4, 4, 45, "K"}, {39, 14, 4, 4, 46, "L"}, {43, 14, 4, 4, 47, ""}, {47, 14, 4, 4, 48, ""},
    {51, 14, 9, 4, 36, ""},
    
    {0, 18, 9, 4, 50, ""},
    {9, 18, 4, 4, 52, "Z"}, {13, 18, 4, 4, 53, "X"}, {17, 18, 4, 4, 54, "C"}, {21, 18, 4, 4, 55, "V"}, {25, 18, 4, 4, 56, "B"}, {29, 18, 4, 4, 57, "N"}, {33, 18, 4, 4, 58, "M"}, {37, 18, 4, 4, 59, ""}, {41, 18, 4, 4, 60, ""}, {45, 18, 4, 4, 61, ""},
    {49, 18, 11, 4, 62, ""},
    
    {0, 22, 5, 4, 37, "Ctrl"}, {5, 22, 5, 4, 133, "Win"}, {10, 22, 5, 4, 64, "Alt"},
    {15, 22, 25, 4, 65, ""},
    {40, 22, 5, 4, 108, "Alt"}, {45, 22, 5, 4, 135, "Win"}, {50, 22, 5, 4, 9, ""}, {55, 22, 5, 4, 105, "Ctrl"},
    
    {62, 0, 4, 4, 107, ""}, {66, 0, 4, 4, 78, ""}, {70, 0, 4, 4, 127, ""},
    {62, 6, 4, 4, 118, ""}, {66, 6, 4, 4, 110, ""}, {70, 6, 4, 4, 112, ""},
    {62, 10, 4, 4, 119, ""}, {66, 10, 4, 4, 115, ""}, {70, 10, 4, 4, 117, ""},
    
    {66, 18, 4, 4, 98, ""}, {62, 22, 4, 4, 100, ""}, {66, 22, 4, 4, 104, ""}, {70, 22, 4, 4, 102, ""},
};

const int layoutWidth = 74;
const int layoutHeight = 26;

void KeyboardButton::setCode(int code)
{
    _code = code;
}

int KeyboardButton::code()
{
    return _code;
}

WindowKeyboard::~WindowKeyboard()
{
    delete[] _buttons;
}

WindowKeyboard::WindowKeyboard() :
    WindowHost(layoutWidth * 8, layoutHeight * 8)
{
    unsigned int n = sizeof(layout) / sizeof(ButtonData);
    
    _buttons = new KeyboardButton[n];
    
    for (unsigned int i = 0; i < n; ++i)
    {
        _buttons[i].setCode(layout[i].code);
        _buttons[i].setLabel(layout[i].label);
        
        CoordinateC x1(1.0 * layout[i].x / layoutWidth, 0);
        CoordinateC y1(1.0 * layout[i].y / layoutHeight, 0);
        CoordinateC x2(1.0 * (layout[i].x + layout[i].width) / layoutWidth, 0);
        CoordinateC y2(1.0 * (layout[i].y + layout[i].height) / layoutHeight, 0);

        addWidget(&_buttons[i], 
            RectangleC(PointC(x1, y1), PointC(x2, y2))
        );
        
        _buttons[i].pressed.connect(this, &WindowKeyboard::buttonPressed);
        _buttons[i].released.connect(this, &WindowKeyboard::buttonReleased);
    }
}

void WindowKeyboard::buttonPressed(WidgetButton *button)
{
    KeyboardButton *k_button = static_cast<KeyboardButton *>(button);
    keyDown(k_button->code());
}

void WindowKeyboard::buttonReleased(WidgetButton *button)
{
    KeyboardButton *k_button = static_cast<KeyboardButton *>(button);
    keyUp(k_button->code());
}
