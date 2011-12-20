#include "math.hh"
#include <cmath>

/* vec */

inline vec::vec () {
}

inline vec::vec (float x, float y, float z)
	: x (x),
	  y (y),
	  z (z),
	  w (1.0) {
}

inline float& vec::operator[] (int i) {
	return (&x)[i];
}

inline const float& vec::operator[] (int i) const {
	return (&x)[i];
}

/* mat */
inline float& mat::operator[] (int i) {
	return m[i];
}

inline const float& mat::operator[] (int i) const {
	return m[i];
}

inline float& mat::operator() (int i, int j) {
	return m[j * 4 + i];
}

inline const float& mat::operator() (int i, int j) const {
	return m[j * 4 + i];
}

inline float* mat::raw () {
	return m;
}

inline mat mat::identity () {
	mat m;
	
	m(0,0) = 1; m(0,1) = 0; m(0,2) = 0; m(0,3) = 0;
	m(1,0) = 0; m(1,1) = 1; m(1,2) = 0; m(1,3) = 0;
	m(2,0) = 0; m(2,1) = 0; m(2,2) = 1; m(2,3) = 0;
	m(3,0) = 0; m(3,1) = 0; m(3,2) = 0; m(3,3) = 1;
	
	return m;
}

inline mat mat::rotation (float x, float y, float z, float t) {
	mat m;

	const float tr =  radian(t);
	const float c = std::cos(tr);
	const float s = std::sin(tr);
	const float ic = 1 - c;
	
	m(0,0) = x*x + (1 - x*x)*c; m(0,1) = ic*x*y - z*s; 		m(0,2) = ic*x*z + y*s; 		m(0,3) = 0;
	m(1,0) = ic*x*y + z*s; 		m(1,1) = y*y + (1 - y*y)*c; m(1,2) = ic*y*z - x*s; 		m(1,3) = 0;
	m(2,0) = ic*x*z - y*s; 		m(2,1) = ic*y*z + x*s;		m(2,2) = z*z + (1 - z*z)*c; m(2,3) = 0;
	m(3,0) = 0;					m(3,1) = 0;					m(3,2) = 0;					m(3,3) = 1;
	
	return m;
}

/*
 * axis of the origin space expressed in the targeted space
 */
inline mat mat::rotation (const vec& x_axis, const vec& y_axis, const vec& z_axis) {
	mat m;
	
	m(0,0) = x_axis.x; m(0,1) = x_axis.y; m(0,2) = x_axis.z; m(0,3) = 0;
	m(1,0) = y_axis.x; m(1,1) = y_axis.y; m(1,2) = y_axis.z; m(1,3) = 0;
	m(2,0) = z_axis.x; m(2,1) = z_axis.y; m(2,2) = z_axis.z; m(2,3) = 0;
	m(3,0) = 0;		   m(3,1) = 0;		  m(3,2) = 0;		 m(3,3) = 1;
	
	return m;
}

inline mat mat::scale (float x, float y, float z) {
	mat m;
	
	m(0,0) = x; m(0,1) = 0; m(0,2) = 0; m(0,3) = 0;
	m(1,0) = 0; m(1,1) = y; m(1,2) = 0; m(1,3) = 0;
	m(2,0) = 0; m(2,1) = 0; m(2,2) = z; m(2,3) = 0;
	m(3,0) = 0; m(3,1) = 0; m(3,2) = 0; m(3,3) = 1;
	
	return m;
}

inline mat mat::translation (float x, float y, float z) {
	mat m;
	
	m(0,0) = 1; m(0,1) = 0; m(0,2) = 0; m(0,3) = x;
	m(1,0) = 0; m(1,1) = 1; m(1,2) = 0; m(1,3) = y;
	m(2,0) = 0; m(2,1) = 0; m(2,2) = 1; m(2,3) = z;
	m(3,0) = 0; m(3,1) = 0; m(3,2) = 0; m(3,3) = 1;
	
	return m;
}

inline mat mat::translation (const vec& v) {
	return translation(v.x, v.y, v.z);
}

/* conversions */

inline float radian (float t) {
	return  t * 2.0f * M_PI / 360.0f;
}

inline vec cartesian (float theta_deg, float phi_deg, float r) {
	float theta = radian(theta_deg), phi = radian(phi_deg);
	float sin_theta = sin(theta);
	return vec (r * sin_theta * cos(phi), r * cos(theta), r * sin_theta * sin(phi));
}

/* operations */

inline float length (const vec& v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline vec normalize (const vec& v) {
	float rlen = 1.0 / length(v);
	return v * rlen;
}

inline vec cross(const vec& a, const vec& b) {
	vec v;

    v.x = a.y * b.z - a.z * b.y;
    v.y = a.z * b.x - a.x * b.z;
    v.z = a.x * b.y - a.y * b.x;

	return v;
}

inline vec operator- (const vec& v) {
	return vec(-v.x, -v.y, -v.z);
}

inline vec operator+ (const vec& a, const vec& b) {
	return vec(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline vec operator- (const vec& a, const vec& b) {
	return vec(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline vec operator* (const vec& a, float b) {
	return vec(a.x * b, a.y * b, a.z * b);
}

inline vec operator* (float b, const vec& a) {
	return a * b;
}

inline mat operator* (const mat& a, const mat& b) {
	mat c;
	
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			c(i, j) = 0;
			for (int k = 0; k < 4; k++)
				c(i,j) += a(i,k) * b(k, j);
		}
	
	return c;
}

inline std::ostream& operator<< (std::ostream& out, const mat& m) {
	out << m(0,0) << ",\t" << m(0,1) << ",\t" << m(0,2) << ",\t" << m(0,3) << std::endl;
	out << m(1,0) << ",\t" << m(1,1) << ",\t" << m(1,2) << ",\t" << m(1,3) << std::endl;
	out << m(2,0) << ",\t" << m(2,1) << ",\t" << m(2,2) << ",\t" << m(2,3) << std::endl;
	out << m(3,0) << ",\t" << m(3,1) << ",\t" << m(3,2) << ",\t" << m(3,3);
	return out;
}

inline std::ostream& operator<< (std::ostream& out, const vec& v) {
	out << '(' << v.x << ", " << v.y << ", " << v.z << ')';
	return out;
}