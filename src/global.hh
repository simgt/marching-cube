#ifndef GLOBAL_HH
#define GLOBAL_HH

// DEBUG
#include <cassert>

#ifdef NDEBUG
#define outlog(x)
#else
#include <iostream>
#define outlog(x) \
	std::cout << #x << ": " << x << std::endl;
#endif


// GENERAL PURPOSE

typedef unsigned int uint;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned short ushort;
typedef unsigned char uchar;

#endif