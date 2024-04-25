#include <core/renderer.h>
#include <string>;
#include <vector>;

#ifndef TESTER_H
#define TESTER_H

class Tester {
public:
	Tester() : width(1920),
		height(1080),
		renderer(nullptr),
		glfw_window(nullptr),
		resultsFolder("./../results/") {
		init();
	};
	glm::mat4 generateRandomRotationMatrix();
private:
	void generateTestImages(int numberOfTestCases);
	void runSingleTestcase(std::vector<float> transferFunctionColor, std::vector<float> transferFunctionDensity, std::vector<float> transferFunctionColor_CV, std::vector<float> transferFunctionDensity_CV, int nrOfRendersteps, int nrOfRendersteps_CV, std::string test_case, glm::mat4 volumePosition);
	int randomInt(int min, int max);
	std::vector<float> generateRandomTransferFunction(int numberOfSamplePoints);
	void init();
	std::string getCurrentTimeAsString();
	bool createDirectory(const std::string& path);


	int width, height;
	Renderer* renderer;
	GLFWwindow* glfw_window;
	std::string resultsFolder;
	std::string currentFolder;
};

#endif