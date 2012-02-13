#ifndef MC_HH
#define MC_HH

#include <vector>

#include <global.hh>
#include <util/math.hh>

typedef float (*density_f)(const vec3f);

void marching_cube (const vec3i offset, const vec3i size,
					std::vector<vec3f>& positions, std::vector<vec3s>& normals, std::vector<uint>& triangles);

inline H3DNode generate_chunk (H3DNode parent) {
	std::vector<vec3f> positions;
	std::vector<vec3s> normals;
	std::vector<uint> elements;

	marching_cube(vec3i(-20, -20, -20), vec3i(40, 40, 40), positions, normals, elements);

	std::cout << positions.size() << " vertices and " << elements.size() / 3 << " triangles generated"<< std::endl;

	H3DRes geometry = h3dutCreateGeometryRes(
							"geoRes",
							positions.size(),
							elements.size(),
							(float*)positions.data(),
							(uint*)elements.data(),
							(short*)normals.data(),
							0, 0, 0, 0
					  );
	H3DNode chunk = h3dAddModelNode(parent, "DynGeoModelNode", geometry);
	H3DRes material = h3dAddResource(H3DResTypes::Material, "materials/mine.material.xml", 0);

	h3dAddMeshNode(chunk, "DynGeoMesh", material, 0, elements.size(), 0, positions.size() - 1);
	h3dutLoadResourcesFromDisk(".");

	return chunk;
}

#endif