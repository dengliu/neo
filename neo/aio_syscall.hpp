/*
** File Name: aio_syscall.hpp
** Author:    Aditya Ramesh
** Date:      07/09/2013
** Contact:   _@adityaramesh.com
*/

#ifndef ZE3A5B4A1_F21D_4B4D_98BB_9B4F3F5F8113
#define ZE3A5B4A1_F21D_4B4D_98BB_9B4F3F5F8113

// For `__NR_*` system call definitions.
#include <sys/syscall.h>
// For variable and structure definitions.
#include <linux/aio_abi.h>
#include <ccbase/platform.hpp>

namespace neo {
namespace detail {

static CC_ALWAYS_INLINE
int io_setup(unsigned n, aio_context_t* c)
{
	return syscall(__NR_io_setup, n, c);
}

static CC_ALWAYS_INLINE
int io_destroy(aio_context_t c)
{
	return syscall(__NR_io_destroy, c);
}

static CC_ALWAYS_INLINE
int io_submit(aio_context_t c, long n, iocb** b)
{
	return syscall(__NR_io_submit, c, n, b);
}

static CC_ALWAYS_INLINE
int io_cancel(aio_context_t c, iocb* b, io_event* e)
{
	return syscall(__NR_io_cancel, c, b, e);
}

static CC_ALWAYS_INLINE
int io_getevents(aio_context_t c, long min, long max, io_event* e, timespec* t)
{
	return syscall(__NR_io_getevents, c, min, max, e, t);
}

}}

#endif
