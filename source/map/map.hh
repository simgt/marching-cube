#ifndef MAP_HH
#define MAP_HH

#include <global.hh>
#include <util/math.hh>
#include <util/h3d.hh>
#include <util/algorithm.hh>

#include <tbb/pipeline.h>
#include <tbb/concurrent_queue.h>
#include <tbb/compat/thread>

#include <Horde3D/Horde3D.h>
#include <Horde3DUtils/Horde3DUtils.h>

#include <vector>

#define MAP_VIEW_DISTANCE 1
#define MAP_VIEW_AREA (2 * MAP_VIEW_DISTANCE + 1) \
					* (2 * MAP_VIEW_DISTANCE + 1) \
					* (2 * MAP_VIEW_DISTANCE + 1)
#define MAP_CHUNKS_PER_ROUND 10

#define MAP_CHUNK_SIZE_X 10
#define MAP_CHUNK_SIZE_Y 10
#define MAP_CHUNK_SIZE_Z 10

#define MAP_BUFFER_SIZE_XZ 5
#define MAP_BUFFER_SIZE_Y  5

class Map;

/* -------- *
 * PIPELINE *
 * -------- */

/* PayloadAllocator
 * 
 * Initialized with a map position
 * Generate 'Chunk' objects which distance to 'middle' is
 * less or equal to MAP_VIEW_DISTANCE */

class PayloadAllocator : public tbb::filter {
public:
	PayloadAllocator (const Map*);
	void* operator() (void*);
	void set_middle (const vec3i& middle);
private:
	vec3i middle;
	vec3i previous;
	vec3i it;
	const Map* map;
};

/* ChunkGenerator
 * 
 *  */

class ChunkGenerator : public tbb::filter {
public:
	ChunkGenerator ();
	void* operator() (void*);
};

/* ChunkTriangulator
 *
 * Take a Chunk and generate the vertices / triangles
 * associated to it in a geometry blob ready to be uploaded
 * into H3D */

class ChunkTriangulator : public tbb::filter {
public:
	ChunkTriangulator ();
	void* operator() (void* chunk);
};

/* ChunkUploader
 *
 * Create a resource from a chunk geometry raw-data
 * upload it to H3D (then to OpenGL)
 * Create the 'model' object associated to the chunk
 * CAUTION: operator() must be executed in the main thread ! */

class ChunkUploader : public tbb::thread_bound_filter {
public:
	ChunkUploader (Map*, const H3DNode);
	void* operator() (void* chunk);
private:
	Map* map;
	H3DNode parent;
};

/* ----- *
 *  MAP  *
 * ----- */

typedef array3<uchar,
			   MAP_CHUNK_SIZE_X + 1,
			   MAP_CHUNK_SIZE_Y + 1,
			   MAP_CHUNK_SIZE_Z + 1> chunk_data_array;

struct Chunk {
	H3DNode node;
	chunk_data_array data;
};

class Map {
public:
	Map (const H3DNode);
	void update (const vec3f&);
	void modify (const vec3i& chunk, const vec3f& position);

private:
	/* buffer */
	circular_array3<Chunk*, MAP_BUFFER_SIZE_XZ, MAP_BUFFER_SIZE_Y, MAP_BUFFER_SIZE_XZ> buffer;

	/* worker */
	std::thread worker;
	tbb::concurrent_bounded_queue<vec3i> queue;

	/* pipeline */
	PayloadAllocator allocator;
	ChunkGenerator generator;
	ChunkTriangulator triangulator;
	ChunkUploader uploader;
	tbb::pipeline pipeline;

	friend void worker_task (Map*);
	friend class PayloadAllocator;
	friend class ChunkUploader;
};

/* ---------- *
 * ALGORITHMS *
 * ---------- */

void marching_cube (const chunk_data_array& grid,
					std::vector<vec3f>& positions,
					std::vector<vec3f>& normals,
					std::vector<uint>& triangles);

#endif