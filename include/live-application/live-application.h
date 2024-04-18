#ifndef LIVE_APPLICATION_H
#define LIVE_APPLICATION_H

#include <core/renderer.h>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class LiveApplication {
public:
	LiveApplication() :
		renderer(nullptr),
		width(2000),
		height(1300),
		glfw_window(nullptr){};
	void start();

	//static LiveApplication* getInstance();
	//// deleting copy constructor
	//LiveApplication(const LiveApplication& obj)
	//	= delete;

private:
	Renderer* renderer;
	int width, height;
	GLFWwindow* glfw_window;
};

////callback functions
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

#endif // !WINDOW_H
