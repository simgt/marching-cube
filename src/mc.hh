#ifndef MC_HH
#define MC_HH

#include <glfw.h>
#include <Horde3D/Horde3D.h>
#include <Horde3DUtils/Horde3DUtils.h>

#include <vector>
#include <sstream>
#include <global.hh>
#include <util/math.hh>
#include <util/h3d.hh>

#define CHUNK_SIZE 10

typedef float (*density_f)(const vec3f);

void marching_cube (const vec3i offset, const vec3i size,
					std::vector<vec3f>& positions, std::vector<vec3f>& normals, std::vector<uint>& triangles);

inline H3DNode generate_chunk (H3DNode parent, vec3i chunk) {
	std::stringstream name ("chunk");
	std::vector<vec3f> positions;
	std::vector<vec3f> normals;
	std::vector<uint> elements;

	name << chunk;

	chunk *= CHUNK_SIZE; // one chunk = a CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE cube

	// out of thread:
	H3DRes geometry = h3dAddResource(H3DResTypes::Geometry, name.str().c_str(), 0);

	// in thread:
	marching_cube(chunk, vec3i(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE), positions, normals, elements);

	// convert vec3f normals to vec3s
	std::vector<vec3s> normals_short (normals.size());
	for (uint i = 0; i < normals.size(); i++) {
		normals_short[i] = normals[i] * 32767;
		//assert(normals_short[i] != 0);
	}
	
	data_block* block = create_geometry_data(
		positions.size(),
		elements.size(),
		(float*)positions.data(),
		(uint*)elements.data(),
		(short*)normals_short.data(),
		0, 0, 0, 0
	);

	// out of thread:
	if (geometry && block) h3dLoadResource(geometry, block->data, block->size);
	delete[] (char*)block;

	/*H3DRes geometry = h3dutCreateGeometryRes(
							name.str().c_str(),
							positions.size(),
							elements.size(),
							(float*)positions.data(),
							(uint*)elements.data(),
							(short*)normals_short.data(),
							0, 0, 0, 0
					  );*/
	H3DNode model = h3dAddModelNode(parent, name.str().c_str(), geometry);
	H3DRes material = h3dAddResource(H3DResTypes::Material, "materials/mine.material.xml", 0);

	h3dAddMeshNode(model, "DynGeoMesh", material, 0, elements.size(), 0, positions.size() - 1);
	h3dutLoadResourcesFromDisk(".");

	h3dSetNodeTransform(model, chunk.x, chunk.y, chunk.z, 0, 0, 0, 1, 1, 1);

	return model;
}

#endif