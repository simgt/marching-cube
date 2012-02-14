/*
 * Made by Simon using Paul Bourke's tables
 * 
 * ISSUES:
 * memoization error on x.oy registers
 * 		offset: vec3i(-15, -15, -15)
 *		size: vec3i(30, 30, 30)
 *		density: x*x + 2*x + 17*y - z*z
 *
 * some triangles are generated with equal vertices
 *		problem is coming from the interpolation
 *
 */

#include "mc.hh"
#include "mc_table.hh"
#include <stdlib.h>

#define FLOAT_CMP 0.01

vec3f middle (unsigned char a, unsigned char b) {
	const vec3f cube[8] = {
		vec3f(0, 0, 0),
		vec3f(1, 0, 0),
		vec3f(1, 0, 1),
		vec3f(0, 0, 1),
		vec3f(0, 1, 0),
		vec3f(1, 1, 0),
		vec3f(1, 1, 1),
		vec3f(0, 1, 1)
	};
	
	return (cube[a] + cube[b]) / 2;
}

#define ISOLEVEL 0
vec3f linear (unsigned char a, float va, unsigned char b, float vb) {
	const vec3f cube[8] = {
		vec3f(0, 0, 0),
		vec3f(1, 0, 0),
		vec3f(1, 0, 1),
		vec3f(0, 0, 1),
		vec3f(0, 1, 0),
		vec3f(1, 1, 0),
		vec3f(1, 1, 1),
		vec3f(0, 1, 1)
	};
	
	assert(a != b);
	return cube[a] + (cube[b] - cube[a]) / (vb - va) * (ISOLEVEL - va);
}

// memo

float density (const vec3f p) {
	/*x -= p.x;
	y -= p.y;
	z -= p.z;*/
	//return p.x * p.x + 2 * p.x + 17 * p.y - p.z * p.z; // TODO: take sphere position into account
	//return p.length() - 5;
	return p.y;
}

// TODO: remove
struct edge_plane {
	int* oy; // vertical edges
	int* oz; // horizontal edges
};

// marching cube

