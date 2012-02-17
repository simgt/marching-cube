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

#define MAP_VIEW_DISTANCE 5
#define MAP_VIEW_AREA (2 * MAP_VIEW_DISTANCE + 1) * (2 * MAP_VIEW_DISTANCE + 1) * (2 * MAP_VIEW_DISTANCE + 1)
#define MAP_CHUNKS_PER_ROUND 10

#define MAP_CHUNK_SIZE_X 10
#define MAP_CHUNK_SIZE_Y 10
#define MAP_CHUNK_SIZE_Z 10

namespace Map {
	/* types */
	typedef array3<float, MAP_CHUNK_SIZE_X + 1, MAP_CHUNK_SIZE_Y + 1, MAP_CHUNK_SIZE_Z + 1> chunk_raw_data_t;
	
	struct Chunk {
		vec3i position;
		chunk_raw_data_t* grid;
		uint vertices_count;
		uint elements_count;
		ResourceBlock* block;
		H3DNode node;

		Chunk (const vec3i& position)
			: position (position) {
		};
	};
	
	/* fonctors */

	/* ChunkAllocator
	 * 
	 * Initialized with a map position
	 * Generate 'Chunk' objects which distance to 'middle' is
	 * less or equal to MAP_VIEW_DISTANCE */

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
		
	/* functions */
	void update (const vec3f&, tbb::concurrent_bounded_queue<vec3i>& queue);
	std::thread* launch_worker (H3DNode parent, tbb::concurrent_bounded_queue<vec3i>* queue);
	void marching_cube (const chunk_raw_data_t& grid,
						std::vector<vec3f>& positions, std::vector<vec3f>& normals, std::vector<uint>& triangles);
}

#endif