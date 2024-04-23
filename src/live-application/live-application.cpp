#include <live-application/live-application.h>
#include <core/main.h>
#include <core/renderer.h>
#include <iostream>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <live-application/GUI/volumeGUI.h>
#include <live-application/controller.h>
#include <tester/tester.h>

//Window* Window::window = NULL;
//Window::Window() {
//	startRenderLoop();
//}

//LiveApplication* LiveApplication::getInstance()
//{
//	if (liveApp == NULL)
//	{
//		liveApp = new LiveApplication();
//		liveApp->start();
//	}
//	return liveApp;
//}

void LiveApplication::start()
{
	if (!glfwInit()) {
		return;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
	glfw_window = glfwCreateWindow(mode->width, mode->height, "Volume Renderer", NULL, NULL);
	if (!glfw_window) {
		// Fehlerbehandlung: Das Fenster konnte nicht erstellt werden
		glfwTerminate();
		return;
	}

	
	glfwSetWindowMonitor(glfw_window, primaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);


	glfwMakeContextCurrent(glfw_window);
	glfwGetFramebufferSize(glfw_window, &width, &height);
	//glViewport(0, 0, width, height);
	//set callback functions

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	renderer = &Renderer(width, height);
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

	glfwSwapInterval(1);


	renderer->setVolumeData(Texture3D("./Assets/vis_male_128x256x256_uint8.raw", glm::vec3(128, 256, 256)));
	glm::mat4 m_0(1.f);
	renderer->setVolumePosition(m_0);
	std::vector<float> x = { 0.0,0.0,1.0,1.0 };
	renderer->setTransferFunction(x, Renderer::COLOR);
	renderer->setTransferFunction(x, Renderer::TRANSPARENCY);
	renderer->setRenderMethod(Renderer::MONTE_CARLO);

	VolumeGUI gui = VolumeGUI();
	Controller controller = Controller();
	controller.init(glfw_window, renderer);

	float lastFrameTimestamp = glfwGetTime();
	while (!glfwWindowShouldClose(glfw_window)) {
		float currentFrameTimestamp = glfwGetTime();
		float delta = currentFrameTimestamp - lastFrameTimestamp;
		lastFrameTimestamp = currentFrameTimestamp;
		// Hier renderst du deine Szene
		//processInput(glfw_window);
		//renderer->processInput(glfw_window, delta);
		glfwPollEvents();
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow(); // Show demo window! :)
		/*renderer->setTransferFunction(transferFunctionColor, Renderer::COLOR);
		renderer->setTransferFunction(transferFunctionDensity, Renderer::TRANSPARENCY);*/
		renderer->setUseControlVariate(false);
		gui.renderGUI();
		if (controller.processInput(delta) || gui.reset_accumulation_frames) {
			renderer->resetAccumulatedFrames();
		}
		gui.reset_accumulation_frames = false;
		renderer->setTransferFunction(gui.getTransferFunction(0), Renderer::COLOR);
		renderer->setTransferFunction(gui.getTransferFunction(1), Renderer::TRANSPARENCY);

		if (gui.set_control_variate) {
			renderer->setControlVariate();
		}
		if (gui.delete_control_variate) {
			renderer->deleteControlVariate();
		}
		renderer->setUseControlVariate(gui.use_control_variate);
		if (gui.run_test_cases) {
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			break;
		}

		renderer->renderScene();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(glfw_window);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	if (gui.run_test_cases) {
		Tester();
	}
}
//void Window::processInput(GLFWwindow* window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//
//}
