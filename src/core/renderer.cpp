#include <core/renderer.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <core/shader.h>
#include <stb_image.h>
#include <core/texture.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <core/texture3D.h>
#include <imgui.h>
#include <core/Scenes/volumeScene.h>
#include <filesystem>


void Renderer::init() {

	//scene.initScene(); // EDIT: vieles aus init scene gehört hierher - eigentlich alles außer camera und volume vao
	initFrameVAO();
	initFBOs();
	initTransferFunctions();
	initShaders();
	// in renderer 
}
void Renderer::setVolumePosition(glm::mat4 modelMatrix)
{
	scene.setVolumePosition(modelMatrix);
}
void Renderer::initFrameVAO() // soll in renderer
{

	float frameVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	// Framebuffer VAO
	glGenVertexArrays(1, &FrameVAO);
	glBindVertexArray(FrameVAO);

	unsigned int vbo_frame;
	glGenBuffers(1, &vbo_frame);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_frame);
	glBufferData(GL_ARRAY_BUFFER, sizeof(frameVertices), &frameVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}
void Renderer::initFBOs() // belongs to renderer
{
	framebuffers.resize(4);
	framebuffers[VOLUME] = Framebuffer(width, height);
	framebuffers[ACCUMULATION] = Framebuffer(width, height);
	framebuffers[LAST_FRAME] = Framebuffer(width, height);
	framebuffers[CONTROL_VARIATE] = Framebuffer(width, height);
}
void Renderer::initTransferFunctions()
{
	transferFunctions.resize(2);
	transferFunctions[COLOR] = { 0.f, 0.f, 1.f, 1.f };
	transferFunctions[TRANSPARENCY] = { 0.f, 0.f, 1.f, 1.f };
}
void Renderer::initShaders() // belongs to renderer
{
	shaders.resize(4);
	initVolumeShaders();
	initAccumulationShader();
	initCopyShader();
	//initDebugShaders();
	updateShaderValues();
}
void Renderer::initVolumeShaders() // in renderer
{
	//dynamic volume path and size
	//Texture3D volumeData("C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/Assets/foot_256x256x256_uint8.raw", GL_TEXTURE3, glm::vec3(256, 256, 256));

	const char* vertPathMonteCarlo = "./shaders/monte_carlo.vert";
	const char* fragPathMonteCarlo = "./shaders/monte_carlo.frag";

	shaders[MC] = Shader(vertPathMonteCarlo, fragPathMonteCarlo);
	shaders[MC].use();

	shaders[MC].setInt("volume", 0);
	shaders[MC].setInt("convergedFrame", 1);

	const char* vertPathRayMarch = "./shaders/volume.vert";
	const char* fragPathRayMarch = "./shaders/ray_marching.frag";

	shaders[RM] = Shader(vertPathRayMarch, fragPathRayMarch);
	shaders[RM].use();

	shaders[RM].setInt("volume", 0);
}
void Renderer::initAccumulationShader() // in renderer
{
	const char* vertPath = "./shaders/copy.vert";
	const char* fragPath = "./shaders/accumulation.frag";

	shaders[ACC] = Shader(vertPath, fragPath);
	shaders[ACC].use();

	shaders[ACC].setInt("screenTexture", 0);
	shaders[ACC].setInt("screenCopy", 1);
}

void Renderer::initCopyShader() // in renderer
{
	const char* vertPath = "./shaders/copy.vert";
	const char* fragPath = "./shaders/copy.frag";

	shaders[COPY] = Shader(vertPath, fragPath);
	shaders[COPY].use();

	shaders[COPY].setInt("screenTexture", 0);
}

