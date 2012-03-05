#include "map.hh"

#include <sstream>


static uchar density (const Vec3i p) {
	return p[1] <= 5 ? 127 : -128;
}

void generate (Block& block, const Vec3i coords) {
	// procedural generation
	for (Vec3i k (0, 0, 0); k[0] < MAP_BLOCK_SIZE; k[0]++)  		//x axis
		for (k[1] = 0; k[1] < MAP_BLOCK_SIZE; k[1]++)		//y axis
			for (k[2] = 0; k[2] < MAP_BLOCK_SIZE; k[2]++) 	//z axis
				block(k).density = density(coords * MAP_BLOCK_SIZE + k);
}

/* Triangulator
 *
 * Triangulate the volume associated to 'sampler': generate vertices
 * and triangles and allocate a geometry blob ready to be uploaded
 * to H3D
 *
 * 'sampler' volume sampler to process
 * 'offset' coordinates of the MAP_CHUNK_SIZE square to triangulate
 */

bool triangulate (GeometryPayload& payload, const Volume& volume, const Vec3i& coords) {
	std::vector<Vec3f> positions;
	std::vector<Vec3f> normals;
	std::vector<uint> elements;

	// run marching cube algorithm
	Volume::ConstSampler sampler (volume);
	marching_cube(sampler, coords * MAP_CHUNK_SIZE, positions, normals, elements);
	sampler.release();

	if (positions.size() == 0 || elements.size() == 0) return false;

	std::vector<Vec3s> normals_short (normals.size());
	for (uint i = 0; i < normals.size(); i++)
		normals_short[i] = (normals[i] * 32767).cast<short>();

	payload.position = coords;
	payload.vertices_count = positions.size();
	payload.elements_count = elements.size();
	payload.resource = create_geometry_data(
			positions.size(),
			elements.size(),
			(float*)positions.data(),
			(uint*)elements.data(),
			(short*)normals_short.data(),
			0, 0, 0, 0
		);

	return true;
}

/* Uploader
 *
 * CAUTION: must be executed in the main thread ! */

void upload (Surface& surface, const H3DNode parent, const GeometryPayload& payload) {
	Chunk& chunk = surface[payload.position];

	std::stringstream name;
	name << "chunk" << payload.position;

	// allocate a new geometry if no previous one
	if (chunk.geometry == 0)
		chunk.geometry = h3dAddResource(
								payload.resource->type,
								name.str().c_str(), 0
					  		);
	else
		h3dUnloadResource(chunk.geometry);

	h3dLoadResource(
		chunk.geometry,
		payload.resource->data,
		payload.resource->size
	);

	// create a node if there is no previous one (only the geometry needs to change)
	if (chunk.node == 0) {
		chunk.node = h3dAddModelNode(
							parent,
							name.str().c_str(),
							chunk.geometry
					  );
	
		h3dSetNodeTransform(
				chunk.node,
				payload.position[0] * MAP_CHUNK_SIZE,
				payload.position[1] * MAP_CHUNK_SIZE,
				payload.position[2] * MAP_CHUNK_SIZE,
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
}