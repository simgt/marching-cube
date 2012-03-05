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

// LIBRARIES

#include <glfw.h>
#include <Horde3D/Horde3D.h>
#include <Horde3DUtils/Horde3DUtils.h>

// MATHS

#include <Eigen/Dense>

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

typedef Eigen::Vector3f Vec3f;
typedef Eigen::Vector3i Vec3i;
typedef Eigen::Matrix<short, 3, 1> Vec3s;

inline Vec3i floor (const Vec3f& v) {
	return Vec3i(floor(v[0]), floor(v[1]), floor(v[2]));
}

inline Vec3i floor (const Vec3f& v, const int inc) {
	return floor(v / (float)inc) * inc;
}

inline Vec3i floor (const Vec3i& v, const int inc) {
	return Vec3i(floor(v[0] / (float)inc) * inc,
				 floor(v[1] / (float)inc) * inc,
				 floor(v[2] / (float)inc) * inc);
}

inline float radian (float t) {
	return  t * 2.0f * M_PI / 360.0f;
}

inline Vec3f cartesian (float theta_deg, float phi_deg, float r) {
	float theta = radian(theta_deg), phi = radian(phi_deg);
	float sin_theta = sin(theta);
	return Vec3f(r * sin_theta * cos(phi), r * cos(theta), r * sin_theta * sin(phi));
}

#endif