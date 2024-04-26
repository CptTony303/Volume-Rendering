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
#include <random>
//#include <Flip/FLIP.h>

void Tester::generateTestImages() {

	Texture3D data_foot("./Assets/foot_256x256x256_uint8.raw",glm::vec3(256));
	Texture3D data_head("./Assets/vis_male_128x256x256_uint8.raw", glm::vec3(128, 256, 256));

	std::string dataset_folder = currentFolder;

	glm::mat4 m_0(1.f);
	glm::mat4 m_1(glm::rotate(m_0, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)));
	glm::mat4 m_2(glm::rotate(m_0, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f)));

	int numberOfTestCases = 10; 
	int changeOfTransferFunction = 50;// the higher, the smaller the change. needs to be even.
	renderer->setVolumeData(data_head);

	currentFolder = std::string().append(dataset_folder).append("tf color changes");
	createDirectory(currentFolder);
	//color changed
	for (int i = 0; i < numberOfTestCases; i++) {
		auto density = generateRandomTransferFunction(changeOfTransferFunction);
		for (int i = 0; i < changeOfTransferFunction / 10 || i < 2; i++)
		{
			density[1 + 2 * i] = 0.f;
		}
		auto color1 = generateRandomTransferFunction(changeOfTransferFunction);
		auto color2 = generateRandomTransferFunction(changeOfTransferFunction);
		runSingleTestcase(color1, density,
			color2, density,
			30, 30, std::to_string(i), m_1);
	}
	currentFolder = std::string().append(dataset_folder).append("tf oppacity changes");
	createDirectory(currentFolder);
	//opaccity changed
	for (int i = 0; i < numberOfTestCases; i++) {
		auto density1 = generateRandomTransferFunction(changeOfTransferFunction);
		auto density2 = generateRandomTransferFunction(changeOfTransferFunction);
		for (int i = 0; i < changeOfTransferFunction/10 || i < 2; i++)
		{
			density1[1 + 2 * i] = 0.f;
			density2[1 + 2 * i] = 0.f;
		}
		auto color = generateRandomTransferFunction(changeOfTransferFunction);
		runSingleTestcase(color, density1,
			color, density2,
			30, 30, std::to_string(i), m_1);
	}
	currentFolder = std::string().append(dataset_folder).append("tf color and oppacity changes");
	createDirectory(currentFolder);
	//color and opaccity changed
	for (int i = 0; i < numberOfTestCases; i++) {
		auto density1 = generateRandomTransferFunction(changeOfTransferFunction);
		auto density2 = generateRandomTransferFunction(changeOfTransferFunction);
		for (int i = 0; i < changeOfTransferFunction/10 || i < 2; i++)
		{
			density1[1 + 2 * i] = 0.f;
			density2[1 + 2 * i] = 0.f;
		}
		auto color1 = generateRandomTransferFunction(changeOfTransferFunction);
		auto color2 = generateRandomTransferFunction(changeOfTransferFunction);
		runSingleTestcase(color1, density1,
			color2, density2,
			30, 30, std::to_string(i), m_1);
	}
	currentFolder = std::string().append(dataset_folder).append("low quality cv");
	createDirectory(currentFolder);
	//low quality cv
	for (int i = 0; i < numberOfTestCases; i++) {
		auto density1 = generateRandomTransferFunction(changeOfTransferFunction);
		auto density2 = generateRandomTransferFunction(changeOfTransferFunction);
		for (int i = 0; i < changeOfTransferFunction/10 || i < 2; i++)
		{
			density1[1 + 2 * i] = 0.f;
			density2[1 + 2 * i] = 0.f;
		}
		auto color1 = generateRandomTransferFunction(changeOfTransferFunction);
		auto color2 = generateRandomTransferFunction(changeOfTransferFunction);
		runSingleTestcase(color1, density1,
			color2, density2,
			30, randomInt(1, 3), std::to_string(i), m_1);
	}
	currentFolder = std::string().append(dataset_folder).append("different volume data");
	createDirectory(currentFolder);
	//different volume data
	renderer->setVolumeData(data_foot);
	for (int i = 0; i < numberOfTestCases; i++) {
		auto density1 = generateRandomTransferFunction(changeOfTransferFunction);
		auto density2 = generateRandomTransferFunction(changeOfTransferFunction);
		for (int i = 0; i < changeOfTransferFunction/10 || i < 2; i++)
		{
			density1[1 + 2 * i] = 0.f;
			density2[1 + 2 * i] = 0.f;
		}
		auto color1 = generateRandomTransferFunction(changeOfTransferFunction);
		auto color2 = generateRandomTransferFunction(changeOfTransferFunction);
		runSingleTestcase(color1, density1,
			color2, density2,
			30, 30, std::to_string(i), m_1);
	}


	//fps tests
	//for (int i = 0; i < numberOfTestCases; i++) {
	//	renderer->setVolumePosition(m_1);
	//	renderer->resetAccumulatedFrames();
	//	renderer->setTransferFunction(transferFunctionColor_CV, Renderer::COLOR);
	//	renderer->setTransferFunction(transferFunctionDensity_CV, Renderer::TRANSPARENCY);
	//	renderer->setUseControlVariate(false);

	//	for (int i = 0; i < nrOfRendersteps_CV; i++) {
	//		renderer->renderScene();
	//		glfwSwapBuffers(glfw_window);
	//	}
	//	renderer->setControlVariate();
	//	renderer->resetAccumulatedFrames();
	//	renderer->setTransferFunction(transferFunctionColor, Renderer::COLOR);
	//	renderer->setTransferFunction(transferFunctionDensity, Renderer::TRANSPARENCY);
	//	for (int i = 0; i < nrOfRendersteps; i++) {
	//		renderer->renderScene();
	//		glfwSwapBuffers(glfw_window);
	//	}
	//	renderer->setUseControlVariate(true);
	//	renderer->renderScene();
	//	glfwSwapBuffers(glfw_window);
	//	std::string newFolder = std::string().append(resultsFolder).append(currentFolder).append("/").append(test_case).append("/");
	//	createDirectory(newFolder);
	//	renderer->saveDataToFile(newFolder);
	//}
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

	for (int i = 0; i < nrOfRendersteps_CV; i++) {
		renderer->renderScene();
		glfwSwapBuffers(glfw_window);
	}
	renderer->setControlVariate();
	renderer->resetAccumulatedFrames();
	renderer->setTransferFunction(transferFunctionColor, Renderer::COLOR);
	renderer->setTransferFunction(transferFunctionDensity, Renderer::TRANSPARENCY);
	for (int i = 0; i < nrOfRendersteps; i++) {
		renderer->renderScene();
		glfwSwapBuffers(glfw_window);
	}
	renderer->setUseControlVariate(true);
	renderer->renderScene();
	glfwSwapBuffers(glfw_window);
	std::string newFolder = std::string().append(resultsFolder).append(currentFolder).append("/").append(test_case).append("/");
	createDirectory(newFolder);
	renderer->saveDataToFile(newFolder);
}
int Tester::randomInt(int min, int max){
	int minVal = min; // Mindestwert
	int maxVal = max; // Maximalwert

	// Zufallszahlengenerator
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(minVal, maxVal);

	// Generiere eine zufällige Zahl zwischen minVal und maxVal
	return dis(gen);
}
 std::vector<float> Tester::generateRandomTransferFunction(int numberOfSamplePoints) {
	// Zufallszahlengenerator
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	// Transferfunktion-Arrays
	std::vector<float> transferFunction;

	// Zufällige Werte für die Transferfunktion generieren
	for (int i = 0; i < numberOfSamplePoints; ++i) {
		transferFunction.push_back(float(i)/float(numberOfSamplePoints-1));
		transferFunction.push_back(dis(gen));
	}

	// Paar von Transferfunktion-Arrays zurückgeben
	return transferFunction;
}
 glm::mat4 Tester::generateRandomRotationMatrix() {
	 // Zufallszahlengenerator
	 std::random_device rd;
	 std::mt19937 gen(rd());
	 std::uniform_real_distribution<float> angleDis(0.0f, 360.0f); // Winkel von 0 bis 360 Grad
	 std::uniform_real_distribution<float> axisDis(-1.0f, 1.0f); // Zufällige Achsenkoordinaten von -1 bis 1

	 // Zufällige Winkel und Achsen generieren
	 float angle = glm::radians(angleDis(gen)); // Zufälliger Winkel in Bogenmaß umwandeln
	 glm::vec3 axis(axisDis(gen), axisDis(gen), axisDis(gen)); // Zufälliger Vektor als Achse

	 // Normalisiere die Achse
	 axis = glm::normalize(axis);

	 // Rotationsmatrix erstellen
	 return glm::rotate(glm::mat4(1.0f), angle, axis);
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