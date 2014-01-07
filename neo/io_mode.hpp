/*
** File Name:	io_mode.hpp
** Author:	Aditya Ramesh
** Date:	07/10/2013
** Contact:	_@adityaramesh.com
*/

#ifndef Z888D107F_0EF8_4F90_B3D2_D09E7D9AEE57
#define Z888D107F_0EF8_4F90_B3D2_D09E7D9AEE57

#include <neo/bitmask_enum.hpp>

namespace neo {

enum class io_mode : int
{
	input,
	output,
	scatter_input,
	scatter_output
};

static constexpr io_mode input          = io_mode::input;
static constexpr io_mode output         = io_mode::output;
static constexpr io_mode scatter_input  = io_mode::scatter_input;
static constexpr io_mode scatter_output = io_mode::scatter_output;

DEFINE_ENUM_BITWISE_OPERATORS(io_mode)

}

#endif
