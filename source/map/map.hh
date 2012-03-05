#ifndef MAP_HH
#define MAP_HH

#include <global.hh>
#include <util/h3d.hh>
#include <util/algorithm.hh>

#include <tbb/pipeline.h>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_hash_map.h>
#include <tbb/compat/thread>

#include <Horde3D/Horde3D.h>
#include <Horde3DUtils/Horde3DUtils.h>

#include <vector>
#include <map>

#define MAP_VIEW_DISTANCE 2
#define MAP_VIEW_AREA (2 * MAP_VIEW_DISTANCE + 1) \
					* (2 * MAP_VIEW_DISTANCE + 1) \
					* (2 * MAP_VIEW_DISTANCE + 1)
#define MAP_VIEW_COUNT (2 * MAP_VIEW_DISTANCE + 1)

#define MAP_CHUNKS_PER_ROUND 30

#define MAP_CHUNK_SIZE_X 20
#define MAP_CHUNK_SIZE_Y 20
#define MAP_CHUNK_SIZE_Z 20

#define MAP_BLOCK_SIZE 32
#define MAP_CHUNK_SIZE 16

#define MAP_BUFFER_SIZE_XZ 20
#define MAP_BUFFER_SIZE_Y  20

/* ----- *
 *  MAP  *
 * ----- */

struct Voxel {
	char density;
	uchar material;
};

typedef array3<Voxel,
			   MAP_BLOCK_SIZE,
			   MAP_BLOCK_SIZE,
			   MAP_BLOCK_SIZE> Block;

struct Chunk {
	H3DNode node;
	H3DRes geometry;
	H3DNode mesh;
	H3DRes material;

	Chunk ()
		: node (0),
		  geometry (0),
		  mesh (0),
		  material (0) {
	};
};

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

struct Vec3iCompare {
	static size_t hash (const Vec3i& a) {
		return a[0] * 256 + a[1] * 128 + a[2];
	};

	static bool equal (const Vec3i& a, const Vec3i& b) {
		return a == b;
	};

	bool operator() (const Vec3i& a, const Vec3i& b) {
		return a[0] < b[0] ? true
			 : a[0] > b[0] ? false
			 : a[1] < b[1] ? true
			 : a[1] > b[1] ? false
			 : a[2] < b[2] ? true
			 : false;
	};
};

class Volume : public tbb::concurrent_hash_map<Vec3i, Block, Vec3iCompare> {
public:
	typedef VolumeSampler<Volume, Volume::accessor, Voxel> Sampler;
	typedef VolumeSampler<const Volume, Volume::const_accessor, const Voxel> ConstSampler;
};

typedef std::map<Vec3i, Chunk, Vec3iCompare> Surface;

struct GeometryPayload {
	Vec3i position; // TODO Vec3i ?
	size_t vertices_count;
	size_t elements_count;
	ResourceBlock* resource;
};

class Map {
public:
	Map (const H3DNode);
	void update (const Vec3f&);
	void modify (const Vec3f& position, char value);

private:
	const H3DNode parent;

	/* volume */
	Volume volume;
	Surface surface;

	/* worker */
	std::thread worker;
	tbb::concurrent_bounded_queue<Vec3i> chunk_queue;
	tbb::concurrent_queue<GeometryPayload> geometry_queue;

	friend void worker_task (Map* const map);
};

/* ---------- *
 *  PIPELINE  *
 * ---------- */

void generate (Block& block, const Vec3i coords);
bool triangulate (GeometryPayload& payload, const Volume& volume, const Vec3i& coords);
void upload (Surface& surface, const H3DNode parent, const GeometryPayload& payload);

/* ---------- *
 * ALGORITHMS *
 * ---------- */

bool marching_cube (Volume::ConstSampler& sampler,
					const Vec3i& offset,
					std::vector<Vec3f>& positions,
					std::vector<Vec3f>& normals,
					std::vector<uint>& triangles);

#endif