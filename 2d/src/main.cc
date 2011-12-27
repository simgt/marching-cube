#include "global.hh"
#include <glfw.h>

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
 
	while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED)) {
		delay();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glfwSwapBuffers();
	}

	glfwCloseWindow();
	glfwTerminate();

    return 0;
}