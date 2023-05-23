#ifndef SHADER_H
#define SHADER_H

#include <glad\glad.h>
#include <glm\glm.hpp>

//String usage
#include <string>

//File readers
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
using namespace glm;

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
		string vertexCode;
		string fragmentCode;

		ifstream vertShaderFile;
		ifstream fragShaderFile;

		//ensure that the ifstream objects can throw exceptions
		vertShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fragShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

		try
		{
			//Open the file
			vertShaderFile.open(vertexPath);
			fragShaderFile.open(fragmentPath);

			//Read file's contents into streams
			stringstream vSStream, fSStream;
			vSStream << vertShaderFile.rdbuf();
			fSStream << fragShaderFile.rdbuf();

			vertShaderFile.close();
			fragShaderFile.close();

			//Convert string Streams into strings
			vertexCode = vSStream.str();
			fragmentCode = fSStream.str();
		}
		catch (ifstream::failure& e)
		{
			cout << "SHADER FILE READ ERROR\n";
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

	void setVec3(const string& name, float x, float y, float z) const
	{
		int location = glGetUniformLocation(ID, name.c_str());
		glUniform3f(location, x, y, z);
	}

	void setVec3(const string& name, const vec3& value) const
	{
		int location = glGetUniformLocation(ID, name.c_str());
		glUniform3fv(location, 1, &value[0]);
	}

	void setMat4(const string& name, const mat4& value) const
	{
		int location = glGetUniformLocation(ID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	}

private:
	void checkForErrors(unsigned int shaderCode, string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shaderCode, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(shaderCode, 1024, NULL, infoLog);

				cout << "SHADER COMPILATION ERROR --- " << type << endl << infoLog
					<< endl << "-- --------------------------- --" << endl;
			}
		}
		else
		{
			glGetProgramiv(shaderCode, GL_LINK_STATUS, &success);

			if (!success)
			{
				glGetProgramInfoLog(shaderCode, 1024, NULL, infoLog);

				cout << "SHADER LINKING ERROR --- " << type << endl << infoLog
					<< endl << "-- --------------------------- --" << endl;
			}
		}
	}
};
#endif