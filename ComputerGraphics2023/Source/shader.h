#ifndef SHADER_H
#define SHADER_H

#include <glad\glad.h>

//String usage
#include <string>

//File readers
#include <iostream>
#include <sstream>
#include <fstream>

//Class declaration
class Shader
{
	//Public var declaration
public:
	//ID of the shader
	unsigned int ID;

	//Constructor
	Shader(const char* vertexPath, const char* fragmentPath)
	{
		std::string vertexCode;
		std::string fragmentCode;

		std::ifstream vertShaderFile;
		std::ifstream fragShaderFile;

		//ensure that the ifstream objects can throw exceptions
		vertShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			//Open the file
			vertShaderFile.open(vertexPath);
			fragShaderFile.open(fragmentPath);

			//Read file's contents into streams
			std::stringstream vSStream, fSStream;
			vSStream << vertShaderFile.rdbuf();
			fSStream << fragShaderFile.rdbuf();

			vertShaderFile.close();
			fragShaderFile.close();

			//Convert string Streams into strings
			vertexCode = vSStream.str();
			fragmentCode = fSStream.str();
		}
		catch (std::ifstream::failure& e)
		{
			std::cout << "SHADER FILE READ ERROR\n";
		}

		//Shader building and linking pipeline
		const char* vertexCode_C = vertexCode.c_str();
		const char* fragmentCode_C = fragmentCode.c_str();

		//Compile shaders --------------------------------
		unsigned int vertex, fragment;

		//Vertex compilation
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexCode_C, NULL);
		glCompileShader(vertex);
		checkForErrors(vertex, "VERTEX");

		//Fragment compilation
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentCode_C, NULL);
		glCompileShader(fragment);
		checkForErrors(fragment, "FRAGMENT");
		//-------------------------------------------------

		//Link to shaders and program creation
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkForErrors(ID, "PROGRAM");

		//Delete shader programs
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void use()
	{
		glUseProgram(ID);
	}

	void setVec3(const std::string& name, float x, float y, float z) const
	{
		int location = glGetUniformLocation(ID, name.c_str());
		glUniform3f(location, x, y, z);
	}

private:
	void checkForErrors(unsigned int shaderCode, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shaderCode, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shaderCode, 1024, NULL, infoLog);

				std::cout << "SHADER COMPILATION ERROR --- " << type << std::endl << infoLog
					<< std::endl << "-- --------------------------- --" << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shaderCode, GL_LINK_STATUS, &success);

			if (!success)
			{
				glGetProgramInfoLog(shaderCode, 1024, NULL, infoLog);

				std::cout << "SHADER LINKING ERROR --- " << type << std::endl << infoLog
					<< std::endl << "-- --------------------------- --" << std::endl;
			}
		}
	}
};

#endif