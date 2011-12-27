#include "object.hh"

#include <iostream>

GLuint load_shader (GLenum type, const char* filepath);
GLuint load_program (const char* vertex_file, const char* fragment_file);

void Program::init (const char* vertex_file, const char* fragment_file) {
	handle = load_program(vertex_file, fragment_file);
	uniforms.p_matrix = glGetUniformLocation(handle, "p_matrix");
	attributes.position = glGetAttribLocation(handle, "position");
}

// util

inline void show_info_log (GLuint object, void (*glGet__iv)(GLuint, GLenum, GLint*), void (*glGet__InfoLog)(GLuint, GLsizei, GLsizei*, GLchar*)) {
    GLint log_length;
    char* log;

    glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
    log = (char*)malloc(log_length);
    glGet__InfoLog(object, log_length, NULL, log);
	std::cerr << log << std::endl;
    free(log);
}

static char* file_to_string (const char *filename, GLint *length) {
    FILE *f = fopen(filename, "r");
    char* buffer;

    if (!f) {
        fprintf(stderr, "Unable to open %s for reading\n", filename);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    *length = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = (char*)malloc(*length+1);
    *length = fread(buffer, 1, *length, f);
    fclose(f);
    ((char*)buffer)[*length] = '\0';

    return buffer;
}

GLuint load_shader(GLenum type, const char* filepath) {
    GLint length;
    GLchar* source = file_to_string(filepath, &length);
    GLuint handle;
    GLint success;

    if (!source)
        return 0;

	handle = glCreateShader(type);
    glShaderSource(handle, 1, (const GLchar**)&source, &length);
    glCompileShader(handle);

	free(source);

    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if (!success) { // TODO throw exception
        std::cerr << "Failed to compile " << filepath << ":" << std::endl;
        show_info_log(handle, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(handle);
        return 0;
    }

    return handle;
}

GLuint load_program (const char* vertex_file, const char* fragment_file) {
	// program building
	GLuint vertex_shader = load_shader(GL_VERTEX_SHADER, vertex_file);
	GLuint fragment_shader = load_shader(GL_FRAGMENT_SHADER, fragment_file);
	
	GLint success;
    GLuint handle = glCreateProgram();
    glAttachShader(handle, vertex_shader);
    glAttachShader(handle, fragment_shader);
    glLinkProgram(handle);

    glGetProgramiv(handle, GL_LINK_STATUS, &success);
    if (!success) {
        std::cerr << "Failed to link shader program:" << std::endl;
        show_info_log(handle, glGetProgramiv, glGetProgramInfoLog);
        glDeleteProgram(handle);
        return 0;
    }

    return handle;
};