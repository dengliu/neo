/*
** File Name:	ifstream.hpp
** Author:	Aditya Ramesh
** Date:	07/06/2013
** Contact:	_@adityaramesh.com
*/

#ifndef Z19E04DA4_DB0E_434F_B839_1874A24D768F
#define Z19E04DA4_DB0E_434F_B839_1874A24D768F

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <memory>
#include <system_error>

namespace cc
{

enum class read_mode : int
{
	nocache = O_DIRECT,
	update  = O_NOATIME
};

static constexpr read_mode nocache = read_mode::nocache;
static constexpr read_mode update  = read_mode::update;

template <class Allocator>
class basic_ifstream
{
private:
	Allocator alloc;
	long      n;
	int       fd;
	char*     buf;
public:
	explicit ifstream(const char* path, const read_mode m)
	throw(std::bad_alloc) :
	alloc{}, n{::sysconf(_SC_PAGESIZE)},
	fd{::open(path, static_cast<int>(m))}
	{

		if (fd == -1) {
			throw std::system_error{::errno, std::system_category{}};
		}
		if (n == -1) {
			n = 4096;
		}
		buf = alloc.allocate(n);
		::new (buf) char[n];
	}

	~ifstream()
	{
		alloc.deallocate(buf, n);
	}
};

}

#endif
