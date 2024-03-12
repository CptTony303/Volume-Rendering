#include <core/main.h>
#include <core/renderer.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <core/window.h>
#include <tester/tester.h>

// am besten mit argument für entweder live app oder tester
int main() {
	Tester();
	//Window* window = Window::getInstance(); // window nur für live app
	//window->initWindow();
	//window->startRenderLoop();
	return 0;
}