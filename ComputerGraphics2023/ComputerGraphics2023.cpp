#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <limits>
#include <vector>

//Custom includes
#include "shader.h"
#include "TextureLoader.h"
#include "Camera.h"

#pragma region SIGNATURES
void GLFW_Init();
GLFWwindow* GLFW_WindowInit();
bool GLAD_Init();
void initShaders();

unsigned int loadCubemap(std::vector<std::string> faces);
void loadEnviromentalMappingShader(glm::mat4 model, glm::mat4 view, glm::mat4 projection, unsigned int skybox);
void loadLightsource(glm::mat4 model, glm::mat4 view, glm::mat4 projection);
void drawSkybox(glm::mat4 view, glm::mat4 projection, unsigned int VAO, unsigned int skyboxVAO, unsigned int skybox);

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
Shader skyboxShader;
Shader envMappingShader;
bool useBlinnPhong = false;
glm::vec3 lightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 lightColor = glm::vec3(1.0f);

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

	initShaders();

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

	// Skybox VAO and VBO
	unsigned int skyboxVBO, skyboxVAO;

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);

	glBindVertexArray(skyboxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// ------------------

	//Texture loading
	unsigned int diffuseMap = TextureLoader().loadTexture("Textures/diffuseMap.png", false);
	unsigned int specularMap = TextureLoader().loadTexture("Textures/specularMap.png", false);
	unsigned int skybox = TextureLoader().loadCubemap(skyboxFaces);

	//VAO - VBO Unbinding to make the pipeline cleaner.
	glBindBuffer(GL_ARRAY_BUFFER, 0); //VBO Unbind
	glBindVertexArray(0); //VAO Unbind

	//Material setup
	mainShader.use();
	mainShader.setVec3("surfaceMat.ambient", glm::vec3(0.7f));

	//Set the index of the textures for rendering below
	mainShader.setInt("surfaceMat.diffuse", 0);
	mainShader.setInt("surfaceMat.specular", 1);

	mainShader.setFloat("surfaceMat.shininess", 128 * 0.8f);

	//Light source setup
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
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// declare transforms
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		// calculate projection matrix
		// attributes: fov, aspect ratio, near clipping plane, far clipping plane
		projection = glm::perspective(glm::radians(camera.fov), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

		// Update view matrix
		view = camera.GetViewMatrix();

		// enable shader and update uniform variables
		mainShader.use();

		// Real time uniforms for blinn-phong shader
		glm::vec3 lightPos = glm::vec3(glm::sin(glfwGetTime()), glm::cos(glfwGetTime()), 2.0f);

		mainShader.setVec3("lightPosition", lightPos);
		mainShader.setVec3("viewPosition", camera.position);
		mainShader.setInt("useBlinnPhong", useBlinnPhong);

		mainShader.setMat4("view", view);
		mainShader.setMat4("projection", projection);

		//RENDERING
		glBindVertexArray(VAO);

		//Effectively sets the samplers2D
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		//Draw the cubes
		for (int i = 1; i < 10; i++)
		{
			model = glm::mat4(1.0f);

			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, glm::radians(10.0f * i), glm::vec3(0.0f, 1.0f, 0.0f));

			mainShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//Env mapping cube
		loadEnviromentalMappingShader(model, view, projection, skybox);

		//Lightsource cube
		loadLightsource(model , view, projection);
		
		//Skybox
		drawSkybox(view, projection, VAO, skyboxVAO, skybox);

		//glfw: double buffering and polling IO events (keyboard, mouse, etc.)
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

#pragma region SHADERS
void initShaders()
{
	mainShader = Shader("Shaders/vertexShader.vs", "Shaders/fragmentShader.fs");
	lightSourceShader = Shader("Shaders/vertexShader.vs", "Shaders/lightSourceShader.fs");
	skyboxShader = Shader("Shaders/skyboxShaderVertex.vs", "Shaders/skyboxShaderFragment.fs");
	envMappingShader = Shader("Shaders/vertexShader.vs", "Shaders/envMappingFragment.fs");
}

void loadEnviromentalMappingShader(glm::mat4 model, glm::mat4 view, glm::mat4 projection, unsigned int skybox)
{
	//Render the environmentally mapped cube
	envMappingShader.use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, cubePositions[0]);
	model = glm::rotate(model, glm::radians(10.0f * 0), glm::vec3(0.0f, 1.0f, 0.0f));

	envMappingShader.setMat4("model", model);
	envMappingShader.setMat4("view", view);
	envMappingShader.setMat4("projection", projection);
	envMappingShader.setFloat("extMediumIdx", 1.000293);
	envMappingShader.setFloat("entMediumIdx", 1.31);
	envMappingShader.setVec3("viewPosition", camera.position);
	envMappingShader.setInt("skybox", 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void loadLightsource(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	//Light source cube 
	lightSourceShader.use();
	model = glm::mat4(1.0f);
	//Light pos lerping
	lightPosition = glm::vec3(glm::sin(glfwGetTime()), glm::cos(glfwGetTime()), 2.0f);
	model = glm::translate(model, lightPosition);
	model = glm::scale(model, glm::vec3(0.2f));

	lightSourceShader.setMat4("model", model);
	lightSourceShader.setMat4("view", view);
	lightSourceShader.setMat4("projection", projection);
	lightSourceShader.setVec3("lightColor", lightColor);
}

void drawSkybox(glm::mat4 view, glm::mat4 projection, unsigned int VAO, unsigned int skyboxVAO, unsigned int skybox)
{
	//In case of different VAO there should be a re-bounding of the Textures etc.
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//OPTIMAL SKYBOX
	glDepthFunc(GL_LEQUAL);
	skyboxShader.use();

	skyboxShader.setInt("skybox", 0);
	skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));
	skyboxShader.setMat4("projection", projection);

	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);
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

// Face order should follow the rule:
// 0. Right face (+X)
// 1. Left face (-X)
// 2. Top face (+Y)
// 3. Bottom face (-Y)
// 4. Front face (+Z)
// 5. Back face (-Z)
unsigned int loadCubemap(std::vector<std::string> faces)
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
#pragma endregion