//#ifndef WINDOW_H
//#define WINDOW_H
//
//#include <core/renderer.h>
//#include <core/window.h>
//#include <glad/glad.h>
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>
//
//class Window {
//public:
//	static Window* getInstance();
//
//	// deleting copy constructor
//	//Window(const Window& obj)
//	//	= delete;
//
//private:
//	Window();
//	void startRenderLoop();
//	void initWindow();
//	enum CallbackType { KEY, MOUSE_BUTTON, SCROLL, FRAMEBUFFER_SIZE };
//	void registerCallback(GLFWwindow* window, int type, void* func);
//	void processInput(GLFWwindow* window);
//
//	static Window* Window::window;
//	Renderer* renderer;
//};
//
////callback functions
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//
//#endif // !WINDOW_H
