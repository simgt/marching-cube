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
typedef unsigned short ushort;
typedef unsigned char uchar;

// HORDE3D

#include <glfw.h>
#include <Horde3D/Horde3D.h>
#include <Horde3DUtils/Horde3DUtils.h>

#endif