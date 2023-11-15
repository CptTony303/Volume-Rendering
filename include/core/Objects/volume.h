#ifndef VOLUME_H
#define VOLUME_H

#include <core/Objects/object.h>
#include <core/shader.h>
#include<glm/glm.hpp>

class Volume : public Object
{
public:
	Shader* volumeShader;


	Volume(char* path, glm::vec3 size, unsigned int glTextureUnit, char* uniformName);
	void renderObject();

private:

};


#endif