/*
** File Name:	ofstream.hpp
** Author:	Aditya Ramesh
** Date:	07/06/2013
** Contact:	_@adityaramesh.com
*/

#ifndef ZD34D976A_39B5_464F_90DC_F7AA6C925D32
#define ZD34D976A_39B5_464F_90DC_F7AA6C925D32

enum class write_mode : int
{
	append   = O_APPEND,
	create   = O_CREAT,
	nocache  = O_DIRECT,
	update   = O_NOATIME,
	truncate = O_TRUNC
};

// permissions enum with user-defined literals

#endif
