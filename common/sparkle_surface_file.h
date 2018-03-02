#ifndef SPARKLE_SURFACE_FILE_H
#define SPARKLE_SURFACE_FILE_H

//==================================================================================================

#ifdef __cplusplus

#include <string>

class SparkleSurfaceFile
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

#else

struct SparkleSurfaceFile;
typedef struct SparkleSurfaceFile SparkleSurfaceFile;

#endif //__cplusplus

//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif

SparkleSurfaceFile *sparkle_surface_file_create(const char *path, int width, int height, int owner);
void sparkle_surface_file_destroy(SparkleSurfaceFile *surface);

int sparkle_surface_file_width(SparkleSurfaceFile *surface);
int sparkle_surface_file_height(SparkleSurfaceFile *surface);
void *sparkle_surface_file_data(SparkleSurfaceFile *surface);

#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //SPARKLE_SURFACE_FILE

