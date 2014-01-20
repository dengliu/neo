/*
** File Name: buffer.hpp
** Author:    Aditya Ramesh
** Date:      07/09/2013
** Contact:   _@adityaramesh.com
*/

#ifndef ZD5C4D90F_B11B_422E_ACF1_F65DE7A910C0
#define ZD5C4D90F_B11B_422E_ACF1_F65DE7A910C0

#include <neo/buffer_size.hpp>
#include <neo/context.hpp>
#include <neo/handle.hpp>
#include <neo/request.hpp>
#include <neo/io_mode.hpp>
#include <neo/open_mode.hpp>
#include <neo/traversal_mode.hpp>

namespace neo {

template <
	io_mode        IOMode,
	open_mode      OpenMode,
	traversal_mode TraversalMode,
	bool           UseDirectIO,
	bool           UseAsyncIO
>
class buffer;

template <
	io_mode        IOMode,
	open_mode      OpenMode,
	traversal_mode TraversalMode,
	bool           UseDirectIO,
	bool           UseAsyncIO
>
class buffer<input, OpenMode, sequential, UseDirectIO, UseAsyncIO>
{
	static_assert(!UseAsyncIO || (UseDirectIO && UseAsyncIO),
		"Asynchronous IO requires direct IO");
public:
	using handle_type   = handle<OpenMode, UseDirectIO>;
	using context_type  = context<input, UseAsyncIO>;
	using read_request  = operation<input, UseAsyncIO>;
	using write_request = operation<output, UseAsyncIO>;

	using size_type   = typename handle::size_type;
	using offset_type = typename handle::offset_type;
	using buffer_type = typename handle::buffer_type;
private:
	handle_type  h;
	read_request rr1;
	read_request rr2;
	buffer_type  buf1;
	buffer_type  buf2;
	size_type    n;
	bool         use_dbuf{UseAsyncIO};
	context_type ctx{};
public:
	explicit buffer(
		const char* path,
		const size_type n = detail::default_input_buffer_size
	) : h{path}, rr1{h}, rr2{h}, n{n}
	{
		if (!UseDirectIO || h.file_size() <= n) {
			db = false;
			h.allocate(buf1, n);
			buf2.reset(nullptr);
			rr1.buffer(buf1.get());
			rr1.count(n);
		}
		else {
			h.allocate(buf1, n);
			h.allocate(buf2, n);
			rr1.buffer(buf1.get());
			rr2.buffer(buf2.get());
			rr1.count(n);
			rr2.count(n);
		}
	}

	buffer(const buffer&)            = delete;
	buffer(buffer&&)                 = delete;
	buffer& operator=(const buffer&) = delete;
	buffer& operator=(buffer&&)      = delete;

	void fill(const offset_type off)
	{
		if (!UseDirectIO || !use_dbuf) {
			rr1.offset(off);
			ctx.submit(rr1);
		}
		else {

		}
	}

	void input_strategy(const traversal_mode m)
	{
		/*
		** If m == forward
		** 	use_db <- true
		** 	allocate buf2 if not allocated
		** If m == random
		** 	use_db <- false
		** 	If buf2 is the active buffer, swap it with buf1
		*/
	}
};

}

#endif
