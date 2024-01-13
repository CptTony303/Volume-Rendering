#include <core/window.h>
#include <core/main.h>
#include <core/renderer.h>
#include <iostream>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

}
Window* Window::window = NULL;
Window::Window() {
}

Window* Window::getInstance()
{
	if (window == NULL)
	{
		window = new Window();
		//window->startRenderLoop();
		return window;
	}
	else
	{
		return window;
	}
}

void Window::registerCallback(int type, void* func)
{
	GLFWwindow* glfw_window = reinterpret_cast<GLFWwindow*>(glfwGetWindowUserPointer(_glfw_window));
	switch (type)
	{
	case KEY:
			glfwSetKeyCallback(glfw_window, reinterpret_cast<GLFWkeyfun>(func));
		break;
	case MOUSE_BUTTON:
		glfwSetMouseButtonCallback(glfw_window, reinterpret_cast<GLFWmousebuttonfun>(func));
		break;
	case SCROLL:
		glfwSetScrollCallback(glfw_window, reinterpret_cast<GLFWscrollfun>(func));
		break;
	case FRAMEBUFFER_SIZE:
		glfwSetFramebufferSizeCallback(glfw_window, reinterpret_cast<GLFWframebuffersizefun>(func));
		break;
	default:
		break;
	}
}

void Window::startRenderLoop() {
	if (!glfwInit()) {
		// Fehlerbehandlung: GLFW konnte nicht initialisiert werden
		return;
	}
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* glfw_window = glfwCreateWindow(800, 600, "Volume Renderer", NULL, NULL);
	int width, height;


	if (!glfw_window) {
		// Fehlerbehandlung: Das Fenster konnte nicht erstellt werden
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(glfw_window);

	//set callback functions
	glfwSetFramebufferSizeCallback(glfw_window, framebuffer_size_callback);
	//glfwSetKeyCallback(glfw_window, key_callback);
	//glfwSetMouseButtonCallback(glfw_window, mouse_button_callback);
	//glfwSetScrollCallback(glfw_window, scroll_callback);

	glfwGetFramebufferSize(glfw_window, &width, &height);
	glfwSwapInterval(1);
	//glViewport(0, 0, width, height);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	renderer = &Renderer();
	renderer->init();
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();
	float lastFrameTimestamp = glfwGetTime();
	while (!glfwWindowShouldClose(glfw_window)) {
		float currentFrameTimestamp = glfwGetTime();
		float delta = currentFrameTimestamp - lastFrameTimestamp;
		lastFrameTimestamp = currentFrameTimestamp;
		// Hier renderst du deine Szene
		processInput(glfw_window);
		renderer->processInput(glfw_window, delta);
		glfwPollEvents();
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow(); // Show demo window! :)

		renderer->renderScene();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(glfw_window);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}
void Window::initWindow()
{
	if (!glfwInit()) {
		// Fehlerbehandlung: GLFW konnte nicht initialisiert werden
		return;
	}
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* glfw_window = glfwCreateWindow(800, 600, "Volume Renderer", NULL, NULL);
	glfwSetWindowUserPointer(glfw_window, _glfw_window);
	int width, height;


	if (!glfw_window) {
		// Fehlerbehandlung: Das Fenster konnte nicht erstellt werden
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(glfw_window);

	//set callback functions
	glfwSetFramebufferSizeCallback(glfw_window, framebuffer_size_callback);
	//glfwSetKeyCallback(glfw_window, key_callback);
	//glfwSetMouseButtonCallback(glfw_window, mouse_button_callback);
	//glfwSetScrollCallback(glfw_window, scroll_callback);

	glfwGetFramebufferSize(glfw_window, &width, &height);
	glfwSwapInterval(1);
	//glViewport(0, 0, width, height);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	renderer = &Renderer();
	renderer->init();
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();
}
void Window::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}