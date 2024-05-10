
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
	"}\0";

// Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColour;\n"
	"void main()\n"
	"{\n"
	"	FragColour = vec4(0.3f, 0.9f, 0.1f, 1.0f);\n"
	"}\0";


int main()
{
	glfwInit();

	// Creating the window using glfw

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);		// If on Mac OS X

	GLFWwindow* window = glfwCreateWindow(800, 600, "Window Created, Yatta!!", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GFL Window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	// Vertex Shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Assigning vertex shader source code to shader object, and compiling
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Checking for compile time errors for vertex shader
	int successvertex;
	char infoLogvertex[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &successvertex);
	if (!successvertex) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLogvertex);
		std::cout << "ERROR:SHADER:VERTEX:COMPILATION:FAILED\n" << infoLogvertex << std::endl;
	}


	
	// Fragment Shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	int successfrag;
	char infoLogfrag[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &successfrag);
	if (!successfrag) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLogfrag);
		std::cout << "ERROR:SHADER:FRAGMENT:COMPILATION:FAILED\n" << infoLogfrag << std::endl;
	}


	// Creating the Shader Program

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	int successlink;
	char infoLoglink[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &successlink);
	if (!successlink) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLoglink);
		std::cout << "ERROR:SHADER:PROGRAM:LINK:FAILED\n" << infoLoglink << std::endl;
	}

	glUseProgram(shaderProgram);

	// Deleting shader objects, as their purpose is fulfilled
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);



	// Vertex Input
	
	float vertices[] = {
		// To render 2D triangle
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f
	};

	// To create a Rectangle
	unsigned int indices[] = {
		0, 1, 3,	// First Triangle
		1, 2, 3		// Second Triangle
	};


	// Creating Vertex Buffer Object
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// Binding VBO to array buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Copies vertex data into buffer memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);		// Copies user defined data into currently bound buffer

	// GL_STREAM_DRAW: Data will be set once, and used by GPU a limited number of times
	// GL_STATIC_DRAW: Data will be set once, and used many times
	// GL_DYNAMIC_DRAW:Data will be changed a lot, and is used many times


	// Linking vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);



	// A VAO is used to draw objects with more ease

	// Creating Vertex Array Object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);


	// Creating Element Buffer Object
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	// Linking VAO
 
	// Initialization
	// 1. Bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. Copy vertices array in buffer for opengl to use
	glBindBuffer(GL_ARRAY_BUFFER, VAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. Copy index array in element buffer for opengl to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// 4. Set Vertex Attribute Pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Continued in Render Loop

	

	// Render Loop (Frame)
	while (!glfwWindowShouldClose(window))
	{
		// Input
		processInput(window);

		// Rendering
		glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//5. Draw the Object		//Continued
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Swap buffers, Check and call event
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// De-allocate all resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteProgram(shaderProgram);

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

	// Enable/Disable Wireframe Mode
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// If User or OS decides to resize window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
