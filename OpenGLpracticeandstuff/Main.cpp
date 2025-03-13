#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "filesystem.h"
#include "shader.h"
#include "camera.h"
#include "model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_call_back(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_enter_callback(GLFWwindow* window, int entry);
void processInput(GLFWwindow* window);
//unsigned int loadTexture(const char* path);

// Settings
unsigned int fullWidth = 800;
unsigned int fullHeight = 600;

void get_resolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	fullWidth = mode->width;
	fullHeight = mode->height;
}

// Camera Settings
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = fullWidth / 2.0f;
float lastY = fullHeight / 2.0f;
bool firstMouse = true;

bool mouseCaptured = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//float mixValue = 0.0f;

//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	glfwInit();

	// Creating the window using glfw

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);		// If on Mac OS X
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);				// For hiding titlebar

	get_resolution();

	GLFWwindow* window = glfwCreateWindow(fullWidth, fullHeight, "Window Created, Yatta!!", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GFL Window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_call_back);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorEnterCallback(window, cursor_enter_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);
	//glEnable(GL_BLEND);
	//glEnable(GL_CULL_FACE);

	Shader ourShader("modelshader.vs", "modelshader.fs");
	Shader screenShader("screenshader.vs", "screenshader.fs");
	//Shader lightCubeShader("lightcubeshader.vs", "lightcubeshader.fs");
	 
	float fullscreenQuad[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	// Quad VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreenQuad), &fullscreenQuad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));	

	
	//Loading Textures
	//stbi_set_flip_vertically_on_load(true);
	//unsigned int diffuseMap = loadTexture(FileSystem::getPath("data/reimu.jpg").c_str());
	//unsigned int specularMap = loadTexture(FileSystem::getPath("data/reimu_shiny.jpg").c_str());

	Model ourModel(FileSystem::getPath("data/models/Mei HoT/Herrscher of Thunder - Raiden Mei.pmx"));

	// Setting uniforms
	/*lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);*/
		
	screenShader.use();
	screenShader.setInt("screentexture", 0);

	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// Conditions for framebuffer: atleast one buffer, one colour attachment, attachments should be complete, same no of sample in each buffer

	unsigned int textureColorBuffer;
	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fullWidth, fullHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fullWidth, fullHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete, " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Render Loop (Frame)
	while (!glfwWindowShouldClose(window))
	{
		// Per frame time
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Input
		processInput(window);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);

		// Rendering
		//glClearColor(0.854f, 0.560f, 0.964f, 1.0);					// #da8ff6
		glClearColor(0.854f, 0.260f, 0.964f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// light pos stuff
		float time = glfwGetTime() / 2.0f;
		float lightX = cos(time);
		float lightY = sin(time);

		ourShader.use();
		ourShader.setVec3("lightPos", glm::vec3(lightX * 10.0f, lightY * 10.0f, 10.0f));
		ourShader.setVec3("viewPos", camera.Position);
		ourShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); 
		ourShader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));

		// View/Projection
		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);	
		projection = glm::perspective(glm::radians(camera.Zoom), (float)fullWidth / (float)fullHeight, 0.1f, 100.0f);
		view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5));
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader);
		
		// Binding to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		screenShader.use();
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Swap buffers, Check and call event
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// De-allocate all resources
	//glDeleteVertexArrays(1, &cubeVAO);
	//glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &framebuffer);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	// Terminate all GLFW
	glfwTerminate();

	return 0;
}

// To take some user input
void processInput(GLFWwindow* window)
{
	// Close Window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Camera Inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);

	// Toggle cursor lock
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		mouseCaptured = !mouseCaptured;
		glfwSetInputMode(window, GLFW_CURSOR, mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	// Enable/Disable Wireframe Mode
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void mouse_call_back(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void cursor_enter_callback(GLFWwindow* window, int entry)
{
	if (entry)
	{
		firstMouse = true;
	}
}

// If User or OS decides to resize window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//unsigned int loadTexture(char const* path)
//{
//	unsigned textureID;
//	glGenTextures(1, &textureID);
//
//	int width, height, nrComponents;
//	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
//	if (data)
//	{
//		GLenum format = GL_RGB;
//		if (nrComponents == 1)
//			format = GL_RED;
//		else if (nrComponents == 3)
//			format = GL_RGB;
//		else if (nrComponents == 4)
//			format = GL_RGBA;
//
//		glBindTexture(GL_TEXTURE_2D, textureID);
//		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		
//		stbi_image_free(data);
//	}
//
//	else
//	{
//		std::cout << "Texture failed to load at path: " << path << std::endl;
//		stbi_image_free(data);
//	}
//
//	return textureID;
//}