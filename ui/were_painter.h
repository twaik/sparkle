#ifndef WERE_PAINTER_H
#define WERE_PAINTER_H

#include "font_default.h"
#include <stdint.h>
#include <string.h>

#include <stdio.h>

class WerePainter
{
public:
    ~WerePainter()
    {
    }
    
    WerePainter(unsigned char *buffer, int stride)
    {
        _buffer = reinterpret_cast<uint32_t *>(buffer);
        _stride = stride;
    }
    
    void setColor(uint32_t color)
    {
        _color = color;
    }
    
    void dot(int x, int y)
    {
        _buffer[y * _stride + x] = _color;
    }
    
    void fill(int x1, int y1, int x2, int y2)
    {
        for (int y = y1; y <= y2; ++y)
        {
            for (int x = x1; x <= x2; ++x)
            {
                dot(x, y);
            }
        }
    }
    
    void drawRectangle1(int x1, int y1, int x2, int y2)
    {
        for (int x = x1; x <= x2; ++x)
        {
            dot(x, y1);
            dot(x, y2);
        }
        
        for (int y = y1; y <= y2; ++y)
        {
            dot(x1, y);
            dot(x2, y);
        }
    }
    
    void drawRectangle2(int x1, int y1, int x2, int y2)
    {
        for (int x = x1 + 1; x <= x2 - 1; ++x)
        {
            dot(x, y1);
            dot(x, y2);
        }
        
        for (int y = y1 + 1; y <= y2 - 1; ++y)
        {
            dot(x1, y);
            dot(x2, y);
        }
    }
    
    void drawCharacter(int x, int y, char c)
    {
        const unsigned char *source = &font_default[c * 9 * 16];
    
        for (int _y = 0; _y < 16; ++_y)
        {
            for (int _x = 0; _x < 9; ++_x)
            {
                if (source[_y * 9 + _x] == 0x00)
                {
                }
                else
                {
                    dot(x + _x, y + _y);
                }
            }
        }
    }
    
    void drawString(int x, int y, const char *s)
    {
        int _x = 0;
        int _y = 0;
        int length = strlen(s);
        
        for (int i = 0; i < length; ++i)
        {
            if (s[i] == '\n')
            {
                _y += 16;
                _x = 0;
            }
            else
            {
                drawCharacter(x + _x, y + _y, s[i]);
                _x += 9;
            }
        }
    }
    
    int charWidth()
    {
        return 9;
    }
    
    int charHeight()
    {
        return 16;
    }
    
    void stringSize(const char *s, int *width, int *height)
    {
        *width = 0;
        *height = 16;
        
        int _x = 0;
        int length = strlen(s);
        
        for (int i = 0; i < length; ++i)
        {
            if (s[i] == '\n')
            {
                *height += 16;
                _x = 0;
            }
            else
            {
                _x += 9;
                if (*width < _x)
                    *width = _x;
            }
        }
    }

    
private:
    uint32_t *_buffer;
    int _stride;
    
    uint32_t _color;
};

#endif //WERE_PAINTER_H