void Renderer::initDebugShaders() // in renderer
{
	////transfer function shader
	//const char* vertPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/copy.vert";
	//const char* fragPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/transfer_function.frag";

	//transferFunctionShader = Shader(vertPath, fragPath);
	//transferFunctionShader.use();

	////volume debug shader
	//const char* vertPathDebug = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/volume.vert";
	//const char* fragPathDebug = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/volume_debug.frag";

	//volumeDebugShader = Shader(vertPathDebug, fragPathDebug);
	//volumeDebugShader.use();
}
void Renderer::updateShaderValues() // in renderer
{
	// hier am besten switch anweisung mit shader als enum
	// dann nur notwendige shader updaten
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, scene.getVolumeData());

	switch (method) {
	case RAY_MARCHING:
		shaders[RM].use();
		shaders[RM].setMat4("model", scene.getVolumePosition());
		shaders[RM].setMat4("view", scene.getCameraView());
		shaders[RM].setMat4("projection", scene.getCameraProjection());
		shaders[RM].setFloat("stepSize", stepSize);
		shaders[RM].setInt("numberOfColorPoints", transferFunctions[COLOR].size()/2);
		shaders[RM].setListVec2("transfer_function_color", transferFunctions[COLOR]);
		shaders[RM].setInt("numberOfDensityPoints", transferFunctions[TRANSPARENCY].size() / 2);
		shaders[RM].setListVec2("transfer_function_density", transferFunctions[TRANSPARENCY]);
		break;
	case MONTE_CARLO:
		shaders[MC].use();
		shaders[MC].setMat4("model", scene.getVolumePosition());
		shaders[MC].setMat4("view", scene.getCameraView());
		shaders[MC].setMat4("projection", scene.getCameraProjection());

		shaders[MC].setFloat("screenSize_x", (float)width);
		shaders[MC].setFloat("screenSize_y", (float)height);

		shaders[MC].setInt("samplesPerFrame", samplesPerFrame);
		shaders[MC].setFloat("randomizer", (float)glfwGetTime());
		shaders[MC].setFloat("brightness", 5.0);

		shaders[MC].setInt("numberOfColorPoints", transferFunctions[COLOR].size() / 2);
		shaders[MC].setListVec2("transfer_function_color", transferFunctions[COLOR]);
		shaders[MC].setInt("numberOfDensityPoints", transferFunctions[TRANSPARENCY].size() / 2);
		shaders[MC].setListVec2("transfer_function_density", transferFunctions[TRANSPARENCY]);

		shaders[MC].setInt("lastNumberOfColorPoints", controlVariate.transferFunctionColor.size() / 2);
		shaders[MC].setListVec2("transfer_function_control_color", controlVariate.transferFunctionColor);
		shaders[MC].setInt("lastNumberOfDensityPoints", controlVariate.transferFunctionDensity.size() / 2);
		shaders[MC].setListVec2("transfer_function_control_density", controlVariate.transferFunctionDensity);
		shaders[MC].setBool("useControlVariate", useControlVariate&&isControlVariateSet);

		shaders[ACC].use();
		shaders[ACC].setInt("runs", accumulatedFrames);
		shaders[ACC].setInt("samplesPerRun", samplesPerFrame);

		glBindTexture(GL_TEXTURE_2D, framebuffers[VOLUME].getTexture());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, framebuffers[CONTROL_VARIATE].getTexture());
		break;
		/*
	case DEBUG:
		transferFunctionShader.use();
		transferFunctionShader.setInt("numberOfColorPoints", gui.trans_func_points_color.size());
		transferFunctionShader.setListVec2("transfer_function_color", trans_func_points_color);
		transferFunctionShader.setInt("numberOfDensityPoints", gui.trans_func_points_density.size());
		transferFunctionShader.setListVec2("transfer_function_density", trans_func_points_density);

		volumeDebugShader.use();
		volumeDebugShader.setMat4("model", model);
		volumeDebugShader.setMat4("view", camera.getViewMatrix());
		volumeDebugShader.setMat4("projection", camera.getProjectionMatrix());
		volumeDebugShader.setFloat("randomizer", (float)glfwGetTime());
		volumeDebugShader.setInt("methode", gui.methode);
		break; 
		*/
	}
}


