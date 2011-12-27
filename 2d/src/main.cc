#include "global.hh"
#include "object.hh"
#include "util/math.hh"

#include <glfw.h>
#include <iostream>

double delay ();

int main (int, char**) {
    glfwInit();
	if (!glfwOpenWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 8, 8, 8, 8, 24, 8, GLFW_WINDOW)) {
		glfwTerminate();
		return 1;
	}

	glfwEnable(GLFW_AUTO_POLL_EVENTS);
	//glfwSetWindowSizeCallback(window_resize);
    
	glClearDepth(1.0f);
	glClearColor(0, 0, 0, 0);

	// reshape
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	mat4f p_matrix = mat4f::translation(-1, -1, 0) * mat4f::scale(2.0 / WINDOW_WIDTH, 2.0 / WINDOW_HEIGHT, 1);
	
	// resources
	Program program ("shd/vertex.glsl", "shd/fragment.glsl");
	glUseProgram(program.handle);
	glUniformMatrix4fv(program.uniforms.p_matrix, 1, GL_FALSE, p_matrix.raw());
	
	Mesh triangle (3, 6);
	triangle.vertex_data[0] = vec3f(0, 0, 0);
	triangle.vertex_data[1] = vec3f(200, 0, 0);
	triangle.vertex_data[2] = vec3f(200, 200, 0);
	triangle.element_data[0] = 0; triangle.element_data[1] = 1;
	triangle.element_data[2] = 1; triangle.element_data[3] = 2;
	triangle.element_data[4] = 2; triangle.element_data[5] = 0;
	triangle.init(program);
	
	std::cout << p_matrix << std::endl;
	
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

inline double delay () {
	static double last = 0;
	double current = glfwGetTime();
	double ellapsed = current - last;
	
	last = current;
	if (ellapsed < 0.01)
		glfwSleep(0.01 - ellapsed);
	
	return (glfwGetKey(KEY_PAUSE)) ? 0 : ellapsed;
}