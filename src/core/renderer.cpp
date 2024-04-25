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
#include <GLFW/stb_image_write.h>


void Renderer::init() {

	//scene.initScene(); // EDIT: vieles aus init scene gehört hierher - eigentlich alles außer camera und volume vao
	initFrameVAO();
	initFBOs();
	initTransferFunctions();
	initShaders();
	// in renderer 
}
void Renderer::setVolumeData(Texture3D volumeData)
{
	scene.setVolumeData(volumeData);
}
void Renderer::setVolumePosition(glm::mat4 modelMatrix)
{
	scene.setVolumePosition(modelMatrix);
}
void Renderer::setCameraPosition(glm::mat4 viewMatrix)
{
	scene.setCameraPosition(viewMatrix);
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
	initVolumeFBO();
	framebuffers[VOLUME] = Framebuffer(width, height, 5);
	framebuffers[ACCUMULATION] = Framebuffer(width, height);
	framebuffers[LAST_FRAME] = Framebuffer(width, height);
	framebuffers[CONTROL_VARIATE] = Framebuffer(width, height);
}
void Renderer::initVolumeFBO() {

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
		shaders[RM].setInt("numberOfColorPoints", transferFunctions[COLOR].size() / 2);
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
		shaders[MC].setFloat("brightness", 50.0);

		shaders[MC].setInt("numberOfColorPoints", transferFunctions[COLOR].size() / 2);
		shaders[MC].setListVec2("transfer_function_color", transferFunctions[COLOR]);
		shaders[MC].setInt("numberOfDensityPoints", transferFunctions[TRANSPARENCY].size() / 2);
		shaders[MC].setListVec2("transfer_function_density", transferFunctions[TRANSPARENCY]);

		shaders[MC].setInt("lastNumberOfColorPoints", controlVariate.transferFunctionColor.size() / 2);
		shaders[MC].setListVec2("transfer_function_control_color", controlVariate.transferFunctionColor);
		shaders[MC].setInt("lastNumberOfDensityPoints", controlVariate.transferFunctionDensity.size() / 2);
		shaders[MC].setListVec2("transfer_function_control_density", controlVariate.transferFunctionDensity);
		shaders[MC].setBool("useControlVariate", useControlVariate && isControlVariateSet);

		shaders[ACC].use();
		shaders[ACC].setInt("runs", accumulatedFrames);
		shaders[ACC].setInt("samplesPerRun", samplesPerFrame);

		glBindTexture(GL_TEXTURE_2D, framebuffers[VOLUME].getTexture()[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, framebuffers[CONTROL_VARIATE].getTexture()[0]);
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
	method = MONTE_CARLO;
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

	glEnable(GL_DEPTH_TEST);
	//if (method != DEBUG) {
	GLenum* drawBuffers = new GLenum[5];
	for (int i = 0; i < 5; i++) {
		drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	//GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT6 };
	glDrawBuffers(5, drawBuffers);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDrawBuffers(1, drawBuffers);
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
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	shaders[ACC].use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffers[VOLUME].getTexture()[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, framebuffers[LAST_FRAME].getTexture()[0]);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[LAST_FRAME].getID());
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	shaders[COPY].use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffers[ACCUMULATION].getTexture()[0]);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	//glBindTexture(GL_TEXTURE_2D, framebuffers[VOLUME].getTexture()[4]);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
	transferFunctions[type] = transferFunction;
}

void Renderer::setStepSize(float stepSize)
{
	this->stepSize = stepSize;
}

void Renderer::setControlVariate()
{
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(FrameVAO);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers[CONTROL_VARIATE].getID());
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	shaders[COPY].use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffers[ACCUMULATION].getTexture()[0]);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	controlVariate.transferFunctionColor = transferFunctions[COLOR];
	controlVariate.transferFunctionDensity = transferFunctions[TRANSPARENCY];
	controlVariate.volumePosition = scene.getVolumePosition();
	controlVariate.runs = accumulatedFrames;
	isControlVariateSet = true;
}

void Renderer::deleteControlVariate()
{
	isControlVariateSet = false;
}

void Renderer::setUseControlVariate(bool useIt)
{
	useControlVariate = useIt;
}

void Renderer::resetAccumulatedFrames()
{
	accumulatedFrames = -1;
}

glm::mat4 Renderer::getCameraPosition()
{
	return scene.getCameraView();
}

glm::mat4 Renderer::getVolumePosition()
{
	return scene.getVolumePosition();
}
void Renderer::saveDataToFile(std::string folder)
{
	JSONSerializer serializer;
	serializer.saveData(
		transferFunctions[COLOR], transferFunctions[TRANSPARENCY],
		controlVariate.transferFunctionColor, controlVariate.transferFunctionDensity,
		accumulatedFrames,
		controlVariate.runs,
		scene.getVolumePosition(),
		std::string().append(folder).append("data.json"));

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[ACCUMULATION].getID());
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	saveImageToFile("F_ACC", folder);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[VOLUME].getID());
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	saveImageToFile("F", folder);
	glReadBuffer(GL_COLOR_ATTACHMENT0+1);
	saveImageToFile("F_STAR", folder);
	glReadBuffer(GL_COLOR_ATTACHMENT0+2);
	saveImageToFile("F1", folder);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + 3);
	saveImageToFile("H1", folder);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + 4);
	saveImageToFile("W_F_STAR", folder);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[CONTROL_VARIATE].getID());
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	saveImageToFile("CV", folder);
}
void Renderer::saveImageToFile(std::string fileName, std::string folder)
{
	int* buffer = new int[width * height * 3];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	stbi_flip_vertically_on_write(1);
	std::string fullPath;
	fullPath.append(folder).append(fileName).append(".png");
	stbi_write_png(fullPath.c_str(), width, height, 3, buffer, width * 3);
}




