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
		/* compute the involved volume Blocks and
		 * generate them if required */
		// OPTIM replace by a parallel_for
		const vec3i beg = floor((vec3f)p * MAP_CHUNK_SIZE / MAP_BLOCK_SIZE);
		const vec3i end = floor((vec3f)(p + 1) * MAP_CHUNK_SIZE / MAP_BLOCK_SIZE);
		std::cout << p << " : " << beg << " -> " << end << std::endl;

		for (vec3i i = beg; i.x <= end.x; i.x++)
			for (i.y = beg.y; i.y <= end.y; i.y++)
				for (i.z = beg.z; i.z <= end.z; i.z++) {
					// proceed Blocks (load or generate) if needed
					Volume::accessor acc;
					if (map->volume.insert(acc, i)) { // insert a new entry if not present
						//std::cout << "inserting " << i << std::endl;
						// tips : ac->second is a reference Block

						// procedural generation
						for (vec3i k (0); k.x < MAP_BLOCK_SIZE; k.x++)  		//x axis
							for (k.y = 0; k.y < MAP_BLOCK_SIZE; k.y++)		//y axis
								for (k.z = 0; k.z < MAP_BLOCK_SIZE; k.z++) 	//z axis
									acc->second(k).density = density(i * MAP_BLOCK_SIZE + k);
					
						acc.release();
					}
				}

		// SURFACE extraction

		GeometryPayload payload;
		if (!triangulate(payload, map->volume, p)) continue;

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
		upload(surface, parent, payload);
	}
}

void Map::modify (const vec3f& p, char value) {
	//std::cout << "modifying " << p << std::endl;
	// chunks modifications
	vec3i pp = floor(p);

	Volume::Sampler sampler (volume);

	for (int i = pp.x - 1; i <= pp.x + 1; i++)
		for (int j = pp.y - 1; j <= pp.y + 1; j++)
			for (int k = pp.z - 1; k <= pp.z + 1; k++)
				sampler(i, j, k).density = std::max(-128, std::min(127, sampler(i, j, k).density + value));

	sampler.release();

	chunk_queue.push(floor(p, (float)MAP_CHUNK_SIZE) / MAP_CHUNK_SIZE);
}