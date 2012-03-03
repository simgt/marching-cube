#ifndef MATH_HH
#define MATH_HH

#include <glfw.h>
#include <iostream>

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

/* =========
 *	 vec3
 * ========= */

template <typename T>
struct vec3 {
	union {
		T x;
		T r;
		T phi;
	};
	
	union {
		T y;
		T g;
		T theta;
	};
	
	union {
		T z;
		T b;
		T radius;
	};

	vec3 ();
	vec3 (T v);
	vec3 (T x, T y, T z);
	
	T& operator[] (int i);
	const T& operator[] (int i) const;
	
	float length () const;
	void length (float);
	void normalize ();

	/* vec3tor to vec3tor casting */
	template <typename R>
	operator vec3<R> () const {
		return vec3<R>(x, y, z);
	};

	vec3<T> operator- () const;
	vec3<T> operator+ (T) const;
	vec3<T> operator+ (const vec3<T>&) const;
	vec3<T> operator- (T) const;
	vec3<T> operator- (const vec3<T>&) const;
	vec3<T> operator* (T) const;
	vec3<T> operator* (const vec3<T>& b) const;
	vec3<T> operator/ (T) const;
	vec3<T> operator/ (const vec3<T>& b) const;
	bool operator== (const vec3<T>&) const;
	bool operator!= (T) const;
	bool operator!= (const vec3<T>&) const;
	bool operator> (const vec3<T>&) const;
	bool operator>= (const vec3<T>&) const;
	bool operator< (const vec3<T>&) const;
	bool operator<= (const vec3<T>&) const;

	vec3<T>& operator= (T);
	vec3<T>& operator+= (const vec3<T>&);
	vec3<T>& operator-= (const vec3<T>&);
	vec3<T>& operator*= (T);
	vec3<T>& operator*= (const vec3<T>& b);
};

template <typename T>
vec3<T> cross (const vec3<T>& u, const vec3<T>& v);

template <typename T>
T dot (const vec3<T>& u, const vec3<T>& v);

template <typename T>
vec3<T> floor (const vec3<T>& v);

template <typename T>
inline vec3<T> floor (const vec3<T>& v, const T inc) {
	return floor((vec3<float>)v / (float)inc) * inc;
}

template <typename T>
std::ostream& operator<< (std::ostream& out, const vec3<T>& v);

typedef vec3<int> vec3i;
typedef vec3<float> vec3f;
typedef vec3<short> vec3s;
typedef vec3<GLubyte> vec3ub;

template <typename T>
size_t tbb_hasher(const vec3<T>& key) {
	return key.x * 256 + key.y * 128 + key.z;
};

/* =========
 *	 mat4f
 * =========
 *
 * 4x4 float mat4frix
 * stored in column major (vec3s = columns)
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
	
	float determinant () const;
	
	mat4f operator* (const mat4f&) const;
	
	/* matrice * vector
	 * uses the vector as a vec4f terminated by 1.0
	 */
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