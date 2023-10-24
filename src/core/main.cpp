#include <core/main.h>
#include <core/renderer.h>
#include <iostream>
#include <GLFW/glfw3.h>

int main() {
	if (!glfwInit()) {
		// Fehlerbehandlung: GLFW konnte nicht initialisiert werden
		return -1;
	}
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Volume Renderer", NULL, NULL);
	Renderer* renderer = new Renderer(window);
	//int width, height;
	//glfwGetFramebufferSize(window, &width, &height);
	//glViewport(0, 0, width, height);
	if (!window) {
		// Fehlerbehandlung: Das Fenster konnte nicht erstellt werden
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window)) {
		// Hier renderst du deine Szene
		renderer->renderScene();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}
