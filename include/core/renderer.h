#ifndef RENDERER_H
#define RENDERER_H


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <core/Scenes/volumeScene.h>
#include <core/shader.h>
#include <core/framebuffer.h>
#include <vector>
#include <core/texture3D.h>

class Renderer
{
public:
	enum TransferFunctionType {COLOR, TRANSPARENCY};
	enum RenderMethods {MONTE_CARLO, RAY_MARCHING};

	Renderer(int width, int height) : width(width),
		height(height),
		scene(VolumeScene(float(width) / float(height))),
		shaders(4),
		framebuffers(3),
		FrameVAO(0),
		method(MONTE_CARLO),
		accumulatedFrames(-1),
		stepSize(0.1),
		samplesPerFrame(10),
		transferFunctions(2),
		useControlVariate(false),
		isControlVariateSet(false){
	}
	void renderScene();
	void renderVolume();
	void init();

	void setVolumeData(Texture3D volumeData);
	void setVolumePosition(glm::mat4 modelMatrix);
	void setCameraPosition(glm::mat4 viewMatrix);
	void setCameraOptions();
	void setRenderMethod(RenderMethods method);
	void setTransferFunction(std::vector <float> transferFunction, TransferFunctionType type);

	void setStepSize(float stepSize);
	void setSampleSize();
	void setControlVariate();
	void deleteControlVariate();
	void setUseControlVariate(bool useIt);
	void resetAccumulatedFrames();

	glm::mat4 getCameraPosition();
	glm::mat4 getVolumePosition();

	void saveImageToFile(std::string fileName, std::string folder);

private:
	void initFrameVAO();
	void initFBOs();
	void initVolumeFBO();
	void initTransferFunctions();
	void initShaders();
	void initVolumeShaders();
	void initAccumulationShader();
	void initCopyShader();
	void initDebugShaders();
	void updateShaderValues();

	void accumulateFrames();

	struct ControlVariate {
		unsigned int texture;
		std::vector <float> transferFunctionColor;
		std::vector <float> transferFunctionDensity;
		glm::mat4 volumePosition;
	};

	int width, height;
	VolumeScene scene;
	enum FramebufferType {VOLUME, ACCUMULATION, LAST_FRAME, CONTROL_VARIATE};
	enum ShaderType {MC, RM, COPY, ACC};
	std::vector <Shader> shaders;
	std::vector <Framebuffer> framebuffers;
	unsigned int FrameVAO;
	RenderMethods method;
	int accumulatedFrames;
	float stepSize;
	int samplesPerFrame;
	std::vector <std::vector <float> > transferFunctions;
	bool useControlVariate;
	bool isControlVariateSet;
	ControlVariate controlVariate;
};

#endif // !RENDERER_H