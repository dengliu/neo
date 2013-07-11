/*
** File Name:	handle.hpp
** Author:	Aditya Ramesh
** Date:	07/09/2013
** Contact:	_@adityaramesh.com
*/

#ifndef ZE0BB04B6_F20C_4FF0_437F_21A823AF7523
#define ZE0BB04B6_F20C_4FF0_437F_21A823AF7523

#include <memory>
#include <system_error>
#include <ccbase/platform.hpp>
#include <neo/open_mode.hpp>
#include <neo/size.hpp>

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

namespace neo
{

template <open_mode OpenMode, bool Cache>
class handle;

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX

struct deleter
{
	void operator()(char* p)
	{
		std::free(p);
	}
};

template <open_mode OpenMode, bool Cache>
class handle
{
public:
	using offset_type     = off_t;
	using integer         = std::underlying_type<open_mode>::type;
	using file_descriptor = int;

	using buffer_type =
	typename std::conditional<
		Cache,
		std::unique_ptr<char[]>,
		std::unique_ptr<char[], deleter>
	>::type;
private:
	static constexpr integer mode_flags = 
	(OpenMode & read) != 0 && (OpenMode & write) != 0 ?
	O_RDWR | static_cast<integer>(OpenMode & ~(read | write)) :
	static_cast<integer>(OpenMode);

	static constexpr integer extra_flags =
	Cache ? static_cast<integer>(O_NOATIME) :
	static_cast<integer>(O_NOATIME | O_DIRECT);

	static constexpr integer flags = mode_flags | extra_flags;

	int fd;
	offset_type fs;
public:
	explicit handle(const char* path) :
	fd{::open(path, flags)}
	{
		if (fd == -1) {
			throw std::system_error{errno, std::system_category()};
		}
		fs = size(fd);
	}

	handle(const handle&)            = delete;
	handle(handle&&)                 = delete;
	handle& operator=(const handle&) = delete;
	handle& operator=(handle&&)      = delete;

	~handle()
	{
		::close(fd);
	}

	offset_type file_size() const { return fs; }

	template <class T = void>
	auto allocate(
		buffer_type&      buf,
		const std::size_t n,
		const T* = 0
	) -> typename std::enable_if<
		std::is_same<T, T>::value && Cache,
		void
	>::type
	{
		buf.reset(new char[n]);
	}

	template <class T = void>
	auto allocate(
		buffer_type&      buf,
		const std::size_t n,
		const T* = 0
	) -> typename std::enable_if<
		std::is_same<T, T>::value && !Cache,
		void
	>::type
	{
		#ifndef NDEBUG
			auto s = ::sysconf(_SC_PAGE_SIZE);
			assert(s > 0 && "Page size cannot be determined.");
			assert(n % s == 0 && "Buffer size must be a multiple of the page size.");
		#endif

		char* p;
		auto r = ::posix_memalign((void**)&p, 512, n);
		if (r != 0) {
			throw std::system_error{r, std::system_category()};
		}
		buf.reset(p);
	}

	operator file_descriptor() const { return fd; }
};

#elif PLATFORM_KERNEL == PLATFORM_KERNEL_MACH

template <open_mode OpenMode, bool Cache>
class handle
{
public:
	using offset_type     = off_t;
	using integer         = std::underlying_type<open_mode>::type;
	using file_descriptor = int;
	using buffer_type     = std::unique_ptr<char[]>;
private:
	static constexpr integer flags = 
	(OpenMode & read) != 0 && (OpenMode & write) != 0 ?
	O_RDWR | static_cast<integer>(OpenMode & ~(read | write)) :
	static_cast<integer>(OpenMode);

	int fd;
	offset_type fs;
public:
	explicit handle(const char* path) :
	fd{::open(path, flags)}
	{
		if (fd == -1) {
			throw std::system_error{errno, std::system_category()};
		}
		if (!Cache && ::fcntl(fd, F_NOCACHE, 1) == -1) {
			throw std::system_error{errno, std::system_category()};
		}
		fs = size(fd);
	}

	handle(const handle&)            = delete;
	handle(handle&&)                 = delete;
	handle& operator=(const handle&) = delete;
	handle& operator=(handle&&)      = delete;

	~handle()
	{
		::close(fd);
	}

	offset_type file_size() const { return fs; }

	void allocate(buffer_type& buf, const std::size_t n)
	{
		buf.reset(new char[n]);
	}

	operator file_descriptor() const { return fd; }
};

#endif

}

#endif
