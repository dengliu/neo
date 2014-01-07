/*
** File Name:	handle.hpp
** Author:	Aditya Ramesh
** Date:	07/09/2013
** Contact:	_@adityaramesh.com
**
** The handle class is an abstraction over the low-level platform functions for
** opening and closing file handles, and for allocating IO buffers that are good
** for IO operations involving the file.
**
** The reason the IO buffer allocation is coupled to the file handle is that the
** operating system may impose various alignment restrictions on the buffer if
** direct IO is used. See `notes/aio_notes.md` for details regarding the
** alignment constraints.
*/

#ifndef ZE0BB04B6_F20C_4FF0_437F_21A823AF7523
#define ZE0BB04B6_F20C_4FF0_437F_21A823AF7523

#include <cstdint>
#include <memory>
#include <system_error>
#include <ccbase/platform.hpp>
#include <neo/file_size.hpp>
#include <neo/open_mode.hpp>

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX || \
    PLATFORM_KERNEL == PLATFORM_KERNEL_MACH
	#include <cassert>
	#include <cstdlib>
	#include <fcntl.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#else
	#error "Unsupported kernel."
#endif

namespace neo {

template <open_mode OpenMode, bool UseDirectIO>
class handle;

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX

namespace detail {

struct deleter
{
	void operator()(uint8_t* p)
	{
		std::free(p);
	}
};

}

template <open_mode OpenMode, bool UseDirectIO>
class handle
{
public:
	using size_type       = size_t;
	using offset_type     = off_t;
	using integer         = std::underlying_type<open_mode>::type;
	using file_descriptor = int;

	using buffer_type =
	typename std::conditional<
		!UseDirectIO,
		std::unique_ptr<uint8_t[]>,
		std::unique_ptr<uint8_t[], detail::deleter>
	>::type;
private:
	static constexpr auto extra_flags =
	!UseDirectIO ? static_cast<integer>(O_NOATIME) :
	static_cast<integer>(O_NOATIME | O_DIRECT);

	static constexpr auto flags =
	posix(OpenMode) | extra_flags;

	int fd{-1};
	offset_type fs;
public:
	explicit handle(const char* path) :
	fd{::open(path, flags)}
	{
		if (fd == -1) {
			throw std::system_error{errno, std::system_category()};
		}
		fs = detail::size(fd);
	}

	handle(const handle&)            = delete;
	handle& operator=(const handle&) = delete;

	handle(handle&& rhs) noexcept :
	fd{rhs.fd}, rs{rhs.fs}
	{
		rhs.fd = -1;
	}

	handle& operator=(handle&& rhs) noexcept
	{
		fd = rhs.fd;
		fs = rhs.fs;
		rhs.fd = -1;
		return *this;
	}

	~handle()
	{
		::close(fd);
	}

	offset_type file_size() const { return fs; }

	template <class T = void>
	auto allocate(
		buffer_type&    buf,
		const size_type n,
		const T* = 0
	) -> typename std::enable_if<
		std::is_same<T, T>::value && !UseDirectIO,
		void
	>::type
	{
		buf.reset(new uint8_t[n]);
	}

	template <class T = void>
	auto allocate(
		buffer_type&    buf,
		const size_type n,
		const T* = 0
	) -> typename std::enable_if<
		std::is_same<T, T>::value && UseDirectIO,
		void
	>::type
	{
		#ifndef NDEBUG
			auto s = ::sysconf(_SC_PAGE_SIZE);
			assert(s > 0 && "Page size cannot be determined.");
			assert(n % s == 0 && "Buffer size must be a multiple of the page size.");
		#endif

		uint8_t* p;
		auto r = ::posix_memalign((void**)&p, 512, n);
		if (r != 0) {
			throw std::system_error{r, std::system_category()};
		}
		buf.reset(p);
	}

	operator file_descriptor() const { return fd; }
};

#elif PLATFORM_KERNEL == PLATFORM_KERNEL_MACH

template <open_mode OpenMode, bool UseDirectIO>
class handle
{
public:
	using size_type       = size_t;
	using offset_type     = off_t;
	using integer         = std::underlying_type<open_mode>::type;
	using file_descriptor = int;
	using buffer_type     = std::unique_ptr<uint8_t[]>;
private:
	static constexpr auto flags = posix(OpenMode)

	int fd;
	offset_type fs;
public:
	explicit handle(const char* path) :
	fd{::open(path, flags)}
	{
		if (fd == -1) {
			throw std::system_error{errno, std::system_category()};
		}
		if (UseDirectIO && ::fcntl(fd, F_NOCACHE, 1) == -1) {
			throw std::system_error{errno, std::system_category()};
		}
		fs = detail::size(fd);
	}

	handle(const handle&)            = delete;
	handle& operator=(const handle&) = delete;

	handle(handle&& rhs) noexcept :
	fd{rhs.fd}, rs{rhs.fs}
	{
		rhs.fd = -1;
	}

	handle& operator=(handle&& rhs) noexcept
	{
		fd = rhs.fd;
		fs = rhs.fs;
		rhs.fd = -1;
		return *this;
	}

	~handle()
	{
		::close(fd);
	}

	offset_type file_size() const { return fs; }

	void allocate(buffer_type& buf, const size_type n)
	{
		buf.reset(new uint8_t[n]);
	}

	operator file_descriptor() const { return fd; }
};

#endif

}

#endif
