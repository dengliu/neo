/*
** File Name:	streambuf.hpp
** Author:	Aditya Ramesh
** Date:	07/09/2013
** Contact:	_@adityaramesh.com
*/

#ifndef ZBA4ECAB3_7213_4303_99BA_B94E54C1682A
#define ZBA4ECAB3_7213_4303_99BA_B94E54C1682A

#include <memory>
#include <ccbase/platform.hpp>
#include <neo/open_mode.hpp>
#include <neo/system.hpp>

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX
	#include <cstdint>
	#include <cstring>
	#include <fcntl.h>
	#include <time.h>
	#include <unistd.h>
	#include <neo/aio_syscall>
#elif PLATFORM_KERNEL == PLATFORM_KERNEL_MACH
	#include <aio.h>
	#include <fcntl.h>
	#include <time.h>
#else
	#error "Unsupported kernel."
#endif

namespace neo
{

template <
	open_mode OpenMode,
	bool      UseAsyncIO,
	bool      UseDirectIO
>
class streambuf;

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX

// TODO
// If UseAsyncIO is true but the file is smaller than the chosen buffer size,
// then revert to blocking IO.
//
// UseAsyncIO = false => Use blocking IO
//
// Add support for seek directions, so that f and l are only stored when
// necessary.

template <open_mode OpenMode, bool UseDirectIO>
class streambuf<OpenMode, true, UseDirectIO> :
	public streambuf_base<OpenMode, UseDirectIO>
{
public:
	using base = streambuf_base<OpenMode, UseDirectIO>;
	using size_type = off_t;
	using pointer = char*;
protected:
	using buffer_type = typename base::buffer_type;
private:
	timespec t{0,0};
	aio_context_t c{0};
	buffer_type buf1{nullptr};
	buffer_type buf2{nullptr};
	bool db{true};
	unsigned n;
	io_event e[1];
	pointer p;
	pointer f;
	pointer l;
public:
	explicit streambuf(
		const char*          path,
		const size_type      off    = 0,
		const unsigned       n      = default_read_buffer_size,
		const traversal_mode m      = sequential
	) : base{path}, n{n}
	{
		if (base::file_size() <= n || (m & random != 0)) {
			db = false;
			n = base::file_size();
			base::allocate(buf1, n);
		}
		else {
			base::allocate(buf1, n);
			base::allocate(buf2, n);
		}
		p = buf1.get();
		f = buf1.get();
		l = buf1.get() + n;
	}

	streambuf(const streambuf&)            = delete;
	streambuf(streambuf&&)                 = delete;
	streambuf& operator=(const streambuf&) = delete;
	streambuf& operator=(streambuf&&)      = delete;

	~streambuf()
	{
		io_destroy(c);
	}

	pointer get() const
	{
		return p;
	}

	void unchecked_increment() { ++p; }
	void unchecked_decrement() { --p; }

	template <class Integer>
	auto unchecked_increment(const Integer i) ->
	std::enable_if<std::is_integer<Integer>::value>::type
	{
		p += n;
	}

	template <class Integer>
	auto unchecked_decrement(const Integer i) ->
	std::enable_if<std::is_integer<Integer>::value>::type
	{
		p -= n;
	}

	void increment()
	{
		if (++p == e) {
			if (db) {
				if (f == buf1.get()) {
					// flush
					p = buf2.get();
					f = buf2.get();
					l = buf2.get() + n;
				}
				else {
					p = buf1.get();
					f = buf1.get();
					l = buf1.get() + n;
				}
			}
			else {
				// flush
				p = buf1.get();
			}
		}
	}

	void decrement()
	{
		if (p == f) {
			if (db) {
				if (f == buf1.get()) {

				}
			}
			else {

			}
		}
		--p;
	}

	void seek(const size_type off)
	{
		
	}

	void advise(const traversal_mode m)
	{
		if (m & sequential != 0) {
			if (!db) {

			}
		}
		else if (m & random != 0) {
			if (db) {

			}
		}
	}
private:
	//template <class T>
	//void read(const size_type off, const size_type n, T* buf, const int fd)
	//{
	//	iocb b;
	//	std::memset(&b, 0, sizeof(b));
	//	b.aio_fildes     = fd;
	//	b.aio_lio_opcode = IOCB_CMD_PREAD;

	//	b.aio_buf    = (uint64_t)buf;
	//	b.aio_offest = off;
	//	b.aio_nbytes = count;

	//	iocb* l[] = {&b};
	//	auto r = ::io_submit(c, 1, l);

	//	if (r != 1) {
	//		if (r == -1) {
	//			throw std::system_error{errno, std::system_category()};
	//		}
	//		else {
	//			throw std::runtime_error{"Error submitting I/O request."};
	//		}
	//	}
	//}

	//template <class T>
	//void read(
	//	const size_type offset, const size_type n1, const size_type n2,
	//	T* buf1, T* buf2, const int fd
	//)
	//{
	//	iocb b[2];
	//	std::memset(b, 0, 2 * sizeof(b));

	//	b[0].aio_fildes     = fd;
	//	b[0].aio_lio_opcode = IOCB_CMD_PREAD;

	//	b[0].aio_buf    = (uint64_t)buf1;
	//	b[0].aio_offset = off;
	//	b[0].aio_nbytes = n1;

	//	b[1].aio_fildes     = fd;
	//	b[1].aio_lio_opcode = IOCB_CMD_PREAD;

	//	b[1].aio_buf    = (uint64_t)buf2;
	//	b[1].aio_offset = off + n1;
	//	b[1].aio_nbytes = n2;

	//	iocb* l[] = { &b[0], &b[1] };
	//	auto r = ::io_submit(c, 2, l);

	//	if (r != 2) {
	//		if (r == -1) {
	//			throw std::system_error{errno, std::system_category()};
	//		}
	//		else {
	//			throw std::runtime_error{"Error submitting I/O request."};
	//		}
	//	}
	//}

	//std::size_t get()
	//{
	//	auto r = ::io_getevents(c, 1, 1, e, &t);
	//	if (r == 0) {
	//		return false;
	//	}
	//	else if (r == n1) {

	//	}
	//	else if (r > 0) {

	//	}
	//	else {
	//		throw std::system_error{errno, std::system_category()};
	//	}
	//}
}

#elif PLATFORM_KERNEL == PLATFORM_KERNEL_MACH

#endif

}

#endif
