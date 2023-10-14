#include <core/main.h>
#include <iostream>
#include <GLFW/glfw3.h>

int main() {
	std::cout << "Hello World" << std::endl;
	if (!glfwInit()) {
		// Fehlerbehandlung: GLFW konnte nicht initialisiert werden
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Mein erstes GLFW-Fenster", NULL, NULL);
	if (!window) {
		// Fehlerbehandlung: Das Fenster konnte nicht erstellt werden
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	while (!glfwWindowShouldClose(window)) {
		// Hier renderst du deine Szene

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}
