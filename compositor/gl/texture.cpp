#include "texture.h"
#include <stddef.h>

Texture::~Texture()
{
    glDeleteTextures(1, &_texture);
}

Texture::Texture()
{
    _texture = 0;
    _width = 0;
    _height = 0;
    
    glGenTextures(1, &_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

GLuint Texture::id()
{
    return _texture;
}

int Texture::width()
{
    return _width;
}

int Texture::height()
{
    return _height;
}

void Texture::resize(int width, int height)
{
    if (_width == width && _height == height)
        return;

    if (width > 0 && height > 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, NULL);
    }

    _width = width;
    _height = height;
}

