/*
** File Name:	read_benchmark.cpp
** Author:	Aditya Ramesh
** Date:	07/07/2013
** Contact:	_@adityaramesh.com
**
** http://www.fsl.cs.sunysb.edu/~vass/linux-aio.txt
*/

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <memory>
#include <system_error>
#include <ccbase/format.hpp>
#include <ccbase/platform.hpp>

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX
	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#elif PLATFORM_KERNEL == PLATFORM_KERNEL_MACH
	#include <fcntl.h>
	#include <unistd.h>
#else
	#error "Unsupported platform kernel."
#endif

#if PLATFORM_KERNEL == PLATFORM_KERNEL_LINUX

static void
test(const char* path, const int n)
{
	// Initialize the file descriptor.
	int fd = ::open(path, O_RDONLY | O_DIRECT | O_NOATIME);
	if (fd == -1) {
		throw std::system_error{errno, std::system_category()};
	}

	// Initialize the buffer. On Linux, the buffer must be aligned to 512
	// bytes (since version 2.6), and the length must be a multiple of the
	// page size. Before version 2.6, there was no available interface for
	// querying the alignment restrictions on the buffer. Usually, the
	// buffer had to be aligned to the file system's block size.
	char* p;
	int r = ::posix_memalign((void**)&p, 512, n);
	if (r != 0) {
		throw std::system_error{errno, std::system_category()};
	}

	auto del = [](char* p) { std::free(p); };
	std::unique_ptr<char[], decltype(del)> buf{p, del};
	
	int c;
	do c = ::read(fd, buf.get(), n); while (c == n);

	if (c == -1) {
		throw std::system_error{errno, std::system_category()};
	}
	if (::close(fd) == -1) {
		throw std::system_error{errno, std::system_category()};
	}
}

#elif PLATFORM_KERNEL == PLATFORM_KERNEL_MACH

static void
test(const char* path, const int n)
{
	int fd = ::open(path, O_RDONLY);
	if (fd == -1) {
		throw std::system_error{errno, std::system_category()};
	}

	// On OS X, the caching must be disabled through `fcntl`. Without a
	// carefully-chosen buffer size, it is better to leave caching on, as
	// the OS chooses a good buffer size. This can be hard to do, especially
	// for SSDs.
	if (::fcntl(fd, F_NOCACHE, 1) == -1) {
		throw std::system_error{errno, std::system_category()};
	}

	std::unique_ptr<char[]> buf{new char[n]};
	int c;
	do c = ::read(fd, buf.get(), n); while (c == n);

	if (c == -1) {
		throw std::system_error{errno, std::system_category()};
	}
	if (::close(fd) == -1) {
		throw std::system_error{errno, std::system_category()};
	}
}

#endif

int main(int argc, char** argv)
{
	if (argc != 3) {
		std::cerr << "Error: wrong number of parameters." << std::endl;
		return EXIT_FAILURE;
	}

	int n{std::atoi(argv[2])};
	if (n <= 0) {
		std::cerr << "Error: invalid buffer size." << std::endl;
		return EXIT_FAILURE;
	}

	using namespace std::chrono;
	auto t1 = high_resolution_clock::now();
	test(argv[1], n);
	auto t2 = high_resolution_clock::now();
	auto c = duration_cast<duration<double>>(t2 - t1).count();
	std::cout << c << std::flush;
	return EXIT_SUCCESS;
}
