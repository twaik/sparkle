#ifndef WERE_SURFACE_H
#define WERE_SURFACE_H

#include "were/were_signal.h"

class WereSurface
{
public:
    virtual ~WereSurface() {}
    //WereSurface();
    
    virtual unsigned char *data() = 0;
    virtual int width() = 0;
    virtual int height() = 0;
    virtual int stride() = 0;
    
werethings:
    WereSignal<void (int x1, int y1, int x2, int y2)> damage;
};

#endif //WERE_SURFACE_H

