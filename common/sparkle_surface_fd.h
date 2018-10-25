#ifndef SPARKLE_SURFACE_FD_H
#define SPARKLE_SURFACE_FD_H

#include "were-graphics/were_surface.h"
#include <string>

/* ================================================================================================================== */

class SparkleSurfaceFd : public WereSurface
{
public:
    ~SparkleSurfaceFd();
    SparkleSurfaceFd(const std::string &path, int width, int height);
    SparkleSurfaceFd(int fd, int width, int height);

    const std::string &path() {return path_;}
    unsigned char *data() {return data_;}
    int width() {return width_;}
    int height() {return height_;}
    int stride() {return width_;}

private:
    void map();
    void unmap();

private:
    std::string path_;
    int width_;
    int height_;

    int fd_;
    unsigned char *data_;
};

/* ================================================================================================================== */

#endif /* SPARKLE_SURFACE_FD */
