#ifndef TEXTURE_H
#define TEXTURE_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

class Texture
{
public:
    ~Texture();
    Texture();

    GLuint id();
    int width();
    int height();
    
    void resize(int width, int height);

private:
    GLuint _texture;
    int _width;
    int _height;
};

#endif //TEXTURE_H

