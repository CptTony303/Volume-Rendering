#include <core/Objects/volume.h>
#include <glad/glad.h>
#include <core/shader.h>
#include <core/texture3D.h>

Volume::Volume(char* path, glm::vec3 size, unsigned int glTextureUnit, char* uniformName)
{
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

	volumeShader = new Shader(vertPath, fragPath);
	volumeShader->use();

	Texture3D volume(path, glTextureUnit, size);
	volumeShader->setInt("volumeTexture", glTextureUnit);
	volumeShader->setFloat("isovalue", 0.45f);
}

void Volume::renderObject()
{
}
