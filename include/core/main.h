#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Main {
public:
	//int main();
	int set_up_glfw();
	//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
private:
	void processInput(GLFWwindow* window);
};