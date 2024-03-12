#ifndef VOLUME_H
#define VOLUME_H

#include <core/Objects/object.h>
#include <core/shader.h>
#include <core/texture3D.h>
#include<glm/glm.hpp>

class Volume : public Object
{
public:
	Volume() : VAO(0),
		data(Texture3D(
			"C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/Assets/foot_256x256x256_uint8.raw",
			glm::vec3(256))),
		modelMatrix(glm::mat4(1.0f)) {
		glGenVertexArrays(1, &VAO);
		initVAO();
	};
	void initVAO();

	void setData(char* pathToFile);
	void setPosition(glm::mat4 modelMatrix);

	unsigned int getVAO();
	unsigned int getData();
	glm::mat4 getPosition();

private:
	unsigned int VAO;
	Texture3D data;
	glm::mat4 modelMatrix;
};


#endif