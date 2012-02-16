#include "map.hh"
#include <util/algorithm.hh>

#include <tbb/pipeline.h>

#include <iostream>
#include <sstream>

namespace Map {
	ChunkAllocator chunk_allocator;
	ChunkTriangulator chunk_triangulator;
	ChunkUploader chunk_uploader;
	tbb::pipeline chunk_pipeline;

	void worker (tbb::concurrent_bounded_queue<vec3i>* queue) {	
		vec3i middle;
	
		while (true) {
			queue->pop(middle);
			chunk_allocator.set_middle(middle);
			chunk_pipeline.run(MAP_VIEW_AREA);
		}
	}
	
	std::thread* launch_worker (H3DNode parent, tbb::concurrent_bounded_queue<vec3i>* queue) {	
		Map::chunk_pipeline.add_filter(Map::chunk_allocator);
		Map::chunk_pipeline.add_filter(Map::chunk_triangulator);
		Map::chunk_pipeline.add_filter(Map::chunk_uploader);
		
		Map::chunk_uploader.set_parent(parent);

		return new std::thread(worker, queue);
	}
	
	void update (const vec3i& p, tbb::concurrent_bounded_queue<vec3i>& queue) {
		static vec3i middle(1 << 15); // TODO FIX THIS SHIT !!

		for (int i = 0; i < 10 && Map::chunk_uploader.try_process_item() != tbb::thread_bound_filter::end_of_stream; i++)
			; // upload at most 5 chunks per round

		if (middle == p)
			return;

		std::cout << middle << " --> " << p << std::endl;		
		queue.push(p);
		middle = p;
	}

	/* ======= *
	 * FILTERS *
	 * ======= */

	ChunkAllocator::ChunkAllocator ()
		: tbb::filter (tbb::filter::parallel),
		  middle (1 << 15),
		  previous (1 << 15), // TODO FIX THIS SHIT !!
		  it (0 - MAP_VIEW_DISTANCE) {
	}

	void* ChunkAllocator::operator() (void*) {
		// increment 'it' while in the previous bounds
		while (it.x >= previous.x - MAP_VIEW_DISTANCE && it.y >= previous.y - MAP_VIEW_DISTANCE && it.z >= previous.z - MAP_VIEW_DISTANCE
		 	&& it.x <= previous.x + MAP_VIEW_DISTANCE && it.y <= previous.y + MAP_VIEW_DISTANCE && it.z <= previous.z + MAP_VIEW_DISTANCE) {
				it.z++;
				if (it.z > middle.z + MAP_VIEW_DISTANCE) {
					it.z = middle.z - MAP_VIEW_DISTANCE;
					it.y++;
				}

				if (it.y > middle.y + MAP_VIEW_DISTANCE) {
					it.y = middle.y - MAP_VIEW_DISTANCE;
					it.x++;
				}

				if (it.x > middle.x + MAP_VIEW_DISTANCE) return 0;
		}
		
		// if the chunk wasn't in the previous bounds, generate it
		Chunk* chunk = new Chunk(it);
		
		// iterate one more time for the next call
		it.z++;
		if (it.z > middle.z + MAP_VIEW_DISTANCE) {
			it.z = middle.z - MAP_VIEW_DISTANCE;
			it.y++;
		}
		
		if (it.y > middle.y + MAP_VIEW_DISTANCE) {
			it.y = middle.y - MAP_VIEW_DISTANCE;
			it.x++;
		}
		
		if (it.x > middle.x + MAP_VIEW_DISTANCE) return 0;
	
		return (void*)chunk;
	}

	void ChunkAllocator::set_middle (const vec3i& middle) {
		this->previous = this->middle;
		this->middle = middle;
		this->it = middle - MAP_VIEW_DISTANCE;
	}


	ChunkTriangulator::ChunkTriangulator ()
		: tbb::filter (tbb::filter::parallel) {
	}

	void* ChunkTriangulator::operator()  (void* ptr) {
		Chunk* chunk = (Chunk*)ptr;
		std::vector<vec3f> positions;
		std::vector<vec3f> normals;
		std::vector<uint> elements;
	
		// run marching cube algorithm
		//std::cout << "triangulating " << chunk->position << std::endl;
		Map::marching_cube(chunk->position * chunk_size, positions, normals, elements);
	
		if (positions.size() == 0 || elements.size() == 0) {
			delete chunk;
			return 0;
		}
	
		// convert vec3f normals to vec3s
		std::vector<vec3s> normals_short (normals.size());
		for (uint i = 0; i < normals.size(); i++)
			normals_short[i] = normals[i] * 32767;
	
		chunk->vertices_count = positions.size();
		chunk->elements_count = elements.size();
	
		// create the resource block
		chunk->block = create_geometry_data(
			positions.size(),
			elements.size(),
			(float*)positions.data(),
			(uint*)elements.data(),
			(short*)normals_short.data(),
			0, 0, 0, 0
		);
	
		return ptr;
	}

