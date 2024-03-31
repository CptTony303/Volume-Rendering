#ifndef CONTROLLER_H
#define CONTROLLER_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <core/renderer.h>

class Controller {
public:
	Controller():
	renderer(nullptr),
	glfw_window(nullptr),
	lastMouse(glm::vec2(0,0)),
	cameraSpeed(1.f),
		sensitivity(1.f){};
	void init(GLFWwindow* glfw_window, Renderer* renderer);
	bool processInput(float delta);
private:
	Renderer* renderer;
	GLFWwindow* glfw_window;

	glm::vec2 lastMouse;
	float cameraSpeed;
	float sensitivity;
};

#endif 