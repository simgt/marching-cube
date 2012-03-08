#include "map.hh"

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

void worker_task (Map* const map) {
	while (true) { // TODO replace true by a lock wait ?
		Vec3i p; // chunk to process
		map->chunk_queue.pop(p); // TODO replace by try_pop of a priority_queue

		// VOLUME
		/* compute the involved volume Blocks and
		 * generate them if required */
		// OPTIM replace by a parallel_for
		const Vec3i beg = floor(p.cast<float>() * MAP_CHUNK_SIZE / MAP_BLOCK_SIZE);
		const Vec3i end = floor((p.cast<float>() + Vec3f(1, 1, 1)) * MAP_CHUNK_SIZE / MAP_BLOCK_SIZE);

		for (Vec3i i = beg; i[0] <= end[0]; i[0]++)
			for (i[1] = beg[1]; i[1] <= end[1]; i[1]++)
				for (i[2] = beg[2]; i[2] <= end[2]; i[2]++) {
					// proceed Blocks (load or generate) if needed
					Volume::accessor acc;
					if (map->volume.insert(acc, i)) // insert a new entry if not present
						generate(acc->second, i);
				}

		// SURFACE extraction

		GeometryPayload payload;
		if (!triangulate(payload, map->volume, p)) continue;

		map->geometry_queue.push(payload);
	}
};

void Map::update (const Vec3f& cp) {
	static Vec3i middle(1 << 15, 1 << 15, 1 << 15); // TODO FIX THIS SHIT !!
	Vec3i p = floor(cp, MAP_CHUNK_SIZE) / MAP_CHUNK_SIZE;

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

	chunk_queue.push(floor(p, MAP_CHUNK_SIZE) / MAP_CHUNK_SIZE);
}