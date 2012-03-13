#ifndef CONCURRENT_UNIQUE_QUEUE_HH
#define CONCURRENT_UNIQUE_QUEUE_HH

#include <list>
#include <tbb/mutex.h>

template <typename T>
class ConcurrentUniqueQueue {
	typedef tbb::mutex Mutex;

public:
	bool push (const T& v) {
		Mutex::scoped_lock lock (_mutex);

		for (auto it = _data.begin(); it != _data.end(); it++)
			if (*it == v) return false;

		_data.push_back(v);
		
		return true;
	};

	bool try_pop (T& r) {
		Mutex::scoped_lock lock (_mutex);		

		if (_data.size() <= 0) return false;

		r = _data.front();
		_data.pop_front();
		
		return true;
	};

private:
	Mutex _mutex;
	std::list<T> _data;
};

#endif