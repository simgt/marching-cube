#ifndef VOLUME_HH
#define VOLUME_HH

#include <util/array3.hh>

struct Voxel {
	char density;
	uchar material;
};

typedef array3<Voxel,
			   MAP_BLOCK_SIZE,
			   MAP_BLOCK_SIZE,
			   MAP_BLOCK_SIZE> Block;

template <typename T, typename A, typename R>
class VolumeSampler {
public:
	VolumeSampler (T& volume)
		: volume (volume) {
	};

	~VolumeSampler () {
		acc.release();
	}

	R& operator() (Vec3i p) {
		Vec3i b = floor(p, MAP_BLOCK_SIZE) / MAP_BLOCK_SIZE; // Block's coordinates
		
		if (acc.empty() || b != acc->first) {
			acc.release();
			volume.find(acc, b);
		}

		assert(!acc.empty());

		p -= floor(p, MAP_BLOCK_SIZE); // Block-relative Voxel's coordinates
		return acc->second(p);
	};

	inline R& operator() (int x, int y, int z) {
		return operator()(Vec3i(x, y, z));
	};

	void release () {
		acc.release();
	}

private:
	T& volume;
	A acc;
};

class Volume : public tbb::concurrent_hash_map<Vec3i, Block, Vec3iCompare> {
public:
	typedef VolumeSampler<Volume, Volume::accessor, Voxel> Sampler;
	typedef VolumeSampler<const Volume, Volume::const_accessor, const Voxel> ConstSampler;
};

#endif