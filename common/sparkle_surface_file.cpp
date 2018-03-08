#include "sparkle_surface_file.h"
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

//==================================================================================================

SparkleSurfaceFile::~SparkleSurfaceFile()
{
    unmap();

    if (_owner)
        unlink(_path.c_str());
}

SparkleSurfaceFile::SparkleSurfaceFile(const std::string &path, int width, int height, bool owner)
{
    _path = path;
    _width = width;
    _height = height;
    _owner = owner;
    
    _fd = -1;
    _data = 0;
    
    map();
}

//==================================================================================================

void SparkleSurfaceFile::map()
{
    if (_data != 0)
        return;
    

    int fd = -1;
    
    if (_owner)
        fd = open(_path.c_str(), O_RDWR | O_CREAT);
    else
        fd = open(_path.c_str(), O_RDWR);

    if (fd == -1)
        throw std::runtime_error("[SparkleSurface::map] Failed to open file.");
    
    if (_owner)
    {
        if (ftruncate(fd, _width * _height * 4) == -1)
            throw std::runtime_error("[SparkleSurface::map] Failed to resize file.");

        if (fchmod(fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1)
            throw std::runtime_error("[SparkleSurface::map] Failed to set file permissions.");
    }
    
    void *data = mmap(NULL, _width * _height * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED)
        throw std::runtime_error("[SparkleSurface::map] Failed to map file.");
    
    _data = reinterpret_cast<unsigned char *>(data);
    _fd = fd;
}

void SparkleSurfaceFile::unmap()
{
    if (_data == 0)
        return;
    
    munmap(_data, _width * _height * 4);
    close(_fd);

    _data = 0;
    _fd = -1;
}

//==================================================================================================

const std::string &SparkleSurfaceFile::path()
{
    return _path;
}

unsigned char *SparkleSurfaceFile::data()
{
    return _data;
}

int SparkleSurfaceFile::width()
{
    return _width;
}

int SparkleSurfaceFile::height()
{
    return _height;
}

int SparkleSurfaceFile::stride()
{
    return width();
}
    
//==================================================================================================

SparkleSurfaceFile *sparkle_surface_file_create(const char *path, int width, int height, int owner)
{
    return new SparkleSurfaceFile(path, width, height, owner);
}

void sparkle_surface_file_destroy(SparkleSurfaceFile *surface)
{
    delete surface;
}

int sparkle_surface_file_width(SparkleSurfaceFile *surface)
{
    return surface->width();
}

int sparkle_surface_file_height(SparkleSurfaceFile *surface)
{
    return surface->height();
}

void *sparkle_surface_file_data(SparkleSurfaceFile *surface)
{
    return surface->data();
}

//==================================================================================================

