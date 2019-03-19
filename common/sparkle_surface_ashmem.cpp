#include "sparkle_surface_ashmem.h"
#include <unistd.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <linux/shm.h>
#include <linux/ashmem.h>
#ifndef shmid_ds
# define shmid_ds shmid64_ds
#endif

static int __unused ashmem_get_size_region(int fd)
{
	//int ret = __ashmem_is_ashmem(fd, 1);
	//if (ret < 0) return ret;
	return TEMP_FAILURE_RETRY(ioctl(fd, ASHMEM_GET_SIZE, NULL));
}

/*
 * From https://android.googlesource.com/platform/system/core/+/master/libcutils/ashmem-dev.c
 *
 * ashmem_create_region - creates a new named ashmem region and returns the file
 * descriptor, or <0 on error.
 *
 * `name' is the label to give the region (visible in /proc/pid/maps)
 * `size' is the size of the region, in page-aligned bytes
 */
static int ashmem_create_region(char const* name, size_t size)
{
	int fd = open("/dev/ashmem", O_RDWR);
	if (fd < 0) return fd;

	char name_buffer[ASHMEM_NAME_LEN] = {0};
	strncpy(name_buffer, name, sizeof(name_buffer));
	name_buffer[sizeof(name_buffer)-1] = 0;

	int ret = ioctl(fd, ASHMEM_SET_NAME, name_buffer);
	if (ret < 0) goto error;

	ret = ioctl(fd, ASHMEM_SET_SIZE, size);
	if (ret < 0) goto error;

	return fd;
error:
	close(fd);
	return ret;
}

/* ================================================================================================================== */

SparkleSurfaceAshmem::~SparkleSurfaceAshmem()
{
	unmap();
	close(fd_);
	fd_ = 0;
}

SparkleSurfaceAshmem::SparkleSurfaceAshmem(int width, int height) {
    width_ = width;
    height_ = height;
    owner_ = false;
    data_ = nullptr;
    
    fd_ = ashmem_create_region("sparkle_region", width_ * height_ * 4);
    map();
}
SparkleSurfaceAshmem::SparkleSurfaceAshmem(int fd, int width, int height)
{
    width_ = width;
    height_ = height;
    owner_ = false;
    data_ = nullptr;
	fd_ = fd;
	
	map();
}

/* ================================================================================================================== */

void SparkleSurfaceAshmem::map()
{
    if (data_ != nullptr)
        return;

    void *data = mmap(NULL, width_ * height_ * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (data == MAP_FAILED)
        throw WereException("[%p][%s] Failed to mmap file.", this, __PRETTY_FUNCTION__);

    data_ = reinterpret_cast<unsigned char *>(data);
}

void SparkleSurfaceAshmem::unmap()
{
    if (data_ == nullptr)
        return;

    munmap(data_, width_ * height_ * 4);

    data_ = nullptr;
}

/* ================================================================================================================== */