	/* In main thread:
	 * chunk_uploader.try_process_item();
	 */

	ChunkUploader::ChunkUploader ()
		: tbb::thread_bound_filter (tbb::filter::serial_out_of_order),
		  parent (0) {
			vec3i it;
			for (int x = 0; x < 2 * MAP_VIEW_DISTANCE + 1; x++)
				for (int y = 0; y < 2 * MAP_VIEW_DISTANCE + 1; y++)
					for (int z = 0; z < 2 * MAP_VIEW_DISTANCE + 1; z++)
						buffer[x][y][z] = 0;
	}

	void* ChunkUploader::operator() (void* ptr) {
		if (ptr == 0) return 0;
		
		Chunk* chunk = (Chunk*)ptr;
		std::stringstream name ("chunk");
		name << chunk->position;
	
		//std::cout << "uploading " << chunk->position << std::endl;
	
		H3DRes geometry = h3dAddResource(chunk->block->type, name.str().c_str(), 0);
		if (geometry) h3dLoadResource(geometry, chunk->block->data, chunk->block->size);

		chunk->node = h3dAddModelNode(parent, name.str().c_str(), geometry);
		
		h3dSetNodeTransform(chunk->node, chunk->position.x * chunk_size.x, chunk->position.y * chunk_size.y, chunk->position.z * chunk_size.z, 0, 0, 0, 1, 1, 1);
	
		H3DRes material = h3dAddResource(H3DResTypes::Material, "materials/mine.material.xml", 0);
		h3dAddMeshNode(chunk->node, "DynGeoMesh", material, 0, chunk->elements_count, 0, chunk->vertices_count - 1);
	
		h3dutLoadResourcesFromDisk(".");
	
		delete[] chunk->block; // TODO check leak
		delete chunk;

		// finally, replace in the buffer
		//H3DNode old = buffer[chunk->position.x][chunk->position.y][chunk->position.z];
		//if (old != 0) h3dRemoveNode(old); // can be removed after initialization
		//buffer[chunk->position.x][chunk->position.y][chunk->position.z] = chunk->node;

		return 0;
	}

	void ChunkUploader::set_parent (const H3DNode parent) {
		this->parent = parent;
	}
}
/*
H3DNode Map::worker (H3DNode parent, vec3i chunk) {
	std::stringstream name ("chunk");
	std::vector<vec3f> positions;
	std::vector<vec3f> normals;
	std::vector<uint> elements;

	name << chunk;

	chunk *= Map::chunk_size; // one chunk = a CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE cube

	// in thread:
	Map::marching_cube(chunk, positions, normals, elements);

	// convert vec3f normals to vec3s
	std::vector<vec3s> normals_short (normals.size());
	for (uint i = 0; i < normals.size(); i++) {
		normals_short[i] = normals[i] * 32767;
		//assert(normals_short[i] != 0);
	}
	
	resource_block* block = create_geometry_data(
		positions.size(),
		elements.size(),
		(float*)positions.data(),
		(uint*)elements.data(),
		(short*)normals_short.data(),
		0, 0, 0, 0
	);

	// out of thread:
	H3DRes geometry = h3dAddResource(H3DResTypes::Geometry, name.str().c_str(), 0);
	if (geometry && block) h3dLoadResource(geometry, block->data, block->size);
	delete[] (char*)block;

	H3DRes geometry = h3dutCreateGeometryRes(
							name.str().c_str(),
							positions.size(),
							elements.size(),
							(float*)positions.data(),
							(uint*)elements.data(),
							(short*)normals_short.data(),
							0, 0, 0, 0
					  );

	H3DNode model = h3dAddModelNode(parent, name.str().c_str(), geometry);
	H3DRes material = h3dAddResource(H3DResTypes::Material, "materials/mine.material.xml", 0);

	h3dAddMeshNode(model, "DynGeoMesh", material, 0, elements.size(), 0, positions.size() - 1);
	h3dutLoadResourcesFromDisk(".");

	h3dSetNodeTransform(model, chunk.x, chunk.y, chunk.z, 0, 0, 0, 1, 1, 1);

	return model;
}*/