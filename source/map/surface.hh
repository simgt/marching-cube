#ifndef SURFACE_HH
#define SURFACE_HH

#include <map>

struct GeometryPayload {
	Vec3i position; // TODO Vec3i ?
	size_t vertices_count;
	size_t elements_count;
	ResourceBlock* resource;
};

struct Chunk {
	H3DNode node;
	H3DRes geometry;
	H3DNode mesh;
	H3DRes material;

	Chunk ()
		: node (0),
		  geometry (0),
		  mesh (0),
		  material (0) {
	};
};

typedef std::map<Vec3i, Chunk, Vec3iCompare> Surface;

#endif