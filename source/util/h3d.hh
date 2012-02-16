#ifndef UTIL_HORDE3D_HH
#define UTIL_HORDE3D_HH

#include <global.hh>
#include <Horde3D.h>

struct ResourceBlock {
	H3DResTypes::List type;
	uint size;
	char data[0];
};

ResourceBlock* create_geometry_data (
					int numVertices, int numTriangleIndices,
					float *posData, 
					uint32 *indexData, 
					short *normalData,
					short *tangentData,
					short *bitangentData,
					float *texData1, float *texData2);

#endif