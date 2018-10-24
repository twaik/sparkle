#ifndef SPARKLE_SURFACE_FILE_H
#define SPARKLE_SURFACE_FILE_H

#include "were-graphics/were_surface.h"
#include <string>

/* ================================================================================================================== */

class SparkleSurfaceFile : public WereSurface
{
public:
    ~SparkleSurfaceFile();
    SparkleSurfaceFile(const std::string &path, int width, int height, bool owner);

    const std::string &path();
    unsigned char *data();
    int width();
    int height();
    int stride();

private:
    void map();
    void unmap();

private:
    std::string _path;
    int _width;
    int _height;
    bool _owner;

    int _fd;
    unsigned char *_data;
};

/* ================================================================================================================== */

#endif /* SPARKLE_SURFACE_FILE */
