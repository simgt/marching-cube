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

// MATHS

#include <Eigen/Dense>

typedef Eigen::Vector3f Vec3f;
typedef Eigen::Vector3i Vec3i;
typedef Eigen::Matrix<short, 3, 1> Vec3s;

inline Vec3i floor (const Vec3f& v) {
	return Vec3i(floor(v[0]), floor(v[1]), floor(v[2]));
}

inline Vec3i floor (const Vec3f& v, const int inc) {
	return floor(v / (float)inc) * inc;
}

#endif