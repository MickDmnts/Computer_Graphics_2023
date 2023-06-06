#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <stb/stb_image.h>

using namespace std;

class TextureLoader
{
public:
	unsigned int loadTexture(const char* texturePath, bool flipVertically)
	{
		unsigned int textureId;

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// Texture wrapping properties
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// ---------------------------

		// Texture filtering properties
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		// ----------------------------

		//Should the texture flip vertically?
		stbi_set_flip_vertically_on_load(flipVertically);

		//Image loading
		int width, height, numOfChannels;
		unsigned char* data = stbi_load(texturePath, &width, &height, &numOfChannels, 0);

		if (data)
		{
			GLenum format;

			switch (numOfChannels)
			{
				case 1:
					format = GL_RED;
					break;

				case 3:
					format = GL_RGB;
					break;

				case 4:
					format = GL_RGBA;
					break;

				default:
					std::cout << "TEXTURE FILE FAILED TO LOAD: INCOMPATIBLE NUMBER OF CHANNELS!!" << std::endl;
					stbi_image_free(data);
					return textureId;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "TEXTURE FILE FAILED TO LOAD FROM PATH " << texturePath << "!!" << std::endl;
		}

		stbi_image_free(data);

		return textureId;
	}
};
#endif