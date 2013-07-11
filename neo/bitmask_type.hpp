/*
** File Name:	bitmask_type.hpp
** Author:	Aditya Ramesh
** Date:	07/10/2013
** Contact:	_@adityaramesh.com
*/

#ifndef ZD8851272_1505_4DDB_ABF4_A079249E4950
#define ZD8851272_1505_4DDB_ABF4_A079249E4950

#include <type_traits>

#define NEO_DEFINE_BITMASK_TYPE(bitmask_type)                              \
	                                                                   \
constexpr auto                                                             \
operator&(const bitmask_type x, const bitmask_type y) ->                   \
typename std::underlying_type<bitmask_type>::type                          \
{                                                                          \
	using integer = typename std::underlying_type<bitmask_type>::type; \
	return static_cast<integer>(x) & static_cast<integer>(y);          \
}                                                                          \
                                                                           \
constexpr bitmask_type                                                     \
operator|(const bitmask_type x, const bitmask_type y)                      \
{                                                                          \
	using integer = typename std::underlying_type<bitmask_type>::type; \
	return static_cast<bitmask_type>(                                  \
		static_cast<integer>(x) |                                  \
		static_cast<integer>(y)                                    \
	);                                                                 \
}                                                                          \
                                                                           \
constexpr bitmask_type                                                     \
operator^(const bitmask_type x, const bitmask_type y)                      \
{                                                                          \
	using integer = typename std::underlying_type<bitmask_type>::type; \
	return static_cast<bitmask_type>(                                  \
		static_cast<integer>(x) ^                                  \
		static_cast<integer>(y)                                    \
	);                                                                 \
}                                                                          \
                                                                           \
constexpr bitmask_type                                                     \
operator~(const bitmask_type x)                                            \
{                                                                          \
	using integer = typename std::underlying_type<bitmask_type>::type; \
	return static_cast<bitmask_type>(~static_cast<integer>(x));        \
}                                                                          \

#endif
