#include "math.hh"
#include <cmath>

/* =========
 *	 vec3
 * ========= */

template <typename T>
inline vec3<T>::vec3 () {
}

template <typename T>
inline vec3<T>::vec3 (T v)
	: x (v),
	  y (v),
	  z (v) {
}

template <typename T>
inline vec3<T>::vec3 (T x, T y, T z)
	: x (x),
	  y (y),
	  z (z) {
}

template <typename T>
inline T& vec3<T>::operator[] (int i) {
	return (&x)[i];
}

template <typename T>
inline const T& vec3<T>::operator[] (int i) const {
	return (&x)[i];
}

template <typename T>
inline float vec3<T>::length () const {
    return sqrtf(x * x + y * y + z * z);
}

template <typename T>
inline void vec3<T>::length (float l) {
	assert(x != 0 || y != 0 || z != 0);
	float ratio = l / length();
	x *= ratio;
	y *= ratio;
	z *= ratio;
}

template <typename T>
inline void vec3<T>::normalize () {
	length(1);
}

template <typename T>
inline vec3<T> vec3<T>::operator- () const {
	return vec3<T>(-x, -y, -z);
}

template <typename T>
vec3<T> vec3<T>::operator+ (T b) const {
	return vec3<T>(x + b, y + b, z + b);
}

template <typename T>
inline vec3<T> vec3<T>::operator+ (const vec3<T>& b) const {
	return vec3<T>(x + b.x, y + b.y, z + b.z);
}

