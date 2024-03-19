#include <tester\tester.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <filesystem>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../lib/glfw/deps/stb_image_write.h"


void Tester::generateTestImages() {

	runSingleTestcase({ 0.0,0.0,1.0,1.0 }, 1,
		Renderer::MONTE_CARLO, false,
		false, true, "x - 20s - no");
	runSingleTestcase({ 0.0,0.0,1.0,1.0 }, 6,
		Renderer::MONTE_CARLO, false,
		true, true, "x - 120s - no");
	runSingleTestcase({ 0.0,0.0,1.0,1.0 }, 1,
		Renderer::MONTE_CARLO, true,
		false, true, "x - 20s - yes");

	runSingleTestcase({ 0.0,0.1,1.0,0.9 }, 1,
		Renderer::MONTE_CARLO, false,
		false, true, "y - 20s - no");
	runSingleTestcase({ 0.0,0.1,1.0,0.9 }, 1,
		Renderer::MONTE_CARLO, true,
		false, true, "y - 20s - yes");
	runSingleTestcase({ 0.0,0.1,1.0,0.9 }, 6,
		Renderer::MONTE_CARLO, false,
		false, true, "y - 120s - no");

	runSingleTestcase({ 0.0,0.3,1.0,0.6 }, 1,
		Renderer::MONTE_CARLO, false,
		false, true, "z - 20s - no");
	runSingleTestcase({ 0.0,0.3,1.0,0.6 }, 1,
		Renderer::MONTE_CARLO, true,
		false, true, "z - 20s - yes");
	runSingleTestcase({ 0.0,0.3,1.0,0.6 }, 6,
		Renderer::MONTE_CARLO, false,
		false, true, "z - 120s - no");
}

void Tester::depricated() {

	//renderer.setVolumeData(file_path);
	renderer->setRenderMethod(Renderer::MONTE_CARLO);
	renderer->setStepSize(0.001f);
	//renderer.setCameraView(glm::mat4);
	//renderer.setCameraOptions();
	//renderer.setTransferFunctions(vector<float>);
	//renderer.setControlVariates(texture);

	for (int i = 0; i < 1; i++) {
		renderer->renderScene();
	}
	saveImage("5_samples_without_control_variate_multi_color");
	renderer->resetAccumulatedFrames();
	//renderer->setVolumePosition(glm::rotate(glm::mat4(1.f), glm::radians(90.f),glm::vec3(0,1,0)));
	for (int i = 0; i < 12; i++) {
		renderer->renderScene();
	}
	saveImage("60_samples_without_control_variate_multi_color");
	renderer->setControlVariate();
	renderer->resetAccumulatedFrames();
	renderer->setUseControlVariate(false);
	renderer->setTransferFunction({ 0.0,0.3,1.0,0.7 }, Renderer::COLOR);
	for (int i = 0; i < 1; i++) {
		renderer->renderScene();
	}
	saveImage("5_samples_without_control_variate_green");
	renderer->resetAccumulatedFrames();
	for (int i = 0; i < 12; i++) {
		renderer->renderScene();
	}
	saveImage("60_samples_without_control_variate_green");
	renderer->resetAccumulatedFrames();
	renderer->setUseControlVariate(true);
	for (int i = 0; i < 1; i++) {
		renderer->renderScene();
	}
	saveImage("5_samples_with_control_variate_green");
	renderer->resetAccumulatedFrames();
	for (int i = 0; i < 6; i++) {
		renderer->renderScene();
	}
	saveImage("30_samples_with_control_variate_green");
	renderer->setControlVariate();
	renderer->setTransferFunction({ 0.0,0.0,1.0,1.0 }, Renderer::COLOR);
	renderer->resetAccumulatedFrames();
	for (int i = 0; i < 1; i++) {
		renderer->renderScene();
	}
	saveImage("5_samples_with_control_variate_multi_color");
	renderer->resetAccumulatedFrames();
	for (int i = 0; i < 6; i++) {
		renderer->renderScene();
	}
	saveImage("30_samples_with_control_variate_multi_color");


}
void Tester::runSingleTestcase(std::vector<float> transferFunction, int nrOfRendersteps,
		Renderer::RenderMethods renderMethod, bool useControlVariate,
		bool setControlVariate, bool saveImageToFile, std::string imageName) {
	renderer->resetAccumulatedFrames();
	renderer->setTransferFunction(transferFunction, Renderer::COLOR);
	renderer->setRenderMethod(renderMethod);
	renderer->setUseControlVariate(useControlVariate);

	for (int i = 0; i < nrOfRendersteps; i++) {
		renderer->renderScene();
	}

	if (saveImageToFile) {
		saveImage(imageName);
	}
	if (setControlVariate) {
		renderer->setControlVariate();
	}
}
void Tester::saveImage(std::string fileName)
{
	int* buffer = new int[width * height * 3];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	stbi_flip_vertically_on_write(1);
	std::string fullPath;
	fullPath.append(resultsFolder)/*.append(currentFolder)*/.append(fileName).append(".png");
	stbi_write_png(fullPath.c_str(), width, height, 3, buffer, width * 3);
}

void Tester::init()
{
	if (!glfwInit()) {
		return;
	}
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);  // Entferne Rahmen und Titel
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);  // Deaktiviere Gr��en�nderung
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfw_window = glfwCreateWindow(width, height, "Volume Renderer", NULL, NULL);
	if (!glfw_window) {
		// Fehlerbehandlung: Das Fenster konnte nicht erstellt werden
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(glfw_window);
	glfwSwapInterval(1);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
	glViewport(0, 0, width, height);

	renderer = &Renderer(width, height);
	renderer->init();

	currentFolder = getCurrentTimeAsString().append("/");
	std::string newFolder;
	newFolder.append(resultsFolder).append(currentFolder);
	createDirectory(newFolder);

	this->generateTestImages();
}
std::string Tester::getCurrentTimeAsString() {
	auto now = std::chrono::system_clock::now();
	std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

	// Convert time to local time
	std::tm* localTime = std::localtime(&currentTime);

	// Format the time as a string
	std::stringstream ss;
	ss << std::put_time(localTime, "%Y-%m-%d %H-%M-%S");
	return ss.str();
}
bool Tester::createDirectory(const std::string& path) {
	try {
		//create_directories(path);
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error creating directory: " << e.what() << std::endl;
		return false;
	}
}