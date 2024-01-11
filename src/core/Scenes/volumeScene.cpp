#include <core/Scenes/volumeScene.h>
#include <glad/glad.h>
#include <core/framebuffer.h>
#include <core/texture3D.h>
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


VolumeScene::VolumeScene()
{
}

void VolumeScene::initScene() {
	//Scene::initScene();
	initTransformationMatrices();
	initVAOs();
	initFBOs();
	initShaders();
}

void VolumeScene::renderScene()
{
	renderGUI();
	updateShaderValues();
	renderVolume();
}


void VolumeScene::initTransformationMatrices()
{
	model = glm::mat4(1.0f);
	//model = glm::scale(model, glm::vec3(103, 94, 161));
	view = glm::mat4(1.0f);
	// note that we're translating the scene in the reverse direction of where we want to move
	glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, -3.0f);
	glm::vec3 cam_rot = glm::vec3(-0.0f, 0.0f, 0.0f);
	//noch nicht richtig, aber ok für testzwecke
	view = glm::rotate(view, cam_rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::rotate(view, cam_rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::rotate(view, cam_rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
	view = glm::translate(view, cam_pos);

	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}

void VolumeScene::initVolumeVAO()
{
	// setting vertices and indices for a cube to render the volume in
	// the vertices also bind a 3D texture coordinate
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
	// create and VAO for the Volume; private member variable to reuse it later
	glGenVertexArrays(1, &VAO_VOLUME);
	glBindVertexArray(VAO_VOLUME);

	// binding the vertices to the vao
	unsigned int vbo_volume;
	glGenBuffers(1, &vbo_volume);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_volume);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//binding the indices to the vao
	unsigned int ebo_volume;
	glGenBuffers(1, &ebo_volume);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_volume);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void VolumeScene::initFrameVAO()
{

	float frameVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	// Framebuffer VAO
	glGenVertexArrays(1, &VAO_FRAME);
	glBindVertexArray(VAO_FRAME);

	unsigned int vbo_frame;
	glGenBuffers(1, &vbo_frame);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_frame);
	glBufferData(GL_ARRAY_BUFFER, sizeof(frameVertices), &frameVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void VolumeScene::initVAOs()
{
	initVolumeVAO();
	initFrameVAO();
}

void VolumeScene::initFBOs()
{
	FBO_VOLUME = Framebuffer();
	FBO_ACCUMULATION = Framebuffer();
	FBO_LAST_FRAME = Framebuffer();
}

void VolumeScene::initShaders()
{
	initVolumeShaders();
	initAccumulationShader();
	initCopyShader();
	updateShaderValues();
}

void VolumeScene::initVolumeShaders()
{
	//dynamic volume path and size
	Texture3D volumeData("C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/Assets/foot_256x256x256_uint8.raw", GL_TEXTURE3, glm::vec3(256, 256, 256));

	const char* vertPathMonteCarlo = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/volume.vert";
	const char* fragPathMonteCarlo = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/monte_carlo.frag";

	monteCarloShader = Shader(vertPathMonteCarlo, fragPathMonteCarlo);
	monteCarloShader.use();

	monteCarloShader.setInt("volume", 3);

	int modelLoc = glGetUniformLocation(monteCarloShader.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	int viewLoc = glGetUniformLocation(monteCarloShader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	int projectionLoc = glGetUniformLocation(monteCarloShader.ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	const char* vertPathRayMarch = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/volume.vert";
	const char* fragPathRayMarch = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/ray_marching.frag";

	rayMarchShader = Shader(vertPathRayMarch, fragPathRayMarch);
	rayMarchShader.use();

	rayMarchShader.setInt("volume", 3);

	int modelLoc2 = glGetUniformLocation(rayMarchShader.ID, "model");
	glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model));
	int viewLoc2 = glGetUniformLocation(rayMarchShader.ID, "view");
	glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
	int projectionLoc2 = glGetUniformLocation(rayMarchShader.ID, "projection");
	glUniformMatrix4fv(projectionLoc2, 1, GL_FALSE, glm::value_ptr(projection));
}

void VolumeScene::initAccumulationShader()
{
	const char* vertPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/copy.vert";
	const char* fragPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/accumulation.frag";

	accumulationShader = Shader(vertPath, fragPath);
	accumulationShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBO_VOLUME.getTexture());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, FBO_LAST_FRAME.getTexture());

	accumulationShader.setInt("screenTexture", 0);
	accumulationShader.setInt("screenCopy", 2);
}

void VolumeScene::initCopyShader()
{
	const char* vertPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/copy.vert";
	const char* fragPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/copy.frag";

	copyShader = Shader(vertPath, fragPath);
	copyShader.use();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, FBO_ACCUMULATION.getTexture());

	copyShader.setInt("screenTexture", 1);
}

void VolumeScene::renderGUI()
{
	ImGui::Begin("Volume Renderer Settings");
	ImGui::Text("This is a simple Volume renderer coded by Anton Hartmann.");
	ImGui::Text("Select your render methode:");
	ImGui::RadioButton("Ray Marching", &gui_methode, 0); ImGui::SameLine();
	ImGui::RadioButton("Monte Carlo Sampling", &gui_methode, 1);
	ImGui::Text("Just for debug purposes:");
	ImGui::RadioButton("Just rng demo", &gui_methode, 2); ImGui::SameLine();
	ImGui::RadioButton("colored cube", &gui_methode, -1);
	if (gui_methode == 0) {
		ImGui::Text("Set the step size for the ray march:");
		ImGui::SliderFloat("stepSize", &gui_stepSize, 0.001f, 0.02f, "%.5f");
	}
	ImGui::End();
}

void VolumeScene::renderVolume()
{
	glBindVertexArray(VAO_VOLUME);

	switch(gui_methode) { 
	case 1://monte carlo
		glBindFramebuffer(GL_FRAMEBUFFER, FBO_VOLUME.getID()); //draw into framebuffer
		monteCarloShader.use();
		break;
	default:
		accumulatedFrames = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0); //else draw on screen
		rayMarchShader.use();
	}
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //set clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	if (gui_methode == 1) { //if monte carlo
		VolumeScene::accumulateFrames(); //accumulate frames
	}
}

void VolumeScene::accumulateFrames()
{
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(VAO_FRAME);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_ACCUMULATION.getID());
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	accumulationShader.use();

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_LAST_FRAME.getID());
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	copyShader.use();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	accumulatedFrames++;
}

void VolumeScene::updateShaderValues()
{
	rayMarchShader.use();
	rayMarchShader.setFloat("stepSize", gui_stepSize);
	monteCarloShader.use();
	monteCarloShader.setInt("samplesPerFrame", gui_samplesPerFrame);
	monteCarloShader.setFloat("randomizer", (float)glfwGetTime());
	accumulationShader.use();
	accumulationShader.setInt("runs", accumulatedFrames);
	accumulationShader.setInt("samplesPerRun", gui_samplesPerFrame);
}
