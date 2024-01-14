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
	Renderer();
	//void loadScene(Scene* scene);
	void renderScene();
	void processInput(GLFWwindow* window, float delta);
	void unusedRender();
	void init();
	//callback functions
	void scroll_callback(double xoffset, double yoffset);
private:
	VolumeScene scene;
};

#endif // !RENDERER_H