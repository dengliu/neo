/*
** File Name:	test_4.cpp
** Author:	Aditya Ramesh
** Date:	07/07/2013
** Contact:	_@adityaramesh.com
**
** Linux version of read test.
*/

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static void test()
{
	int fd = ::open("dat/test", O_RDONLY);
	if (fd == -1) {
		throw std::system_error{errno, std::system_category()};
	}

	//stat buf;
	//::stat("/", &buf);
	//// block size: buf.st_blksize;
	
	char* p;
	int r = std::posix_memalign(&p, 512, n);
	if (r != 0) {
		throw std::system_error{r, std::system_category{}};
	}

	auto del = [](char* p) { std::free(p); };
	std::unique_ptr<char[], decltype(del)> buf{p, del};
	
	//int c;
	//do c = ::read(fd, buf.get(), n); while (c != n);
	//if (c == -1) {
	//	throw std::system_error{errno, std::system_category()};
	//}
	if (::close(fd) == -1) {
		throw std::system_error{errno, std::system_category()};
	}
}

int main()
{
	test();
	return EXIT_SUCCESS;
}
