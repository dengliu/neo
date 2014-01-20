/*
** File Name: context.hpp
** Author:    Aditya Ramesh
** Date:      07/09/2013
** Contact:   _@adityaramesh.com
*/

#ifndef ZE0BB04B6_F20C_4D20_BE7F_21A823AF7522
#define ZE0BB04B6_F20C_4D20_BE7F_21A823AF7522

#include <chrono>
#include <cstdint>
#include <cstring>
#include <system_error>
#include <ccbase/platform.hpp>
#include <neo/io_mode.hpp>
#include <neo/open_mode.hpp>

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX
	// For `timespec`.
	#include <time.h>
	// For `ssize_t`.
	#include <sys/types.h>
	// For wrappers over Linux `aio` syscalls.
	#include <neo/aio_syscall.hpp>
#elif PLATFORM_KERNEL == PLATFORM_KERNEL_XNU
	// For `aio_read`, `aio_write`, `aio_suspend`, and `aio_return`.
	#include <aio.h>
	// For `timespec`.
	#include <time.h>
	// For `ssize_t`.
	#include <sys/types.h>
#endif

namespace neo {

using std::chrono::seconds;
using std::chrono::nanoseconds;
using std::chrono::duration;
using std::chrono::duration_cast;

template <io_mode Type, bool Asynchronous>
class request;

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX || \
    PLATFORM_KERNEL == PLATFORM_KERNEL_XNU

namespace detail {

template <class Rep, class Period>
static CC_ALWAYS_INLINE timespec 
to_timespec(const duration<Rep, Period> d)
{
	return {
		duration_cast<seconds>(d).count(),
		duration_cast<nanoseconds>(d).count() -
		1000000000 * duration_cast<seconds>(d).count()
	};
}

static CC_ALWAYS_INLINE ssize_t
safe_pread(int fd, void* buf, size_t count, off_t offset)
{
	auto rem = count;
	do {
		auto r = ::pread(fd, (uint8_t*)buf + count - rem, rem,
			offset + count - rem);
		if (r <= 0) {
			return r;
		}
		else {
			rem -= r;
		}
	}
	while (rem > 0);
	return count;
}

static CC_ALWAYS_INLINE ssize_t
safe_pwrite(int fd, void* buf, size_t count, off_t offset)
{
	auto rem = count;
	do {
		auto r = ::pwrite(fd, (uint8_t*)buf + count - rem, rem,
			offset + count - rem);
		if (r <= 0) {
			return r;
		}
		else {
			rem -= r;
		}
	}
	while (rem > 0);
	return count;
}

}

template <bool Asynchronous>
class context;

template <>
class context<false>
{
public:
	explicit context() {}
	context(const context&)            = delete;
	context(context&&)                 = delete;
	context& operator=(const context&) = delete;
	context& operator=(context&&)      = delete;

	template <io_mode... Ts>
	void submit(request<Ts, false>&... ops) const
	{
		submit_impl(ops...);
	}

	template <io_mode IOMode, class Rep = uint64_t, class Period = std::nano>
	constexpr bool poll(request<IOMode, false>&, const duration<Rep, Period>) const
	{
		return true;
	}
private:
	template <io_mode IOMode, class... Ts>
	void submit_impl(request<IOMode, false>& op, Ts&... ts) const
	{
		using detail::safe_pread;
		using detail::safe_pwrite;

		if ((IOMode & input) != 0) {
			if (safe_pread(op.handle(), op.buffer(), op.count(), op.offset()) < 0) {
				throw std::system_error{errno, std::system_category()};
			}
		}
		else {
			if (safe_pwrite(op.handle(), op.buffer(), op.count(), op.offset()) < 0) {
				throw std::system_error{errno, std::system_category()};
			}
		}
		submit_impl(ts...);
	}

