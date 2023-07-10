#include <stb_image.h>

class TextureLoader
{
public:
	// Face order should follow the rule:
	// 0. Right face (+X)
	// 1. Left face (-X)
	// 2. Top face (+Y)
	// 3. Bottom face (-Y)
	// 4. Front face (+Z)
	// 5. Back face (-Z)
	unsigned int loadCubemap(std::vector<std::string> faces) const
	{
		unsigned int textureID = 0;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		// Texture wrapping properties
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		// ---------------------------

		// Texture filtering properties
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// ----------------------------

		int width, height, numOfChannels;

		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &numOfChannels, 0);

			if (data)
			{
				GLenum format;

				if (numOfChannels == 1)
				{
					format = GL_RED;
				}
				else if (numOfChannels == 3)
				{
					format = GL_RGB;
				}
				else if (numOfChannels == 4)
				{
					format = GL_RGBA;
				}
				else
				{
					std::cout << "TEXTURE FILE " << faces[i] << " FAILED TO LOAD: INCOMPATIBLE NUMBER OF CHANNELS!!" << std::endl;
					stbi_image_free(data);
					continue;
				}

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				std::cout << "TEXTURE FILE FAILED TO LOAD FROM PATH" << faces[i] << "!!" << std::endl;
			}

			stbi_image_free(data);
		}

		return textureID;
	}

	unsigned int loadTexture(const char* texturePath, bool flipVertically) const
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