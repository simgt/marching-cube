#include "glfw.h"
#include <Horde3D/Horde3D.h>
#include <Horde3DUtils/Horde3DUtils.h>

#include <util/math.hh>
#include <iostream>
#include <string>

#define WIN_W 800
#define WIN_H 600

#define CAMERA_X_SPEED 10.0f
#define CAMERA_Z_SPEED 10.0f
#define CAMERA_RX_SPEED 0.1f // angular speed (degrees)
#define CAMERA_RY_SPEED 0.1f

H3DNode model = 0;

// time
double delay ();

// camera

struct {
	H3DNode node;
	float x, y, z;
	float rx, ry;
} camera = { 0, 0, 0, 0, 0, 0 };


// events

void mouse_position_listener(int mx, int my) {
	camera.rx = - CAMERA_RX_SPEED * my; // rotation autour de l'axe x (donc verticale)
	camera.ry = - fmod(CAMERA_RY_SPEED * mx, 360); // rotation autour de l'axe y (donc horizontale)
}

// main

int main() {
	// init
	glfwInit();

	if (!glfwOpenWindow(WIN_W, WIN_H, 8, 8, 8, 8, 24, 8, GLFW_WINDOW)) {
		glfwTerminate();
		return 1;
	}
	
	glfwDisable(GLFW_MOUSE_CURSOR);

	glfwSetMousePosCallback(mouse_position_listener);

	// Initialize engine
	h3dInit();

	// Add pipeline resource
	H3DRes pipeRes = h3dAddResource(H3DResTypes::Pipeline, "pipelines/forward.pipeline.xml", 0);
	// Add model resource
	H3DRes modelRes = h3dAddResource(H3DResTypes::SceneGraph, "models/sphere/sphere.scene.xml", 0);
	// Load added resources
	h3dutLoadResourcesFromDisk("."); // important!

	// Add model to scene
	model = h3dAddNodes(H3DRootNode, modelRes);

	h3dSetNodeTransform(model, 0, 0, 0, 0, 0, 0, 5, 5, 5);

	// Add light source
	H3DNode light = h3dAddLightNode(H3DRootNode, "Light1", 0, "LIGHTING", "SHADOWMAP");
	h3dSetNodeTransform(light, 0, 10, 0, 0, 0, 0, 1, 1, 1);
	h3dSetNodeParamF(light, H3DLight::RadiusF, 0, 50);
	h3dSetNodeParamF(light, H3DLight::FovF, 0, 90 );
	h3dSetNodeParamI(light, H3DLight::ShadowMapCountI, 1 );
	h3dSetNodeParamF(light, H3DLight::ShadowMapBiasF, 0, 0.01f );
	h3dSetNodeParamF(light, H3DLight::ColorF3, 0, 1.0f );
	h3dSetNodeParamF(light, H3DLight::ColorF3, 1, 0.8f );
	h3dSetNodeParamF(light, H3DLight::ColorF3, 2, 0.7f );
	h3dSetNodeParamF(light, H3DLight::ColorMultiplierF, 0, 1.0f );

	// Add camera
	camera.node = h3dAddCameraNode(H3DRootNode, "Camera", pipeRes);
	// Setup viewport and render target sizes
	h3dSetNodeParamI(camera.node, H3DCamera::ViewportXI, 0);
	h3dSetNodeParamI(camera.node, H3DCamera::ViewportYI, 0);
	h3dSetNodeParamI(camera.node, H3DCamera::ViewportWidthI, WIN_W);
	h3dSetNodeParamI(camera.node, H3DCamera::ViewportHeightI, WIN_H);
	h3dSetupCameraView(camera.node, 45.0f, (float)WIN_W / WIN_H, 0.1f, 2048.0f);

	h3dResizePipelineBuffers(pipeRes, WIN_W, WIN_H);
	
	while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED)) {
		// Increase animation time
	    double t = delay();

		if (glfwGetKey('P')) {
			std::cout << "cos(ry) = " << cosf(radian(camera.ry)) << std::endl;
			std::cout << "sin(ry) = " << sinf(radian(camera.ry)) << std::endl;
		}

		if (glfwGetKey('W')) {
			camera.x -= sinf(radian(camera.ry)) * CAMERA_X_SPEED * t;
			camera.z -= cosf(radian(camera.ry)) * CAMERA_Z_SPEED * t;
		}
		if (glfwGetKey('S')) {
			camera.x += sinf(radian(camera.ry)) * CAMERA_X_SPEED * t;
			camera.z += cosf(radian(camera.ry)) * CAMERA_Z_SPEED * t;
		}
		if (glfwGetKey('A')) {
			camera.x -= sinf(radian(camera.ry - 90)) * CAMERA_X_SPEED * t;
			camera.z -= cosf(radian(camera.ry - 90)) * CAMERA_Z_SPEED * t;
		}
		if (glfwGetKey('D')) {
			camera.x += sinf(radian(camera.ry + 90)) * CAMERA_X_SPEED * t;
			camera.z += cosf(radian(camera.ry + 90)) * CAMERA_Z_SPEED * t;
		}
		
		h3dSetNodeTransform(camera.node, camera.x, 0, camera.z, camera.rx, camera.ry, 0, 1, 1, 1);

	    // Set new model position
	    /*h3dSetNodeTransform(model, t * 10, 0, 0,  // Translation
	                         0, 0, 0,              // Rotation
	                         1, 1, 1);            // Scale */

	    // Render scene
	    h3dRender(camera.node);

	    // Finish rendering of frame
	    h3dFinalizeFrame();
		h3dClearOverlays();
		glfwSwapBuffers();
	}
	
	h3dRelease();
   
	glfwCloseWindow();
	glfwTerminate();
}

inline double delay () {
	static double last = 0;
	double current = glfwGetTime();
	double ellapsed = current - last;
	
	last = current;
	if (ellapsed < 0.01)
		glfwSleep(0.01 - ellapsed);
	
	return (glfwGetKey('P')) ? 0 : ellapsed;
}