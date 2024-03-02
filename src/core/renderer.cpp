#include <core/renderer.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <core/shader.h>
#include <stb_image.h>
#include <core/texture.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <core/texture3D.h>
#include <imgui.h>
#include <core/Scenes/volumeScene.h>


Renderer::Renderer(int width, int height)
{
	scene = VolumeScene(width, height);
}

void Renderer::init() {
	scene.initScene();
}
void Renderer::renderScene() {
	scene.renderScene();
}

void Renderer::processInput(GLFWwindow* window, float delta)
{
	scene.processInput(window, delta);
}
//callback functions
void Renderer::scroll_callback(double xoffset, double yoffset)
{
	scene.scroll_callback(xoffset, yoffset);
}

void Renderer::framebuffer_size_callback(int width, int height)
{
	scene.framebuffer_size_callback( width,  height);
}
