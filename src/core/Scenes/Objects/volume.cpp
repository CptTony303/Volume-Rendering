#include <core/Objects/volume.h>
#include <glad/glad.h>
#include <core/shader.h>
#include <core/texture3D.h>


void Volume::initVAO()
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
	glBindVertexArray(VAO);

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

void Volume::setData(Texture3D volumeData)
{
	data = volumeData; //todo: interpreter for nhdr files -> remove dimensions
}

void Volume::setPosition(glm::mat4 modelMatrix)
{
	this->modelMatrix = modelMatrix;
}

unsigned int Volume::getVAO()
{
	return VAO;
}

unsigned int Volume::getData()
{
	return data.ID;
}

glm::mat4 Volume::getPosition()
{
	return modelMatrix;
}