template <typename T>
vec3<T> vec3<T>::operator- (T b) const {
	return vec3<T>(x - b, y - b, z - b);
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
inline vec3<T> vec3<T>::operator* (const vec3<T>& b) const {
	return vec3<T>(x * b.x, y * b.y, z * b.z);
}

template <typename T>
inline vec3<T> vec3<T>::operator/ (T b) const {
	return vec3<T>(x / b, y / b, z / b);
}

template <typename T>
inline bool vec3<T>::operator== (const vec3<T>& v) const {
	return x == v.x && y == v.y && z == v.z;
}

template <typename T>
inline bool vec3<T>::operator!= (T v) const {
	return x != v || y != v || z != v;
}

template <typename T>
inline bool vec3<T>::operator!= (const vec3<T>& v) const {
	return x != v.x || y != v.y || z != v.z;
}

template <typename T>
bool vec3<T>::operator>= (const vec3<T>& v) const {
	return x >= v.x && y >= v.y && z >= v.z;
}

template <typename T>
bool vec3<T>::operator<= (const vec3<T>& v) const {
	return x <= v.x && y <= v.y && z <= v.z;
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
inline vec3<T>& vec3<T>::operator*= (T v) {
	x *= v;
	y *= v;
	z *= v;
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
inline T dot (const vec3<T>& u, const vec3<T>& v) {
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

template <typename T>
inline vec3<T> floor (const vec3<T>& v) {
	return vec3<T>(floor(v.x), floor(v.y), floor(v.z));
}

template <typename T>
inline std::ostream& operator<< (std::ostream& out, const vec3<T>& v) {
	out << '(' << v.x << ", " << v.y << ", " << v.z << ')';
	return out;
}


/* =========
 *	 mat4f
 * ========= */

inline float& mat4f::operator[] (int i) {
	return m[i];
}

inline const float& mat4f::operator[] (int i) const {
	return m[i];
}

inline float& mat4f::operator() (int i, int j) {
	return m[j * 4 + i];
}

inline const float& mat4f::operator() (int i, int j) const {
	return m[j * 4 + i];
}

inline float* mat4f::raw () {
	return m;
}

inline mat4f mat4f::identity () {
	mat4f m;
	
	m(0,0) = 1; m(0,1) = 0; m(0,2) = 0; m(0,3) = 0;
	m(1,0) = 0; m(1,1) = 1; m(1,2) = 0; m(1,3) = 0;
	m(2,0) = 0; m(2,1) = 0; m(2,2) = 1; m(2,3) = 0;
	m(3,0) = 0; m(3,1) = 0; m(3,2) = 0; m(3,3) = 1;
	
	return m;
}

inline mat4f mat4f::rotation (float x, float y, float z, float t) {
	mat4f m;

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

// axis of the origin space expressed in the targeted space
inline mat4f mat4f::rotation (const vec3f& x_axis, const vec3f& y_axis, const vec3f& z_axis) {
	mat4f m;
	
	m(0,0) = x_axis.x; m(0,1) = x_axis.y; m(0,2) = x_axis.z; m(0,3) = 0;
	m(1,0) = y_axis.x; m(1,1) = y_axis.y; m(1,2) = y_axis.z; m(1,3) = 0;
	m(2,0) = z_axis.x; m(2,1) = z_axis.y; m(2,2) = z_axis.z; m(2,3) = 0;
	m(3,0) = 0;		   m(3,1) = 0;		  m(3,2) = 0;		 m(3,3) = 1;
	
	return m;
}

inline mat4f mat4f::scale (float x, float y, float z) {
	mat4f m;
	
	m(0,0) = x; m(0,1) = 0; m(0,2) = 0; m(0,3) = 0;
	m(1,0) = 0; m(1,1) = y; m(1,2) = 0; m(1,3) = 0;
	m(2,0) = 0; m(2,1) = 0; m(2,2) = z; m(2,3) = 0;
	m(3,0) = 0; m(3,1) = 0; m(3,2) = 0; m(3,3) = 1;
	
	return m;
}

inline mat4f mat4f::translation (float x, float y, float z) {
	mat4f m;
	
	m(0,0) = 1; m(0,1) = 0; m(0,2) = 0; m(0,3) = x;
	m(1,0) = 0; m(1,1) = 1; m(1,2) = 0; m(1,3) = y;
	m(2,0) = 0; m(2,1) = 0; m(2,2) = 1; m(2,3) = z;
	m(3,0) = 0; m(3,1) = 0; m(3,2) = 0; m(3,3) = 1;
	
	return m;
}

inline mat4f mat4f::translation (const vec3f& v) {
	return translation(v.x, v.y, v.z);
}

inline mat4f mat4f::inverse (const mat4f& a) {
	mat4f m;
	float d = a.determinant();
	
	if (d == 0)
		return identity();
		
	d = 1.0f / d;
	
	m.n[0][0] = d * (a.n[1][2]*a.n[2][3]*a.n[3][1] - a.n[1][3]*a.n[2][2]*a.n[3][1] + a.n[1][3]*a.n[2][1]*a.n[3][2] - a.n[1][1]*a.n[2][3]*a.n[3][2] - a.n[1][2]*a.n[2][1]*a.n[3][3] + a.n[1][1]*a.n[2][2]*a.n[3][3]);
	m.n[0][1] = d * (a.n[0][3]*a.n[2][2]*a.n[3][1] - a.n[0][2]*a.n[2][3]*a.n[3][1] - a.n[0][3]*a.n[2][1]*a.n[3][2] + a.n[0][1]*a.n[2][3]*a.n[3][2] + a.n[0][2]*a.n[2][1]*a.n[3][3] - a.n[0][1]*a.n[2][2]*a.n[3][3]);
	m.n[0][2] = d * (a.n[0][2]*a.n[1][3]*a.n[3][1] - a.n[0][3]*a.n[1][2]*a.n[3][1] + a.n[0][3]*a.n[1][1]*a.n[3][2] - a.n[0][1]*a.n[1][3]*a.n[3][2] - a.n[0][2]*a.n[1][1]*a.n[3][3] + a.n[0][1]*a.n[1][2]*a.n[3][3]);
	m.n[0][3] = d * (a.n[0][3]*a.n[1][2]*a.n[2][1] - a.n[0][2]*a.n[1][3]*a.n[2][1] - a.n[0][3]*a.n[1][1]*a.n[2][2] + a.n[0][1]*a.n[1][3]*a.n[2][2] + a.n[0][2]*a.n[1][1]*a.n[2][3] - a.n[0][1]*a.n[1][2]*a.n[2][3]);
	m.n[1][0] = d * (a.n[1][3]*a.n[2][2]*a.n[3][0] - a.n[1][2]*a.n[2][3]*a.n[3][0] - a.n[1][3]*a.n[2][0]*a.n[3][2] + a.n[1][0]*a.n[2][3]*a.n[3][2] + a.n[1][2]*a.n[2][0]*a.n[3][3] - a.n[1][0]*a.n[2][2]*a.n[3][3]);
	m.n[1][1] = d * (a.n[0][2]*a.n[2][3]*a.n[3][0] - a.n[0][3]*a.n[2][2]*a.n[3][0] + a.n[0][3]*a.n[2][0]*a.n[3][2] - a.n[0][0]*a.n[2][3]*a.n[3][2] - a.n[0][2]*a.n[2][0]*a.n[3][3] + a.n[0][0]*a.n[2][2]*a.n[3][3]);
	m.n[1][2] = d * (a.n[0][3]*a.n[1][2]*a.n[3][0] - a.n[0][2]*a.n[1][3]*a.n[3][0] - a.n[0][3]*a.n[1][0]*a.n[3][2] + a.n[0][0]*a.n[1][3]*a.n[3][2] + a.n[0][2]*a.n[1][0]*a.n[3][3] - a.n[0][0]*a.n[1][2]*a.n[3][3]);
	m.n[1][3] = d * (a.n[0][2]*a.n[1][3]*a.n[2][0] - a.n[0][3]*a.n[1][2]*a.n[2][0] + a.n[0][3]*a.n[1][0]*a.n[2][2] - a.n[0][0]*a.n[1][3]*a.n[2][2] - a.n[0][2]*a.n[1][0]*a.n[2][3] + a.n[0][0]*a.n[1][2]*a.n[2][3]);
	m.n[2][0] = d * (a.n[1][1]*a.n[2][3]*a.n[3][0] - a.n[1][3]*a.n[2][1]*a.n[3][0] + a.n[1][3]*a.n[2][0]*a.n[3][1] - a.n[1][0]*a.n[2][3]*a.n[3][1] - a.n[1][1]*a.n[2][0]*a.n[3][3] + a.n[1][0]*a.n[2][1]*a.n[3][3]);
	m.n[2][1] = d * (a.n[0][3]*a.n[2][1]*a.n[3][0] - a.n[0][1]*a.n[2][3]*a.n[3][0] - a.n[0][3]*a.n[2][0]*a.n[3][1] + a.n[0][0]*a.n[2][3]*a.n[3][1] + a.n[0][1]*a.n[2][0]*a.n[3][3] - a.n[0][0]*a.n[2][1]*a.n[3][3]);
	m.n[2][2] = d * (a.n[0][1]*a.n[1][3]*a.n[3][0] - a.n[0][3]*a.n[1][1]*a.n[3][0] + a.n[0][3]*a.n[1][0]*a.n[3][1] - a.n[0][0]*a.n[1][3]*a.n[3][1] - a.n[0][1]*a.n[1][0]*a.n[3][3] + a.n[0][0]*a.n[1][1]*a.n[3][3]);
	m.n[2][3] = d * (a.n[0][3]*a.n[1][1]*a.n[2][0] - a.n[0][1]*a.n[1][3]*a.n[2][0] - a.n[0][3]*a.n[1][0]*a.n[2][1] + a.n[0][0]*a.n[1][3]*a.n[2][1] + a.n[0][1]*a.n[1][0]*a.n[2][3] - a.n[0][0]*a.n[1][1]*a.n[2][3]);
	m.n[3][0] = d * (a.n[1][2]*a.n[2][1]*a.n[3][0] - a.n[1][1]*a.n[2][2]*a.n[3][0] - a.n[1][2]*a.n[2][0]*a.n[3][1] + a.n[1][0]*a.n[2][2]*a.n[3][1] + a.n[1][1]*a.n[2][0]*a.n[3][2] - a.n[1][0]*a.n[2][1]*a.n[3][2]);
	m.n[3][1] = d * (a.n[0][1]*a.n[2][2]*a.n[3][0] - a.n[0][2]*a.n[2][1]*a.n[3][0] + a.n[0][2]*a.n[2][0]*a.n[3][1] - a.n[0][0]*a.n[2][2]*a.n[3][1] - a.n[0][1]*a.n[2][0]*a.n[3][2] + a.n[0][0]*a.n[2][1]*a.n[3][2]);
	m.n[3][2] = d * (a.n[0][2]*a.n[1][1]*a.n[3][0] - a.n[0][1]*a.n[1][2]*a.n[3][0] - a.n[0][2]*a.n[1][0]*a.n[3][1] + a.n[0][0]*a.n[1][2]*a.n[3][1] + a.n[0][1]*a.n[1][0]*a.n[3][2] - a.n[0][0]*a.n[1][1]*a.n[3][2]);
	m.n[3][3] = d * (a.n[0][1]*a.n[1][2]*a.n[2][0] - a.n[0][2]*a.n[1][1]*a.n[2][0] + a.n[0][2]*a.n[1][0]*a.n[2][1] - a.n[0][0]*a.n[1][2]*a.n[2][1] - a.n[0][1]*a.n[1][0]*a.n[2][2] + a.n[0][0]*a.n[1][1]*a.n[2][2]);
	
	return m;
}

inline float mat4f::determinant() const {
	return 
		n[0][3]*n[1][2]*n[2][1]*n[3][0] - n[0][2]*n[1][3]*n[2][1]*n[3][0] - n[0][3]*n[1][1]*n[2][2]*n[3][0] + n[0][1]*n[1][3]*n[2][2]*n[3][0] +
		n[0][2]*n[1][1]*n[2][3]*n[3][0] - n[0][1]*n[1][2]*n[2][3]*n[3][0] - n[0][3]*n[1][2]*n[2][0]*n[3][1] + n[0][2]*n[1][3]*n[2][0]*n[3][1] +
		n[0][3]*n[1][0]*n[2][2]*n[3][1] - n[0][0]*n[1][3]*n[2][2]*n[3][1] - n[0][2]*n[1][0]*n[2][3]*n[3][1] + n[0][0]*n[1][2]*n[2][3]*n[3][1] +
		n[0][3]*n[1][1]*n[2][0]*n[3][2] - n[0][1]*n[1][3]*n[2][0]*n[3][2] - n[0][3]*n[1][0]*n[2][1]*n[3][2] + n[0][0]*n[1][3]*n[2][1]*n[3][2] +
		n[0][1]*n[1][0]*n[2][3]*n[3][2] - n[0][0]*n[1][1]*n[2][3]*n[3][2] - n[0][2]*n[1][1]*n[2][0]*n[3][3] + n[0][1]*n[1][2]*n[2][0]*n[3][3] +
		n[0][2]*n[1][0]*n[2][1]*n[3][3] - n[0][0]*n[1][2]*n[2][1]*n[3][3] - n[0][1]*n[1][0]*n[2][2]*n[3][3] + n[0][0]*n[1][1]*n[2][2]*n[3][3];
}

inline mat4f mat4f::operator* (const mat4f& b) const {
	const mat4f& a = *this;
	mat4f c;
	
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			c(i, j) = 0;
			for (int k = 0; k < 4; k++)
				c(i,j) += a(i,k) * b(k, j);
		}
	
	return c;
}

inline vec3f mat4f::operator* (const vec3f& b) const {
	const mat4f& m = *this;
	vec3f c;
	
	c.x = m(0,0) * b.x + m(0,1) * b.y + m(0,2) * b.z + m(0,3);
	c.y = m(1,0) * b.x + m(1,1) * b.y + m(1,2) * b.z + m(1,3);
	c.z = m(2,0) * b.x + m(2,1) * b.y + m(2,2) * b.z + m(2,3);
	
	return c;
}

inline std::ostream& operator<< (std::ostream& out, const mat4f& m) {
	out << m(0,0) << ",\t" << m(0,1) << ",\t" << m(0,2) << ",\t" << m(0,3) << std::endl;
	out << m(1,0) << ",\t" << m(1,1) << ",\t" << m(1,2) << ",\t" << m(1,3) << std::endl;
	out << m(2,0) << ",\t" << m(2,1) << ",\t" << m(2,2) << ",\t" << m(2,3) << std::endl;
	out << m(3,0) << ",\t" << m(3,1) << ",\t" << m(3,2) << ",\t" << m(3,3);
	return out;
}

/* ===============
 *	 conversions
 * =============== */

inline float radian (float t) {
	return  t * 2.0f * M_PI / 360.0f;
}

inline vec3f cartesian (float theta_deg, float phi_deg, float r) {
	float theta = radian(theta_deg), phi = radian(phi_deg);
	float sin_theta = sin(theta);
	return vec3f(r * sin_theta * cos(phi), r * cos(theta), r * sin_theta * sin(phi));
}