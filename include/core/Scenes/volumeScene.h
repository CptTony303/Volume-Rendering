#ifndef VOLUME_SCENE_H
#define VOLUME_SCENE_H


#include <core/shader.h>
#include <core/framebuffer.h>
#include <glm/glm.hpp>

class VolumeScene{
public:
	VolumeScene();
	void initScene();
	void renderScene();
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
	unsigned int VAO_VOLUME, VAO_FRAME; //VAOs
	Shader monteCarloShader, rayMarchShader, accumulationShader, copyShader; //Shaders
	Framebuffer FBO_VOLUME, FBO_ACCUMULATION, FBO_LAST_FRAME; //Framebuffers

	int accumulatedFrames = 0; //number of accumulated frames for monte carlo

	//GUI variables:
	int gui_methode = 0;
	float gui_stepSize = 0.01f;
	int gui_samplesPerFrame = 25;
};

#endif