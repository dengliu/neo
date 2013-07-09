/*
** File Name:	test_2.cpp
** Author:	Aditya Ramesh
** Date:	07/06/2013
** Contact:	_@adityaramesh.com
*/

#include <cstddef>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

static void test()
{
	static constexpr auto eof = std::char_traits<char>::eof();
	std::ifstream is{"dat/test", std::ios::binary};
	if (!is) {
		std::cerr << "Failed to open file." << std::endl;
		return;
	}
	auto rb = is.rdbuf();
	while (rb->sbumpc() != eof) {}
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
