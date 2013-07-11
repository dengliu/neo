/*
** File Name:	context.hpp
** Author:	Aditya Ramesh
** Date:	07/09/2013
** Contact:	_@adityaramesh.com
*/

#ifndef ZE0BB04B6_F20C_4D20_BE7F_21A823AF7522
#define ZE0BB04B6_F20C_4D20_BE7F_21A823AF7522

#include <chrono>
#include <cstdint>
#include <cstring>
#include <ratio>
#include <system_error>
#include <ccbase/platform.hpp>
#include <neo/io_mode.hpp>
#include <neo/open_mode.hpp>

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX
	#include <fcntl.h>
	#include <time.h>
	#include <unistd.h>
	#include <neo/aio_syscall.hpp>
#elif PLATFORM_KERNEL == PLATFORM_KERNEL_MACH
	#include <aio.h>
	#include <fcntl.h>
	#include <time.h>
	#include <unistd.h>
#endif

namespace neo
{

using std::ratio;
using std::chrono::seconds;
using std::chrono::nanoseconds;
using std::chrono::duration;
using std::chrono::duration_cast;

template <io_mode Type, bool Asynchronous>
class operation;

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX || \
    PLATFORM_KERNEL == PLATFORM_KERNEL_MACH

template <class Rep, class Period>
static inline timespec 
to_timespec(const duration<Rep, Period> d)
{
	return {
		duration_cast<seconds>(d).count(),
		duration_cast<nanoseconds>(d).count() -
		1000000000 * duration_cast<seconds>(d).count()
	};
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
	void submit(operation<Ts, false>&... ops) const
	{
		submit_impl(ops...);
	}

	template <io_mode IOMode, class Rep = uint64_t, class Period = std::nano>
	constexpr bool poll(operation<IOMode, false>&, const duration<Rep, Period>) const
	{
		return true;
	}
private:
	template <io_mode IOMode, class... Ts>
	void submit_impl(operation<IOMode, false>& op, Ts&... ts) const
	{
		if ((IOMode & input) != 0) {
			::pread(op.handle(), op.buffer(), op.count(), op.offset());
		}
		else {
			::pwrite(op.handle(), op.buffer(), op.count(), op.offset());
		}
		submit_impl(ts...);
	}

	template <io_mode IOMode>
	void submit_impl(operation<IOMode, false>& op) const
	{
		if ((IOMode & input) != 0) {
			::pread(op.handle(), op.buffer(), op.count(), op.offset());
		}
		else {
			::pwrite(op.handle(), op.buffer(), op.count(), op.offset());
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
		auto r = io_setup(n, &c);
		if (r == -1) {
			throw std::system_error{errno, std::system_category()};
		}
	}

	context(const context&)            = delete;
	context(context&&)                 = delete;
	context& operator=(const context&) = delete;
	context& operator=(context&&)      = delete;

	~context()
	{
		io_destroy(c);
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
	void submit(operation<Ts, true>&... ops) const
	{
		iocb* l[] = { ops... };
		auto r = io_submit(c, sizeof...(ops), l);
		if (r == -1) {
			throw std::system_error{errno, std::system_category()};
		}
	}

	template <io_mode IOMode, class Rep = uint64_t, class Period = std::nano>
	bool poll(operation<IOMode, true>&, const duration<Rep, Period> d = 0) const
	{
		io_event e[1];
		auto t = to_timespec(d);
		auto r = io_getevents(c, 1, 1, e, &t);
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

#elif PLATFORM_KERNEL == PLATFORM_KERNEL_MACH

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
	void submit(operation<Ts, Us>&... ops) const
	{
		submit_impl(ops...);
	}

	template <io_mode IOMode, class Rep = uint64_t, class Period = std::nano>
	bool poll(operation<IOMode, true>& op, const duration<Rep, Period> d) const
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
	void submit_impl(operation<IOMode, true>& op, Ts&... ts) const
	{
		if ((IOMode & input) != 0) {
			::aio_read(op);
		}
		else {
			::aio_write(op);
		}
		submit_impl(ts...);
	}

	template <io_mode IOMode>
	void submit_impl(operation<IOMode, true>& op) const
	{
		if ((IOMode & input) != 0) {
			::aio_read(op);
		}
		else {
			::aio_write(op);
		}
	}
};

#endif

}

#endif
