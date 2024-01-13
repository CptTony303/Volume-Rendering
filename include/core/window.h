#ifndef WINDOW_H
#define WINDOW_H

#include <core/renderer.h>
#include <core/window.h>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Window {
public:
	enum CallbackType {KEY, MOUSE_BUTTON, SCROLL, FRAMEBUFFER_SIZE};
	static Window* getInstance();
	void startRenderLoop();
	void initWindow();
	void registerCallback(int type, void* func);

	GLFWwindow* _glfw_window;
	// deleting copy constructor
	//Window(const Window& obj)
	//	= delete;

private:
	Window();
	void processInput(GLFWwindow* window);

	static Window* Window::window;
	Renderer* renderer;

};

#endif // !WINDOW_H
