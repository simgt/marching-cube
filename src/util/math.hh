#ifndef MATH_HH
#define MATH_HH

#include <glfw.h>
#include <iostream>

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

/*
 * 3 vector represented by a 4 vector (for 128 bits alignment)
 */

struct vec {
	float x, y, z, w;

	vec ();
	vec (float x, float y, float z);
	float& operator[] (int i);
	const float& operator[] (int i) const;
};

/*
 * 4x4 float matrix
 * stored in column major (vecs = columns)
 */
struct mat {
	float m[16];

	float& operator[] (int i);
	const float& operator[] (int i) const;
	float& operator() (int i, int j);
	const float& operator() (int i, int j) const;
	float* raw ();
	
	static mat identity ();
	static mat rotation (float x, float y, float z, float t);
	static mat rotation (const vec& x_axis, const vec& y_axis, const vec& z_axis);
	static mat scale (float x, float y, float z);
	static mat translation (float x, float y, float z);
	static mat translation (const vec& v);
};

float radian (float t);
vec cartesian (float theta, float phi, float r);

float length (const vec& v);
vec normalize (const vec& v);
vec cross(const vec& u, const vec& v);

vec operator- (const vec&);
vec operator+ (const vec&, const vec&);
vec operator- (const vec&, const vec&);
vec operator* (const vec&, float);
vec operator* (float, const vec&);
mat operator* (const mat&, const mat&);

std::ostream& operator<< (std::ostream& out, const vec& v);
std::ostream& operator<< (std::ostream& out, const mat& m);

#include "math.hxx"

#endif