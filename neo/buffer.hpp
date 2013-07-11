/*
** File Name:	buffer.hpp
** Author:	Aditya Ramesh
** Date:	07/09/2013
** Contact:	_@adityaramesh.com
*/

#ifndef ZD5C4D90F_B11B_422E_ACF1_F65DE7A910C0
#define ZD5C4D90F_B11B_422E_ACF1_F65DE7A910C0

#include <array>
#include <chrono>
#include <neo/handle.hpp>
#include <neo/open_mode.hpp>
#include <neo/traversal_mode.hpp>

namespace neo
{

template <
	open_mode      IOMode,
	open_mode      OpenMode,
	traversal_mode TraversalMode,
	bool           Asynchronous,
	bool           Cache
>
class buffer;

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX



#elif PLATFORM_KERNEL == PLATFORM_KERNEL_MACH



#endif

}

#endif
