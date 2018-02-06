#ifndef SPARKLE_SURFACE_FILE_H
#define SPARKLE_SURFACE_FILE_H

//==================================================================================================

#ifdef __cplusplus

#include "sparkle_surface.h"
#include <string>

class SparkleSurfaceFile : public SparkleSurface
{
public:
    ~SparkleSurfaceFile();
    SparkleSurfaceFile(const std::string &path, int width, int height, bool owner);
    
    const std::string &path();
    int width();
    int height();
    unsigned char *data();
    
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

#endif //__cplusplus

//==================================================================================================

typedef void sparkle_surface_file_t;

#ifdef __cplusplus
extern "C" {
#endif

sparkle_surface_file_t *sparkle_surface_file_create(const char *path, int width, int height, int owner);
void sparkle_surface_file_destroy(sparkle_surface_file_t *surface);

int sparkle_surface_file_width(sparkle_surface_file_t *surface);
int sparkle_surface_file_height(sparkle_surface_file_t *surface);
void *sparkle_surface_file_data(sparkle_surface_file_t *surface);

#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //SPARKLE_SURFACE_FILE

