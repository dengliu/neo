/*
** File Name:	request.hpp
** Author:	Aditya Ramesh
** Date:	07/10/2013
** Contact:	_@adityaramesh.com
**
** The `neo::request` class provides an abstraction over the
** implementation-specific structures used to submit asynchronous IO requests.
** For consistency, synchronous IO requests are also initialized using a similar
** interface. Operations are submitted to a `neo::context` using the
** `neo::submit` member function.
*/

#ifndef Z8F3AF0EB_18C3_4225_81DF_CE36DD613B89
#define Z8F3AF0EB_18C3_4225_81DF_CE36DD613B89

#include <ccbase/platform.hpp>
#include <neo/io_mode.hpp>
#include <neo/open_mode.hpp>

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX
	#include <cstring>
	#include <neo/aio_syscall.hpp>
#elif PLATFORM_KERNEL == PLATFORM_KERNEL_MACH
	#include <cstring>
	#include <aio.h>
	#include <fcntl.h>
#endif

namespace neo
{

template <open_mode OpenMode, bool UseDirectIO>
class handle;

template <io_mode Type, bool Asynchronous>
class request;

template <io_mode Type>
class request<Type, false>
{
public:
	using size_type       = size_t;
	using offset_type     = off_t;
	using pointer         = char*;
	using file_descriptor = int;
private:
	file_descriptor fd;
	pointer p;
	size_type n;
	offset_type off;
public:
	template <open_mode OpenMode, bool UseDirectIO>
	explicit request(const handle<OpenMode, UseDirectIO>& h)
	noexcept : fd{h} {}

	file_descriptor handle() const { return fd;    }
	pointer buffer()         const { return p;     }
	size_type count()        const { return count; }
	offset_type offset()     const { return off;   }

	void handle(const file_descriptor fd) { this->fd = fd;   }
	void buffer(const pointer p)          { this->p = p;     }
	void count(const size_type n)         { this->n = n;     }
	void offset(const offset_type off)    { this->off = off; }
};

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX

template <io_mode Type>
class request<Type, true>
{
public:
	using size_type       = size_t;
	using offset_type     = off_t;
	using pointer         = char*;
	using file_descriptor = int;
private:
	static constexpr auto op =
	(Type & input) != 0 ? IOCB_CMD_PREAD : IOCB_CMD_PWRITE;

	iocb b;
public:
	template <open_mode OpenMode, bool UseDirectIO>
	explicit request(const handle<OpenMode, UseDirectIO>& h) noexcept
	{
		std::memset(&b, 0, sizeof(b));
		b.aio_lio_opcode = op;
		b.aio_fildes     = h;
	}

	file_descriptor handle() const { return b.aio_fildes; }
	size_type count()        const { return b.aio_nbytes; }
	offset_type offset()     const { return b.aio_offset; }
	pointer buffer()         const { return reinterpret_cast<pointer>(b.aio_buf); }

	void handle(const file_descriptor fd) { b.aio_fildes = fd;  }
	void count(const size_type n)         { b.aio_nbytes = n;   }
	void offset(const offset_type off)    { b.aio_offset = off; }
	void buffer(const pointer p)          { b.aio_buf = reinterpret_cast<uint64_t>(p); }

	operator iocb*() { return &b; }
};

#elif PLATFORM_KERNEL_MACH == PLATFORM_KERNEL_MACH

template <io_mode Type>
class request<Type, true>
{
public:
	using size_type       = size_t;
	using offset_type     = off_t;
	using pointer         = char*;
	using file_descriptor = int;
private:
	aiocb b;
public:
	template <open_mode OpenMode, bool UseDirectIO>
	explicit request(const handle<OpenMode, UseDirectIO>& h) noexcept
	{
		std::memset(&b, 0, sizeof(b));
		b.aio_sigevent.sigev_notify = SIGEV_NONE;
		b.aio_fildes                = h;
	}

	file_descriptor handle() const { return b.aio_fildes; }
	pointer buffer()         const { return b.aio_buf;    }
	size_type count()        const { return b.aio_nbytes; }
	offset_type offset()     const { return b.aio_offset; }

	void handle(const file_descriptor fd) { b.aio_fildes = fd;  }
	void buffer(const pointer p)          { b.aio_buf = p;      }
	void count(const size_type n)         { b.aio_nbytes = n;   }
	void offset(const offset_type off)    { b.aio_offset = off; }

	operator aiocb*() { return &b; }
};

#endif

}

#endif
