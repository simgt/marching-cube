#ifndef MATH_HH
#define MATH_HH

#include <glfw.h>
#include <iostream>

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

/* =========
 *	 vec3f
 * ========= */

struct vec3f{
	float x, y, z;

	vec3f();
	vec3f(float v);
	vec3f(float x, float y, float z);
	float& operator[] (int i);
	const float& operator[] (int i) const;
	
	float length () const; // should return a float ?
	void length (float);
	void normalize ();

	vec3f operator- () const;
	vec3f operator+ (const vec3f&) const;
	vec3f operator- (const vec3f&) const;
	vec3f operator* (float) const;

	vec3f& operator= (float);
	vec3f& operator+= (const vec3f&);
	vec3f& operator-= (const vec3f&);
};

vec3f cross(const vec3f& u, const vec3f& v);

std::ostream& operator<< (std::ostream& out, const vec3f& v);

/* =========
 *	 mat4f
 * =========
 *
 * 4x4 float matrix
 * stored in column major (vecs = columns)
 */

struct mat4f {
	union {
		float m[16];
		float n[4][4]; // m[column][row]
	};

	float& operator[] (int i);
	const float& operator[] (int i) const;
	float& operator() (int i, int j);
	const float& operator() (int i, int j) const;
	float* raw ();
	
	static mat4f identity ();
	static mat4f rotation (float x, float y, float z, float t);
	static mat4f rotation (const vec3f& x_axis, const vec3f& y_axis, const vec3f& z_axis);
	static mat4f scale (float x, float y, float z);
	static mat4f translation (float x, float y, float z);
	static mat4f translation (const vec3f& v);
	static mat4f inverse (const mat4f& a);
	
	float determinant() const;
	
	mat4f operator* (const mat4f&) const;
	vec3f operator* (const vec3f&) const;
};

std::ostream& operator<< (std::ostream& out, const mat4f& m);

/* ===============
 *	 conversions
 * =============== */

float radian (float t);
vec3f cartesian (float theta, float phi, float r);

#include "math.hxx"

#endif