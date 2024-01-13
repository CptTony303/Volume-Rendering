#include <core/main.h>
#include <core/renderer.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <core/window.h>
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//	glViewport(0, 0, width, height);
//}
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//	//std::cout << "Key: " << key << ", Action: " << action << std::endl;
//}
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
//	//std::cout << "Key: " << key << ", Action: " << action << std::endl;
//}
//void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
//	//std::cout << "Key: " << button << ", Action: " << action << std::endl;
//}
//void processInput(GLFWwindow* window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//
//}
int main() {
	Window* window = Window::getInstance();
	//window->initWindow();
	window->startRenderLoop();
	return 0;
}
//int old_main() {
//	if (!glfwInit()) {
//		// Fehlerbehandlung: GLFW konnte nicht initialisiert werden
//		return -1;
//	}
//	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//	GLFWwindow* window = glfwCreateWindow(800, 600, "Volume Renderer", NULL, NULL);
//	int width, height;
//	
//
//	if (!window) {
//		// Fehlerbehandlung: Das Fenster konnte nicht erstellt werden
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//
//	//set callback functions
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//	glfwSetKeyCallback(window, key_callback);
//	glfwSetMouseButtonCallback(window, mouse_button_callback);
//	glfwSetScrollCallback(window, scroll_callback);
//
//	glfwGetFramebufferSize(window, &width, &height);
//	glfwSwapInterval(1);
//	//glViewport(0, 0, width, height);
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//	{
//		std::cout << "Failed to initialize GLAD" << std::endl;
//		return -1;
//	}
//	Renderer renderer = Renderer();
//	renderer.init();
//	// Setup Dear ImGui context
//	IMGUI_CHECKVERSION();
//	ImGui::CreateContext();
//	ImGuiIO& io = ImGui::GetIO();
//	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
//
//	// Setup Platform/Renderer backends
//	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
//	ImGui_ImplOpenGL3_Init();
//	float lastFrameTimestamp = glfwGetTime();
//	while (!glfwWindowShouldClose(window)) {
//		float currentFrameTimestamp = glfwGetTime();
//		float delta = currentFrameTimestamp - lastFrameTimestamp;
//		lastFrameTimestamp = currentFrameTimestamp;
//		// Hier renderst du deine Szene
//		processInput(window);
//		renderer.processInput(window, delta);
//		glfwPollEvents();
//		// Start the Dear ImGui frame
//		ImGui_ImplOpenGL3_NewFrame();
//		ImGui_ImplGlfw_NewFrame();
//		ImGui::NewFrame();
//		//ImGui::ShowDemoWindow(); // Show demo window! :)
//
//		renderer.renderScene();
//
//		ImGui::Render();
//		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//		glfwSwapBuffers(window);
//	}
//	ImGui_ImplOpenGL3_Shutdown();
//	ImGui_ImplGlfw_Shutdown();
//	ImGui::DestroyContext();
//	glfwTerminate();
//	return 0;
//}
