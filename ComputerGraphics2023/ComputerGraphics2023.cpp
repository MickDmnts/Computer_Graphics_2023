#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <limits>

//Custom includes
#include "shader.h"
#include "TextureLoader.h"
#include "Camera.h"

#pragma region SIGNATURES
void GLFW_Init();
GLFWwindow* GLFW_WindowInit();
bool GLAD_Init();

void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
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
float keyCD = 0.0f;
float toggleCD = 1.0f;

//Shader
Shader mainShader;
Shader lightSourceShader;
bool useBlinnPhong = false;
glm::vec3 lightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 lightColor = glm::vec3(1.0f);

//Texture loader
TextureLoader textLoader;

//Mouse position data
float previousX = 0.0f;
float previousY = 0.0f;
bool isFirstFrame = true;

// Camera declaration
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, -1.0f));
#pragma endregion

int main()
{
	GLFW_Init();

	window = GLFW_WindowInit();
	if (window == NULL)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// Cursor grabbing settings
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!GLAD_Init())
	{
		glfwTerminate();
		return -1;
	}

	//Enabling-Disabling depth testing
	glEnable(GL_DEPTH_TEST);

	mainShader = Shader("Shaders/vertexShader.vs", "Shaders/fragmentShader.fs");
	lightSourceShader = Shader("Shaders/vertexShader.vs", "Shaders/lightSourceShader.fs");
	textLoader = TextureLoader();

	// Geometry definition
	float vertices[] = {
		// positions         // normals           // texture
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f),
	};

	unsigned int VBO, VAO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vertex attribute for POSITION
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Vertex attribute for NORMAL
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Vertex attribute for TEXTURE
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//VAO - VBO Unbinding to make the pipeline clearer.
	glBindBuffer(GL_ARRAY_BUFFER, 0); //VBO Unbind
	glBindVertexArray(0); //VAO Unbind

	//Material setup
	mainShader.use();
	mainShader.setVec3("surfaceMat.ambient", glm::vec3(0.0215f, 0.1745f, 0.0215f));
	mainShader.setVec3("surfaceMat.diffuse", glm::vec3(0.07568f, 0.61424f, 0.07568f));
	mainShader.setVec3("surfaceMat.specular", glm::vec3(0.633f, 0.727811f, 0.633f));
	mainShader.setFloat("surfaceMat.shininess", 128 * 0.6f);

	lightSourceShader.setVec3("lightColor", lightColor);

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
		
		//Main shader usage
		mainShader.use();
		mainShader.setVec3("lightPosition", lightPosition);
		mainShader.setVec3("viewPosition", camera.position);
		mainShader.setInt("useBlinnPhong", useBlinnPhong);

		// declare transforms
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		// calculate projection matrix
		// attributes: fov, aspect ratio, near clipping plane, far clipping plane
		projection = glm::perspective(glm::radians(camera.fov), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

		// Update view matrix
		view = camera.GetViewMatrix();

		mainShader.setMat4("view", view);
		mainShader.setMat4("projection", projection);

		//RENDERING
		glBindVertexArray(VAO);

		for (int i = 0; i < 10; i++)
		{
			model = glm::mat4(1.0f);

			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, glm::radians(10.0f * i), glm::vec3(0.0f, 1.0f, 0.0f));

			mainShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//Light source cube 
		lightSourceShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.2f));

		lightSourceShader.setMat4("model", model);
		lightSourceShader.setMat4("view", view);
		lightSourceShader.setMat4("projection", projection);
		lightSourceShader.setVec3("lightColor", lightColor);

		//In case of different VAO there should be a re-bounding of the Textures etc.

		glDrawArrays(GL_TRIANGLES, 0, 36);

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
	if (keyCD + deltaTime <= FLT_MAX)
	{keyCD += deltaTime; }

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.HandleKeyboard(FORWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.HandleKeyboard(BACKWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.HandleKeyboard(RIGHT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.HandleKeyboard(LEFT, deltaTime);
	}

	// Move up or down
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera.HandleKeyboard(UP, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera.HandleKeyboard(DOWN, deltaTime);
	}

	//Illumination model toggling
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		if (keyCD >= toggleCD)
		{
			useBlinnPhong = !useBlinnPhong;
			keyCD = 0.0f;
		}
	}
}

//frame buffer resizing callback
void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}

// Mouse position callback
void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (isFirstFrame)
	{
		previousX = xPos;
		previousY = yPos;
		isFirstFrame = false;
	}

	float xOffset = xPos - previousX;
	float yOffset = yPos - previousY;

	previousX = xPos;
	previousY = yPos;

	camera.HandleMouseMovement(xOffset, yOffset, deltaTime);
}

// Scrolling callback
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.HandleMouseScroll((float)yOffset);
}
#pragma endregion