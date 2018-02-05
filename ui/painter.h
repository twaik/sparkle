#ifndef PAINTER_H
#define PAINTER_H

#include "font_default.h"
#include <string.h>

    
inline void dot(uint32_t *buffer, int stride, int x, int y, uint32_t color)
{
    buffer[y * stride + x] = color;
}
    
inline void drawRectangle(uint32_t *buffer, int stride, int x1, int y1, int x2, int y2, uint32_t color)
{
    for (int x = x1; x <= x2; ++x)
    {
        dot(buffer, stride, x, y1, color);
        dot(buffer, stride, x, y2, color);
    }
        
    for (int y = y1; y <= y2; ++y)
    {
        dot(buffer, stride, x1, y, color);
        dot(buffer, stride, x2, y, color);
    }
}
    
inline void drawRectangle2(uint32_t *buffer, int stride, int x1, int y1, int x2, int y2, uint32_t color)
{
    for (int x = x1 + 1; x <= x2 - 1; ++x)
    {
        dot(buffer, stride, x, y1, color);
        dot(buffer, stride, x, y2, color);
    }
        
    for (int y = y1 + 1; y <= y2 - 1; ++y)
    {
        dot(buffer, stride, x1, y, color);
        dot(buffer, stride, x2, y, color);
    }
}
    
inline void drawCharacter(uint32_t *buffer, int stride, int x, int y, char c)
{
    uint32_t *destination = &buffer[y * stride + x];
    const unsigned char *source = &font_default[c * 9 * 16];
    
    for (int y = 0; y < 16; ++y)
    {
        for (int x = 0; x < 9; ++x)
        {
            if (source[y * 9 + x] == 0x00)
            {
                //dot(destination, stride, x, y, 0xFF000000);
            }
            else
            {
                dot(destination, stride, x, y, 0xFFAAAAAA);
            }
        }
    }
}
    
inline void drawString(uint32_t *buffer, int stride, int x, int y, const char *s)
{
    for (unsigned int c = 0; c < strlen(s); ++c)
    {
        drawCharacter(buffer, stride, x + c * 9, y, s[c]);
    }
}

#endif //PAINTER_H

