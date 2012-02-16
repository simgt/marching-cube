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

#define MAP_VIEW_DISTANCE 20
#define MAP_VIEW_AREA (2 * MAP_VIEW_DISTANCE + 1) * (2 * MAP_VIEW_DISTANCE + 1) * (2 * MAP_VIEW_DISTANCE + 1)

namespace Map {
	/* types */
	struct Chunk {
		vec3i position;
		uint vertices_count;
		uint elements_count;
		ResourceBlock* block;
		H3DNode node;

		Chunk (const vec3i& position)
			: position (position) {
		};
	};
	
	/* constants */
	const vec3i chunk_size (10, 10, 10);
	
	/* fonctors */
	class ChunkAllocator : public tbb::filter {
	public:
		ChunkAllocator ();
		void* operator() (void*);
		void set_middle (const vec3i& middle);
	private:
		vec3i middle;
		vec3i previous;
		vec3i it;
	};

	class ChunkTriangulator : public tbb::filter {
	public:
		ChunkTriangulator ();
		void* operator() (void* chunk);
	};
	
	class ChunkUploader : public tbb::thread_bound_filter {
	public:
		ChunkUploader ();
		void* operator() (void* chunk);
		void set_parent (const H3DNode parent);
	private:
		H3DNode parent;
		circular_array<
			circular_array<
				circular_array<
					H3DNode,
					2 * MAP_VIEW_DISTANCE + 1>,
				2 * MAP_VIEW_DISTANCE + 1>,
			2 * MAP_VIEW_DISTANCE + 1> buffer;
	};
	
	/* globals */
	extern ChunkUploader chunk_uploader;
		
	/* functions */
	void update (const vec3i&, tbb::concurrent_bounded_queue<vec3i>& queue);
	std::thread* launch_worker (H3DNode parent, tbb::concurrent_bounded_queue<vec3i>* queue);
	void marching_cube (const vec3i offset, std::vector<vec3f>& positions, std::vector<vec3f>& normals, std::vector<uint>& triangles);
}

#endif