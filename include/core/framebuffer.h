#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

class Framebuffer {
public:
	Framebuffer();
	unsigned int getID();
	unsigned int getTexture();
private:
	unsigned int ID, TEXTURE;
};

#endif