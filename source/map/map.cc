#include "map.hh"
#include "util.hh"
#include <util/algorithm.hh>

#include <tbb/pipeline.h>

#include <iostream>
#include <algorithm>

void worker_task (Map* map);

Map::Map (const H3DNode parent)
	: allocator (this),
	  generator (),
	  triangulator (),
	  uploader (this, parent),
	  pipeline () {

	std::memset(buffer.data(), 0, sizeof(buffer));

	pipeline.add_filter(allocator);
	pipeline.add_filter(generator);
	pipeline.add_filter(triangulator);
	pipeline.add_filter(uploader);

	std::thread th (worker_task, this);
	worker = th;
}

void worker_task (Map* map) {	
	vec3i middle;

	while (true) {
		map->queue.pop(middle);
		map->allocator.set_middle(middle);
		map->pipeline.run(MAP_VIEW_AREA);
	}
}

void Map::update (const vec3f& camera_position) {
	static vec3i middle(1 << 15); // TODO FIX THIS SHIT !!
	vec3i p = chunk_coord(camera_position);

	for (int i = 0; i < MAP_CHUNKS_PER_ROUND && uploader.try_process_item() != tbb::thread_bound_filter::end_of_stream; i++)
		; // upload at most 5 chunks per round

	if (middle != p) {
		//std::cout << middle << " --> " << p << std::endl;		
		queue.push(p);
		middle = p;
	}
}

void Map::modify (const vec3i& cp, const vec3f& p, char value) {
	//std::cout << "modifying " << cp << std::endl;

	// payload creation
	Payload* payload = new Payload;
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
	uploader(payload);
}