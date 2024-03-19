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
		resultsFolder("./results/") {
		init();
	};
	void generateTestImages();
private:
	void runSingleTestcase(std::vector<float> transferFunction, int nrOfRendersteps,
		Renderer::RenderMethods renderMethod, bool useControlVariate,
		bool setControlVariate, bool saveImageToFile, std::string imageName);
	void saveImage(std::string fileName);
	void init();
	std::string getCurrentTimeAsString();
	bool createDirectory(const std::string& path);

	void depricated();

	int width, height;
	Renderer* renderer;
	GLFWwindow* glfw_window;
	std::string resultsFolder;
	std::string currentFolder;
};

#endif