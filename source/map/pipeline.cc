#include "map.hh"

#include <tbb/pipeline.h>
#include <sstream>
#include <algorithm>

/* Payload
 *
 * Structure built and transmitted through the pipeline */

struct Payload {
	vec3i position;
	Chunk* chunk;
	uint vertices_count;
	uint elements_count;
	ResourceBlock* block;

	Payload (const vec3i& position)
		: position (position) {
	};
};

/* ChunkAllocator */

PayloadAllocator::PayloadAllocator (const Map* map)
	: tbb::filter (tbb::filter::serial_in_order),
	  middle (1 << 15), // TODO FIX THIS SHIT !!
	  previous (0),
	  it (0 - MAP_VIEW_DISTANCE),
	  map (map) {
}

void* PayloadAllocator::operator() (void*) {
	// increment 'it' while in the previous bounds
	/*while (it.x >= previous.x - MAP_VIEW_DISTANCE
		&& it.y >= previous.y - MAP_VIEW_DISTANCE
		&& it.z >= previous.z - MAP_VIEW_DISTANCE
	 	&& it.x <= previous.x + MAP_VIEW_DISTANCE
	 	&& it.y <= previous.y + MAP_VIEW_DISTANCE
	 	&& it.z <= previous.z + MAP_VIEW_DISTANCE) {
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
	}*/
	
	// if the chunk wasn't in the previous bounds, generate it
	Payload* payload = new Payload(it);
	
	payload->chunk = map->buffer(it);
	if (payload->chunk == 0)
		payload->chunk = new Chunk;

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
	
	if (it.x > middle.x + MAP_VIEW_DISTANCE) return 0; // TODO forgetting a chunk?

	return (void*)payload;
}

void PayloadAllocator::set_middle (const vec3i& middle) {
	this->previous = this->middle;
	this->middle = middle;
	this->it = middle - MAP_VIEW_DISTANCE;
}


/* ChunkGenerator */

uchar density (const vec3i p) {
	//return p.length() - 15;
	/*int v = p.x * p.x + 17 * p.y - p.z * p.z;
	return v >= 0 ? 0 : 255;*/
	return p.y <= 5 ? 255 : 0;
	//return p.length() < 10 ? 255 : 0;
	/*return -p.y <= 0 ? 0
		 : -p.y >= 255 ? 255
		 : -p.y;*/
}

ChunkGenerator::ChunkGenerator ()
	: tbb::filter (tbb::filter::parallel) {
}

void* ChunkGenerator::operator() (void* ptr) {
	Payload* payload = (Payload*)ptr;

	// skip if the chunks already has data
	if (payload->chunk->node != 0) return ptr;

	vec3i offset (payload->position * vec3i(MAP_CHUNK_SIZE_X,
										  MAP_CHUNK_SIZE_Y,
										  MAP_CHUNK_SIZE_Z));
	
	for (int i = 0; i < MAP_CHUNK_SIZE_X + 1; i++)  		//x axis
		for (int j = 0; j < MAP_CHUNK_SIZE_Y + 1; j++)		//y axis
			for (int k = 0; k < MAP_CHUNK_SIZE_Z + 1; k++) 	//z axis
				payload->chunk->data(i, j, k) = density(vec3i(offset.x + i,
												 offset.y + j,
												 offset.z + k));
	
	return ptr;
}


/* ChunkTriangulator */

ChunkTriangulator::ChunkTriangulator ()
	: tbb::filter (tbb::filter::parallel) {
}

void* ChunkTriangulator::operator()  (void* ptr) {
	Payload* payload = (Payload*)ptr;
	std::vector<vec3f> positions;
	std::vector<vec3f> normals;
	std::vector<uint> elements;

	// run marching cube algorithm
	//std::cout << "triangulating " << chunk->position << std::endl;
	marching_cube(payload->chunk->data, positions, normals, elements);

	if (positions.size() == 0 || elements.size() == 0) {
		delete payload;
		return 0;
	}

	// convert vec3f normals to vec3s
	std::vector<vec3s> normals_short (normals.size());
	for (uint i = 0; i < normals.size(); i++)
		normals_short[i] = normals[i] * 32767;

	payload->vertices_count = positions.size();
	payload->elements_count = elements.size();

	// create the resource block
	payload->block = create_geometry_data(
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

ChunkUploader::ChunkUploader (Map* map, const H3DNode parent)
	: tbb::thread_bound_filter (tbb::filter::serial_out_of_order),
	  map (map),
	  parent (parent) {
}

void* ChunkUploader::operator() (void* ptr) {
	if (ptr == 0) return 0;
	
	Payload* payload = (Payload*)ptr;
	std::stringstream name ("chunk");
	name << payload->position << rand();

	//std::cout << "uploading " << chunk->position << std::endl;

	H3DRes geometry = h3dAddResource(
								payload->block->type,
								name.str().c_str(), 0
					  		);
	if (geometry) h3dLoadResource(
							geometry,
							payload->block->data,
							payload->block->size
				  		);

	H3DNode node = h3dAddModelNode(parent, name.str().c_str(), geometry);
	
	h3dSetNodeTransform(
				node,
				payload->position.x * MAP_CHUNK_SIZE_X,
				payload->position.y * MAP_CHUNK_SIZE_Y,
				payload->position.z * MAP_CHUNK_SIZE_Z,
				0, 0, 0, 1, 1, 1
			);

	H3DRes material = h3dAddResource(
								H3DResTypes::Material,
								"materials/mine.material.xml", 0
							);
	h3dAddMeshNode(
			node,
			"DynGeoMesh",
			material,
			0,
			payload->elements_count,
			0,
			payload->vertices_count - 1
		);

	h3dutLoadResourcesFromDisk(".");

	// delete the buffered Chunk if different and assign the new one
	Chunk* chunk = map->buffer(payload->position);
	if (chunk != 0) { // if a chunk is bufferised
		h3dRemoveNode(chunk->node); // always remove its node
		if (chunk != payload->chunk) delete chunk; // delete if different
	}

	payload->chunk->node = node;
	map->buffer(payload->position) = payload->chunk; // save the chunk into the buffer

	delete[] payload->block; // TODO check leak
	delete payload;

	// finally, replace in the buffer
	//H3DNode old = buffer[chunk->position.x][chunk->position.y][chunk->position.z];
	//if (old != 0) h3dRemoveNode(old); // can be removed after initialization
	//buffer[chunk->position.x][chunk->position.y][chunk->position.z] = chunk->node;

	return 0;
}