void Renderer::renderScene() {
	updateShaderValues(); // -> renderer; diese methode unbedingt verbessern, vllt extra methode pro shader
	renderVolume(); // definitive in den renderer
}
void Renderer::renderVolume() // in renderer
{
	glBindVertexArray(scene.getVolumeVAO());
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //draw on screen

	switch (method) {
	case RAY_MARCHING:
		accumulatedFrames = -1;
		shaders[RM].use();
		break;
	case MONTE_CARLO:
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[VOLUME].getID()); //draw into framebuffer
		shaders[MC].use();
		break;
		/*
	case DEBUG:
		accumulatedFrames = -1;
		shaders[(ShaderType)DEBUG].use();
		break;
	case DEBUG://transfer function
		accumulatedFrames = -1;
		glBindVertexArray(FrameVAO);
		transferFunctionShader.use();
		break;
		*/
	}
	glClearColor(0.f, 0.f, 0.f, 1.0f); //set clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	//if (method != DEBUG) {

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	/*
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	*/

	if (method == MONTE_CARLO) {
		accumulateFrames();
	}
}

void Renderer::accumulateFrames()
{


	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(FrameVAO);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[ACCUMULATION].getID());
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	shaders[ACC].use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffers[VOLUME].getTexture());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, framebuffers[LAST_FRAME].getTexture());

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[LAST_FRAME].getID());
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	shaders[COPY].use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffers[ACCUMULATION].getTexture());

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	accumulatedFrames++;
}

void Renderer::setRenderMethod(RenderMethods method)
{
	this->method = method;
}

void Renderer::setTransferFunction(std::vector <float> transferFunction, TransferFunctionType type)
{
	transferFunctions[type] = transferFunction; // Klappt evtl nicht wegen vs error
	/*
	trans_func_points_color.clear();
	trans_func_points_color.resize(gui.trans_func_points_color.size() * 2);
	for (int i = 0; i < gui.trans_func_points_color.size(); i++)
	{
		trans_func_points_color[i * 2] = gui.trans_func_points_color[i].x;
		trans_func_points_color[i * 2 + 1] = gui.trans_func_points_color[i].y;
	}
	*/
	/*
		for (int i = 0; i < trans_func_points_color.size(); i++)
		{
			last_trans_func_points_color.push_back(trans_func_points_color[i]);
		}
		if (trans_func_points_color.size() != gui.trans_func_points_color.size() * 2) {
			trans_func_points_color.clear();
			trans_func_points_color.shrink_to_fit();
			trans_func_points_color.resize(gui.trans_func_points_color.size() * 2);
		}
		for (int i = 0; i < gui.trans_func_points_color.size(); i++)
		{
			trans_func_points_color[i * 2] = gui.trans_func_points_color[i].x;
			trans_func_points_color[i * 2 + 1] = gui.trans_func_points_color[i].y;
		}
	std::vector <float> trans_func_points_density;
	for each (glm::vec2 p in gui.trans_func_points_density)
	{
		trans_func_points_density.push_back(p.x);
		trans_func_points_density.push_back(p.y);
	}
	*/
}

void Renderer::setStepSize(float stepSize)
{
	this->stepSize = stepSize;
}

void Renderer::setControlVariate()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers[CONTROL_VARIATE].getID());
	glBlitFramebuffer( 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	//float* buffer = new float[width * height * 3];
	//glReadPixels(0, 0, width, height, GL_RGB, GL_FLOAT, buffer);
	////for (int i = width * height; i < width * height * 3;i++) {
	////	std::cout << buffer[i] << " ";
	////}
	////std::cout << std::endl;
	//unsigned int texture;
	//glActiveTexture(GL_TEXTURE0);
	//glGenTextures(1, &texture);
	//glBindTexture(GL_TEXTURE_2D, texture);
	////get actual screen size
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, buffer);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//controlVariate.texture = texture;
	controlVariate.transferFunctionColor = transferFunctions[COLOR];
	controlVariate.transferFunctionDensity = transferFunctions[TRANSPARENCY];
	controlVariate.volumePosition = scene.getVolumePosition();
	isControlVariateSet = true;
}

void Renderer::setUseControlVariate(bool useIt)
{
	useControlVariate = useIt;
}

void Renderer::resetAccumulatedFrames()
{
	accumulatedFrames = -1;
}
