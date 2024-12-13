#include "mmap_shared.h"
#ifdef __linux
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#else//windows
#include <windows.h>
#include <intsafe.h>
#endif

SharedMemory::SharedMemory():hdl(0),basep(0)
{
#ifdef __linux
	mapsize = 0;
#endif
}

#ifdef __linux

SharedMemory::~SharedMemory()
{
	if (basep) {
		munmap(basep, mapsize);
	}
	if (hdl) {
		close(hdl);
	}
}

int SharedMemory::create(const char* name, size_t size)
{
	if (hdl) return 1;

	int fd = shm_open(name, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd == -1) {
		return -1;
	}
	
	hdl = fd;	
	if (ftruncate(fd, size) < 0) {
		return -2;
	}
	mapsize = size;
	basep = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
	if (basep == MAP_FAILED) {
		basep = 0;
		return -3;
	}
	return 0;
}

int SharedMemory::open(const char* name)
{
	if (hdl) return 1;

	int fd = shm_open(name, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd == -1) {
		return -1;
	}

	hdl = fd;
	struct stat statbuff;
	if (fstat(fd, &statbuff) < 0) {
		return -2;
	}
	mapsize = statbuff.st_size;
	basep = mmap(NULL, mapsize, PROT_WRITE, MAP_SHARED, fd, 0);
	if (basep == MAP_FAILED) {
		basep = 0;
		return -3;
	}
	return 0;
}


#else //windows
SharedMemory::~SharedMemory()
{
	if (basep) {
		UnmapViewOfFile(basep);
	}
	if (hdl) {
		CloseHandle((HANDLE)hdl);
	}
}

int SharedMemory::create(const char* name, size_t size)
{
	if (hdl) return 1;
	
	HANDLE h = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, HIDWORD(size), LODWORD(size), name);
	if (h == INVALID_HANDLE_VALUE) {
		return -1;
	}
	hdl = (uintptr_t)h;
	basep = MapViewOfFile(h, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (basep == 0) return -2;
	return 0;
}

int SharedMemory::open(const char* name)
{
	if (hdl) return 1;

	HANDLE h = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name);
	if (h == INVALID_HANDLE_VALUE) {
		return -1;
	}
	hdl = (uintptr_t)h;
	basep = MapViewOfFile(h, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (basep == 0) return -2;
	return 0;
}


#endif
