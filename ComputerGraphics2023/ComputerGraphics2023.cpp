#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

//Custom includes
#include "shader.h"
#include "TextureLoader.h"
#include "Camera.h"

using namespace std;
using namespace glm;

#pragma region SIGNATURES
void GLFW_Init();
GLFWwindow* GLFW_WindowInit();
bool GLAD_Init();

void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
#pragma endregion

#pragma region VARS
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

GLFWwindow* window;

// time settings
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

//Input handling
vec3 displacement = vec3(0.0f, 0.0f, -2.0f);
float movementSpeed = 1.0f;

//Texture loader
TextureLoader textLoader;
#pragma endregion

int main()
{
	GLFW_Init();

	window = GLFW_WindowInit();

	if (!GLAD_Init())
	{
		glfwTerminate();
		return -1;
	}

	//Enabling-Disabling depth testing
	glEnable(GL_DEPTH_TEST);

	Shader shader = Shader("Shaders/vertexShader.vs", "Shaders/fragmentShader.fs");
	textLoader = TextureLoader();

	//Geometry definition - FBX model mock
	float vertices[] = {
		//positions		     //texture coords
	   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	   -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	vec3 cubePositions[] = {
		vec3(0.0f, 0.0f, -10.0f),
		vec3(1.0f, 1.0f, -10.0f),
		vec3(2.0f, 2.0f, -10.0f),
		vec3(3.0f, 3.0f, -10.0f),
		vec3(4.0f, 4.0f, -10.0f),
		vec3(-1.0f, -1.0f,-10.0f),
		vec3(-2.0f, -2.0f, -10.0f),
		vec3(-3.0f, -3.0f, -10.0f),
		vec3(-4.0f, -4.0f, -10.0f),
	};

	unsigned int VBO, VAO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Vertex attribute for Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Vertex attribute for texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Texture Loading
	unsigned int texturePNG = textLoader.loadTexture("Textures/awesomeface.png", true);
	unsigned int textureJPG = textLoader.loadTexture("Textures/RTS_Crate.jpg", true);

	//VAO - VBO Unbinding to make the pipeline clearer.
	glBindBuffer(GL_ARRAY_BUFFER, 0); //VBO Unbind
	glBindVertexArray(0); //VAO Unbind

	// MAIN RENDERING LOOP
	while (!glfwWindowShouldClose(window))
	{
		//DeltaTime calculation
		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		// handle user input
		processInput(window);

		// Reinitialize frame buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Enable shader and update uniform variables
		shader.use();

		//Transform declaration
		mat4 model = mat4(1.0f);
		mat4 view = mat4(1.0f);
		mat4 projection = mat4(1.0f);

		// calculate projection matrix
		// attributes: fov, aspect ratio, near clipping plane, far clipping plane
		projection = perspective(radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		//Set texture samplers
		shader.setInt("textureObj[0]", 0);
		shader.setInt("textureObj[1]", 1);

		//RENDERING
		glBindVertexArray(VAO);

		//JPG Layer
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureJPG);

		//PNG Layer
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texturePNG);
		
		for (int i = 1; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++)
		{
			model = mat4(1.0f);
			model = translate(model, cubePositions[i] + displacement);
			model = rotate(model, (float)((1 % 360) * glfwGetTime()), vec3(0.0f, 1.0f, 0.0f));
			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// glfw: double buffering and polling IO events (keyboard, mouse, etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Resource and GLFW cleanup
	glfwTerminate();

	return 0;
}

#pragma region GLFW_GLAD_INIT
void GLFW_Init()
{
	//glfw init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* GLFW_WindowInit()
{
	//Window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Computer Graphics 2023", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window \n";
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

	return window;
}

bool GLAD_Init()
{
	// glad: load OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to load OpenGL function pointers!" << endl;
		return false;
	}

	return true;
}
#pragma endregion

#pragma region UTILS
//Handle keyboard input events
void processInput(GLFWwindow* window)
{
	//Exit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	//Movement
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		displacement.x -= movementSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		displacement.y -= movementSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		displacement.x += movementSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		displacement.y += movementSpeed * deltaTime;
	}

	//Depth
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		displacement.z -= movementSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		displacement.z += movementSpeed * deltaTime;
	}
}

//frame buffer resizing callback
void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}
#pragma endregion