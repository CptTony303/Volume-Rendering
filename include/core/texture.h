#ifndef TEXTURE_H
#define TEXTURE_H

class Texture {
public:
	unsigned int ID;
	Texture(int width, int height, int* data, unsigned int glColorChannel);
	Texture(char * texturePath, unsigned int glColorChannel);
private:
	void init(int width, int height, int* data, unsigned int glColorChannel);
};

#endif