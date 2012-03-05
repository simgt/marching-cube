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

static uchar density (const Vec3i p) {
	return p[1] <= 5 ? 127 : -128;
}

void worker_task (Map* const map) {
	while (true) { // TODO replace true by a lock wait ?
		Vec3i p; // chunk to process
		map->chunk_queue.pop((vec3i&)p); // TODO replace by try_pop of a priority_queue

		// VOLUME
		/* compute the involved volume Blocks and
		 * generate them if required */
		// OPTIM replace by a parallel_for
		const Vec3i beg = floor(p.cast<float>() * MAP_CHUNK_SIZE / MAP_BLOCK_SIZE);
		const Vec3i end = floor((p.cast<float>() + Vec3f(1, 1, 1)) * MAP_CHUNK_SIZE / MAP_BLOCK_SIZE);
		std::cout << p << " : " << beg << " -> " << end << std::endl;

		for (Vec3i i = beg; i[0] <= end[0]; i[0]++)
			for (i[1] = beg[1]; i[1] <= end[1]; i[1]++)
				for (i[2] = beg[2]; i[2] <= end[2]; i[2]++) {
					// proceed Blocks (load or generate) if needed
					Volume::accessor acc;
					if (map->volume.insert(acc, (vec3i&)i)) { // insert a new entry if not present
						//std::cout << "inserting " << i << std::endl;
						// tips : ac->second is a reference Block

						// procedural generation
						for (Vec3i k (0, 0, 0); k[0] < MAP_BLOCK_SIZE; k[0]++)  		//x axis
							for (k[1] = 0; k[1] < MAP_BLOCK_SIZE; k[1]++)		//y axis
								for (k[2] = 0; k[2] < MAP_BLOCK_SIZE; k[2]++) 	//z axis
									acc->second((vec3i&)k).density = density(i * MAP_BLOCK_SIZE + k);
					
						acc.release();
					}
				}

		// SURFACE extraction

		GeometryPayload payload;
		if (!triangulate(payload, map->volume, (vec3i&)p)) continue;

		map->geometry_queue.push(payload);
	}
};

void Map::update (const Vec3f& cp) {
	static Vec3i middle(1 << 15, 1 << 15, 1 << 15); // TODO FIX THIS SHIT !!
	Vec3i p = floor(cp, MAP_CHUNK_SIZE) / MAP_CHUNK_SIZE;

	if (middle != p) {
		chunk_queue.push((vec3i&)p);
		middle = p;
	}

	// upload geometry
	GeometryPayload payload;
	while (geometry_queue.try_pop(payload)) {
		upload(surface, parent, payload);
	}
}

void Map::modify (const Vec3f& p, char value) {
	//std::cout << "modifying " << p << std::endl;
	// chunks modifications
	Vec3i pp = floor(p);

	Volume::Sampler sampler (volume);

	for (int i = pp[0] - 1; i <= pp[0] + 1; i++)
		for (int j = pp[1] - 1; j <= pp[1] + 1; j++)
			for (int k = pp[2] - 1; k <= pp[2] + 1; k++)
				sampler(i, j, k).density = std::max(-128, std::min(127, sampler(i, j, k).density + value));

	sampler.release();

	Vec3i tmp = floor(p, MAP_CHUNK_SIZE) / MAP_CHUNK_SIZE;
	chunk_queue.push((vec3i&)tmp);
}