#ifndef ALGORITHM_HH
#define ALGORITHM_HH

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

#endif