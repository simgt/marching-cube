#include "math.hh"
#include <cmath>

/* vec3 */

template <typename T>
inline vec3<T>::vec3 () {
}

template <typename T>
inline vec3<T>::vec3 (T x, T y, T z)
	: x (x),
	  y (y),
	  z (z) {
}

template <typename T>
inline vec3<T>::vec3 (T v)
	: x (v),
	  y (v),
	  z (v) {
}

template <typename T>
inline T& vec3<T>::operator[] (int i) {
	return (&x)[i];
}

template <typename T>
inline const T& vec3<T>::operator[] (int i) const {
	return (&x)[i];
}

/* operations */

template <typename T>
inline float vec3<T>::length () const {
    return sqrtf(x * x + y * y + z * z);
}

template <typename T>
inline void vec3<T>::length (float l) {
	float ratio = l / length();
	x *= ratio;
	y *= ratio;
	z *= ratio;
}

template <typename T>
inline void vec3<T>::normalize () {
	length(1.0);
}

template <typename T>
inline vec3<T> vec3<T>::operator- () const {
	return vec3<T>(-x, -y, -z);
}

template <typename T>
inline vec3<T> vec3<T>::operator+ (const vec3<T>& b) const {
	return vec3<T>(x + b.x, y + b.y, z + b.z);
}

template <typename T>
inline vec3<T> vec3<T>::operator- (const vec3<T>& b) const {
	return vec3<T>(x - b.x, y - b.y, z - b.z);
}

template <typename T>
inline vec3<T> vec3<T>::operator* (T b) const {
	return vec3<T>(x * b, y * b, z * b);
}

template <typename T>
inline vec3<T>& vec3<T>::operator= (T b) {
	x = b;
	y = b;
	z = b;
	return *this;
}

template <typename T>
inline vec3<T>& vec3<T>::operator+= (const vec3<T>& b) {
	x += b.x;
	y += b.y;
	z += b.z;
	return *this;
}

template <typename T>
inline vec3<T>& vec3<T>::operator-= (const vec3<T>& b) {
	x -= b.x;
	y -= b.y;
	z -= b.z;
	return *this;
}

template <typename T>
inline vec3<T> cross(const vec3<T>& a, const vec3<T>& b) {
	vec3<T> v;

    v.x = a.y * b.z - a.z * b.y;
    v.y = a.z * b.x - a.x * b.z;
    v.z = a.x * b.y - a.y * b.x;

	return v;
}

template <typename T>
inline std::ostream& operator<< (std::ostream& out, const vec3<T>& v) {
	out << '(' << v.x << ", " << v.y << ", " << v.z << ')';
	return out;
}

/* conversions */

inline float radian (float t) {
	return  t * 2.0f * M_PI / 360.0f;
}

inline vec3<float> cartesian (float theta_deg, float phi_deg, float r) {
	float theta = radian(theta_deg), phi = radian(phi_deg);
	float sin_theta = sin(theta);
	return vec3<float> (r * sin_theta * cos(phi), r * cos(theta), r * sin_theta * sin(phi));
}