/*
** File Name: open_mode.hpp
** Author:    Aditya Ramesh
** Date:      07/09/2013
** Contact:   _@adityaramesh.com
*/

#ifndef ZBBA9CC37_2920_4B73_8CD5_26374197953F
#define ZBBA9CC37_2920_4B73_8CD5_26374197953F

#include <type_traits>
#include <ccbase/platform.hpp>
#include <neo/bitmask_enum.hpp>

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX || \
    PLATFORM_KERNEL == PLATFORM_KERNEL_XNU
	// For `O_*` macros.
	#include <fcntl.h>
#endif

namespace neo {

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX || \
    PLATFORM_KERNEL == PLATFORM_KERNEL_XNU

enum class open_mode : int
{
	read     = O_RDONLY,
	write    = O_WRONLY,
	create   = O_CREAT,
	truncate = O_TRUNC
};

#endif

static constexpr open_mode read     = open_mode::read;
static constexpr open_mode write    = open_mode::write;
static constexpr open_mode create   = open_mode::create;
static constexpr open_mode truncate = open_mode::truncate;

DEFINE_ENUM_BITWISE_OPERATORS(open_mode)

constexpr int
posix(const open_mode& m)
{
	/*
	** `O_RDWR` is in fact not defined as `O_RDONLY | O_WRONLY`. So if the
	** open mode has both the `read` and `write` bits set, then we need to
	** retrieve the other bits associated with the read mode, and bitwise-or
	** them with `O_RDWR`.
	**
	** Otherwise, the open mode enum value is defined to be the same as the
	** POSIX open mode value, so we can just return the underlying integer
	** representation.
	*/

	using integer = std::underlying_type<open_mode>::type;

	return !!(m & read) && !!(m & write) ?
	O_RDWR | static_cast<integer>(m & ~(read | write)) :
	static_cast<integer>(m);
}

}

#endif
