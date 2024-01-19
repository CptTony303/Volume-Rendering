#ifndef VOLUME_SCENE_H
#define VOLUME_SCENE_H


#include <core/shader.h>
#include <core/framebuffer.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <core/camera.h>
#include <core/GUI/volumeGUI.h>

class VolumeScene{
public:
	VolumeScene() {};
	VolumeScene(int width, int height);
	void initScene();
	void renderScene();
	void processInput(GLFWwindow* window, float delta);
	//callback functions
	void scroll_callback(double xoffset, double yoffset);
	void framebuffer_size_callback(int width, int height);
private:
	void initTransformationMatrices();
	void initVolumeVAO();
	void initFrameVAO();
	void initVAOs();
	void initFBOs();
	void initShaders();
	void initVolumeShaders();
	void initAccumulationShader();
	void initCopyShader();
	void renderGUI();
	void renderVolume();
	void accumulateFrames();
	void updateShaderValues();

	//private member variables
	glm::mat4 model, view, projection;
	Camera camera;
	unsigned int VAO_VOLUME, VAO_FRAME; //VAOs
	Shader monteCarloShader, rayMarchShader, accumulationShader, copyShader; //Shaders
	Framebuffer FBO_VOLUME, FBO_ACCUMULATION, FBO_LAST_FRAME; //Framebuffers

	int accumulatedFrames = 0; //number of accumulated frames for monte carlo
	VolumeGUI gui;
};

#endif