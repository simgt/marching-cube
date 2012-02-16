#include <global.hh>
#include <util/math.hh>
#include <map/map.hh>

#include <glfw.h>
#include <Horde3D/Horde3D.h>
#include <Horde3DUtils/Horde3DUtils.h>

#include <string>


#define WIN_W 800
#define WIN_H 600

#define CAMERA_T_SPEED 10.0f
#define CAMERA_R_SPEED 0.1f // angular speed (degrees)

#define PICK_RAY_LENGTH 10.0f 

// time
double delay ();

// camera

struct {
	H3DNode node;
	vec3f position;
	vec3f orientation;
} camera = {0, vec3f(0, 5, 0), vec3f(0)};

// events

tbb::concurrent_bounded_queue<vec3i> chunks_queue;

void keyboard_listener (int key, int state) {
	if (state == GLFW_RELEASE)
		return;
	
	switch (key) {
		case 'R':
			h3dSetOption(H3DOptions::WireframeMode, !h3dGetOption(H3DOptions::WireframeMode));
			h3dSetOption(H3DOptions::DebugViewMode, false);
			break;
		case 'F':
			h3dSetOption(H3DOptions::WireframeMode, false);
			h3dSetOption(H3DOptions::DebugViewMode, !h3dGetOption(H3DOptions::DebugViewMode));
			break;
		case GLFW_KEY_SPACE:
			chunks_queue.push(floor(camera.position / Map::chunk_size));
			std::cout << "Pushed " << floor(camera.position / Map::chunk_size) << std::endl;
			break;
	}
}

void mouse_position_listener (int mx, int my) {
	camera.orientation.x = - CAMERA_R_SPEED * my; // rotation autour de l'axe x (donc verticale)
	camera.orientation.y = - fmod(CAMERA_R_SPEED * mx, 360); // rotation autour de l'axe y (donc horizontale)
}

void mouse_button_listener (int button, int status) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && status == GLFW_PRESS) {
		vec3f p, d; // position and direction of the picking ray
		
		h3dutPickRay(camera.node, 0.5, 0.5, &p.x, &p.y, &p.z, &d.x, &d.y, &d.z); // get the picking ray of the center of the screen
		d.length(PICK_RAY_LENGTH); // set ray length to maximum reachable by player
		
		if (h3dCastRay(H3DRootNode, p.x, p.y, p.z, d.x, d.y, d.z, 1) > 0) { // cast the ray, stop to the 1st collision
			H3DNode node;
			assert(h3dGetCastRayResult(0, &node, 0, (float*)&p)); // recover node and intersection point
			outlog(node);
			
			// OPTIM check if the intersection point is not available directly in model-space in H3D
			const mat4f* m;
			h3dGetNodeTransMats(node, 0, (const float**)&m); // both matrix representation are in column major mode (internal H3D and mat)
			mat4f inv = mat4f::inverse(*m); // world to model matrix
			p = inv * p; // inverse the matrix and apply it to the position to recover the model-space position
			outlog(p);
		}
	}
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

	glfwSetKeyCallback(keyboard_listener);
	glfwSetMousePosCallback(mouse_position_listener);
	glfwSetMouseButtonCallback(mouse_button_listener);

	// Initialize engine
	h3dInit();

	// Add pipeline resource
	H3DRes pipeRes = h3dAddResource(H3DResTypes::Pipeline, "pipelines/mine.pipeline.xml", 0);
	// Add model resource
	//H3DRes sphere_scene = h3dAddResource(H3DResTypes::SceneGraph, "models/sphere/sphere.scene.xml", 0);
	// Font texture
	//H3DRes font_tex = h3dAddResource(H3DResTypes::Material, "overlays/font.material.xml", 0);
	//H3DRes panel_material = h3dAddResource(H3DResTypes::Material, "overlays/panel.material.xml", 0);
	// Load added resources
	h3dutLoadResourcesFromDisk("."); // important!

	// Add model to scene
	H3DNode world = h3dAddGroupNode(H3DRootNode, "world");
	
	//H3DNode sphere = h3dAddNodes(terrain, sphere_scene);
	//h3dSetNodeTransform(sphere, 0, 0, 0, 0, 0, 0, 5, 5, 5);


	// Add light source
	H3DNode light = h3dAddLightNode(H3DRootNode, "Light1", 0, "LIGHTING", "SHADOWMAP");
	h3dSetNodeTransform(light, 0, 6, 10, -10, 0, 0, 1, 1, 1);
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
	
	//H3DRes panel_material2 = h3dAddResource(H3DResTypes::Material, "overlays/panel.material.xml", 0);
	
	// MAP
	std::thread* map_worker = Map::launch_worker(world, &chunks_queue);
	
	// MAIN LOOP
	
	while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED)) {
		// Increase animation time
	    double t = delay();

		Map::update(floor(camera.position / Map::chunk_size), chunks_queue);

		// HUD
		//h3dutShowText("0.01a", 0.01, 0.01, 0.03f, 1, 1, 1, font_tex);
		//h3dutShowFrameStats(font_tex, panel_material2, H3DUTMaxStatMode);

		// inputs
		if (glfwGetKey('E')) {
			h3dSetNodeTransform(light, camera.position.x, camera.position.y, camera.position.z, camera.orientation.x, camera.orientation.y, camera.orientation.z, 1, 1, 1);
		}

		if (glfwGetKey('W')) { // forward
			camera.position.x += -sinf(radian(camera.orientation.y)) * cosf(-radian(camera.orientation.x)) * CAMERA_T_SPEED * t;
			camera.position.y += -sinf(-radian(camera.orientation.x)) * CAMERA_T_SPEED * t;
			camera.position.z += -cosf(radian(camera.orientation.y)) * cosf(-radian(camera.orientation.x)) * CAMERA_T_SPEED * t;
		}

		if (glfwGetKey('S')) { // backward
			camera.position.x += sinf(radian(camera.orientation.y)) * cosf(-radian(camera.orientation.x)) * CAMERA_T_SPEED * t;
			camera.position.y += sinf(-radian(camera.orientation.x)) * CAMERA_T_SPEED * t;
			camera.position.z += cosf(radian(camera.orientation.y)) * cosf(-radian(camera.orientation.x)) * CAMERA_T_SPEED * t;
		}

		if (glfwGetKey('A')) { // left
			camera.position.x += -sinf(radian(camera.orientation.y + 90)) * CAMERA_T_SPEED * t;
			camera.position.z += -cosf(radian(camera.orientation.y + 90)) * CAMERA_T_SPEED * t;
		}

		if (glfwGetKey('D')) { // right
			camera.position.x += sinf(radian(camera.orientation.y + 90)) * CAMERA_T_SPEED * t;
			camera.position.z += cosf(radian(camera.orientation.y + 90)) * CAMERA_T_SPEED * t;
		}
		
		h3dSetNodeTransform(camera.node, camera.position.x, camera.position.y, camera.position.z, camera.orientation.x, camera.orientation.y, 0, 1, 1, 1);

	    // Render scene
	    h3dRender(camera.node);

	    // Finish rendering of frame
	    h3dFinalizeFrame();
		h3dClearOverlays();
		glfwSwapBuffers();
	}
	
	h3dutDumpMessages();
	h3dRelease();
   
	glfwCloseWindow();
	glfwTerminate();
}

inline double delay () {
	static double last = 0;
	double current = glfwGetTime();
	double ellapsed = current - last;
	
	last = current;
	if (ellapsed < 0.03)
		glfwSleep(0.03 - ellapsed);
	
	return (glfwGetKey('P')) ? 0 : ellapsed;
}