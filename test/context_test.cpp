/*
** File Name:	context_test.cpp
** Author:	Aditya Ramesh
** Date:	07/10/2013
** Contact:	_@adityaramesh.com
*/

#include <neo/context.hpp>
#include <neo/operation.hpp>
#include <neo/handle.hpp>

int main()
{
	neo::context<true> c;
	neo::handle<neo::read, false> h{"test.dat"};
	neo::operation<neo::input, true> op{h};
	c.submit(op);
}
