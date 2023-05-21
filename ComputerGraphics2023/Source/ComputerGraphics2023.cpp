#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Shader.h"

void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

// time settings
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

//Input handling
float displacement_X = 0.0f;
float displacement_Y = 0.0f;
float displacement_Z = 0.0f;
float movementSpeed = 1.0f;

int main()
{
	//glfw init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Computer Graphics 2023", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window \n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

	// glad: load OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load OpenGL function pointers!" << std::endl;
		glfwTerminate();
		return -1;
	}

	Shader shader("Source/Shaders/vertexShader.vs", "Source/Shaders/fragmentShader.fs");

	//Geometry definition - FBX model mock
	float vertices[] = {
		//First 3 -> Position - Last 3 -> Color
		-0.5, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, //Left corner
		 0.5, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, //Right corner
		 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f //Top Corner
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO); //VAO Bind
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //VBO Bind
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Vertex attribute for Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glad_glEnableVertexAttribArray(0);

	//Vertex attribute for Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glad_glEnableVertexAttribArray(1);

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

		// RENDER STUFF

		// Reinitialize frame buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Enable shader and update uniform variables
		shader.use();
		shader.setVec3("displacement", displacement_X, displacement_Y, displacement_Z);

		//RENDERING
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// glfw: double buffering and polling IO events (keyboard, mouse, etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Resource and GLFW cleanup
	glfwTerminate();

	return 0;
}

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
		displacement_X -= movementSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		displacement_Y -= movementSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		displacement_X += movementSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		displacement_Y += movementSpeed * deltaTime;
	}

	//Depth
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		displacement_Z -= movementSpeed * deltaTime;
		std::cout << displacement_Z << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		displacement_Z += movementSpeed * deltaTime;
		std::cout << displacement_Z << std::endl;
	}
}

//frame buffer resizing callback
void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}