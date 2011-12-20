#ifndef MATH_HH
#define MATH_HH

#include <glfw.h>
#include <iostream>

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

/*
 * 3 vec
 */

template <typename T>
struct vec3 {
	T x, y, z;

	vec3 ();
	vec3 (T v);
	vec3 (T x, T y, T z);
	T& operator[] (int i);
	const T& operator[] (int i) const;
	
	float length () const; // should return a T ?
	void length (float);
	void normalize ();

	vec3<T> operator- () const;
	vec3<T> operator+ (const vec3<T>&) const;
	vec3<T> operator- (const vec3<T>&) const;
	vec3<T> operator* (T) const;

	vec3<T>& operator= (T);
	vec3<T>& operator+= (const vec3<T>&);
	vec3<T>& operator-= (const vec3<T>&);
};

template <typename T>
vec3<T> cross(const vec3<T>& u, const vec3<T>& v);

template <typename T>
std::ostream& operator<< (std::ostream& out, const vec3<T>& v);

typedef vec3<float> vec3f;
typedef vec3<int> vec3i;
typedef vec3<bool> vec3b;

// utils

float radian (float t);
vec3<float> cartesian (float theta, float phi, float r);

#include "math.hxx"

#endif