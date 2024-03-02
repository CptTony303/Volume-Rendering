#ifndef RENDERER_H
#define RENDERER_H


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <core/shader.h>
#include <glm/glm.hpp>
#include <core/Scenes/volumeScene.h>

class Renderer
{
public:
	Renderer() {};
	Renderer(int width, int height);
	//void loadScene(Scene* scene);
	void renderScene();
	void processInput(GLFWwindow* window, float delta);
	void init();
	//callback functions
	void scroll_callback(double xoffset, double yoffset);
	void framebuffer_size_callback(int width, int height);
private:
	VolumeScene scene;
};

#endif // !RENDERER_H