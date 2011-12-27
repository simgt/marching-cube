#include "global.hh"
#include "object.hh"
#include "util/math.hh"

#include <glfw.h>
#include <iostream>

void resize (int width, int height);
double delay ();

mat4f p_matrix;
Program program;

int main (int, char**) {
    glfwInit();
	if (!glfwOpenWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 8, 8, 8, 8, 24, 8, GLFW_WINDOW)) {
		glfwTerminate();
		return 1;
	}
	
	program.init("shd/vertex.glsl", "shd/fragment.glsl");

	glfwEnable(GLFW_AUTO_POLL_EVENTS);
	glfwSetWindowSizeCallback(resize);
    
	glClearDepth(1.0f);
	glClearColor(0.2, 0.2, 0.2, 0);
	
	// resources
	
	Mesh triangle (3, 6);
	triangle.vertex_data[0] = vec3f(0, 0, 0);
	triangle.vertex_data[1] = vec3f(200, 0, 0);
	triangle.vertex_data[2] = vec3f(100, 200, 0);
	triangle.element_data[0] = 0; triangle.element_data[1] = 1;
	triangle.element_data[2] = 1; triangle.element_data[3] = 2;
	triangle.element_data[4] = 2; triangle.element_data[5] = 0;
	triangle.init(program);
	
	// loop

	while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED)) {
		delay();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(program.handle);
		triangle.draw();
		glUseProgram(0);
		
		glfwSwapBuffers();
	}

	glfwCloseWindow();
	glfwTerminate();

    return 0;
}

void resize (int width, int height) {
	glViewport(0, 0, width, height); // update viewport
	p_matrix = mat4f::translation(-1, -1, 0) * mat4f::scale(2.0 / width, 2.0 / height, 1) * mat4f::translation(1, 1, 0); // update projection matrix
	
	// upload projection matrix
	glUseProgram(program.handle);
	glUniformMatrix4fv(program.uniforms.p_matrix, 1, GL_FALSE, p_matrix.raw());
	glUseProgram(0);
}

inline double delay () {
	static double last = 0;
	double current = glfwGetTime();
	double ellapsed = current - last;
	
	last = current;
	if (ellapsed < 0.01)
		glfwSleep(0.01 - ellapsed);
	
	return (glfwGetKey(KEY_PAUSE)) ? 0 : ellapsed;
}