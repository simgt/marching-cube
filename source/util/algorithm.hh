#ifndef ALGORITHM_HH
#define ALGORITHM_HH

#include <array>

template <typename T, size_t X, size_t Y, size_t Z>
class array3 : public std::array<T, X * Y * Z> {
public:
	typename std::array<T, X * Y * Z>::reference operator() (size_t x, size_t y, size_t z) {
		return std::array<T, X * Y * Z>::operator[](x * Y * Z + y * Z + z);
	};
	
	typename std::array<T, X * Y * Z>::const_reference operator() (size_t x, size_t y, size_t z) const {
		return std::array<T, X * Y * Z>::operator[](x * Y * Z + y * Z + z);
	};

	typename std::array<T, X * Y * Z>::reference operator() (const vec3i p) {
		return std::array<T, X * Y * Z>::operator[](p.x * Y * Z + p.y * Z + p.z);
	};
	
	typename std::array<T, X * Y * Z>::const_reference operator() (const vec3i p) const {
		return std::array<T, X * Y * Z>::operator[](p.x * Y * Z + p.y * Z + p.z);
	};

	typename std::array<T, X * Y * Z>::reference at (size_t x, size_t y, size_t z) {
		return operator()(x, y, z);
	};
	
	typename std::array<T, X * Y * Z>::const_reference at (size_t x, size_t y, size_t z) const {
		return operator()(x, y, z);
	};

	typename std::array<T, X * Y * Z>::reference at (const vec3i p) {
		return operator()(p);
	};
	
	typename std::array<T, X * Y * Z>::const_reference at (const vec3i p) const {
		return operator()(p);
	};
};

template <typename T, size_t X, size_t Y, size_t Z>
class circular_array3 : public array3<T, X, Y, Z> {
public:
	typename array3<T, X, Y, Z>::reference operator() (int x, int y, int z) {
		x = x >= 0 ? x % X : (X - (-x % X)) % X;
		y = y >= 0 ? y % Y : (Y - (-y % Y)) % Y;
		z = z >= 0 ? z % Z : (Z - (-z % Z)) % Z;
		return array3<T, X, Y, Z>::operator()(x, y, z);
	};
	
	typename array3<T, X, Y, Z>::const_reference operator() (int x, int y, int z) const {
		x = x >= 0 ? x % X : (X - (-x % X)) % X;
		y = y >= 0 ? y % Y : (Y - (-y % Y)) % Y;
		z = z >= 0 ? z % Z : (Z - (-z % Z)) % Z;
		return array3<T, X, Y, Z>::operator()(x, y, z);
	};

	typename array3<T, X, Y, Z>::reference operator() (const vec3i p) {
		return operator()(p.x, p.y, p.z);
	};
	
	typename array3<T, X, Y, Z>::const_reference operator() (const vec3i p) const {
		return operator()(p.x, p.y, p.z);
	};
};

#endif