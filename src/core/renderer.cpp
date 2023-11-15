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

Renderer::Renderer(GLFWwindow* window) {
	_window = window;
}

void Renderer::init() {

	char* projectPath = "X:/Anton/Projects/Programming/C/Volume-Rendering/";

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glEnable(GL_DEPTH_TEST);

	/*float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};*/

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3,

		1, 2, 5,
		2, 5, 6,

		0, 3, 4,
		3, 4, 7,

		0, 1, 4,
		1, 4, 5,

		2, 3, 7,
		2, 6, 7,

		4, 5, 6,
		4, 6, 7
	};
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	const char* vertPath = "X:/Anton/Projects/Programming/C/Volume-Rendering/shaders/volume.vert";
	const char* fragPath = "X:/Anton/Projects/Programming/C/Volume-Rendering/shaders/volume.frag";

	myShader = new Shader(vertPath, fragPath);
	myShader->use();

	Texture container_text("X:/Anton/Projects/Programming/C/Volume-Rendering/Assets/container.jpg", GL_TEXTURE0, GL_RGB);
	Texture face_text("X:/Anton/Projects/Programming/C/Volume-Rendering/Assets/awesomeface.png", GL_TEXTURE1, GL_RGBA);
	Texture3D tooth("X:/Anton/Projects/Programming/C/Volume-Rendering/Assets/tooth_103x94x161_uint8.raw", GL_TEXTURE2, glm::vec3(103, 94, 161));
	Texture3D foot("X:/Anton/Projects/Programming/C/Volume-Rendering/Assets/foot_256x256x256_uint8.raw", GL_TEXTURE3, glm::vec3(256, 256, 256)); 
	Texture3D head("X:/Anton/Projects/Programming/C/Volume-Rendering/Assets/vis_male_128x256x256_uint8.raw", GL_TEXTURE4, glm::vec3(128, 256, 256));
	myShader->setInt("texture1", 0);
	myShader->setInt("texture2", 1);
	myShader->setInt("volumeTexture", 4);
	myShader->setFloat("isovalue", 0.45f);
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::scale(model, glm::vec3(103, 94, 161));
	glm::mat4 view = glm::mat4(1.0f);
	// note that we're translating the scene in the reverse direction of where we want to move
	_cam_pos = glm::vec3(0.0f, 0.0f, -3.0f);
	_cam_rot = glm::vec3(-0.0f, 0.0f, 0.0f);
	//noch nicht richtig, aber ok für testzwecke
	view = glm::rotate(view, _cam_rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::rotate(view, _cam_rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::rotate(view, _cam_rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
	view = glm::translate(view, _cam_pos);

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	int modelLoc = glGetUniformLocation(myShader->ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	int viewLoc = glGetUniformLocation(myShader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	int projectionLoc = glGetUniformLocation(myShader->ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Renderer::renderScene() {
	bool oszillate = true;
	bool spin = true;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);
	myShader->use();

	if (spin)
	{
		glm::mat4 model = glm::mat4(1.0f);
		float angle = 20.0f;
		//model = glm::scale(model, glm::vec3(103, 94, 800));
		//model = glm::scale(model, glm::vec3(0.01, 0.01, 0.01));
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		int modelLoc = glGetUniformLocation(myShader->ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	}
	if (oszillate)
	{
		myShader->setFloat("isovalue", sin((float)glfwGetTime() * 0.25) * 0.5 + 0.5);
	}

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//glm::vec3 cubePositions[] = {
	//glm::vec3(0.0f,  0.0f,  0.0f),
	//glm::vec3(2.0f,  5.0f, -15.0f),
	//glm::vec3(-1.5f, -2.2f, -2.5f),
	//glm::vec3(-3.8f, -2.0f, -12.3f),
	//glm::vec3(2.4f, -0.4f, -3.5f),
	//glm::vec3(-1.7f,  3.0f, -7.5f),
	//glm::vec3(1.3f, -2.0f, -2.5f),
	//glm::vec3(1.5f,  2.0f, -2.5f),
	//glm::vec3(1.5f,  0.2f, -1.5f),
	//glm::vec3(-1.3f,  1.0f, -1.5f)
	//};

	//for (unsigned int i = 0; i < 10; i++)
	//{
	//    glm::mat4 model = glm::mat4(1.0f);
	//    model = glm::translate(model, cubePositions[i]);
	//    float angle = 20.0f * i;
	//    if (angle == 0.0f) {
	//        angle = 10.0f;
	//    }
	//    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
	//    int modelLoc = glGetUniformLocation(myShader->ID, "model");
	//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//    //glDrawArrays(GL_TRIANGLES, 0, 36);
	//}
}