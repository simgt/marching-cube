#include "glfw.h"
#include <Horde3D/Horde3D.h>
#include <Horde3DUtils/Horde3DUtils.h>

#include <cmath>

#define WIN_W 640
#define WIN_H 480

H3DNode model = 0, cam = 0;

inline double delay () {
	static double last = 0;
	double current = glfwGetTime();
	double ellapsed = current - last;
	
	last = current;
	if (ellapsed < 0.01)
		glfwSleep(0.01 - ellapsed);
	
	return (glfwGetKey('P')) ? 0 : ellapsed;
}

int main() {
	glfwInit();

	if (!glfwOpenWindow(WIN_W, WIN_H, 8, 8, 8, 8, 24, 8, GLFW_WINDOW)) {
		glfwTerminate();
		return 1;
	}

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
	h3dSetNodeParamF(light, H3DLight::RadiusF, 0, 30);
	h3dSetNodeParamF(light, H3DLight::FovF, 0, 90 );
	h3dSetNodeParamI(light, H3DLight::ShadowMapCountI, 1 );
	h3dSetNodeParamF(light, H3DLight::ShadowMapBiasF, 0, 0.01f );
	h3dSetNodeParamF(light, H3DLight::ColorF3, 0, 1.0f );
	h3dSetNodeParamF(light, H3DLight::ColorF3, 1, 0.8f );
	h3dSetNodeParamF(light, H3DLight::ColorF3, 2, 0.7f );
	h3dSetNodeParamF(light, H3DLight::ColorMultiplierF, 0, 1.0f );

	// Add camera
	cam = h3dAddCameraNode(H3DRootNode, "Camera", pipeRes);
	// Setup viewport and render target sizes
	h3dSetNodeParamI(cam, H3DCamera::ViewportXI, 0);
	h3dSetNodeParamI(cam, H3DCamera::ViewportYI, 0);
	h3dSetNodeParamI(cam, H3DCamera::ViewportWidthI, WIN_W);
	h3dSetNodeParamI(cam, H3DCamera::ViewportHeightI, WIN_H);
	h3dSetupCameraView(cam, 45.0f, (float)WIN_W / WIN_H, 0.1f, 2048.0f);

	h3dResizePipelineBuffers(pipeRes, WIN_W, WIN_H);
	
	while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED)) {
		// Increase animation time
	    double t = delay();
		float z;

		if (glfwGetKey('W'))
			z -= 10.0f * t;
		if (glfwGetKey('S'))
			z += 10.0f * t;
		
		h3dSetNodeTransform(cam, 0, 0, z, 0, 0, 0, 1, 1, 1 );

	    // Set new model position
	    /*h3dSetNodeTransform(model, t * 10, 0, 0,  // Translation
	                         0, 0, 0,              // Rotation
	                         1, 1, 1);            // Scale */

	    // Render scene
	    h3dRender(cam);

	    // Finish rendering of frame
	    h3dFinalizeFrame();
		h3dClearOverlays();
		glfwSwapBuffers();
	}
	
	h3dRelease();
   
	glfwCloseWindow();
	glfwTerminate();
}