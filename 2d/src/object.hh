#ifndef OBJECT_HH
#define OBJECT_HH

#include "global.hh"
#include "util/math.hh"

struct Mesh;
struct Program;

struct Mesh {
	GLuint vertex_array;
	
	vec3f* vertex_data;
	uint vertex_count;
	GLuint vertex_buffer;

	uint* element_data;
	uint element_count;
	GLenum element_mode;
	GLuint element_buffer;

	Mesh (size_t vertex_count, size_t element_count);
	~Mesh ();

	void init (const Program& program);
	void upload ();
	void draw ();
};

struct Program {
	GLuint handle;

	struct {
		GLuint p_matrix;
	} uniforms;

	struct {
		GLuint position;
	} attributes;

	void init (const char* vertex_file, const char* fragment_file);
};

#endif