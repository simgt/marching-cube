#ifndef ALGORITHM_HH
#define ALGORITHM_HH

#include <array>

template <typename T, uint S>
struct circular_array {
	T data[S];

	circular_array () {
	};
	
	circular_array (T v) {
		for (uint i = 0; i < S; i++)
			data[i] = T(v);
	};
	
	circular_array<T, S> operator= (const circular_array<T, S>& v) {
		for (uint i = 0; i < S; i++)
			data[i] = v.data[i];
	}

	T& operator[] (int i) {
		return data[i >= 0 ? i % S : (S - (-i % S)) % S];
	};
	
	const T& operator[] (int i) const {
		return data[i >= 0 ? i % S : (S - (-i % S)) % S];
	};
	
	//offset = m >= 0 ? (offset + m) % 8 : (offset + 8 - (-m % 8)) % 8;
};

template <typename T, size_t X, size_t Y, size_t Z>
class array3 : public std::array<T, X * Y * Z> {
public:
	typename std::array<T, X * Y * Z>::reference operator() (size_t x, size_t y, size_t z) {
		return std::array<T, X * Y * Z>::operator[](x * Y * Z + y * Z + z);
	};
	
	typename std::array<T, X * Y * Z>::const_reference operator() (size_t x, size_t y, size_t z) const {
		return std::array<T, X * Y * Z>::operator[](x * Y * Z + y * Z + z);
	};
};

#endif