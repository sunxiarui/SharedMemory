#ifndef _MMAP_SHARED_H
#define _MMAP_SHARED_H

#include <stdlib.h>
#include <stdint.h>

/*
	共享大内存块封装类型
*/
class SharedMemory
{
	SharedMemory(const SharedMemory&);
public:
	SharedMemory();
	~SharedMemory();

	int create(const char* name, size_t size);

	int open(const char* name);

	void* get_ptr() const {
		return basep;
	}

	void* address_from_offset(uintptr_t off) const {
		return ((char*)basep) + off;
	}
private:
	uintptr_t hdl;
	void* basep;
#ifdef __linux
	size_t mapsize;
#endif
};



#endif // !_MMAP_SHARED_H

