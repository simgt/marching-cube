#include "object.hh"

Mesh::Mesh (size_t vertex_count, size_t element_count)
 	: vertex_count (vertex_count),
 	  element_count (element_count),
 	  element_mode (GL_LINES) {
	vertex_data = new vec3f[vertex_count];
	element_data = new GLuint[element_count];
}
	
Mesh::~Mesh () {
	delete[] vertex_data;
	delete[] element_data;
}

void Mesh::init (const Program& program) {
	// generate opengl objects
	glGenVertexArraysAPPLE(1, &vertex_array); // TODO remove Apple
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &element_buffer);

	// bind objects
	glBindVertexArrayAPPLE(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

	// upload vertex buffer data
	glBufferData(
		GL_ARRAY_BUFFER,
		vertex_count * sizeof(vec3f),
		vertex_data,
		GL_STATIC_DRAW
	);

	// upload element_buffer
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		element_count * sizeof(uint),
		element_data,
		GL_STREAM_DRAW
	);

	// attributes mapping
	glEnableVertexAttribArray(program.attributes.position);
    glVertexAttribPointer(
        program.attributes.position,
        3, GL_FLOAT, GL_FALSE,
		sizeof(vec3f),
        0
    );

	glBindVertexArrayAPPLE(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::upload () {
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferSubData(
		GL_ARRAY_BUFFER,
		0,
		vertex_count * sizeof(vec3f),
		vertex_data
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::draw () {
	glBindVertexArrayAPPLE(vertex_array); // TODO remove APPLE
	glDrawElements(
	    element_mode,
	    element_count,
	    GL_UNSIGNED_INT,
	    (void*)0
	);
	glBindVertexArrayAPPLE(0); // TODO remove APPLE
}