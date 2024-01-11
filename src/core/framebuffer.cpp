#include <core/framebuffer.h>
#include <glad/glad.h>
#include <iostream>

Framebuffer::Framebuffer() {
	unsigned int local_id;
	glGenFramebuffers(1, &local_id);
	glBindFramebuffer(GL_FRAMEBUFFER, local_id);

	//save currently bound Texture
	/*unsigned int boundTexture;
	glGet(GL_TEXTURE_BINDING_2D);*/
	// generate texture
	unsigned int texture;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//get actual screen size
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// generate depthbuffer
	unsigned int depthbuffer;
	glGenTextures(1, &depthbuffer);
	glBindTexture(GL_TEXTURE_2D, depthbuffer);
	//get actual screen size
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	//rebind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthbuffer, 0);

	ID = local_id;
	TEXTURE = texture;
}
unsigned int Framebuffer::getID() {
	return ID;
}

unsigned int Framebuffer::getTexture()
{
	return TEXTURE;
}
