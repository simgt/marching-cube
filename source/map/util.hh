#ifndef MAP_UTIL_HH
#define MAP_UTIL_HH

#include "map.hh"

inline vec3i chunk_coord (const vec3f& p) {
	return floor(p / vec3i(MAP_CHUNK_SIZE_X,
						   MAP_CHUNK_SIZE_Y,
						   MAP_CHUNK_SIZE_Z));
}

#endif