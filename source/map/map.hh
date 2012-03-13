#ifndef MAP_HH
#define MAP_HH

#include <global.hh>

#include <util/h3d.hh>
#include <util/concurrent_unique_queue.hh>

#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_hash_map.h>
#include <tbb/compat/thread>

#define MAP_BLOCK_SIZE 32
#define MAP_CHUNK_SIZE 16

#include "volume.hh"
#include "surface.hh"

/* ----- *
 *  MAP  *
 * ----- */

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
	ConcurrentUniqueQueue<Vec3i> chunk_queue;
	//tbb::concurrent_bounded_queue<Vec3i> chunk_queue;
	tbb::concurrent_queue<GeometryPayload> geometry_queue;

	friend void worker_task (Map* const map);
};

/* ---------- *
 *  PIPELINE  *
 * ---------- */

void generate (Block& block, const Vec3i coords);
bool triangulate (GeometryPayload& payload, const Volume& volume, const Vec3i& coords);
void upload (Surface& surface, const H3DNode parent, const GeometryPayload& payload);

bool marching_cube (Volume::ConstSampler& sampler,
					const Vec3i& offset,
					std::vector<Vec3f>& positions,
					std::vector<Vec3f>& normals,
					std::vector<uint>& triangles);

#endif