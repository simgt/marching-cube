#include "glfw.h"
#include <Horde3D/Horde3D.h>
#include <Horde3DUtils/Horde3DUtils.h>

int main() {
   glfwInit();

   if (!glfwOpenWindow(640,480,8,8,8,8,24,8,GLFW_WINDOW)) {
       glfwTerminate();
       return 1;
   }

   h3dInit();
   h3dRelease();
   glfwCloseWindow();
   glfwTerminate();
 }