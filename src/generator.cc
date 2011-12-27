#include "global.hh"

#include <util/math.hh>

H3DNode generate_chunk (H3DNode parent) {
	vec3f positions[] = {
		vec3f(0, 0, 0),
		vec3f(1, 0, 0),
		vec3f(0, 0, 1),
		vec3f(1, 0, 1)
	};

	uint indexes[] = {
		0, 2, 1,
		1, 2, 3
	};

	short normals[] = {
	 0, 1, 0,
	 0, 1, 0,
	 0, 1, 0,
	 0, 1, 0
	};
	
	short tangents[] = {
	 1, 0, 0,
	 1, 0, 0,
	 1, 0, 0,
	 1, 0, 0
	};
	
	short bitangents[] = {
	 0, 0, 1,
	 0, 0, 1,
	 0, 0, 1,
	 0, 0, 1
	};

	float texture1coords[] = {
	 4 * 0.0625, 1 - 0 * 0.0625,
	 5 * 0.0625, 1 - 0 * 0.0625,
	 4 * 0.0625, 1 - 1 * 0.0625,
	 5 * 0.0625, 1 - 1 * 0.0625
	};

	H3DRes geometry = h3dutCreateGeometryRes("geoRes", 4, 6, (float*)positions, indexes, normals, 0, 0, texture1coords, 0);
	H3DNode chunk = h3dAddModelNode(parent, "DynGeoModelNode", geometry);
	H3DRes material = h3dAddResource(H3DResTypes::Material, "terrain/terrain.material.xml", 0);

	h3dAddMeshNode(chunk, "DynGeoMesh", material, 0, 6, 0, 3);
	h3dutLoadResourcesFromDisk(".");
	
	return chunk;
}