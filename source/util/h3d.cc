#include "h3d.hh"

#include <cstring>

ResourceBlock* create_geometry_data (
	int numVertices, int numTriangleIndices,
	float *posData, 
	uint32 *indexData, 
	short *normalData,
	short *tangentData,
	short *bitangentData,
	float *texData1, float *texData2 )
{
	if( numVertices == 0 || numTriangleIndices == 0 ) return 0;
				
	uint32 size =
		// General data
		4 +					// Horde Flag 
		sizeof( uint32 ) +	// Version 
		sizeof( uint32 ) +	// Joint Count ( will be always set to set to zero because this method does not create joints )
		sizeof( uint32 ) +	// number of streams 
		sizeof( uint32 ) +	// streamsize
		// vertex stream data
		sizeof( uint32 ) +	// stream id 
		sizeof( uint32 ) +	// stream element size 
		numVertices * sizeof( float ) * 3 + // vertices data
	
		// morph targets 
		sizeof( uint32 );	// number of morph targets ( will be always set to zero because this method does not create morph targets )

	if( normalData )
	{
		size += 
			// normal stream data
			sizeof( uint32 ) +	// stream id 
			sizeof( uint32 ) +	// stream element size 
			numVertices * sizeof( uint16 ) * 3; // normal data
	}

	if( tangentData && bitangentData )
	{
		size += 
			// normal stream data
			sizeof( uint32 ) +	// stream id 
			sizeof( uint32 ) +	// stream element size 
			numVertices * sizeof( uint16 ) * 3 + // tangent data
			// normal stream data
			sizeof( uint32 ) +	// stream id 
			sizeof( uint32 ) +	// stream element size 
			numVertices * sizeof( uint16 ) * 3; // bitangent data
	}

	int numTexSets = 0;
	if( texData1 ) ++numTexSets;
	if( texData2 ) ++numTexSets;

	for( int i = 0; i < numTexSets; ++i )
	{
		size += 
			// texture stream data
			sizeof( uint32 ) +	// stream id 
			sizeof( uint32 ) +	// stream element size 
		numVertices * sizeof( float ) * 2; // texture data
	}

	size += 
		// index stream data
		sizeof( uint32 ) +	// index count 
		numTriangleIndices * sizeof( uint32 ); // index data


	// Create resource data block
	ResourceBlock* block = (ResourceBlock*)new char[sizeof(ResourceBlock) + size];
	block->type = H3DResTypes::Geometry;
	block->size = size;

	char* pData = block->data;
	// Write Horde flag
	pData[0] = 'H'; pData[1] = '3'; pData[2] = 'D'; pData[3] = 'G'; pData += 4;
	// Set version to 5 
	*( (uint32 *)pData ) = 5; pData += sizeof( uint32 );
	// Set joint count (zero for this method)
	*( (uint32 *)pData ) = 0; pData += sizeof( uint32 );
	// Set number of streams
	*( (uint32 *)pData ) = 1 + numTexSets + ( normalData ? 1 : 0 ) + ((tangentData && bitangentData) ? 2 : 0); pData += sizeof( uint32 );
	// Write number of elements in each stream
	*( (uint32 *)pData ) = numVertices; pData += sizeof( uint32 );

	// Beginning of stream data

	// Vertex Stream ID
	*( (uint32 *)pData ) = 0; pData += sizeof( uint32 );
	// set vertex stream element size
	*( (uint32 *)pData ) = sizeof( float ) * 3; pData += sizeof( uint32 );
	// vertex data
	std::memcpy( (float*) pData, posData, numVertices * sizeof( float ) * 3 );
	pData += numVertices * sizeof( float ) * 3;

	if( normalData )
	{
		// Normals Stream ID
		*( (uint32 *)pData ) = 1; pData += sizeof( uint32 );
		// set normal stream element size
		*( (uint32 *)pData ) = sizeof( short ) * 3; pData += sizeof( uint32 );
		// normal data
		std::memcpy( (short*) pData, normalData, numVertices * sizeof( short ) * 3 );
		pData += numVertices * sizeof( short ) * 3;
	}

	if( tangentData && bitangentData )
	{
		// Tangent Stream ID
		*( (uint32 *)pData ) = 2; pData += sizeof( uint32 );
		// set tangent stream element size
		*( (uint32 *)pData ) = sizeof( short ) * 3; pData += sizeof( uint32 );
		// tangent data
		std::memcpy( (short*) pData, tangentData, numVertices * sizeof( short ) * 3 );
		pData += numVertices * sizeof( short ) * 3;
	
		// Bitangent Stream ID
		*( (uint32 *)pData ) = 3; pData += sizeof( uint32 );
		// set bitangent stream element size
		*( (uint32 *)pData ) = sizeof( short ) * 3; pData += sizeof( uint32 );
		// bitangent data
		std::memcpy( (short*) pData, bitangentData, numVertices * sizeof( short ) * 3 );
		pData += numVertices * sizeof( short ) * 3;
	}

	// texture coordinates stream
	if( texData1 )
	{
		*( (uint32 *)pData ) = 6; pData += sizeof( uint32 ); // Tex Set 1
		*( (uint32 *)pData ) = sizeof( float ) * 2; pData += sizeof( uint32 ); // stream element size
		std::memcpy( (float *)pData, texData1, sizeof( float ) * 2 * numVertices ); // stream data
		pData += sizeof( float ) * 2 * numVertices; 
	}
	if( texData2 )
	{
		*( (uint32 *)pData ) = 7; pData += sizeof( uint32 ); // Tex Set 2
		*( (uint32 *)pData ) = sizeof( float ) * 2; pData += sizeof( uint32 ); // stream element size
		std::memcpy( (float *)pData, texData2, sizeof( float ) * 2 * numVertices ); // stream data
		pData += sizeof( float ) * 2 * numVertices; 
	}

	// Set number of indices
	*( (uint32 *) pData ) = numTriangleIndices; pData += sizeof( uint32 );	
	
	// index data
	std::memcpy( pData, indexData, numTriangleIndices * sizeof( uint32 ) );
	pData += numTriangleIndices * sizeof( uint32 );				

	// Set number of morph targets to zero
	*( (uint32 *) pData ) = 0;	pData += sizeof( uint32 );

	return block;
}