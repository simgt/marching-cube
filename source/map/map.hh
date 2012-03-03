#ifndef MAP_HH
#define MAP_HH

#include <global.hh>
#include <util/math.hh>
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

class Map;

typedef array3<volatile char,
			   MAP_CHUNK_SIZE_X,
			   MAP_CHUNK_SIZE_Y,
			   MAP_CHUNK_SIZE_Z> chunk_data_array;

/* ----- *
 *  MAP  *
 * ----- */

struct voxel {
	char density;
	uchar material;
};

typedef array3<voxel,
			   MAP_BLOCK_SIZE,
			   MAP_BLOCK_SIZE,
			   MAP_BLOCK_SIZE> block;

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

typedef tbb::concurrent_hash_map<vec3i, block> block_table;
typedef std::map<vec3i, Chunk> chunk_table;

class const_volume_sampler {
public:
	const_volume_sampler (const block_table& volume)
		: volume (volume) {
	};

	~const_volume_sampler () {
		acc.release();
	}

	const voxel& operator() (vec3i p) {
		vec3i b = floor(p, MAP_BLOCK_SIZE) / MAP_BLOCK_SIZE; // block's coordinates
		
		if (acc.empty() || b != acc->first) {
			acc.release();
			volume.find(acc, b);
		}

		assert(!acc.empty());

		p -= floor(p, MAP_BLOCK_SIZE); // block-relative voxel's coordinates
		return acc->second(p);
	};

	inline const voxel& operator() (int x, int y, int z) {
		return operator()(vec3i(x, y, z));
	};

	void release () {
		acc.release();
	}

private:
	const block_table& volume;
	block_table::const_accessor acc;
};

struct GeometryPayload {
	vec3f position;
	size_t vertices_count;
	size_t elements_count;
	ResourceBlock* resource;
};

class Map {
public:
	Map (const H3DNode);
	void update (const vec3f&);
	void modify (const vec3i& chunk, const vec3f& position, char value);

	voxel& operator() (const vec3i p);
	const voxel& operator() (const vec3i p) const;

private:
	const H3DNode parent;

	/* volume */
	block_table volume;
	chunk_table surface;

	/* worker */
	std::thread worker;
	tbb::concurrent_bounded_queue<vec3i> chunk_queue;
	tbb::concurrent_queue<GeometryPayload> geometry_queue;

	friend void worker_task (Map* const map);
};

/* ---------- *
 * ALGORITHMS *
 * ---------- */

bool marching_cube (const_volume_sampler& sampler,
					const vec3i& offset,
					std::vector<vec3f>& positions,
					std::vector<vec3f>& normals,
					std::vector<uint>& triangles);

#endif