	template <io_mode IOMode>
	void submit_impl(request<IOMode, false>& op) const
	{
		using detail::safe_pread;
		using detail::safe_pwrite;

		if ((IOMode & input) != 0) {
			if (safe_pread(op.handle(), op.buffer(), op.count(), op.offset()) < 0) {
				throw std::system_error{errno, std::system_category()};
			}
		}
		else {
			if (safe_pwrite(op.handle(), op.buffer(), op.count(), op.offset()) < 0) {
				throw std::system_error{errno, std::system_category()};
			}
		}
	}
};

#endif

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX

template <>
class context<true>
{
private:
	mutable aio_context_t c{0};
public:
	explicit context(const unsigned n = 4)
	{
		if (detail::io_setup(n, &c) == -1) {
			throw std::system_error{errno, std::system_category()};
		}
	}

	context(const context&)            = delete;
	context(context&&)                 = delete;
	context& operator=(const context&) = delete;
	context& operator=(context&&)      = delete;

	~context()
	{
		detail::io_destroy(c);
	}

	/*
	** The `aiocb` buffer needs to be addressable until the event with which
	** it is associated has terminated. Hence, this method only accepts
	** references to constant lvalues. It is more efficient to enqueue
	** several requests at once than to enqueue each request separately.
	** Therefore, this method uses variadic templates, to give the
	** programmer the option to submit multiple requests at once.
	*/
	
	template <io_mode... Ts>
	void submit(request<Ts, true>&... ops) const
	{
		iocb* l[] = {ops...};
		auto r = detail::io_submit(c, sizeof...(ops), l);
		if (r == -1) {
			throw std::system_error{errno, std::system_category()};
		}
	}

	template <io_mode IOMode, class Rep = uint64_t, class Period = std::nano>
	bool poll(request<IOMode, true>&, const duration<Rep, Period> d = 0) const
	{
		io_event e[1];
		auto t = detail::to_timespec(d);
		auto r = detail::io_getevents(c, 1, 1, e, &t);
		if (r == 1) {
			return true;
		}
		else if (r == 0) {
			return false;
		}
		else {
			throw std::system_error{errno, std::system_category()};
		}
	}
};

#elif PLATFORM_KERNEL == PLATFORM_KERNEL_XNU

template <>
class context<true>
{
public:
	explicit context() noexcept {}
	context(const context&)            = delete;
	context(context&&)                 = delete;
	context& operator=(const context&) = delete;
	context& operator=(context&&)      = delete;

	template <io_mode... Ts, bool... Us>
	void submit(request<Ts, Us>&... ops) const
	{
		submit_impl(ops...);
	}

	template <io_mode IOMode, class Rep = uint64_t, class Period = std::nano>
	bool poll(request<IOMode, true>& op, const duration<Rep, Period> d) const
	{
		const aiocb* l[] = {op};
		auto t = to_timespec(d);
		auto r = ::aio_suspend(l, 1, &t);
		if (r == 0) {
			if (::aio_return(op) == -1) {
				throw std::system_error{errno, std::system_category()};
			}
			return true;
		}
		else if (errno == EAGAIN) {
			return false;
		}
		else {
			throw std::system_error{errno, std::system_category()};
		}
	}
private:
	template <io_mode IOMode, class... Ts>
	void submit_impl(request<IOMode, true>& op, Ts&... ts) const
	{
		if ((IOMode & input) != 0) {
			if (::aio_read(op) == -1) {
				throw std::system_error{errno, std::system_category()};
			}
		}
		else {
			if (::aio_write(op) == -1) {
				throw std::system_error{errno, std::system_category()};
			}
		}
		submit_impl(ts...);
	}

	template <io_mode IOMode>
	void submit_impl(request<IOMode, true>& op) const
	{
		if ((IOMode & input) != 0) {
			if (::aio_read(op) == -1) {
				throw std::system_category{errno, std::system_category()};
			}
		}
		else {
			if (::aio_write(op) == -1) {
				throw std::system_category{errno, std::system_category()};
			}
		}
	}
};

#endif

}

#endif
