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
#include <GLFW/stb_image_write.h>
//#include <Flip/FLIP.h>


void Tester::generateTestImages() {
	glm::mat4 m_0(1.f);
	glm::mat4 m_1(glm::rotate(m_0, glm::radians(45.f), glm::vec3(1.f, 1.f, 0.f)));

	std::vector<float> x  = { 0.0,0.0,1.0,1.0 };
	std::vector<float> y = { 0.0, 0.1, 1.0, 0.9 };
	//std::vector<float> z = { 0.0, 0.1, 1.0, 0.9 };

	std::vector<float> a = { 0.0,0.0, 0.07, 0.0 ,1.0,1.0 };
	std::vector<float> b = { 0.0,0.0, 0.3, 0.0 ,1.0,1.0 };
	//std::vector<float> c = { 0.0,0.0, 0.3, 0.0,0.5,0.3 , 0.7, 0.0 , 1.0,1.0 };
	int count = 0;
	runSingleTestcase(x, a, x, a, 1, 12, std::to_string(count++), m_0);
	runSingleTestcase(x, a, y, a, 1, 12, std::to_string(count++), m_0);
	runSingleTestcase(x, b, x, a, 1, 12, std::to_string(count++), m_0);
	runSingleTestcase(x, a, x, b, 1, 12, std::to_string(count++), m_0);
}

void Tester::runSingleTestcase(std::vector<float> transferFunctionColor, std::vector<float> transferFunctionDensity,
	std::vector<float> transferFunctionColor_CV, std::vector<float> transferFunctionDensity_CV,
	int nrOfRendersteps, int nrOfRendersteps_CV,
		std::string test_case, glm::mat4 volumePosition) {
	renderer->setVolumePosition(volumePosition);
	renderer->resetAccumulatedFrames();
	renderer->setTransferFunction(transferFunctionColor_CV, Renderer::COLOR);
	renderer->setTransferFunction(transferFunctionDensity_CV, Renderer::TRANSPARENCY);
	renderer->setUseControlVariate(false);

	for (int i = 0; i < nrOfRendersteps; i++) {
		renderer->renderScene();
	}
	renderer->setControlVariate();
	renderer->setUseControlVariate(true);
	renderer->resetAccumulatedFrames();
	renderer->setTransferFunction(transferFunctionColor, Renderer::COLOR);
	renderer->setTransferFunction(transferFunctionDensity, Renderer::TRANSPARENCY);
	for (int i = 0; i < nrOfRendersteps; i++) {
		renderer->renderScene();
	}
	std::string newFolder = std::string().append(resultsFolder).append(currentFolder).append("/").append(test_case).append("/");
	createDirectory(newFolder);
	renderer->saveDataToFile(newFolder);
}
void Tester::saveImage(std::string fileName)
{
	int* buffer = new int[width * height * 3];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	stbi_flip_vertically_on_write(1);
	std::string fullPath;
	fullPath.append(resultsFolder).append(currentFolder).append(fileName).append(".png");
	stbi_write_png(fullPath.c_str(), width, height, 3, buffer, width * 3);
}

void Tester::init()
{
	if (!glfwInit()) {
		return;
	}
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);  // Entferne Rahmen und Titel
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);  // Deaktiviere Größenänderung
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

	glfwTerminate();
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
		std::filesystem::create_directories(path);
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error creating directory: " << e.what() << std::endl;
		return false;
	}
}