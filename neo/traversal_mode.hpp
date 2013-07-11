/*
** File Name:	traversal_mode.hpp
** Author:	Aditya Ramesh
** Date:	07/09/2013
** Contact:	_@adityaramesh.com
*/

#ifndef Z6FF0F77F_9481_4BAC_9167_138A93A6487C
#define Z6FF0F77F_9481_4BAC_9167_138A93A6487C

#include <neo/bitmask_type.hpp>

namespace cc
{

enum class traversal_mode : unsigned
{
	sequential,
	random
};

static constexpr traversal_mode sequential = traversal_mode::sequential;
static constexpr traversal_mode random     = traversal_mode::random;

NEO_DEFINE_BITMASK_TYPE(traversal_mode)

}

#endif
