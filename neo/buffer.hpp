/*
** File Name:	buffer.hpp
** Author:	Aditya Ramesh
** Date:	07/09/2013
** Contact:	_@adityaramesh.com
*/

#ifndef ZD5C4D90F_B11B_422E_ACF1_F65DE7A910C0
#define ZD5C4D90F_B11B_422E_ACF1_F65DE7A910C0

#include <neo/context.hpp>
#include <neo/handle.hpp>
#include <neo/operation.hpp>
#include <neo/io_mode.hpp>
#include <neo/open_mode.hpp>
#include <neo/system.hpp>
#include <neo/traversal_mode.hpp>

namespace neo {

template <
	io_mode        IOMode,
	open_mode      OpenMode,
	traversal_mode TraversalMode,
	bool           Asynchronous,
	bool           UseDirectIO
>
class buffer;

template <
	io_mode        IOMode,
	open_mode      OpenMode,
	traversal_mode TraversalMode,
	bool           Asynchronous,
	bool           UseDirectIO
>
class buffer<input, OpenMode, sequential, Asynchronous, UseDirectIO>
{
public:
	using handle_type      = handle<OpenMode, UseDirectIO>;
	using context_type     = context<input, Asynchronous>;
	using input_operation  = operation<input, Asynchronous>;
	using output_operation = operation<output, Asynchronous>;

	using size_type   = typename handle::size_type;
	using offset_type = typename handle::offset_type;
	using buffer_type = typename handle::buffer_type;
private:
	handle_type h;
	input_operation i1;
	input_operation i2;
	buffer_type b1;
	buffer_type b2;
	size_type n;
	bool db{Asynchronous};
	context_type c;
public:
	explicit buffer(
		const char* path,
		const size_type n = default_input_buffer_size
	) : h{path}, i1{h}, i2{h}, n{n}, c{}
	{
		if (h.file_size() <= n) {
			db = false;
			h.allocate(b1, n);
			b2.reset(nullptr);
			i1.buffer(b1.get());
		}
		else {
			h.allocate(b1, n);
			h.allocate(b2, n);
			i1.buffer(b1.get());
			i2.buffer(b2.get());
		}
	}

	template <class T = void>
	auto fill(const offset_type off, const T* = 0) ->
	std::enable_if<>
	{

	}

	void flush(const offset_type off, const T* = 0) ->
	std::enable_if<>
	{

	}

	void input_strategy(const traversal_mode m)
	{

	}

	void output_strategy(const traversal_mode m)
	{

	}
};

}

#endif
