#include "global.hh"

#include <glfw.h>

double delay () {
	static double last = 0;
	double current = glfwGetTime();
	double ellapsed = current - last;
	
	last = current;
	if (ellapsed < 0.01)
		glfwSleep(0.01 - ellapsed);
	
	return (glfwGetKey(KEY_PAUSE)) ? 0 : ellapsed;
}