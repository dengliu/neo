/*
** File Name:	test_1.cpp
** Author:	Aditya Ramesh
** Date:	07/06/2013
** Contact:	_@adityaramesh.com
** 
** OS X version of read test.
*/

#include <cassert>
#include <cstddef>
#include <chrono>
#include <iostream>
#include <memory>
#include <system_error>
#include <fcntl.h>
#include <unistd.h>

// test fadvise, mmap

static void
test(const int n)
{
	int fd = ::open("dat/test.dat", O_RDONLY);
	if (fd == -1) {
		throw std::system_error{errno, std::system_category()};
	}
	
	// A value of 0 in `arg` turns data caching on, while a nonzero value
	// turns data caching off. Using `true` improves legibility a little.
	//if (::fcntl(fd, F_NOCACHE, 1) == -1) {
	//	throw std::system_error{errno, std::system_category()};
	//}

	std::unique_ptr<char[]> buf{new char[n]};
	int c;
	//int t{0};
	do {
		c = ::read(fd, buf.get(), n);
		//for (unsigned i{0}; i != c; ++i) {
		//	t += buf[i] == 'a';
		//}
	}
	while (c == n);
	//assert(t == 11830069);

	if (c == -1) {
		throw std::system_error{errno, std::system_category()};
	}
	if (::close(fd) == -1) {
		throw std::system_error{errno, std::system_category()};
	}
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "Error: wrong number of parameters." << std::endl;
		return EXIT_FAILURE;
	}

	int n{std::atoi(argv[1])};
	if (n <= 0) {
		std::cerr << "Error: invalid buffer size." << std::endl;
		return EXIT_FAILURE;
	}

	using namespace std::chrono;
	auto t1 = high_resolution_clock::now();
	test(std::atoi(argv[1]));
	auto t2 = high_resolution_clock::now();
	auto c = duration_cast<duration<double>>(t2 - t1).count();
	std::cout << c << std::flush;
	return EXIT_SUCCESS;
}
