#ifndef SPARKLE_SURFACE_SHM_H
#define SPARKLE_SURFACE_SHM_H

#include "were-graphics/were_surface.h"
#include <string>

/* ================================================================================================================== */

class SparkleSurfaceShm : public WereSurface
{
public:
    ~SparkleSurfaceShm();
    SparkleSurfaceShm(key_t key, int width, int height, bool owner);

    key_t key() {return key_;}
    unsigned char *data() {return data_;}
    int width() {return width_;}
    int height() {return height_;}
    int stride() {return width_;}

private:

    int width_;
    int height_;
    bool owner_;

    key_t key_;
    int shmId_;
    unsigned char *data_;
};

/* ================================================================================================================== */

#endif /* SPARKLE_SURFACE_SHM */
