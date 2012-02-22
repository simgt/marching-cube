#include "map.hh"

#include <tbb/pipeline.h>
#include <sstream>
#include <algorithm>

/* ChunkAllocator */

ChunkAllocator::ChunkAllocator ()
	: tbb::filter (tbb::filter::parallel),
	  middle (1 << 15), // TODO FIX THIS SHIT !!
	  previous (0),
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


/* ChunkGenerator */

uchar density (const vec3i p) {
	//return p.length() - 15;
	//return p.x * p.x + 17 * p.y - p.z * p.z;
	//return p.y <= 0 ? 255 : 0;
	return p.length() < 10 ? 255 : 0;
	/*return -p.y <= 0 ? 0
		 : -p.y >= 255 ? 255
		 : -p.y;*/
}

ChunkGenerator::ChunkGenerator ()
	: tbb::filter (tbb::filter::parallel) {
}

void* ChunkGenerator::operator() (void* ptr) {
	Chunk* chunk = (Chunk*)ptr;
	Chunk::raw_data_t* grid = new Chunk::raw_data_t;
	
	vec3i offset (chunk->position * vec3i(MAP_CHUNK_SIZE_X, MAP_CHUNK_SIZE_Y, MAP_CHUNK_SIZE_Z));
	
	for (int i = 0; i < MAP_CHUNK_SIZE_X + 1; i++)  		//x axis
		for (int j = 0; j < MAP_CHUNK_SIZE_Y + 1; j++)		//y axis
			for (int k = 0; k < MAP_CHUNK_SIZE_Z + 1; k++) 	//z axis
				(*grid)(i, j, k) = density(vec3i(offset.x + i, offset.y + j, offset.z + k));
	
	chunk->grid = grid;
	
	return ptr;
}


/* ChunkTriangulator */

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
	marching_cube(*chunk->grid, positions, normals, elements);

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


/* ChunkUploader */

ChunkUploader::ChunkUploader (const H3DNode parent)
	: tbb::thread_bound_filter (tbb::filter::serial_out_of_order),
	  parent (parent) {
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
	
	h3dSetNodeTransform(
		chunk->node,
		chunk->position.x * MAP_CHUNK_SIZE_X,
		chunk->position.y * MAP_CHUNK_SIZE_Y,
		chunk->position.z * MAP_CHUNK_SIZE_Z,
		0, 0, 0, 1, 1, 1
	);

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