#include "map.hh"
#include "util.hh"
#include <util/algorithm.hh>

#include <tbb/pipeline.h>

#include <iostream>
#include <sstream>
#include <algorithm>

void worker_task (Map* const map);

Map::Map (const H3DNode parent)
	: parent (parent) {
	//std::memset(buffer.data(), 0, sizeof(buffer));

	std::thread th (worker_task, this);
	worker = th;
}

static uchar density (const vec3i p) {
	return p.y <= 5 ? 127 : -128;
}

void worker_task (Map* const map) {
	while (true) { // TODO replace true by a lock wait ?
		vec3i p; // chunk to process
		map->chunk_queue.pop(p); // TODO replace by try_pop of a priority_queue

		// VOLUME
		/* compute the involved volume blocks and
		 * generate them if required */
		// OPTIM replace by a parallel_for
		const vec3i beg = floor((vec3f)p * MAP_CHUNK_SIZE / MAP_BLOCK_SIZE);
		const vec3i end = floor((vec3f)(p + 1) * MAP_CHUNK_SIZE / MAP_BLOCK_SIZE);
		std::cout << p << " : " << beg << " -> " << end << std::endl;

		for (vec3i i = beg; i.x <= end.x; i.x++)
			for (i.y = beg.y; i.y <= end.y; i.y++)
				for (i.z = beg.z; i.z <= end.z; i.z++) {
					// proceed blocks (load or generate) if needed
					block_table::accessor acc;
					if (map->volume.insert(acc, i)) { // insert a new entry if not present
						//std::cout << "inserting " << i << std::endl;
						// tips : ac->second is a reference block

						// procedural generation
						for (vec3i k (0); k.x < MAP_BLOCK_SIZE; k.x++)  		//x axis
							for (k.y = 0; k.y < MAP_BLOCK_SIZE; k.y++)		//y axis
								for (k.z = 0; k.z < MAP_BLOCK_SIZE; k.z++) 	//z axis
									acc->second(k).density = density(i * MAP_BLOCK_SIZE + k);
					
						acc.release();
					}
				}

		// SURFACE
		/* call marching cube and enqueue the generated chunk
		 * to be uploaded to the GPU by the main thread */
		std::vector<vec3f> positions;
		std::vector<vec3f> normals;
		std::vector<uint> elements;

		// run marching cube algorithm
		const_volume_sampler sampler (map->volume);
		marching_cube(sampler, p * MAP_CHUNK_SIZE, positions, normals, elements);
		sampler.release();

		if (positions.size() == 0 || elements.size() == 0) continue;

		std::vector<vec3s> normals_short (normals.size());
		for (uint i = 0; i < normals.size(); i++)
			normals_short[i] = normals[i] * 32767;

		GeometryPayload payload = {
			p,
			positions.size(),
			elements.size(),
			create_geometry_data(
				positions.size(),
				elements.size(),
				(float*)positions.data(),
				(uint*)elements.data(),
				(short*)normals_short.data(),
				0, 0, 0, 0
			)
		};

		map->geometry_queue.push(payload);
	}
};

void Map::update (const vec3f& cp) {
	static vec3i middle(1 << 15); // TODO FIX THIS SHIT !!
	vec3i p = floor(cp, (float)MAP_CHUNK_SIZE) / MAP_CHUNK_SIZE;

	if (middle != p) {
		chunk_queue.push(p);
		middle = p;
	}

	// upload geometry
	GeometryPayload payload;
	while (geometry_queue.try_pop(payload)) {
		std::cout << "hash(" << payload.position << ") = " << tbb_hasher(payload.position) << std::endl;
		std::cout << "surface.count(" << payload.position << ") = " << surface.count(payload.position) << std::endl;
		Chunk& chunk = surface[payload.position];

		std::stringstream name;
		name << "chunk" << payload.position;

		std::cout << "uploading " << payload.position << std::endl;

		// allocate a new geometry if no previous one
		if (chunk.geometry == 0) {
			chunk.geometry = h3dAddResource(
									payload.resource->type,
									name.str().c_str(), 0
						  		);
			std::cout << "  creating geometry" << std::endl;
		}
		else {
			h3dUnloadResource(chunk.geometry);
			std::cout << "  unloading geometry" << std::endl;
		}

		std::cout << "  loading geometry" << std::endl;
		h3dLoadResource(
			chunk.geometry,
			payload.resource->data,
			payload.resource->size
		);

		std::cout << "  " << payload.vertices_count << " vertices and "
				  << payload.elements_count << " elements ("
				  << payload.resource->size << " total size)" << std::endl;

		// create a node if there is no previous one (only the geometry needs to change)
		if (chunk.node == 0) {
			std::cout << "  creating node" << std::endl;

			chunk.node = h3dAddModelNode(
								parent,
								name.str().c_str(),
								chunk.geometry
						  );
		
			h3dSetNodeTransform(
					chunk.node,
					payload.position.x * MAP_CHUNK_SIZE,
					payload.position.y * MAP_CHUNK_SIZE,
					payload.position.z * MAP_CHUNK_SIZE,
					0, 0, 0, 1, 1, 1
				);

			chunk.material = h3dAddResource(
									H3DResTypes::Material,
									"materials/mine.material.xml", 0
								);

			h3dutLoadResourcesFromDisk(".");
		}
		
		h3dRemoveNode(chunk.mesh);
		chunk.mesh = h3dAddMeshNode(
							chunk.node,
							name.str().c_str(),
							chunk.material,
							0,
							payload.elements_count,
							0,
							payload.vertices_count - 1
						);

		std::cout << std::endl;
	}
}

void Map::modify (const vec3i& cp, const vec3f& p, char value) {
	//std::cout << "modifying " << cp << std::endl;

	// payload creation
	/*Payload* payload = new Payload;
	payload->position = cp;
	payload->chunk = buffer(cp);

	// chunks modifications
	chunk_data_array* data = &buffer(cp)->data;
	vec3i pp = floor(p);
	
	for (int i = std::max(0, pp.x - 1); i <= std::min(MAP_CHUNK_SIZE_X - 1, pp.x + 1); i++)
		for (int j = std::max(0, pp.y - 1); j <= std::min(MAP_CHUNK_SIZE_Y - 1, pp.y + 1); j++)
			for (int k = std::max(0, pp.z - 1); k <= std::min(MAP_CHUNK_SIZE_Z - 1, pp.z + 1); k++)
				data->at(i, j, k) = std::max(-128, std::min(127, data->at(i, j, k) + value));

	// running partial pipeline on chunk
	triangulator(payload);
	uploader(payload);*/
}