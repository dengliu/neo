/*
** File Name:	enum_bitmask.hpp
** Author:	Aditya Ramesh
** Date:	12/06/2013
** Contact:	_@adityaramesh.com
*/

#ifndef Z1BDA578F_503C_4EA8_A331_A79CB6F1199C
#define Z1BDA578F_503C_4EA8_A331_A79CB6F1199C

#include <type_traits>

#define DEFINE_ENUM_BITWISE_OPERATORS(name)                         \
	constexpr name operator&(const name& x, const name& y)      \
	{                                                           \
		using integer = std::underlying_type<name>::type;   \
		return static_cast<name>(                           \
			static_cast<integer>(x) &                   \
			static_cast<integer>(y)                     \
		);                                                  \
	}                                                           \
                                                                    \
	constexpr name operator|(const name& x, const name& y)      \
	{                                                           \
		using integer = std::underlying_type<name>::type;   \
		return static_cast<name>(                           \
			static_cast<integer>(x) |                   \
			static_cast<integer>(y)                     \
		);                                                  \
	}                                                           \
                                                                    \
	constexpr name operator^(const name& x, const name& y)      \
	{                                                           \
		using integer = std::underlying_type<name>::type;   \
		return static_cast<name>(                           \
			static_cast<integer>(x) ^                   \
			static_cast<integer>(y)                     \
		);                                                  \
	}                                                           \
                                                                    \
	constexpr name operator~(const name& x)                     \
	{                                                           \
		using integer = std::underlying_type<name>::type;   \
		return static_cast<name>(~static_cast<integer>(x)); \
	}                                                           \
                                                                    \
	name operator&=(name& x, const name& y)                     \
	{                                                           \
		x = x & y;                                          \
		return x;                                           \
	}                                                           \
                                                                    \
	name operator|=(name& x, const name& y)                     \
	{                                                           \
		x = x | y;                                          \
		return x;                                           \
	}                                                           \
                                                                    \
	name operator^(name& x, const name& y)                      \
	{                                                           \
		x = x ^ y;                                          \
		return x;                                           \
	}                                                           \

#endif