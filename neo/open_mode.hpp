/*
** File Name:	open_mode.hpp
** Author:	Aditya Ramesh
** Date:	07/09/2013
** Contact:	_@adityaramesh.com
*/

#ifndef ZBBA9CC37_2920_4B73_8CD5_26374197953F
#define ZBBA9CC37_2920_4B73_8CD5_26374197953F

#include <ccbase/platform.hpp>
#include <neo/bitmask_type.hpp>

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX || \
    PLATFORM_KERNEL == PLATFORM_KERNEL_MACH

#include <fcntl.h>

#endif

namespace neo
{

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX || \
    PLATFORM_KERNEL == PLATFORM_KERNEL_MACH

enum class open_mode : int
{
	read     = O_RDONLY,
	write    = O_WRONLY,
	create   = O_CREAT,
	truncate = O_TRUNC
};

#endif

static constexpr open_mode read = open_mode::read;
static constexpr open_mode write = open_mode::write;
static constexpr open_mode create = open_mode::create;
static constexpr open_mode truncate = open_mode::truncate;

NEO_DEFINE_BITMASK_TYPE(open_mode)

}

#endif
