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

//#include <imgui_widgets.cpp>

Renderer::Renderer(GLFWwindow* window) {
	_window = window;
}

void Renderer::init() {

	int samplesPerFrame = 25;

	char* projectPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/";

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
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f,
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

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


	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// generate texture
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// generate depthbuffer
	unsigned int depthbuffer;
	glGenTextures(1, &depthbuffer);
	glBindTexture(GL_TEXTURE_2D, depthbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthbuffer, 0);

	const char* vertPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/volume.vert";
	const char* fragPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/volume.frag";

	myShader = new Shader(vertPath, fragPath);
	myShader->use();

	Texture container_text("C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/Assets/container.jpg", GL_TEXTURE0, GL_RGB);
	Texture face_text("C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/Assets/awesomeface.png", GL_TEXTURE1, GL_RGBA);
	Texture3D tooth("C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/Assets/tooth_103x94x161_uint8.raw", GL_TEXTURE2, glm::vec3(103, 94, 161));
	Texture3D foot("C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/Assets/foot_256x256x256_uint8.raw", GL_TEXTURE3, glm::vec3(256, 256, 256));
	myShader->setInt("texture1", 0);
	myShader->setInt("texture2", 1);
	myShader->setInt("texture3", 3);
	myShader->setFloat("isovalue", isovalue);
	myShader->setInt("brightness", brightness);
	myShader->setInt("methode", methode);
	myShader->setInt("samplesPerFrame", samplesPerFrame);
	myShader->setFloat("randomizer", (float)glfwGetTime());
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


	// Framebuffer VAO
	glGenVertexArrays(1, &VAO_framebuffer);
	glBindVertexArray(VAO_framebuffer);

	float framebufferVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int VBO_framebuffer;
	glGenBuffers(1, &VBO_framebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_framebuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(framebufferVertices), &framebufferVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glGenFramebuffers(1, &fbo_switch);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_switch);
	// generate texture
	unsigned int textureSwitch;
	glGenTextures(1, &textureSwitch);
	glBindTexture(GL_TEXTURE_2D, textureSwitch);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureSwitch, 0);

	const char* vertPathFramebuffer = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/framebuffer.vert";
	const char* fragPathFramebuffer = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/framebuffer.frag";

	framebufferShader = new Shader(vertPathFramebuffer, fragPathFramebuffer);
	framebufferShader->use();


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

	framebufferShader->setInt("screenTexture", 0);
	//framebufferShader->setInt("screenCopy", 1);
	framebufferShader->setInt("runs", 0);
	framebufferShader->setInt("samplesPerRun", samplesPerFrame);


	glGenVertexArrays(1, &VAO_copy);
	glBindVertexArray(VAO_copy);

	unsigned int VBO_copy;
	glGenBuffers(1, &VBO_copy);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_copy);
	glBufferData(GL_ARRAY_BUFFER, sizeof(framebufferVertices), &framebufferVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glGenFramebuffers(1, &fbo_copy);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_copy);

	glActiveTexture(GL_TEXTURE1);
	// generate texture
	unsigned int textureCopy;
	glGenTextures(1, &textureCopy);
	glBindTexture(GL_TEXTURE_2D, textureCopy);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureCopy, 0);

	const char* vertPathcopy = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/copy.vert";
	const char* fragPathcopy = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/copy.frag";

	copyShader = new Shader(vertPathcopy, fragPathcopy);
	copyShader->use();

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureSwitch);

	copyShader->setInt("screenTexture", 2);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureCopy);
	framebufferShader->use();
	framebufferShader->setInt("screenCopy", 1);
}

void Renderer::renderScene() {

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	//glDeleteFramebuffers(1, &fbo);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);
	myShader->use();
	
	ImGui::Begin("Volume Renderer Settings");
	ImGui::Text("Hello, world");
	ImGui::Checkbox("oszillate iso", &oszillate);
	ImGui::Checkbox("spin", &spin);
	ImGui::SliderFloat("isovalue", &isovalue, 0.0f, 1.0f, "%.3f");
	ImGui::SliderInt("brightness", &brightness, 1, 75);
	ImGui::SliderFloat("stepSize", &stepSize, 0.001f, 0.02f, "%.5f");
	ImGui::RadioButton("Ray Marching", &methode, 0); ImGui::SameLine();
	ImGui::RadioButton("Monte Carlo Sampling", &methode, 1);
	ImGui::RadioButton("Just rng demo", &methode, 2);
	ImGui::RadioButton("colored cube", &methode, -1);
	ImGui::End();

	myShader->setInt("brightness", brightness);
	myShader->setFloat("stepSize", stepSize);
	myShader->setInt("methode", methode);

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
		isovalue = sin((float)glfwGetTime() * 0.25) * 0.5 + 0.5;
	}
	myShader->setFloat("isovalue", isovalue);
	myShader->setFloat("randomizer", (float)glfwGetTime());

	glEnable(GL_DEPTH_TEST);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_switch);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
	}
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	framebufferShader->use();
	glBindVertexArray(VAO_framebuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	runs++;
	if (methode != 1) {
		runs = 0;
	}
	framebufferShader->setInt("runs", runs);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_copy);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	copyShader->use();
	glBindVertexArray(VAO_copy);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(VAO_copy);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
}