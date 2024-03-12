#include <core/renderer.h>
#include <string>;

#ifndef TESTER_H
#define TESTER_H

class Tester {
public:
	Tester() : width(1920),
		height(1080),
		renderer(nullptr),
		glfw_window(nullptr),
		resultsFolder("C:/Users/Anton/Desktop/Results/") {
		init();
	};
	void generateTestImages();
private:
	void saveImage(std::string fileName);
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