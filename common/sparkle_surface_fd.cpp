#include "sparkle_surface_fd.h"
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

/* ================================================================================================================== */

SparkleSurfaceFd::~SparkleSurfaceFd()
{
    unmap();

    close(fd_);

    if (path_.size() != 0)
        unlink(path_.c_str());
}

SparkleSurfaceFd::SparkleSurfaceFd(const std::string &path, int width, int height)
{
    path_ = path;
    width_ = width;
    height_ = height;
    fd_ = -1;
    data_ = nullptr;

    int fd = open(path_.c_str(), O_RDWR | O_CREAT);

    if (fd == -1)
        throw WereException("[%p][%s] Failed to open file.", this, __PRETTY_FUNCTION__);

    if (ftruncate(fd, width_ * height_ * 4) == -1)
        throw WereException("[%p][%s] Failed to resize file.", this, __PRETTY_FUNCTION__);

    if (fchmod(fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1)
        throw WereException("[%p][%s] Failed to chmod file.", this, __PRETTY_FUNCTION__);

    fd_ = fd;

    map();
}

SparkleSurfaceFd::SparkleSurfaceFd(int fd, int width, int height)
{
    width_ = width;
    height_ = height;
    fd_ = -1;
    data_ = nullptr;

    fd_ = fd;

    map();
}

/* ================================================================================================================== */

void SparkleSurfaceFd::map()
{
    if (data_ != nullptr)
        return;

    void *data = mmap(NULL, width_ * height_ * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (data == MAP_FAILED)
        throw WereException("[%p][%s] Failed to mmap file.", this, __PRETTY_FUNCTION__);

    data_ = reinterpret_cast<unsigned char *>(data);
}

void SparkleSurfaceFd::unmap()
{
    if (data_ == nullptr)
        return;

    munmap(data_, width_ * height_ * 4);

    data_ = nullptr;
}

/* ================================================================================================================== */
