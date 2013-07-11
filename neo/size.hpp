/*
** File Name:	size.hpp
** Author:	Aditya Ramesh
** Date:	07/09/2013
** Contact:	_@adityaramesh.com
*/

#ifndef Z1A92501E_D056_4195_8AA2_4DE209C3BA60
#define Z1A92501E_D056_4195_8AA2_4DE209C3BA60

#include <system_error>
#include <ccbase/platform.hpp>

#if CCBASE_KERNEL == CCBASE_KERNEL_LINUX || \
    CCBASE_KERNEL == CCBASE_KERNEL_MACH
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#endif

namespace neo
{

#if CCBASE_KERNEL == CCBASE_KERNEL_LINUX || \
    CCBASE_KERNEL == CCBASE_KERNEL_MACH

off_t size(const int fd)
{
	struct stat buf;
	auto r = ::fstat(fd, &buf);
	if (r == -1) {
		throw std::system_error{errno, std::system_category()};
	}
	return buf.st_size;
}

#endif

}

#endif