void marching_cube (const vec3i offset, const vec3i size, // input
					std::vector<vec3f>& positions, std::vector<vec3f>& normals, std::vector<uint>& triangles) { // output
	const uchar edg[12][2] = {
		/*  0 */ {0, 1},
		/*  1 */ {1, 2},
		/*  2 */ {2, 3},
		/*  3 */ {3, 0},
		/*  4 */ {4, 5},
		/*  5 */ {5, 6},
		/*  6 */ {6, 7},
		/*  7 */ {7, 4},
		/*  8 */ {0, 4},
		/*  9 */ {1, 5},
		/* 10 */ {2, 6},
		/* 11 */ {3, 7}
	};
	
	uint disc = 0; // discarded triangles
	
	// temp
	float grid[size.x + 1][size.y + 1][size.z + 1];
	for (int i = 0; i < size.x + 1; i++)  		 //x axis
		for (int j = 0; j < size.y + 1; j++)		 //y axis
			for (int k = 0; k < size.z + 1; k++) //z axis
				grid[i][j][k] = density(vec3f(offset.x + i, offset.y + j, offset.z + k));

	// X-axis memoization (YZ plane)
	edge_plane memo_x = {
		(int*)alloca((size.y + 1) * size.z * sizeof(int)),
		(int*)alloca(size.y * (size.z + 1) * sizeof(int))
	};

	std::memset(memo_x.oy, -1, (size.y + 1) * size.z * sizeof(int));
	std::memset(memo_x.oz, -1, size.y * (size.z + 1) * sizeof(int));
	
	// X-AXIS
	for (int i = 0; i < size.x; i++) { 		//x axis
		// X-axis memoization (YZ plane)
		edge_plane next_x = {
			(int*)alloca((size.y + 1) * size.z * sizeof(int)),
			(int*)alloca(size.y * (size.z + 1) * sizeof(int))
		};

		std::memset(next_x.oy, -1, (size.y + 1) * size.z * sizeof(int));
		std::memset(next_x.oz, -1, size.y * (size.z + 1) * sizeof(int));
		
		// Y-axis memoization: indexes of the previous z-line
		int memo_y[3 * size.y + 1]; // TODO: check if not 3 * Y + 1 ???
		std::memset(memo_y, -1, (3 * size.y + 1) * sizeof(int));

		// Y-AXIS
		for (int j = 0; j < size.y; j++) {
			// memoization: indexes of the previous z-line
			int next_y[3 * size.y + 1];
			std::memset(next_y, -1, (3 * size.y + 1) * sizeof(int));

			//z axis
			for (int k = 0; k < size.z; k++) {
				float val[8] = { // fetch the value of the eight vertices of the cube
					grid[i    ][j    ][k    ],
					grid[i + 1][j    ][k    ],
					grid[i + 1][j    ][k + 1],
					grid[i    ][j    ][k + 1],
					grid[i    ][j + 1][k    ],
					grid[i + 1][j + 1][k    ],
					grid[i + 1][j + 1][k + 1],
					grid[i    ][j + 1][k + 1] 
				};
				
				// get the index representing the cube's vertices configuration
				uchar index = 0;
				for (int n = 0; n < 8; n++)
					if (val[n] <= ISOLEVEL) index |= (1 << n); // set nth bit to 1

				//check if the cube is completely inside or outside the volume
    			if (edge_table[index] == 0) continue; // || edge_table[index] ??

				// retrieve indexes in the vertices array of the previously built vertices from the memoization register
				int memo_cube[12] = {
					/*  0 */ memo_y[3 * k],
					/*  1 */ next_x.oz[j * size.z + k], //
					/*  2 */ memo_y[3 * k + 3],
					/*  3 */ memo_x.oz[j * size.z + k],
					/*  4 */ next_y[3 * k],
					/*  5 */ -1,
					/*  6 */ -1,
					/*  7 */ memo_x.oz[(j + 1) * size.z + k],
					/*  8 */ memo_x.oy[j * (size.z - 1) + k], // BUG
					/*  9 */ next_x.oy[j * (size.z - 1) + k], // BUG
					/* 10 */ -1,
					/* 11 */ memo_x.oy[j * (size.z - 1) + k + 1] // BUG
				};

				// get the origin corner of the cube
				vec3f origin (i, j, k);

				// build the triangles using tri_table
				for (int n = 0; tri_table[index][n] != -1; n += 3) {
					uint v[3]; // 3 vertices of the contructed triangle, used for normals calculation
					
					// add the 3 vertices to the triangles array (create vertices if required)
					for (int m = 0; m < 3; m++) { // browse triangle's vertices
						int e = tri_table[index][n + m]; // retrieve the edge's cube-index

						// check if the vertex has already been created
						// create it and save it to the register if not
						if (memo_cube[e] == -1) { // not memoized
							vec3f position = origin + linear(edg[e][0], val[edg[e][0]], edg[e][1], val[edg[e][1]]);
							
							// check if the interpolation has not already produced a vertex at this position
							/*for (int i = 0; i < 12; i++)
								if (memo_cube[i] != -1 && (position >= (positions[i] - 0.01)) && (position <= (positions[i] + 0.01))) {
									memo_cube[e] = memo_cube[i];
									break;
								}*/
							for (uint i = 0; i < positions.size(); i++)
								if ((position >= (positions[i] - FLOAT_CMP)) && (position <= (positions[i] + FLOAT_CMP))) {
									memo_cube[e] = i;
									break;
								}
							
							if (memo_cube[e] == -1) {
								// construct the triangle's vertex and save it to the cube register
								memo_cube[e] = positions.size();
								positions.push_back(position);
								normals.push_back(0);
								//normals.push_back(vec3s(rand() % 255, rand() % 255, rand() % 255));
							}
						}
				
						// add the vertex index to the element array
						v[m] = memo_cube[e];
					}
					
					// add the normalized face normal to the vertices normals
					// TODO fix the algorithm to remove those quick-fixes					
					if (v[0] == v[1] || v[1] == v[2] || v[0] == v[2]) {
						disc++;
						continue;
					}
					
					triangles.push_back(v[0]);
					triangles.push_back(v[1]);
					triangles.push_back(v[2]);
					
					// face normal computing and adding to the vertices
					vec3f fn = cross(positions[v[1]] - positions[v[0]], positions[v[2]] - positions[v[0]]); // compute the triangle's normal
					fn.normalize();
					for (int m = 0; m < 3; m++)
						normals[v[m]] += fn;
				}
				
				// save current cube into memoization registers
				/*  0 */ memo_y[3 * k] = memo_cube[0];
				/*  1 */ next_x.oz[j * size.z + k] = memo_cube[1];
				/*  2 */ memo_y[3 * k + 3] = memo_cube[2];
				/*  3 */ memo_x.oz[j * size.z + k] = memo_cube[3];
				/*  4 */ next_y[3 * k] = memo_cube[4];
				/*  5 */ next_x.oz[(j + 1) * size.z + k] = next_y[3 * k + 1] = memo_cube[5]; //
				/*  6 */ next_y[3 * k + 3] = memo_cube[6];
				/*  7 */ next_y[3 * k + 2] = memo_x.oz[(j + 1) * size.z + k] = memo_cube[7]; //
				/*  8 */ memo_x.oy[j * (size.z - 1) + k] = memo_cube[8];
				/*  9 */ next_x.oy[j * (size.z - 1) + k] = memo_cube[9];
				/* 10 */ next_x.oy[j * (size.z - 1) + k + 1] = memo_cube[10];
				/* 11 */ memo_x.oy[j * (size.z - 1) + k + 1] = memo_cube[11];
			} // end of Z-loop

			std::memcpy(memo_y, next_y, (3 * size.y + 1) * sizeof(int));
		} // end of Y-loop

		std::memcpy(memo_x.oy, next_x.oy, (size.y + 1) * size.z * sizeof(int));
		std::memcpy(memo_x.oz, next_x.oz, size.y * (size.z + 1) * sizeof(int));
	} // end of X-loop
	
	// set normals length to 1.0
	for (uint i = 0; i < normals.size(); i++)
		if (normals[i] != vec3f(0)) normals[i].normalize(); // TODO
	
	//std::cout << positions.size() << " vertices and " << triangles.size() / 3 << " triangles generated (" << disc << " discarded)"<< std::endl;
	
	assert(positions.size() == normals.size());
}