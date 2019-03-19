#ifndef SPARKLE_SURFACE_SHM_H
#define SPARKLE_SURFACE_SHM_H

#include "were-graphics/were_surface.h"

/* ================================================================================================================== */

class SparkleSurfaceAshmem : public WereSurface
{
public:
    ~SparkleSurfaceAshmem();
    SparkleSurfaceAshmem(int width, int height);
    SparkleSurfaceAshmem(int fd, int width, int height);

    unsigned char *data() {return data_;}
    int fd() { return fd_; }
    int width() {return width_;}
    int height() {return height_;}
    int stride() {return width_;}

private:
	void map();
	void unmap();
    int width_;
    int height_;
    bool owner_;

    int fd_;
    unsigned char *data_;
};

/* ================================================================================================================== */

#endif /* SPARKLE_SURFACE_SHM_H */
