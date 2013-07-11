/*
** File Name:	io_mode.hpp
** Author:	Aditya Ramesh
** Date:	07/10/2013
** Contact:	_@adityaramesh.com
*/

#ifndef Z888D107F_0EF8_4F90_B3D2_D09E7D9AEE57
#define Z888D107F_0EF8_4F90_B3D2_D09E7D9AEE57

#include <neo/bitmask_type.hpp>

namespace neo
{

enum class io_mode : int
{
	input,
	output
};

static constexpr io_mode input = io_mode::input;
static constexpr io_mode output = io_mode::output;

NEO_DEFINE_BITMASK_TYPE(io_mode)

}

#endif
