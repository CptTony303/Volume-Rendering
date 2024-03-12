#include <core/texture3D.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

Texture3D::Texture3D(char * texturePath, glm::vec3 dimensions) {
	glEnable(GL_TEXTURE_3D);
	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, texture);
	// Setze die Texturparameter
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

	int width = dimensions.x;
	int depth = dimensions.y;
	int height= dimensions.z;

	std::ifstream file(texturePath, std::ios::binary);

	if (file.is_open()) {
		// Bestimme die Größe der Datei
		file.seekg(0, std::ios::end);
		std::streampos fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		// Lese die Daten in einen Vektor
		std::vector<uint8_t> data(fileSize);
		file.read(reinterpret_cast<char*>(data.data()), fileSize);

		file.close();

		if (fileSize != (width * height * depth * sizeof(uint8_t))) {
			std::cout << "Die Größe der gelesenen Daten stimmt nicht mit den erwarteten Dimensionen überein" << std::endl;
		}

		/*for (uint8_t value : data) {
			std::cout << static_cast<int>(value) << " ";
		}*/
		/*
		std::vector <uint8_t> blownUpData(fileSize);
		int index = 0;
		for (int z = 0; z < depth; z++) {
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					blownUpData[index++] = data[width - 1 - x + z * width + y * width* depth];
					//sortedData[x + y * width + z * depth*width] = data[index++];
				}
			}
		}
		*/
		glPixelStorei(GL_UNPACK_LSB_FIRST, true);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
		glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, height);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, width - 1, height - 1, depth - 1, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			// Hier kannst du Fehlerprotokollierung oder Debug-Ausgabe hinzufügen
			std::cerr << "OpenGL-Fehler: " << error << std::endl;
		}
		glGenerateMipmap(GL_TEXTURE_3D);
	}
	else {
		std::cout << "Failed to load texture3D" << std::endl;
	}



	ID = texture;
}

void Texture3D::readHeaderFile(char* headerPath)
{
}
