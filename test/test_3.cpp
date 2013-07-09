/*
** File Name:	test_3.cpp
** Author:	Aditya Ramesh
** Date:	07/06/2013
** Contact:	_@adityaramesh.com
*/

#include <cstddef>
#include <cstdio>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>

static void test()
{
	static constexpr int n = 8192;
	std::unique_ptr<char> buf{new char[n]};
	FILE* fd = std::fopen("dat/test", "rb");
	if (fd == nullptr) {
		std::cerr << "Error opening file." << std::endl;
	}
	std::setbuf(fd, nullptr);
	while (std::fread(buf.get(), 1, n, fd) == n) {}
	if (std::ferror(fd) != 0) {
		std::cerr << "Error reading file." << std::endl;
	}
	std::fclose(fd);
}

int main()
{
	using namespace std::chrono;
	auto t1 = high_resolution_clock::now();
	test();
	auto t2 = high_resolution_clock::now();
	auto c = duration_cast<duration<double>>(t2 - t1).count();
	std::cout << "Time: " << c << " seconds." << std::endl;
	return EXIT_SUCCESS;
}
